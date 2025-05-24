#include <iostream>
#include <string>
#include <vector>
#include <limits> 
#include <algorithm> 
#include <iomanip> 
#include <set>
#include <chrono> // 用于订单超时 - 但由于控制台特性未完全实现

#include "UserManager.h"
#include "ProductManager.h"
#include "ShoppingCart.h" // 通过Users.h（Consumer）包含
#include "OrderManager.h"   // 新管理器
// User.h、Product.h、Order.h 通过各自的管理器或Consumer包含

// 菜单函数前置声明
void showMainMenu(UserManager& um, ProductManager& pm, OrderManager& om, User*& currentUser);
void showConsumerMenu(UserManager& um, ProductManager& pm, OrderManager& om, User*& currentUser);
void showMerchantMenu(UserManager& um, ProductManager& pm, OrderManager& om, User*& currentUser); // 添加OrderManager

// 健壮输入辅助函数（无变化）
template <typename T>
T getValidatedInput(const std::string& prompt) {
    T value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.good()) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        else {
            std::cout << "输入无效。请重试。" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

std::string getLineInput(const std::string& prompt, bool allowEmpty = false) {
    std::string value;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, value);
        if (allowEmpty || !value.empty()) {
            return value;
        }
        std::cout << "输入不能为空。请重试。" << std::endl;
    }
}

// 用户相关操作（无显著变化，确保调用persistChanges）
void handleRegister(UserManager& um) {
    std::cout << "\n--- 用户注册 ---" << std::endl;
    std::string uname = getLineInput("请输入用户名: ");
    if (um.isUsernameTaken(uname)) {
        std::cout << "用户名已存在。请尝试其他用户名。" << std::endl;
        return;
    }
    std::string pwd1, pwd2;
    do {
        pwd1 = getLineInput("请输入密码（不能为空）: ");
        pwd2 = getLineInput("确认密码: ");
        if (pwd1 != pwd2) {
            std::cout << "密码不匹配。请重试。" << std::endl;
        }
    } while (pwd1 != pwd2);
    std::string typeChoice;
    std::cout << "注册为 (1) 消费者 或 (2) 商家: ";
    std::getline(std::cin, typeChoice);
    std::string accountType;
    if (typeChoice == "1") accountType = "Consumer";
    else if (typeChoice == "2") accountType = "Merchant";
    else {
        std::cout << "无效选择。注册失败。" << std::endl;
        return;
    }
    if (um.registerUser(uname, pwd1, accountType)) {
        std::cout << "注册成功，身份为 " << accountType << "!" << std::endl;
    }
    else {
        std::cout << "注册失败。用户名可能已存在或类型无效。" << std::endl;
    }
}

void handleLogin(UserManager& um, User*& currentUser) {
    std::cout << "\n--- 用户登录 ---" << std::endl;
    if (currentUser) {
        std::cout << "已登录用户: " << currentUser->getUsername() << std::endl;
        return;
    }
    std::string uname = getLineInput("请输入用户名: ");
    std::string pwd = getLineInput("请输入密码: ");
    currentUser = um.loginUser(uname, pwd);
    if (currentUser) {
        std::cout << "登录成功。欢迎你，" << currentUser->getUsername() << "!" << std::endl;
    }
    else {
        std::cout << "登录失败。用户名或密码错误。" << std::endl;
    }
}

void handleLogout(User*& currentUser, UserManager& um, ProductManager& pm, OrderManager& om) { // 添加PM、OM
    if (currentUser) {
        std::cout << "正在退出 " << currentUser->getUsername() << "。" << std::endl;
        um.persistChanges();
        pm.persistChanges(); // 持久化可能发生的产品库存变更
        om.persistChanges(); // 持久化任何订单状态变更
        currentUser = nullptr;
    }
    else {
        std::cout << "未登录。" << std::endl;
    }
}

void handleChangePassword(UserManager& um, User* currentUser) {
    if (!currentUser) {
        std::cout << "你必须先登录才能修改密码。" << std::endl;
        return;
    }
    std::cout << "\n--- 修改密码 ---" << std::endl;
    std::string oldPwd = getLineInput("请输入当前密码: ");
    if (!currentUser->checkPassword(oldPwd)) {
        std::cout << "当前密码错误。" << std::endl;
        return;
    }
    std::string pwd1, pwd2;
    do {
        pwd1 = getLineInput("请输入新密码（不能为空）: ");
        pwd2 = getLineInput("确认新密码: ");
        if (pwd1 != pwd2) {
            std::cout << "新密码不匹配。请重试。" << std::endl;
        }
    } while (pwd1 != pwd2);
    currentUser->setPassword(pwd1);
    um.persistChanges();
    std::cout << "密码修改成功。" << std::endl;
}

