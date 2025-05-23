#include "ProductManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm> // For std::max, std::sort, std::unique
#include <set> // For unique product types

// Helper function (can be in a utility file or here if only used here)
// Already defined in UserManager.cpp, consider moving to a common utility header
/*
std::vector<std::string> split_prod(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}
*/
// Assuming split is globally available or defined in a common util.h
extern std::vector<std::string> split(const std::string& s, char delimiter); // Declare if in another .cpp

ProductManager::ProductManager() : nextProductID(1) {
    loadProductsFromFile();
    // Determine nextProductID based on loaded products
    if (!products.empty()) {
        for (const auto* p : products) {
            try {
                // Assuming ID is "P<number>"
                if (p->getID().length() > 1 && p->getID()[0] == 'P') {
                    int idNum = std::stoi(p->getID().substr(1));
                    nextProductID = std::max(nextProductID, idNum + 1);
                }
            }
            catch (const std::exception& e) {
                // std::cerr << "Warning: Could not parse product ID " << p->getID() << std::endl;
            }
        }
    }
}

ProductManager::~ProductManager() {
    // saveProductsToFile(); // Save on exit or manage explicitly
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
        // std::cerr << "Warning: Could not open product file: " << filename << ". Starting fresh." << std::endl;
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
                std::replace(name.begin(), name.end(), ';', ','); // Restore commas
                std::string desc = tokens[2];
                std::replace(desc.begin(), desc.end(), ';', ','); // Restore commas
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
                else { // Support for dynamically added types
                    // For simplicity, we create a generic Product or a specific one if we add more classes
                    // Or, if we want to stick to defined classes, we could log an error or skip
                    // For now, let's treat unknown types as generic Product instances if we had one,
                    // but the prompt specifies Book, Food, Clothing. So, we can assume type is one of these
                    // or if a merchant adds "Electronics", we would need an Electronics class.
                    // The current design expects product type to match a known class.
                    // To support truly dynamic types without new classes, the Product base class would be instantiated.
                    // But the prompt requires "Book class, Food class, etc." as subclasses.
                    // This implies merchants pick from predefined *categories* that map to classes.
                    // "Merchants can add product types" - this is a bit ambiguous.
                    // Interpretation: They can assign a string like "Electronics" to productType.
                    // If we want to instantiate specific classes for these, we need more classes.
                    // For this implementation, we'll stick to the 3 examples.
                    // If a merchant adds a product of type "CustomToy", it will be stored with that type string.
                    // When displaying/loading, we need a strategy.
                    // Let's assume "adding product type" means they provide a string for the type.
                    // We can instantiate it as a base Product if no specific class, or have a default handling.
                    // For simplicity, we will use the provided classes and if a new type is encountered,
                    // it will still be created as a specific class type based on some logic or default to one.
                    // A better approach for truly dynamic types would be a factory pattern.
                    // Given the prompt, it's likely "adding product type" means the *value* of the type string,
                    // not dynamically creating new C++ classes at runtime.

                    // Let's adjust: for file loading, if type is unknown, we can log or skip.
                    // When adding a new product, the merchant *specifies* the type string.
                    // The challenge is what class to instantiate.
                    // For this exercise, let's assume merchants choose from "Book", "Food", "Clothing" or new string.
                    // If it's a new string, it's just stored. The specific *classes* are fixed.
                    std::cout << "Warning: Product type '" << type << "' loaded from file. For simplicity, created as generic. This product will behave like base product if not Book/Food/Clothing." << std::endl;
                    // This means when we search for type "Book", we get `Book` instances.
                    // When creating, if merchant says "Electronics", we'll store "Electronics" as type.
                    // But it will be an instance of `Book`, `Food`, or `Clothing` depending on user choice or a default.
                    // This part of the requirement is tricky without dynamic class creation.
                    // Let's refine: The *category types* are Food, Clothing, Books. Merchants add products to *these* types.
                    // "Merchants can add product types" could mean they can suggest new *categories* for future system updates,
                    // rather than create new C++ classes on the fly.
                    // For this implementation, we will assume the 3 specified types are the main ones.
                    // If a product in the file has another type, we'll need a strategy.
                    // Simpler: when a merchant ADDS a product, they select Book, Food, or Clothing.
                    // The "add product types" might be a higher-level system feature not code-dynamic.
                    // Let's stick to the 3 provided example types for instantiation.
                    // If file contains other types, we'll print a warning and skip.
                    std::cerr << "Warning: Unknown product type '" << type << "' for product '" << name << "'. Skipping." << std::endl;
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

    // For simplicity, we instantiate based on the productType string matching our known classes.
    // Requirement "merchants can add product types" -> this usually means they can define a *new string* for type,
    // not dynamically create C++ classes. How this new string type is handled (e.g., defaults to a base behavior
    // or requires admin to map it to a class) is a design choice.
    // Here, we'll map to our existing classes. If they enter "MyNewType", it won't map to a specific subclass easily
    // without a factory and potentially more generic product handling.
    // For this assignment, let's assume "adding product type" means they can enter any string,
    // and we decide which class to instantiate. A common way is to ask the merchant
    // "What kind of product is this (Book, Food, Clothing, Other)?" and then they can refine the `productType` string.

    // Let's assume for this project, productType will be one of the known types for instantiation.
    // The string itself can be more specific if needed (e.g. productType="SciFiBook" but still a Book object).

    if (productType == "Book") {
        newProd = new Book(newID, name, description, originalPrice, originalPrice, stock, ownerMerchantUsername); // Initial sale price = original
    }
    else if (productType == "Food") {
        newProd = new Food(newID, name, description, originalPrice, originalPrice, stock, ownerMerchantUsername);
    }
    else if (productType == "Clothing") {
        newProd = new Clothing(newID, name, description, originalPrice, originalPrice, stock, ownerMerchantUsername);
    }
    // What if merchant enters "Electronics"? For now, we require them to pick from existing *classes*.
    // The "productType" string they enter can be more granular, but the C++ object is one of these.
    // So, when adding, we'd ask "Name, Desc, Price, Stock, Owner, Category (Book/Food/Clothing)".
    // The `productType` string stored in the object can be the more specific type the merchant typed.
    // This means the `productType` constructor argument for Book, Food, Clothing should take the specific string.
    // Let's adjust Product subclasses constructors to take the type string.
    // (Revisiting Product.h/cpp: subclasses already pass their type string to base, which is good)
    // So, if merchant selects category "Book" and names it "SciFi Adventures", its type is "Book".
    // If "merchants can add product types" means they create a new *category* like "Electronics",
    // then the system should ideally have an `ElectronicsProduct` class.
    // For now, we'll limit to adding products under existing categories (Book, Food, Clothing).
    // The `productType` parameter in `addProduct` should be one of these.

    else {
        std::cerr << "Error: Unsupported product category for instantiation: " << productType << std::endl;
        // To fulfill "merchants can add product types", perhaps we should have a generic product instantiation.
        // For now, let's make it strict to the 3 types for object creation.
        // The prompt seems to imply the *existence* of at least Food, Clothing, Books.
        // "商家可以添加商品类型" (Merchants can add product types)
        // This means the string `productType` can be arbitrary.
        // But we need to decide which *C++ class* to instantiate.
        // Simple solution: All user-defined types map to a generic Product instance, or one of the existing ones by default.
        // For this exercise, let's assume if it's not Book, Food, Clothing, it's an error for now,
        // or it should be clarified how to handle instantiation.
        //
        // A better interpretation: The system has base classes (Book, Food, Clothing).
        // A merchant *adding a type* means they are adding a product whose `productType` string might be novel,
        // but it still must be categorized under an existing *class structure*.
        // E.g., they add "My Fancy Shirt", class Clothing, productType string "Fancy Party Wear".
        // If "add product types" means adding a *new category* like "Electronics", this requires
        // either dynamic class creation (very advanced) or system modification to add `ElectronicsProduct` class.
        //
        // Let's assume "adding product types" means they specify the `productType` string,
        // and we map it to an existing concrete class or fail.
        // For the project, it's safer to stick to the defined classes.
        // We will make `productType` in `addProduct` one of "Book", "Food", "Clothing".
        std::cout << "Unsupported product category. Please choose Book, Food, or Clothing." << std::endl;
        nextProductID--; // Rollback ID if not used
        return false;
    }


    products.push_back(newProd);
    saveProductsToFile(); // Persist
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
    return products; // Returns a copy of the vector of pointers
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
    std::transform(termLower.begin(), termLower.end(), termLower.begin(), ::tolower);

    for (Product* p : products) {
        bool match = false;
        if (searchBy == "name") {
            std::string nameLower = p->getName();
            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
            if (nameLower.find(termLower) != std::string::npos) { // Fuzzy: substring
                match = true;
            }
        }
        else if (searchBy == "type") {
            std::string typeLower = p->getProductType();
            std::transform(typeLower.begin(), typeLower.end(), typeLower.begin(), ::tolower);
            if (typeLower.find(termLower) != std::string::npos) { // Fuzzy: substring for type too
                match = true;
            }
        }
        else if (searchBy == "merchant") {
            std::string merchantLower = p->getOwnerMerchantUsername();
            std::transform(merchantLower.begin(), merchantLower.end(), merchantLower.begin(), ::tolower);
            if (merchantLower.find(termLower) != std::string::npos) { // Fuzzy: substring
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
            delete* it; // Free memory
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
    // This function is called after a product's members are changed
    // The primary action is to save all products to reflect changes
    if (product) { // Basic check
        saveProductsToFile();
    }
}

std::vector<std::string> ProductManager::getAvailableProductTypes() const {
    std::set<std::string> typesSet; // Use set to get unique types
    for (const auto* p : products) {
        typesSet.insert(p->getProductType());
    }
    return std::vector<std::string>(typesSet.begin(), typesSet.end());
}