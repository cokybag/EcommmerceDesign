#pragma once
#ifndef SHOPPINGCART_H
#define SHOPPINGCART_H

#include "CartItem.h"
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <iomanip>

class ShoppingCart {
private:
    // ʹ�ò�ƷID��Ϊ���Ա��ڲ��Һ͸���
    std::map<std::string, CartItem> items;

public:
    ShoppingCart();

    // ���һ�������Ĳ�Ʒ�������Ʒ���ڹ��ﳵ�У�����������
    // �ɹ�����true����治�㷵��false��
    bool addItem(Product* product, int quantity);

    // �Ƴ�һ�������Ĳ�Ʒ�����Ҫ�Ƴ������� >= ��ǰ�������Ƴ�����Ŀ��
    // �ɹ�����true��
    bool removeItem(const std::string& productID, int quantityToRemove);

    // ����������Ŀ��������
    // �ɹ�����true�������������治�㷵��false��
    bool updateItemQuantity(const std::string& productID, int newQuantity);

    // �ӹ��ﳵ����ȫ�Ƴ�ĳ����Ŀ��
    void clearItem(const std::string& productID);

    void clearCart(); // ��չ��ﳵ

    const std::map<std::string, CartItem>& getItems() const; // ��ȡ���ﳵ��Ŀ���������ã�
    CartItem* getItem(const std::string& productID); // ������Ŀ��δ�ҵ�����nullptr

    double getTotalValue() const; // ���㹺�ﳵ�ܼ�ֵ
    bool isEmpty() const; // �жϹ��ﳵ�Ƿ�Ϊ��
    void displayCart() const; // ��ʾ���ﳵ����
};

#endif // SHOPPINGCART_H