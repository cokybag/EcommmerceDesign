// (Previous includes and helper functions from the first part of main.cpp)
// ...
#include <iostream>
#include <string>
#include <vector>
#include <limits> 
#include <algorithm> 
#include <iomanip> 
#include <set>

#include "UserManager.h"
#include "ProductManager.h"
// User.h and Product.h are included via UserManager.h and ProductManager.h

// Forward declarations for menu functions
void showMainMenu(UserManager& um, ProductManager& pm, User*& currentUser);
void showConsumerMenu(UserManager& um, ProductManager& pm, User*& currentUser);
void showMerchantMenu(UserManager& um, ProductManager& pm, User*& currentUser);

// Helper for robust input
template <typename T>
T getValidatedInput(const std::string& prompt) {
    T value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.good()) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer
            return value;
        }
        else {
            std::cout << "Invalid input. Please try again." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

std::string getLineInput(const std::string& prompt, bool allowEmpty = false) {
    std::string value;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, value);
        if (allowEmpty || !value.empty()) {
            return value;
        }
        std::cout << "Input cannot be empty. Please try again." << std::endl;
    }
}


// --- User related actions --- 
void handleRegister(UserManager& um) {
    std::cout << "\n--- User Registration ---" << std::endl;
    std::string uname = getLineInput("Enter username: ");
    if (um.isUsernameTaken(uname)) {
        std::cout << "Username already taken. Please try another." << std::endl;
        return;
    }

    std::string pwd1, pwd2;
    do {
        pwd1 = getLineInput("Enter password (cannot be empty): ");
        pwd2 = getLineInput("Confirm password: ");
        if (pwd1 != pwd2) {
            std::cout << "Passwords do not match. Please try again." << std::endl;
        }
    } while (pwd1 != pwd2);

    std::string typeChoice;
    std::cout << "Register as (1) Consumer or (2) Merchant: ";
    std::getline(std::cin, typeChoice);

    std::string accountType;
    if (typeChoice == "1") accountType = "Consumer";
    else if (typeChoice == "2") accountType = "Merchant";
    else {
        std::cout << "Invalid choice. Registration failed." << std::endl;
        return;
    }

    if (um.registerUser(uname, pwd1, accountType)) {
        std::cout << "Registration successful as " << accountType << "!" << std::endl;
    }
    else {
        std::cout << "Registration failed. Username might be taken or invalid type." << std::endl;
    }
}

void handleLogin(UserManager& um, User*& currentUser) {
    std::cout << "\n--- User Login ---" << std::endl;
    if (currentUser) {
        std::cout << "Already logged in as " << currentUser->getUsername() << std::endl;
        return;
    }
    std::string uname = getLineInput("Enter username: ");
    std::string pwd = getLineInput("Enter password: ");

    currentUser = um.loginUser(uname, pwd);
    if (currentUser) {
        std::cout << "Login successful. Welcome, " << currentUser->getUsername() << "!" << std::endl;
    }
    else {
        std::cout << "Login failed. Invalid username or password." << std::endl;
    }
}

void handleLogout(User*& currentUser, UserManager& um) {
    if (currentUser) {
        std::cout << "Logging out " << currentUser->getUsername() << "." << std::endl;
        um.persistChanges();
        currentUser = nullptr;
    }
    else {
        std::cout << "Not logged in." << std::endl;
    }
}

void handleChangePassword(UserManager& um, User* currentUser) {
    if (!currentUser) {
        std::cout << "You must be logged in to change your password." << std::endl;
        return;
    }
    std::cout << "\n--- Change Password ---" << std::endl;
    std::string oldPwd = getLineInput("Enter current password: ");
    if (!currentUser->checkPassword(oldPwd)) {
        std::cout << "Incorrect current password." << std::endl;
        return;
    }

    std::string pwd1, pwd2;
    do {
        pwd1 = getLineInput("Enter new password (cannot be empty): ");
        pwd2 = getLineInput("Confirm new password: ");
        if (pwd1 != pwd2) {
            std::cout << "New passwords do not match. Please try again." << std::endl;
        }
    } while (pwd1 != pwd2);

    currentUser->setPassword(pwd1);
    um.persistChanges();
    std::cout << "Password changed successfully." << std::endl;
}

