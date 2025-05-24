#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>
#include <fstream>
#include <iostream> // 用于潜在的调试/显示功能

class Product {
protected:
    std::string productID;          // 产品ID（唯一标识）
    std::string name;               // 产品名称
    std::string description;        // 产品描述
    double originalPrice;           // 原价
    double currentSalePrice;        // 商品折扣价（当前售价）
    int stock;                      // 库存数量
    std::string ownerMerchantUsername; // 所属商家用户名
    std::string productType;        // 产品类型（如"Book", "Food", "Clothing"）

public:
    // 构造函数
    Product(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner, std::string type);
    virtual ~Product() = default;   // 虚析构函数（支持多态）

    // 获取价格的虚函数（返回当前售价）
    virtual double getPrice() const;

    // 获取器（Getters）
    std::string getID() const;              // 获取产品ID
    std::string getName() const;            // 获取产品名称
    std::string getDescription() const;     // 获取产品描述
    double getOriginalPrice() const;        // 获取原价
    double getCurrentSalePrice() const;     // 获取当前售价（折扣价）
    int getStock() const;                   // 获取库存数量
    std::string getOwnerMerchantUsername() const; // 获取所属商家用户名
    std::string getProductType() const;     // 获取产品类型

    // 设置器（Setters，主要供商家使用）
    void setOriginalPrice(double price);     // 设置原价（可能触发售价调整）
    void setCurrentSalePrice(double price);  // 设置当前售价（用于折扣管理）
    void setStock(int newStock);             // 设置库存数量
    void setDescription(const std::string& desc); // 更新产品描述
    void setName(const std::string& newName); // 更新产品名称

    // 显示产品详情的虚函数（供子类重写）
    virtual void displayDetails() const;

    // 文件存储相关：序列化到输出流（供子类重写）
    virtual void serialize(std::ostream& os) const;
    // 反序列化由ProductManager处理（不在基类中实现）
};

// 示例子类：书籍类
class Book : public Product {
public:
    // 构造函数（调用基类构造函数并指定类型为"Book"）
    Book(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner);
    // 继承getPrice()函数（若逻辑与基类相同可无需重写）
    void displayDetails() const override; // 重写显示详情方法
    void serialize(std::ostream& os) const override; // 重写序列化方法
};

// 示例子类：食品类
class Food : public Product {
public:
    Food(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner);
    void displayDetails() const override;
    void serialize(std::ostream& os) const override;
};

// 示例子类：服装类
class Clothing : public Product {
public:
    Clothing(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner);
    void displayDetails() const override;
    void serialize(std::ostream& os) const override;
};


#endif // PRODUCT_H