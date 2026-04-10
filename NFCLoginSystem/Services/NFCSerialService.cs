using System;
using System.IO.Ports;
using System.Threading;
using System.Threading.Tasks;

namespace NFCLoginSystem.Services
{
    public class NFCSerialService
    {
        private SerialPort? _serialPort;
        private readonly string _portName;
        private readonly int _baudRate;
        private bool _isReading;
        private CancellationTokenSource? _cancellationTokenSource;

        public event EventHandler<string>? CardDetected;
        public event EventHandler<string>? ErrorOccurred;

        public NFCSerialService(string portName = "COM3", int baudRate = 9600)
        {
            _portName = portName;
            _baudRate = baudRate;
        }

        public bool IsConnected => _serialPort?.IsOpen ?? false;

        public bool Connect()
        {
            try
            {
                if (_serialPort?.IsOpen == true)
                {
                    return true;
                }

                _serialPort = new SerialPort(_portName, _baudRate)
                {
                    Parity = Parity.None,
                    DataBits = 8,
                    StopBits = StopBits.One,
                    Handshake = Handshake.None,
                    ReadTimeout = 1000,
                    WriteTimeout = 1000
                };

                _serialPort.Open();
                return true;
            }
            catch (Exception ex)
            {
                ErrorOccurred?.Invoke(this, $"连接NFC读卡器失败: {ex.Message}");
                return false;
            }
        }

        public void Disconnect()
        {
            try
            {
                StopReading();
                _serialPort?.Close();
                _serialPort?.Dispose();
                _serialPort = null;
            }
            catch (Exception ex)
            {
                ErrorOccurred?.Invoke(this, $"断开连接失败: {ex.Message}");
            }
        }

        public void StartReading()
        {
            if (_isReading || !IsConnected)
                return;

            _isReading = true;
            _cancellationTokenSource = new CancellationTokenSource();
            
            Task.Run(() => ReadNFCData(_cancellationTokenSource.Token));
        }

        public void StopReading()
        {
            if (!_isReading)
                return;

            _isReading = false;
            _cancellationTokenSource?.Cancel();
            _cancellationTokenSource?.Dispose();
            _cancellationTokenSource = null;
        }

        private async Task ReadNFCData(CancellationToken cancellationToken)
        {
            var buffer = new byte[256];
            
            while (!cancellationToken.IsCancellationRequested && IsConnected)
            {
                try
                {
                    if (_serialPort?.BytesToRead > 0)
                    {
                        int bytesRead = _serialPort.Read(buffer, 0, buffer.Length);
                        if (bytesRead > 0)
                        {
                            string cardData = System.Text.Encoding.ASCII.GetString(buffer, 0, bytesRead).Trim();
                            
                            // 简单的NFC卡ID解析
                            if (!string.IsNullOrEmpty(cardData) && cardData.Length >= 8)
                            {
                                // 假设卡ID是8-16位的十六进制字符串
                                string cardId = ExtractCardId(cardData);
                                if (!string.IsNullOrEmpty(cardId))
                                {
                                    CardDetected?.Invoke(this, cardId);
                                }
                            }
                        }
                    }
                    
                    await Task.Delay(100, cancellationToken);
                }
                catch (TimeoutException)
                {
                    // 超时是正常的，继续读取
                    continue;
                }
                catch (Exception ex)
                {
                    ErrorOccurred?.Invoke(this, $"读取NFC数据失败: {ex.Message}");
                    break;
                }
            }
        }

        private string ExtractCardId(string rawData)
        {
            // 移除空格和特殊字符
            string cleaned = rawData.Replace(" ", "").Replace("\r", "").Replace("\n", "");
            
            // 检查是否是有效的十六进制卡ID
            if (System.Text.RegularExpressions.Regex.IsMatch(cleaned, "^[0-9A-Fa-f]{8,16}$"))
            {
                return cleaned.ToUpper();
            }
            
            return cleaned.Length >= 8 ? cleaned.Substring(0, 16) : string.Empty;
        }

        public string[] GetAvailablePorts()
        {
            return SerialPort.GetPortNames();
        }

        public void Dispose()
        {
            Disconnect();
            _serialPort?.Dispose();
        }
    }
}