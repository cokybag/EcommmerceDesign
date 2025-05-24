// (main.cpp 第一部分中之前的包含文件和辅助函数)
#include <iostream>
#include <string>
#include <vector>
#include <limits> 
#include <algorithm> 
#include <iomanip> 
#include <set>

#include "UserManager.h"
#include "ProductManager.h"
// User.h 和 Product.h 通过 UserManager.h 和 ProductManager.h 包含进来

// 菜单函数的前向声明
void showMainMenu(UserManager& um, ProductManager& pm, User*& currentUser);
void showConsumerMenu(UserManager& um, ProductManager& pm, User*& currentUser);
void showMerchantMenu(UserManager& um, ProductManager& pm, User*& currentUser);

// 健壮输入辅助函数
template <typename T>
T getValidatedInput(const std::string& prompt) {
    T value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.good()) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 清空缓冲区
            return value;
        }
        else {
            std::cout << "输入无效，请重试。" << std::endl;
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
        std::cout << "输入不能为空，请重试。" << std::endl;
    }
}


// --- 用户相关操作 --- 
void handleRegister(UserManager& um) {
    std::cout << "\n--- 用户注册 ---" << std::endl;
    std::string uname = getLineInput("请输入用户名：");
    if (um.isUsernameTaken(uname)) {
        std::cout << "用户名已被占用，请尝试其他用户名。" << std::endl;
        return;
    }

    std::string pwd1, pwd2;
    do {
        pwd1 = getLineInput("请输入密码（不能为空）：");
        pwd2 = getLineInput("请确认密码：");
        if (pwd1 != pwd2) {
            std::cout << "密码不一致，请重试。" << std::endl;
        }
    } while (pwd1 != pwd2);

    std::string typeChoice;
    std::cout << "注册类型（1）消费者 或（2）商家：";
    std::getline(std::cin, typeChoice);

    std::string accountType;
    if (typeChoice == "1") accountType = "Consumer";
    else if (typeChoice == "2") accountType = "Merchant";
    else {
        std::cout << "选择无效，注册失败。" << std::endl;
        return;
    }

    if (um.registerUser(uname, pwd1, accountType)) {
        std::cout << "注册成功，类型为 " << accountType << "！" << std::endl;
    }
    else {
        std::cout << "注册失败，用户名可能已被占用或类型无效。" << std::endl;
    }
}

void handleLogin(UserManager& um, User*& currentUser) {
    std::cout << "\n--- 用户登录 ---" << std::endl;
    if (currentUser) {
        std::cout << "已登录，用户：" << currentUser->getUsername() << std::endl;
        return;
    }
    std::string uname = getLineInput("请输入用户名：");
    std::string pwd = getLineInput("请输入密码：");

    currentUser = um.loginUser(uname, pwd);
    if (currentUser) {
        std::cout << "登录成功，欢迎 " << currentUser->getUsername() << "！" << std::endl;
    }
    else {
        std::cout << "登录失败，用户名或密码错误。" << std::endl;
    }
}

void handleLogout(User*& currentUser, UserManager& um) {
    if (currentUser) {
        std::cout << "正在退出 " << currentUser->getUsername() << "。" << std::endl;
        um.persistChanges();
        currentUser = nullptr;
    }
    else {
        std::cout << "未登录。" << std::endl;
    }
}

void handleChangePassword(UserManager& um, User* currentUser) {
    if (!currentUser) {
        std::cout << "您必须先登录才能修改密码。" << std::endl;
        return;
    }
    std::cout << "\n--- 修改密码 ---" << std::endl;
    std::string oldPwd = getLineInput("请输入当前密码：");
    if (!currentUser->checkPassword(oldPwd)) {
        std::cout << "当前密码错误。" << std::endl;
        return;
    }

    std::string pwd1, pwd2;
    do {
        pwd1 = getLineInput("请输入新密码（不能为空）：");
        pwd2 = getLineInput("请确认新密码：");
        if (pwd1 != pwd2) {
            std::cout << "新密码不一致，请重试。" << std::endl;
        }
    } while (pwd1 != pwd2);

    currentUser->setPassword(pwd1);
    um.persistChanges();
    std::cout << "密码修改成功。" << std::endl;
}

