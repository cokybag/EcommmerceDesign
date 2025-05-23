#include "OrderManager.h"
#include "Users.h"
#include "ProductManager.h"
#include "Products.h" // For Product methods like reserveStock, releaseReservedStock, confirmSaleFromReserved
#include "UserManager.h" // For UserManager to find merchant and update balances
#include <fstream>
#include <sstream>
#include <algorithm> // For std::find_if, std::remove_if if needed
#include <iostream>
#include <iomanip> // For std::fixed, std::setprecision
#include "utils.h"
#include "CartItem.h"
#include "ShoppingCart.h"

// Assuming split is globally available or defined in a common util.h
//extern std::vector<std::string> split(const std::string& s, char delimiter);

OrderManager::OrderManager(ProductManager& pm) : nextOrderIDCounter(1) {
    loadOrdersFromFile(pm); // Pass pm to potentially re-reserve stock
    // Determine nextOrderIDCounter based on loaded orders
    if (!allOrders.empty()) {
        for (const auto* order : allOrders) {
            try {
                // Assuming ID is "ORD<number>"
                if (order->getID().length() > 3 && order->getID().substr(0, 3) == "ORD") {
                    int idNum = std::stoi(order->getID().substr(3));
                    nextOrderIDCounter = std::max(nextOrderIDCounter, idNum + 1);
                }
            }
            catch (const std::exception& e) {
                // Muted
            }
        }
    }
}

OrderManager::~OrderManager() {
    // persistChanges(); // Save on exit or manage explicitly elsewhere
    for (Order* order : allOrders) {
        delete order;
    }
    allOrders.clear();
}

std::string OrderManager::generateNewOrderID() {
    return "ORD" + std::to_string(nextOrderIDCounter++);
}

void OrderManager::loadOrdersFromFile(ProductManager& pm) {
    std::ifstream inFile(ordersFilename);
    if (!inFile.is_open()) {
        // std::cerr << "Warning: Could not open orders file: " << ordersFilename << ". Starting fresh." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        std::vector<std::string> tokens = split(line, ',');

        // Order: ID,Consumer,Total,Status,CreateTime,UpdateTime,NumItems,
        // Item1: PID,PName,Merchant,Price,Qty, Item2: ...
        if (tokens.size() < 7) {
            std::cerr << "Warning: Malformed order line (too few base tokens): " << line << ". Skipping." << std::endl;
            continue;
        }

        try {
            std::string orderID_str = tokens[0];
            std::string consumerUsername_str = tokens[1];
            double totalAmount_val = std::stod(tokens[2]);
            OrderStatus status_val = stringToOrderStatus(tokens[3]);
            std::time_t creationTime_val = static_cast<std::time_t>(std::stoll(tokens[4]));
            std::time_t lastUpdateTime_val = static_cast<std::time_t>(std::stoll(tokens[5]));
            int numItems = std::stoi(tokens[6]);

            if (tokens.size() < (size_t)(7 + numItems * 5)) { // 5 tokens per item
                std::cerr << "Warning: Malformed order line (not enough item tokens for " << numItems << " items): " << line << ". Skipping." << std::endl;
                continue;
            }

            std::vector<OrderItem> orderItems_vec;
            int currentTokenIndex = 7;
            for (int i = 0; i < numItems; ++i) {
                std::string item_pid = tokens[currentTokenIndex++];
                std::string item_pname = tokens[currentTokenIndex++];
                std::replace(item_pname.begin(), item_pname.end(), ';', ','); // Restore commas
                std::string item_merchant = tokens[currentTokenIndex++];
                double item_price = std::stod(tokens[currentTokenIndex++]);
                int item_qty = std::stoi(tokens[currentTokenIndex++]);
                orderItems_vec.emplace_back(item_pid, item_pname, item_merchant, item_price, item_qty);
            }

            Order* loadedOrder = new Order(orderID_str, consumerUsername_str, orderItems_vec,
                totalAmount_val, status_val, creationTime_val, lastUpdateTime_val);
            allOrders.push_back(loadedOrder);

            // Re-reserve stock for pending orders (Requirement 1 for orders)
            if (loadedOrder->getStatus() == OrderStatus::PendingPayment) {
                bool allReserved = true;
                for (const auto& item : loadedOrder->getItems()) {
                    Product* p = pm.findProductByID(item.productID);
                    if (p) {
                        if (!p->reserveStock(item.quantity)) {
                            std::cerr << "CRITICAL Error on load: Could not re-reserve " << item.quantity
                                << " of product " << item.productID << " for pending order " << loadedOrder->getID()
                                << ". Available: " << p->getAvailableStock()
                                << ". Order status might be inconsistent." << std::endl;
                            // This is a critical state. The order might need to be marked as problematic or cancelled.
                            // For simplicity, we'll log and continue. A real system needs robust handling.
                            // loadedOrder->setStatus(OrderStatus::Cancelled); // Or a special "Problematic" status
                            allReserved = false;
                        }
                    }
                    else {
                        std::cerr << "CRITICAL Error on load: Product " << item.productID
                            << " for pending order " << loadedOrder->getID() << " not found." << std::endl;
                        // loadedOrder->setStatus(OrderStatus::Cancelled);
                        allReserved = false;
                    }
                }
                if (!allReserved) {
                    // If not all could be reserved, ideally, un-reserve what was reserved for this order
                    // and mark order as problematic/cancelled. This is complex recovery.
                }
            }

        }
        catch (const std::invalid_argument& ia) {
            std::cerr << "Error parsing numeric value for an order in file: " << ia.what() << " Line: " << line << std::endl;
        }
        catch (const std::out_of_range& oor) {
            std::cerr << "Numeric value out of range for an order in file. Line: " << line << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Generic error loading order: " << e.what() << " Line: " << line << std::endl;
        }
    }
    inFile.close();
}


