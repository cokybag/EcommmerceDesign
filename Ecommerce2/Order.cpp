#define   _CRT_SECURE_NO_WARNINGS

#include "Order.h"
#include <iostream>
#include <iomanip> // 用于std::fixed, std::setprecision
#include <algorithm> // 用于序列化时替换逗号（如有需要）

// 辅助函数：将订单状态转换为字符串
std::string orderStatusToString(OrderStatus status) {
    switch (status) {
    case OrderStatus::PendingPayment: return "待付款";
    case OrderStatus::Paid: return "已付款";
    case OrderStatus::Cancelled: return "已取消";
    case OrderStatus::FailedPayment: return "付款失败";
    default: return "未知";
    }
}

// 辅助函数：将字符串转换为订单状态
OrderStatus stringToOrderStatus(const std::string& statusStr) {
    if (statusStr == "PendingPayment") return OrderStatus::PendingPayment;
    if (statusStr == "Paid") return OrderStatus::Paid;
    if (statusStr == "Cancelled") return OrderStatus::Cancelled;
    if (statusStr == "FailedPayment") return OrderStatus::FailedPayment;
    // 可能抛出错误或返回默认值
    std::cerr << "警告：未知的订单状态字符串'" << statusStr << "'。默认设置为付款失败。" << std::endl;
    return OrderStatus::FailedPayment;
}


Order::Order(std::string id, std::string cUsername, const std::vector<OrderItem>& oItems)
    : orderID(id), consumerUsername(cUsername), items(oItems), status(OrderStatus::PendingPayment) {
    creationTime = std::time(nullptr); // 获取当前时间戳
    lastUpdateTime = creationTime;
    calculateTotalAmount(); // 计算订单总金额
}

// 从文件加载的构造函数
Order::Order(std::string id, std::string cUsername, const std::vector<OrderItem>& oItems,
    double total, OrderStatus stat, std::time_t cTime, std::time_t uTime)
    : orderID(id), consumerUsername(cUsername), items(oItems), totalAmount(total),
    status(stat), creationTime(cTime), lastUpdateTime(uTime) {
    // 直接传入总金额（不重新计算）
}


void Order::setStatus(OrderStatus newStatus) {
    status = newStatus;
    lastUpdateTime = std::time(nullptr); // 更新最后更新时间为当前时间
}

void Order::calculateTotalAmount() {
    totalAmount = 0.0;
    for (const auto& item : items) {
        totalAmount += item.getSubtotal(); // 累加每个订单项的子总价
    }
}

void Order::displayOrderDetails() const {
    char timeBuffer[80];
    // 将时间戳格式化为"年-月-日 时:分:秒"
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&creationTime));
    std::string creationTimeStr(timeBuffer);

    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&lastUpdateTime));
    std::string updateTimeStr(timeBuffer);

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "订单编号: " << orderID << std::endl;
    std::cout << "消费者: " << consumerUsername << std::endl;
    std::cout << "状态: " << orderStatusToString(status) << std::endl;
    std::cout << "创建时间: " << creationTimeStr << std::endl;
    std::cout << "最后更新时间: " << updateTimeStr << std::endl;
    std::cout << "总金额: $" << std::fixed << std::setprecision(2) << totalAmount << std::endl;
    std::cout << "商品项 (" << items.size() << "):" << std::endl;
    for (const auto& item : items) {
        item.display(); // 显示每个订单项详情
    }
    std::cout << "----------------------------------------" << std::endl;
}

void Order::serialize(std::ostream& os) const {
    // 序列化格式：CSV格式，字段用逗号分隔
    os << orderID << ","
        << consumerUsername << ","
        << totalAmount << ","
        << orderStatusToString(status) << ","
        << creationTime << "," // 将time_t存储为长整型数值
        << lastUpdateTime << "," // 将time_t存储为长整型数值
        << items.size(); // 商品项数量

    for (const auto& item : items) {
        // 替换商品名称中的逗号（避免破坏CSV格式）
        std::string safeProductName = item.productName;
        std::replace(safeProductName.begin(), safeProductName.end(), ',', ';'); // 用分号替换逗号

        os << "," << item.productID // 商品ID
            << "," << safeProductName // 处理后的商品名称
            << "," << item.merchantUsername // 商家用户名
            << "," << item.priceAtOrder // 下单时价格
            << "," << item.quantity; // 购买数量
    }
}