void handleBalanceManagement(UserManager& um, User* currentUser) {
    if (!currentUser) {
        std::cout << "您必须先登录才能管理余额。" << std::endl;
        return;
    }
    std::cout << "\n--- 余额管理 ---" << std::endl;
    std::cout << "当前余额：$" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
    std::cout << "1. 充值余额" << std::endl;
    std::cout << "2. 查看余额（已显示）" << std::endl;
    // 任务1中通常通过消费展示"Consume"部分。
    // 根据要求，任务1中移除了直接购买功能，
    // 因此必要时可以用通用的提现选项代表"消费"，
    // 或仅保留充值和查看功能即可满足余额管理需求。
    // 为简化起见，此处仅保留充值和查看功能。
    // 如需明确展示"消费"，可添加"提现"选项。
    std::cout << "0. 返回" << std::endl;

    std::string choiceStr = getLineInput("请选择：");
    int choice = -1;
    try {
        choice = std::stoi(choiceStr);
    }
    catch (const std::exception& e) {
        std::cout << "输入无效。" << std::endl;
        return;
    }

    if (choice == 1) {
        double amount = getValidatedInput<double>("请输入充值金额：$");
        if (amount <= 0) {
            std::cout << "充值金额必须为正数。" << std::endl;
            return;
        }
        currentUser->deposit(amount);
        um.persistChanges();
        std::cout << "充值成功，新余额：$" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
    }
    else if (choice == 0) {
        return;
    }
    else if (choice != 2) {
        std::cout << "选择无效。" << std::endl;
    }
}
// --- 用户相关操作结束 ---


// --- 商品相关操作（通用） ---
void displayProducts(const std::vector<Product*>& productList) {
    if (productList.empty()) {
        std::cout << "没有可显示的商品。" << std::endl;
        return;
    }
    std::cout << "\n--- 可用商品 ---" << std::endl;
    for (const auto* product : productList) {
        product->displayDetails();
    }
}

void handleDisplayAllProducts(ProductManager& pm) {
    displayProducts(pm.getAllProducts());
}

void handleSearchProducts(ProductManager& pm) {
    std::cout << "\n--- 搜索商品 ---" << std::endl;
    std::cout << "搜索条件（1）名称，（2）类型，（3）商家：";
    std::string choiceStr = getLineInput("");
    int choice = -1;
    try {
        choice = std::stoi(choiceStr);
    }
    catch (const std::exception& e) {
        std::cout << "输入无效。" << std::endl;
        return;
    }

    std::string searchTerm = getLineInput("请输入搜索词：");
    std::string searchBy;

    if (choice == 1) searchBy = "name";
    else if (choice == 2) searchBy = "type";
    else if (choice == 3) searchBy = "merchant";
    else {
        std::cout << "搜索条件无效。" << std::endl;
        return;
    }

    std::vector<Product*> results = pm.searchProducts(searchTerm, searchBy);
    if (results.empty()) {
        std::cout << "未找到符合条件的商品。" << std::endl;
    }
    else {
        std::cout << "--- 搜索结果 ---" << std::endl;
        displayProducts(results);
    }
}

// --- 消费者特定操作 ---
// 根据任务1简化要求，移除了handlePurchaseProduct函数
/*
void handlePurchaseProduct(UserManager& um, ProductManager& pm, User* consumerUser) {
    // ... 整个函数体已移除 ...
}
*/

