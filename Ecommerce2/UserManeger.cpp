#include "UserManager.h"
#include <fstream>
#include <sstream> // 用于解析行数据
#include <iostream> // 用于错误信息
#include "utils.h"

UserManager::UserManager() {
    loadUsersFromFile(); // 从文件加载用户数据
}

UserManager::~UserManager() {
    // 析构时清理用户内存（显式保存需调用persistChanges()）
    for (User* user : users) {
        delete user;
    }
    users.clear();
}

// 从文件加载用户数据
void UserManager::loadUsersFromFile() {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        // 文件不存在或无法打开时默认返回（首次运行无文件属正常）
        return;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        std::vector<std::string> tokens = split(line, ','); // 按逗号分割字段
        if (tokens.size() == 4) { // 预期格式：用户名,密码,余额,类型
            try {
                std::string uname = tokens[0];
                std::string pwd = tokens[1];
                double bal = std::stod(tokens[2]); // 解析余额
                std::string type = tokens[3]; // 用户类型（Consumer/Merchant）

                // 检查用户名唯一性（文件一致性校验）
                if (isUsernameTaken(uname)) {
                    continue; // 跳过重复用户名
                }

                // 根据类型创建用户对象
                if (type == "Consumer") {
                    users.push_back(new Consumer(uname, pwd, bal));
                }
                else if (type == "Merchant") {
                    users.push_back(new Merchant(uname, pwd, bal));
                }
            }
            catch (const std::invalid_argument& ia) {
                // 忽略解析错误（如余额格式不正确）
            }
            catch (const std::out_of_range& oor) {
                // 忽略数值范围错误
            }
        }
    }
    inFile.close();
}

// 将用户数据保存到文件
void UserManager::saveUsersToFile() const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "错误：无法打开用户文件进行写入: " << filename << std::endl;
        return;
    }
    for (const User* user : users) {
        user->serialize(outFile); // 调用用户对象的序列化方法
        outFile << std::endl; // 每行一个用户
    }
    outFile.close();
}

// 持久化用户数据（保存到文件）
void UserManager::persistChanges() {
    saveUsersToFile();
}

// 检查用户名是否已存在
bool UserManager::isUsernameTaken(const std::string& uname) const {
    // 在用户列表中查找匹配的用户名
    auto it = std::find_if(users.begin(), users.end(),
        [&](const User* u) { return u->getUsername() == uname; });
    return it != users.end(); // 存在返回true
}

// 注册新用户
bool UserManager::registerUser(const std::string& uname, const std::string& pwd, const std::string& type) {
    if (isUsernameTaken(uname)) {
        return false; // 用户名已存在，注册失败
    }

    User* newUser = nullptr;
    if (type == "Consumer") { // 创建消费者用户
        newUser = new Consumer(uname, pwd); // 余额默认0.0
    }
    else if (type == "Merchant") { // 创建商家用户
        newUser = new Merchant(uname, pwd); // 余额默认0.0
    }
    else {
        return false; // 不支持的用户类型
    }

    users.push_back(newUser);
    saveUsersToFile(); // 注册成功后立即保存到文件
    return true;
}

// 用户登录
User* UserManager::loginUser(const std::string& uname, const std::string& pwd) {
    // 遍历用户列表验证用户名和密码
    for (User* user : users) {
        if (user->getUsername() == uname && user->checkPassword(pwd)) {
            return user; // 登录成功，返回用户对象
        }
    }
    return nullptr; // 用户名或密码错误，登录失败
}

// 根据用户名查找用户
User* UserManager::findUser(const std::string& uname) {
    auto it = std::find_if(users.begin(), users.end(),
        [&](const User* u) { return u->getUsername() == uname; });
    if (it != users.end()) {
        return *it; // 返回找到的用户指针
    }
    return nullptr; // 未找到用户
}