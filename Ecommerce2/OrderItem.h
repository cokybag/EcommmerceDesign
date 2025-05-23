#pragma once
#ifndef ORDERITEM_H
#define ORDERITEM_H

#include <string>
#include <iostream>
#include <iomanip>

struct OrderItem {
    std::string productID;
    std::string productName;
    std::string merchantUsername; // To credit the correct merchant
    double priceAtOrder;      // Price when the order was placed
    int quantity;

    OrderItem(std::string pid, std::string pName, std::string mUser, double price, int qty)
        : productID(pid), productName(pName), merchantUsername(mUser), priceAtOrder(price), quantity(qty) {
    }

    // Default constructor for vector resizing or other needs
    OrderItem() : productID(""), productName(""), merchantUsername(""), priceAtOrder(0.0), quantity(0) {}


    double getSubtotal() const {
        return priceAtOrder * quantity;
    }

    void display() const {
        std::cout << "    - Product: " << productName << " (ID: " << productID << ")"
            << ", Qty: " << quantity
            << ", Price Paid: $" << std::fixed << std::setprecision(2) << priceAtOrder
            << ", Subtotal: $" << getSubtotal()
            << " (Sold by: " << merchantUsername << ")" << std::endl;
    }
};

#endif // ORDERITEM_H