// --- 商家特定操作 ---
void handleAddProduct(ProductManager& pm, User* merchantUser) {
    if (!merchantUser || merchantUser->getUserType() != "Merchant") {
        std::cout << "错误：必须以商家身份登录。" << std::endl;
        return;
    }
    Merchant* merchant = static_cast<Merchant*>(merchantUser);

    std::cout << "\n--- 添加新商品 ---" << std::endl;
    std::string name = getLineInput("请输入商品名称：");
    std::string description = getLineInput("请输入商品描述：");
    double originalPrice = -1.0;
    while (originalPrice < 0) {
        originalPrice = getValidatedInput<double>("请输入商品原价：$");
        if (originalPrice < 0) std::cout << "价格不能为负数。" << std::endl;
    }
    int stock = -1;
    while (stock < 0) {
        stock = getValidatedInput<int>("请输入商品库存数量：");
        if (stock < 0) std::cout << "库存不能为负数。" << std::endl;
    }

    std::string productTypeChoice;
    std::string productType; // 用于类实例化的"Book"、"Food"或"Clothing"
    while (true) {
        std::cout << "选择商品分类进行实例化：\n1. 书籍\n2. 食品\n3. 服装\n请选择：";
        std::getline(std::cin, productTypeChoice);
        if (productTypeChoice == "1") { productType = "Book"; break; }
        if (productTypeChoice == "2") { productType = "Food"; break; }
        if (productTypeChoice == "3") { productType = "Clothing"; break; }
        std::cout << "选择无效，请选择1、2或3作为基础商品类别。" << std::endl;
    }

    // 可选：允许商家指定更详细的类型字符串（如有需要），
    // 但C++类仍为上述三种之一。
    // 对于任务1，productType为"Book"、"Food"、"Clothing"已足够。

    if (pm.addProduct(name, description, originalPrice, stock, merchant->getUsername(), productType)) {
        std::cout << "商品'" << name << "'添加成功。" << std::endl;
        // 折扣应用将在pm.addProduct内部处理（如有规则存在）
    }
    else {
        std::cout << "添加商品失败。" << std::endl;
    }
}

