#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream> // 用于潜在的调试/显示功能

class User {
protected:
    std::string username;       // 用户名
    std::string password;       // 在真实系统中，此密码应进行哈希处理
    double balance;             // 账户余额
    std::string accountType;    // 用户类型（"Consumer"或"Merchant"）

public:
    User(std::string uname, std::string pwd, double bal, std::string type);
    virtual ~User() = default;  // 对于包含虚函数的基类，虚析构函数至关重要

    // 纯虚函数：获取用户类型（子类必须实现）
    virtual std::string getUserType() const = 0;

    // 通用功能方法
    std::string getUsername() const;           // 获取用户名
    bool checkPassword(const std::string& pwd) const; // 验证密码是否正确
    void setPassword(const std::string& newPwd); // 设置新密码（需考虑安全影响）
    double getBalance() const;                  // 获取账户余额
    void deposit(double amount);                // 存款（金额必须为正数）
    bool withdraw(double amount);               // 取款（成功返回true）

    // 显示用户信息的虚函数（供调试或通用信息展示）
    virtual void displayInfo() const;

    // 文件存储相关：序列化到输出流（供子类重写）
    virtual void serialize(std::ostream& os) const;
    // 反序列化的静态工厂方法可能有用，但实现较复杂
    // 为简化起见，反序列化逻辑在UserManager中处理
};

// 消费者类
class Consumer : public User {
public:
    // 构造函数（默认余额为0.0）
    Consumer(std::string uname, std::string pwd, double bal = 0.0);
    std::string getUserType() const override; // 重写获取用户类型方法（返回"Consumer"）
    void displayInfo() const override;        // 重写显示信息方法
    void serialize(std::ostream& os) const override; // 重写序列化方法
};

// 商家类
class Merchant : public User {
public:
    // 构造函数（默认余额为0.0）
    Merchant(std::string uname, std::string pwd, double bal = 0.0);
    std::string getUserType() const override; // 重写获取用户类型方法（返回"Merchant"）
    void displayInfo() const override;        // 重写显示信息方法
    void serialize(std::ostream& os) const override; // 重写序列化方法
    // 商家后续可能添加特定方法（如列出其所有商品）
};

#endif // USER_H