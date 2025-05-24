#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream> // ����Ǳ�ڵĵ���/��ʾ����

class User {
protected:
    std::string username;       // �û���
    std::string password;       // ����ʵϵͳ�У�������Ӧ���й�ϣ����
    double balance;             // �˻����
    std::string accountType;    // �û����ͣ�"Consumer"��"Merchant"��

public:
    User(std::string uname, std::string pwd, double bal, std::string type);
    virtual ~User() = default;  // ���ڰ����麯���Ļ��࣬����������������Ҫ

    // ���麯������ȡ�û����ͣ��������ʵ�֣�
    virtual std::string getUserType() const = 0;

    // ͨ�ù��ܷ���
    std::string getUsername() const;           // ��ȡ�û���
    bool checkPassword(const std::string& pwd) const; // ��֤�����Ƿ���ȷ
    void setPassword(const std::string& newPwd); // ���������루�迼�ǰ�ȫӰ�죩
    double getBalance() const;                  // ��ȡ�˻����
    void deposit(double amount);                // ��������Ϊ������
    bool withdraw(double amount);               // ȡ��ɹ�����true��

    // ��ʾ�û���Ϣ���麯���������Ի�ͨ����Ϣչʾ��
    virtual void displayInfo() const;

    // �ļ��洢��أ����л������������������д��
    virtual void serialize(std::ostream& os) const;
    // �����л��ľ�̬���������������ã���ʵ�ֽϸ���
    // Ϊ������������л��߼���UserManager�д���
};

// ��������
class Consumer : public User {
public:
    // ���캯����Ĭ�����Ϊ0.0��
    Consumer(std::string uname, std::string pwd, double bal = 0.0);
    std::string getUserType() const override; // ��д��ȡ�û����ͷ���������"Consumer"��
    void displayInfo() const override;        // ��д��ʾ��Ϣ����
    void serialize(std::ostream& os) const override; // ��д���л�����
};

// �̼���
class Merchant : public User {
public:
    // ���캯����Ĭ�����Ϊ0.0��
    Merchant(std::string uname, std::string pwd, double bal = 0.0);
    std::string getUserType() const override; // ��д��ȡ�û����ͷ���������"Merchant"��
    void displayInfo() const override;        // ��д��ʾ��Ϣ����
    void serialize(std::ostream& os) const override; // ��д���л�����
    // �̼Һ�����������ض����������г���������Ʒ��
};

#endif // USER_H