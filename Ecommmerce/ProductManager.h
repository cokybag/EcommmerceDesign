#ifndef PRODUCTMANAGER_H
#define PRODUCTMANAGER_H

#include "Products.h"
#include <vector>
#include <string>
#include <algorithm> // For std::sort, std::remove_if
#include <map> // For product types

class ProductManager {
private:
    std::vector<Product*> products;
    const std::string filename = "products.txt";
    int nextProductID; // To generate unique IDs

    void loadProductsFromFile();
    void saveProductsToFile() const;
    std::string generateNewProductID();


public:
    ProductManager();
    ~ProductManager();

    bool addProduct(const std::string& name, const std::string& description, double originalPrice,
        int stock, const std::string& ownerMerchantUsername, const std::string& productType);

    Product* findProductByID(const std::string& id);
    std::vector<Product*> getAllProducts() const;
    std::vector<Product*> getProductsByMerchant(const std::string& merchantUsername) const;
    std::vector<Product*> searchProducts(const std::string& searchTerm, const std::string& searchBy) const; // searchBy: "name", "type", "merchant"

    bool removeProduct(const std::string& productID, const std::string& merchantUsername); // Merchant can only remove their own
    void updateProduct(Product* product); // Generic update, then save

    void persistChanges(); // Explicitly save to file

    // For requirement: merchants can add product types implicitly by adding products
    // No explicit "add product type" function needed for products themselves,
    // but we might want a way to list available types for searching.
    std::vector<std::string> getAvailableProductTypes() const;
};

#endif // PRODUCTMANAGER_H