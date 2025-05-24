#include "UserManager.h"
#include <fstream>
#include <sstream> // ���ڽ���������
#include <iostream> // ���ڴ�����Ϣ
#include "utils.h"

UserManager::UserManager() {
    loadUsersFromFile(); // ���ļ������û�����
}

UserManager::~UserManager() {
    // ����ʱ�����û��ڴ棨��ʽ���������persistChanges()��
    for (User* user : users) {
        delete user;
    }
    users.clear();
}

// ���ļ������û�����
void UserManager::loadUsersFromFile() {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        // �ļ������ڻ��޷���ʱĬ�Ϸ��أ��״��������ļ���������
        return;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        std::vector<std::string> tokens = split(line, ','); // �����ŷָ��ֶ�
        if (tokens.size() == 4) { // Ԥ�ڸ�ʽ���û���,����,���,����
            try {
                std::string uname = tokens[0];
                std::string pwd = tokens[1];
                double bal = std::stod(tokens[2]); // �������
                std::string type = tokens[3]; // �û����ͣ�Consumer/Merchant��

                // ����û���Ψһ�ԣ��ļ�һ����У�飩
                if (isUsernameTaken(uname)) {
                    continue; // �����ظ��û���
                }

                // �������ʹ����û�����
                if (type == "Consumer") {
                    users.push_back(new Consumer(uname, pwd, bal));
                }
                else if (type == "Merchant") {
                    users.push_back(new Merchant(uname, pwd, bal));
                }
            }
            catch (const std::invalid_argument& ia) {
                // ���Խ�������������ʽ����ȷ��
            }
            catch (const std::out_of_range& oor) {
                // ������ֵ��Χ����
            }
        }
    }
    inFile.close();
}

// ���û����ݱ��浽�ļ�
void UserManager::saveUsersToFile() const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "�����޷����û��ļ�����д��: " << filename << std::endl;
        return;
    }
    for (const User* user : users) {
        user->serialize(outFile); // �����û���������л�����
        outFile << std::endl; // ÿ��һ���û�
    }
    outFile.close();
}

// �־û��û����ݣ����浽�ļ���
void UserManager::persistChanges() {
    saveUsersToFile();
}

// ����û����Ƿ��Ѵ���
bool UserManager::isUsernameTaken(const std::string& uname) const {
    // ���û��б��в���ƥ����û���
    auto it = std::find_if(users.begin(), users.end(),
        [&](const User* u) { return u->getUsername() == uname; });
    return it != users.end(); // ���ڷ���true
}

// ע�����û�
bool UserManager::registerUser(const std::string& uname, const std::string& pwd, const std::string& type) {
    if (isUsernameTaken(uname)) {
        return false; // �û����Ѵ��ڣ�ע��ʧ��
    }

    User* newUser = nullptr;
    if (type == "Consumer") { // �����������û�
        newUser = new Consumer(uname, pwd); // ���Ĭ��0.0
    }
    else if (type == "Merchant") { // �����̼��û�
        newUser = new Merchant(uname, pwd); // ���Ĭ��0.0
    }
    else {
        return false; // ��֧�ֵ��û�����
    }

    users.push_back(newUser);
    saveUsersToFile(); // ע��ɹ����������浽�ļ�
    return true;
}

// �û���¼
User* UserManager::loginUser(const std::string& uname, const std::string& pwd) {
    // �����û��б���֤�û���������
    for (User* user : users) {
        if (user->getUsername() == uname && user->checkPassword(pwd)) {
            return user; // ��¼�ɹ��������û�����
        }
    }
    return nullptr; // �û�����������󣬵�¼ʧ��
}

// �����û��������û�
User* UserManager::findUser(const std::string& uname) {
    auto it = std::find_if(users.begin(), users.end(),
        [&](const User* u) { return u->getUsername() == uname; });
    if (it != users.end()) {
        return *it; // �����ҵ����û�ָ��
    }
    return nullptr; // δ�ҵ��û�
}