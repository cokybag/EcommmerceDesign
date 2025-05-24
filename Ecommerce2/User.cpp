#include "Users.h"
#include "ShoppingCart.h" // 包含购物车头文件
#include <sstream> 
#include <iomanip> 

// 用户基类（与任务1版本无变化）
User::User(std::string uname, std::string pwd, double bal, std::string type)
    : username(uname), password(pwd), balance(bal), accountType(type) {
}

std::string User::getUsername() const { return username; } // 获取用户名

bool User::checkPassword(const std::string& pwd) const {
    return password == pwd; // 验证密码是否匹配
}

void User::setPassword(const std::string& newPwd) {
    password = newPwd; // 设置新密码
}

double User::getBalance() const { return balance; } // 获取账户余额

void User::deposit(double amount) {
    if (amount > 0) { // 存款金额必须为正数
        balance += amount;
    }
}

bool User::withdraw(double amount) {
    if (amount > 0 && balance >= amount) { // 取款金额为正数且余额充足
        balance -= amount;
        return true;
    }
    return false; // 取款失败
}

void User::displayInfo() const {
    std::cout << "用户名: " << username << "\n" // 显示用户信息
        << "类型: " << accountType << "\n"
        << "余额: $" << std::fixed << std::setprecision(2) << balance << std::endl;
}

void User::serialize(std::ostream& os) const {
    os << username << "," << password << "," << balance << "," << accountType; // 序列化为CSV格式
}

// --- 消费者类 ---
Consumer::Consumer(std::string uname, std::string pwd, double bal)
    : User(uname, pwd, bal, "Consumer") {
    cart = new ShoppingCart(); // 为消费者创建新的购物车
}

Consumer::~Consumer() {
    delete cart; // 释放购物车内存
    cart = nullptr;
}

std::string Consumer::getUserType() const { return "Consumer"; } // 返回用户类型为消费者

void Consumer::displayInfo() const {
    User::displayInfo(); // 复用基类的信息显示逻辑
    // （可选）如需显示购物车摘要可在此处添加代码
    // if (cart && !cart->isEmpty()) {
    //    std::cout << "购物车: " << cart->getItems().size() << " 件商品." << std::endl;
    // }
}

void Consumer::serialize(std::ostream& os) const {
    User::serialize(os); // 复用基类的序列化逻辑
    // 购物车数据不直接随用户序列化（购物车为运行时对象，通常控制台应用不跨会话持久化购物车）
    // 如需持久化，需单独实现CartManager或存储购物车条目到文件
}

ShoppingCart* Consumer::getShoppingCart() {
    return cart; // 获取购物车指针
}


// --- 商家类 ---
Merchant::Merchant(std::string uname, std::string pwd, double bal)
    : User(uname, pwd, bal, "Merchant") {
}

std::string Merchant::getUserType() const { return "Merchant"; } // 返回用户类型为商家

void Merchant::displayInfo() const {
    User::displayInfo(); // 复用基类的信息显示逻辑
}

void Merchant::serialize(std::ostream& os) const {
    User::serialize(os); // 复用基类的序列化逻辑
}