#include "Users.h"
#include "ShoppingCart.h" // Include the ShoppingCart header
#include <sstream> 
#include <iomanip> 

// User Base Class (no changes from Task 1 version)
User::User(std::string uname, std::string pwd, double bal, std::string type)
    : username(uname), password(pwd), balance(bal), accountType(type) {
}

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
    : User(uname, pwd, bal, "Consumer") {
    cart = new ShoppingCart(); // Create a new shopping cart for the consumer
}

Consumer::~Consumer() {
    delete cart; // Clean up the shopping cart
    cart = nullptr;
}

std::string Consumer::getUserType() const { return "Consumer"; }

void Consumer::displayInfo() const {
    User::displayInfo();
    // Optionally display cart summary if needed here
    // if (cart && !cart->isEmpty()) {
    //    std::cout << "Shopping Cart: " << cart->getItems().size() << " unique item(s)." << std::endl;
    // }
}
void Consumer::serialize(std::ostream& os) const {
    User::serialize(os);
    // Shopping cart data is not directly serialized with the user.
    // It's a runtime object. If persistence is needed for carts
    // across sessions (not typical for simple console apps),
    // a separate CartManager or saving cart items to a file would be required.
    // For this assignment, cart is session-based.
}

ShoppingCart* Consumer::getShoppingCart() {
    return cart;
}


// --- Merchant ---
Merchant::Merchant(std::string uname, std::string pwd, double bal)
    : User(uname, pwd, bal, "Merchant") {
}

std::string Merchant::getUserType() const { return "Merchant"; }

void Merchant::displayInfo() const {
    User::displayInfo();
}
void Merchant::serialize(std::ostream& os) const {
    User::serialize(os);
}