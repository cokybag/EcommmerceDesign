#ifndef USERMANAGER_H
#define USERMANAGER_H

#include "Users.h"
#include <vector>
#include <string>
#include <algorithm> // ����std::find_if

class UserManager {
private:
    std::vector<User*> users;          // �洢�����û���ָ���б�
    const std::string filename = "users.txt"; // �洢�û����ݵ��ļ�����Ĭ��"users.txt"��

    void loadUsersFromFile();          // ���ļ������û����ݣ�˽�з�����
    void saveUsersToFile() const;      // ���û����ݱ��浽�ļ���˽�з�����

public:
    UserManager();                     // ���캯������ʼ��ʱ�����ļ����ݣ�
    ~UserManager();                    // �����������ͷ��ڴ棩

    // ע���û�
    // �������û��������롢�û����ͣ�"Consumer"��"Merchant"��
    bool registerUser(const std::string& uname, const std::string& pwd, const std::string& type);

    // �û���¼
    // �������û��������룬����ƥ����û�ָ�룬ʧ�ܷ���nullptr
    User* loginUser(const std::string& uname, const std::string& pwd);

    // ����û����Ƿ��Ѵ���
    bool isUsernameTaken(const std::string& uname) const;

    // �����û��������û�������������
    User* findUser(const std::string& uname);

    // ��ʽ�����ݳ־û����ļ�������saveUsersToFile��
    void persistChanges();
};

#endif // USERMANAGER_H