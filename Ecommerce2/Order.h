#pragma once
#ifndef ORDER_H
#define ORDER_H

#include "OrderItem.h"
#include <vector>
#include <string>
#include <ctime>    // For std::time_t
#include <fstream>  // For serialization
#include <sstream>  // For serialization

enum class OrderStatus {
    PendingPayment,
    Paid,
    Cancelled,
    FailedPayment
    // No Expired for simplicity in console app
};

std::string orderStatusToString(OrderStatus status);
OrderStatus stringToOrderStatus(const std::string& statusStr);


class Order {
private:
    std::string orderID;
    std::string consumerUsername;
    std::vector<OrderItem> items;
    double totalAmount;
    OrderStatus status;
    std::time_t creationTime;
    std::time_t lastUpdateTime; // Could be payment time, cancellation time etc.

public:
    Order(std::string id, std::string cUsername, const std::vector<OrderItem>& oItems);
    // Constructor for loading from file
    Order(std::string id, std::string cUsername, const std::vector<OrderItem>& oItems,
        double total, OrderStatus stat, std::time_t cTime, std::time_t uTime);


    std::string getID() const { return orderID; }
    std::string getConsumerUsername() const { return consumerUsername; }
    const std::vector<OrderItem>& getItems() const { return items; }
    double getTotalAmount() const { return totalAmount; }
    OrderStatus getStatus() const { return status; }
    std::time_t getCreationTime() const { return creationTime; }
    std::time_t getLastUpdateTime() const { return lastUpdateTime; }


    void setStatus(OrderStatus newStatus);
    void calculateTotalAmount(); // Recalculates based on items, useful if items change (though unlikely post-creation)
    void displayOrderDetails() const;

    // Serialization (to a single line for orders.txt)
    void serialize(std::ostream& os) const;
    // Deserialization is typically handled by OrderManager
};

#endif // ORDER_H