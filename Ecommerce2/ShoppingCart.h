#pragma once
#ifndef SHOPPINGCART_H
#define SHOPPINGCART_H

#include "CartItem.h"
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <iomanip>

class ShoppingCart {
private:
    // 使用产品ID作为键以便于查找和更新
    std::map<std::string, CartItem> items;

public:
    ShoppingCart();

    // 添加一定数量的产品。如果产品已在购物车中，更新数量。
    // 成功返回true，库存不足返回false。
    bool addItem(Product* product, int quantity);

    // 移除一定数量的产品。如果要移除的数量 >= 当前数量，移除该条目。
    // 成功返回true。
    bool removeItem(const std::string& productID, int quantityToRemove);

    // 更新现有条目的数量。
    // 成功返回true，新数量所需库存不足返回false。
    bool updateItemQuantity(const std::string& productID, int newQuantity);

    // 从购物车中完全移除某个条目。
    void clearItem(const std::string& productID);

    void clearCart(); // 清空购物车

    const std::map<std::string, CartItem>& getItems() const; // 获取购物车条目（常量引用）
    CartItem* getItem(const std::string& productID); // 查找条目，未找到返回nullptr

    double getTotalValue() const; // 计算购物车总价值
    bool isEmpty() const; // 判断购物车是否为空
    void displayCart() const; // 显示购物车详情
};

#endif // SHOPPINGCART_H