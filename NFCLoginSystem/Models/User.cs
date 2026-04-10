using System;

namespace NFCLoginSystem.Models
{
    public class User
    {
        public int Id { get; set; }
        public string Username { get; set; } = string.Empty;
        public string PasswordHash { get; set; } = string.Empty;
        public string DisplayName { get; set; } = string.Empty;
        public string? NFCCardId { get; set; }
        public string? ProfileImage { get; set; }
        public bool IsActive { get; set; } = true;
        public bool IsAdmin { get; set; } = false;
        public DateTime CreatedAt { get; set; } = DateTime.Now;
        public DateTime LastLogin { get; set; }
        public int FailedLoginAttempts { get; set; } = 0;
        public DateTime? LockoutEnd { get; set; }

        public bool IsLockedOut => LockoutEnd.HasValue && LockoutEnd.Value > DateTime.Now;
    }
}