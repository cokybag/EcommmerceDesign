#pragma once
#ifndef CARTITEM_H
#define CARTITEM_H

#include "Products.h" // ����Product�ඨ���ڴ˴�
#include <string>
#include <iomanip>

// ��Product.h���ݽ϶࣬��ǰ������
// class Product; 

struct CartItem {
    Product* product;    // ָ��ʵ����Ʒ��ָ��
    int quantity;        // ���ﳵ�е�����

    // ���캯������ʼ����Ʒָ�������
    CartItem(Product* p, int qty) : product(p), quantity(qty) {}

    // Ĭ�Ϲ��캯������mapʹ�ã�������Ҫ��
    CartItem() : product(nullptr), quantity(0) {}

    // �������ܼۣ���Ʒ��ǰ�ۼ� �� ������
    double getSubtotal() const {
        if (product) {
            return product->getCurrentSalePrice() * quantity;
        }
        return 0.0;
    }

    // ��ʾ���ﳵ��Ŀ����
    void display() const {
        if (product) {
            std::cout << "  - ��Ʒ: " << product->getName()
                << " (ID: " << product->getID() << ")"
                << ", ����: " << quantity
                << ", ����: $" << std::fixed << std::setprecision(2) << product->getCurrentSalePrice()
                << ", ���ܼ�: $" << getSubtotal() << std::endl;
        }
    }
};

#endif // CARTITEM_H