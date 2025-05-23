#pragma once
#ifndef SHOPPINGCART_H
#define SHOPPINGCART_H

#include "CartItem.h"
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <iomanip>

class ShoppingCart {
private:
    // Using product ID as key for easy lookup and update
    std::map<std::string, CartItem> items;

public:
    ShoppingCart();

    // Adds a certain quantity of a product. If product already in cart, updates quantity.
    // Returns true if successful, false if not enough available stock.
    bool addItem(Product* product, int quantity);

    // Removes a certain quantity of a product. If quantity to remove >= current, removes item.
    // Returns true if successful.
    bool removeItem(const std::string& productID, int quantityToRemove);

    // Updates the quantity of an existing item.
    // Returns true if successful, false if not enough available stock for new quantity.
    bool updateItemQuantity(const std::string& productID, int newQuantity);

    // Completely removes an item from the cart.
    void clearItem(const std::string& productID);

    void clearCart();

    const std::map<std::string, CartItem>& getItems() const;
    CartItem* getItem(const std::string& productID); // Returns nullptr if not found

    double getTotalValue() const;
    bool isEmpty() const;
    void displayCart() const;
};

#endif // SHOPPINGCART_H