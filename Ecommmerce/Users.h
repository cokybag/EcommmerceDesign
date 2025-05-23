#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream> // For potential debugging/display

class User {
protected:
    std::string username;
    std::string password; // In a real system, this would be hashed
    double balance;
    std::string accountType; // "Consumer" or "Merchant"

public:
    User(std::string uname, std::string pwd, double bal, std::string type);
    virtual ~User() = default; // Important for base class with virtual functions

    // Pure virtual function
    virtual std::string getUserType() const = 0;

    // Common functionalities
    std::string getUsername() const;
    bool checkPassword(const std::string& pwd) const;
    void setPassword(const std::string& newPwd); // Consider security implications
    double getBalance() const;
    void deposit(double amount);
    bool withdraw(double amount); // Returns true if successful

    virtual void displayInfo() const; // For debugging or general info

    // For file storage
    virtual void serialize(std::ostream& os) const;
    // Static factory method for deserialization might be useful but can be complex
    // For simplicity, we'll handle deserialization in UserManager
};

class Consumer : public User {
public:
    Consumer(std::string uname, std::string pwd, double bal = 0.0);
    std::string getUserType() const override;
    void displayInfo() const override;
    void serialize(std::ostream& os) const override;
};

class Merchant : public User {
public:
    Merchant(std::string uname, std::string pwd, double bal = 0.0);
    std::string getUserType() const override;
    void displayInfo() const override;
    void serialize(std::ostream& os) const override;
    // Merchants might have specific methods later, e.g., listTheirProducts
};

#endif // USER_H