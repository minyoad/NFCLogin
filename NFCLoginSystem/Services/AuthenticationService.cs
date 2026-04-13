using NFCLoginSystem.Models;
using System;
using System.DirectoryServices.AccountManagement;

namespace NFCLoginSystem.Services
{
    public class AuthenticationService
    {
        private readonly DatabaseService _databaseService;
        private UserPrincipal? _currentUser;

        public AuthenticationService(DatabaseService databaseService)
        {
            _databaseService = databaseService;
        }

        public UserPrincipal? CurrentUser => _currentUser;
        public bool IsAuthenticated => _currentUser != null;

        public AuthenticationResult Login(string username, string password)
        {
            using (var context = new PrincipalContext(ContextType.Machine))
            {
                try
                {
                    // 1. 验证凭据
                    bool isValid = context.ValidateCredentials(username, password);
                    if (!isValid)
                    {
                        return new AuthenticationResult { Success = false, Message = "用户名或密码错误" };
                    }

                    // 2. 获取用户信息
                    var user = UserPrincipal.FindByIdentity(context, IdentityType.SamAccountName, username);
                    if (user == null)
                    {
                        // 这通常不应该发生，因为ValidateCredentials已经成功
                        return new AuthenticationResult { Success = false, Message = "无法找到用户信息" };
                    }

                    // 3. 检查账户是否被禁用
                    if (user.Enabled == false)
                    {
                        return new AuthenticationResult { Success = false, Message = "用户账户已被禁用" };
                    }

                    // 4. 检查账户是否被锁定
                    if (user.IsAccountLockedOut())
                    {
                        return new AuthenticationResult { Success = false, Message = "账户已被锁定，请联系管理员" };
                    }

                    _currentUser = user;
                    return new AuthenticationResult { Success = true, Message = "登录成功", User = user };
                }
                catch (Exception ex)
                {
                    // 记录日志会更好
                    return new AuthenticationResult { Success = false, Message = $"验证过程中发生错误: {ex.Message}" };
                }
            }
        }

        public AuthenticationResult LoginWithNFC(string nfcCardId)
        {
            if (string.IsNullOrEmpty(nfcCardId))
            {
                return new AuthenticationResult { Success = false, Message = "NFC卡ID无效" };
            }

            // 1. 从数据库查找绑定的用户
            var dbUser = _databaseService.GetUserByNFCCardId(nfcCardId);
            if (dbUser == null || string.IsNullOrEmpty(dbUser.Username))
            {
                return new AuthenticationResult { Success = false, Message = "未找到绑定的NFC卡" };
            }

            using (var context = new PrincipalContext(ContextType.Machine))
            {
                try
                {
                    // 2. 获取系统用户信息
                    var user = UserPrincipal.FindByIdentity(context, IdentityType.SamAccountName, dbUser.Username);
                    if (user == null)
                    {
                        return new AuthenticationResult { Success = false, Message = $"NFC卡绑定的系统用户 '{dbUser.Username}' 不存在" };
                    }

                    // 3. 检查账户状态
                    if (user.Enabled == false)
                    {
                        return new AuthenticationResult { Success = false, Message = "用户账户已被禁用" };
                    }

                    if (user.IsAccountLockedOut())
                    {
                        return new AuthenticationResult { Success = false, Message = "账户已被锁定，请联系管理员" };
                    }

                    _currentUser = user;
                    return new AuthenticationResult { Success = true, Message = "NFC登录成功", User = user };
                }
                catch (Exception ex)
                {
                    return new AuthenticationResult { Success = false, Message = $"验证过程中发生错误: {ex.Message}" };
                }
            }
        }

        public void Logout()
        {
            _currentUser = null;
        }

        public bool IsCurrentUserAdmin()
        {
            // 最终方案：直接检查当前运行进程的令牌是否属于管理员组
            // 因为程序已配置为 requireAdministrator，所以这个检查应该能反映登录用户的真实权限
            using (var identity = System.Security.Principal.WindowsIdentity.GetCurrent())
            {
                var principal = new System.Security.Principal.WindowsPrincipal(identity);
                return principal.IsInRole(System.Security.Principal.WindowsBuiltInRole.Administrator);
            }
        }
    }

    public class AuthenticationResult
    {
        public bool Success { get; set; }
        public string Message { get; set; } = string.Empty;
        public UserPrincipal? User { get; set; }
    }
}