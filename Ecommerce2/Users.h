#pragma once
#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

// Forward declaration for ShoppingCart
class ShoppingCart;

class User {
protected:
    std::string username;
    std::string password;
    double balance;
    std::string accountType;

public:
    User(std::string uname, std::string pwd, double bal, std::string type);
    virtual ~User() = default;

    virtual std::string getUserType() const = 0;

    std::string getUsername() const;
    bool checkPassword(const std::string& pwd) const;
    void setPassword(const std::string& newPwd);
    double getBalance() const;
    void deposit(double amount);
    bool withdraw(double amount);

    virtual void displayInfo() const;

    virtual void serialize(std::ostream& os) const;
};

class Consumer : public User {
private:
    ShoppingCart* cart; // Pointer to the shopping cart

public:
    Consumer(std::string uname, std::string pwd, double bal = 0.0);
    ~Consumer(); // Destructor to delete the cart

    std::string getUserType() const override;
    void displayInfo() const override;
    void serialize(std::ostream& os) const override;

    ShoppingCart* getShoppingCart(); // Getter for the cart
};

class Merchant : public User {
public:
    Merchant(std::string uname, std::string pwd, double bal = 0.0);
    std::string getUserType() const override;
    void displayInfo() const override;
    void serialize(std::ostream& os) const override;
};

#endif // USER_H