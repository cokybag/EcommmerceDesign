#include "Products.h"
#include <iomanip> // 用于std::fixed和std::setprecision
#include <sstream> // 用于字符串流
#include <algorithm>

// 产品类构造函数
Product::Product(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner, std::string type)
    : productID(id), name(n), description(desc), originalPrice(origP), currentSalePrice(saleP), stock(s), ownerMerchantUsername(owner), productType(type) {
}

// 获取产品当前价格
double Product::getPrice() const {
    return currentSalePrice;
}

// 获取产品ID
std::string Product::getID() const { return productID; }
// 获取产品名称
std::string Product::getName() const { return name; }
// 获取产品描述
std::string Product::getDescription() const { return description; }
// 获取产品原价
double Product::getOriginalPrice() const { return originalPrice; }
// 获取产品当前售价
double Product::getCurrentSalePrice() const { return currentSalePrice; }
// 获取产品库存
int Product::getStock() const { return stock; }
// 获取产品所属商家用户名
std::string Product::getOwnerMerchantUsername() const { return ownerMerchantUsername; }
// 获取产品类型
std::string Product::getProductType() const { return productType; }

// 设置产品原价
// 如果新价格有效（非负），则更新原价
// 如果当前售价高于新原价，则将售价调整为原价
void Product::setOriginalPrice(double price) {
    if (price >= 0) originalPrice = price;
    // 如果当前售价高于原价，可能需要调整当前售价
    if (currentSalePrice > originalPrice) currentSalePrice = originalPrice;
}

// 设置产品当前售价
// 确保售价不低于0且不高于原价
void Product::setCurrentSalePrice(double price) {
    if (price >= 0 && price <= originalPrice) { // 折扣价不能高于原价
        currentSalePrice = price;
    }
    else if (price > originalPrice) {
        currentSalePrice = originalPrice; // 上限为原价
    }
}

// 设置产品库存数量（确保库存非负）
void Product::setStock(int newStock) {
    if (newStock >= 0) stock = newStock;
}

// 更新产品描述
void Product::setDescription(const std::string& desc) {
    description = desc;
}

// 更新产品名称
void Product::setName(const std::string& newName) {
    name = newName;
}

// 显示产品详细信息
void Product::displayDetails() const {
    std::cout << "-------------------------------------\n"
        << "ID: " << productID << "\n"
        << "名称: " << name << "\n"
        << "类型: " << productType << "\n"
        << "描述: " << description << "\n"
        << "原价: $" << std::fixed << std::setprecision(2) << originalPrice << "\n"
        << "当前价格: $" << std::fixed << std::setprecision(2) << currentSalePrice << "\n"
        << "库存: " << stock << "\n"
        << "商家: " << ownerMerchantUsername << "\n"
        << "-------------------------------------" << std::endl;
}

// 将产品信息序列化为CSV格式
// 替换名称和描述中的逗号，防止破坏CSV格式
void Product::serialize(std::ostream& os) const {
    // 替换名称和描述中的逗号，避免破坏CSV格式
    std::string safeName = name;
    std::replace(safeName.begin(), safeName.end(), ',', ';');
    std::string safeDesc = description;
    std::replace(safeDesc.begin(), safeDesc.end(), ',', ';');

    os << productID << ","
        << safeName << ","
        << safeDesc << ","
        << originalPrice << ","
        << currentSalePrice << ","
        << stock << ","
        << ownerMerchantUsername << ","
        << productType;
}

// --- 书籍类 ---
// 书籍类构造函数
Book::Book(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner)
    : Product(id, n, desc, origP, saleP, s, owner, "Book") {
}

// 显示书籍详细信息
void Book::displayDetails() const {
    // std::cout << "--- 书籍详情 ---" << std::endl;
    Product::displayDetails();
    // 可添加书籍特定信息，如作者、ISBN等
}

// 序列化书籍信息
void Book::serialize(std::ostream& os) const {
    Product::serialize(os); // 基类写入基本类型信息
}

// --- 食品类 ---
// 食品类构造函数
Food::Food(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner)
    : Product(id, n, desc, origP, saleP, s, owner, "Food") {
}

// 显示食品详细信息
void Food::displayDetails() const {
    // std::cout << "--- 食品详情 ---" << std::endl;
    Product::displayDetails();
    // 可添加食品特定信息，如保质期等
}

// 序列化食品信息
void Food::serialize(std::ostream& os) const {
    Product::serialize(os);
}

// --- 服装类 ---
// 服装类构造函数
Clothing::Clothing(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner)
    : Product(id, n, desc, origP, saleP, s, owner, "Clothing") {
}

// 显示服装详细信息
void Clothing::displayDetails() const {
    // std::cout << "--- 服装详情 ---" << std::endl;
    Product::displayDetails();
    // 可添加服装特定信息，如尺寸、颜色等
}

// 序列化服装信息
void Clothing::serialize(std::ostream& os) const {
    Product::serialize(os);
}