void handleManageMyProducts(ProductManager& pm, User* merchantUser) {
    if (!merchantUser || merchantUser->getUserType() != "Merchant") {
        std::cout << "错误：必须以商家身份登录。" << std::endl;
        return;
    }
    Merchant* merchant = static_cast<Merchant*>(merchantUser);

    std::cout << "\n--- 管理我的商品 ---" << std::endl;
    std::vector<Product*> myProducts = pm.getProductsByMerchant(merchant->getUsername());
    if (myProducts.empty()) {
        std::cout << "您尚未添加任何商品。" << std::endl;
        return;
    }
    displayProducts(myProducts);

    std::string productID = getLineInput("请输入要管理的商品ID（或0取消）：");
    if (productID == "0") return;

    Product* product = pm.findProductByID(productID);
    if (!product || product->getOwnerMerchantUsername() != merchant->getUsername()) {
        std::cout << "商品ID未找到或您不拥有该商品。" << std::endl;
        return;
    }

    std::cout << "\n管理商品：" << product->getName() << "（ID：" << product->getID() << "）" << std::endl;
    std::cout << "1. 更新价格（原价&售价）" << std::endl;
    std::cout << "2. 更新库存" << std::endl;
    std::cout << "3. 设置折扣（更新此商品的售价）" << std::endl;
    std::cout << "4. 删除商品" << std::endl;
    std::cout << "5. 更新描述" << std::endl;
    std::cout << "6. 更新名称" << std::endl;
    std::cout << "0. 返回" << std::endl;

    std::string choiceStr = getLineInput("请选择：");
    int choice = -1;
    try {
        choice = std::stoi(choiceStr);
    }
    catch (const std::exception& e) {
        std::cout << "输入无效。" << std::endl;
        return;
    }

    bool changed = false;
    switch (choice) {
    case 1: {
        double newOrigPrice = -1.0;
        while (newOrigPrice < 0) {
            newOrigPrice = getValidatedInput<double>("请输入新原价：$");
            if (newOrigPrice < 0) std::cout << "价格不能为负数。\n";
        }
        product->setOriginalPrice(newOrigPrice);
        double newSalePrice = -1.0;
        while (newSalePrice < 0 || newSalePrice > newOrigPrice) {
            newSalePrice = getValidatedInput<double>("请输入新售价（不能超过原价）：$");
            if (newSalePrice < 0) std::cout << "价格不能为负数。\n";
            if (newSalePrice > newOrigPrice) std::cout << "售价不能超过原价。\n";
        }
        product->setCurrentSalePrice(newSalePrice);
        changed = true;
        break;
    }
    case 2: {
        int newStock = -1;
        while (newStock < 0) {
            newStock = getValidatedInput<int>("请输入新库存数量：");
            if (newStock < 0) std::cout << "库存不能为负数。\n";
        }
        product->setStock(newStock);
        changed = true;
        break;
    }
    case 3: {
        double discountPercent = -1.0;
        while (discountPercent < 0 || discountPercent > 100) {
            discountPercent = getValidatedInput<double>("请输入此商品的折扣百分比（如10表示9折）：");
            if (discountPercent < 0 || discountPercent > 100) std::cout << "折扣必须在0到100之间。\n";
        }
        double newSalePrice = product->getOriginalPrice() * (1.0 - (discountPercent / 100.0));
        product->setCurrentSalePrice(newSalePrice);
        std::cout << "此商品新售价已设置为：$" << std::fixed << std::setprecision(2) << product->getCurrentSalePrice() << std::endl;
        changed = true;
        break;
    }
    case 4: {
        std::string confirm = getLineInput("确认删除此商品？（y/n）：");
        if (confirm == "y" || confirm == "Y") {
            if (pm.removeProduct(product->getID(), merchant->getUsername())) {
                std::cout << "商品已删除。" << std::endl;
            }
            else {
                std::cout << "删除商品失败。" << std::endl;
            }
            return;
        }
        break;
    }
    case 5: {
        std::string newDesc = getLineInput("请输入新描述：");
        product->setDescription(newDesc);
        changed = true;
        break;
    }
    case 6: {
        std::string newName = getLineInput("请输入新名称：");
        product->setName(newName);
        changed = true;
        break;
    }
    case 0:
        return;
    default:
        std::cout << "选择无效。" << std::endl;
        break;
    }

    if (changed) {
        pm.updateProduct(product);
        std::cout << "商品详情已更新。" << std::endl;
    }
}

