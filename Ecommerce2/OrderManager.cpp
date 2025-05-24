#include "OrderManager.h"
#include "Users.h"
#include "ProductManager.h"
#include "Products.h" // For Product methods like reserveStock, releaseReservedStock, confirmSaleFromReserved
#include "UserManager.h" // For UserManager to find merchant and update balances
#include <fstream>
#include <sstream>
#include <algorithm> // For std::find_if, std::remove_if if needed
#include <iostream>
#include <iomanip> // For std::fixed, std::setprecision
#include "utils.h"
#include "CartItem.h"
#include "ShoppingCart.h"

// 假设split函数全局可用或在公共的util.h中定义
//extern std::vector<std::string> split(const std::string& s, char delimiter);

// 构造函数：初始化订单管理器，加载订单文件并设置下一个订单ID计数器
OrderManager::OrderManager(ProductManager& pm) : nextOrderIDCounter(1) {
    loadOrdersFromFile(pm); // 传入ProductManager以重新预订库存
    // 根据已加载的订单确定nextOrderIDCounter
    if (!allOrders.empty()) {
        for (const auto* order : allOrders) {
            try {
                // 假设订单ID格式为"ORD<数字>"
                if (order->getID().length() > 3 && order->getID().substr(0, 3) == "ORD") {
                    int idNum = std::stoi(order->getID().substr(3));
                    nextOrderIDCounter = std::max(nextOrderIDCounter, idNum + 1); // 确保下一个ID是最大现有ID+1
                }
            }
            catch (const std::exception& e) {
                // 异常静音（忽略解析错误）
            }
        }
    }
}

OrderManager::~OrderManager() {
    // persistChanges(); // 退出时保存或在其他地方显式管理
    for (Order* order : allOrders) { // 释放内存
        delete order;
    }
    allOrders.clear();
}

// 生成新订单ID（格式：ORD+递增数字）
std::string OrderManager::generateNewOrderID() {
    return "ORD" + std::to_string(nextOrderIDCounter++); // 计数器自增
}

// 从文件加载订单数据（恢复库存预订状态）
void OrderManager::loadOrdersFromFile(ProductManager& pm) {
    std::ifstream inFile(ordersFilename);
    if (!inFile.is_open()) {
        // std::cerr << "警告：无法打开订单文件: " << ordersFilename << ". 重新开始." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        std::vector<std::string> tokens = split(line, ','); // 按逗号分割行数据

        // 订单格式：ID,消费者用户名,总金额,状态,创建时间,更新时间,商品数量,
        // 商品1: PID,商品名,商家用户名,单价,数量, 商品2: ...
        if (tokens.size() < 7) { // 基础字段至少7个
            std::cerr << "警告：订单行格式错误（基础字段不足）: " << line << ". 跳过." << std::endl;
            continue;
        }

        try {
            std::string orderID_str = tokens[0]; // 订单ID
            std::string consumerUsername_str = tokens[1]; // 消费者用户名
            double totalAmount_val = std::stod(tokens[2]); // 总金额
            OrderStatus status_val = stringToOrderStatus(tokens[3]); // 状态转换
            std::time_t creationTime_val = static_cast<std::time_t>(std::stoll(tokens[4])); // 创建时间（时间戳）
            std::time_t lastUpdateTime_val = static_cast<std::time_t>(std::stoll(tokens[5])); // 更新时间（时间戳）
            int numItems = std::stoi(tokens[6]); // 商品数量

            // 检查商品字段是否足够（每个商品5个字段：PID、名称、商家、单价、数量）
            if (tokens.size() < (size_t)(7 + numItems * 5)) {
                std::cerr << "警告：订单行格式错误（商品字段不足，需" << numItems << "个商品）: " << line << ". 跳过." << std::endl;
                continue;
            }

            std::vector<OrderItem> orderItems_vec; // 订单商品列表
            int currentTokenIndex = 7; // 从第8个字段开始解析商品
            for (int i = 0; i < numItems; ++i) {
                std::string item_pid = tokens[currentTokenIndex++]; // 商品ID
                std::string item_pname = tokens[currentTokenIndex++]; // 商品名（可能包含转义的逗号）
                std::replace(item_pname.begin(), item_pname.end(), ';', ','); // 恢复被转义的逗号
                std::string item_merchant = tokens[currentTokenIndex++]; // 商家用户名
                double item_price = std::stod(tokens[currentTokenIndex++]); // 单价
                int item_qty = std::stoi(tokens[currentTokenIndex++]); // 数量
                orderItems_vec.emplace_back(item_pid, item_pname, item_merchant, item_price, item_qty); // 创建订单商品项
            }

            // 创建订单对象并添加到列表
            Order* loadedOrder = new Order(orderID_str, consumerUsername_str, orderItems_vec,
                totalAmount_val, status_val, creationTime_val, lastUpdateTime_val);
            allOrders.push_back(loadedOrder);

            // 需求1：为待支付订单重新预订库存
            if (loadedOrder->getStatus() == OrderStatus::PendingPayment) {
                bool allReserved = true; // 标记所有商品是否预订成功
                for (const auto& item : loadedOrder->getItems()) {
                    Product* p = pm.findProductByID(item.productID); // 查找商品
                    if (p) {
                        if (!p->reserveStock(item.quantity)) { // 尝试预订库存
                            std::cerr << "严重错误：加载时无法为待支付订单" << loadedOrder->getID()
                                << "重新预订" << item.quantity << "个商品" << item.productID
                                << ". 可用库存: " << p->getAvailableStock()
                                << ". 订单状态可能不一致." << std::endl;
                            allReserved = false; // 标记失败
                        }
                    }
                    else {
                        std::cerr << "严重错误：加载时未找到订单" << loadedOrder->getID()
                            << "中的商品" << item.productID << "." << std::endl;
                        allReserved = false; // 标记失败
                    }
                }
                if (!allReserved) {
                    // 理想情况：释放已成功预订的库存并标记订单为异常/取消（此处简化处理）
                }
            }

        }
        catch (const std::invalid_argument& ia) { // 数值解析错误
            std::cerr << "解析订单文件中的数值时出错: " << ia.what() << " 行: " << line << std::endl;
        }
        catch (const std::out_of_range& oor) { // 数值范围错误
            std::cerr << "订单文件中的数值超出范围. 行: " << line << std::endl;
        }
        catch (const std::exception& e) { // 通用异常
            std::cerr << "加载订单时发生错误: " << e.what() << " 行: " << line << std::endl;
        }
    }
    inFile.close();
}