void handleBalanceManagement(UserManager& um, User* currentUser) {
    if (!currentUser) {
        std::cout << "You must be logged in to manage balance." << std::endl;
        return;
    }
    std::cout << "\n--- Balance Management ---" << std::endl;
    std::cout << "Current balance: $" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
    std::cout << "1. Recharge balance" << std::endl;
    std::cout << "2. View balance (already shown)" << std::endl;
    // "Consume" part for Task 1 is typically demonstrated by spending.
    // Since direct purchase is removed for Task 1 as per request,
    // a generic withdraw option can represent "consume" if needed,
    // or just recharge and view are sufficient for balance management.
    // For simplicity, let's keep it to recharge and view.
    // If "consume" needs to be explicitly shown, a "Withdraw" option could be added.
    std::cout << "0. Back" << std::endl;

    std::string choiceStr = getLineInput("Your choice: ");
    int choice = -1;
    try {
        choice = std::stoi(choiceStr);
    }
    catch (const std::exception& e) {
        std::cout << "Invalid input." << std::endl;
        return;
    }

    if (choice == 1) {
        double amount = getValidatedInput<double>("Enter amount to recharge: $");
        if (amount <= 0) {
            std::cout << "Recharge amount must be positive." << std::endl;
            return;
        }
        currentUser->deposit(amount);
        um.persistChanges();
        std::cout << "Recharge successful. New balance: $" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
    }
    else if (choice == 0) {
        return;
    }
    else if (choice != 2) {
        std::cout << "Invalid choice." << std::endl;
    }
}
// --- End of User related actions ---


// --- Product related actions (General) ---
void displayProducts(const std::vector<Product*>& productList) {
    if (productList.empty()) {
        std::cout << "No products to display." << std::endl;
        return;
    }
    std::cout << "\n--- Available Products ---" << std::endl;
    for (const auto* product : productList) {
        product->displayDetails();
    }
}

void handleDisplayAllProducts(ProductManager& pm) {
    displayProducts(pm.getAllProducts());
}

void handleSearchProducts(ProductManager& pm) {
    std::cout << "\n--- Search Products ---" << std::endl;
    std::cout << "Search by (1) Name, (2) Type, (3) Merchant: ";
    std::string choiceStr = getLineInput("");
    int choice = -1;
    try {
        choice = std::stoi(choiceStr);
    }
    catch (const std::exception& e) {
        std::cout << "Invalid input." << std::endl;
        return;
    }

    std::string searchTerm = getLineInput("Enter search term: ");
    std::string searchBy;

    if (choice == 1) searchBy = "name";
    else if (choice == 2) searchBy = "type";
    else if (choice == 3) searchBy = "merchant";
    else {
        std::cout << "Invalid search criteria." << std::endl;
        return;
    }

    std::vector<Product*> results = pm.searchProducts(searchTerm, searchBy);
    if (results.empty()) {
        std::cout << "No products found matching your criteria." << std::endl;
    }
    else {
        std::cout << "--- Search Results ---" << std::endl;
        displayProducts(results);
    }
}

// --- Consumer-Specific Actions ---
// REMOVED: handlePurchaseProduct function as per request for Task 1 simplification
/*
void handlePurchaseProduct(UserManager& um, ProductManager& pm, User* consumerUser) {
    // ... entire function body removed ...
}
*/

// --- Merchant-Specific Actions ---
void handleAddProduct(ProductManager& pm, User* merchantUser) {
    if (!merchantUser || merchantUser->getUserType() != "Merchant") {
        std::cout << "Error: Must be logged in as a Merchant." << std::endl;
        return;
    }
    Merchant* merchant = static_cast<Merchant*>(merchantUser);

    std::cout << "\n--- Add New Product ---" << std::endl;
    std::string name = getLineInput("Enter product name: ");
    std::string description = getLineInput("Enter product description: ");
    double originalPrice = -1.0;
    while (originalPrice < 0) {
        originalPrice = getValidatedInput<double>("Enter product original price: $");
        if (originalPrice < 0) std::cout << "Price cannot be negative." << std::endl;
    }
    int stock = -1;
    while (stock < 0) {
        stock = getValidatedInput<int>("Enter product stock quantity: ");
        if (stock < 0) std::cout << "Stock cannot be negative." << std::endl;
    }

    std::string productTypeChoice;
    std::string productType; // This will be "Book", "Food", or "Clothing" for class instantiation
    while (true) {
        std::cout << "Select product category for instantiation:\n1. Book\n2. Food\n3. Clothing\nYour choice: ";
        std::getline(std::cin, productTypeChoice);
        if (productTypeChoice == "1") { productType = "Book"; break; }
        if (productTypeChoice == "2") { productType = "Food"; break; }
        if (productTypeChoice == "3") { productType = "Clothing"; break; }
        std::cout << "Invalid choice. Please select 1, 2, or 3 for the base product class." << std::endl;
    }

    // Optional: Allow merchant to specify a more detailed type string if desired,
    // but the C++ class will be one of the above.
    // For Task 1, productType being "Book", "Food", "Clothing" is sufficient.

    if (pm.addProduct(name, description, originalPrice, stock, merchant->getUsername(), productType)) {
        std::cout << "Product '" << name << "' added successfully." << std::endl;
        // Discount application will be handled within pm.addProduct if a rule exists
    }
    else {
        std::cout << "Failed to add product." << std::endl;
    }
}

