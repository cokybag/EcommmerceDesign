#include "UserManager.h"
#include <fstream>
#include <sstream> // 用于解析行数据
#include <iostream> // 用于错误信息

// 辅助函数：按分隔符分割字符串
std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}


UserManager::UserManager() {
    loadUsersFromFile(); // 从文件加载用户数据
}

UserManager::~UserManager() {
    // saveUsersToFile(); // 退出时保存数据，或显式管理保存逻辑
    for (User* user : users) {
        delete user; // 释放用户对象内存
    }
    users.clear(); // 清空用户列表
}

void UserManager::loadUsersFromFile() {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        // std::cerr << "Warning: Could not open user file: " << filename << ". Starting fresh." << std::endl;
        // 首次运行时文件不存在属于正常情况
        return;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue; // 跳过空行
        std::vector<std::string> tokens = split(line, ','); // 按逗号分割行数据
        if (tokens.size() == 4) { // 验证字段数量（用户名、密码、余额、类型）
            try {
                std::string uname = tokens[0];
                std::string pwd = tokens[1];
                double bal = std::stod(tokens[2]);
                std::string type = tokens[3];

                if (isUsernameTaken(uname)) { // 检查用户名是否重复（文件数据应保证唯一性）
                    // std::cerr << "Warning: Duplicate username '" << uname << "' found in file. Skipping." << std::endl;
                    continue;
                }

                // 根据用户类型创建对应对象
                if (type == "Consumer") {
                    users.push_back(new Consumer(uname, pwd, bal));
                }
                else if (type == "Merchant") {
                    users.push_back(new Merchant(uname, pwd, bal));
                }
                else {
                    // std::cerr << "Warning: Unknown user type '" << type << "' for user '" << uname << "'. Skipping." << std::endl;
                }
            }
            catch (const std::invalid_argument& ia) {
                // std::cerr << "Error parsing balance for a user in file: " << ia.what() << " Line: " << line << std::endl;
            }
            catch (const std::out_of_range& oor) {
                // std::cerr << "Balance value out of range for a user in file. Line: " << line << std::endl;
            }
        }
        else {
            // std::cerr << "Warning: Malformed line in user file: " << line << ". Skipping." << std::endl;
        }
    }
    inFile.close(); // 关闭文件流
}

void UserManager::saveUsersToFile() const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open user file for writing: " << filename << std::endl;
        return;
    }
    for (const User* user : users) {
        user->serialize(outFile); // 调用用户对象的序列化方法
        outFile << std::endl; // 换行
    }
    outFile.close(); // 关闭文件流
}

void UserManager::persistChanges() {
    saveUsersToFile(); // 持久化用户数据到文件
}

// 检查用户名是否已被使用
bool UserManager::isUsernameTaken(const std::string& uname) const {
    auto it = std::find_if(users.begin(), users.end(),
        [&](const User* u) { return u->getUsername() == uname; });
    return it != users.end(); // 找到则返回true
}

// 注册新用户
bool UserManager::registerUser(const std::string& uname, const std::string& pwd, const std::string& type) {
    if (isUsernameTaken(uname)) {
        return false; // 用户名已存在
    }

    User* newUser = nullptr;
    // 根据类型创建对应用户对象
    if (type == "Consumer") {
        newUser = new Consumer(uname, pwd);
    }
    else if (type == "Merchant") {
        newUser = new Merchant(uname, pwd);
    }
    else {
        return false; // 无效用户类型
    }

    users.push_back(newUser); // 添加到用户列表
    saveUsersToFile(); // 立即保存到文件
    return true;
}

// 用户登录验证
User* UserManager::loginUser(const std::string& uname, const std::string& pwd) {
    for (User* user : users) {
        if (user->getUsername() == uname && user->checkPassword(pwd)) {
            return user; // 验证成功，返回用户对象
        }
    }
    return nullptr; // 登录失败
}

// 根据用户名查找用户
User* UserManager::findUser(const std::string& uname) {
    auto it = std::find_if(users.begin(), users.end(),
        [&](const User* u) { return u->getUsername() == uname; });
    if (it != users.end()) {
        return *it; // 找到则返回用户指针
    }
    return nullptr; // 未找到
}