#ifndef USERMANAGER_H
#define USERMANAGER_H

#include "Users.h"
#include <vector>
#include <string>
#include <algorithm> // 用于std::find_if

class UserManager {
private:
    std::vector<User*> users;          // 存储所有用户的指针列表
    const std::string filename = "users.txt"; // 存储用户数据的文件名（默认"users.txt"）

    void loadUsersFromFile();          // 从文件加载用户数据（私有方法）
    void saveUsersToFile() const;      // 将用户数据保存到文件（私有方法）

public:
    UserManager();                     // 构造函数（初始化时加载文件数据）
    ~UserManager();                    // 析构函数（释放内存）

    // 注册用户
    // 参数：用户名、密码、用户类型（"Consumer"或"Merchant"）
    bool registerUser(const std::string& uname, const std::string& pwd, const std::string& type);

    // 用户登录
    // 参数：用户名、密码，返回匹配的用户指针，失败返回nullptr
    User* loginUser(const std::string& uname, const std::string& pwd);

    // 检查用户名是否已存在
    bool isUsernameTaken(const std::string& uname) const;

    // 根据用户名查找用户（辅助方法）
    User* findUser(const std::string& uname);

    // 显式将数据持久化到文件（调用saveUsersToFile）
    void persistChanges();
};

#endif // USERMANAGER_H