void handleBalanceManagement(UserManager& um, User* currentUser) {
    if (!currentUser) {
        std::cout << "你必须先登录才能管理余额。" << std::endl;
        return;
    }
    std::cout << "\n--- 余额管理 ---" << std::endl;
    std::cout << "当前余额: $" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
    std::cout << "1. 充值余额" << std::endl;
    std::cout << "2. 查看余额（已显示）" << std::endl;
    std::cout << "0. 返回" << std::endl;
    std::string choiceStr = getLineInput("你的选择: ");
    int choice = -1;
    try { choice = std::stoi(choiceStr); }
    catch (const std::exception&) { std::cout << "输入无效。" << std::endl; return; }
    if (choice == 1) {
        double amount = getValidatedInput<double>("请输入充值金额: $");
        if (amount <= 0) {
            std::cout << "充值金额必须为正数。" << std::endl; return;
        }
        currentUser->deposit(amount);
        um.persistChanges();
        std::cout << "充值成功。新余额: $" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
    }
    else if (choice == 0) { return; }
    else if (choice != 2) { std::cout << "无效选择。" << std::endl; }
}

// 产品相关操作（通用 - 无重大变化）
void displayProducts(const std::vector<Product*>& productList, bool showStockDetail = true) { // 添加showStockDetail
    if (productList.empty()) {
        std::cout << "无产品可显示。" << std::endl;
        return;
    }
    std::cout << "\n--- 可用产品 ---" << std::endl;
    for (const auto* product : productList) {
        // product->displayDetails(); // 已显示详细库存
        // 对于购物车视图，可能需要更少细节，但displayDetails足够
        if (showStockDetail) {
            product->displayDetails();
        }
        else { // 用于快速浏览的更紧凑视图
            std::cout << "-------------------------------------\n"
                << "ID: " << product->getID() << ", 名称: " << product->getName() << "\n"
                << "价格: $" << std::fixed << std::setprecision(2) << product->getCurrentSalePrice()
                << ", 可用库存: " << product->getAvailableStock() << "\n"
                << "销售方: " << product->getOwnerMerchantUsername() << "\n"
                << "-------------------------------------" << std::endl;
        }
    }
}

void handleDisplayAllProducts(ProductManager& pm, bool compact = false) {
    displayProducts(pm.getAllProducts(), !compact);
}

void handleSearchProducts(ProductManager& pm) {
    std::cout << "\n--- 搜索产品 ---" << std::endl;
    std::cout << "搜索方式 (1) 名称, (2) 类型, (3) 商家: ";
    std::string choiceStr = getLineInput("");
    int choice = -1;
    try { choice = std::stoi(choiceStr); }
    catch (const std::exception&) { std::cout << "输入无效。" << std::endl; return; }
    std::string searchTerm = getLineInput("请输入搜索词: ");
    std::string searchBy;
    if (choice == 1) searchBy = "name";
    else if (choice == 2) searchBy = "type";
    else if (choice == 3) searchBy = "merchant";
    else { std::cout << "无效搜索条件。" << std::endl; return; }
    std::vector<Product*> results = pm.searchProducts(searchTerm, searchBy);
    if (results.empty()) {
        std::cout << "未找到符合条件的产品。" << std::endl;
    }
    else {
        std::cout << "--- 搜索结果 ---" << std::endl;
        displayProducts(results);
    }
}

