using NFCLoginSystem.Forms;
using System;
using System.Windows.Forms;

namespace NFCLoginSystem
{
    internal static class Program
    {
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            
            try
            {
                Application.Run(new LoginForm());
            }
            catch (Exception ex)
            {
                MessageBox.Show($"应用程序发生错误: {ex.Message}\n\n{ex.StackTrace}", 
                    "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
    }
}