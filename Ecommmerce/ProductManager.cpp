#include "ProductManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm> // 用于std::max, std::sort, std::unique, std::transform
#include <set> // 用于获取唯一商品类型

// 假设split函数全局可用或在公共util.h中定义
// 若未定义，请确保其已实现（例如来自UserManager.cpp或工具文件）
extern std::vector<std::string> split(const std::string& s, char delimiter);

// 产品管理器构造函数
ProductManager::ProductManager() : nextProductID(1) {
    loadProductsFromFile(); // 从文件加载现有产品数据
    if (!products.empty()) { // 若存在已加载的产品
        for (const auto* p : products) { // 遍历所有产品
            try {
                // 解析产品ID（假设ID格式为"P数字"，如"P123"）
                if (p->getID().length() > 1 && p->getID()[0] == 'P') {
                    int idNum = std::stoi(p->getID().substr(1)); // 提取数字部分
                    // 更新下一个可用ID（取最大值+1）
                    nextProductID = std::max(nextProductID, idNum + 1);
                }
            }
            catch (const std::exception& e) {
                // 忽略解析错误（静默警告，不中断程序）
                // std::cerr << "Warning: Could not parse product ID " << p->getID() << std::endl;
            }
        }
    }
}

// 析构函数：释放所有动态分配的产品对象
ProductManager::~ProductManager() {
    for (Product* product : products) {
        delete product; // 释放单个产品对象
    }
    products.clear(); // 清空产品列表
}

// 生成新的产品ID（格式为"P+递增数字"，如"P1"、"P2"）
std::string ProductManager::generateNewProductID() {
    return "P" + std::to_string(nextProductID++); // 数字自增后转换为字符串
}

// 从文件加载产品数据（CSV格式）
void ProductManager::loadProductsFromFile() {
    std::ifstream inFile(filename); // 打开产品数据文件
    if (!inFile.is_open()) { // 若文件打开失败，直接返回
        return;
    }

    std::string line;
    while (std::getline(inFile, line)) { // 逐行读取文件
        if (line.empty()) continue; // 跳过空行

        // 按逗号分割行数据为字段列表
        std::vector<std::string> tokens = split(line, ',');
        if (tokens.size() == 8) { // 验证字段数量是否符合CSV格式（8个字段）
            try {
                // 解析各字段值
                std::string id = tokens[0]; // 产品ID
                std::string name = tokens[1]; // 产品名称（替换分号为逗号，还原原始数据）
                std::replace(name.begin(), name.end(), ';', ',');
                std::string desc = tokens[2]; // 产品描述（同上）
                std::replace(desc.begin(), desc.end(), ';', ',');
                double origP = std::stod(tokens[3]); // 原价
                double saleP = std::stod(tokens[4]); // 售价
                int stock = std::stoi(tokens[5]); // 库存
                std::string owner = tokens[6]; // 所属商家用户名
                std::string type = tokens[7]; // 产品类型（Book/Food/Clothing）

                Product* newProd = nullptr; // 初始化产品指针

                // 根据类型创建具体产品对象
                if (type == "Book") {
                    newProd = new Book(id, name, desc, origP, saleP, stock, owner);
                }
                else if (type == "Food") {
                    newProd = new Food(id, name, desc, origP, saleP, stock, owner);
                }
                else if (type == "Clothing") {
                    newProd = new Clothing(id, name, desc, origP, saleP, stock, owner);
                }
                else { // 未知类型，跳过并警告
                    std::cerr << "Warning: Unknown product type '" << type << "' for product '" << name << "' in file. Skipping." << std::endl;
                    continue;
                }

                if (newProd) products.push_back(newProd); // 将有效产品添加到列表

            }
            catch (const std::invalid_argument& ia) { // 处理数值解析错误
                std::cerr << "Error parsing numeric value for a product in file: " << ia.what() << " Line: " << line << std::endl;
            }
            catch (const std::out_of_range& oor) { // 处理数值范围错误
                std::cerr << "Numeric value out of range for a product in file. Line: " << line << std::endl;
            }
        }
        else { // 字段数量错误，跳过并警告
            std::cerr << "Warning: Malformed line in product file: " << line << ". Skipping." << std::endl;
        }
    }
    inFile.close(); // 关闭文件
}

