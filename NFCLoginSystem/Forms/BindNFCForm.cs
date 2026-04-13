using NFCLoginSystem.Services;
using System;
using System.Windows.Forms;

namespace NFCLoginSystem.Forms
{
    public partial class BindNFCForm : Form
    {
        private readonly NFCPCSCService _nfcService;
        private readonly DatabaseService _dbService;
        private readonly string _username;

        public BindNFCForm(string username)
        {
            InitializeComponent();
            _nfcService = new NFCPCSCService();
            _dbService = new DatabaseService();
            _username = username;

            _nfcService.CardDetected += OnCardDetected;
            _nfcService.ReaderStatusChanged += OnReaderStatusChanged;
            _nfcService.StartMonitoring();
        }

        private void OnCardDetected(object? sender, string cardId)
        {
            if (InvokeRequired)
            {
                Invoke(new Action(() => OnCardDetected(sender, cardId)));
                return;
            }

            try
            {
                _dbService.UpdateUserNFCCard(_username, cardId);
                MessageBox.Show($"成功将卡 {cardId} 绑定到用户 {_username}。", "绑定成功", MessageBoxButtons.OK, MessageBoxIcon.Information);
                DialogResult = DialogResult.OK;
                Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show($"绑定失败: {ex.Message}", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void OnReaderStatusChanged(object? sender, string status)
        {
            if (InvokeRequired)
            {
                Invoke(new Action(() => OnReaderStatusChanged(sender, status)));
                return;
            }
            lblStatus.Text = status;
        }

        private void BindNFCForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            _nfcService.StopMonitoring();
            _nfcService.Dispose();
        }
    }
}