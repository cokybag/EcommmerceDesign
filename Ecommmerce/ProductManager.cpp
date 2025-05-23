#include "ProductManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm> // For std::max, std::sort, std::unique, std::transform
#include <set> // For unique product types

// Assuming split is globally available or defined in a common util.h
// If not, ensure it's defined (e.g., from UserManager.cpp or a utility file)
extern std::vector<std::string> split(const std::string& s, char delimiter);

ProductManager::ProductManager() : nextProductID(1) {
    loadProductsFromFile();
    if (!products.empty()) {
        for (const auto* p : products) {
            try {
                if (p->getID().length() > 1 && p->getID()[0] == 'P') {
                    int idNum = std::stoi(p->getID().substr(1));
                    nextProductID = std::max(nextProductID, idNum + 1);
                }
            }
            catch (const std::exception& e) {
                // Muted warning: std::cerr << "Warning: Could not parse product ID " << p->getID() << std::endl;
            }
        }
    }
}

ProductManager::~ProductManager() {
    for (Product* product : products) {
        delete product;
    }
    products.clear();
}

std::string ProductManager::generateNewProductID() {
    return "P" + std::to_string(nextProductID++);
}

void ProductManager::loadProductsFromFile() {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        std::vector<std::string> tokens = split(line, ',');
        if (tokens.size() == 8) { // ID, Name, Desc, OrigP, SaleP, Stock, Owner, Type
            try {
                std::string id = tokens[0];
                std::string name = tokens[1];
                std::replace(name.begin(), name.end(), ';', ',');
                std::string desc = tokens[2];
                std::replace(desc.begin(), desc.end(), ';', ',');
                double origP = std::stod(tokens[3]);
                double saleP = std::stod(tokens[4]);
                int stock = std::stoi(tokens[5]);
                std::string owner = tokens[6];
                std::string type = tokens[7];

                Product* newProd = nullptr;
                if (type == "Book") {
                    newProd = new Book(id, name, desc, origP, saleP, stock, owner);
                }
                else if (type == "Food") {
                    newProd = new Food(id, name, desc, origP, saleP, stock, owner);
                }
                else if (type == "Clothing") {
                    newProd = new Clothing(id, name, desc, origP, saleP, stock, owner);
                }
                else {
                    std::cerr << "Warning: Unknown product type '" << type << "' for product '" << name << "' in file. Skipping." << std::endl;
                    continue;
                }
                if (newProd) products.push_back(newProd);

            }
            catch (const std::invalid_argument& ia) {
                std::cerr << "Error parsing numeric value for a product in file: " << ia.what() << " Line: " << line << std::endl;
            }
            catch (const std::out_of_range& oor) {
                std::cerr << "Numeric value out of range for a product in file. Line: " << line << std::endl;
            }
        }
        else {
            std::cerr << "Warning: Malformed line in product file: " << line << ". Skipping." << std::endl;
        }
    }
    inFile.close();
}

void ProductManager::saveProductsToFile() const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open product file for writing: " << filename << std::endl;
        return;
    }
    for (const Product* product : products) {
        product->serialize(outFile);
        outFile << std::endl;
    }
    outFile.close();
}

void ProductManager::persistChanges() {
    saveProductsToFile();
}

bool ProductManager::addProduct(const std::string& name, const std::string& description, double originalPrice,
    int stock, const std::string& ownerMerchantUsername, const std::string& productType) {
    std::string newID = generateNewProductID();
    Product* newProd = nullptr;

    if (productType == "Book") {
        newProd = new Book(newID, name, description, originalPrice, originalPrice, stock, ownerMerchantUsername);
    }
    else if (productType == "Food") {
        newProd = new Food(newID, name, description, originalPrice, originalPrice, stock, ownerMerchantUsername);
    }
    else if (productType == "Clothing") {
        newProd = new Clothing(newID, name, description, originalPrice, originalPrice, stock, ownerMerchantUsername);
    }
    else {
        std::cout << "Unsupported product category. Please choose Book, Food, or Clothing." << std::endl;
        nextProductID--;
        return false;
    }

    // Check for active discount for this merchant and category
    double discountPercent = getActiveCategoryDiscount(ownerMerchantUsername, productType);
    if (discountPercent > 0.0) { // Assuming discountPercent is 0 if no discount or invalid
        double newSalePrice = newProd->getOriginalPrice() * (1.0 - (discountPercent / 100.0));
        newProd->setCurrentSalePrice(newSalePrice);
        std::cout << "Info: Product '" << name << "' automatically discounted by " << discountPercent << "% upon adding." << std::endl;
    }

    products.push_back(newProd);
    saveProductsToFile();
    return true;
}

