#pragma once
#ifndef CARTITEM_H
#define CARTITEM_H

#include "Products.h" // Assuming Product class definition is here
#include <string>
#include <iomanip>

// Forward declaration if Product.h is heavy
// class Product; 

struct CartItem {
    Product* product;    // Pointer to the actual product
    int quantity;        // Quantity in cart

    CartItem(Product* p, int qty) : product(p), quantity(qty) {}

    // Default constructor for map usage if needed
    CartItem() : product(nullptr), quantity(0) {}

    double getSubtotal() const {
        if (product) {
            return product->getCurrentSalePrice() * quantity;
        }
        return 0.0;
    }

    void display() const {
        if (product) {
            std::cout << "  - Product: " << product->getName()
                << " (ID: " << product->getID() << ")"
                << ", Quantity: " << quantity
                << ", Price: $" << std::fixed << std::setprecision(2) << product->getCurrentSalePrice()
                << ", Subtotal: $" << getSubtotal() << std::endl;
        }
    }
};

#endif // CARTITEM_H