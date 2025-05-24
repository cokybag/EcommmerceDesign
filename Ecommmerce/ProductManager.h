#ifndef PRODUCTMANAGER_H
#define PRODUCTMANAGER_H

#include "Products.h"
#include <vector>
#include <string>
#include <algorithm> // 用于std::sort, std::remove_if
#include <map>       // 用于存储产品类型和类别折扣

class ProductManager {
private:
    std::vector<Product*> products;          // 存储所有产品的指针列表
    const std::string filename = "products.txt"; // 存储产品数据的文件名（默认"products.txt"）
    int nextProductID;                        // 用于生成唯一产品ID的计数器

    // 存储商家类别折扣规则
    // 键：商家用户名，值：（键：类别类型，值：折扣百分比）的映射
    std::map<std::string, std::map<std::string, double>> activeCategoryDiscounts;

    void loadProductsFromFile();              // 从文件加载产品数据（私有方法）
    void saveProductsToFile() const;          // 将产品数据保存到文件（私有方法）
    std::string generateNewProductID();       // 生成唯一产品ID（私有方法）

public:
    ProductManager();                         // 构造函数（初始化时加载文件数据）
    ~ProductManager();                        // 析构函数（释放内存）

    // 添加新产品
    // 参数：名称、描述、原价、库存、所属商家用户名、产品类型（Book/Food/Clothing）
    bool addProduct(const std::string& name, const std::string& description, double originalPrice,
        int stock, const std::string& ownerMerchantUsername, const std::string& productType);

    Product* findProductByID(const std::string& id); // 根据ID查找产品（返回指针，未找到返回nullptr）
    std::vector<Product*> getAllProducts() const;    // 获取所有产品列表（副本）
    std::vector<Product*> getProductsByMerchant(const std::string& merchantUsername) const; // 获取指定商家的产品列表

    // 搜索产品（支持按名称、类型、商家搜索）
    // searchBy参数可选值："name"（名称）、"type"（类型）、"merchant"（商家）
    std::vector<Product*> searchProducts(const std::string& searchTerm, const std::string& searchBy) const;

    // 删除产品（仅允许商家删除自有产品）
    // 参数：产品ID、操作商家用户名（需验证所有权）
    bool removeProduct(const std::string& productID, const std::string& merchantUsername);

    void updateProduct(Product* product); // 更新产品信息后保存（通用更新方法）
    void persistChanges(); // 显式将数据持久化到文件（调用saveProductsToFile）

    // 获取所有有效产品类型（去重后的列表，如["Book", "Food", "Clothing"]）
    std::vector<std::string> getAvailableProductTypes() const;

    // 类别折扣管理方法
    void applyCategoryDiscount(const std::string& merchantUsername, const std::string& categoryType, double discountPercentage); // 应用/更新类别折扣
    double getActiveCategoryDiscount(const std::string& merchantUsername, const std::string& categoryType) const; // 获取当前有效折扣率（无则返回0）
};

#endif // PRODUCTMANAGER_H