// 将订单数据持久化到文件
void OrderManager::persistChanges() const {
    std::ofstream outFile(ordersFilename);
    if (!outFile.is_open()) {
        std::cerr << "错误：无法打开订单文件进行写入: " << ordersFilename << std::endl;
        return;
    }
    for (const Order* order : allOrders) {
        order->serialize(outFile); // 调用Order的序列化方法
        outFile << std::endl; // 每行一个订单
    }
    outFile.close();
}

// 从购物车创建订单（核心业务逻辑）
Order* OrderManager::createOrderFromCart(Consumer* consumer, ProductManager& pm) {
    if (!consumer || consumer->getShoppingCart()->isEmpty()) { // 校验消费者和购物车有效性
        std::cout << "错误：无法创建订单。购物车为空或消费者无效。" << std::endl;
        return nullptr;
    }

    ShoppingCart* cart = consumer->getShoppingCart();
    std::vector<OrderItem> orderItemsList; // 订单商品列表
    bool stockReservationSuccess = true; // 库存预订状态

    // 第一轮：检查并预订所有商品库存
    for (const auto& pair : cart->getItems()) {
        const CartItem& cartItem = pair.second;
        Product* product = cartItem.product; // 从购物车项获取商品指针
        if (!product) { // 防御性检查（正常情况不应为空）
            std::cout << "错误：购物车中存在无效商品（空指针）。" << std::endl;
            stockReservationSuccess = false; // 标记失败
            break;
        }

        if (!product->reserveStock(cartItem.quantity)) { // 尝试预订库存
            std::cout << "订单创建失败：" << product->getName() << "库存不足。需要："
                << cartItem.quantity << "，可用：" << product->getAvailableStock() << std::endl;
            stockReservationSuccess = false; // 标记失败
            break;
        }
    }

    // 若有库存预订失败，回滚所有已预订的库存
    if (!stockReservationSuccess) {
        for (const auto& pair : cart->getItems()) { // 遍历购物车所有商品
            const CartItem& cartItem = pair.second;
            Product* product = cartItem.product;
            if (product) {
                // 释放本应在本次操作中预订的库存（可能部分成功预订）
                product->releaseReservedStock(cartItem.quantity); // 尝试释放
            }
        }
        pm.persistChanges(); // 保存商品库存变更（释放）
        std::cout << "因库存问题放弃创建订单。已释放之前预订的库存（如有）。" << std::endl;
        return nullptr;
    }

    // 若所有库存预订成功，构建订单商品项
    for (const auto& pair : cart->getItems()) {
        const CartItem& cartItem = pair.second;
        // 从购物车项获取商品信息并创建订单商品项
        orderItemsList.emplace_back(cartItem.product->getID(), cartItem.product->getName(),
            cartItem.product->getOwnerMerchantUsername(),
            cartItem.product->getCurrentSalePrice(), cartItem.quantity);
    }


    std::string newID = generateNewOrderID(); // 生成新订单ID
    Order* newOrder = new Order(newID, consumer->getUsername(), orderItemsList); // 创建订单对象
    allOrders.push_back(newOrder); // 添加到订单列表

    pm.persistChanges(); // 保存商品库存变更（预订）
    persistChanges();    // 保存新订单到文件

    // 需求3：购物车仅在订单成功完成后删除或用户主动清空
    // 因此，此处不清空购物车，待支付成功后处理

    std::cout << "订单" << newID << "创建成功。总计：$"
        << std::fixed << std::setprecision(2) << newOrder->getTotalAmount()
        << ". 请前往支付。" << std::endl;
    return newOrder;
}

