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

            // Simplified Users table to only map Username to NFCCardId
            var createUsersTable = @"
                CREATE TABLE IF NOT EXISTS Users (
                    Id INTEGER PRIMARY KEY AUTOINCREMENT,
                    Username TEXT NOT NULL UNIQUE,
                    NFCCardId TEXT UNIQUE
                )";

            using var command = connection.CreateCommand();
            command.CommandText = createUsersTable;
            command.ExecuteNonQuery();

            // The default admin is now a Windows account, but we might need a mapping if we bind a card to it.
            // We'll handle the mapping on demand instead of on initialization.
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
                return new User
                {
                    Id = reader.GetInt32(0),
                    Username = reader.GetString(1),
                    NFCCardId = reader.IsDBNull(2) ? null : reader.GetString(2)
                };
            }

            return null;
        }

        public User? GetUserByNFCCardId(string nfcCardId)
        {
            using var connection = new SqliteConnection(_connectionString);
            connection.Open();

            using var command = connection.CreateCommand();
            command.CommandText = "SELECT * FROM Users WHERE NFCCardId = @nfcCardId";
            command.Parameters.AddWithValue("@nfcCardId", nfcCardId);

            using var reader = command.ExecuteReader();
            if (reader.Read())
            {
                return new User
                {
                    Id = reader.GetInt32(0),
                    Username = reader.GetString(1),
                    NFCCardId = reader.IsDBNull(2) ? null : reader.GetString(2)
                };
            }

            return null;
        }

        public void UpdateUserNFCCard(string username, string? nfcCardId)
        {
            var user = GetUserByUsername(username);

            using var connection = new SqliteConnection(_connectionString);
            connection.Open();
            using var command = connection.CreateCommand();

            if (user != null)
            {
                // Update existing user's NFC card
                command.CommandText = "UPDATE Users SET NFCCardId = @nfcCardId WHERE Username = @username";
            }
            else
            {
                // Create a new mapping for this user
                command.CommandText = "INSERT INTO Users (Username, NFCCardId) VALUES (@username, @nfcCardId)";
            }
            
            command.Parameters.AddWithValue("@username", username);
            command.Parameters.AddWithValue("@nfcCardId", nfcCardId ?? (object)DBNull.Value);
            
            command.ExecuteNonQuery();
        }

        public void DeleteUserMapping(string username)
        {
            using var connection = new SqliteConnection(_connectionString);
            connection.Open();
            using var command = connection.CreateCommand();
            command.CommandText = "DELETE FROM Users WHERE Username = @username";
            command.Parameters.AddWithValue("@username", username);
            command.ExecuteNonQuery();
        }

        // The MapUser method is no longer needed as we are creating the user object directly.
        // All other methods like CreateUser, UpdateUser, DeleteUser, etc. are removed as user management is now done via Windows API.
    }
}