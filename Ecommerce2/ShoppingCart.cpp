#include "ShoppingCart.h"
#include "Products.h" // 用于Product::getAvailableStock()（获取可用库存）
#include <iostream>
#include <iomanip> // 用于std::fixed, std::setprecision（固定小数格式）

ShoppingCart::ShoppingCart() {
    // 构造函数：map会自动初始化，无需额外操作
}

bool ShoppingCart::addItem(Product* product, int quantity) {
    if (!product || quantity <= 0) { // 检查产品指针有效性和数量合法性
        std::cout << "错误：无效的产品或数量。" << std::endl;
        return false;
    }

    if (product->getAvailableStock() < quantity) { // 检查可用库存是否足够
        std::cout << "错误：" << product->getName() << "的可用库存不足。可用数量：" << product->getAvailableStock() << std::endl;
        return false;
    }

    std::string productID = product->getID();
    auto it = items.find(productID);
    if (it != items.end()) { // 产品已存在于购物车，更新数量
        if (product->getAvailableStock() < (it->second.quantity + quantity)) { // 检查增量后的库存是否足够
            std::cout << "错误：无法添加" << quantity << "件" << product->getName()
                << "。购物车现有数量：" << it->second.quantity
                << "，平台可用库存：" << product->getAvailableStock() << std::endl;
            return false;
        }
        it->second.quantity += quantity; // 增加数量
    }
    else { // 新产品加入购物车
        items[productID] = CartItem(product, quantity); // 创建新条目
    }
    std::cout << quantity << "件'" << product->getName() << "'已加入购物车。" << std::endl;
    return true;
}

bool ShoppingCart::removeItem(const std::string& productID, int quantityToRemove) {
    if (quantityToRemove <= 0) { // 检查移除数量合法性
        std::cout << "错误：移除数量必须为正数。" << std::endl;
        return false;
    }

    auto it = items.find(productID);
    if (it != items.end()) {
        if (it->second.quantity > quantityToRemove) { // 部分移除
            it->second.quantity -= quantityToRemove;
            std::cout << quantityToRemove << "件'" << it->second.product->getName() << "'已从购物车移除。" << std::endl;
        }
        else { // 全部移除
            std::cout << "购物车中所有'" << it->second.product->getName() << "'已移除。" << std::endl;
            items.erase(it); // 删除条目
        }
        return true;
    }
    else {
        std::cout << "错误：购物车中未找到产品ID " << productID << "。" << std::endl;
        return false;
    }
}

bool ShoppingCart::updateItemQuantity(const std::string& productID, int newQuantity) {
    if (newQuantity < 0) { // 检查新数量合法性（不能为负）
        std::cout << "错误：新数量不能为负数。" << std::endl;
        return false;
    }

    auto it = items.find(productID);
    if (it != items.end()) {
        if (newQuantity == 0) { // 数量设为0时移除条目
            std::cout << "数量设为0，产品'" << it->second.product->getName() << "'已从购物车移除。" << std::endl;
            items.erase(it);
            return true;
        }
        // 检查新数量是否超过可用库存
        if (it->second.product->getAvailableStock() < newQuantity) {
            std::cout << "错误：无法将" << it->second.product->getName() << "的数量设置为" << newQuantity
                << "。可用库存：" << it->second.product->getAvailableStock() << std::endl;
            return false;
        }
        it->second.quantity = newQuantity; // 更新数量
        std::cout << "'" << it->second.product->getName() << "'的数量已更新为" << newQuantity << "。" << std::endl;
        return true;
    }
    else {
        std::cout << "错误：购物车中未找到产品ID " << productID << "以更新数量。" << std::endl;
        return false;
    }
}

void ShoppingCart::clearItem(const std::string& productID) {
    auto it = items.find(productID);
    if (it != items.end()) { // 移除指定条目
        std::cout << "产品'" << it->second.product->getName() << "'已从购物车完全移除。" << std::endl;
        items.erase(it);
    }
    else {
        std::cout << "购物车中未找到产品ID " << productID << "以执行清除操作。" << std::endl;
    }
}


void ShoppingCart::clearCart() {
    if (!items.empty()) { // 清空购物车
        items.clear();
        std::cout << "购物车已清空。" << std::endl;
    }
    else {
        std::cout << "购物车已为空。" << std::endl;
    }
}

const std::map<std::string, CartItem>& ShoppingCart::getItems() const {
    return items; // 返回购物车条目（常量引用）
}

CartItem* ShoppingCart::getItem(const std::string& productID) {
    auto it = items.find(productID);
    if (it != items.end()) {
        return &(it->second); // 返回条目指针
    }
    return nullptr; // 未找到返回空指针
}


double ShoppingCart::getTotalValue() const {
    double total = 0.0;
    for (const auto& pair : items) {
        total += pair.second.getSubtotal(); // 累加所有条目的子总价
    }
    return total; // 返回总价值
}

bool ShoppingCart::isEmpty() const {
    return items.empty(); // 判断购物车是否为空
}

void ShoppingCart::displayCart() const {
    if (items.empty()) { // 购物车为空时提示
        std::cout << "你的购物车为空。" << std::endl;
        return;
    }
    std::cout << "\n--- 你的购物车 ---" << std::endl;
    for (const auto& pair : items) {
        pair.second.display(); // 显示每个条目详情
    }
    std::cout << "--------------------------" << std::endl;
    std::cout << "购物车总价值：$" << std::fixed << std::setprecision(2) << getTotalValue() << std::endl; // 显示总价值（两位小数）
    std::cout << "--------------------------" << std::endl;
}