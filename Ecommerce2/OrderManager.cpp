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

// ����split����ȫ�ֿ��û��ڹ�����util.h�ж���
//extern std::vector<std::string> split(const std::string& s, char delimiter);

// ���캯������ʼ�����������������ض����ļ���������һ������ID������
OrderManager::OrderManager(ProductManager& pm) : nextOrderIDCounter(1) {
    loadOrdersFromFile(pm); // ����ProductManager������Ԥ�����
    // �����Ѽ��صĶ���ȷ��nextOrderIDCounter
    if (!allOrders.empty()) {
        for (const auto* order : allOrders) {
            try {
                // ���趩��ID��ʽΪ"ORD<����>"
                if (order->getID().length() > 3 && order->getID().substr(0, 3) == "ORD") {
                    int idNum = std::stoi(order->getID().substr(3));
                    nextOrderIDCounter = std::max(nextOrderIDCounter, idNum + 1); // ȷ����һ��ID���������ID+1
                }
            }
            catch (const std::exception& e) {
                // �쳣���������Խ�������
            }
        }
    }
}

OrderManager::~OrderManager() {
    // persistChanges(); // �˳�ʱ������������ط���ʽ����
    for (Order* order : allOrders) { // �ͷ��ڴ�
        delete order;
    }
    allOrders.clear();
}

// �����¶���ID����ʽ��ORD+�������֣�
std::string OrderManager::generateNewOrderID() {
    return "ORD" + std::to_string(nextOrderIDCounter++); // ����������
}

