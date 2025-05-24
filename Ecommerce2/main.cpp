#include <iostream>
#include <string>
#include <vector>
#include <limits> 
#include <algorithm> 
#include <iomanip> 
#include <set>
#include <chrono> // ���ڶ�����ʱ - �����ڿ���̨����δ��ȫʵ��

#include "UserManager.h"
#include "ProductManager.h"
#include "ShoppingCart.h" // ͨ��Users.h��Consumer������
#include "OrderManager.h"   // �¹�����
// User.h��Product.h��Order.h ͨ�����ԵĹ�������Consumer����

// �˵�����ǰ������
void showMainMenu(UserManager& um, ProductManager& pm, OrderManager& om, User*& currentUser);
void showConsumerMenu(UserManager& um, ProductManager& pm, OrderManager& om, User*& currentUser);
void showMerchantMenu(UserManager& um, ProductManager& pm, OrderManager& om, User*& currentUser); // ���OrderManager

// ��׳���븨���������ޱ仯��
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
            std::cout << "������Ч�������ԡ�" << std::endl;
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
        std::cout << "���벻��Ϊ�ա������ԡ�" << std::endl;
    }
}

// �û���ز������������仯��ȷ������persistChanges��
void handleRegister(UserManager& um) {
    std::cout << "\n--- �û�ע�� ---" << std::endl;
    std::string uname = getLineInput("�������û���: ");
    if (um.isUsernameTaken(uname)) {
        std::cout << "�û����Ѵ��ڡ��볢�������û�����" << std::endl;
        return;
    }
    std::string pwd1, pwd2;
    do {
        pwd1 = getLineInput("���������루����Ϊ�գ�: ");
        pwd2 = getLineInput("ȷ������: ");
        if (pwd1 != pwd2) {
            std::cout << "���벻ƥ�䡣�����ԡ�" << std::endl;
        }
    } while (pwd1 != pwd2);
    std::string typeChoice;
    std::cout << "ע��Ϊ (1) ������ �� (2) �̼�: ";
    std::getline(std::cin, typeChoice);
    std::string accountType;
    if (typeChoice == "1") accountType = "Consumer";
    else if (typeChoice == "2") accountType = "Merchant";
    else {
        std::cout << "��Чѡ��ע��ʧ�ܡ�" << std::endl;
        return;
    }
    if (um.registerUser(uname, pwd1, accountType)) {
        std::cout << "ע��ɹ������Ϊ " << accountType << "!" << std::endl;
    }
    else {
        std::cout << "ע��ʧ�ܡ��û��������Ѵ��ڻ�������Ч��" << std::endl;
    }
}

void handleLogin(UserManager& um, User*& currentUser) {
    std::cout << "\n--- �û���¼ ---" << std::endl;
    if (currentUser) {
        std::cout << "�ѵ�¼�û�: " << currentUser->getUsername() << std::endl;
        return;
    }
    std::string uname = getLineInput("�������û���: ");
    std::string pwd = getLineInput("����������: ");
    currentUser = um.loginUser(uname, pwd);
    if (currentUser) {
        std::cout << "��¼�ɹ�����ӭ�㣬" << currentUser->getUsername() << "!" << std::endl;
    }
    else {
        std::cout << "��¼ʧ�ܡ��û������������" << std::endl;
    }
}

void handleLogout(User*& currentUser, UserManager& um, ProductManager& pm, OrderManager& om) { // ���PM��OM
    if (currentUser) {
        std::cout << "�����˳� " << currentUser->getUsername() << "��" << std::endl;
        um.persistChanges();
        pm.persistChanges(); // �־û����ܷ����Ĳ�Ʒ�����
        om.persistChanges(); // �־û��κζ���״̬���
        currentUser = nullptr;
    }
    else {
        std::cout << "δ��¼��" << std::endl;
    }
}

