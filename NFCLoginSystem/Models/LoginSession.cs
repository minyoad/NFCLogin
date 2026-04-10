using System;

namespace NFCLoginSystem.Models
{
    public class LoginSession
    {
        public int Id { get; set; }
        public int UserId { get; set; }
        public string SessionId { get; set; } = string.Empty;
        public DateTime LoginTime { get; set; } = DateTime.Now;
        public DateTime? LogoutTime { get; set; }
        public string LoginMethod { get; set; } = "Password"; // Password, NFC, etc.
        public string? NFCCardId { get; set; }
        public bool IsActive { get; set; } = true;
    }
}