// ���ļ����ض������ݣ��ָ����Ԥ��״̬��
void OrderManager::loadOrdersFromFile(ProductManager& pm) {
    std::ifstream inFile(ordersFilename);
    if (!inFile.is_open()) {
        // std::cerr << "���棺�޷��򿪶����ļ�: " << ordersFilename << ". ���¿�ʼ." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        std::vector<std::string> tokens = split(line, ','); // �����ŷָ�������

        // ������ʽ��ID,�������û���,�ܽ��,״̬,����ʱ��,����ʱ��,��Ʒ����,
        // ��Ʒ1: PID,��Ʒ��,�̼��û���,����,����, ��Ʒ2: ...
        if (tokens.size() < 7) { // �����ֶ�����7��
            std::cerr << "���棺�����и�ʽ���󣨻����ֶβ��㣩: " << line << ". ����." << std::endl;
            continue;
        }

        try {
            std::string orderID_str = tokens[0]; // ����ID
            std::string consumerUsername_str = tokens[1]; // �������û���
            double totalAmount_val = std::stod(tokens[2]); // �ܽ��
            OrderStatus status_val = stringToOrderStatus(tokens[3]); // ״̬ת��
            std::time_t creationTime_val = static_cast<std::time_t>(std::stoll(tokens[4])); // ����ʱ�䣨ʱ�����
            std::time_t lastUpdateTime_val = static_cast<std::time_t>(std::stoll(tokens[5])); // ����ʱ�䣨ʱ�����
            int numItems = std::stoi(tokens[6]); // ��Ʒ����

            // �����Ʒ�ֶ��Ƿ��㹻��ÿ����Ʒ5���ֶΣ�PID�����ơ��̼ҡ����ۡ�������
            if (tokens.size() < (size_t)(7 + numItems * 5)) {
                std::cerr << "���棺�����и�ʽ������Ʒ�ֶβ��㣬��" << numItems << "����Ʒ��: " << line << ". ����." << std::endl;
                continue;
            }

            std::vector<OrderItem> orderItems_vec; // ������Ʒ�б�
            int currentTokenIndex = 7; // �ӵ�8���ֶο�ʼ������Ʒ
            for (int i = 0; i < numItems; ++i) {
                std::string item_pid = tokens[currentTokenIndex++]; // ��ƷID
                std::string item_pname = tokens[currentTokenIndex++]; // ��Ʒ�������ܰ���ת��Ķ��ţ�
                std::replace(item_pname.begin(), item_pname.end(), ';', ','); // �ָ���ת��Ķ���
                std::string item_merchant = tokens[currentTokenIndex++]; // �̼��û���
                double item_price = std::stod(tokens[currentTokenIndex++]); // ����
                int item_qty = std::stoi(tokens[currentTokenIndex++]); // ����
                orderItems_vec.emplace_back(item_pid, item_pname, item_merchant, item_price, item_qty); // ����������Ʒ��
            }

            // ��������������ӵ��б�
            Order* loadedOrder = new Order(orderID_str, consumerUsername_str, orderItems_vec,
                totalAmount_val, status_val, creationTime_val, lastUpdateTime_val);
            allOrders.push_back(loadedOrder);

            // ����1��Ϊ��֧����������Ԥ�����
            if (loadedOrder->getStatus() == OrderStatus::PendingPayment) {
                bool allReserved = true; // ���������Ʒ�Ƿ�Ԥ���ɹ�
                for (const auto& item : loadedOrder->getItems()) {
                    Product* p = pm.findProductByID(item.productID); // ������Ʒ
                    if (p) {
                        if (!p->reserveStock(item.quantity)) { // ����Ԥ�����
                            std::cerr << "���ش��󣺼���ʱ�޷�Ϊ��֧������" << loadedOrder->getID()
                                << "����Ԥ��" << item.quantity << "����Ʒ" << item.productID
                                << ". ���ÿ��: " << p->getAvailableStock()
                                << ". ����״̬���ܲ�һ��." << std::endl;
                            allReserved = false; // ���ʧ��
                        }
                    }
                    else {
                        std::cerr << "���ش��󣺼���ʱδ�ҵ�����" << loadedOrder->getID()
                            << "�е���Ʒ" << item.productID << "." << std::endl;
                        allReserved = false; // ���ʧ��
                    }
                }
                if (!allReserved) {
                    // ����������ͷ��ѳɹ�Ԥ���Ŀ�沢��Ƕ���Ϊ�쳣/ȡ�����˴��򻯴���
                }
            }

        }
        catch (const std::invalid_argument& ia) { // ��ֵ��������
            std::cerr << "���������ļ��е���ֵʱ����: " << ia.what() << " ��: " << line << std::endl;
        }
        catch (const std::out_of_range& oor) { // ��ֵ��Χ����
            std::cerr << "�����ļ��е���ֵ������Χ. ��: " << line << std::endl;
        }
        catch (const std::exception& e) { // ͨ���쳣
            std::cerr << "���ض���ʱ��������: " << e.what() << " ��: " << line << std::endl;
        }
    }
    inFile.close();
}


// ���������ݳ־û����ļ�
void OrderManager::persistChanges() const {
    std::ofstream outFile(ordersFilename);
    if (!outFile.is_open()) {
        std::cerr << "�����޷��򿪶����ļ�����д��: " << ordersFilename << std::endl;
        return;
    }
    for (const Order* order : allOrders) {
        order->serialize(outFile); // ����Order�����л�����
        outFile << std::endl; // ÿ��һ������
    }
    outFile.close();
}