void handleChangePassword(UserManager& um, User* currentUser) {
    if (!currentUser) {
        std::cout << "������ȵ�¼�����޸����롣" << std::endl;
        return;
    }
    std::cout << "\n--- �޸����� ---" << std::endl;
    std::string oldPwd = getLineInput("�����뵱ǰ����: ");
    if (!currentUser->checkPassword(oldPwd)) {
        std::cout << "��ǰ�������" << std::endl;
        return;
    }
    std::string pwd1, pwd2;
    do {
        pwd1 = getLineInput("�����������루����Ϊ�գ�: ");
        pwd2 = getLineInput("ȷ��������: ");
        if (pwd1 != pwd2) {
            std::cout << "�����벻ƥ�䡣�����ԡ�" << std::endl;
        }
    } while (pwd1 != pwd2);
    currentUser->setPassword(pwd1);
    um.persistChanges();
    std::cout << "�����޸ĳɹ���" << std::endl;
}

void handleBalanceManagement(UserManager& um, User* currentUser) {
    if (!currentUser) {
        std::cout << "������ȵ�¼���ܹ�����" << std::endl;
        return;
    }
    std::cout << "\n--- ������ ---" << std::endl;
    std::cout << "��ǰ���: $" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
    std::cout << "1. ��ֵ���" << std::endl;
    std::cout << "2. �鿴������ʾ��" << std::endl;
    std::cout << "0. ����" << std::endl;
    std::string choiceStr = getLineInput("���ѡ��: ");
    int choice = -1;
    try { choice = std::stoi(choiceStr); }
    catch (const std::exception&) { std::cout << "������Ч��" << std::endl; return; }
    if (choice == 1) {
        double amount = getValidatedInput<double>("�������ֵ���: $");
        if (amount <= 0) {
            std::cout << "��ֵ������Ϊ������" << std::endl; return;
        }
        currentUser->deposit(amount);
        um.persistChanges();
        std::cout << "��ֵ�ɹ��������: $" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
    }
    else if (choice == 0) { return; }
    else if (choice != 2) { std::cout << "��Чѡ��" << std::endl; }
}

// ��Ʒ��ز�����ͨ�� - ���ش�仯��
void displayProducts(const std::vector<Product*>& productList, bool showStockDetail = true) { // ���showStockDetail
    if (productList.empty()) {
        std::cout << "�޲�Ʒ����ʾ��" << std::endl;
        return;
    }
    std::cout << "\n--- ���ò�Ʒ ---" << std::endl;
    for (const auto* product : productList) {
        // product->displayDetails(); // ����ʾ��ϸ���
        // ���ڹ��ﳵ��ͼ��������Ҫ����ϸ�ڣ���displayDetails�㹻
        if (showStockDetail) {
            product->displayDetails();
        }
        else { // ���ڿ�������ĸ�������ͼ
            std::cout << "-------------------------------------\n"
                << "ID: " << product->getID() << ", ����: " << product->getName() << "\n"
                << "�۸�: $" << std::fixed << std::setprecision(2) << product->getCurrentSalePrice()
                << ", ���ÿ��: " << product->getAvailableStock() << "\n"
                << "���۷�: " << product->getOwnerMerchantUsername() << "\n"
                << "-------------------------------------" << std::endl;
        }
    }
}

void handleDisplayAllProducts(ProductManager& pm, bool compact = false) {
    displayProducts(pm.getAllProducts(), !compact);
}

void handleSearchProducts(ProductManager& pm) {
    std::cout << "\n--- ������Ʒ ---" << std::endl;
    std::cout << "������ʽ (1) ����, (2) ����, (3) �̼�: ";
    std::string choiceStr = getLineInput("");
    int choice = -1;
    try { choice = std::stoi(choiceStr); }
    catch (const std::exception&) { std::cout << "������Ч��" << std::endl; return; }
    std::string searchTerm = getLineInput("������������: ");
    std::string searchBy;
    if (choice == 1) searchBy = "name";
    else if (choice == 2) searchBy = "type";
    else if (choice == 3) searchBy = "merchant";
    else { std::cout << "��Ч����������" << std::endl; return; }
    std::vector<Product*> results = pm.searchProducts(searchTerm, searchBy);
    if (results.empty()) {
        std::cout << "δ�ҵ����������Ĳ�Ʒ��" << std::endl;
    }
    else {
        std::cout << "--- ������� ---" << std::endl;
        displayProducts(results);
    }
}

