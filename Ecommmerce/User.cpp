#include "Users.h"
#include <sstream> // For string stream in serialization (if needed, but simple CSV is fine)
#include <iomanip> // For std::fixed and std::setprecision

User::User(std::string uname, std::string pwd, double bal, std::string type)
    : username(uname), password(pwd), balance(bal), accountType(type) {}

std::string User::getUsername() const { return username; }

bool User::checkPassword(const std::string& pwd) const {
    return password == pwd;
}

void User::setPassword(const std::string& newPwd) {
    password = newPwd;
}

double User::getBalance() const { return balance; }

void User::deposit(double amount) {
    if (amount > 0) {
        balance += amount;
    }
}

bool User::withdraw(double amount) {
    if (amount > 0 && balance >= amount) {
        balance -= amount;
        return true;
    }
    return false;
}

void User::displayInfo() const {
    std::cout << "Username: " << username << "\n"
        << "Type: " << accountType << "\n"
        << "Balance: $" << std::fixed << std::setprecision(2) << balance << std::endl;
}

void User::serialize(std::ostream& os) const {
    os << username << "," << password << "," << balance << "," << accountType;
}

// --- Consumer ---
Consumer::Consumer(std::string uname, std::string pwd, double bal)
    : User(uname, pwd, bal, "Consumer") {}

std::string Consumer::getUserType() const { return "Consumer"; }

void Consumer::displayInfo() const {
    User::displayInfo(); // Call base display
    // Add any consumer-specific info if needed
}
void Consumer::serialize(std::ostream& os) const {
    User::serialize(os); // Base class already writes type
}


// --- Merchant ---
Merchant::Merchant(std::string uname, std::string pwd, double bal)
    : User(uname, pwd, bal, "Merchant") {}

std::string Merchant::getUserType() const { return "Merchant"; }

void Merchant::displayInfo() const {
    User::displayInfo(); // Call base display
    // Add any merchant-specific info if needed
}
void Merchant::serialize(std::ostream& os) const {
    User::serialize(os); // Base class already writes type
}