// �ӹ��ﳵ��������������ҵ���߼���
Order* OrderManager::createOrderFromCart(Consumer* consumer, ProductManager& pm) {
    if (!consumer || consumer->getShoppingCart()->isEmpty()) { // У�������ߺ͹��ﳵ��Ч��
        std::cout << "�����޷��������������ﳵΪ�ջ���������Ч��" << std::endl;
        return nullptr;
    }

    ShoppingCart* cart = consumer->getShoppingCart();
    std::vector<OrderItem> orderItemsList; // ������Ʒ�б�
    bool stockReservationSuccess = true; // ���Ԥ��״̬

    // ��һ�֣���鲢Ԥ��������Ʒ���
    for (const auto& pair : cart->getItems()) {
        const CartItem& cartItem = pair.second;
        Product* product = cartItem.product; // �ӹ��ﳵ���ȡ��Ʒָ��
        if (!product) { // �����Լ�飨���������ӦΪ�գ�
            std::cout << "���󣺹��ﳵ�д�����Ч��Ʒ����ָ�룩��" << std::endl;
            stockReservationSuccess = false; // ���ʧ��
            break;
        }

        if (!product->reserveStock(cartItem.quantity)) { // ����Ԥ�����
            std::cout << "��������ʧ�ܣ�" << product->getName() << "��治�㡣��Ҫ��"
                << cartItem.quantity << "�����ã�" << product->getAvailableStock() << std::endl;
            stockReservationSuccess = false; // ���ʧ��
            break;
        }
    }

    // ���п��Ԥ��ʧ�ܣ��ع�������Ԥ���Ŀ��
    if (!stockReservationSuccess) {
        for (const auto& pair : cart->getItems()) { // �������ﳵ������Ʒ
            const CartItem& cartItem = pair.second;
            Product* product = cartItem.product;
            if (product) {
                // �ͷű�Ӧ�ڱ��β�����Ԥ���Ŀ�棨���ܲ��ֳɹ�Ԥ����
                product->releaseReservedStock(cartItem.quantity); // �����ͷ�
            }
        }
        pm.persistChanges(); // ������Ʒ��������ͷţ�
        std::cout << "������������������������ͷ�֮ǰԤ���Ŀ�棨���У���" << std::endl;
        return nullptr;
    }

    // �����п��Ԥ���ɹ�������������Ʒ��
    for (const auto& pair : cart->getItems()) {
        const CartItem& cartItem = pair.second;
        // �ӹ��ﳵ���ȡ��Ʒ��Ϣ������������Ʒ��
        orderItemsList.emplace_back(cartItem.product->getID(), cartItem.product->getName(),
            cartItem.product->getOwnerMerchantUsername(),
            cartItem.product->getCurrentSalePrice(), cartItem.quantity);
    }


    std::string newID = generateNewOrderID(); // �����¶���ID
    Order* newOrder = new Order(newID, consumer->getUsername(), orderItemsList); // ������������
    allOrders.push_back(newOrder); // ��ӵ������б�

    pm.persistChanges(); // ������Ʒ�������Ԥ����
    persistChanges();    // �����¶������ļ�

    // ����3�����ﳵ���ڶ����ɹ���ɺ�ɾ�����û��������
    // ��ˣ��˴�����չ��ﳵ����֧���ɹ�����

    std::cout << "����" << newID << "�����ɹ����ܼƣ�$"
        << std::fixed << std::setprecision(2) << newOrder->getTotalAmount()
        << ". ��ǰ��֧����" << std::endl;
    return newOrder;
}

// ȡ���������ͷſ�沢����״̬��
bool OrderManager::cancelOrder(const std::string& orderID, ProductManager& pm) {
    Order* order = findOrderById(orderID); // ���Ҷ���
    if (!order) {
        std::cout << "����δ�ҵ�����ID" << orderID << "." << std::endl;
        return false;
    }

    // У�鶩��״̬�Ƿ�����ȡ������֧����֧��ʧ�ܣ�
    if (order->getStatus() != OrderStatus::PendingPayment && order->getStatus() != OrderStatus::FailedPayment) {
        std::cout << "�����޷�ȡ������" << orderID << "��״̬��"
            << orderStatusToString(order->getStatus()) << "����" << std::endl;
        return false;
    }

    // �ͷ�Ԥ�����
    bool allReleased = true;
    for (const auto& item : order->getItems()) {
        Product* p = pm.findProductByID(item.productID); // ������Ʒ
        if (p) {
            if (!p->releaseReservedStock(item.quantity)) { // �����ͷſ��
                std::cerr << "���棺�޷���ȫ�ͷŶ���" << orderID << "��" << item.quantity
                    << "����Ʒ" << item.productID << "�Ŀ�档" << std::endl;
                allReleased = false; // ���ʧ��
            }
        }
        else {
            std::cerr << "���棺ȡ������" << orderID << "ʱδ�ҵ���Ʒ" << item.productID << "���޷��ͷſ�档" << std::endl;
            allReleased = false; // ���ʧ��
        }
    }

    order->setStatus(OrderStatus::Cancelled); // ���¶���״̬Ϊ��ȡ��
    pm.persistChanges(); // ������Ʒ�����
    persistChanges();    // ���涩��״̬���

    std::cout << "����" << orderID << "��ȡ����" << std::endl;
    if (!allReleased) {
        std::cout << "���棺���ܴ��ڲ��ֿ���ͷ�ʧ�ܣ�������Ʒ��档" << std::endl;
    }
    return true;
}