// --- ������ר�ù��ﳵ���� ---
void handleManageShoppingCart(Consumer* consumer, ProductManager& pm) {
    if (!consumer) { std::cout << "����δ����������ݵ�¼��" << std::endl; return; }
    ShoppingCart* cart = consumer->getShoppingCart();

    while (true) {
        cart->displayCart();
        std::cout << "\n--- ���ﳵ�˵� ---" << std::endl;
        std::cout << "1. ��Ӳ�Ʒ�����ﳵ" << std::endl;
        std::cout << "2. ���¹��ﳵ�в�Ʒ����" << std::endl;
        std::cout << "3. �ӹ��ﳵ�Ƴ���Ʒ����������" << std::endl;
        std::cout << "4. ��չ��ﳵ" << std::endl;
        std::cout << "0. ���������߲˵�" << std::endl;
        std::string choiceStr = getLineInput("���ѡ��: ");
        int choice = -1;
        try { choice = std::stoi(choiceStr); }
        catch (const std::exception&) { choice = -99; }

        switch (choice) {
        case 1: {
            handleDisplayAllProducts(pm, true); // ��ʾ������Ĳ�Ʒ
            std::string pid = getLineInput("������Ҫ��ӵĲ�ƷID: ");
            Product* p = pm.findProductByID(pid);
            if (!p) { std::cout << "��Ʒδ�ҵ���" << std::endl; break; }
            if (p->getAvailableStock() == 0) { std::cout << "��Ʒ'" << p->getName() << "'��������" << std::endl; break; }
            int qty = getValidatedInput<int>("����������: ");
            cart->addItem(p, qty);
            break;
        }
        case 2: {
            if (cart->isEmpty()) { std::cout << "���ﳵΪ�ա�" << std::endl; break; }
            std::string pid = getLineInput("�����빺�ﳵ��Ҫ���µĲ�ƷID: ");
            int newQty = getValidatedInput<int>("��������������0��ʾ�Ƴ���: ");
            cart->updateItemQuantity(pid, newQty);
            break;
        }
        case 3: {
            if (cart->isEmpty()) { std::cout << "���ﳵΪ�ա�" << std::endl; break; }
            std::string pid = getLineInput("�����빺�ﳵ��Ҫ�Ƴ�/���ٵĲ�ƷID: ");
            int qtyRemove = getValidatedInput<int>("�������Ƴ����������뵱ǰ������ȫ���Ƴ���: ");
            cart->removeItem(pid, qtyRemove);
            break;
        }
        case 4:
            cart->clearCart();
            break;
        case 0:
            return;
        default:
            std::cout << "��Чѡ��" << std::endl;
        }
        // ���ﳵ����־û�����Ϊ�Ự�ڴ��д洢
    }
}

// --- ������ר�ö������� ---
void handleCreateOrder(Consumer* consumer, ProductManager& pm, OrderManager& om) {
    if (!consumer) { std::cout << "����δ��¼��" << std::endl; return; }
    if (consumer->getShoppingCart()->isEmpty()) {
        std::cout << "��Ĺ��ﳵΪ�ա��������Ʒ�󴴽�������" << std::endl;
        return;
    }
    consumer->getShoppingCart()->displayCart();
    std::string confirm = getLineInput("ȷ��ʹ����Щ��Ʒ����������(y/n): ");
    if (confirm == "y" || confirm == "Y") {
        Order* newOrder = om.createOrderFromCart(consumer, pm);
        if (newOrder) {
            // newOrder->displayOrderDetails(); // �����ɹ�ʱ����createOrderFromCart��ʾ
            // ���ﳵ�˴�����գ�����֧���ɹ������������3��
        }
        else {
            std::cout << "��������ʧ�ܡ������Ʒ�����ﳵ��" << std::endl;
        }
    }
    else {
        std::cout << "����������ȡ����" << std::endl;
    }
}