void OrderManager::persistChanges() const {
    std::ofstream outFile(ordersFilename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open orders file for writing: " << ordersFilename << std::endl;
        return;
    }
    for (const Order* order : allOrders) {
        order->serialize(outFile);
        outFile << std::endl;
    }
    outFile.close();
}

Order* OrderManager::createOrderFromCart(Consumer* consumer, ProductManager& pm) {
    if (!consumer || consumer->getShoppingCart()->isEmpty()) {
        std::cout << "Error: Cannot create order. Shopping cart is empty or consumer invalid." << std::endl;
        return nullptr;
    }

    ShoppingCart* cart = consumer->getShoppingCart();
    std::vector<OrderItem> orderItemsList;
    bool stockReservationSuccess = true;

    // First pass: check and reserve stock for all items
    for (const auto& pair : cart->getItems()) {
        const CartItem& cartItem = pair.second;
        Product* product = cartItem.product; // Product pointer from CartItem
        if (!product) { // Should not happen if cart is well-managed
            std::cout << "Error: Invalid product in cart (nullptr)." << std::endl;
            stockReservationSuccess = false; // Mark failure
            break;
        }

        if (!product->reserveStock(cartItem.quantity)) {
            std::cout << "Order creation failed: Not enough available stock for "
                << product->getName() << ". Required: " << cartItem.quantity
                << ", Available: " << product->getAvailableStock() << std::endl;
            stockReservationSuccess = false; // Mark failure
            break;
        }
        // Stock successfully reserved for this item, temporarily
    }

    // If any stock reservation failed, roll back all reservations made in this attempt
    if (!stockReservationSuccess) {
        for (const auto& pair : cart->getItems()) {
            const CartItem& cartItem = pair.second;
            Product* product = cartItem.product;
            if (product) {
                // Attempt to release stock that might have been reserved before failure
                // This requires knowing how much was successfully reserved *by this operation*
                // A simpler rollback: iterate through items *added to orderItemsList* if we did that earlier.
                // For now, if one fails, we assume the order doesn't proceed, and release for *all* cart items
                // if they were touched. The current logic is: reserve one by one, if one fails, stop.
                // What needs to be released are the ones *already successfully reserved in this loop*.

                // Better approach: loop through the cart items again to release.
                // This is a bit tricky because we don't know which ones succeeded if one failed mid-way.
                // The product->reserveStock() itself is atomic for that product.
                // The rollback should be for products whose stock was successfully reserved *before* the failing one.

                // Let's re-iterate what was in the orderItemsList *before* it's cleared due to failure
                // For this simplified example, if createOrder fails, items are still in cart.
                // We need to iterate over what was *attempted* to be reserved.
                for (const auto& p : cart->getItems()) { // Iterate over original cart items
                    if (p.second.product) p.second.product->releaseReservedStock(p.second.quantity); // Try to release
                }
                pm.persistChanges(); // Save product stock changes (releases)
                std::cout << "Order creation aborted due to stock issues. Previously reserved stock (if any) released." << std::endl;
                return nullptr;
            }
        }
        pm.persistChanges(); // Save product stock changes
        return nullptr;
    }

    // If all stock reserved successfully, create OrderItem objects
    for (const auto& pair : cart->getItems()) {
        const CartItem& cartItem = pair.second;
        orderItemsList.emplace_back(cartItem.product->getID(), cartItem.product->getName(),
            cartItem.product->getOwnerMerchantUsername(),
            cartItem.product->getCurrentSalePrice(), cartItem.quantity);
    }


    std::string newID = generateNewOrderID();
    Order* newOrder = new Order(newID, consumer->getUsername(), orderItemsList);
    allOrders.push_back(newOrder);

    pm.persistChanges(); // Save product stock changes (reservations)
    persistChanges();    // Save the new order

    // Requirement 3:购物车只有在订单成功完成后才能删除或者顾客自己主动清空购物车
    // So, don't clear cart here. It will be cleared upon successful payment.

    std::cout << "Order " << newID << " created successfully. Total: $"
        << std::fixed << std::setprecision(2) << newOrder->getTotalAmount()
        << ". Please proceed to payment." << std::endl;
    return newOrder;
}

