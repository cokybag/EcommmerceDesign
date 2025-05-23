#include "UserManager.h"
#include <fstream>
#include <sstream> // For parsing lines
#include <iostream> // For error messages

// Helper to split string by delimiter
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
    loadUsersFromFile();
}

UserManager::~UserManager() {
    // saveUsersToFile(); // Save on exit, or manage explicitly
    for (User* user : users) {
        delete user;
    }
    users.clear();
}

void UserManager::loadUsersFromFile() {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        // std::cerr << "Warning: Could not open user file: " << filename << ". Starting fresh." << std::endl;
        // This is fine if the file doesn't exist on first run
        return;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        std::vector<std::string> tokens = split(line, ',');
        if (tokens.size() == 4) {
            try {
                std::string uname = tokens[0];
                std::string pwd = tokens[1];
                double bal = std::stod(tokens[2]);
                std::string type = tokens[3];

                if (isUsernameTaken(uname)) { // Should not happen if file is consistent
                    // std::cerr << "Warning: Duplicate username '" << uname << "' found in file. Skipping." << std::endl;
                    continue;
                }

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
    inFile.close();
}

void UserManager::saveUsersToFile() const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open user file for writing: " << filename << std::endl;
        return;
    }
    for (const User* user : users) {
        user->serialize(outFile);
        outFile << std::endl;
    }
    outFile.close();
}

void UserManager::persistChanges() {
    saveUsersToFile();
}

bool UserManager::isUsernameTaken(const std::string& uname) const {
    auto it = std::find_if(users.begin(), users.end(),
        [&](const User* u) { return u->getUsername() == uname; });
    return it != users.end();
}

bool UserManager::registerUser(const std::string& uname, const std::string& pwd, const std::string& type) {
    if (isUsernameTaken(uname)) {
        return false; // Username already exists
    }

    User* newUser = nullptr;
    if (type == "Consumer") {
        newUser = new Consumer(uname, pwd);
    }
    else if (type == "Merchant") {
        newUser = new Merchant(uname, pwd);
    }
    else {
        return false; // Invalid type
    }

    users.push_back(newUser);
    saveUsersToFile(); // Persist immediately after registration
    return true;
}

User* UserManager::loginUser(const std::string& uname, const std::string& pwd) {
    for (User* user : users) {
        if (user->getUsername() == uname && user->checkPassword(pwd)) {
            return user;
        }
    }
    return nullptr; // Login failed
}

User* UserManager::findUser(const std::string& uname) {
    auto it = std::find_if(users.begin(), users.end(),
        [&](const User* u) { return u->getUsername() == uname; });
    if (it != users.end()) {
        return *it;
    }
    return nullptr;
}