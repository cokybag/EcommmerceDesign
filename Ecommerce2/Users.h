#pragma once
#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

// ǰ���������ﳵ��
class ShoppingCart;

class User {
protected:
    std::string username;       // �û���
    std::string password;       // ����
    double balance;             // ���
    std::string accountType;    // �˻�����

public:
    User(std::string uname, std::string pwd, double bal, std::string type);
    virtual ~User() = default;  // ����������

    // ���麯������ȡ�û�����
    virtual std::string getUserType() const = 0;

    std::string getUsername() const;           // ��ȡ�û���
    bool checkPassword(const std::string& pwd) const; // ��֤����
    void setPassword(const std::string& newPwd); // ����������
    double getBalance() const;                  // ��ȡ���
    void deposit(double amount);                // ���
    bool withdraw(double amount);               // ȡ��ɹ�����true��

    virtual void displayInfo() const;           // ��ʾ�û���Ϣ

    virtual void serialize(std::ostream& os) const; // ���л�Ϊ�����
};

class Consumer : public User {
private:
    ShoppingCart* cart; // ָ���ﳵ��ָ��

public:
    Consumer(std::string uname, std::string pwd, double bal = 0.0);
    ~Consumer(); // ����������ɾ�����ﳵ

    std::string getUserType() const override; // ��д��ȡ�û�����
    void displayInfo() const override;        // ��д��ʾ�û���Ϣ
    void serialize(std::ostream& os) const override; // ��д���л�����

    ShoppingCart* getShoppingCart(); // ��ȡ���ﳵָ��
};

class Merchant : public User {
public:
    Merchant(std::string uname, std::string pwd, double bal = 0.0);
    std::string getUserType() const override; // ��д��ȡ�û�����
    void displayInfo() const override;        // ��д��ʾ�û���Ϣ
    void serialize(std::ostream& os) const override; // ��д���л�����
};

#endif // USER_H