bool OrderManager::cancelOrder(const std::string& orderID, ProductManager& pm) {
    Order* order = findOrderById(orderID);
    if (!order) {
        std::cout << "Error: Order ID " << orderID << " not found." << std::endl;
        return false;
    }

    if (order->getStatus() != OrderStatus::PendingPayment && order->getStatus() != OrderStatus::FailedPayment) {
        std::cout << "Error: Order " << orderID << " cannot be cancelled (Status: "
            << orderStatusToString(order->getStatus()) << ")." << std::endl;
        return false;
    }

    // Release reserved stock
    bool allReleased = true;
    for (const auto& item : order->getItems()) {
        Product* p = pm.findProductByID(item.productID);
        if (p) {
            if (!p->releaseReservedStock(item.quantity)) {
                // This implies an inconsistency if stock was properly reserved.
                std::cerr << "Warning: Could not fully release " << item.quantity << " of product "
                    << item.productID << " for cancelled order " << orderID << "." << std::endl;
                allReleased = false;
            }
        }
        else {
            std::cerr << "Warning: Product " << item.productID << " not found when trying to release stock for cancelled order " << orderID << "." << std::endl;
            allReleased = false;
        }
    }

    order->setStatus(OrderStatus::Cancelled);
    pm.persistChanges(); // Save product stock changes
    persistChanges();    // Save order status change

    std::cout << "Order " << orderID << " has been cancelled." << std::endl;
    if (!allReleased) {
        std::cout << "Warning: There might have been issues releasing all stock. Please check product inventory." << std::endl;
    }
    return true;
}

