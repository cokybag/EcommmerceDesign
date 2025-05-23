#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>
#include <fstream>
#include <iostream> // For potential debugging/display

class Product {
protected:
    std::string productID;
    std::string name;
    std::string description;
    double originalPrice;
    double currentSalePrice; // This is the "…Ã∆∑’€ø€º€"
    int stock;
    std::string ownerMerchantUsername;
    std::string productType; // e.g., "Book", "Food", "Clothing"

public:
    Product(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner, std::string type);
    virtual ~Product() = default;

    // Virtual function for getting the price
    virtual double getPrice() const; // Returns currentSalePrice

    // Getters
    std::string getID() const;
    std::string getName() const;
    std::string getDescription() const;
    double getOriginalPrice() const;
    double getCurrentSalePrice() const;
    int getStock() const;
    std::string getOwnerMerchantUsername() const;
    std::string getProductType() const;

    // Setters (primarily for merchants)
    void setOriginalPrice(double price);
    void setCurrentSalePrice(double price); // For discount management
    void setStock(int newStock);
    void setDescription(const std::string& desc);
    void setName(const std::string& newName);


    virtual void displayDetails() const;

    // For file storage
    virtual void serialize(std::ostream& os) const;
    // Deserialization will be handled in ProductManager
};

// Example subclasses
class Book : public Product {
public:
    Book(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner);
    // getPrice() can be inherited if logic is same
    void displayDetails() const override;
    void serialize(std::ostream& os) const override;
};

class Food : public Product {
public:
    Food(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner);
    void displayDetails() const override;
    void serialize(std::ostream& os) const override;
};

class Clothing : public Product {
public:
    Clothing(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner);
    void displayDetails() const override;
    void serialize(std::ostream& os) const override;
};


#endif // PRODUCT_H