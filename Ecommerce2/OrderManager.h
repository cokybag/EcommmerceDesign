#pragma once
#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H

#include <string> // For std::string
#include <vector> // For std::vector
// No need for <map> here

#include "Order.h" // Defines Order

// Forward declarations are good practice here
class Consumer;
class UserManager;
class ProductManager;
// class ShoppingCart; // Not directly in method signatures

class OrderManager {
private:
    std::vector<Order*> allOrders;
    const std::string ordersFilename = "orders.txt";
    int nextOrderIDCounter;

    std::string generateNewOrderID();
    // Pass ProductManager by reference for the load
    void loadOrdersFromFile(ProductManager& pm);

public:
    OrderManager(ProductManager& pm);
    ~OrderManager();

    Order* createOrderFromCart(Consumer* consumer, ProductManager& pm);
    bool cancelOrder(const std::string& orderID, ProductManager& pm);
    bool processPayment(Order* order, Consumer* consumer, const std::string& enteredPassword,
        UserManager& um, ProductManager& pm);

    Order* findOrderById(const std::string& orderID);
    std::vector<Order*> getOrdersByConsumer(const std::string& consumerUsername, bool pendingOnly = false);
    std::vector<Order*> getPendingOrdersByConsumer(const std::string& consumerUsername);

    void persistChanges() const;
};

#endif // ORDERMANAGER_H