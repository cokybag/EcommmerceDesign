#pragma once
#ifndef USERMANAGER_H
#define USERMANAGER_H

#include "Users.h"
#include <vector>
#include <string>
#include <algorithm> // 用于std::find_if

class UserManager {
private:
    std::vector<User*> users;          // 用户列表
    const std::string filename = "users.txt"; // 用户数据文件

    void loadUsersFromFile();          // 从文件加载用户数据
    void saveUsersToFile() const;      // 将用户数据保存到文件

public:
    UserManager();                     // 构造函数
    ~UserManager();                    // 析构函数

    bool registerUser(const std::string& uname, const std::string& pwd, const std::string& type); // 注册用户
    User* loginUser(const std::string& uname, const std::string& pwd); // 用户登录
    bool isUsernameTaken(const std::string& uname) const; // 检查用户名是否已存在
    User* findUser(const std::string& uname); // 查找用户（辅助方法）
    void persistChanges(); // 显式保存更改到文件
};

#endif // USERMANAGER_H