void handleViewMyOrders(Consumer* consumer, OrderManager& om) {
    if (!consumer) { std::cout << "����δ��¼��" << std::endl; return; }
    std::vector<Order*> orders = om.getOrdersByConsumer(consumer->getUsername());
    if (orders.empty()) {
        std::cout << "��û���κζ�����" << std::endl;
        return;
    }
    std::cout << "\n--- ��Ķ��� --- (��ʱ��˳��������ǰ)" << std::endl;
    for (const auto* order : orders) {
        order->displayOrderDetails();
    }
}

void handlePayForOrder(Consumer* consumer, OrderManager& om, UserManager& um, ProductManager& pm) {
    if (!consumer) { std::cout << "����δ��¼��" << std::endl; return; }

    std::vector<Order*> pendingOrders = om.getPendingOrdersByConsumer(consumer->getUsername());
    if (pendingOrders.empty()) {
        std::cout << "��û�д�֧���Ķ�����" << std::endl;
        return;
    }

    std::cout << "\n--- ���֧���Ķ��� --- (��ʱ��˳��������ǰ)" << std::endl;
    for (size_t i = 0; i < pendingOrders.size(); ++i) {
        std::cout << (i + 1) << ". ";
        pendingOrders[i]->displayOrderDetails(); // ��ʾ��ҪժҪ��������Ϣ
    }

    std::string orderIdToPay = getLineInput("������Ҫ֧���Ķ���ID����0ȡ����: ");
    if (orderIdToPay == "0") return;

    Order* order = om.findOrderById(orderIdToPay);
    if (!order || order->getConsumerUsername() != consumer->getUsername()) {
        std::cout << "����δ�ҵ��������㡣" << std::endl;
        return;
    }
    if (order->getStatus() != OrderStatus::PendingPayment && order->getStatus() != OrderStatus::FailedPayment) {
        std::cout << "�ö�����ǰ����֧����״̬: " << orderStatusToString(order->getStatus()) << "����" << std::endl;
        return;
    }

    // ֧������ǰ�������¼���Ƿ��и����δ֧������
    // ��processPayment�ڲ��Ѵ������˴�����ǰ��飩
    std::vector<Order*> allUserPending = om.getPendingOrdersByConsumer(consumer->getUsername());
    for (Order* po : allUserPending) {
        if (po->getID() != order->getID() && po->getCreationTime() < order->getCreationTime()) {
            std::cout << "�������֧����ȡ������Ķ��� " << po->getID() << "��" << std::endl;
            return;
        }
    }

    std::cout << "��֧������: " << std::endl;
    order->displayOrderDetails();
    std::cout << "��ĵ�ǰ���: $" << std::fixed << std::setprecision(2) << consumer->getBalance() << std::endl;
    if (consumer->getBalance() < order->getTotalAmount()) {
        std::cout << "���㣬�޷�֧���ö�����" << std::endl;
        return;
    }

    std::string pwd = getLineInput("����������ȷ��֧��: ");
    if (om.processPayment(order, consumer, pwd, um, pm)) {
        // ֧���ɹ���Ϣ��processPayment����
        // ��֧����Ʒ�Ĺ��ﳵ������processPayment����
    }
    else {
        // ֧��ʧ����Ϣ��processPayment����
        std::cout << "���鶩��״̬�����ԡ�" << std::endl;
    }
}

