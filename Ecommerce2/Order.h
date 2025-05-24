#pragma once
#ifndef ORDER_H
#define ORDER_H

#include "OrderItem.h"
#include <vector>
#include <string>
#include <ctime>    // ����std::time_tʱ������
#include <fstream>  // �������л�����
#include <sstream>  // �������л��ַ�����

// ����״̬ö��
enum class OrderStatus {
    PendingPayment,  // ������
    Paid,            // �Ѹ���
    Cancelled,       // ��ȡ��
    FailedPayment    // ����ʧ��
    // �ڿ���̨Ӧ����Ϊ��������ݲ������ѹ���״̬
};

// ������״̬ת��Ϊ�ַ���
std::string orderStatusToString(OrderStatus status);
// ���ַ���ת��Ϊ����״̬
OrderStatus stringToOrderStatus(const std::string& statusStr);


class Order {
private:
    std::string orderID;           // ����ID��Ψһ��ʶ��
    std::string consumerUsername;  // �������û���
    std::vector<OrderItem> items;  // �������б�
    double totalAmount;            // �����ܽ��
    OrderStatus status;            // ����״̬
    std::time_t creationTime;      // ����ʱ�䣨ʱ�����
    std::time_t lastUpdateTime;    // ������ʱ�䣨�縶��ʱ�䡢ȡ��ʱ��ȣ�

public:
    // ���湹�캯���������¶���ʱʹ�ã�
    Order(std::string id, std::string cUsername, const std::vector<OrderItem>& oItems);
    // �ļ����ع��캯���������л�ʱʹ�ã�
    Order(std::string id, std::string cUsername, const std::vector<OrderItem>& oItems,
        double total, OrderStatus stat, std::time_t cTime, std::time_t uTime);


    // ��ȡ����Getters��
    std::string getID() const { return orderID; }             // ��ȡ����ID
    std::string getConsumerUsername() const { return consumerUsername; } // ��ȡ�������û���
    const std::vector<OrderItem>& getItems() const { return items; } // ��ȡ�������б��������ã�
    double getTotalAmount() const { return totalAmount; }     // ��ȡ�����ܽ��
    OrderStatus getStatus() const { return status; }          // ��ȡ����״̬
    std::time_t getCreationTime() const { return creationTime; } // ��ȡ����ʱ��
    std::time_t getLastUpdateTime() const { return lastUpdateTime; } // ��ȡ������ʱ��


    // ���ö���״̬
    void setStatus(OrderStatus newStatus);
    // ���¼��㶩���ܽ����ڶ�����ڶ�������ʱʹ�ã����ܴ�����ͨ�����䣩
    void calculateTotalAmount();
    // ��ʾ��������
    void displayOrderDetails() const;

    // ���л�����������������ڴ洢��orders.txt����ʽΪ�����ı���
    void serialize(std::ostream& os) const;
    // �����л�ͨ����OrderManager�������ڴ�����ʵ�֣�
};

#endif // ORDER_H