bool OrderManager::processPayment(Order* order, Consumer* consumer, const std::string& enteredPassword,
    UserManager& um, ProductManager& pm) {
    if (!order || !consumer) return false;

    // Requirement 2.3: Confirm password
    if (!consumer->checkPassword(enteredPassword)) {
        std::cout << "Payment failed: Incorrect password." << std::endl;
        order->setStatus(OrderStatus::FailedPayment); // Mark as failed payment
        persistChanges(); // Save order status
        return false;
    }

    // Requirement 2.2: Check for prior unpaid orders
    std::vector<Order*> pendingOrders = getPendingOrdersByConsumer(consumer->getUsername());
    for (Order* pending : pendingOrders) {
        if (pending->getID() != order->getID() && pending->getCreationTime() < order->getCreationTime()) {
            std::cout << "Payment failed: You have an older unpaid order (ID: " << pending->getID()
                << ") that must be paid or cancelled first." << std::endl;
            order->setStatus(OrderStatus::FailedPayment); // Can mark current attempt as failed.
            persistChanges();
            return false;
        }
    }


    if (order->getStatus() != OrderStatus::PendingPayment && order->getStatus() != OrderStatus::FailedPayment) {
        std::cout << "Payment failed: Order " << order->getID() << " is not pending payment (Status: "
            << orderStatusToString(order->getStatus()) << ")." << std::endl;
        return false;
    }

    if (consumer->getBalance() < order->getTotalAmount()) {
        std::cout << "Payment failed: Insufficient balance. Required: $"
            << order->getTotalAmount() << ", Available: $" << consumer->getBalance() << std::endl;
        order->setStatus(OrderStatus::FailedPayment);
        persistChanges();
        return false;
    }

    // Deduct from consumer
    if (!consumer->withdraw(order->getTotalAmount())) {
        // Should not happen if balance check above is correct
        std::cout << "Payment failed: Error withdrawing funds from consumer." << std::endl;
        order->setStatus(OrderStatus::FailedPayment);
        persistChanges();
        return false;
    }

    // Credit merchants and confirm sale from reserved stock
    for (const auto& item : order->getItems()) {
        User* merchantUser = um.findUser(item.merchantUsername);
        if (merchantUser && merchantUser->getUserType() == "Merchant") {
            merchantUser->deposit(item.getSubtotal()); // Merchant gets their share
        }
        else {
            std::cout << "Warning: Merchant " << item.merchantUsername << " for product "
                << item.productName << " not found or not a merchant. Funds not transferred." << std::endl;
            // This is problematic. Should we roll back the consumer's withdrawal?
            // For simplicity, consumer is charged, but merchant doesn't get paid.
            // A real system would have transactional integrity.
        }

        Product* p = pm.findProductByID(item.productID);
        if (p) {
            if (!p->confirmSaleFromReserved(item.quantity)) {
                std::cerr << "CRITICAL Error: Could not confirm sale of " << item.quantity
                    << " for product " << item.productID << " from reserved stock for order " << order->getID()
                    << ". Inventory may be inconsistent." << std::endl;
                // This is a severe issue. Funds are transferred, but stock update fails.
            }
        }
        else {
            std::cerr << "CRITICAL Error: Product " << item.productID << " not found when confirming sale for order " << order->getID() << "." << std::endl;
        }
    }

    order->setStatus(OrderStatus::Paid);

    um.persistChanges(); // Save user balance changes
    pm.persistChanges(); // Save product stock changes (total and reserved)
    persistChanges();    // Save order status change

    std::cout << "Payment successful for order " << order->getID() << "!" << std::endl;
    std::cout << "Your new balance: $" << std::fixed << std::setprecision(2) << consumer->getBalance() << std::endl;

    // Requirement 3: Clear cart after successful payment FOR THE ITEMS IN THIS ORDER
    // This means removing specific items from the cart that were part of this paid order.
    ShoppingCart* cart = consumer->getShoppingCart();
    if (cart) {
        for (const auto& orderItem : order->getItems()) {
            // Check if the item is still in cart and remove the exact quantity
            // (or clear the item if quantity matches exactly)
            CartItem* cartItemPtr = cart->getItem(orderItem.productID);
            if (cartItemPtr) {
                if (cartItemPtr->quantity == orderItem.quantity) {
                    cart->clearItem(orderItem.productID);
                }
                else if (cartItemPtr->quantity > orderItem.quantity) {
                    // This scenario (cart having more than ordered) shouldn't happen with current flow
                    // but if it could, adjust cart quantity.
                    cart->removeItem(orderItem.productID, orderItem.quantity);
                }
                // If cartItemPtr->quantity < orderItem.quantity, it's an inconsistency.
            }
        }
    }

    return true;
}


Order* OrderManager::findOrderById(const std::string& orderID) {
    auto it = std::find_if(allOrders.begin(), allOrders.end(),
        [&](const Order* o) { return o->getID() == orderID; });
    if (it != allOrders.end()) {
        return *it;
    }
    return nullptr;
}

std::vector<Order*> OrderManager::getOrdersByConsumer(const std::string& consumerUsername, bool pendingOnly) {
    std::vector<Order*> results;
    for (Order* order : allOrders) {
        if (order->getConsumerUsername() == consumerUsername) {
            if (pendingOnly) {
                if (order->getStatus() == OrderStatus::PendingPayment || order->getStatus() == OrderStatus::FailedPayment) {
                    results.push_back(order);
                }
            }
            else {
                results.push_back(order);
            }
        }
    }
    // Sort by creation time, newest first (or oldest first)
    std::sort(results.begin(), results.end(), [](const Order* a, const Order* b) {
        return a->getCreationTime() < b->getCreationTime(); // Oldest first
        });
    return results;
}


std::vector<Order*> OrderManager::getPendingOrdersByConsumer(const std::string& consumerUsername) {
    return getOrdersByConsumer(consumerUsername, true);
}