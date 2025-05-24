#include "Users.h"
#include <sstream> // �������л��е��ַ�������������Ҫ������CSV���㹻��
#include <iomanip> // ����std::fixed��std::setprecision

// �û��๹�캯��
User::User(std::string uname, std::string pwd, double bal, std::string type)
    : username(uname), password(pwd), balance(bal), accountType(type) {
}

// ��ȡ�û���
std::string User::getUsername() const { return username; }

// ��֤�����Ƿ���ȷ
bool User::checkPassword(const std::string& pwd) const {
    return password == pwd;
}

// ����������
void User::setPassword(const std::string& newPwd) {
    password = newPwd;
}

// ��ȡ�˻����
double User::getBalance() const { return balance; }

// ��������Ϊ������
void User::deposit(double amount) {
    if (amount > 0) {
        balance += amount;
    }
}

// ȡ�������Ϊ�����Ҳ�������
bool User::withdraw(double amount) {
    if (amount > 0 && balance >= amount) {
        balance -= amount;
        return true;
    }
    return false;
}

// ��ʾ�û���Ϣ
void User::displayInfo() const {
    std::cout << "�û���: " << username << "\n"
        << "����: " << accountType << "\n"
        << "���: $" << std::fixed << std::setprecision(2) << balance << std::endl;
}

// ���û���Ϣ���л�ΪCSV��ʽ
void User::serialize(std::ostream& os) const {
    os << username << "," << password << "," << balance << "," << accountType;
}

// --- �������� ---
// �������๹�캯��
Consumer::Consumer(std::string uname, std::string pwd, double bal)
    : User(uname, pwd, bal, "Consumer") {
}

// ��ȡ�û����ͣ������ߣ�
std::string Consumer::getUserType() const { return "Consumer"; }

// ��ʾ��������Ϣ�����û��෽����������������ض���Ϣ��
void Consumer::displayInfo() const {
    User::displayInfo(); // ���û�����ʾ����
    // ��������������ض���Ϣ���ڴ˴���չ
}

// ���л���������Ϣ��������д�����ͣ�������������
void Consumer::serialize(std::ostream& os) const {
    User::serialize(os); // �����Ѵ������л�
}


// --- �̼��� ---
// �̼��๹�캯��
Merchant::Merchant(std::string uname, std::string pwd, double bal)
    : User(uname, pwd, bal, "Merchant") {
}

// ��ȡ�û����ͣ��̼ң�
std::string Merchant::getUserType() const { return "Merchant"; }

// ��ʾ�̼���Ϣ�����û��෽����������̼��ض���Ϣ��
void Merchant::displayInfo() const {
    User::displayInfo(); // ���û�����ʾ����
    // ��������̼��ض���Ϣ���ڴ˴���չ
}

// ���л��̼���Ϣ��������д�����ͣ�������������
void Merchant::serialize(std::ostream& os) const {
    User::serialize(os); // �����Ѵ������л�
}