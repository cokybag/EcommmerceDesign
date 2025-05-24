#include "ProductManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm> 
#include <set> 
#include "utils.h"
//extern std::vector<std::string> split(const std::string& s, char delimiter);

// 产品管理器构造函数：初始化产品ID生成器并从文件加载产品
ProductManager::ProductManager() : nextProductID(1) {
    loadProductsFromFile();
    if (!products.empty()) {
        for (const auto* p : products) {
            try {
                if (p->getID().length() > 1 && p->getID()[0] == 'P') {
                    int idNum = std::stoi(p->getID().substr(1));
                    nextProductID = std::max(nextProductID, idNum + 1);
                }
            }
            catch (const std::exception& e) {
                // 忽略异常，保持静默
            }
        }
    }
}

// 析构函数：清理产品内存并清空产品列表
ProductManager::~ProductManager() {
    for (Product* product : products) {
        delete product;
    }
    products.clear();
}

// 生成新的产品ID（格式：P+数字）
std::string ProductManager::generateNewProductID() {
    return "P" + std::to_string(nextProductID++);
}

// 从文件加载产品数据
void ProductManager::loadProductsFromFile() {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        return; // 文件不存在或无法打开，直接返回
    }

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        // 当前格式：ID,名称,描述,原价,售价,总库存,商家,类型 (若不保存预留库存则为7个字段)
        std::vector<std::string> tokens = split(line, ',');
        // 若保存预留库存，此处应为tokens.size() == 9 (ID,名称,描述,原价,售价,总库存,预留库存,商家,类型)
        // 目前采用8个字段：ID,名称,描述,原价,售价,库存,商家,类型
        if (tokens.size() == 8) {
            try {
                std::string id = tokens[0];
                std::string name = tokens[1];
                std::replace(name.begin(), name.end(), ';', ','); // 还原被替换的逗号
                std::string desc = tokens[2];
                std::replace(desc.begin(), desc.end(), ';', ','); // 还原被替换的逗号
                double origP = std::stod(tokens[3]);
                double saleP = std::stod(tokens[4]);
                int totalStockVal = std::stoi(tokens[5]); // 这是总库存
                // int reservedStockVal = 0; // 若不加载预留库存则默认为0
                // if (tokens.size() == 9) reservedStockVal = std::stoi(tokens[6]); // 若加载预留库存
                std::string owner = tokens[6]; // 若加载预留库存则需调整索引
                std::string type = tokens[7];  // 若加载预留库存则需调整索引

                // 创建对应类型的产品对象
                Product* newProd = nullptr;
                if (type == "Book") {
                    // 将总库存值传递给构造函数
                    newProd = new Book(id, name, desc, origP, saleP, totalStockVal, owner);
                }
                else if (type == "Food") {
                    newProd = new Food(id, name, desc, origP, saleP, totalStockVal, owner);
                }
                else if (type == "Clothing") {
                    newProd = new Clothing(id, name, desc, origP, saleP, totalStockVal, owner);
                }
                else {
                    std::cerr << "警告：文件中产品'" << name << "'的类型'" << type << "'未知。跳过该产品。" << std::endl;
                    continue;
                }

                // 若从文件加载预留库存且有设置预留库存的方法：
                // if (newProd && tokens.size() == 9) { /* newProd->setReservedStock_after_creation(reservedStockVal); */ }
                // 目前预留库存初始化为0，将由OrderManager加载订单时设置

                if (newProd) products.push_back(newProd);

            }
            catch (const std::invalid_argument& ia) {
                std::cerr << "解析文件中产品的数值时出错: " << ia.what() << " 行内容: " << line << std::endl;
            }
            catch (const std::out_of_range& oor) {
                std::cerr << "文件中产品的数值超出范围。行内容: " << line << std::endl;
            }
        }
        else {
            std::cerr << "警告：产品文件中的行格式错误（期望8个字段）: " << line << "。跳过此行。" << std::endl;
        }
    }
    inFile.close();
}

// 将产品数据保存到文件
void ProductManager::saveProductsToFile() const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "错误：无法打开产品文件进行写入: " << filename << std::endl;
        return;
    }
    for (const Product* product : products) {
        product->serialize(outFile); // Product::serialize现在写入总库存
        outFile << std::endl;
    }
    outFile.close();
}

// 持久化所有更改（保存到文件）
void ProductManager::persistChanges() {
    saveProductsToFile();
}

// 添加新产品
bool ProductManager::addProduct(const std::string& name, const std::string& description, double originalPrice,
    int initialTotalStock, const std::string& ownerMerchantUsername, const std::string& productType) { // 参数重命名
    std::string newID = generateNewProductID();
    Product* newProd = nullptr;

    // 根据产品类型创建对应对象
    if (productType == "Book") {
        newProd = new Book(newID, name, description, originalPrice, originalPrice, initialTotalStock, ownerMerchantUsername);
    }
    else if (productType == "Food") {
        newProd = new Food(newID, name, description, originalPrice, originalPrice, initialTotalStock, ownerMerchantUsername);
    }
    else if (productType == "Clothing") {
        newProd = new Clothing(newID, name, description, originalPrice, originalPrice, initialTotalStock, ownerMerchantUsername);
    }
    else {
        std::cout << "不支持的产品类别。请选择书籍、食品或服装。" << std::endl;
        nextProductID--; // 回退ID生成器
        return false;
    }

    // 应用类别折扣（如果有）
    double discountPercent = getActiveCategoryDiscount(ownerMerchantUsername, productType);
    if (discountPercent > 0.0) {
        double newSalePrice = newProd->getOriginalPrice() * (1.0 - (discountPercent / 100.0));
        newProd->setCurrentSalePrice(newSalePrice);
        std::cout << "信息：产品'" << name << "'添加时自动应用" << discountPercent << "%的折扣。" << std::endl;
    }

    products.push_back(newProd);
    saveProductsToFile();
    return true;
}

