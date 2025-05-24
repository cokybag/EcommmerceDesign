#pragma once
#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H

#include <string> // ����std::string
#include <vector> // ����std::vector
// �˴���������<map>

#include "Order.h" // ����Order��

// ǰ�����������õı��ʵ����
class Consumer;
class UserManager;
class ProductManager;
// class ShoppingCart; // δ�ڷ���ǩ����ֱ��ʹ��

class OrderManager {
private:
    std::vector<Order*> allOrders;          // �洢���ж�����ָ���б�
    const std::string ordersFilename = "orders.txt"; // �������ݴ洢�ļ�����Ĭ��"orders.txt"��
    int nextOrderIDCounter;                 // ��������Ψһ����ID�ļ�����

    std::string generateNewOrderID();       // ����Ψһ����ID��˽�з�����
    // ͨ�����ô���ProductManager�Լ��ض������������Ʒ��Ϣ��
    void loadOrdersFromFile(ProductManager& pm);

public:
    OrderManager(ProductManager& pm);       // ���캯������ʼ��ʱ���ض����ļ���
    ~OrderManager();                        // �����������ͷ��ڴ棩

    // �ӹ��ﳵ���������������ߡ���Ʒ��������Ϊ������
    Order* createOrderFromCart(Consumer* consumer, ProductManager& pm);

    // ȡ���������趩��ID��������Ʒ���������¿�棩
    bool cancelOrder(const std::string& orderID, ProductManager& pm);

    // ����֧���������������ߡ��������롢�û�����������Ʒ��������
    bool processPayment(Order* order, Consumer* consumer, const std::string& enteredPassword,
        UserManager& um, ProductManager& pm);

    // ���ݶ���ID���Ҷ���������ָ�룬δ�ҵ�����nullptr��
    Order* findOrderById(const std::string& orderID);

    // ��ȡ�����ߵĶ����б���ѡ�Ƿ����ȡ���������
    std::vector<Order*> getOrdersByConsumer(const std::string& consumerUsername, bool pendingOnly = false);

    // ��ȡ�����ߵĴ�������б���ݷ�����
    std::vector<Order*> getPendingOrdersByConsumer(const std::string& consumerUsername);

    // �־û��������ݵ��ļ���������Ա������
    void persistChanges() const;
};

#endif // ORDERMANAGER_H