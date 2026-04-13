using PCSC;
using PCSC.Iso7816;
using PCSC.Monitoring;
using System;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace NFCLoginSystem.Services
{
    public class NFCPCSCService : IDisposable
    {
        public event EventHandler<string>? CardDetected;
        public event EventHandler<string>? ErrorOccurred;
        public event EventHandler<string>? ReaderStatusChanged;

        private ISCardMonitor? _monitor;
        private readonly ISCardContext _context;
        private CancellationTokenSource _cancellationTokenSource;
        private string[]? _readerNames;

        public NFCPCSCService()
        {
            _context = ContextFactory.Instance.Establish(SCardScope.System);
            _cancellationTokenSource = new CancellationTokenSource();
        }

        public void StartMonitoring()
        {
            try
            {
                _readerNames = _context.GetReaders();
                if (_readerNames == null || _readerNames.Length == 0)
                {
                    ReaderStatusChanged?.Invoke(this, "未找到NFC读卡器");
                    return;
                }

                _monitor = MonitorFactory.Instance.Create(SCardScope.System);
                _monitor.StatusChanged += OnStatusChanged;
                _monitor.Start(_readerNames);

                ReaderStatusChanged?.Invoke(this, $"正在监控: {_readerNames.FirstOrDefault()}");
            }
            catch (Exception ex)
            {
                ErrorOccurred?.Invoke(this, $"启动监控失败: {ex.Message}");
            }
        }

        public void StopMonitoring()
        {
            if (_monitor != null)
            {
                _monitor.StatusChanged -= OnStatusChanged;
                _monitor.Cancel();
                _monitor.Dispose();
                _monitor = null;
            }
        }

        public void Dispose()
        {
            StopMonitoring();
            _context?.Dispose();
            _cancellationTokenSource?.Dispose();
        }

        private void OnStatusChanged(object sender, StatusChangeEventArgs e)
        {
            // 检查卡片是否插入
            if (e.NewState.HasFlag(SCRState.Present) && !e.LastState.HasFlag(SCRState.Present))
            {
                OnCardInserted(e.ReaderName);
            }
            // 检查卡片是否移除
            else if (!e.NewState.HasFlag(SCRState.Present) && e.LastState.HasFlag(SCRState.Present))
            {
                ReaderStatusChanged?.Invoke(this, "NFC卡已移除");
            }
        }

        private void OnCardInserted(string readerName)
        {
            if (_context == null)
            {
                ErrorOccurred?.Invoke(this, "PC/SC context not initialized.");
                return;
            }

            try
            {
                using (var isoReader = new IsoReader(
                    context: _context,
                    readerName: readerName,
                    mode: SCardShareMode.Shared,
                    protocol: SCardProtocol.Any,
                    releaseContextOnDispose: false))
                {
                    var uid = GetCardUid(isoReader);
                    if (!string.IsNullOrEmpty(uid))
                    {
                        CardDetected?.Invoke(this, uid);
                    }
                }
            }
            catch (Exception ex)
            {
                ErrorOccurred?.Invoke(this, $"读取卡片UID失败: {ex.Message}");
            }
        }

        private string? GetCardUid(IsoReader isoReader)
        {
            var apdu = new CommandApdu(IsoCase.Case2Short, isoReader.ActiveProtocol)
            {
                CLA = 0xFF,
                Instruction = InstructionCode.GetData,
                P1 = 0x00,
                P2 = 0x00,
                Le = 0 // Let the card send as many bytes as it has for the UID
            };

            try
            {
                var response = isoReader.Transmit(apdu);

                // SW1 '90' (0x90) indicates normal processing.
                if (response.SW1 == 0x90 && response.HasData)
                {
                    return BitConverter.ToString(response.GetData()).Replace("-", "");
                }
                else
                {
                    ErrorOccurred?.Invoke(this, $"获取卡片UID失败: SW1={response.SW1}, SW2={response.SW2}");
                }
            }
            catch (Exception ex)
            {
                ErrorOccurred?.Invoke(this, $"发送APDU时发生异常: {ex.Message}");
            }

            return null;
        }
    }
}