void handleManageMyProducts(ProductManager& pm, User* merchantUser) {
    if (!merchantUser || merchantUser->getUserType() != "Merchant") {
        std::cout << "Error: Must be logged in as a Merchant." << std::endl;
        return;
    }
    Merchant* merchant = static_cast<Merchant*>(merchantUser);

    std::cout << "\n--- Manage My Products ---" << std::endl;
    std::vector<Product*> myProducts = pm.getProductsByMerchant(merchant->getUsername());
    if (myProducts.empty()) {
        std::cout << "You have not added any products yet." << std::endl;
        return;
    }
    displayProducts(myProducts);

    std::string productID = getLineInput("Enter ID of product to manage (or 0 to cancel): ");
    if (productID == "0") return;

    Product* product = pm.findProductByID(productID);
    if (!product || product->getOwnerMerchantUsername() != merchant->getUsername()) {
        std::cout << "Product ID not found or you do not own this product." << std::endl;
        return;
    }

    std::cout << "\nManaging Product: " << product->getName() << " (ID: " << product->getID() << ")" << std::endl;
    std::cout << "1. Update Price (Original & Sale)" << std::endl;
    std::cout << "2. Update Stock" << std::endl;
    std::cout << "3. Set Discount (Update Sale Price for this specific product)" << std::endl;
    std::cout << "4. Remove Product" << std::endl;
    std::cout << "5. Update Description" << std::endl;
    std::cout << "6. Update Name" << std::endl;
    std::cout << "0. Back" << std::endl;

    std::string choiceStr = getLineInput("Your choice: ");
    int choice = -1;
    try {
        choice = std::stoi(choiceStr);
    }
    catch (const std::exception& e) {
        std::cout << "Invalid input." << std::endl;
        return;
    }

    bool changed = false;
    switch (choice) {
    case 1: {
        double newOrigPrice = -1.0;
        while (newOrigPrice < 0) {
            newOrigPrice = getValidatedInput<double>("Enter new original price: $");
            if (newOrigPrice < 0) std::cout << "Price cannot be negative.\n";
        }
        product->setOriginalPrice(newOrigPrice);
        double newSalePrice = -1.0;
        while (newSalePrice < 0 || newSalePrice > newOrigPrice) {
            newSalePrice = getValidatedInput<double>("Enter new sale price (cannot exceed original): $");
            if (newSalePrice < 0) std::cout << "Price cannot be negative.\n";
            if (newSalePrice > newOrigPrice) std::cout << "Sale price cannot exceed original price.\n";
        }
        product->setCurrentSalePrice(newSalePrice);
        changed = true;
        break;
    }
    case 2: {
        int newStock = -1;
        while (newStock < 0) {
            newStock = getValidatedInput<int>("Enter new stock quantity: ");
            if (newStock < 0) std::cout << "Stock cannot be negative.\n";
        }
        product->setStock(newStock);
        changed = true;
        break;
    }
    case 3: {
        double discountPercent = -1.0;
        while (discountPercent < 0 || discountPercent > 100) {
            discountPercent = getValidatedInput<double>("Enter discount percentage for this product (e.g., 10 for 10% off): ");
            if (discountPercent < 0 || discountPercent > 100) std::cout << "Discount must be between 0 and 100.\n";
        }
        double newSalePrice = product->getOriginalPrice() * (1.0 - (discountPercent / 100.0));
        product->setCurrentSalePrice(newSalePrice);
        std::cout << "New sale price for this product set to: $" << std::fixed << std::setprecision(2) << product->getCurrentSalePrice() << std::endl;
        changed = true;
        break;
    }
    case 4: {
        std::string confirm = getLineInput("Are you sure you want to remove this product? (y/n): ");
        if (confirm == "y" || confirm == "Y") {
            if (pm.removeProduct(product->getID(), merchant->getUsername())) {
                std::cout << "Product removed." << std::endl;
            }
            else {
                std::cout << "Failed to remove product." << std::endl;
            }
            return;
        }
        break;
    }
    case 5: {
        std::string newDesc = getLineInput("Enter new description: ");
        product->setDescription(newDesc);
        changed = true;
        break;
    }
    case 6: {
        std::string newName = getLineInput("Enter new name: ");
        product->setName(newName);
        changed = true;
        break;
    }
    case 0:
        return;
    default:
        std::cout << "Invalid choice." << std::endl;
        break;
    }

    if (changed) {
        pm.updateProduct(product);
        std::cout << "Product details updated." << std::endl;
    }
}