void handleDiscountCategory(ProductManager& pm, User* merchantUser) {
    if (!merchantUser || merchantUser->getUserType() != "Merchant") {
        std::cout << "错误：必须以商家身份登录。" << std::endl;
        return;
    }
    Merchant* merchant = static_cast<Merchant*>(merchantUser);

    std::cout << "\n--- 按类别设置商品折扣（针对您的商品） ---" << std::endl;

    std::vector<Product*> myProducts = pm.getProductsByMerchant(merchant->getUsername());
    if (myProducts.empty()) {
        std::cout << "您没有可设置折扣的商品。" << std::endl;
        return;
    }

    std::set<std::string> myTypesSet;
    for (const auto* p : myProducts) {
        myTypesSet.insert(p->getProductType());
    }
    if (myTypesSet.empty() && pm.getAvailableProductTypes().empty()) { // 检查商家类型和平台所有类型
        std::cout << "未找到您的商品或平台上的商品类别。" << std::endl;
        return;
    }

    std::cout << "您当前的商品类别：";
    if (myTypesSet.empty()) std::cout << "无。 ";
    for (const auto& type : myTypesSet) {
        std::cout << type << " ";
    }
    std::cout << "\n所有平台类别： ";
    std::vector<std::string> allTypes = pm.getAvailableProductTypes();
    if (allTypes.empty()) std::cout << "无。";
    for (const auto& type : allTypes) {
        std::cout << type << " ";
    }
    std::cout << std::endl;


    std::string categoryToDiscount = getLineInput("请输入要应用/更新折扣的商品类别（如Book、Food、Clothing）： ");
    // 验证类别字符串（可选，可允许任何字符串用于未来类型）
    // 为简单起见，此处期望为已知类型之一，但ProductManager可处理任何字符串。
    bool platformHasCategory = false;
    for (const auto& type : allTypes) {
        if (type == categoryToDiscount) {
            platformHasCategory = true;
            break;
        }
    }
    // 如果要严格限制，可检查所选类别是否为基本类别之一。
    if (categoryToDiscount != "Book" && categoryToDiscount != "Food" && categoryToDiscount != "Clothing" && !platformHasCategory) {
        std::cout << "警告：'" << categoryToDiscount << "'不是标准或现有类别。 "
            << "如果您计划添加此类商品，仍可为其设置折扣规则。" << std::endl;
    }


    double discountPercent = -1.0;
    while (discountPercent < 0 || discountPercent > 100) {
        discountPercent = getValidatedInput<double>("请输入折扣百分比（0-100，0表示移除折扣）： ");
        if (discountPercent < 0 || discountPercent > 100) std::cout << "折扣必须在0到100之间。\n";
    }

    // 使用新的ProductManager方法
    pm.applyCategoryDiscount(merchant->getUsername(), categoryToDiscount, discountPercent);
    // pm.applyCategoryDiscount方法将打印消息并保存更改。
    // 此处无需调用pm.persistChanges()，因为applyCategoryDiscount已调用saveProductsToFile。
}


// --- 菜单函数 ---
void showMainMenu(UserManager& um, ProductManager& pm, User*& currentUser) {
    std::string choiceStr;
    int choice = -1;

    while (true) {
        std::cout << "\n========= 电子商务平台 =========" << std::endl;
        if (currentUser) {
            std::cout << "当前登录：" << currentUser->getUsername()
                << " (" << currentUser->getUserType()
                << ") | 余额：$" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
        }
        else {
            std::cout << "欢迎，访客！" << std::endl;
        }
        std::cout << "---------------------------------------" << std::endl;
        std::cout << "1. 显示所有商品" << std::endl;
        std::cout << "2. 搜索商品" << std::endl;
        if (!currentUser) {
            std::cout << "3. 注册" << std::endl;
            std::cout << "4. 登录" << std::endl;
        }
        else {
            std::cout << "3. 我的账户选项" << std::endl;
            std::cout << "4. 退出登录" << std::endl;
        }
        std::cout << "0. 退出" << std::endl;
        std::cout << "---------------------------------------" << std::endl;

        choiceStr = getLineInput("请选择：");
        try {
            choice = std::stoi(choiceStr);
        }
        catch (const std::exception& e) {
            choice = -1;
        }

        switch (choice) {
        case 1: handleDisplayAllProducts(pm); break;
        case 2: handleSearchProducts(pm); break;
        case 3:
            if (!currentUser) handleRegister(um);
            else {
                if (currentUser->getUserType() == "Consumer") {
                    showConsumerMenu(um, pm, currentUser);
                }
                else if (currentUser->getUserType() == "Merchant") {
                    showMerchantMenu(um, pm, currentUser);
                }
            }
            break;
        case 4:
            if (!currentUser) handleLogin(um, currentUser);
            else handleLogout(currentUser, um);
            break;
        case 0:
            std::cout << "正在退出平台，再见！" << std::endl;
            if (currentUser) um.persistChanges();
            pm.persistChanges();
            return;
        default:
            std::cout << "选择无效，请重试。" << std::endl;
        }
    }
}

