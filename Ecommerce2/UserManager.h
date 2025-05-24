#pragma once
#ifndef USERMANAGER_H
#define USERMANAGER_H

#include "Users.h"
#include <vector>
#include <string>
#include <algorithm> // ����std::find_if

class UserManager {
private:
    std::vector<User*> users;          // �û��б�
    const std::string filename = "users.txt"; // �û������ļ�

    void loadUsersFromFile();          // ���ļ������û�����
    void saveUsersToFile() const;      // ���û����ݱ��浽�ļ�

public:
    UserManager();                     // ���캯��
    ~UserManager();                    // ��������

    bool registerUser(const std::string& uname, const std::string& pwd, const std::string& type); // ע���û�
    User* loginUser(const std::string& uname, const std::string& pwd); // �û���¼
    bool isUsernameTaken(const std::string& uname) const; // ����û����Ƿ��Ѵ���
    User* findUser(const std::string& uname); // �����û�������������
    void persistChanges(); // ��ʽ������ĵ��ļ�
};

#endif // USERMANAGER_H