void handleDiscountCategory(ProductManager& pm, User* merchantUser) {
    if (!merchantUser || merchantUser->getUserType() != "Merchant") {
        std::cout << "Error: Must be logged in as a Merchant." << std::endl;
        return;
    }
    Merchant* merchant = static_cast<Merchant*>(merchantUser);

    std::cout << "\n--- Discount Products by Category (for your products) ---" << std::endl;

    std::vector<Product*> myProducts = pm.getProductsByMerchant(merchant->getUsername());
    if (myProducts.empty()) {
        std::cout << "You have no products to discount." << std::endl;
        return;
    }

    std::set<std::string> myTypesSet;
    for (const auto* p : myProducts) {
        myTypesSet.insert(p->getProductType());
    }
    if (myTypesSet.empty() && pm.getAvailableProductTypes().empty()) { // Check both merchant's types and all platform types
        std::cout << "No product categories found for your products or on the platform." << std::endl;
        return;
    }

    std::cout << "Your current product categories: ";
    if (myTypesSet.empty()) std::cout << "None. ";
    for (const auto& type : myTypesSet) {
        std::cout << type << " ";
    }
    std::cout << "\nAll platform categories: ";
    std::vector<std::string> allTypes = pm.getAvailableProductTypes();
    if (allTypes.empty()) std::cout << "None.";
    for (const auto& type : allTypes) {
        std::cout << type << " ";
    }
    std::cout << std::endl;


    std::string categoryToDiscount = getLineInput("Enter product category (e.g., Book, Food, Clothing) to apply/update discount for: ");
    // Validate category string (optional, could allow any string for future types)
    // For now, we expect one of the known types for simplicity, but ProductManager handles any string.
    bool platformHasCategory = false;
    for (const auto& type : allTypes) {
        if (type == categoryToDiscount) {
            platformHasCategory = true;
            break;
        }
    }
    // Check if the chosen category is one of the basic ones if you want to be strict.
    if (categoryToDiscount != "Book" && categoryToDiscount != "Food" && categoryToDiscount != "Clothing" && !platformHasCategory) {
        std::cout << "Warning: '" << categoryToDiscount << "' is not a standard or existing category. "
            << "You can still set a discount rule for it if you plan to add such products." << std::endl;
    }


    double discountPercent = -1.0;
    while (discountPercent < 0 || discountPercent > 100) {
        discountPercent = getValidatedInput<double>("Enter discount percentage (0-100, 0 to remove discount): ");
        if (discountPercent < 0 || discountPercent > 100) std::cout << "Discount must be between 0 and 100.\n";
    }

    // Use the new ProductManager method
    pm.applyCategoryDiscount(merchant->getUsername(), categoryToDiscount, discountPercent);
    // The pm.applyCategoryDiscount method will print messages and save changes.
    // No need for pm.persistChanges() here as applyCategoryDiscount calls saveProductsToFile.
}