void showConsumerMenu(UserManager& um, ProductManager& pm, User*& currentUser) {
    std::string choiceStr;
    int choice = -1;

    while (currentUser && currentUser->getUserType() == "Consumer") {
        std::cout << "\n--- 消费者菜单 (" << currentUser->getUsername() << ") ---" << std::endl;
        std::cout << "余额：$" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
        std::cout << "1. 查看所有商品" << std::endl;
        std::cout << "2. 搜索商品" << std::endl;
        // std::cout << "3. 购买商品" << std::endl; // 已移除
        std::cout << "3. 管理余额" << std::endl;     // 重新编号
        std::cout << "4. 修改密码" << std::endl;    // 重新编号
        std::cout << "0. 返回主菜单（退出登录）" << std::endl;

        choiceStr = getLineInput("请选择：");
        try {
            choice = std::stoi(choiceStr);
        }
        catch (const std::exception& e) {
            choice = -1;
        }

        switch (choice) {
        case 1: handleDisplayAllProducts(pm); break;
        case 2: handleSearchProducts(pm); break;
            // case 3: handlePurchaseProduct(um, pm, currentUser); break; // 已移除
        case 3: handleBalanceManagement(um, currentUser); break; // 现为case 3
        case 4: handleChangePassword(um, currentUser); break;    // 现为case 4
        case 0:
            handleLogout(currentUser, um);
            return;
        default:
            std::cout << "选择无效，请重试。" << std::endl;
        }
    }
    if (!currentUser) {
        return;
    }
}

void showMerchantMenu(UserManager& um, ProductManager& pm, User*& currentUser) {
    std::string choiceStr;
    int choice = -1;

    while (currentUser && currentUser->getUserType() == "Merchant") {
        std::cout << "\n--- 商家菜单 (" << currentUser->getUsername() << ") ---" << std::endl;
        std::cout << "余额：$" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
        std::cout << "1. 添加新商品" << std::endl;
        std::cout << "2. 管理我的商品" << std::endl;
        std::cout << "3. 按类别设置/更新折扣（针对我的商品）" << std::endl; // 已澄清名称
        std::cout << "4. 查看所有平台商品" << std::endl;
        std::cout << "5. 搜索平台商品" << std::endl;
        std::cout << "6. 管理余额" << std::endl;
        std::cout << "7. 修改密码" << std::endl;
        std::cout << "0. 返回主菜单（退出登录）" << std::endl;

        choiceStr = getLineInput("请选择：");
        try {
            choice = std::stoi(choiceStr);
        }
        catch (const std::exception& e) {
            choice = -1;
        }

        switch (choice) {
        case 1: handleAddProduct(pm, currentUser); break;
        case 2: handleManageMyProducts(pm, currentUser); break;
        case 3: handleDiscountCategory(pm, currentUser); break;
        case 4: handleDisplayAllProducts(pm); break;
        case 5: handleSearchProducts(pm); break;
        case 6: handleBalanceManagement(um, currentUser); break;
        case 7: handleChangePassword(um, currentUser); break;
        case 0:
            handleLogout(currentUser, um);
            return;
        default:
            std::cout << "选择无效，请重试。" << std::endl;
        }
    }
    if (!currentUser) {
        return;
    }
}


// --- 主函数 ---
int main() {
    UserManager userManager;
    ProductManager productManager;

    User* currentUser = nullptr;

    showMainMenu(userManager, productManager, currentUser);

    // 在从主菜单退出和登出时会调用PersistChanges。
    return 0;
}

// 确保split函数已定义。如果它在UserManager.cpp中且不在公共头文件中，
// 可能需要在此处重新定义或将其移至实用工具文件并包含它。
// 为简单起见，如果它仅在UserManager.cpp中，可能需要在此处重新声明或移动其定义。
// 假设它可用（例如，如果UserManager.cpp已编译并链接，其'split'函数在非静态时可用）。
// 为安全起见，可以在单独的util.h和util.cpp中定义它，或在main中使其成为静态助手。
// 鉴于项目结构，在ProductManager.cpp中使用'extern std::vector<std::string> split(...);'
// 并依赖UserManager.cpp在链接期间提供定义是常见做法。