// --- 消费者专用购物车操作 ---
void handleManageShoppingCart(Consumer* consumer, ProductManager& pm) {
    if (!consumer) { std::cout << "错误：未以消费者身份登录。" << std::endl; return; }
    ShoppingCart* cart = consumer->getShoppingCart();

    while (true) {
        cart->displayCart();
        std::cout << "\n--- 购物车菜单 ---" << std::endl;
        std::cout << "1. 添加产品到购物车" << std::endl;
        std::cout << "2. 更新购物车中产品数量" << std::endl;
        std::cout << "3. 从购物车移除产品（或数量）" << std::endl;
        std::cout << "4. 清空购物车" << std::endl;
        std::cout << "0. 返回消费者菜单" << std::endl;
        std::string choiceStr = getLineInput("你的选择: ");
        int choice = -1;
        try { choice = std::stoi(choiceStr); }
        catch (const std::exception&) { choice = -99; }

        switch (choice) {
        case 1: {
            handleDisplayAllProducts(pm, true); // 显示带详情的产品
            std::string pid = getLineInput("请输入要添加的产品ID: ");
            Product* p = pm.findProductByID(pid);
            if (!p) { std::cout << "产品未找到。" << std::endl; break; }
            if (p->getAvailableStock() == 0) { std::cout << "产品'" << p->getName() << "'已售罄。" << std::endl; break; }
            int qty = getValidatedInput<int>("请输入数量: ");
            cart->addItem(p, qty);
            break;
        }
        case 2: {
            if (cart->isEmpty()) { std::cout << "购物车为空。" << std::endl; break; }
            std::string pid = getLineInput("请输入购物车中要更新的产品ID: ");
            int newQty = getValidatedInput<int>("请输入新数量（0表示移除）: ");
            cart->updateItemQuantity(pid, newQty);
            break;
        }
        case 3: {
            if (cart->isEmpty()) { std::cout << "购物车为空。" << std::endl; break; }
            std::string pid = getLineInput("请输入购物车中要移除/减少的产品ID: ");
            int qtyRemove = getValidatedInput<int>("请输入移除数量（输入当前数量以全部移除）: ");
            cart->removeItem(pid, qtyRemove);
            break;
        }
        case 4:
            cart->clearCart();
            break;
        case 0:
            return;
        default:
            std::cout << "无效选择。" << std::endl;
        }
        // 购物车无需持久化，因为会话内存中存储
    }
}

// --- 消费者专用订单操作 ---
void handleCreateOrder(Consumer* consumer, ProductManager& pm, OrderManager& om) {
    if (!consumer) { std::cout << "错误：未登录。" << std::endl; return; }
    if (consumer->getShoppingCart()->isEmpty()) {
        std::cout << "你的购物车为空。请添加商品后创建订单。" << std::endl;
        return;
    }
    consumer->getShoppingCart()->displayCart();
    std::string confirm = getLineInput("确认使用这些商品创建订单？(y/n): ");
    if (confirm == "y" || confirm == "Y") {
        Order* newOrder = om.createOrderFromCart(consumer, pm);
        if (newOrder) {
            // newOrder->displayOrderDetails(); // 创建成功时已由createOrderFromCart显示
            // 购物车此处不清空，仅在支付成功后清除（需求3）
        }
        else {
            std::cout << "订单创建失败。请检查产品库存或购物车。" << std::endl;
        }
    }
    else {
        std::cout << "订单创建已取消。" << std::endl;
    }
}

void handleViewMyOrders(Consumer* consumer, OrderManager& om) {
    if (!consumer) { std::cout << "错误：未登录。" << std::endl; return; }
    std::vector<Order*> orders = om.getOrdersByConsumer(consumer->getUsername());
    if (orders.empty()) {
        std::cout << "你没有任何订单。" << std::endl;
        return;
    }
    std::cout << "\n--- 你的订单 --- (按时间顺序，最早在前)" << std::endl;
    for (const auto* order : orders) {
        order->displayOrderDetails();
    }
}