// 根据ID查找产品（非const版本）
Product* ProductManager::findProductByID(const std::string& id) {
    auto it = std::find_if(products.begin(), products.end(),
        [&](const Product* p) { return p->getID() == id; });
    if (it != products.end()) {
        return *it;
    }
    return nullptr;
}

// 获取所有产品
std::vector<Product*> ProductManager::getAllProducts() const {
    return products; // 返回指针向量的副本
}

// 获取特定商家的所有产品
std::vector<Product*> ProductManager::getProductsByMerchant(const std::string& merchantUsername) const {
    std::vector<Product*> result;
    for (Product* p : products) { // 如果products存储Product*，则此处正常。如果存储const Product*，则result也应为const Product*
        if (p->getOwnerMerchantUsername() == merchantUsername) {
            result.push_back(p);
        }
    }
    return result;
}

// 搜索产品（支持按名称、类型、商家搜索）
std::vector<Product*> ProductManager::searchProducts(const std::string& searchTerm, const std::string& searchBy) const {
    std::vector<Product*> results;
    std::string termLower = searchTerm;
    // 将搜索词转换为小写
    std::transform(termLower.begin(), termLower.end(), termLower.begin(),
        [](unsigned char c) { return std::tolower(c); });

    for (Product* p : products) {
        bool match = false;
        if (searchBy == "name") {
            std::string nameLower = p->getName();
            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(),
                [](unsigned char c) { return std::tolower(c); });
            if (nameLower.find(termLower) != std::string::npos) {
                match = true;
            }
        }
        else if (searchBy == "type") {
            std::string typeLower = p->getProductType();
            std::transform(typeLower.begin(), typeLower.end(), typeLower.begin(),
                [](unsigned char c) { return std::tolower(c); });
            if (typeLower.find(termLower) != std::string::npos) {
                match = true;
            }
        }
        else if (searchBy == "merchant") {
            std::string merchantLower = p->getOwnerMerchantUsername();
            std::transform(merchantLower.begin(), merchantLower.end(), merchantLower.begin(),
                [](unsigned char c) { return std::tolower(c); });
            if (merchantLower.find(termLower) != std::string::npos) {
                match = true;
            }
        }
        // 可根据需要添加更多搜索条件

        if (match) {
            results.push_back(p);
        }
    }
    return results;
}

// 移除产品（商家只能移除自己的产品，且产品不能有预留库存）
bool ProductManager::removeProduct(const std::string& productID, const std::string& merchantUsername) {
    auto it = std::find_if(products.begin(), products.end(),
        [&](const Product* p) { return p->getID() == productID; });

    if (it != products.end()) {
        // 检查：如果产品有预留库存（待处理订单），则不能移除
        if ((*it)->getReservedStock() > 0) {
            std::cout << "错误：产品'" << (*it)->getName() << "'有" << (*it)->getReservedStock()
                << "件被预留用于待处理订单。无法移除。" << std::endl;
            std::cout << "请确保所有涉及此产品的订单已完成或取消。" << std::endl;
            return false;
        }
        if ((*it)->getOwnerMerchantUsername() == merchantUsername) {
            delete* it;
            products.erase(it);
            saveProductsToFile();
            return true;
        }
        else {
            std::cout << "错误：您只能移除自己的产品。" << std::endl;
            return false;
        }
    }
    std::cout << "错误：未找到该产品ID。" << std::endl;
    return false;
}

// 更新产品信息（实际仅保存文件，逻辑简化版）
void ProductManager::updateProduct(Product* product) {
    if (product) {
        saveProductsToFile();
    }
}

// 获取所有可用的产品类型
std::vector<std::string> ProductManager::getAvailableProductTypes() const {
    std::set<std::string> typesSet;
    for (const auto* p : products) {
        typesSet.insert(p->getProductType());
    }
    return std::vector<std::string>(typesSet.begin(), typesSet.end());
}

// 应用类别折扣（对特定商家的特定类别产品）
void ProductManager::applyCategoryDiscount(const std::string& merchantUsername, const std::string& categoryType, double discountPercentage) {
    if (discountPercentage < 0 || discountPercentage > 100) {
        std::cout << "错误：无效的折扣百分比。必须在0到100之间。" << std::endl;
        return;
    }
    activeCategoryDiscounts[merchantUsername][categoryType] = discountPercentage;
    int count = 0;
    for (Product* p : products) {
        if (p->getOwnerMerchantUsername() == merchantUsername && p->getProductType() == categoryType) {
            double newSalePrice = p->getOriginalPrice() * (1.0 - (discountPercentage / 100.0));
            p->setCurrentSalePrice(newSalePrice);
            count++;
        }
    }
    if (count > 0) {
        saveProductsToFile();
        std::cout << "您的类别为'" << categoryType
            << "'的" << count << "个产品已应用" << discountPercentage << "%的折扣。" << std::endl;
    }
    else {
        std::cout << "您没有类别为'" << categoryType << "'的产品可应用折扣，但此规则已设置，将应用于未来添加的产品。" << std::endl;
    }
}

// 获取特定商家的特定类别当前折扣百分比
double ProductManager::getActiveCategoryDiscount(const std::string& merchantUsername, const std::string& categoryType) const {
    auto merchIt = activeCategoryDiscounts.find(merchantUsername);
    if (merchIt != activeCategoryDiscounts.end()) {
        auto catIt = merchIt->second.find(categoryType);
        if (catIt != merchIt->second.end()) {
            return catIt->second;
        }
    }
    return 0.0; // 默认无折扣
}