void handleCancelOrder(Consumer* consumer, OrderManager& om, ProductManager& pm) {
    if (!consumer) { std::cout << "����: δ��¼��" << std::endl; return; }

    std::vector<Order*> pendingOrders = om.getPendingOrdersByConsumer(consumer->getUsername());
    if (pendingOrders.empty()) {
        std::cout << "��û�д�֧���Ķ�����ȡ����" << std::endl;
        return;
    }

    std::cout << "\n--- ���ȡ���Ķ��� --- (�������ǰ)" << std::endl;
    for (size_t i = 0; i < pendingOrders.size(); ++i) {
        std::cout << (i + 1) << ". ";
        // ֻ��ʾժҪ�Ա�������ı�
        std::cout << "����ID: " << pendingOrders[i]->getID()
            << ", ״̬: " << orderStatusToString(pendingOrders[i]->getStatus())
            << ", �ܼ�: $" << std::fixed << std::setprecision(2) << pendingOrders[i]->getTotalAmount() << std::endl;
    }

    std::string orderIdToCancel = getLineInput("����Ҫȡ���Ķ���ID (��0����): ");
    if (orderIdToCancel == "0") return;

    if (om.cancelOrder(orderIdToCancel, pm)) {
        // �ɹ���Ϣ��cancelOrder����
    }
    else {
        // ʧ����Ϣ��cancelOrder����
    }
}

// --- �̼��ض����� (��������1, ȷ����ȷ����ProductManager��User) ---
void handleAddProduct(ProductManager& pm, User* merchantUser) {
    if (!merchantUser || merchantUser->getUserType() != "Merchant") {
        std::cout << "����: �������̼���ݵ�¼��" << std::endl; return;
    }
    Merchant* merchant = static_cast<Merchant*>(merchantUser);
    std::cout << "\n--- ����²�Ʒ ---" << std::endl;
    std::string name = getLineInput("�����Ʒ����: ");
    std::string description = getLineInput("�����Ʒ����: ");
    double originalPrice = -1.0;
    while (originalPrice < 0) {
        originalPrice = getValidatedInput<double>("�����Ʒԭ��: $");
        if (originalPrice < 0) std::cout << "�۸���Ϊ����" << std::endl;
    }
    int stock = -1; // ���ǳ�ʼ�ܿ��
    while (stock < 0) {
        stock = getValidatedInput<int>("�����Ʒ��ʼ�ܿ������: ");
        if (stock < 0) std::cout << "��治��Ϊ����" << std::endl;
    }
    std::string productTypeChoice, productType;
    while (true) {
        std::cout << "ѡ���Ʒ���:\n1. �鼮\n2. ʳƷ\n3. ��װ\n���ѡ��: ";
        std::getline(std::cin, productTypeChoice);
        if (productTypeChoice == "1") { productType = "Book"; break; }
        if (productTypeChoice == "2") { productType = "Food"; break; }
        if (productTypeChoice == "3") { productType = "Clothing"; break; }
        std::cout << "��Чѡ��" << std::endl;
    }
    if (pm.addProduct(name, description, originalPrice, stock, merchant->getUsername(), productType)) {
        std::cout << "��Ʒ '" << name << "' ��ӳɹ���" << std::endl;
    }
    else {
        std::cout << "��Ӳ�Ʒʧ�ܡ�" << std::endl;
    }
}