void handlePayForOrder(Consumer* consumer, OrderManager& om, UserManager& um, ProductManager& pm) {
    if (!consumer) { std::cout << "错误：未登录。" << std::endl; return; }

    std::vector<Order*> pendingOrders = om.getPendingOrdersByConsumer(consumer->getUsername());
    if (pendingOrders.empty()) {
        std::cout << "你没有待支付的订单。" << std::endl;
        return;
    }

    std::cout << "\n--- 你待支付的订单 --- (按时间顺序，最早在前)" << std::endl;
    for (size_t i = 0; i < pendingOrders.size(); ++i) {
        std::cout << (i + 1) << ". ";
        pendingOrders[i]->displayOrderDetails(); // 显示简要摘要或完整信息
    }

    std::string orderIdToPay = getLineInput("请输入要支付的订单ID（或0取消）: ");
    if (orderIdToPay == "0") return;

    Order* order = om.findOrderById(orderIdToPay);
    if (!order || order->getConsumerUsername() != consumer->getUsername()) {
        std::cout << "订单未找到或不属于你。" << std::endl;
        return;
    }
    if (order->getStatus() != OrderStatus::PendingPayment && order->getStatus() != OrderStatus::FailedPayment) {
        std::cout << "该订单当前不可支付（状态: " << orderStatusToString(order->getStatus()) << "）。" << std::endl;
        return;
    }

    // 支付尝试前立即重新检查是否有更早的未支付订单
    // （processPayment内部已处理，但此处可提前检查）
    std::vector<Order*> allUserPending = om.getPendingOrdersByConsumer(consumer->getUsername());
    for (Order* po : allUserPending) {
        if (po->getID() != order->getID() && po->getCreationTime() < order->getCreationTime()) {
            std::cout << "你必须先支付或取消更早的订单 " << po->getID() << "。" << std::endl;
            return;
        }
    }

    std::cout << "待支付订单: " << std::endl;
    order->displayOrderDetails();
    std::cout << "你的当前余额: $" << std::fixed << std::setprecision(2) << consumer->getBalance() << std::endl;
    if (consumer->getBalance() < order->getTotalAmount()) {
        std::cout << "余额不足，无法支付该订单。" << std::endl;
        return;
    }

    std::string pwd = getLineInput("请输入密码确认支付: ");
    if (om.processPayment(order, consumer, pwd, um, pm)) {
        // 支付成功消息由processPayment处理
        // 已支付商品的购物车清理由processPayment处理
    }
    else {
        // 支付失败消息由processPayment处理
        std::cout << "请检查订单状态或重试。" << std::endl;
    }
}

void handleCancelOrder(Consumer* consumer, OrderManager& om, ProductManager& pm) {
    if (!consumer) { std::cout << "错误: 未登录。" << std::endl; return; }

    std::vector<Order*> pendingOrders = om.getPendingOrdersByConsumer(consumer->getUsername());
    if (pendingOrders.empty()) {
        std::cout << "你没有待支付的订单可取消。" << std::endl;
        return;
    }

    std::cout << "\n--- 你可取消的订单 --- (最早的在前)" << std::endl;
    for (size_t i = 0; i < pendingOrders.size(); ++i) {
        std::cout << (i + 1) << ". ";
        // 只显示摘要以避免过多文本
        std::cout << "订单ID: " << pendingOrders[i]->getID()
            << ", 状态: " << orderStatusToString(pendingOrders[i]->getStatus())
            << ", 总价: $" << std::fixed << std::setprecision(2) << pendingOrders[i]->getTotalAmount() << std::endl;
    }

    std::string orderIdToCancel = getLineInput("输入要取消的订单ID (或0返回): ");
    if (orderIdToCancel == "0") return;

    if (om.cancelOrder(orderIdToCancel, pm)) {
        // 成功消息由cancelOrder处理
    }
    else {
        // 失败消息由cancelOrder处理
    }
}

// --- 商家特定操作 (来自任务1, 确保正确传递ProductManager和User) ---
void handleAddProduct(ProductManager& pm, User* merchantUser) {
    if (!merchantUser || merchantUser->getUserType() != "Merchant") {
        std::cout << "错误: 必须以商家身份登录。" << std::endl; return;
    }
    Merchant* merchant = static_cast<Merchant*>(merchantUser);
    std::cout << "\n--- 添加新产品 ---" << std::endl;
    std::string name = getLineInput("输入产品名称: ");
    std::string description = getLineInput("输入产品描述: ");
    double originalPrice = -1.0;
    while (originalPrice < 0) {
        originalPrice = getValidatedInput<double>("输入产品原价: $");
        if (originalPrice < 0) std::cout << "价格不能为负。" << std::endl;
    }
    int stock = -1; // 这是初始总库存
    while (stock < 0) {
        stock = getValidatedInput<int>("输入产品初始总库存数量: ");
        if (stock < 0) std::cout << "库存不能为负。" << std::endl;
    }
    std::string productTypeChoice, productType;
    while (true) {
        std::cout << "选择产品类别:\n1. 书籍\n2. 食品\n3. 服装\n你的选择: ";
        std::getline(std::cin, productTypeChoice);
        if (productTypeChoice == "1") { productType = "Book"; break; }
        if (productTypeChoice == "2") { productType = "Food"; break; }
        if (productTypeChoice == "3") { productType = "Clothing"; break; }
        std::cout << "无效选择。" << std::endl;
    }
    if (pm.addProduct(name, description, originalPrice, stock, merchant->getUsername(), productType)) {
        std::cout << "产品 '" << name << "' 添加成功。" << std::endl;
    }
    else {
        std::cout << "添加产品失败。" << std::endl;
    }
}

