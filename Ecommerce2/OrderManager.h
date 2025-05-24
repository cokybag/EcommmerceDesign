#pragma once
#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H

#include <string> // 用于std::string
#include <vector> // 用于std::vector
// 此处无需引入<map>

#include "Order.h" // 定义Order类

// 前置声明（良好的编程实践）
class Consumer;
class UserManager;
class ProductManager;
// class ShoppingCart; // 未在方法签名中直接使用

class OrderManager {
private:
    std::vector<Order*> allOrders;          // 存储所有订单的指针列表
    const std::string ordersFilename = "orders.txt"; // 订单数据存储文件名（默认"orders.txt"）
    int nextOrderIDCounter;                 // 用于生成唯一订单ID的计数器

    std::string generateNewOrderID();       // 生成唯一订单ID（私有方法）
    // 通过引用传递ProductManager以加载订单（需解析商品信息）
    void loadOrdersFromFile(ProductManager& pm);

public:
    OrderManager(ProductManager& pm);       // 构造函数（初始化时加载订单文件）
    ~OrderManager();                        // 析构函数（释放内存）

    // 从购物车创建订单（消费者、商品管理器作为参数）
    Order* createOrderFromCart(Consumer* consumer, ProductManager& pm);

    // 取消订单（需订单ID，调用商品管理器更新库存）
    bool cancelOrder(const std::string& orderID, ProductManager& pm);

    // 处理支付（订单、消费者、输入密码、用户管理器、商品管理器）
    bool processPayment(Order* order, Consumer* consumer, const std::string& enteredPassword,
        UserManager& um, ProductManager& pm);

    // 根据订单ID查找订单（返回指针，未找到返回nullptr）
    Order* findOrderById(const std::string& orderID);

    // 获取消费者的订单列表（可选是否仅获取待付款订单）
    std::vector<Order*> getOrdersByConsumer(const std::string& consumerUsername, bool pendingOnly = false);

    // 获取消费者的待付款订单列表（快捷方法）
    std::vector<Order*> getPendingOrdersByConsumer(const std::string& consumerUsername);

    // 持久化订单数据到文件（常量成员函数）
    void persistChanges() const;
};

#endif // ORDERMANAGER_H