// 将产品数据保存到文件（CSV格式）
void ProductManager::saveProductsToFile() const {
    std::ofstream outFile(filename); // 打开文件用于写入
    if (!outFile.is_open()) { // 若打开失败，输出错误并返回
        std::cerr << "Error: Could not open product file for writing: " << filename << std::endl;
        return;
    }
    for (const Product* product : products) { // 遍历所有产品
        product->serialize(outFile); // 调用产品的序列化方法写入数据
        outFile << std::endl; // 每行数据后换行
    }
    outFile.close(); // 关闭文件
}

// 持久化更改（保存到文件）
void ProductManager::persistChanges() {
    saveProductsToFile(); // 调用保存方法
}

// 添加新产品
bool ProductManager::addProduct(const std::string& name, const std::string& description, double originalPrice,
    int stock, const std::string& ownerMerchantUsername, const std::string& productType) {
    std::string newID = generateNewProductID(); // 生成新ID
    Product* newProd = nullptr; // 初始化产品指针

    // 根据类型创建具体产品对象（初始售价等于原价）
    if (productType == "Book") {
        newProd = new Book(newID, name, description, originalPrice, originalPrice, stock, ownerMerchantUsername);
    }
    else if (productType == "Food") {
        newProd = new Food(newID, name, description, originalPrice, originalPrice, stock, ownerMerchantUsername);
    }
    else if (productType == "Clothing") {
        newProd = new Clothing(newID, name, description, originalPrice, originalPrice, stock, ownerMerchantUsername);
    }
    else { // 不支持的类型，输出错误并回退ID计数器
        std::cout << "Unsupported product category. Please choose Book, Food, or Clothing." << std::endl;
        nextProductID--; // 撤销ID生成
        return false;
    }

    // 检查该商家和类别是否有有效的折扣规则
    double discountPercent = getActiveCategoryDiscount(ownerMerchantUsername, productType);
    if (discountPercent > 0.0) { // 若存在有效折扣
        // 计算折后价并更新产品售价
        double newSalePrice = newProd->getOriginalPrice() * (1.0 - (discountPercent / 100.0));
        newProd->setCurrentSalePrice(newSalePrice);
        // 输出提示信息
        std::cout << "Info: Product '" << name << "' automatically discounted by " << discountPercent << "% upon adding." << std::endl;
    }

    products.push_back(newProd); // 添加产品到列表
    saveProductsToFile(); // 保存到文件
    return true;
}

// 根据ID查找产品（返回指针，未找到则返回nullptr）
Product* ProductManager::findProductByID(const std::string& id) {
    // 使用lambda表达式在列表中查找匹配ID的产品
    auto it = std::find_if(products.begin(), products.end(),
        [&](const Product* p) { return p->getID() == id; });
    if (it != products.end()) { // 找到则返回指针
        return *it;
    }
    return nullptr; // 未找到返回nullptr
}

// 获取所有产品列表（副本）
std::vector<Product*> ProductManager::getAllProducts() const {
    return products; // 返回产品列表副本
}

// 获取指定商家的所有产品
std::vector<Product*> ProductManager::getProductsByMerchant(const std::string& merchantUsername) const {
    std::vector<Product*> result; // 结果列表
    for (Product* p : products) { // 遍历所有产品
        if (p->getOwnerMerchantUsername() == merchantUsername) { // 匹配商家用户名
            result.push_back(p); // 添加到结果列表
        }
    }
    return result; // 返回结果列表
}

// 搜索产品（支持按名称、类型、商家搜索，不区分大小写）
std::vector<Product*> ProductManager::searchProducts(const std::string& searchTerm, const std::string& searchBy) const {
    std::vector<Product*> results; // 结果列表
    std::string termLower = searchTerm; // 转换搜索词为小写
    std::transform(termLower.begin(), termLower.end(), termLower.begin(),
        [](unsigned char c) { return std::tolower(c); }); // 转换函数：字符转小写

    for (Product* p : products) { // 遍历所有产品
        bool match = false; // 匹配标志

        // 根据搜索类型执行不同匹配逻辑
        if (searchBy == "name") { // 按名称搜索
            std::string nameLower = p->getName(); // 获取产品名称并转小写
            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(),
                [](unsigned char c) { return std::tolower(c); });
            // 检查是否包含搜索词（不区分大小写）
            if (nameLower.find(termLower) != std::string::npos) {
                match = true;
            }
        }
        else if (searchBy == "type") { // 按类型搜索
            std::string typeLower = p->getProductType(); // 获取产品类型并转小写
            std::transform(typeLower.begin(), typeLower.end(), typeLower.begin(),
                [](unsigned char c) { return std::tolower(c); });
            if (typeLower.find(termLower) != std::string::npos) {
                match = true;
            }
        }
        else if (searchBy == "merchant") { // 按商家搜索
            std::string merchantLower = p->getOwnerMerchantUsername(); // 获取商家名并转小写
            std::transform(merchantLower.begin(), merchantLower.end(), merchantLower.begin(),
                [](unsigned char c) { return std::tolower(c); });
            if (merchantLower.find(termLower) != std::string::npos) {
                match = true;
            }
        }

        if (match) { // 若匹配成功，添加到结果列表
            results.push_back(p);
        }
    }
    return results; // 返回搜索结果
}

