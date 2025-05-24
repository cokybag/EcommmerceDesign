#pragma once
#ifndef ORDER_H
#define ORDER_H

#include "OrderItem.h"
#include <vector>
#include <string>
#include <ctime>    // 用于std::time_t时间类型
#include <fstream>  // 用于序列化操作
#include <sstream>  // 用于序列化字符串流

// 订单状态枚举
enum class OrderStatus {
    PendingPayment,  // 待付款
    Paid,            // 已付款
    Cancelled,       // 已取消
    FailedPayment    // 付款失败
    // 在控制台应用中为简化起见，暂不包含已过期状态
};

// 将订单状态转换为字符串
std::string orderStatusToString(OrderStatus status);
// 将字符串转换为订单状态
OrderStatus stringToOrderStatus(const std::string& statusStr);


class Order {
private:
    std::string orderID;           // 订单ID（唯一标识）
    std::string consumerUsername;  // 消费者用户名
    std::vector<OrderItem> items;  // 订单项列表
    double totalAmount;            // 订单总金额
    OrderStatus status;            // 订单状态
    std::time_t creationTime;      // 创建时间（时间戳）
    std::time_t lastUpdateTime;    // 最后更新时间（如付款时间、取消时间等）

public:
    // 常规构造函数（创建新订单时使用）
    Order(std::string id, std::string cUsername, const std::vector<OrderItem>& oItems);
    // 文件加载构造函数（反序列化时使用）
    Order(std::string id, std::string cUsername, const std::vector<OrderItem>& oItems,
        double total, OrderStatus stat, std::time_t cTime, std::time_t uTime);


    // 获取器（Getters）
    std::string getID() const { return orderID; }             // 获取订单ID
    std::string getConsumerUsername() const { return consumerUsername; } // 获取消费者用户名
    const std::vector<OrderItem>& getItems() const { return items; } // 获取订单项列表（常量引用）
    double getTotalAmount() const { return totalAmount; }     // 获取订单总金额
    OrderStatus getStatus() const { return status; }          // 获取订单状态
    std::time_t getCreationTime() const { return creationTime; } // 获取创建时间
    std::time_t getLastUpdateTime() const { return lastUpdateTime; } // 获取最后更新时间


    // 设置订单状态
    void setStatus(OrderStatus newStatus);
    // 重新计算订单总金额（基于订单项，在订单项变更时使用，尽管创建后通常不变）
    void calculateTotalAmount();
    // 显示订单详情
    void displayOrderDetails() const;

    // 序列化订单到输出流（用于存储到orders.txt，格式为单行文本）
    void serialize(std::ostream& os) const;
    // 反序列化通常由OrderManager处理（不在此类中实现）
};

#endif // ORDER_H