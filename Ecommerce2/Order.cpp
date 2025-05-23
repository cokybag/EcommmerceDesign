#define   _CRT_SECURE_NO_WARNINGS

#include "Order.h"
#include <iostream>
#include <iomanip> // For std::fixed, std::setprecision
#include <algorithm> // For std::replace in serialization if needed

// Helper function to convert OrderStatus to string
std::string orderStatusToString(OrderStatus status) {
    switch (status) {
    case OrderStatus::PendingPayment: return "PendingPayment";
    case OrderStatus::Paid: return "Paid";
    case OrderStatus::Cancelled: return "Cancelled";
    case OrderStatus::FailedPayment: return "FailedPayment";
    default: return "Unknown";
    }
}

// Helper function to convert string to OrderStatus
OrderStatus stringToOrderStatus(const std::string& statusStr) {
    if (statusStr == "PendingPayment") return OrderStatus::PendingPayment;
    if (statusStr == "Paid") return OrderStatus::Paid;
    if (statusStr == "Cancelled") return OrderStatus::Cancelled;
    if (statusStr == "FailedPayment") return OrderStatus::FailedPayment;
    // Potentially throw an error or return a default
    std::cerr << "Warning: Unknown order status string '" << statusStr << "'. Defaulting to FailedPayment." << std::endl;
    return OrderStatus::FailedPayment;
}


Order::Order(std::string id, std::string cUsername, const std::vector<OrderItem>& oItems)
    : orderID(id), consumerUsername(cUsername), items(oItems), status(OrderStatus::PendingPayment) {
    creationTime = std::time(nullptr);
    lastUpdateTime = creationTime;
    calculateTotalAmount();
}

// Constructor for loading from file
Order::Order(std::string id, std::string cUsername, const std::vector<OrderItem>& oItems,
    double total, OrderStatus stat, std::time_t cTime, std::time_t uTime)
    : orderID(id), consumerUsername(cUsername), items(oItems), totalAmount(total),
    status(stat), creationTime(cTime), lastUpdateTime(uTime) {
    // Total amount is passed directly
}


void Order::setStatus(OrderStatus newStatus) {
    status = newStatus;
    lastUpdateTime = std::time(nullptr);
}

void Order::calculateTotalAmount() {
    totalAmount = 0.0;
    for (const auto& item : items) {
        totalAmount += item.getSubtotal();
    }
}

void Order::displayOrderDetails() const {
    char timeBuffer[80];
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&creationTime));
    std::string creationTimeStr(timeBuffer);

    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&lastUpdateTime));
    std::string updateTimeStr(timeBuffer);

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Order ID: " << orderID << std::endl;
    std::cout << "Consumer: " << consumerUsername << std::endl;
    std::cout << "Status: " << orderStatusToString(status) << std::endl;
    std::cout << "Created: " << creationTimeStr << std::endl;
    std::cout << "Last Updated: " << updateTimeStr << std::endl;
    std::cout << "Total Amount: $" << std::fixed << std::setprecision(2) << totalAmount << std::endl;
    std::cout << "Items (" << items.size() << "):" << std::endl;
    for (const auto& item : items) {
        item.display();
    }
    std::cout << "----------------------------------------" << std::endl;
}

void Order::serialize(std::ostream& os) const {
    os << orderID << ","
        << consumerUsername << ","
        << totalAmount << ","
        << orderStatusToString(status) << ","
        << creationTime << "," // Store time_t as long
        << lastUpdateTime << "," // Store time_t as long
        << items.size(); // Number of items

    for (const auto& item : items) {
        // Replace commas in productName to avoid breaking CSV format for items
        std::string safeProductName = item.productName;
        std::replace(safeProductName.begin(), safeProductName.end(), ',', ';');

        os << "," << item.productID
            << "," << safeProductName
            << "," << item.merchantUsername
            << "," << item.priceAtOrder
            << "," << item.quantity;
    }
}