void handleManageMyProducts(ProductManager& pm, User* merchantUser) {
    if (!merchantUser || merchantUser->getUserType() != "Merchant") {
        std::cout << "错误: 必须以商家身份登录。" << std::endl; return;
    }
    Merchant* merchant = static_cast<Merchant*>(merchantUser);
    std::cout << "\n--- 管理我的产品 ---" << std::endl;
    std::vector<Product*> myProducts = pm.getProductsByMerchant(merchant->getUsername());
    if (myProducts.empty()) {
        std::cout << "你还没有添加任何产品。" << std::endl; return;
    }
    displayProducts(myProducts);
    std::string productID = getLineInput("输入要管理的产品ID (或0取消): ");
    if (productID == "0") return;
    Product* product = pm.findProductByID(productID);
    if (!product || product->getOwnerMerchantUsername() != merchant->getUsername()) {
        std::cout << "产品ID未找到或你不拥有此产品。" << std::endl; return;
    }
    std::cout << "\n管理产品: " << product->getName() << " (ID: " << product->getID() << ")" << std::endl;
    std::cout << "1. 更新价格 (原价和售价)" << std::endl;
    std::cout << "2. 更新总库存" << std::endl; // 从更新库存改为更新总库存
    std::cout << "3. 设置折扣 (为该特定产品更新售价)" << std::endl;
    std::cout << "4. 移除产品" << std::endl;
    std::cout << "5. 更新描述" << std::endl;
    std::cout << "6. 更新名称" << std::endl;
    std::cout << "0. 返回" << std::endl;
    std::string choiceStr = getLineInput("你的选择: ");
    int choice = -1;
    try { choice = std::stoi(choiceStr); }
    catch (const std::exception&) { std::cout << "输入无效。" << std::endl; return; }
    bool changed = false;
    switch (choice) {
    case 1: { /* 价格更新逻辑 */
        double newOrigPrice = -1.0;
        while (newOrigPrice < 0) { newOrigPrice = getValidatedInput<double>("新原价: $"); if (newOrigPrice < 0) std::cout << "无效\n"; }
        product->setOriginalPrice(newOrigPrice);
        double newSalePrice = -1.0;
        while (newSalePrice < 0 || newSalePrice > newOrigPrice) { newSalePrice = getValidatedInput<double>("新售价: $"); if (newSalePrice<0 || newSalePrice > newOrigPrice) std::cout << "无效\n"; }
        product->setCurrentSalePrice(newSalePrice);
        changed = true; break;
    }
    case 2: { // 更新总库存
        int newTotalStock = -1;
        while (newTotalStock < product->getReservedStock()) { // 确保新总库存不小于已预留库存
            newTotalStock = getValidatedInput<int>("输入新的总库存数量: ");
            if (newTotalStock < product->getReservedStock()) {
                std::cout << "总库存不能小于当前已预留库存 ("
                    << product->getReservedStock() << ")。请输入更高的值。" << std::endl;
            }
            else if (newTotalStock < 0) {
                std::cout << "库存不能为负。\n";
            }
        }
        product->setTotalStock(newTotalStock); // 使用setTotalStock
        changed = true; break;
    }
    case 3: { /* 特定产品的折扣 */
        double disc = -1.0;
        while (disc < 0 || disc > 100) { disc = getValidatedInput<double>("折扣百分比 (0-100): "); if (disc < 0 || disc>100) std::cout << "无效\n"; }
        product->setCurrentSalePrice(product->getOriginalPrice() * (1.0 - (disc / 100.0)));
        changed = true; break;
    }
    case 4: { /* 移除产品 */
        std::string conf = getLineInput("确定? (y/n):");
        if (conf == "y" || conf == "Y") { if (pm.removeProduct(product->getID(), merchant->getUsername())) { std::cout << "已移除。\n"; return; } else { std::cout << "失败。\n"; } }
        break; // 不需要changed = true，因为removeProduct会保存
    }
    case 5: { product->setDescription(getLineInput("新描述: ")); changed = true; break; }
    case 6: { product->setName(getLineInput("新名称: ")); changed = true; break; }
    case 0: return;
    default: std::cout << "无效选择。" << std::endl; break;
    }
    if (changed) {
        pm.updateProduct(product);
        std::cout << "产品详情已更新。" << std::endl;
    }
}

