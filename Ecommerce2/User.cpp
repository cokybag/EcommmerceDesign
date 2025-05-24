#include "Users.h"
#include "ShoppingCart.h" // �������ﳵͷ�ļ�
#include <sstream> 
#include <iomanip> 

// �û����ࣨ������1�汾�ޱ仯��
User::User(std::string uname, std::string pwd, double bal, std::string type)
    : username(uname), password(pwd), balance(bal), accountType(type) {
}

std::string User::getUsername() const { return username; } // ��ȡ�û���

bool User::checkPassword(const std::string& pwd) const {
    return password == pwd; // ��֤�����Ƿ�ƥ��
}

void User::setPassword(const std::string& newPwd) {
    password = newPwd; // ����������
}

double User::getBalance() const { return balance; } // ��ȡ�˻����

void User::deposit(double amount) {
    if (amount > 0) { // ��������Ϊ����
        balance += amount;
    }
}

bool User::withdraw(double amount) {
    if (amount > 0 && balance >= amount) { // ȡ����Ϊ������������
        balance -= amount;
        return true;
    }
    return false; // ȡ��ʧ��
}

void User::displayInfo() const {
    std::cout << "�û���: " << username << "\n" // ��ʾ�û���Ϣ
        << "����: " << accountType << "\n"
        << "���: $" << std::fixed << std::setprecision(2) << balance << std::endl;
}

void User::serialize(std::ostream& os) const {
    os << username << "," << password << "," << balance << "," << accountType; // ���л�ΪCSV��ʽ
}

// --- �������� ---
Consumer::Consumer(std::string uname, std::string pwd, double bal)
    : User(uname, pwd, bal, "Consumer") {
    cart = new ShoppingCart(); // Ϊ�����ߴ����µĹ��ﳵ
}

Consumer::~Consumer() {
    delete cart; // �ͷŹ��ﳵ�ڴ�
    cart = nullptr;
}

std::string Consumer::getUserType() const { return "Consumer"; } // �����û�����Ϊ������

void Consumer::displayInfo() const {
    User::displayInfo(); // ���û������Ϣ��ʾ�߼�
    // ����ѡ��������ʾ���ﳵժҪ���ڴ˴���Ӵ���
    // if (cart && !cart->isEmpty()) {
    //    std::cout << "���ﳵ: " << cart->getItems().size() << " ����Ʒ." << std::endl;
    // }
}

void Consumer::serialize(std::ostream& os) const {
    User::serialize(os); // ���û�������л��߼�
    // ���ﳵ���ݲ�ֱ�����û����л������ﳵΪ����ʱ����ͨ������̨Ӧ�ò���Ự�־û����ﳵ��
    // ����־û����赥��ʵ��CartManager��洢���ﳵ��Ŀ���ļ�
}

ShoppingCart* Consumer::getShoppingCart() {
    return cart; // ��ȡ���ﳵָ��
}


// --- �̼��� ---
Merchant::Merchant(std::string uname, std::string pwd, double bal)
    : User(uname, pwd, bal, "Merchant") {
}

std::string Merchant::getUserType() const { return "Merchant"; } // �����û�����Ϊ�̼�

void Merchant::displayInfo() const {
    User::displayInfo(); // ���û������Ϣ��ʾ�߼�
}

void Merchant::serialize(std::ostream& os) const {
    User::serialize(os); // ���û�������л��߼�
}