// ����֧���߼������Ľ������̣�
bool OrderManager::processPayment(Order* order, Consumer* consumer, const std::string& enteredPassword,
    UserManager& um, ProductManager& pm) {
    if (!order || !consumer) return false; // ��ָ��У��

    // ����2.3����֤����������
    if (!consumer->checkPassword(enteredPassword)) {
        std::cout << "֧��ʧ�ܣ��������" << std::endl;
        order->setStatus(OrderStatus::FailedPayment); // ���֧��ʧ��״̬
        persistChanges(); // ���涩��״̬���
        return false;
    }

    // ����2.2������Ƿ���δ֧���ľɶ���
    std::vector<Order*> pendingOrders = getPendingOrdersByConsumer(consumer->getUsername());
    for (Order* pending : pendingOrders) {
        // ��ǰ����ID��ͬ�Ҵ���ʱ������δ֧�����������ȴ���
        if (pending->getID() != order->getID() && pending->getCreationTime() < order->getCreationTime()) {
            std::cout << "֧��ʧ�ܣ�����δ֧���ľɶ�����ID��" << pending->getID()
                << "�����������֧����ȡ����" << std::endl;
            order->setStatus(OrderStatus::FailedPayment); // ��ǵ�ǰ֧������ʧ��
            persistChanges();
            return false;
        }
    }


    // У�鶩��״̬�Ƿ�Ϊ��֧����֧��ʧ�ܣ���������֧����
    if (order->getStatus() != OrderStatus::PendingPayment && order->getStatus() != OrderStatus::FailedPayment) {
        std::cout << "֧��ʧ�ܣ�����" << order->getID() << "���Ǵ�֧��״̬��״̬��"
            << orderStatusToString(order->getStatus()) << "����" << std::endl;
        return false;
    }

    // У������������Ƿ��㹻
    if (consumer->getBalance() < order->getTotalAmount()) {
        std::cout << "֧��ʧ�ܣ����㡣��Ҫ��$"
            << order->getTotalAmount() << "�����ã�$" << consumer->getBalance() << std::endl;
        order->setStatus(OrderStatus::FailedPayment);
        persistChanges();
        return false;
    }

    // ���������˻��ۿ�
    if (!consumer->withdraw(order->getTotalAmount())) {
        std::cout << "֧��ʧ�ܣ����������˻��ۿ�ʱ��������" << std::endl;
        order->setStatus(OrderStatus::FailedPayment);
        persistChanges();
        return false;
    }

    // ���̼�ת�˲�ȷ�Ͽ������
    for (const auto& item : order->getItems()) {
        User* merchantUser = um.findUser(item.merchantUsername); // �����̼��û�
        if (merchantUser && merchantUser->getUserType() == "Merchant") {
            merchantUser->deposit(item.getSubtotal()); // �̼��տ�ӽ��=����*������
        }
        else {
            std::cout << "���棺δ�ҵ���Ʒ" << item.productName << "���̼�" << item.merchantUsername
                << "����̼��û����ʽ�δת�ơ�" << std::endl;
            // ע�⣺�˴����ܵ����������ѿۿ�̼�δ�տ�������Դ�����ʵ��δ����
        }

        Product* p = pm.findProductByID(item.productID); // ������Ʒ
        if (p) {
            if (!p->confirmSaleFromReserved(item.quantity)) { // ��Ԥ�����ȷ�����ۣ��ۼ�ʵ�ʿ�棩
                std::cerr << "���ش����޷�ȷ�϶���" << order->getID()
                    << "��" << item.quantity << "����Ʒ" << item.productID
                    << "�Ŀ�����ۣ������ܲ�һ�¡�" << std::endl;
            }
        }
        else {
            std::cerr << "���ش���ȷ�϶���" << order->getID()
                << "����ʱδ�ҵ���Ʒ" << item.productID << "." << std::endl;
        }
    }

    order->setStatus(OrderStatus::Paid); // ��Ƕ���Ϊ��֧��

    um.persistChanges(); // �����û������
    pm.persistChanges(); // ������Ʒ��������۳���Ԥ����棩
    persistChanges();    // ���涩��״̬���

    std::cout << "����" << order->getID() << "֧���ɹ���" << std::endl;
    std::cout << "��������$" << std::fixed << std::setprecision(2) << consumer->getBalance() << std::endl;

    // ����3��֧���ɹ���ӹ��ﳵ�Ƴ���Ӧ��Ʒ
    ShoppingCart* cart = consumer->getShoppingCart();
    if (cart) {
        for (const auto& orderItem : order->getItems()) {
            CartItem* cartItemPtr = cart->getItem(orderItem.productID); // ���ҹ��ﳵ�е���Ʒ��
            if (cartItemPtr) {
                if (cartItemPtr->quantity == orderItem.quantity) { // ������ȫƥ�䣬�Ƴ�������Ʒ��
                    cart->clearItem(orderItem.productID);
                }
                else if (cartItemPtr->quantity > orderItem.quantity) { // �����ϲ�Ӧ���֣����ﳵ����Ӧ���ڶ���������
                    cart->removeItem(orderItem.productID, orderItem.quantity); // �����������򻯴���
                }
                // �����ﳵ����С�ڶ�����������Ϊ���ݲ�һ�£�������
            }
        }
    }

    return true;
}