// 取消订单（释放库存并更新状态）
bool OrderManager::cancelOrder(const std::string& orderID, ProductManager& pm) {
    Order* order = findOrderById(orderID); // 查找订单
    if (!order) {
        std::cout << "错误：未找到订单ID" << orderID << "." << std::endl;
        return false;
    }

    // 校验订单状态是否允许取消（待支付或支付失败）
    if (order->getStatus() != OrderStatus::PendingPayment && order->getStatus() != OrderStatus::FailedPayment) {
        std::cout << "错误：无法取消订单" << orderID << "（状态："
            << orderStatusToString(order->getStatus()) << "）。" << std::endl;
        return false;
    }

    // 释放预订库存
    bool allReleased = true;
    for (const auto& item : order->getItems()) {
        Product* p = pm.findProductByID(item.productID); // 查找商品
        if (p) {
            if (!p->releaseReservedStock(item.quantity)) { // 尝试释放库存
                std::cerr << "警告：无法完全释放订单" << orderID << "中" << item.quantity
                    << "个商品" << item.productID << "的库存。" << std::endl;
                allReleased = false; // 标记失败
            }
        }
        else {
            std::cerr << "警告：取消订单" << orderID << "时未找到商品" << item.productID << "，无法释放库存。" << std::endl;
            allReleased = false; // 标记失败
        }
    }

    order->setStatus(OrderStatus::Cancelled); // 更新订单状态为已取消
    pm.persistChanges(); // 保存商品库存变更
    persistChanges();    // 保存订单状态变更

    std::cout << "订单" << orderID << "已取消。" << std::endl;
    if (!allReleased) {
        std::cout << "警告：可能存在部分库存释放失败，请检查商品库存。" << std::endl;
    }
    return true;
}

