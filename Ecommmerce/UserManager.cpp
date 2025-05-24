#include "UserManager.h"
#include <fstream>
#include <sstream> // ���ڽ���������
#include <iostream> // ���ڴ�����Ϣ

// �������������ָ����ָ��ַ���
std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}


UserManager::UserManager() {
    loadUsersFromFile(); // ���ļ������û�����
}

UserManager::~UserManager() {
    // saveUsersToFile(); // �˳�ʱ�������ݣ�����ʽ�������߼�
    for (User* user : users) {
        delete user; // �ͷ��û������ڴ�
    }
    users.clear(); // ����û��б�
}

void UserManager::loadUsersFromFile() {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        // std::cerr << "Warning: Could not open user file: " << filename << ". Starting fresh." << std::endl;
        // �״�����ʱ�ļ������������������
        return;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue; // ��������
        std::vector<std::string> tokens = split(line, ','); // �����ŷָ�������
        if (tokens.size() == 4) { // ��֤�ֶ��������û��������롢�����ͣ�
            try {
                std::string uname = tokens[0];
                std::string pwd = tokens[1];
                double bal = std::stod(tokens[2]);
                std::string type = tokens[3];

                if (isUsernameTaken(uname)) { // ����û����Ƿ��ظ����ļ�����Ӧ��֤Ψһ�ԣ�
                    // std::cerr << "Warning: Duplicate username '" << uname << "' found in file. Skipping." << std::endl;
                    continue;
                }

                // �����û����ʹ�����Ӧ����
                if (type == "Consumer") {
                    users.push_back(new Consumer(uname, pwd, bal));
                }
                else if (type == "Merchant") {
                    users.push_back(new Merchant(uname, pwd, bal));
                }
                else {
                    // std::cerr << "Warning: Unknown user type '" << type << "' for user '" << uname << "'. Skipping." << std::endl;
                }
            }
            catch (const std::invalid_argument& ia) {
                // std::cerr << "Error parsing balance for a user in file: " << ia.what() << " Line: " << line << std::endl;
            }
            catch (const std::out_of_range& oor) {
                // std::cerr << "Balance value out of range for a user in file. Line: " << line << std::endl;
            }
        }
        else {
            // std::cerr << "Warning: Malformed line in user file: " << line << ". Skipping." << std::endl;
        }
    }
    inFile.close(); // �ر��ļ���
}

void UserManager::saveUsersToFile() const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open user file for writing: " << filename << std::endl;
        return;
    }
    for (const User* user : users) {
        user->serialize(outFile); // �����û���������л�����
        outFile << std::endl; // ����
    }
    outFile.close(); // �ر��ļ���
}

void UserManager::persistChanges() {
    saveUsersToFile(); // �־û��û����ݵ��ļ�
}

// ����û����Ƿ��ѱ�ʹ��
bool UserManager::isUsernameTaken(const std::string& uname) const {
    auto it = std::find_if(users.begin(), users.end(),
        [&](const User* u) { return u->getUsername() == uname; });
    return it != users.end(); // �ҵ��򷵻�true
}

// ע�����û�
bool UserManager::registerUser(const std::string& uname, const std::string& pwd, const std::string& type) {
    if (isUsernameTaken(uname)) {
        return false; // �û����Ѵ���
    }

    User* newUser = nullptr;
    // �������ʹ�����Ӧ�û�����
    if (type == "Consumer") {
        newUser = new Consumer(uname, pwd);
    }
    else if (type == "Merchant") {
        newUser = new Merchant(uname, pwd);
    }
    else {
        return false; // ��Ч�û�����
    }

    users.push_back(newUser); // ��ӵ��û��б�
    saveUsersToFile(); // �������浽�ļ�
    return true;
}

// �û���¼��֤
User* UserManager::loginUser(const std::string& uname, const std::string& pwd) {
    for (User* user : users) {
        if (user->getUsername() == uname && user->checkPassword(pwd)) {
            return user; // ��֤�ɹ��������û�����
        }
    }
    return nullptr; // ��¼ʧ��
}

// �����û��������û�
User* UserManager::findUser(const std::string& uname) {
    auto it = std::find_if(users.begin(), users.end(),
        [&](const User* u) { return u->getUsername() == uname; });
    if (it != users.end()) {
        return *it; // �ҵ��򷵻��û�ָ��
    }
    return nullptr; // δ�ҵ�
}