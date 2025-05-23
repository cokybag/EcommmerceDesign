#include <iostream>
#include <string>
#include <vector>
#include <limits> 
#include <algorithm> 
#include <iomanip> 
#include <set>
#include <chrono> // For order timeout - though not fully implemented due to console nature

#include "UserManager.h"
#include "ProductManager.h"
#include "ShoppingCart.h" // Included via Users.h (Consumer)
#include "OrderManager.h"   // New manager
// User.h, Product.h, Order.h are included via their respective managers or Consumer

// Forward declarations for menu functions
void showMainMenu(UserManager& um, ProductManager& pm, OrderManager& om, User*& currentUser);
void showConsumerMenu(UserManager& um, ProductManager& pm, OrderManager& om, User*& currentUser);
void showMerchantMenu(UserManager& um, ProductManager& pm, OrderManager& om, User*& currentUser); // Added OrderManager

// Helper for robust input (no changes)
template <typename T>
T getValidatedInput(const std::string& prompt) {
    T value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.good()) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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

// User related actions (no significant changes, ensure persistChanges is called)
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

void handleLogout(User*& currentUser, UserManager& um, ProductManager& pm, OrderManager& om) { // Added PM, OM
    if (currentUser) {
        std::cout << "Logging out " << currentUser->getUsername() << "." << std::endl;
        um.persistChanges();
        pm.persistChanges(); // Persist product stock changes that might have occurred
        om.persistChanges(); // Persist any order status changes
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
    std::cout << "0. Back" << std::endl;
    std::string choiceStr = getLineInput("Your choice: ");
    int choice = -1;
    try { choice = std::stoi(choiceStr); }
    catch (const std::exception&) { std::cout << "Invalid input." << std::endl; return; }
    if (choice == 1) {
        double amount = getValidatedInput<double>("Enter amount to recharge: $");
        if (amount <= 0) {
            std::cout << "Recharge amount must be positive." << std::endl; return;
        }
        currentUser->deposit(amount);
        um.persistChanges();
        std::cout << "Recharge successful. New balance: $" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
    }
    else if (choice == 0) { return; }
    else if (choice != 2) { std::cout << "Invalid choice." << std::endl; }
}

// Product related actions (General - no major changes)
void displayProducts(const std::vector<Product*>& productList, bool showStockDetail = true) { // Added showStockDetail
    if (productList.empty()) {
        std::cout << "No products to display." << std::endl;
        return;
    }
    std::cout << "\n--- Available Products ---" << std::endl;
    for (const auto* product : productList) {
        // product->displayDetails(); // Already shows detailed stock
        // For cart view, we might want less detail, but displayDetails is fine
        if (showStockDetail) {
            product->displayDetails();
        }
        else { // A more compact view for quick browsing
            std::cout << "-------------------------------------\n"
                << "ID: " << product->getID() << ", Name: " << product->getName() << "\n"
                << "Price: $" << std::fixed << std::setprecision(2) << product->getCurrentSalePrice()
                << ", Available Stock: " << product->getAvailableStock() << "\n"
                << "Sold by: " << product->getOwnerMerchantUsername() << "\n"
                << "-------------------------------------" << std::endl;
        }
    }
}

void handleDisplayAllProducts(ProductManager& pm, bool compact = false) {
    displayProducts(pm.getAllProducts(), !compact);
}

void handleSearchProducts(ProductManager& pm) {
    std::cout << "\n--- Search Products ---" << std::endl;
    std::cout << "Search by (1) Name, (2) Type, (3) Merchant: ";
    std::string choiceStr = getLineInput("");
    int choice = -1;
    try { choice = std::stoi(choiceStr); }
    catch (const std::exception&) { std::cout << "Invalid input." << std::endl; return; }
    std::string searchTerm = getLineInput("Enter search term: ");
    std::string searchBy;
    if (choice == 1) searchBy = "name";
    else if (choice == 2) searchBy = "type";
    else if (choice == 3) searchBy = "merchant";
    else { std::cout << "Invalid search criteria." << std::endl; return; }
    std::vector<Product*> results = pm.searchProducts(searchTerm, searchBy);
    if (results.empty()) {
        std::cout << "No products found matching your criteria." << std::endl;
    }
    else {
        std::cout << "--- Search Results ---" << std::endl;
        displayProducts(results);
    }
}

// --- Consumer-Specific Shopping Cart Actions ---
void handleManageShoppingCart(Consumer* consumer, ProductManager& pm) {
    if (!consumer) { std::cout << "Error: Not logged in as consumer." << std::endl; return; }
    ShoppingCart* cart = consumer->getShoppingCart();

    while (true) {
        cart->displayCart();
        std::cout << "\n--- Shopping Cart Menu ---" << std::endl;
        std::cout << "1. Add Product to Cart" << std::endl;
        std::cout << "2. Update Product Quantity in Cart" << std::endl;
        std::cout << "3. Remove Product (or quantity) from Cart" << std::endl;
        std::cout << "4. Clear Entire Cart" << std::endl;
        std::cout << "0. Back to Consumer Menu" << std::endl;
        std::string choiceStr = getLineInput("Your choice: ");
        int choice = -1;
        try { choice = std::stoi(choiceStr); }
        catch (const std::exception&) { choice = -99; }

        switch (choice) {
        case 1: {
            handleDisplayAllProducts(pm, true); // Show products with details
            std::string pid = getLineInput("Enter Product ID to add: ");
            Product* p = pm.findProductByID(pid);
            if (!p) { std::cout << "Product not found." << std::endl; break; }
            if (p->getAvailableStock() == 0) { std::cout << "Product '" << p->getName() << "' is out of stock." << std::endl; break; }
            int qty = getValidatedInput<int>("Enter quantity: ");
            cart->addItem(p, qty);
            break;
        }
        case 2: {
            if (cart->isEmpty()) { std::cout << "Cart is empty." << std::endl; break; }
            std::string pid = getLineInput("Enter Product ID in cart to update: ");
            int newQty = getValidatedInput<int>("Enter new quantity (0 to remove): ");
            cart->updateItemQuantity(pid, newQty);
            break;
        }
        case 3: {
            if (cart->isEmpty()) { std::cout << "Cart is empty." << std::endl; break; }
            std::string pid = getLineInput("Enter Product ID in cart to remove/reduce: ");
            int qtyRemove = getValidatedInput<int>("Enter quantity to remove (enter current quantity to remove all): ");
            cart->removeItem(pid, qtyRemove);
            break;
        }
        case 4:
            cart->clearCart();
            break;
        case 0:
            return;
        default:
            std::cout << "Invalid choice." << std::endl;
        }
        // No persist for cart needed as it's in memory for the session
    }
}

// --- Consumer-Specific Order Actions ---
void handleCreateOrder(Consumer* consumer, ProductManager& pm, OrderManager& om) {
    if (!consumer) { std::cout << "Error: Not logged in." << std::endl; return; }
    if (consumer->getShoppingCart()->isEmpty()) {
        std::cout << "Your shopping cart is empty. Add items to create an order." << std::endl;
        return;
    }
    consumer->getShoppingCart()->displayCart();
    std::string confirm = getLineInput("Proceed to create order with these items? (y/n): ");
    if (confirm == "y" || confirm == "Y") {
        Order* newOrder = om.createOrderFromCart(consumer, pm);
        if (newOrder) {
            // newOrder->displayOrderDetails(); // Already displayed by createOrderFromCart on success
            // Cart is NOT cleared here, only on successful payment (Req 3)
        }
        else {
            std::cout << "Order creation failed. Please check product availability or cart." << std::endl;
        }
    }
    else {
        std::cout << "Order creation cancelled." << std::endl;
    }
}

void handleViewMyOrders(Consumer* consumer, OrderManager& om) {
    if (!consumer) { std::cout << "Error: Not logged in." << std::endl; return; }
    std::vector<Order*> orders = om.getOrdersByConsumer(consumer->getUsername());
    if (orders.empty()) {
        std::cout << "You have no orders." << std::endl;
        return;
    }
    std::cout << "\n--- Your Orders --- (Oldest first)" << std::endl;
    for (const auto* order : orders) {
        order->displayOrderDetails();
    }
}

void handlePayForOrder(Consumer* consumer, OrderManager& om, UserManager& um, ProductManager& pm) {
    if (!consumer) { std::cout << "Error: Not logged in." << std::endl; return; }

    std::vector<Order*> pendingOrders = om.getPendingOrdersByConsumer(consumer->getUsername());
    if (pendingOrders.empty()) {
        std::cout << "You have no orders pending payment." << std::endl;
        return;
    }

    std::cout << "\n--- Your Orders Pending Payment --- (Oldest first)" << std::endl;
    for (size_t i = 0; i < pendingOrders.size(); ++i) {
        std::cout << (i + 1) << ". ";
        pendingOrders[i]->displayOrderDetails(); // Display brief summary or full
    }

    std::string orderIdToPay = getLineInput("Enter Order ID to pay (or 0 to cancel): ");
    if (orderIdToPay == "0") return;

    Order* order = om.findOrderById(orderIdToPay);
    if (!order || order->getConsumerUsername() != consumer->getUsername()) {
        std::cout << "Order not found or does not belong to you." << std::endl;
        return;
    }
    if (order->getStatus() != OrderStatus::PendingPayment && order->getStatus() != OrderStatus::FailedPayment) {
        std::cout << "This order is not eligible for payment (Status: " << orderStatusToString(order->getStatus()) << ")." << std::endl;
        return;
    }

    // Re-check for prior unpaid orders immediately before payment attempt
    // (Already handled inside processPayment, but can be an early check here too)
    std::vector<Order*> allUserPending = om.getPendingOrdersByConsumer(consumer->getUsername());
    for (Order* po : allUserPending) {
        if (po->getID() != order->getID() && po->getCreationTime() < order->getCreationTime()) {
            std::cout << "You must pay or cancel older order " << po->getID() << " first." << std::endl;
            return;
        }
    }


    std::cout << "Order to Pay: " << std::endl;
    order->displayOrderDetails();
    std::cout << "Your current balance: $" << std::fixed << std::setprecision(2) << consumer->getBalance() << std::endl;
    if (consumer->getBalance() < order->getTotalAmount()) {
        std::cout << "Insufficient balance to pay for this order." << std::endl;
        return;
    }

    std::string pwd = getLineInput("Enter your password to confirm payment: ");
    if (om.processPayment(order, consumer, pwd, um, pm)) {
        // Success message handled by processPayment
        // Cart clearing for paid items handled by processPayment
    }
    else {
        // Failure message handled by processPayment
        std::cout << "Please check order status or try again." << std::endl;
    }
}

void handleCancelOrder(Consumer* consumer, OrderManager& om, ProductManager& pm) {
    if (!consumer) { std::cout << "Error: Not logged in." << std::endl; return; }

    std::vector<Order*> pendingOrders = om.getPendingOrdersByConsumer(consumer->getUsername());
    if (pendingOrders.empty()) {
        std::cout << "You have no orders pending payment to cancel." << std::endl;
        return;
    }

    std::cout << "\n--- Your Orders Eligible for Cancellation --- (Oldest first)" << std::endl;
    for (size_t i = 0; i < pendingOrders.size(); ++i) {
        std::cout << (i + 1) << ". ";
        // Only show a summary to avoid too much text
        std::cout << "Order ID: " << pendingOrders[i]->getID()
            << ", Status: " << orderStatusToString(pendingOrders[i]->getStatus())
            << ", Total: $" << std::fixed << std::setprecision(2) << pendingOrders[i]->getTotalAmount() << std::endl;
    }

    std::string orderIdToCancel = getLineInput("Enter Order ID to cancel (or 0 to go back): ");
    if (orderIdToCancel == "0") return;

    if (om.cancelOrder(orderIdToCancel, pm)) {
        // Success message handled by cancelOrder
    }
    else {
        // Failure message handled by cancelOrder
    }
}


// --- Merchant-Specific Actions (from Task 1, ensure ProductManager and User passed correctly) ---
void handleAddProduct(ProductManager& pm, User* merchantUser) {
    if (!merchantUser || merchantUser->getUserType() != "Merchant") {
        std::cout << "Error: Must be logged in as a Merchant." << std::endl; return;
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
    int stock = -1; // This is initialTotalStock
    while (stock < 0) {
        stock = getValidatedInput<int>("Enter product initial total stock quantity: ");
        if (stock < 0) std::cout << "Stock cannot be negative." << std::endl;
    }
    std::string productTypeChoice, productType;
    while (true) {
        std::cout << "Select product category:\n1. Book\n2. Food\n3. Clothing\nYour choice: ";
        std::getline(std::cin, productTypeChoice);
        if (productTypeChoice == "1") { productType = "Book"; break; }
        if (productTypeChoice == "2") { productType = "Food"; break; }
        if (productTypeChoice == "3") { productType = "Clothing"; break; }
        std::cout << "Invalid choice." << std::endl;
    }
    if (pm.addProduct(name, description, originalPrice, stock, merchant->getUsername(), productType)) {
        std::cout << "Product '" << name << "' added successfully." << std::endl;
    }
    else {
        std::cout << "Failed to add product." << std::endl;
    }
}

void handleManageMyProducts(ProductManager& pm, User* merchantUser) {
    if (!merchantUser || merchantUser->getUserType() != "Merchant") {
        std::cout << "Error: Must be logged in as a Merchant." << std::endl; return;
    }
    Merchant* merchant = static_cast<Merchant*>(merchantUser);
    std::cout << "\n--- Manage My Products ---" << std::endl;
    std::vector<Product*> myProducts = pm.getProductsByMerchant(merchant->getUsername());
    if (myProducts.empty()) {
        std::cout << "You have not added any products yet." << std::endl; return;
    }
    displayProducts(myProducts);
    std::string productID = getLineInput("Enter ID of product to manage (or 0 to cancel): ");
    if (productID == "0") return;
    Product* product = pm.findProductByID(productID);
    if (!product || product->getOwnerMerchantUsername() != merchant->getUsername()) {
        std::cout << "Product ID not found or you do not own this product." << std::endl; return;
    }
    std::cout << "\nManaging Product: " << product->getName() << " (ID: " << product->getID() << ")" << std::endl;
    std::cout << "1. Update Price (Original & Sale)" << std::endl;
    std::cout << "2. Update Total Stock" << std::endl; // Changed from Update Stock
    std::cout << "3. Set Discount (Update Sale Price for this specific product)" << std::endl;
    std::cout << "4. Remove Product" << std::endl;
    std::cout << "5. Update Description" << std::endl;
    std::cout << "6. Update Name" << std::endl;
    std::cout << "0. Back" << std::endl;
    std::string choiceStr = getLineInput("Your choice: ");
    int choice = -1;
    try { choice = std::stoi(choiceStr); }
    catch (const std::exception&) { std::cout << "Invalid input." << std::endl; return; }
    bool changed = false;
    switch (choice) {
    case 1: { /* Price update logic */
        double newOrigPrice = -1.0;
        while (newOrigPrice < 0) { newOrigPrice = getValidatedInput<double>("New original price: $"); if (newOrigPrice < 0) std::cout << "Invalid\n"; }
        product->setOriginalPrice(newOrigPrice);
        double newSalePrice = -1.0;
        while (newSalePrice < 0 || newSalePrice > newOrigPrice) { newSalePrice = getValidatedInput<double>("New sale price: $"); if (newSalePrice<0 || newSalePrice > newOrigPrice) std::cout << "Invalid\n"; }
        product->setCurrentSalePrice(newSalePrice);
        changed = true; break;
    }
    case 2: { // Update Total Stock
        int newTotalStock = -1;
        while (newTotalStock < product->getReservedStock()) { // Ensure new total stock is not less than reserved
            newTotalStock = getValidatedInput<int>("Enter new total stock quantity: ");
            if (newTotalStock < product->getReservedStock()) {
                std::cout << "Total stock cannot be less than currently reserved stock ("
                    << product->getReservedStock() << "). Please enter a higher value." << std::endl;
            }
            else if (newTotalStock < 0) {
                std::cout << "Stock cannot be negative.\n";
            }
        }
        product->setTotalStock(newTotalStock); // Use setTotalStock
        changed = true; break;
    }
    case 3: { /* Discount for specific product */
        double disc = -1.0;
        while (disc < 0 || disc > 100) { disc = getValidatedInput<double>("Discount % (0-100): "); if (disc < 0 || disc>100) std::cout << "Invalid\n"; }
        product->setCurrentSalePrice(product->getOriginalPrice() * (1.0 - (disc / 100.0)));
        changed = true; break;
    }
    case 4: { /* Remove product */
        std::string conf = getLineInput("Sure? (y/n):");
        if (conf == "y" || conf == "Y") { if (pm.removeProduct(product->getID(), merchant->getUsername())) { std::cout << "Removed.\n"; return; } else { std::cout << "Failed.\n"; } }
        break; // No changed = true as removeProduct saves
    }
    case 5: { product->setDescription(getLineInput("New description: ")); changed = true; break; }
    case 6: { product->setName(getLineInput("New name: ")); changed = true; break; }
    case 0: return;
    default: std::cout << "Invalid choice." << std::endl; break;
    }
    if (changed) {
        pm.updateProduct(product);
        std::cout << "Product details updated." << std::endl;
    }
}

void handleDiscountCategory(ProductManager& pm, User* merchantUser) {
    // This function remains largely the same from Task 1 update, uses pm.applyCategoryDiscount
    if (!merchantUser || merchantUser->getUserType() != "Merchant") {
        std::cout << "Error: Must be logged in as a Merchant." << std::endl; return;
    }
    Merchant* merchant = static_cast<Merchant*>(merchantUser);
    std::cout << "\n--- Discount Products by Category (for your products) ---" << std::endl;
    // ... (rest of the function is the same as in the previous version for Task 1) ...
    // Display merchant's categories and all platform categories
    std::vector<Product*> myProducts = pm.getProductsByMerchant(merchant->getUsername());
    std::set<std::string> myTypesSet;
    for (const auto* p : myProducts) myTypesSet.insert(p->getProductType());
    std::cout << "Your product categories: ";
    if (myTypesSet.empty()) std::cout << "None. "; else for (const auto& type : myTypesSet) std::cout << type << " ";
    std::cout << "\nAll platform categories: ";
    std::vector<std::string> allTypes = pm.getAvailableProductTypes();
    if (allTypes.empty()) std::cout << "None."; else for (const auto& type : allTypes) std::cout << type << " ";
    std::cout << std::endl;

    std::string categoryToDiscount = getLineInput("Enter product category to apply/update discount for: ");
    double discountPercent = -1.0;
    while (discountPercent < 0 || discountPercent > 100) {
        discountPercent = getValidatedInput<double>("Enter discount percentage (0-100, 0 to remove discount): ");
        if (discountPercent < 0 || discountPercent > 100) std::cout << "Discount must be between 0 and 100.\n";
    }
    pm.applyCategoryDiscount(merchant->getUsername(), categoryToDiscount, discountPercent);
}


// --- Menu Functions ---
void showMainMenu(UserManager& um, ProductManager& pm, OrderManager& om, User*& currentUser) { // Added OM
    std::string choiceStr; int choice = -1;
    while (true) {
        std::cout << "\n========= E-commerce Platform =========" << std::endl;
        if (currentUser) {
            std::cout << "Logged in as: " << currentUser->getUsername()
                << " (" << currentUser->getUserType()
                << ") | Balance: $" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
        }
        else { std::cout << "Welcome, Guest!" << std::endl; }
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
        try { choice = std::stoi(choiceStr); }
        catch (const std::exception&) { choice = -1; }

        switch (choice) {
        case 1: handleDisplayAllProducts(pm); break;
        case 2: handleSearchProducts(pm); break;
        case 3:
            if (!currentUser) handleRegister(um);
            else {
                if (currentUser->getUserType() == "Consumer") {
                    showConsumerMenu(um, pm, om, currentUser); // Pass OM
                }
                else if (currentUser->getUserType() == "Merchant") {
                    showMerchantMenu(um, pm, om, currentUser); // Pass OM
                }
            }
            break;
        case 4:
            if (!currentUser) handleLogin(um, currentUser);
            else handleLogout(currentUser, um, pm, om); // Pass PM, OM
            break;
        case 0:
            std::cout << "Exiting platform. Goodbye!" << std::endl;
            if (currentUser) um.persistChanges();
            pm.persistChanges();
            om.persistChanges(); // Persist orders
            return;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
}

void showConsumerMenu(UserManager& um, ProductManager& pm, OrderManager& om, User*& currentUser) { // Added OM
    if (!currentUser || currentUser->getUserType() != "Consumer") return;
    Consumer* consumer = static_cast<Consumer*>(currentUser);
    std::string choiceStr; int choice = -1;

    while (currentUser && currentUser->getUserType() == "Consumer") { // Check currentUser in loop for logout
        std::cout << "\n--- Consumer Menu (" << consumer->getUsername() << ") ---" << std::endl;
        std::cout << "Balance: $" << std::fixed << std::setprecision(2) << consumer->getBalance() << std::endl;
        std::cout << "1. View All Products" << std::endl;
        std::cout << "2. Search Products" << std::endl;
        std::cout << "3. Manage Shopping Cart" << std::endl;
        std::cout << "4. Create Order from Cart" << std::endl;
        std::cout << "5. View My Orders" << std::endl;
        std::cout << "6. Pay for Order" << std::endl;
        std::cout << "7. Cancel Pending Order" << std::endl;
        std::cout << "8. Manage Balance" << std::endl;
        std::cout << "9. Change Password" << std::endl;
        std::cout << "0. Back to Main Menu (Logout)" << std::endl;
        choiceStr = getLineInput("Your choice: ");
        try { choice = std::stoi(choiceStr); }
        catch (const std::exception&) { choice = -1; }

        // If currentUser becomes null (due to logout inside a handler), break
        if (!currentUser) break;

        switch (choice) {
        case 1: handleDisplayAllProducts(pm); break;
        case 2: handleSearchProducts(pm); break;
        case 3: handleManageShoppingCart(consumer, pm); break;
        case 4: handleCreateOrder(consumer, pm, om); break;
        case 5: handleViewMyOrders(consumer, om); break;
        case 6: handlePayForOrder(consumer, om, um, pm); break;
        case 7: handleCancelOrder(consumer, om, pm); break;
        case 8: handleBalanceManagement(um, currentUser); break;
        case 9: handleChangePassword(um, currentUser); break;
        case 0:
            handleLogout(currentUser, um, pm, om); // Logout will set currentUser to null
            return; // Exit consumer menu
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
}

void showMerchantMenu(UserManager& um, ProductManager& pm, OrderManager& om, User*& currentUser) { // Added OM
    std::string choiceStr; int choice = -1;
    while (currentUser && currentUser->getUserType() == "Merchant") { // Check currentUser
        std::cout << "\n--- Merchant Menu (" << currentUser->getUsername() << ") ---" << std::endl;
        std::cout << "Balance: $" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
        std::cout << "1. Add New Product" << std::endl;
        std::cout << "2. Manage My Products" << std::endl;
        std::cout << "3. Set/Update Discount by Category" << std::endl;
        std::cout << "4. View All Platform Products" << std::endl;
        std::cout << "5. Search Platform Products" << std::endl;
        std::cout << "6. Manage Balance" << std::endl;
        std::cout << "7. Change Password" << std::endl;
        std::cout << "0. Back to Main Menu (Logout)" << std::endl;
        choiceStr = getLineInput("Your choice: ");
        try { choice = std::stoi(choiceStr); }
        catch (const std::exception&) { choice = -1; }

        if (!currentUser) break; // Check again after input

        switch (choice) {
        case 1: handleAddProduct(pm, currentUser); break;
        case 2: handleManageMyProducts(pm, currentUser); break;
        case 3: handleDiscountCategory(pm, currentUser); break;
        case 4: handleDisplayAllProducts(pm); break;
        case 5: handleSearchProducts(pm); break;
        case 6: handleBalanceManagement(um, currentUser); break;
        case 7: handleChangePassword(um, currentUser); break;
        case 0:
            handleLogout(currentUser, um, pm, om);
            return; // Exit merchant menu
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
}

// --- Main Function ---
int main() {
    UserManager userManager;
    ProductManager productManager;
    OrderManager orderManager(productManager); // Initialize OrderManager, passing ProductManager for stock ops

    User* currentUser = nullptr;

    showMainMenu(userManager, productManager, orderManager, currentUser); // Pass OrderManager

    // PersistChanges is called at exit from main menu and on logout.
    return 0;
}

// Ensure the 'split' function is defined somewhere accessible.
// If it's in UserManager.cpp, and OrderManager.cpp also uses it via 'extern',
// ensure UserManager.cpp is compiled and linked.
// A common utility file is best for 'split'.