void handleManageMyProducts(ProductManager& pm, User* merchantUser) {
    if (!merchantUser || merchantUser->getUserType() != "Merchant") {
        std::cout << "����: �������̼���ݵ�¼��" << std::endl; return;
    }
    Merchant* merchant = static_cast<Merchant*>(merchantUser);
    std::cout << "\n--- �����ҵĲ�Ʒ ---" << std::endl;
    std::vector<Product*> myProducts = pm.getProductsByMerchant(merchant->getUsername());
    if (myProducts.empty()) {
        std::cout << "�㻹û������κβ�Ʒ��" << std::endl; return;
    }
    displayProducts(myProducts);
    std::string productID = getLineInput("����Ҫ����Ĳ�ƷID (��0ȡ��): ");
    if (productID == "0") return;
    Product* product = pm.findProductByID(productID);
    if (!product || product->getOwnerMerchantUsername() != merchant->getUsername()) {
        std::cout << "��ƷIDδ�ҵ����㲻ӵ�д˲�Ʒ��" << std::endl; return;
    }
    std::cout << "\n�����Ʒ: " << product->getName() << " (ID: " << product->getID() << ")" << std::endl;
    std::cout << "1. ���¼۸� (ԭ�ۺ��ۼ�)" << std::endl;
    std::cout << "2. �����ܿ��" << std::endl; // �Ӹ��¿���Ϊ�����ܿ��
    std::cout << "3. �����ۿ� (Ϊ���ض���Ʒ�����ۼ�)" << std::endl;
    std::cout << "4. �Ƴ���Ʒ" << std::endl;
    std::cout << "5. ��������" << std::endl;
    std::cout << "6. ��������" << std::endl;
    std::cout << "0. ����" << std::endl;
    std::string choiceStr = getLineInput("���ѡ��: ");
    int choice = -1;
    try { choice = std::stoi(choiceStr); }
    catch (const std::exception&) { std::cout << "������Ч��" << std::endl; return; }
    bool changed = false;
    switch (choice) {
    case 1: { /* �۸�����߼� */
        double newOrigPrice = -1.0;
        while (newOrigPrice < 0) { newOrigPrice = getValidatedInput<double>("��ԭ��: $"); if (newOrigPrice < 0) std::cout << "��Ч\n"; }
        product->setOriginalPrice(newOrigPrice);
        double newSalePrice = -1.0;
        while (newSalePrice < 0 || newSalePrice > newOrigPrice) { newSalePrice = getValidatedInput<double>("���ۼ�: $"); if (newSalePrice<0 || newSalePrice > newOrigPrice) std::cout << "��Ч\n"; }
        product->setCurrentSalePrice(newSalePrice);
        changed = true; break;
    }
    case 2: { // �����ܿ��
        int newTotalStock = -1;
        while (newTotalStock < product->getReservedStock()) { // ȷ�����ܿ�治С����Ԥ�����
            newTotalStock = getValidatedInput<int>("�����µ��ܿ������: ");
            if (newTotalStock < product->getReservedStock()) {
                std::cout << "�ܿ�治��С�ڵ�ǰ��Ԥ����� ("
                    << product->getReservedStock() << ")����������ߵ�ֵ��" << std::endl;
            }
            else if (newTotalStock < 0) {
                std::cout << "��治��Ϊ����\n";
            }
        }
        product->setTotalStock(newTotalStock); // ʹ��setTotalStock
        changed = true; break;
    }
    case 3: { /* �ض���Ʒ���ۿ� */
        double disc = -1.0;
        while (disc < 0 || disc > 100) { disc = getValidatedInput<double>("�ۿ۰ٷֱ� (0-100): "); if (disc < 0 || disc>100) std::cout << "��Ч\n"; }
        product->setCurrentSalePrice(product->getOriginalPrice() * (1.0 - (disc / 100.0)));
        changed = true; break;
    }
    case 4: { /* �Ƴ���Ʒ */
        std::string conf = getLineInput("ȷ��? (y/n):");
        if (conf == "y" || conf == "Y") { if (pm.removeProduct(product->getID(), merchant->getUsername())) { std::cout << "���Ƴ���\n"; return; } else { std::cout << "ʧ�ܡ�\n"; } }
        break; // ����Ҫchanged = true����ΪremoveProduct�ᱣ��
    }
    case 5: { product->setDescription(getLineInput("������: ")); changed = true; break; }
    case 6: { product->setName(getLineInput("������: ")); changed = true; break; }
    case 0: return;
    default: std::cout << "��Чѡ��" << std::endl; break;
    }
    if (changed) {
        pm.updateProduct(product);
        std::cout << "��Ʒ�����Ѹ��¡�" << std::endl;
    }
}

