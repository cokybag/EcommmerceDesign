#include "Products.h"
#include <iomanip> // For std::fixed and std::setprecision
#include <sstream> // For string stream
#include <algorithm>

Product::Product(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner, std::string type)
    : productID(id), name(n), description(desc), originalPrice(origP), currentSalePrice(saleP), stock(s), ownerMerchantUsername(owner), productType(type) {}

double Product::getPrice() const {
    return currentSalePrice;
}

std::string Product::getID() const { return productID; }
std::string Product::getName() const { return name; }
std::string Product::getDescription() const { return description; }
double Product::getOriginalPrice() const { return originalPrice; }
double Product::getCurrentSalePrice() const { return currentSalePrice; }
int Product::getStock() const { return stock; }
std::string Product::getOwnerMerchantUsername() const { return ownerMerchantUsername; }
std::string Product::getProductType() const { return productType; }


void Product::setOriginalPrice(double price) {
    if (price >= 0) originalPrice = price;
    // Potentially adjust currentSalePrice if it becomes higher than original
    if (currentSalePrice > originalPrice) currentSalePrice = originalPrice;
}

void Product::setCurrentSalePrice(double price) {
    if (price >= 0 && price <= originalPrice) { // Discounted price can't be > original
        currentSalePrice = price;
    }
    else if (price > originalPrice) {
        currentSalePrice = originalPrice; // Cap at original price
    }
}

void Product::setStock(int newStock) {
    if (newStock >= 0) stock = newStock;
}

void Product::setDescription(const std::string& desc) {
    description = desc;
}
void Product::setName(const std::string& newName) {
    name = newName;
}


void Product::displayDetails() const {
    std::cout << "-------------------------------------\n"
        << "ID: " << productID << "\n"
        << "Name: " << name << "\n"
        << "Type: " << productType << "\n"
        << "Description: " << description << "\n"
        << "Original Price: $" << std::fixed << std::setprecision(2) << originalPrice << "\n"
        << "Current Price: $" << std::fixed << std::setprecision(2) << currentSalePrice << "\n"
        << "Stock: " << stock << "\n"
        << "Sold by: " << ownerMerchantUsername << "\n"
        << "-------------------------------------" << std::endl;
}

void Product::serialize(std::ostream& os) const {
    // Replace commas in name and description to not break CSV
    std::string safeName = name;
    std::replace(safeName.begin(), safeName.end(), ',', ';');
    std::string safeDesc = description;
    std::replace(safeDesc.begin(), safeDesc.end(), ',', ';');

    os << productID << ","
        << safeName << ","
        << safeDesc << ","
        << originalPrice << ","
        << currentSalePrice << ","
        << stock << ","
        << ownerMerchantUsername << ","
        << productType;
}

// --- Book ---
Book::Book(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner)
    : Product(id, n, desc, origP, saleP, s, owner, "Book") {}

void Book::displayDetails() const {
    // std::cout << "--- Book Details ---" << std::endl;
    Product::displayDetails();
    // Add book-specific details if any, e.g., Author, ISBN
}
void Book::serialize(std::ostream& os) const {
    Product::serialize(os); // Base class writes type
}

// --- Food ---
Food::Food(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner)
    : Product(id, n, desc, origP, saleP, s, owner, "Food") {}

void Food::displayDetails() const {
    // std::cout << "--- Food Details ---" << std::endl;
    Product::displayDetails();
    // Add food-specific details if any, e.g., Expiry Date
}
void Food::serialize(std::ostream& os) const {
    Product::serialize(os);
}

// --- Clothing ---
Clothing::Clothing(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner)
    : Product(id, n, desc, origP, saleP, s, owner, "Clothing") {}

void Clothing::displayDetails() const {
    // std::cout << "--- Clothing Details ---" << std::endl;
    Product::displayDetails();
    // Add clothing-specific details if any, e.g., Size, Color
}
void Clothing::serialize(std::ostream& os) const {
    Product::serialize(os);
}