// 删除产品（需验证是否为商家自有产品）
bool ProductManager::removeProduct(const std::string& productID, const std::string& merchantUsername) {
    // 查找匹配ID的产品
    auto it = std::find_if(products.begin(), products.end(),
        [&](const Product* p) { return p->getID() == productID; });

    if (it != products.end()) { // 找到产品
        if ((*it)->getOwnerMerchantUsername() == merchantUsername) { // 验证属于当前商家
            delete* it; // 释放内存
            products.erase(it); // 从列表移除
            saveProductsToFile(); // 保存更改
            return true;
        }
        else { // 非商家自有产品，拒绝删除
            std::cout << "Error: You can only remove your own products." << std::endl;
            return false;
        }
    }
    // 未找到产品
    std::cout << "Error: Product ID not found." << std::endl;
    return false;
}

// 更新产品信息（保存到文件）
void ProductManager::updateProduct(Product* product) {
    if (product) { // 若产品指针有效
        saveProductsToFile(); // 重新保存所有产品数据（触发序列化）
    }
}

// 获取所有可用产品类型（去重后的列表）
std::vector<std::string> ProductManager::getAvailableProductTypes() const {
    std::set<std::string> typesSet; // 使用set自动去重
    for (const auto* p : products) { // 遍历所有产品
        typesSet.insert(p->getProductType()); // 添加类型到set
    }
    return std::vector<std::string>(typesSet.begin(), typesSet.end()); // 转换为列表返回
}

// 应用类别折扣（设置商家某类别产品的折扣规则，并更新现有产品）
void ProductManager::applyCategoryDiscount(const std::string& merchantUsername, const std::string& categoryType, double discountPercentage) {
    if (discountPercentage < 0 || discountPercentage > 100) { // 验证折扣范围
        std::cout << "Error: Invalid discount percentage. Must be between 0 and 100." << std::endl;
        return;
    }

    // 存储折扣规则（关联商家-类别-折扣率）
    activeCategoryDiscounts[merchantUsername][categoryType] = discountPercentage;

    // 应用折扣到现有产品
    int count = 0; // 记录受影响的产品数量
    for (Product* p : products) {
        // 匹配商家和类别
        if (p->getOwnerMerchantUsername() == merchantUsername && p->getProductType() == categoryType) {
            // 计算折后价并更新
            double newSalePrice = p->getOriginalPrice() * (1.0 - (discountPercentage / 100.0));
            p->setCurrentSalePrice(newSalePrice);
            count++; // 计数加一
        }
    }

    if (count > 0) { // 若有产品被更新
        saveProductsToFile(); // 保存更改
        std::cout << count << " of your products in category '" << categoryType
            << "' have been discounted by " << discountPercentage << "%." << std::endl; // 输出成功信息
    }
    else { // 无现有产品匹配，但规则已保存（适用于未来添加的产品）
        std::cout << "You have no products in category '" << categoryType << "' to apply discount to, but the rule is set for future additions." << std::endl;
    }
}

// 获取有效类别折扣（返回折扣率，无则返回0）
double ProductManager::getActiveCategoryDiscount(const std::string& merchantUsername, const std::string& categoryType) const {
    // 查找商家的折扣规则
    auto merchIt = activeCategoryDiscounts.find(merchantUsername);
    if (merchIt != activeCategoryDiscounts.end()) { // 商家存在折扣规则
        auto catIt = merchIt->second.find(categoryType); // 查找类别的折扣规则
        if (catIt != merchIt->second.end()) { // 类别存在折扣规则
            return catIt->second; // 返回折扣率
        }
    }
    return 0.0; // 无有效折扣
}