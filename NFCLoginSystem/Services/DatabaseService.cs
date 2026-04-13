using Microsoft.Data.Sqlite;
using NFCLoginSystem.Models;
using System;
using System.Collections.Generic;
using System.IO;

namespace NFCLoginSystem.Services
{
    public class DatabaseService
    {
        private readonly string _connectionString;

        public DatabaseService()
        {
            var dbPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "users.db");
            _connectionString = $"Data Source={dbPath}";
            InitializeDatabase();
        }

        private void InitializeDatabase()
        {
            using var connection = new SqliteConnection(_connectionString);
            connection.Open();

            var createUsersTable = @"
                CREATE TABLE IF NOT EXISTS Users (
                    Id INTEGER PRIMARY KEY AUTOINCREMENT,
                    Username TEXT NOT NULL UNIQUE,
                    PasswordHash TEXT NOT NULL,
                    DisplayName TEXT NOT NULL,
                    NFCCardId TEXT UNIQUE,
                    ProfileImage TEXT,
                    IsActive INTEGER DEFAULT 1,
                    IsAdmin INTEGER DEFAULT 0,
                    CreatedAt TEXT DEFAULT CURRENT_TIMESTAMP,
                    LastLogin TEXT,
                    FailedLoginAttempts INTEGER DEFAULT 0,
                    LockoutEnd TEXT
                )";

            var createSessionsTable = @"
                CREATE TABLE IF NOT EXISTS LoginSessions (
                    Id INTEGER PRIMARY KEY AUTOINCREMENT,
                    UserId INTEGER NOT NULL,
                    SessionId TEXT NOT NULL,
                    LoginTime TEXT DEFAULT CURRENT_TIMESTAMP,
                    LogoutTime TEXT,
                    LoginMethod TEXT DEFAULT 'Password',
                    NFCCardId TEXT,
                    IsActive INTEGER DEFAULT 1,
                    FOREIGN KEY (UserId) REFERENCES Users (Id)
                )";

            using var command = connection.CreateCommand();
            command.CommandText = createUsersTable;
            command.ExecuteNonQuery();

            command.CommandText = createSessionsTable;
            command.ExecuteNonQuery();

            // 创建默认管理员用户
            CreateDefaultAdmin();
        }

        private void CreateDefaultAdmin()
        {
            var adminUser = GetUserByUsername("admin");
            if (adminUser == null)
            {
                var passwordHash = AuthenticationService.HashPassword("admin123");
                CreateUser("admin", passwordHash, "管理员", true);
            }
        }

        public User? GetUserByUsername(string username)
        {
            using var connection = new SqliteConnection(_connectionString);
            connection.Open();

            using var command = connection.CreateCommand();
            command.CommandText = "SELECT * FROM Users WHERE Username = @username";
            command.Parameters.AddWithValue("@username", username);

            using var reader = command.ExecuteReader();
            if (reader.Read())
            {
                return MapUser(reader);
            }

            return null;
        }

        public User? GetUserByNFCCardId(string nfcCardId)
        {
            using var connection = new SqliteConnection(_connectionString);
            connection.Open();

            using var command = connection.CreateCommand();
            command.CommandText = "SELECT * FROM Users WHERE NFCCardId = @nfcCardId AND IsActive = 1";
            command.Parameters.AddWithValue("@nfcCardId", nfcCardId);

            using var reader = command.ExecuteReader();
            if (reader.Read())
            {
                return MapUser(reader);
            }

            return null;
        }

        public List<User> GetAllUsers()
        {
            var users = new List<User>();
            using var connection = new SqliteConnection(_connectionString);
            connection.Open();

            using var command = connection.CreateCommand();
            command.CommandText = "SELECT * FROM Users ORDER BY Username";

            using var reader = command.ExecuteReader();
            while (reader.Read())
            {
                users.Add(MapUser(reader));
            }

            return users;
        }