// --- Menu Functions ---
void showMainMenu(UserManager& um, ProductManager& pm, User*& currentUser) {
    std::string choiceStr;
    int choice = -1;

    while (true) {
        std::cout << "\n========= E-commerce Platform =========" << std::endl;
        if (currentUser) {
            std::cout << "Logged in as: " << currentUser->getUsername()
                << " (" << currentUser->getUserType()
                << ") | Balance: $" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
        }
        else {
            std::cout << "Welcome, Guest!" << std::endl;
        }
        std::cout << "---------------------------------------" << std::endl;
        std::cout << "1. Display All Products" << std::endl;
        std::cout << "2. Search Products" << std::endl;
        if (!currentUser) {
            std::cout << "3. Register" << std::endl;
            std::cout << "4. Login" << std::endl;
        }
        else {
            std::cout << "3. My Account Options" << std::endl;
            std::cout << "4. Logout" << std::endl;
        }
        std::cout << "0. Exit" << std::endl;
        std::cout << "---------------------------------------" << std::endl;

        choiceStr = getLineInput("Your choice: ");
        try {
            choice = std::stoi(choiceStr);
        }
        catch (const std::exception& e) {
            choice = -1;
        }

        switch (choice) {
        case 1: handleDisplayAllProducts(pm); break;
        case 2: handleSearchProducts(pm); break;
        case 3:
            if (!currentUser) handleRegister(um);
            else {
                if (currentUser->getUserType() == "Consumer") {
                    showConsumerMenu(um, pm, currentUser);
                }
                else if (currentUser->getUserType() == "Merchant") {
                    showMerchantMenu(um, pm, currentUser);
                }
            }
            break;
        case 4:
            if (!currentUser) handleLogin(um, currentUser);
            else handleLogout(currentUser, um);
            break;
        case 0:
            std::cout << "Exiting platform. Goodbye!" << std::endl;
            if (currentUser) um.persistChanges();
            pm.persistChanges();
            return;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
}

void showConsumerMenu(UserManager& um, ProductManager& pm, User*& currentUser) {
    std::string choiceStr;
    int choice = -1;

    while (currentUser && currentUser->getUserType() == "Consumer") {
        std::cout << "\n--- Consumer Menu (" << currentUser->getUsername() << ") ---" << std::endl;
        std::cout << "Balance: $" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
        std::cout << "1. View All Products" << std::endl;
        std::cout << "2. Search Products" << std::endl;
        // std::cout << "3. Purchase Product" << std::endl; // REMOVED
        std::cout << "3. Manage Balance" << std::endl;     // Re-numbered
        std::cout << "4. Change Password" << std::endl;    // Re-numbered
        std::cout << "0. Back to Main Menu (Logout)" << std::endl;

        choiceStr = getLineInput("Your choice: ");
        try {
            choice = std::stoi(choiceStr);
        }
        catch (const std::exception& e) {
            choice = -1;
        }

        switch (choice) {
        case 1: handleDisplayAllProducts(pm); break;
        case 2: handleSearchProducts(pm); break;
            // case 3: handlePurchaseProduct(um, pm, currentUser); break; // REMOVED
        case 3: handleBalanceManagement(um, currentUser); break; // Now case 3
        case 4: handleChangePassword(um, currentUser); break;    // Now case 4
        case 0:
            handleLogout(currentUser, um);
            return;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
    if (!currentUser) {
        return;
    }
}

void showMerchantMenu(UserManager& um, ProductManager& pm, User*& currentUser) {
    std::string choiceStr;
    int choice = -1;

    while (currentUser && currentUser->getUserType() == "Merchant") {
        std::cout << "\n--- Merchant Menu (" << currentUser->getUsername() << ") ---" << std::endl;
        std::cout << "Balance: $" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
        std::cout << "1. Add New Product" << std::endl;
        std::cout << "2. Manage My Products" << std::endl;
        std::cout << "3. Set/Update Discount by Category (for my products)" << std::endl; // Clarified name
        std::cout << "4. View All Platform Products" << std::endl;
        std::cout << "5. Search Platform Products" << std::endl;
        std::cout << "6. Manage Balance" << std::endl;
        std::cout << "7. Change Password" << std::endl;
        std::cout << "0. Back to Main Menu (Logout)" << std::endl;

        choiceStr = getLineInput("Your choice: ");
        try {
            choice = std::stoi(choiceStr);
        }
        catch (const std::exception& e) {
            choice = -1;
        }

        switch (choice) {
        case 1: handleAddProduct(pm, currentUser); break;
        case 2: handleManageMyProducts(pm, currentUser); break;
        case 3: handleDiscountCategory(pm, currentUser); break;
        case 4: handleDisplayAllProducts(pm); break;
        case 5: handleSearchProducts(pm); break;
        case 6: handleBalanceManagement(um, currentUser); break;
        case 7: handleChangePassword(um, currentUser); break;
        case 0:
            handleLogout(currentUser, um);
            return;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
    if (!currentUser) {
        return;
    }
}


// --- Main Function ---
int main() {
    UserManager userManager;
    ProductManager productManager;

    User* currentUser = nullptr;

    showMainMenu(userManager, productManager, currentUser);

    // PersistChanges is called at exit from main menu and on logout.
    return 0;
}

// Make sure the split function is defined. If it's in UserManager.cpp and not in a common header,
// you might need to define it again or move it to a utility file and include it.
// For simplicity, if it's only in UserManager.cpp, you might need to redeclare it here or move its definition.
// Let's assume it's available (e.g., if UserManager.cpp is compiled and linked, its 'split' is available if not static).
// To be safe, one could define it in a separate util.h and util.cpp or make it a static helper in main.
// Given the project structure, having 'extern std::vector<std::string> split(...);' in ProductManager.cpp
// and relying on UserManager.cpp providing the definition during linking is a common approach.