#pragma once
#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

// 前置声明购物车类
class ShoppingCart;

class User {
protected:
    std::string username;       // 用户名
    std::string password;       // 密码
    double balance;             // 余额
    std::string accountType;    // 账户类型

public:
    User(std::string uname, std::string pwd, double bal, std::string type);
    virtual ~User() = default;  // 虚析构函数

    // 纯虚函数：获取用户类型
    virtual std::string getUserType() const = 0;

    std::string getUsername() const;           // 获取用户名
    bool checkPassword(const std::string& pwd) const; // 验证密码
    void setPassword(const std::string& newPwd); // 设置新密码
    double getBalance() const;                  // 获取余额
    void deposit(double amount);                // 存款
    bool withdraw(double amount);               // 取款（成功返回true）

    virtual void displayInfo() const;           // 显示用户信息

    virtual void serialize(std::ostream& os) const; // 序列化为输出流
};

class Consumer : public User {
private:
    ShoppingCart* cart; // 指向购物车的指针

public:
    Consumer(std::string uname, std::string pwd, double bal = 0.0);
    ~Consumer(); // 析构函数：删除购物车

    std::string getUserType() const override; // 重写获取用户类型
    void displayInfo() const override;        // 重写显示用户信息
    void serialize(std::ostream& os) const override; // 重写序列化方法

    ShoppingCart* getShoppingCart(); // 获取购物车指针
};

class Merchant : public User {
public:
    Merchant(std::string uname, std::string pwd, double bal = 0.0);
    std::string getUserType() const override; // 重写获取用户类型
    void displayInfo() const override;        // 重写显示用户信息
    void serialize(std::ostream& os) const override; // 重写序列化方法
};

#endif // USER_H