        public int CreateUser(string username, string passwordHash, string displayName, bool isAdmin = false)
        {
            using var connection = new SqliteConnection(_connectionString);
            connection.Open();

            using var command = connection.CreateCommand();
            command.CommandText = @"
                INSERT INTO Users (Username, PasswordHash, DisplayName, IsAdmin, CreatedAt)
                VALUES (@username, @passwordHash, @displayName, @isAdmin, @createdAt)";
            
            command.Parameters.AddWithValue("@username", username);
            command.Parameters.AddWithValue("@passwordHash", passwordHash);
            command.Parameters.AddWithValue("@displayName", displayName);
            command.Parameters.AddWithValue("@isAdmin", isAdmin ? 1 : 0);
            command.Parameters.AddWithValue("@createdAt", DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss"));

            return command.ExecuteNonQuery();
        }

        public bool UpdateUserNFCCard(int userId, string nfcCardId)
        {
            using var connection = new SqliteConnection(_connectionString);
            connection.Open();

            using var command = connection.CreateCommand();
            command.CommandText = "UPDATE Users SET NFCCardId = @nfcCardId WHERE Id = @userId";
            command.Parameters.AddWithValue("@nfcCardId", nfcCardId);
            command.Parameters.AddWithValue("@userId", userId);

            return command.ExecuteNonQuery() > 0;
        }

        public bool UpdateLastLogin(int userId)
        {
            using var connection = new SqliteConnection(_connectionString);
            connection.Open();

            using var command = connection.CreateCommand();
            command.CommandText = "UPDATE Users SET LastLogin = @lastLogin WHERE Id = @userId";
            command.Parameters.AddWithValue("@lastLogin", DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss"));
            command.Parameters.AddWithValue("@userId", userId);

            return command.ExecuteNonQuery() > 0;
        }

        public bool UpdateFailedLoginAttempts(int userId, int attempts, DateTime? lockoutEnd = null)
        {
            using var connection = new SqliteConnection(_connectionString);
            connection.Open();

            using var command = connection.CreateCommand();
            command.CommandText = "UPDATE Users SET FailedLoginAttempts = @attempts, LockoutEnd = @lockoutEnd WHERE Id = @userId";
            command.Parameters.AddWithValue("@attempts", attempts);
            command.Parameters.AddWithValue("@lockoutEnd", lockoutEnd?.ToString("yyyy-MM-dd HH:mm:ss") ?? (object)DBNull.Value);
            command.Parameters.AddWithValue("@userId", userId);

            return command.ExecuteNonQuery() > 0;
        }

        public int CreateLoginSession(int userId, string sessionId, string loginMethod, string? nfcCardId = null)
        {
            using var connection = new SqliteConnection(_connectionString);
            connection.Open();

            using var command = connection.CreateCommand();
            command.CommandText = @"
                INSERT INTO LoginSessions (UserId, SessionId, LoginMethod, NFCCardId)
                VALUES (@userId, @sessionId, @loginMethod, @nfcCardId)";
            
            command.Parameters.AddWithValue("@userId", userId);
            command.Parameters.AddWithValue("@sessionId", sessionId);
            command.Parameters.AddWithValue("@loginMethod", loginMethod);
            command.Parameters.AddWithValue("@nfcCardId", nfcCardId ?? (object)DBNull.Value);

            return command.ExecuteNonQuery();
        }

        public int UpdateUser(User user)
        {
            using var connection = new SqliteConnection(_connectionString);
            connection.Open();
            using var command = connection.CreateCommand();
            command.CommandText = @"
                UPDATE Users 
                SET DisplayName = @displayName, 
                    NFCCardId = @nfcCardId, 
                    IsActive = @isActive, 
                    IsAdmin = @isAdmin,
                    ProfileImage = @profileImage
                WHERE Id = @id";
            
            command.Parameters.AddWithValue("@id", user.Id);
            command.Parameters.AddWithValue("@displayName", user.DisplayName);
            command.Parameters.AddWithValue("@nfcCardId", user.NFCCardId ?? (object)DBNull.Value);
            command.Parameters.AddWithValue("@isActive", user.IsActive ? 1 : 0);
            command.Parameters.AddWithValue("@isAdmin", user.IsAdmin ? 1 : 0);
            command.Parameters.AddWithValue("@profileImage", user.ProfileImage ?? (object)DBNull.Value);
            
            return command.ExecuteNonQuery();
        }

        public int UpdateUserPassword(int userId, string newPasswordHash)
        {
            using var connection = new SqliteConnection(_connectionString);
            connection.Open();
            using var command = connection.CreateCommand();
            command.CommandText = "UPDATE Users SET PasswordHash = @passwordHash WHERE Id = @id";
            command.Parameters.AddWithValue("@id", userId);
            command.Parameters.AddWithValue("@passwordHash", newPasswordHash);
            return command.ExecuteNonQuery();
        }

        public int DeleteUser(int userId)
        {
            using var connection = new SqliteConnection(_connectionString);
            connection.Open();
            
            // 使用事务确保数据一致性
            using var transaction = connection.BeginTransaction();
            try
            {
                // 首先删除相关的登录会话
                using var deleteSessionsCommand = connection.CreateCommand();
                deleteSessionsCommand.Transaction = transaction;
                deleteSessionsCommand.CommandText = "DELETE FROM LoginSessions WHERE UserId = @userId";
                deleteSessionsCommand.Parameters.AddWithValue("@userId", userId);
                deleteSessionsCommand.ExecuteNonQuery();
                
                // 然后删除用户
                using var deleteUserCommand = connection.CreateCommand();
                deleteUserCommand.Transaction = transaction;
                deleteUserCommand.CommandText = "DELETE FROM Users WHERE Id = @id";
                deleteUserCommand.Parameters.AddWithValue("@id", userId);
                int rowsAffected = deleteUserCommand.ExecuteNonQuery();
                
                transaction.Commit();
                return rowsAffected;
            }
            catch
            {
                transaction.Rollback();
                throw;
            }
        }

        private User MapUser(SqliteDataReader reader)
        {
            return new User
            {
                Id = reader.GetInt32(0),
                Username = reader.GetString(1),
                PasswordHash = reader.GetString(2),
                DisplayName = reader.GetString(3),
                NFCCardId = reader.IsDBNull(4) ? null : reader.GetString(4),
                ProfileImage = reader.IsDBNull(5) ? null : reader.GetString(5),
                IsActive = reader.GetInt32(6) == 1,
                IsAdmin = reader.GetInt32(7) == 1,
                CreatedAt = DateTime.Parse(reader.GetString(8)),
                LastLogin = reader.IsDBNull(9) ? DateTime.MinValue : DateTime.Parse(reader.GetString(9)),
                FailedLoginAttempts = reader.GetInt32(10),
                LockoutEnd = reader.IsDBNull(11) ? null : DateTime.Parse(reader.GetString(11))
            };
        }
    }
}