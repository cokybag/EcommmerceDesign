#include "Products.h"
#include <iomanip> // 用于std::fixed和std::setprecision（固定小数位数输出）
#include <sstream> // 用于字符串流操作
#include <algorithm> // 用于std::replace（字符串替换）

// Product 类实现
Product::Product(std::string id, std::string n, std::string desc, double origP, double saleP, int initialStock, std::string owner, std::string type)
    : productID(id), name(n), description(desc), originalPrice(origP), currentSalePrice(saleP),
    totalStock(initialStock), reservedStock(0), // 初始化预留库存为0
    ownerMerchantUsername(owner), productType(type) {
}

double Product::getPrice() const {
    return currentSalePrice; // 返回当前销售价
}

std::string Product::getID() const { return productID; }
std::string Product::getName() const { return name; }
std::string Product::getDescription() const { return description; }
double Product::getOriginalPrice() const { return originalPrice; }
double Product::getCurrentSalePrice() const { return currentSalePrice; }
int Product::getTotalStock() const { return totalStock; } // 获取总库存
int Product::getReservedStock() const { return reservedStock; } // 获取预留库存

int Product::getAvailableStock() const {
    return totalStock - reservedStock; // 可用库存 = 总库存 - 预留库存
}

std::string Product::getOwnerMerchantUsername() const { return ownerMerchantUsername; }
std::string Product::getProductType() const { return productType; }


void Product::setOriginalPrice(double price) {
    if (price >= 0) originalPrice = price; // 确保原价非负
    if (currentSalePrice > originalPrice) currentSalePrice = originalPrice; // 售价不超过原价
}

void Product::setCurrentSalePrice(double price) {
    if (price >= 0 && price <= originalPrice) { // 售价在合理范围内（0 ≤ 售价 ≤ 原价）
        currentSalePrice = price;
    }
    else if (price > originalPrice) { // 售价超过原价时，自动设为原价
        currentSalePrice = originalPrice;
    }
    // 负数售价不处理（可选择日志记录或设为0，此处暂不处理）
}

void Product::setTotalStock(int newTotalStock) { // 重命名自setStock（总库存设置）
    if (newTotalStock >= 0) {
        // 确保总库存不低于已预留库存
        if (newTotalStock < reservedStock) {
            std::cerr << "警告：产品" << productID << "的总库存设置为" << newTotalStock << "，低于预留库存" << reservedStock << "。自动调整为预留库存值。" << std::endl;
            totalStock = reservedStock;
        }
        else {
            totalStock = newTotalStock;
        }
    }
}

void Product::setDescription(const std::string& desc) {
    description = desc; // 更新产品描述
}
void Product::setName(const std::string& newName) {
    name = newName; // 更新产品名称
}

// 库存预留方法
bool Product::reserveStock(int quantity) {
    if (quantity <= 0) return false; // 不能预留非正数量
    if (getAvailableStock() >= quantity) { // 可用库存足够时执行预留
        reservedStock += quantity;
        return true;
    }
    return false; // 库存不足，预留失败
}

bool Product::releaseReservedStock(int quantity) {
    if (quantity <= 0) return false;
    if (reservedStock >= quantity) { // 按请求数量释放预留库存
        reservedStock -= quantity;
        return true;
    }
    // 尝试释放超过预留量的库存时，释放全部预留并记录警告
    std::cerr << "警告：产品" << productID << "尝试释放" << quantity << "件预留库存，但当前仅预留" << reservedStock << "件。已释放全部预留库存。" << std::endl;
    reservedStock = 0;
    return false; // 表示部分成功或存在异常
}

bool Product::confirmSaleFromReserved(int quantity) {
    if (quantity <= 0) return false;
    if (reservedStock >= quantity && totalStock >= quantity) { // 预留库存和总库存均足够时确认销售
        totalStock -= quantity; // 减少总库存
        reservedStock -= quantity; // 减少预留库存
        if (reservedStock < 0) reservedStock = 0; // 防止预留库存为负数（应由调用逻辑保证，此处作为防御性处理）
        return true;
    }
    return false; // 库存不足，销售确认失败（正常逻辑下不应发生）
}


void Product::displayDetails() const {
    std::cout << "-------------------------------------\n"
        << "ID: " << productID << "\n" // 产品ID
        << "Name: " << name << "\n" // 产品名称
        << "Type: " << productType << "\n" // 产品类型
        << "Description: " << description << "\n" // 产品描述
        << "Original Price: $" << std::fixed << std::setprecision(2) << originalPrice << "\n" // 原价（保留两位小数）
        << "Current Price: $" << std::fixed << std::setprecision(2) << currentSalePrice << "\n" // 当前售价（保留两位小数）
        << "Total Stock: " << totalStock << "\n" // 总库存
        << "Reserved Stock: " << reservedStock << "\n" // 预留库存
        << "Available Stock: " << getAvailableStock() << "\n" // 可用库存
        << "Sold by: " << ownerMerchantUsername << "\n" // 销售商家
        << "-------------------------------------" << std::endl;
}

void Product::serialize(std::ostream& os) const {
    std::string safeName = name;
    std::replace(safeName.begin(), safeName.end(), ',', ';'); // 替换名称中的逗号为分号（避免CSV解析问题）
    std::string safeDesc = description;
    std::replace(safeDesc.begin(), safeDesc.end(), ',', ';'); // 替换描述中的逗号为分号

    os << productID << "," // 产品ID
        << safeName << "," // 处理后的产品名称
        << safeDesc << "," // 处理后的产品描述
        << originalPrice << "," // 原价
        << currentSalePrice << "," // 当前售价
        << totalStock << "," // 总库存（预留库存不保存，运行时通过订单重建）
        << ownerMerchantUsername << "," // 商家用户名
        << productType; // 产品类型
    // 注意：预留库存（reservedStock）未保存到文件，需通过订单数据重新计算
}

// --- 书籍类 ---
Book::Book(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner)
    : Product(id, n, desc, origP, saleP, s, owner, "Book") { // 类型固定为"Book"
}

void Book::displayDetails() const {
    Product::displayDetails(); // 复用基类的详情显示逻辑
}
void Book::serialize(std::ostream& os) const {
    Product::serialize(os); // 复用基类的序列化逻辑
}

// --- 食品类 ---
Food::Food(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner)
    : Product(id, n, desc, origP, saleP, s, owner, "Food") { // 类型固定为"Food"
}

void Food::displayDetails() const {
    Product::displayDetails();
}
void Food::serialize(std::ostream& os) const {
    Product::serialize(os);
}

// --- 服装类 ---
Clothing::Clothing(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner)
    : Product(id, n, desc, origP, saleP, s, owner, "Clothing") { // 类型固定为"Clothing"
}

void Clothing::displayDetails() const {
    Product::displayDetails();
}
void Clothing::serialize(std::ostream& os) const {
    Product::serialize(os);
}