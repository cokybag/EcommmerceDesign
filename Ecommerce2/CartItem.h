#pragma once
#ifndef CARTITEM_H
#define CARTITEM_H

#include "Products.h" // 假设Product类定义在此处
#include <string>
#include <iomanip>

// 若Product.h内容较多，可前置声明
// class Product; 

struct CartItem {
    Product* product;    // 指向实际商品的指针
    int quantity;        // 购物车中的数量

    // 构造函数：初始化商品指针和数量
    CartItem(Product* p, int qty) : product(p), quantity(qty) {}

    // 默认构造函数：供map使用（如有需要）
    CartItem() : product(nullptr), quantity(0) {}

    // 计算子总价（商品当前售价 × 数量）
    double getSubtotal() const {
        if (product) {
            return product->getCurrentSalePrice() * quantity;
        }
        return 0.0;
    }

    // 显示购物车条目详情
    void display() const {
        if (product) {
            std::cout << "  - 商品: " << product->getName()
                << " (ID: " << product->getID() << ")"
                << ", 数量: " << quantity
                << ", 单价: $" << std::fixed << std::setprecision(2) << product->getCurrentSalePrice()
                << ", 子总价: $" << getSubtotal() << std::endl;
        }
    }
};

#endif // CARTITEM_H