void handleDiscountCategory(ProductManager& pm, User* merchantUser) {
    // 此函数与任务1更新基本相同，使用pm.applyCategoryDiscount
    if (!merchantUser || merchantUser->getUserType() != "Merchant") {
        std::cout << "错误: 必须以商家身份登录。" << std::endl; return;
    }
    Merchant* merchant = static_cast<Merchant*>(merchantUser);
    std::cout << "\n--- 按类别设置产品折扣 (针对你的产品) ---" << std::endl;
    // ... (函数其余部分与任务1的先前版本相同) ...
    // 显示商家的类别和所有平台类别
    std::vector<Product*> myProducts = pm.getProductsByMerchant(merchant->getUsername());
    std::set<std::string> myTypesSet;
    for (const auto* p : myProducts) myTypesSet.insert(p->getProductType());
    std::cout << "你的产品类别: ";
    if (myTypesSet.empty()) std::cout << "无。 "; else for (const auto& type : myTypesSet) std::cout << type << " ";
    std::cout << "\n所有平台类别: ";
    std::vector<std::string> allTypes = pm.getAvailableProductTypes();
    if (allTypes.empty()) std::cout << "无。"; else for (const auto& type : allTypes) std::cout << type << " ";
    std::cout << std::endl;

    std::string categoryToDiscount = getLineInput("输入要应用/更新折扣的产品类别: ");
    double discountPercent = -1.0;
    while (discountPercent < 0 || discountPercent > 100) {
        discountPercent = getValidatedInput<double>("输入折扣百分比 (0-100, 0表示移除折扣): ");
        if (discountPercent < 0 || discountPercent > 100) std::cout << "折扣必须在0到100之间。\n";
    }
    pm.applyCategoryDiscount(merchant->getUsername(), categoryToDiscount, discountPercent);
}

// --- 菜单函数 ---
void showMainMenu(UserManager& um, ProductManager& pm, OrderManager& om, User*& currentUser) { // 添加OM
    std::string choiceStr; int choice = -1;
    while (true) {
        std::cout << "\n========= 电子商务平台 =========" << std::endl;
        if (currentUser) {
            std::cout << "当前登录: " << currentUser->getUsername()
                << " (" << currentUser->getUserType()
                << ") | 余额: $" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
        }
        else { std::cout << "欢迎, 访客!" << std::endl; }
        std::cout << "---------------------------------------" << std::endl;
        std::cout << "1. 显示所有产品" << std::endl;
        std::cout << "2. 搜索产品" << std::endl;
        if (!currentUser) {
            std::cout << "3. 注册" << std::endl;
            std::cout << "4. 登录" << std::endl;
        }
        else {
            std::cout << "3. 我的账户选项" << std::endl;
            std::cout << "4. 登出" << std::endl;
        }
        std::cout << "0. 退出" << std::endl;
        std::cout << "---------------------------------------" << std::endl;
        choiceStr = getLineInput("你的选择: ");
        try { choice = std::stoi(choiceStr); }
        catch (const std::exception&) { choice = -1; }

        switch (choice) {
        case 1: handleDisplayAllProducts(pm); break;
        case 2: handleSearchProducts(pm); break;
        case 3:
            if (!currentUser) handleRegister(um);
            else {
                if (currentUser->getUserType() == "Consumer") {
                    showConsumerMenu(um, pm, om, currentUser); // 传递OM
                }
                else if (currentUser->getUserType() == "Merchant") {
                    showMerchantMenu(um, pm, om, currentUser); // 传递OM
                }
            }
            break;
        case 4:
            if (!currentUser) handleLogin(um, currentUser);
            else handleLogout(currentUser, um, pm, om); // 传递PM, OM
            break;
        case 0:
            std::cout << "正在退出平台。再见!" << std::endl;
            if (currentUser) um.persistChanges();
            pm.persistChanges();
            om.persistChanges(); // 持久化订单
            return;
        default:
            std::cout << "无效选择。请重试。" << std::endl;
        }
    }
}

