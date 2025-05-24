#pragma once
#ifndef PRODUCTMANAGER_H
#define PRODUCTMANAGER_H

#include "Products.h"
#include <vector>
#include <string>
#include <algorithm> // 用于std::sort, std::remove_if
#include <map>       // 用于产品类型和类别折扣

class ProductManager {
private:
    std::vector<Product*> products;          // 存储所有产品的指针
    const std::string filename = "products.txt"; // 产品数据存储的文件名
    int nextProductID;                       // 用于生成唯一产品ID的计数器

    // 存储特定商家对某类产品设置的折扣百分比
    // 键：商家用户名，值：（键：类别类型，值：折扣百分比）的映射
    std::map<std::string, std::map<std::string, double>> activeCategoryDiscounts;

    void loadProductsFromFile();              // 从文件加载产品数据
    void saveProductsToFile() const;          // 将产品数据保存到文件
    std::string generateNewProductID();       // 生成新的唯一产品ID

public:
    ProductManager();                         // 构造函数
    ~ProductManager();                        // 析构函数

    // 添加新产品
    // 参数：名称、描述、原价、库存、所属商家用户名、产品类型
    bool addProduct(const std::string& name, const std::string& description, double originalPrice,
        int stock, const std::string& ownerMerchantUsername, const std::string& productType);

    Product* findProductByID(const std::string& id); // 根据ID查找产品
    std::vector<Product*> getAllProducts() const;    // 获取所有产品
    std::vector<Product*> getProductsByMerchant(const std::string& merchantUsername) const; // 获取特定商家的所有产品
    // 搜索产品（searchBy参数可选："name"按名称，"type"按类型，"merchant"按商家）
    std::vector<Product*> searchProducts(const std::string& searchTerm, const std::string& searchBy) const;

    // 删除产品（商家只能删除自己的产品）
    bool removeProduct(const std::string& productID, const std::string& merchantUsername);
    void updateProduct(Product* product);     // 更新产品信息并保存

    void persistChanges();                    // 显式将更改保存到文件

    std::vector<std::string> getAvailableProductTypes() const; // 获取所有可用的产品类型

    // 类别折扣管理新方法
    void applyCategoryDiscount(const std::string& merchantUsername, const std::string& categoryType, double discountPercentage); // 应用类别折扣
    double getActiveCategoryDiscount(const std::string& merchantUsername, const std::string& categoryType) const; // 获取活动类别折扣
};

#endif // PRODUCTMANAGER_H