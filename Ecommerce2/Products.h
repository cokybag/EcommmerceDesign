#pragma once
#ifndef PRODUCTS_H
#define PRODUCTS_H

#include <string>
#include <iostream>
#include <fstream> // 用于serialize中的ostream

class Product {
protected:
    std::string productID;           // 产品ID
    std::string name;                // 产品名称
    std::string description;         // 产品描述
    double originalPrice;            // 原价
    double currentSalePrice;         // 当前销售价
    int totalStock;                  // 总库存（从'stock'重命名为'totalStock'以明确含义）
    int reservedStock;               // 新：为待处理订单预留的库存
    std::string ownerMerchantUsername; // 所属商家用户名
    std::string productType;         // 产品类型（如"Book", "Food", "Electronics", "SciFiBook"）

public:
    // 构造函数
    Product(std::string id, std::string n, std::string desc, double origP, double saleP, int initialStock, std::string owner, std::string type);
    virtual ~Product() = default;    // 虚析构函数

    virtual double getPrice() const; // 实际返回currentSalePrice

    // 获取器方法
    std::string getID() const;
    std::string getName() const;
    std::string getDescription() const;
    double getOriginalPrice() const;
    double getCurrentSalePrice() const;
    int getTotalStock() const;       // 获取总实物库存
    int getReservedStock() const;    // 获取为订单预留的库存
    int getAvailableStock() const;   // 获取可用库存（总库存 - 预留库存）
    std::string getOwnerMerchantUsername() const;
    std::string getProductType() const;

    // 设置器方法
    void setOriginalPrice(double price);
    void setCurrentSalePrice(double price);
    void setTotalStock(int newTotalStock); // 替换setStock
    void setDescription(const std::string& desc);
    void setName(const std::string& newName);

    // 订单库存预留方法
    bool reserveStock(int quantity);   // 成功返回true
    bool releaseReservedStock(int quantity); // 成功返回true
    bool confirmSaleFromReserved(int quantity); // 减少总库存和预留库存

    virtual void displayDetails() const; // 显示产品详情
    virtual void serialize(std::ostream& os) const; // 序列化为输出流
};

// 书籍类 - 继承自Product
class Book : public Product {
public:
    Book(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner);
    void displayDetails() const override; // 重写显示详情方法
    void serialize(std::ostream& os) const override; // 重写序列化方法
    // 如果getPrice只是返回currentSalePrice，则无需重写
};

// 食品类 - 继承自Product
class Food : public Product {
public:
    Food(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner);
    void displayDetails() const override; // 重写显示详情方法
    void serialize(std::ostream& os) const override; // 重写序列化方法
};

// 服装类 - 继承自Product
class Clothing : public Product {
public:
    Clothing(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner);
    void displayDetails() const override; // 重写显示详情方法
    void serialize(std::ostream& os) const override; // 重写序列化方法
};

#endif // PRODUCTS_H