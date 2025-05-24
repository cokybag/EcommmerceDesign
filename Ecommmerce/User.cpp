#include "Users.h"
#include <sstream> // 用于序列化中的字符串流（如有需要，但简单CSV已足够）
#include <iomanip> // 用于std::fixed和std::setprecision

// 用户类构造函数
User::User(std::string uname, std::string pwd, double bal, std::string type)
    : username(uname), password(pwd), balance(bal), accountType(type) {
}

// 获取用户名
std::string User::getUsername() const { return username; }

// 验证密码是否正确
bool User::checkPassword(const std::string& pwd) const {
    return password == pwd;
}

// 设置新密码
void User::setPassword(const std::string& newPwd) {
    password = newPwd;
}

// 获取账户余额
double User::getBalance() const { return balance; }

// 存款（金额必须为正数）
void User::deposit(double amount) {
    if (amount > 0) {
        balance += amount;
    }
}

// 取款（金额必须为正数且不超过余额）
bool User::withdraw(double amount) {
    if (amount > 0 && balance >= amount) {
        balance -= amount;
        return true;
    }
    return false;
}

// 显示用户信息
void User::displayInfo() const {
    std::cout << "用户名: " << username << "\n"
        << "类型: " << accountType << "\n"
        << "余额: $" << std::fixed << std::setprecision(2) << balance << std::endl;
}

// 将用户信息序列化为CSV格式
void User::serialize(std::ostream& os) const {
    os << username << "," << password << "," << balance << "," << accountType;
}

// --- 消费者类 ---
// 消费者类构造函数
Consumer::Consumer(std::string uname, std::string pwd, double bal)
    : User(uname, pwd, bal, "Consumer") {
}

// 获取用户类型（消费者）
std::string Consumer::getUserType() const { return "Consumer"; }

// 显示消费者信息（调用基类方法，可添加消费者特定信息）
void Consumer::displayInfo() const {
    User::displayInfo(); // 调用基类显示方法
    // 如需添加消费者特定信息，在此处扩展
}

// 序列化消费者信息（基类已写入类型，无需额外操作）
void Consumer::serialize(std::ostream& os) const {
    User::serialize(os); // 基类已处理序列化
}


// --- 商家类 ---
// 商家类构造函数
Merchant::Merchant(std::string uname, std::string pwd, double bal)
    : User(uname, pwd, bal, "Merchant") {
}

// 获取用户类型（商家）
std::string Merchant::getUserType() const { return "Merchant"; }

// 显示商家信息（调用基类方法，可添加商家特定信息）
void Merchant::displayInfo() const {
    User::displayInfo(); // 调用基类显示方法
    // 如需添加商家特定信息，在此处扩展
}

// 序列化商家信息（基类已写入类型，无需额外操作）
void Merchant::serialize(std::ostream& os) const {
    User::serialize(os); // 基类已处理序列化
}