-- 初始化测试数据
-- 运行此脚本前请确保应用程序已经运行过一次，这样数据库文件会被创建

-- 插入管理员用户（密码: admin123）
INSERT INTO Users (Username, PasswordHash, DisplayName, IsAdmin, CreatedAt) 
VALUES ('admin', 'jZae727K08KaOmKSgOaGzZhXQFPxHcT8u6UXF1L7z9I=', '管理员', 1, datetime('now'));

-- 插入普通用户（密码: user123）
INSERT INTO Users (Username, PasswordHash, DisplayName, IsAdmin, CreatedAt) 
VALUES ('user', 'fEqNCco3Yq9h5ZUglD3CZJT4lBs=', '普通用户', 0, datetime('now'));

-- 插入带NFC卡的用户（密码: nfc123）
INSERT INTO Users (Username, PasswordHash, DisplayName, NFCCardId, IsAdmin, CreatedAt) 
VALUES ('nfcuser', 'h+s2pQT2w5H8vHQpT6G5ZfALn8E=', 'NFC用户', 'A1B2C3D4', 0, datetime('now'));

-- 插入被禁用的用户
INSERT INTO Users (Username, PasswordHash, DisplayName, IsActive, IsAdmin, CreatedAt) 
VALUES ('disabled', 'fEqNCco3Yq9h5ZUglD3CZJT4lBs=', '被禁用用户', 0, 0, datetime('now'));

-- 查看插入的数据
SELECT * FROM Users;