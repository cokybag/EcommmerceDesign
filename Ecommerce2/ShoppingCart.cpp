#include "ShoppingCart.h"
#include "Products.h" // For Product::getAvailableStock()
#include <iostream>
#include <iomanip> // For std::fixed, std::setprecision

ShoppingCart::ShoppingCart() {
    // Constructor can be empty if map initializes itself
}

bool ShoppingCart::addItem(Product* product, int quantity) {
    if (!product || quantity <= 0) {
        std::cout << "Error: Invalid product or quantity." << std::endl;
        return false;
    }

    if (product->getAvailableStock() < quantity) {
        std::cout << "Error: Not enough available stock for " << product->getName()
            << ". Available: " << product->getAvailableStock() << std::endl;
        return false;
    }

    std::string productID = product->getID();
    auto it = items.find(productID);
    if (it != items.end()) { // Product already in cart, update quantity
        if (product->getAvailableStock() < (it->second.quantity + quantity)) {
            std::cout << "Error: Not enough available stock to add " << quantity
                << " more of " << product->getName()
                << ". Current in cart: " << it->second.quantity
                << ", Available on platform: " << product->getAvailableStock() << std::endl;
            return false;
        }
        it->second.quantity += quantity;
    }
    else { // New product for the cart
        items[productID] = CartItem(product, quantity);
    }
    std::cout << quantity << " unit(s) of '" << product->getName() << "' added to cart." << std::endl;
    return true;
}

bool ShoppingCart::removeItem(const std::string& productID, int quantityToRemove) {
    if (quantityToRemove <= 0) {
        std::cout << "Error: Quantity to remove must be positive." << std::endl;
        return false;
    }

    auto it = items.find(productID);
    if (it != items.end()) {
        if (it->second.quantity > quantityToRemove) {
            it->second.quantity -= quantityToRemove;
            std::cout << quantityToRemove << " unit(s) of '" << it->second.product->getName() << "' removed from cart." << std::endl;
        }
        else {
            std::cout << "All units of '" << it->second.product->getName() << "' removed from cart." << std::endl;
            items.erase(it);
        }
        return true;
    }
    else {
        std::cout << "Error: Product ID " << productID << " not found in cart." << std::endl;
        return false;
    }
}

bool ShoppingCart::updateItemQuantity(const std::string& productID, int newQuantity) {
    if (newQuantity < 0) {
        std::cout << "Error: New quantity cannot be negative." << std::endl;
        return false;
    }

    auto it = items.find(productID);
    if (it != items.end()) {
        if (newQuantity == 0) {
            std::cout << "Quantity set to 0. Item '" << it->second.product->getName() << "' removed from cart." << std::endl;
            items.erase(it);
            return true;
        }
        // Check available stock for the *change* in quantity or total new quantity
        // Simpler: check against total new quantity
        if (it->second.product->getAvailableStock() < newQuantity) {
            std::cout << "Error: Not enough available stock for " << it->second.product->getName()
                << " to set quantity to " << newQuantity
                << ". Available: " << it->second.product->getAvailableStock() << std::endl;
            return false;
        }
        it->second.quantity = newQuantity;
        std::cout << "Quantity for '" << it->second.product->getName() << "' updated to " << newQuantity << "." << std::endl;
        return true;
    }
    else {
        std::cout << "Error: Product ID " << productID << " not found in cart to update." << std::endl;
        return false;
    }
}

void ShoppingCart::clearItem(const std::string& productID) {
    auto it = items.find(productID);
    if (it != items.end()) {
        std::cout << "Item '" << it->second.product->getName() << "' completely removed from cart." << std::endl;
        items.erase(it);
    }
    else {
        std::cout << "Product ID " << productID << " not found in cart to clear." << std::endl;
    }
}


void ShoppingCart::clearCart() {
    if (!items.empty()) {
        items.clear();
        std::cout << "Shopping cart has been cleared." << std::endl;
    }
    else {
        std::cout << "Shopping cart is already empty." << std::endl;
    }
}

const std::map<std::string, CartItem>& ShoppingCart::getItems() const {
    return items;
}

CartItem* ShoppingCart::getItem(const std::string& productID) {
    auto it = items.find(productID);
    if (it != items.end()) {
        return &(it->second);
    }
    return nullptr;
}


double ShoppingCart::getTotalValue() const {
    double total = 0.0;
    for (const auto& pair : items) {
        total += pair.second.getSubtotal();
    }
    return total;
}

bool ShoppingCart::isEmpty() const {
    return items.empty();
}

void ShoppingCart::displayCart() const {
    if (items.empty()) {
        std::cout << "Your shopping cart is empty." << std::endl;
        return;
    }
    std::cout << "\n--- Your Shopping Cart ---" << std::endl;
    for (const auto& pair : items) {
        pair.second.display();
    }
    std::cout << "--------------------------" << std::endl;
    std::cout << "Total Cart Value: $" << std::fixed << std::setprecision(2) << getTotalValue() << std::endl;
    std::cout << "--------------------------" << std::endl;
}