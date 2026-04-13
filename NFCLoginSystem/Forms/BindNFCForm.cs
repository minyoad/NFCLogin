using NFCLoginSystem.Services;
using System;
using System.Drawing;
using System.Windows.Forms;

namespace NFCLoginSystem.Forms
{
    public partial class BindNFCForm : Form
    {
        public string? CardId { get; private set; }
        private readonly NFCPCSCService _nfcService;

        public BindNFCForm()
        {
            InitializeComponent();
            _nfcService = new NFCPCSCService();
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
            CardId = cardId;
            DialogResult = DialogResult.OK;
            Close();
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