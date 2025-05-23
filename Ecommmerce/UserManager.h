#ifndef USERMANAGER_H
#define USERMANAGER_H

#include "Users.h"
#include <vector>
#include <string>
#include <algorithm> // For std::find_if

class UserManager {
private:
    std::vector<User*> users;
    const std::string filename = "users.txt";

    void loadUsersFromFile();
    void saveUsersToFile() const;

public:
    UserManager();
    ~UserManager();

    bool registerUser(const std::string& uname, const std::string& pwd, const std::string& type);
    User* loginUser(const std::string& uname, const std::string& pwd);
    bool isUsernameTaken(const std::string& uname) const;
    User* findUser(const std::string& uname); // Helper to find user
    void persistChanges(); // Explicitly save to file
};

#endif // USERMANAGER_H