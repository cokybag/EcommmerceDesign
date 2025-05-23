#include "Products.h"
#include <iomanip> // For std::fixed and std::setprecision
#include <sstream> // For string stream
#include <algorithm> // For std::replace

// Product Implementation
Product::Product(std::string id, std::string n, std::string desc, double origP, double saleP, int initialStock, std::string owner, std::string type)
    : productID(id), name(n), description(desc), originalPrice(origP), currentSalePrice(saleP),
    totalStock(initialStock), reservedStock(0), // Initialize reservedStock
    ownerMerchantUsername(owner), productType(type) {
}

double Product::getPrice() const {
    return currentSalePrice;
}

std::string Product::getID() const { return productID; }
std::string Product::getName() const { return name; }
std::string Product::getDescription() const { return description; }
double Product::getOriginalPrice() const { return originalPrice; }
double Product::getCurrentSalePrice() const { return currentSalePrice; }
int Product::getTotalStock() const { return totalStock; }
int Product::getReservedStock() const { return reservedStock; }

int Product::getAvailableStock() const {
    return totalStock - reservedStock;
}

std::string Product::getOwnerMerchantUsername() const { return ownerMerchantUsername; }
std::string Product::getProductType() const { return productType; }


void Product::setOriginalPrice(double price) {
    if (price >= 0) originalPrice = price;
    if (currentSalePrice > originalPrice) currentSalePrice = originalPrice;
}

void Product::setCurrentSalePrice(double price) {
    if (price >= 0 && price <= originalPrice) {
        currentSalePrice = price;
    }
    else if (price > originalPrice) {
        currentSalePrice = originalPrice;
    }
    else if (price < 0) {
        // Optionally handle: currentSalePrice = 0; or log error
        // For now, just don't set if negative and not 0
    }
}

void Product::setTotalStock(int newTotalStock) { // Renamed from setStock
    if (newTotalStock >= 0) {
        // Ensure new total stock isn't less than already reserved stock
        if (newTotalStock < reservedStock) {
            std::cerr << "Warning: Cannot set total stock (" << newTotalStock
                << ") less than reserved stock (" << reservedStock
                << ") for product " << productID << ". Adjusting to reserved stock." << std::endl;
            totalStock = reservedStock;
        }
        else {
            totalStock = newTotalStock;
        }
    }
}

void Product::setDescription(const std::string& desc) {
    description = desc;
}
void Product::setName(const std::string& newName) {
    name = newName;
}

// Stock reservation methods
bool Product::reserveStock(int quantity) {
    if (quantity <= 0) return false; // Cannot reserve non-positive quantity
    if (getAvailableStock() >= quantity) {
        reservedStock += quantity;
        return true;
    }
    return false; // Not enough available stock
}

bool Product::releaseReservedStock(int quantity) {
    if (quantity <= 0) return false;
    if (reservedStock >= quantity) {
        reservedStock -= quantity;
        return true;
    }
    // Attempting to release more than reserved. Log error or cap at reservedStock.
    std::cerr << "Warning: Attempting to release " << quantity
        << " but only " << reservedStock << " reserved for product " << productID
        << ". Releasing all reserved." << std::endl;
    reservedStock = 0;
    return false; // Indicate partial success or an issue
}

bool Product::confirmSaleFromReserved(int quantity) {
    if (quantity <= 0) return false;
    if (reservedStock >= quantity && totalStock >= quantity) {
        totalStock -= quantity;
        reservedStock -= quantity;
        // Ensure reservedStock doesn't go negative if quantity > reservedStock (should be caught by caller logic)
        if (reservedStock < 0) reservedStock = 0;
        return true;
    }
    return false; // Not enough reserved or total stock (should not happen if reserved properly)
}


void Product::displayDetails() const {
    std::cout << "-------------------------------------\n"
        << "ID: " << productID << "\n"
        << "Name: " << name << "\n"
        << "Type: " << productType << "\n"
        << "Description: " << description << "\n"
        << "Original Price: $" << std::fixed << std::setprecision(2) << originalPrice << "\n"
        << "Current Price: $" << std::fixed << std::setprecision(2) << currentSalePrice << "\n"
        << "Total Stock: " << totalStock << "\n"
        << "Reserved Stock: " << reservedStock << "\n"
        << "Available Stock: " << getAvailableStock() << "\n" // Display available stock
        << "Sold by: " << ownerMerchantUsername << "\n"
        << "-------------------------------------" << std::endl;
}

void Product::serialize(std::ostream& os) const {
    std::string safeName = name;
    std::replace(safeName.begin(), safeName.end(), ',', ';');
    std::string safeDesc = description;
    std::replace(safeDesc.begin(), safeDesc.end(), ',', ';');

    os << productID << ","
        << safeName << ","
        << safeDesc << ","
        << originalPrice << ","
        << currentSalePrice << ","
        << totalStock << "," // Save totalStock
        // reservedStock is runtime state, not typically saved directly to product file.
        // It's derived when orders are loaded. If you must save it, add another field.
        // For now, we assume reservedStock is re-calculated on load based on pending orders.
        // If orders are not persistent or re-reservation is complex, you might need to save it.
        // Let's keep it simple for now and not save reservedStock in products.txt
        // It will be reconstructed by OrderManager loading orders.
        << ownerMerchantUsername << ","
        << productType;
    // If you decide to save reservedStock:
    // os << "," << reservedStock; // This would change the token count in ProductManager::loadProductsFromFile
}

// --- Book ---
Book::Book(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner)
    : Product(id, n, desc, origP, saleP, s, owner, "Book") {
}

void Book::displayDetails() const {
    Product::displayDetails();
}
void Book::serialize(std::ostream& os) const {
    Product::serialize(os);
}

// --- Food ---
Food::Food(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner)
    : Product(id, n, desc, origP, saleP, s, owner, "Food") {
}

void Food::displayDetails() const {
    Product::displayDetails();
}
void Food::serialize(std::ostream& os) const {
    Product::serialize(os);
}

// --- Clothing ---
Clothing::Clothing(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner)
    : Product(id, n, desc, origP, saleP, s, owner, "Clothing") {
}

void Clothing::displayDetails() const {
    Product::displayDetails();
}
void Clothing::serialize(std::ostream& os) const {
    Product::serialize(os);
}