// 处理支付逻辑（核心交易流程）
bool OrderManager::processPayment(Order* order, Consumer* consumer, const std::string& enteredPassword,
    UserManager& um, ProductManager& pm) {
    if (!order || !consumer) return false; // 空指针校验

    // 需求2.3：验证消费者密码
    if (!consumer->checkPassword(enteredPassword)) {
        std::cout << "支付失败：密码错误。" << std::endl;
        order->setStatus(OrderStatus::FailedPayment); // 标记支付失败状态
        persistChanges(); // 保存订单状态变更
        return false;
    }

    // 需求2.2：检查是否有未支付的旧订单
    std::vector<Order*> pendingOrders = getPendingOrdersByConsumer(consumer->getUsername());
    for (Order* pending : pendingOrders) {
        // 当前订单ID不同且创建时间更早的未支付订单需优先处理
        if (pending->getID() != order->getID() && pending->getCreationTime() < order->getCreationTime()) {
            std::cout << "支付失败：您有未支付的旧订单（ID：" << pending->getID()
                << "），请先完成支付或取消。" << std::endl;
            order->setStatus(OrderStatus::FailedPayment); // 标记当前支付尝试失败
            persistChanges();
            return false;
        }
    }


    // 校验订单状态是否为待支付或支付失败（允许重试支付）
    if (order->getStatus() != OrderStatus::PendingPayment && order->getStatus() != OrderStatus::FailedPayment) {
        std::cout << "支付失败：订单" << order->getID() << "并非待支付状态（状态："
            << orderStatusToString(order->getStatus()) << "）。" << std::endl;
        return false;
    }

    // 校验消费者余额是否足够
    if (consumer->getBalance() < order->getTotalAmount()) {
        std::cout << "支付失败：余额不足。需要：$"
            << order->getTotalAmount() << "，可用：$" << consumer->getBalance() << std::endl;
        order->setStatus(OrderStatus::FailedPayment);
        persistChanges();
        return false;
    }

    // 从消费者账户扣款
    if (!consumer->withdraw(order->getTotalAmount())) {
        std::cout << "支付失败：从消费者账户扣款时发生错误。" << std::endl;
        order->setStatus(OrderStatus::FailedPayment);
        persistChanges();
        return false;
    }

    // 向商家转账并确认库存销售
    for (const auto& item : order->getItems()) {
        User* merchantUser = um.findUser(item.merchantUsername); // 查找商家用户
        if (merchantUser && merchantUser->getUserType() == "Merchant") {
            merchantUser->deposit(item.getSubtotal()); // 商家收款（子金额=单价*数量）
        }
        else {
            std::cout << "警告：未找到商品" << item.productName << "的商家" << item.merchantUsername
                << "或非商家用户，资金未转移。" << std::endl;
            // 注意：此处可能导致消费者已扣款但商家未收款，需事务性处理（简化实现未处理）
        }

        Product* p = pm.findProductByID(item.productID); // 查找商品
        if (p) {
            if (!p->confirmSaleFromReserved(item.quantity)) { // 从预订库存确认销售（扣减实际库存）
                std::cerr << "严重错误：无法确认订单" << order->getID()
                    << "中" << item.quantity << "个商品" << item.productID
                    << "的库存销售，库存可能不一致。" << std::endl;
            }
        }
        else {
            std::cerr << "严重错误：确认订单" << order->getID()
                << "销售时未找到商品" << item.productID << "." << std::endl;
        }
    }

    order->setStatus(OrderStatus::Paid); // 标记订单为已支付

    um.persistChanges(); // 保存用户余额变更
    pm.persistChanges(); // 保存商品库存变更（扣除已预订库存）
    persistChanges();    // 保存订单状态变更

    std::cout << "订单" << order->getID() << "支付成功！" << std::endl;
    std::cout << "您的新余额：$" << std::fixed << std::setprecision(2) << consumer->getBalance() << std::endl;

    // 需求3：支付成功后从购物车移除对应商品
    ShoppingCart* cart = consumer->getShoppingCart();
    if (cart) {
        for (const auto& orderItem : order->getItems()) {
            CartItem* cartItemPtr = cart->getItem(orderItem.productID); // 查找购物车中的商品项
            if (cartItemPtr) {
                if (cartItemPtr->quantity == orderItem.quantity) { // 数量完全匹配，移除整个商品项
                    cart->clearItem(orderItem.productID);
                }
                else if (cartItemPtr->quantity > orderItem.quantity) { // 理论上不应出现（购物车数量应等于订单数量）
                    cart->removeItem(orderItem.productID, orderItem.quantity); // 调整数量（简化处理）
                }
                // 若购物车数量小于订单数量：视为数据不一致，不处理
            }
        }
    }

    return true;
}


// 根据订单ID查找订单（内部工具方法）
Order* OrderManager::findOrderById(const std::string& orderID) {
    // 使用lambda表达式在订单列表中查找匹配ID的订单
    auto it = std::find_if(allOrders.begin(), allOrders.end(),
        [&](const Order* o) { return o->getID() == orderID; });
    if (it != allOrders.end()) {
        return *it; // 返回找到的订单指针
    }
    return nullptr; // 未找到返回空指针
}

// 根据消费者用户名获取订单列表（支持筛选待支付订单）
std::vector<Order*> OrderManager::getOrdersByConsumer(const std::string& consumerUsername, bool pendingOnly) {
    std::vector<Order*> results; // 结果列表
    for (Order* order : allOrders) {
        if (order->getConsumerUsername() == consumerUsername) { // 过滤属于该消费者的订单
            if (pendingOnly) { // 仅筛选待支付或支付失败的订单
                if (order->getStatus() == OrderStatus::PendingPayment || order->getStatus() == OrderStatus::FailedPayment) {
                    results.push_back(order);
                }
            }
            else { // 返回所有状态订单
                results.push_back(order);
            }
        }
    }
    // 按创建时间排序（默认：最早的在前）
    std::sort(results.begin(), results.end(), [](const Order* a, const Order* b) {
        return a->getCreationTime() < b->getCreationTime(); // 升序排列（ oldest first ）
        });
    return results;
}


// 获取消费者的待支付订单（快捷方法）
std::vector<Order*> OrderManager::getPendingOrdersByConsumer(const std::string& consumerUsername) {
    return getOrdersByConsumer(consumerUsername, true); // 调用通用方法并筛选待支付状态
}