#pragma once
#ifndef ORDERITEM_H
#define ORDERITEM_H

#include <string>
#include <iostream>
#include <iomanip>

struct OrderItem {
    std::string productID;         // ��ƷID
    std::string productName;       // ��Ʒ����
    std::string merchantUsername;  // �̼��û��������ڹ�����ȷ���̼ң�
    double priceAtOrder;           // �µ�ʱ�ļ۸񣨹̶��۸񣬲��ܺ����ۿ�Ӱ�죩
    int quantity;                  // ��������

    // ���캯������ʼ����������Ϣ
    OrderItem(std::string pid, std::string pName, std::string mUser, double price, int qty)
        : productID(pid), productName(pName), merchantUsername(mUser), priceAtOrder(price), quantity(qty) {
    }

    // Ĭ�Ϲ��캯������������������С��������Ҫ
    OrderItem() : productID(""), productName(""), merchantUsername(""), priceAtOrder(0.0), quantity(0) {}


    // �������ܼۣ��µ�ʱ�۸� �� ������
    double getSubtotal() const {
        return priceAtOrder * quantity;
    }

    // ��ʾ����������
    void display() const {
        std::cout << "    - ��Ʒ: " << productName << " (ID: " << productID << ")"
            << ", ����: " << quantity
            << ", ֧������: $" << std::fixed << std::setprecision(2) << priceAtOrder
            << ", ���ܼ�: $" << getSubtotal()
            << " (�����̼�: " << merchantUsername << ")" << std::endl;
    }
};

#endif // ORDERITEM_H