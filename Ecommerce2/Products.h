#pragma once
#ifndef PRODUCTS_H
#define PRODUCTS_H

#include <string>
#include <iostream>
#include <fstream> // For ostream in serialize

class Product {
protected:
    std::string productID;
    std::string name;
    std::string description;
    double originalPrice;
    double currentSalePrice;
    int totalStock;        // Renamed from 'stock' to 'totalStock' for clarity
    int reservedStock;     // New: stock reserved for pending orders
    std::string ownerMerchantUsername;
    std::string productType;   // e.g., "Book", "Food", "Electronics", "SciFiBook"

public:
    Product(std::string id, std::string n, std::string desc, double origP, double saleP, int initialStock, std::string owner, std::string type);
    virtual ~Product() = default;

    virtual double getPrice() const; // This is effectively getCurrentSalePrice

    std::string getID() const;
    std::string getName() const;
    std::string getDescription() const;
    double getOriginalPrice() const;
    double getCurrentSalePrice() const;
    int getTotalStock() const;      // Gets total physical stock
    int getReservedStock() const;   // Gets stock reserved for orders
    int getAvailableStock() const;  // Gets totalStock - reservedStock
    std::string getOwnerMerchantUsername() const;
    std::string getProductType() const;

    void setOriginalPrice(double price);
    void setCurrentSalePrice(double price);
    void setTotalStock(int newTotalStock); // Replaces setStock
    void setDescription(const std::string& desc);
    void setName(const std::string& newName);

    // Stock reservation methods for orders
    bool reserveStock(int quantity);   // Returns true if successful
    bool releaseReservedStock(int quantity); // Returns true if successful
    bool confirmSaleFromReserved(int quantity); // Decrements totalStock and reservedStock

    virtual void displayDetails() const;
    virtual void serialize(std::ostream& os) const;
};

class Book : public Product {
public:
    Book(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner);
    void displayDetails() const override;
    void serialize(std::ostream& os) const override;
    // No override for getPrice needed if it just returns currentSalePrice
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

#endif // PRODUCTS_H