// ���ݶ���ID���Ҷ������ڲ����߷�����
Order* OrderManager::findOrderById(const std::string& orderID) {
    // ʹ��lambda���ʽ�ڶ����б��в���ƥ��ID�Ķ���
    auto it = std::find_if(allOrders.begin(), allOrders.end(),
        [&](const Order* o) { return o->getID() == orderID; });
    if (it != allOrders.end()) {
        return *it; // �����ҵ��Ķ���ָ��
    }
    return nullptr; // δ�ҵ����ؿ�ָ��
}

// �����������û�����ȡ�����б�֧��ɸѡ��֧��������
std::vector<Order*> OrderManager::getOrdersByConsumer(const std::string& consumerUsername, bool pendingOnly) {
    std::vector<Order*> results; // ����б�
    for (Order* order : allOrders) {
        if (order->getConsumerUsername() == consumerUsername) { // �������ڸ������ߵĶ���
            if (pendingOnly) { // ��ɸѡ��֧����֧��ʧ�ܵĶ���
                if (order->getStatus() == OrderStatus::PendingPayment || order->getStatus() == OrderStatus::FailedPayment) {
                    results.push_back(order);
                }
            }
            else { // ��������״̬����
                results.push_back(order);
            }
        }
    }
    // ������ʱ������Ĭ�ϣ��������ǰ��
    std::sort(results.begin(), results.end(), [](const Order* a, const Order* b) {
        return a->getCreationTime() < b->getCreationTime(); // �������У� oldest first ��
        });
    return results;
}


// ��ȡ�����ߵĴ�֧����������ݷ�����
std::vector<Order*> OrderManager::getPendingOrdersByConsumer(const std::string& consumerUsername) {
    return getOrdersByConsumer(consumerUsername, true); // ����ͨ�÷�����ɸѡ��֧��״̬
}