void showConsumerMenu(UserManager& um, ProductManager& pm, OrderManager& om, User*& currentUser) { // 添加OM
    if (!currentUser || currentUser->getUserType() != "Consumer") return;
    Consumer* consumer = static_cast<Consumer*>(currentUser);
    std::string choiceStr; int choice = -1;

    while (currentUser && currentUser->getUserType() == "Consumer") { // 在循环中检查currentUser以处理登出
        std::cout << "\n--- 消费者菜单 (" << consumer->getUsername() << ") ---" << std::endl;
        std::cout << "余额: $" << std::fixed << std::setprecision(2) << consumer->getBalance() << std::endl;
        std::cout << "1. 查看所有产品" << std::endl;
        std::cout << "2. 搜索产品" << std::endl;
        std::cout << "3. 管理购物车" << std::endl;
        std::cout << "4. 从购物车创建订单" << std::endl;
        std::cout << "5. 查看我的订单" << std::endl;
        std::cout << "6. 支付订单" << std::endl;
        std::cout << "7. 取消待处理订单" << std::endl;
        std::cout << "8. 管理余额" << std::endl;
        std::cout << "9. 更改密码" << std::endl;
        std::cout << "0. 返回主菜单 (登出)" << std::endl;
        choiceStr = getLineInput("你的选择: ");
        try { choice = std::stoi(choiceStr); }
        catch (const std::exception&) { choice = -1; }

        // 如果currentUser变为null(由于在处理程序中登出)，则跳出
        if (!currentUser) break;

        switch (choice) {
        case 1: handleDisplayAllProducts(pm); break;
        case 2: handleSearchProducts(pm); break;
        case 3: handleManageShoppingCart(consumer, pm); break;
        case 4: handleCreateOrder(consumer, pm, om); break;
        case 5: handleViewMyOrders(consumer, om); break;
        case 6: handlePayForOrder(consumer, om, um, pm); break;
        case 7: handleCancelOrder(consumer, om, pm); break;
        case 8: handleBalanceManagement(um, currentUser); break;
        case 9: handleChangePassword(um, currentUser); break;
        case 0:
            handleLogout(currentUser, um, pm, om); // 登出将设置currentUser为null
            return; // 退出消费者菜单
        default:
            std::cout << "无效选择。请重试。" << std::endl;
        }
    }
}

void showMerchantMenu(UserManager& um, ProductManager& pm, OrderManager& om, User*& currentUser) { // 添加OM
    std::string choiceStr; int choice = -1;
    while (currentUser && currentUser->getUserType() == "Merchant") { // 检查currentUser
        std::cout << "\n--- 商家菜单 (" << currentUser->getUsername() << ") ---" << std::endl;
        std::cout << "余额: $" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
        std::cout << "1. 添加新产品" << std::endl;
        std::cout << "2. 管理我的产品" << std::endl;
        std::cout << "3. 按类别设置/更新折扣" << std::endl;
        std::cout << "4. 查看所有平台产品" << std::endl;
        std::cout << "5. 搜索平台产品" << std::endl;
        std::cout << "6. 管理余额" << std::endl;
        std::cout << "7. 更改密码" << std::endl;
        std::cout << "0. 返回主菜单 (登出)" << std::endl;
        choiceStr = getLineInput("你的选择: ");
        try { choice = std::stoi(choiceStr); }
        catch (const std::exception&) { choice = -1; }

        if (!currentUser) break; // 输入后再次检查

        switch (choice) {
        case 1: handleAddProduct(pm, currentUser); break;
        case 2: handleManageMyProducts(pm, currentUser); break;
        case 3: handleDiscountCategory(pm, currentUser); break;
        case 4: handleDisplayAllProducts(pm); break;
        case 5: handleSearchProducts(pm); break;
        case 6: handleBalanceManagement(um, currentUser); break;
        case 7: handleChangePassword(um, currentUser); break;
        case 0:
            handleLogout(currentUser, um, pm, om);
            return; // 退出商家菜单
        default:
            std::cout << "无效选择。请重试。" << std::endl;
        }
    }
}

// --- 主函数 ---
int main() {
    UserManager userManager;
    ProductManager productManager;
    OrderManager orderManager(productManager); // 初始化OrderManager，传递ProductManager用于库存操作

    User* currentUser = nullptr;

    showMainMenu(userManager, productManager, orderManager, currentUser); // 传递OrderManager

    // PersistChanges在从主菜单退出和登出时调用。
    return 0;
}

// 确保'split'函数在某个可访问的位置定义。
// 如果它在UserManager.cpp中，并且OrderManager.cpp也通过'extern'使用它，
// 确保UserManager.cpp被编译并链接。
// 通用工具文件最适合'split'函数。