Product* ProductManager::findProductByID(const std::string& id) {
    auto it = std::find_if(products.begin(), products.end(),
        [&](const Product* p) { return p->getID() == id; });
    if (it != products.end()) {
        return *it;
    }
    return nullptr;
}

std::vector<Product*> ProductManager::getAllProducts() const {
    return products;
}

std::vector<Product*> ProductManager::getProductsByMerchant(const std::string& merchantUsername) const {
    std::vector<Product*> result;
    for (Product* p : products) {
        if (p->getOwnerMerchantUsername() == merchantUsername) {
            result.push_back(p);
        }
    }
    return result;
}

std::vector<Product*> ProductManager::searchProducts(const std::string& searchTerm, const std::string& searchBy) const {
    std::vector<Product*> results;
    std::string termLower = searchTerm;
    std::transform(termLower.begin(), termLower.end(), termLower.begin(),
        [](unsigned char c) { return std::tolower(c); });


    for (Product* p : products) {
        bool match = false;
        if (searchBy == "name") {
            std::string nameLower = p->getName();
            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(),
                [](unsigned char c) { return std::tolower(c); });
            if (nameLower.find(termLower) != std::string::npos) {
                match = true;
            }
        }
        else if (searchBy == "type") {
            std::string typeLower = p->getProductType();
            std::transform(typeLower.begin(), typeLower.end(), typeLower.begin(),
                [](unsigned char c) { return std::tolower(c); });
            if (typeLower.find(termLower) != std::string::npos) {
                match = true;
            }
        }
        else if (searchBy == "merchant") {
            std::string merchantLower = p->getOwnerMerchantUsername();
            std::transform(merchantLower.begin(), merchantLower.end(), merchantLower.begin(),
                [](unsigned char c) { return std::tolower(c); });
            if (merchantLower.find(termLower) != std::string::npos) {
                match = true;
            }
        }
        if (match) {
            results.push_back(p);
        }
    }
    return results;
}

bool ProductManager::removeProduct(const std::string& productID, const std::string& merchantUsername) {
    auto it = std::find_if(products.begin(), products.end(),
        [&](const Product* p) { return p->getID() == productID; });

    if (it != products.end()) {
        if ((*it)->getOwnerMerchantUsername() == merchantUsername) {
            delete* it;
            products.erase(it);
            saveProductsToFile();
            return true;
        }
        else {
            std::cout << "Error: You can only remove your own products." << std::endl;
            return false;
        }
    }
    std::cout << "Error: Product ID not found." << std::endl;
    return false;
}

void ProductManager::updateProduct(Product* product) {
    if (product) {
        saveProductsToFile();
    }
}

std::vector<std::string> ProductManager::getAvailableProductTypes() const {
    std::set<std::string> typesSet;
    for (const auto* p : products) {
        typesSet.insert(p->getProductType());
    }
    return std::vector<std::string>(typesSet.begin(), typesSet.end());
}

// New method implementation
void ProductManager::applyCategoryDiscount(const std::string& merchantUsername, const std::string& categoryType, double discountPercentage) {
    if (discountPercentage < 0 || discountPercentage > 100) {
        std::cout << "Error: Invalid discount percentage. Must be between 0 and 100." << std::endl;
        return;
    }

    // Store the active discount rule
    activeCategoryDiscounts[merchantUsername][categoryType] = discountPercentage;

    // Apply to existing products of this merchant in this category
    int count = 0;
    for (Product* p : products) {
        if (p->getOwnerMerchantUsername() == merchantUsername && p->getProductType() == categoryType) {
            double newSalePrice = p->getOriginalPrice() * (1.0 - (discountPercentage / 100.0));
            p->setCurrentSalePrice(newSalePrice);
            count++;
        }
    }
    if (count > 0) {
        saveProductsToFile(); // Persist changes
        std::cout << count << " of your products in category '" << categoryType
            << "' have been discounted by " << discountPercentage << "%." << std::endl;
    }
    else {
        std::cout << "You have no products in category '" << categoryType << "' to apply discount to, but the rule is set for future additions." << std::endl;
    }
}

// New method implementation
double ProductManager::getActiveCategoryDiscount(const std::string& merchantUsername, const std::string& categoryType) const {
    auto merchIt = activeCategoryDiscounts.find(merchantUsername);
    if (merchIt != activeCategoryDiscounts.end()) {
        auto catIt = merchIt->second.find(categoryType);
        if (catIt != merchIt->second.end()) {
            return catIt->second; // Return stored discount percentage
        }
    }
    return 0.0; // No discount active or merchant/category not found
}