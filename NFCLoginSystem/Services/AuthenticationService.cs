using NFCLoginSystem.Models;
using NFCLoginSystem.Services;
using System;
using System.Security.Cryptography;
using System.Text;

namespace NFCLoginSystem.Services
{
    public class AuthenticationService
    {
        private readonly DatabaseService _databaseService;
        private User? _currentUser;

        public AuthenticationService(DatabaseService databaseService)
        {
            _databaseService = databaseService;
        }

        public User? CurrentUser => _currentUser;
        public bool IsAuthenticated => _currentUser != null;

        public AuthenticationResult Login(string username, string password)
        {
            var user = _databaseService.GetUserByUsername(username);
            
            if (user == null)
            {
                return new AuthenticationResult { Success = false, Message = "用户名或密码错误" };
            }

            if (!user.IsActive)
            {
                return new AuthenticationResult { Success = false, Message = "用户账户已被禁用" };
            }

            if (user.IsLockedOut)
            {
                return new AuthenticationResult { Success = false, Message = $"账户已被锁定，请稍后再试" };
            }

            var passwordHash = HashPassword(password);
            if (user.PasswordHash != passwordHash)
            {
                // 增加失败登录次数
                var failedAttempts = user.FailedLoginAttempts + 1;
                DateTime? lockoutEnd = null;

                if (failedAttempts >= 5)
                {
                    lockoutEnd = DateTime.Now.AddMinutes(15);
                }

                _databaseService.UpdateFailedLoginAttempts(user.Id, failedAttempts, lockoutEnd);
                
                return new AuthenticationResult { Success = false, Message = "用户名或密码错误" };
            }

            // 登录成功，重置失败次数
            _databaseService.UpdateFailedLoginAttempts(user.Id, 0, null);
            _databaseService.UpdateLastLogin(user.Id);

            // 创建登录会话
            var sessionId = Guid.NewGuid().ToString();
            _databaseService.CreateLoginSession(user.Id, sessionId, "Password");

            _currentUser = user;

            return new AuthenticationResult { Success = true, Message = "登录成功", User = user };
        }

        public AuthenticationResult LoginWithNFC(string nfcCardId)
        {
            if (string.IsNullOrEmpty(nfcCardId))
            {
                return new AuthenticationResult { Success = false, Message = "NFC卡ID无效" };
            }

            var user = _databaseService.GetUserByNFCCardId(nfcCardId);
            
            if (user == null)
            {
                return new AuthenticationResult { Success = false, Message = "未找到绑定的NFC卡" };
            }

            if (!user.IsActive)
            {
                return new AuthenticationResult { Success = false, Message = "用户账户已被禁用" };
            }

            if (user.IsLockedOut)
            {
                return new AuthenticationResult { Success = false, Message = "账户已被锁定，请稍后再试" };
            }

            // 更新最后登录时间
            _databaseService.UpdateLastLogin(user.Id);

            // 创建登录会话
            var sessionId = Guid.NewGuid().ToString();
            _databaseService.CreateLoginSession(user.Id, sessionId, "NFC", nfcCardId);

            _currentUser = user;

            return new AuthenticationResult { Success = true, Message = "NFC登录成功", User = user };
        }

        public void Logout()
        {
            _currentUser = null;
        }

        public bool ChangePassword(string currentPassword, string newPassword)
        {
            if (_currentUser == null)
                return false;

            var currentHash = HashPassword(currentPassword);
            if (_currentUser.PasswordHash != currentHash)
                return false;

            var newHash = HashPassword(newPassword);
            // 这里应该实现更新密码的逻辑
            return true;
        }

        public static string HashPassword(string password)
        {
            using var sha256 = SHA256.Create();
            var bytes = Encoding.UTF8.GetBytes(password);
            var hash = sha256.ComputeHash(bytes);
            return Convert.ToBase64String(hash);
        }

        public bool ValidatePassword(string password)
        {
            if (string.IsNullOrEmpty(password))
                return false;

            if (password.Length < 6)
                return false;

            return true;
        }
    }

    public class AuthenticationResult
    {
        public bool Success { get; set; }
        public string Message { get; set; } = string.Empty;
        public User? User { get; set; }
    }
}