void handleDiscountCategory(ProductManager& pm, User* merchantUser) {
    // �˺���������1���»�����ͬ��ʹ��pm.applyCategoryDiscount
    if (!merchantUser || merchantUser->getUserType() != "Merchant") {
        std::cout << "����: �������̼���ݵ�¼��" << std::endl; return;
    }
    Merchant* merchant = static_cast<Merchant*>(merchantUser);
    std::cout << "\n--- ��������ò�Ʒ�ۿ� (�����Ĳ�Ʒ) ---" << std::endl;
    // ... (�������ಿ��������1����ǰ�汾��ͬ) ...
    // ��ʾ�̼ҵ���������ƽ̨���
    std::vector<Product*> myProducts = pm.getProductsByMerchant(merchant->getUsername());
    std::set<std::string> myTypesSet;
    for (const auto* p : myProducts) myTypesSet.insert(p->getProductType());
    std::cout << "��Ĳ�Ʒ���: ";
    if (myTypesSet.empty()) std::cout << "�ޡ� "; else for (const auto& type : myTypesSet) std::cout << type << " ";
    std::cout << "\n����ƽ̨���: ";
    std::vector<std::string> allTypes = pm.getAvailableProductTypes();
    if (allTypes.empty()) std::cout << "�ޡ�"; else for (const auto& type : allTypes) std::cout << type << " ";
    std::cout << std::endl;

    std::string categoryToDiscount = getLineInput("����ҪӦ��/�����ۿ۵Ĳ�Ʒ���: ");
    double discountPercent = -1.0;
    while (discountPercent < 0 || discountPercent > 100) {
        discountPercent = getValidatedInput<double>("�����ۿ۰ٷֱ� (0-100, 0��ʾ�Ƴ��ۿ�): ");
        if (discountPercent < 0 || discountPercent > 100) std::cout << "�ۿ۱�����0��100֮�䡣\n";
    }
    pm.applyCategoryDiscount(merchant->getUsername(), categoryToDiscount, discountPercent);
}

// --- �˵����� ---
void showMainMenu(UserManager& um, ProductManager& pm, OrderManager& om, User*& currentUser) { // ���OM
    std::string choiceStr; int choice = -1;
    while (true) {
        std::cout << "\n========= ��������ƽ̨ =========" << std::endl;
        if (currentUser) {
            std::cout << "��ǰ��¼: " << currentUser->getUsername()
                << " (" << currentUser->getUserType()
                << ") | ���: $" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
        }
        else { std::cout << "��ӭ, �ÿ�!" << std::endl; }
        std::cout << "---------------------------------------" << std::endl;
        std::cout << "1. ��ʾ���в�Ʒ" << std::endl;
        std::cout << "2. ������Ʒ" << std::endl;
        if (!currentUser) {
            std::cout << "3. ע��" << std::endl;
            std::cout << "4. ��¼" << std::endl;
        }
        else {
            std::cout << "3. �ҵ��˻�ѡ��" << std::endl;
            std::cout << "4. �ǳ�" << std::endl;
        }
        std::cout << "0. �˳�" << std::endl;
        std::cout << "---------------------------------------" << std::endl;
        choiceStr = getLineInput("���ѡ��: ");
        try { choice = std::stoi(choiceStr); }
        catch (const std::exception&) { choice = -1; }

        switch (choice) {
        case 1: handleDisplayAllProducts(pm); break;
        case 2: handleSearchProducts(pm); break;
        case 3:
            if (!currentUser) handleRegister(um);
            else {
                if (currentUser->getUserType() == "Consumer") {
                    showConsumerMenu(um, pm, om, currentUser); // ����OM
                }
                else if (currentUser->getUserType() == "Merchant") {
                    showMerchantMenu(um, pm, om, currentUser); // ����OM
                }
            }
            break;
        case 4:
            if (!currentUser) handleLogin(um, currentUser);
            else handleLogout(currentUser, um, pm, om); // ����PM, OM
            break;
        case 0:
            std::cout << "�����˳�ƽ̨���ټ�!" << std::endl;
            if (currentUser) um.persistChanges();
            pm.persistChanges();
            om.persistChanges(); // �־û�����
            return;
        default:
            std::cout << "��Чѡ�������ԡ�" << std::endl;
        }
    }
}

