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


// --- User related actions --- (Copied from previous response for completeness)
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
    std::getline(std::cin, typeChoice); // Use getline for safety after other inputs

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
        um.persistChanges(); // Persist any user data changes (like balance) before logout
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
    um.persistChanges(); // Save updated user data
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
void handlePurchaseProduct(UserManager& um, ProductManager& pm, User* consumerUser) {
    if (!consumerUser || consumerUser->getUserType() != "Consumer") {
        std::cout << "Error: Must be logged in as a Consumer." << std::endl;
        return;
    }
    Consumer* consumer = static_cast<Consumer*>(consumerUser);

    std::cout << "\n--- Purchase Product ---" << std::endl;
    handleDisplayAllProducts(pm); // Show available products

    std::string productID = getLineInput("Enter ID of the product to purchase (or 0 to cancel): ");
    if (productID == "0") return;

    Product* product = pm.findProductByID(productID);
    if (!product) {
        std::cout << "Product ID not found." << std::endl;
        return;
    }

    if (product->getStock() <= 0) {
        std::cout << "Sorry, this product is out of stock." << std::endl;
        return;
    }

    int quantity = getValidatedInput<int>("Enter quantity to purchase: ");
    if (quantity <= 0) {
        std::cout << "Quantity must be positive." << std::endl;
        return;
    }
    if (quantity > product->getStock()) {
        std::cout << "Not enough stock. Available: " << product->getStock() << std::endl;
        return;
    }

    double totalCost = product->getPrice() * quantity;
    std::cout << "Product: " << product->getName() << ", Quantity: " << quantity
        << ", Total Cost: $" << std::fixed << std::setprecision(2) << totalCost << std::endl;

    if (consumer->getBalance() < totalCost) {
        std::cout << "Insufficient balance. Your balance: $" << consumer->getBalance() << std::endl;
        return;
    }

    std::string confirm = getLineInput("Confirm purchase? (y/n): ");
    if (confirm == "y" || confirm == "Y") {
        if (consumer->withdraw(totalCost)) {
            product->setStock(product->getStock() - quantity);

            // Find merchant and credit them
            User* merchantUser = um.findUser(product->getOwnerMerchantUsername());
            if (merchantUser && merchantUser->getUserType() == "Merchant") {
                merchantUser->deposit(totalCost); // Merchant gets the full sale price
            }
            else {
                std::cout << "Warning: Merchant account " << product->getOwnerMerchantUsername()
                    << " not found or not a merchant. Funds not transferred to seller." << std::endl;
            }

            um.persistChanges(); // Save user balance changes
            pm.persistChanges(); // Save product stock changes
            std::cout << "Purchase successful! Your new balance: $" << std::fixed << std::setprecision(2) << consumer->getBalance() << std::endl;
        }
        else {
            std::cout << "Purchase failed due to an unexpected balance issue." << std::endl; // Should have been caught
        }
    }
    else {
        std::cout << "Purchase cancelled." << std::endl;
    }
}


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
    std::string productType;
    while (true) {
        std::cout << "Select product category:\n1. Book\n2. Food\n3. Clothing\nYour choice: ";
        std::getline(std::cin, productTypeChoice);
        if (productTypeChoice == "1") { productType = "Book"; break; }
        if (productTypeChoice == "2") { productType = "Food"; break; }
        if (productTypeChoice == "3") { productType = "Clothing"; break; }
        std::cout << "Invalid choice. Please select 1, 2, or 3." << std::endl;
    }
    // The requirement "商家可以添加商品类型" (Merchants can add product types)
    // is interpreted as they can input a string for the product's type attribute.
    // For instantiation, we are limited to Book, Food, Clothing classes.
    // For now, we will use the category as the productType string.
    // A more advanced version might allow a custom string here that gets stored,
    // while still instantiating one of the base types.
    // For this project, `productType` string will be "Book", "Food", or "Clothing".

    if (pm.addProduct(name, description, originalPrice, stock, merchant->getUsername(), productType)) {
        std::cout << "Product '" << name << "' added successfully." << std::endl;
    }
    else {
        std::cout << "Failed to add product. Ensure category is valid if system restricts." << std::endl;
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
    std::cout << "3. Set Discount (Update Sale Price)" << std::endl;
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
        product->setOriginalPrice(newOrigPrice); // This might also adjust sale price
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
    case 3: { // Set Discount
        double discountPercent = -1.0;
        while (discountPercent < 0 || discountPercent > 100) {
            discountPercent = getValidatedInput<double>("Enter discount percentage (e.g., 10 for 10% off): ");
            if (discountPercent < 0 || discountPercent > 100) std::cout << "Discount must be between 0 and 100.\n";
        }
        double newSalePrice = product->getOriginalPrice() * (1.0 - (discountPercent / 100.0));
        product->setCurrentSalePrice(newSalePrice); // setCurrentSalePrice handles validation
        std::cout << "New sale price set to: $" << std::fixed << std::setprecision(2) << product->getCurrentSalePrice() << std::endl;
        changed = true;
        break;
    }
    case 4: {
        std::string confirm = getLineInput("Are you sure you want to remove this product? (y/n): ");
        if (confirm == "y" || confirm == "Y") {
            if (pm.removeProduct(product->getID(), merchant->getUsername())) {
                std::cout << "Product removed." << std::endl;
                // product pointer is now dangling, don't use it further
            }
            else {
                std::cout << "Failed to remove product." << std::endl; // Should be caught by earlier checks
            }
            // No 'changed = true' here as removeProduct saves.
            return; // Return to avoid further ops on deleted product
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
        pm.updateProduct(product); // This calls saveProductsToFile
        std::cout << "Product details updated." << std::endl;
    }
}

void handleDiscountCategory(ProductManager& pm, User* merchantUser) {
    if (!merchantUser || merchantUser->getUserType() != "Merchant") {
        std::cout << "Error: Must be logged in as a Merchant." << std::endl;
        return;
    }
    Merchant* merchant = static_cast<Merchant*>(merchantUser);

    std::cout << "\n--- Discount Products by Category ---" << std::endl;

    // Get available types from merchant's products
    std::vector<Product*> myProducts = pm.getProductsByMerchant(merchant->getUsername());
    if (myProducts.empty()) {
        std::cout << "You have no products to discount." << std::endl;
        return;
    }

    std::set<std::string> myTypesSet;
    for (const auto* p : myProducts) {
        myTypesSet.insert(p->getProductType());
    }
    if (myTypesSet.empty()) {
        std::cout << "No product categories found for your products." << std::endl;
        return;
    }

    std::cout << "Your product categories: ";
    for (const auto& type : myTypesSet) {
        std::cout << type << " ";
    }
    std::cout << std::endl;

    std::string categoryToDiscount = getLineInput("Enter product category (e.g., Book, Food, Clothing) to discount: ");
    // Validate if this category exists for this merchant
    bool categoryExists = false;
    for (const auto* p : myProducts) {
        if (p->getProductType() == categoryToDiscount) {
            categoryExists = true;
            break;
        }
    }
    if (!categoryExists) {
        std::cout << "You don't have any products in the category: " << categoryToDiscount << std::endl;
        return;
    }


    double discountPercent = -1.0;
    while (discountPercent < 0 || discountPercent > 100) {
        discountPercent = getValidatedInput<double>("Enter discount percentage (0-100): ");
        if (discountPercent < 0 || discountPercent > 100) std::cout << "Discount must be between 0 and 100.\n";
    }


    int count = 0;
    for (Product* p : myProducts) {
        if (p->getProductType() == categoryToDiscount) {
            double newSalePrice = p->getOriginalPrice() * (1.0 - (discountPercent / 100.0));
            p->setCurrentSalePrice(newSalePrice);
            count++;
        }
    }

    if (count > 0) {
        pm.persistChanges(); // Save all product changes
        std::cout << count << " products in category '" << categoryToDiscount
            << "' have been discounted by " << discountPercent << "%." << std::endl;
    }
    else {
        // This case should be caught by 'categoryExists' check, but as a fallback:
        std::cout << "No products found for you in category '" << categoryToDiscount << "'." << std::endl;
    }
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
            std::cout << "3. My Account Options" << std::endl; // Leads to user-specific menu
            std::cout << "4. Logout" << std::endl;
        }
        std::cout << "0. Exit" << std::endl;
        std::cout << "---------------------------------------" << std::endl;

        choiceStr = getLineInput("Your choice: ");
        try {
            choice = std::stoi(choiceStr);
        }
        catch (const std::exception& e) {
            choice = -1; // Invalid input
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
            if (currentUser) um.persistChanges(); // Ensure user data is saved if logged in
            pm.persistChanges(); // Ensure product data is saved
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
        std::cout << "3. Purchase Product" << std::endl;
        std::cout << "4. Manage Balance" << std::endl;
        std::cout << "5. Change Password" << std::endl;
        std::cout << "0. Back to Main Menu (Logout)" << std::endl; // Changed to logout for simplicity

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
        case 3: handlePurchaseProduct(um, pm, currentUser); break;
        case 4: handleBalanceManagement(um, currentUser); break;
        case 5: handleChangePassword(um, currentUser); break;
        case 0:
            handleLogout(currentUser, um); // Effectively logs out and will return to main menu's loop
            return;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
    if (!currentUser) { // If logout happened, ensure we exit this menu loop
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
        std::cout << "2. Manage My Products" << std::endl; // View, Update, Remove
        std::cout << "3. Discount Products by Category" << std::endl;
        std::cout << "4. View All Platform Products" << std::endl;
        std::cout << "5. Search Platform Products" << std::endl;
        std::cout << "6. Manage Balance" << std::endl;
        std::cout << "7. Change Password" << std::endl;
        std::cout << "0. Back to Main Menu (Logout)" << std::endl; // Changed to logout

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
            handleLogout(currentUser, um); // Logs out and returns to main menu's loop
            return;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
    if (!currentUser) { // If logout happened, ensure we exit this menu loop
        return;
    }
}


// --- Main Function ---
int main() {
    // Create managers: They will load data from files in their constructors
    UserManager userManager;
    ProductManager productManager;

    User* currentUser = nullptr; // No user logged in initially

    showMainMenu(userManager, productManager, currentUser);

    // Destructors of userManager and productManager will handle cleanup
    // (deleting dynamically allocated User and Product objects)
    // and optionally save data if not done explicitly elsewhere.
    // PersistChanges is called at exit from main menu and on logout.

    return 0;
}