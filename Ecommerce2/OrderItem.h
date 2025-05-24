#pragma once
#ifndef ORDERITEM_H
#define ORDERITEM_H

#include <string>
#include <iostream>
#include <iomanip>

struct OrderItem {
    std::string productID;         // 商品ID
    std::string productName;       // 商品名称
    std::string merchantUsername;  // 商家用户名（用于归属正确的商家）
    double priceAtOrder;           // 下单时的价格（固定价格，不受后续折扣影响）
    int quantity;                  // 购买数量

    // 构造函数：初始化订单项信息
    OrderItem(std::string pid, std::string pName, std::string mUser, double price, int qty)
        : productID(pid), productName(pName), merchantUsername(mUser), priceAtOrder(price), quantity(qty) {
    }

    // 默认构造函数：用于向量调整大小或其他需要
    OrderItem() : productID(""), productName(""), merchantUsername(""), priceAtOrder(0.0), quantity(0) {}


    // 计算子总价（下单时价格 × 数量）
    double getSubtotal() const {
        return priceAtOrder * quantity;
    }

    // 显示订单项详情
    void display() const {
        std::cout << "    - 商品: " << productName << " (ID: " << productID << ")"
            << ", 数量: " << quantity
            << ", 支付单价: $" << std::fixed << std::setprecision(2) << priceAtOrder
            << ", 子总价: $" << getSubtotal()
            << " (销售商家: " << merchantUsername << ")" << std::endl;
    }
};

#endif // ORDERITEM_H