void showConsumerMenu(UserManager& um, ProductManager& pm, OrderManager& om, User*& currentUser) { // ���OM
    if (!currentUser || currentUser->getUserType() != "Consumer") return;
    Consumer* consumer = static_cast<Consumer*>(currentUser);
    std::string choiceStr; int choice = -1;

    while (currentUser && currentUser->getUserType() == "Consumer") { // ��ѭ���м��currentUser�Դ���ǳ�
        std::cout << "\n--- �����߲˵� (" << consumer->getUsername() << ") ---" << std::endl;
        std::cout << "���: $" << std::fixed << std::setprecision(2) << consumer->getBalance() << std::endl;
        std::cout << "1. �鿴���в�Ʒ" << std::endl;
        std::cout << "2. ������Ʒ" << std::endl;
        std::cout << "3. �����ﳵ" << std::endl;
        std::cout << "4. �ӹ��ﳵ��������" << std::endl;
        std::cout << "5. �鿴�ҵĶ���" << std::endl;
        std::cout << "6. ֧������" << std::endl;
        std::cout << "7. ȡ����������" << std::endl;
        std::cout << "8. �������" << std::endl;
        std::cout << "9. ��������" << std::endl;
        std::cout << "0. �������˵� (�ǳ�)" << std::endl;
        choiceStr = getLineInput("���ѡ��: ");
        try { choice = std::stoi(choiceStr); }
        catch (const std::exception&) { choice = -1; }

        // ���currentUser��Ϊnull(�����ڴ�������еǳ�)��������
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
            handleLogout(currentUser, um, pm, om); // �ǳ�������currentUserΪnull
            return; // �˳������߲˵�
        default:
            std::cout << "��Чѡ�������ԡ�" << std::endl;
        }
    }
}

void showMerchantMenu(UserManager& um, ProductManager& pm, OrderManager& om, User*& currentUser) { // ���OM
    std::string choiceStr; int choice = -1;
    while (currentUser && currentUser->getUserType() == "Merchant") { // ���currentUser
        std::cout << "\n--- �̼Ҳ˵� (" << currentUser->getUsername() << ") ---" << std::endl;
        std::cout << "���: $" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
        std::cout << "1. ����²�Ʒ" << std::endl;
        std::cout << "2. �����ҵĲ�Ʒ" << std::endl;
        std::cout << "3. ���������/�����ۿ�" << std::endl;
        std::cout << "4. �鿴����ƽ̨��Ʒ" << std::endl;
        std::cout << "5. ����ƽ̨��Ʒ" << std::endl;
        std::cout << "6. �������" << std::endl;
        std::cout << "7. ��������" << std::endl;
        std::cout << "0. �������˵� (�ǳ�)" << std::endl;
        choiceStr = getLineInput("���ѡ��: ");
        try { choice = std::stoi(choiceStr); }
        catch (const std::exception&) { choice = -1; }

        if (!currentUser) break; // ������ٴμ��

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
            return; // �˳��̼Ҳ˵�
        default:
            std::cout << "��Чѡ�������ԡ�" << std::endl;
        }
    }
}

// --- ������ ---
int main() {
    UserManager userManager;
    ProductManager productManager;
    OrderManager orderManager(productManager); // ��ʼ��OrderManager������ProductManager���ڿ�����

    User* currentUser = nullptr;

    showMainMenu(userManager, productManager, orderManager, currentUser); // ����OrderManager

    // PersistChanges�ڴ����˵��˳��͵ǳ�ʱ���á�
    return 0;
}

// ȷ��'split'������ĳ���ɷ��ʵ�λ�ö��塣
// �������UserManager.cpp�У�����OrderManager.cppҲͨ��'extern'ʹ������
// ȷ��UserManager.cpp�����벢���ӡ�
// ͨ�ù����ļ����ʺ�'split'������