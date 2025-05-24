#define   _CRT_SECURE_NO_WARNINGS

#include "Order.h"
#include <iostream>
#include <iomanip> // ����std::fixed, std::setprecision
#include <algorithm> // �������л�ʱ�滻���ţ�������Ҫ��

// ����������������״̬ת��Ϊ�ַ���
std::string orderStatusToString(OrderStatus status) {
    switch (status) {
    case OrderStatus::PendingPayment: return "������";
    case OrderStatus::Paid: return "�Ѹ���";
    case OrderStatus::Cancelled: return "��ȡ��";
    case OrderStatus::FailedPayment: return "����ʧ��";
    default: return "δ֪";
    }
}

// �������������ַ���ת��Ϊ����״̬
OrderStatus stringToOrderStatus(const std::string& statusStr) {
    if (statusStr == "PendingPayment") return OrderStatus::PendingPayment;
    if (statusStr == "Paid") return OrderStatus::Paid;
    if (statusStr == "Cancelled") return OrderStatus::Cancelled;
    if (statusStr == "FailedPayment") return OrderStatus::FailedPayment;
    // �����׳�����򷵻�Ĭ��ֵ
    std::cerr << "���棺δ֪�Ķ���״̬�ַ���'" << statusStr << "'��Ĭ������Ϊ����ʧ�ܡ�" << std::endl;
    return OrderStatus::FailedPayment;
}


Order::Order(std::string id, std::string cUsername, const std::vector<OrderItem>& oItems)
    : orderID(id), consumerUsername(cUsername), items(oItems), status(OrderStatus::PendingPayment) {
    creationTime = std::time(nullptr); // ��ȡ��ǰʱ���
    lastUpdateTime = creationTime;
    calculateTotalAmount(); // ���㶩���ܽ��
}

// ���ļ����صĹ��캯��
Order::Order(std::string id, std::string cUsername, const std::vector<OrderItem>& oItems,
    double total, OrderStatus stat, std::time_t cTime, std::time_t uTime)
    : orderID(id), consumerUsername(cUsername), items(oItems), totalAmount(total),
    status(stat), creationTime(cTime), lastUpdateTime(uTime) {
    // ֱ�Ӵ����ܽ������¼��㣩
}


void Order::setStatus(OrderStatus newStatus) {
    status = newStatus;
    lastUpdateTime = std::time(nullptr); // ����������ʱ��Ϊ��ǰʱ��
}

void Order::calculateTotalAmount() {
    totalAmount = 0.0;
    for (const auto& item : items) {
        totalAmount += item.getSubtotal(); // �ۼ�ÿ������������ܼ�
    }
}

void Order::displayOrderDetails() const {
    char timeBuffer[80];
    // ��ʱ�����ʽ��Ϊ"��-��-�� ʱ:��:��"
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&creationTime));
    std::string creationTimeStr(timeBuffer);

    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&lastUpdateTime));
    std::string updateTimeStr(timeBuffer);

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "�������: " << orderID << std::endl;
    std::cout << "������: " << consumerUsername << std::endl;
    std::cout << "״̬: " << orderStatusToString(status) << std::endl;
    std::cout << "����ʱ��: " << creationTimeStr << std::endl;
    std::cout << "������ʱ��: " << updateTimeStr << std::endl;
    std::cout << "�ܽ��: $" << std::fixed << std::setprecision(2) << totalAmount << std::endl;
    std::cout << "��Ʒ�� (" << items.size() << "):" << std::endl;
    for (const auto& item : items) {
        item.display(); // ��ʾÿ������������
    }
    std::cout << "----------------------------------------" << std::endl;
}

void Order::serialize(std::ostream& os) const {
    // ���л���ʽ��CSV��ʽ���ֶ��ö��ŷָ�
    os << orderID << ","
        << consumerUsername << ","
        << totalAmount << ","
        << orderStatusToString(status) << ","
        << creationTime << "," // ��time_t�洢Ϊ��������ֵ
        << lastUpdateTime << "," // ��time_t�洢Ϊ��������ֵ
        << items.size(); // ��Ʒ������

    for (const auto& item : items) {
        // �滻��Ʒ�����еĶ��ţ������ƻ�CSV��ʽ��
        std::string safeProductName = item.productName;
        std::replace(safeProductName.begin(), safeProductName.end(), ',', ';'); // �÷ֺ��滻����

        os << "," << item.productID // ��ƷID
            << "," << safeProductName // ��������Ʒ����
            << "," << item.merchantUsername // �̼��û���
            << "," << item.priceAtOrder // �µ�ʱ�۸�
            << "," << item.quantity; // ��������
    }
}