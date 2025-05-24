#include "ShoppingCart.h"
#include "Products.h" // ����Product::getAvailableStock()����ȡ���ÿ�棩
#include <iostream>
#include <iomanip> // ����std::fixed, std::setprecision���̶�С����ʽ��

ShoppingCart::ShoppingCart() {
    // ���캯����map���Զ���ʼ��������������
}

bool ShoppingCart::addItem(Product* product, int quantity) {
    if (!product || quantity <= 0) { // ����Ʒָ����Ч�Ժ������Ϸ���
        std::cout << "������Ч�Ĳ�Ʒ��������" << std::endl;
        return false;
    }

    if (product->getAvailableStock() < quantity) { // �����ÿ���Ƿ��㹻
        std::cout << "����" << product->getName() << "�Ŀ��ÿ�治�㡣����������" << product->getAvailableStock() << std::endl;
        return false;
    }

    std::string productID = product->getID();
    auto it = items.find(productID);
    if (it != items.end()) { // ��Ʒ�Ѵ����ڹ��ﳵ����������
        if (product->getAvailableStock() < (it->second.quantity + quantity)) { // ���������Ŀ���Ƿ��㹻
            std::cout << "�����޷����" << quantity << "��" << product->getName()
                << "�����ﳵ����������" << it->second.quantity
                << "��ƽ̨���ÿ�棺" << product->getAvailableStock() << std::endl;
            return false;
        }
        it->second.quantity += quantity; // ��������
    }
    else { // �²�Ʒ���빺�ﳵ
        items[productID] = CartItem(product, quantity); // ��������Ŀ
    }
    std::cout << quantity << "��'" << product->getName() << "'�Ѽ��빺�ﳵ��" << std::endl;
    return true;
}

bool ShoppingCart::removeItem(const std::string& productID, int quantityToRemove) {
    if (quantityToRemove <= 0) { // ����Ƴ������Ϸ���
        std::cout << "�����Ƴ���������Ϊ������" << std::endl;
        return false;
    }

    auto it = items.find(productID);
    if (it != items.end()) {
        if (it->second.quantity > quantityToRemove) { // �����Ƴ�
            it->second.quantity -= quantityToRemove;
            std::cout << quantityToRemove << "��'" << it->second.product->getName() << "'�Ѵӹ��ﳵ�Ƴ���" << std::endl;
        }
        else { // ȫ���Ƴ�
            std::cout << "���ﳵ������'" << it->second.product->getName() << "'���Ƴ���" << std::endl;
            items.erase(it); // ɾ����Ŀ
        }
        return true;
    }
    else {
        std::cout << "���󣺹��ﳵ��δ�ҵ���ƷID " << productID << "��" << std::endl;
        return false;
    }
}

bool ShoppingCart::updateItemQuantity(const std::string& productID, int newQuantity) {
    if (newQuantity < 0) { // ����������Ϸ��ԣ�����Ϊ����
        std::cout << "��������������Ϊ������" << std::endl;
        return false;
    }

    auto it = items.find(productID);
    if (it != items.end()) {
        if (newQuantity == 0) { // ������Ϊ0ʱ�Ƴ���Ŀ
            std::cout << "������Ϊ0����Ʒ'" << it->second.product->getName() << "'�Ѵӹ��ﳵ�Ƴ���" << std::endl;
            items.erase(it);
            return true;
        }
        // ����������Ƿ񳬹����ÿ��
        if (it->second.product->getAvailableStock() < newQuantity) {
            std::cout << "�����޷���" << it->second.product->getName() << "����������Ϊ" << newQuantity
                << "�����ÿ�棺" << it->second.product->getAvailableStock() << std::endl;
            return false;
        }
        it->second.quantity = newQuantity; // ��������
        std::cout << "'" << it->second.product->getName() << "'�������Ѹ���Ϊ" << newQuantity << "��" << std::endl;
        return true;
    }
    else {
        std::cout << "���󣺹��ﳵ��δ�ҵ���ƷID " << productID << "�Ը���������" << std::endl;
        return false;
    }
}

void ShoppingCart::clearItem(const std::string& productID) {
    auto it = items.find(productID);
    if (it != items.end()) { // �Ƴ�ָ����Ŀ
        std::cout << "��Ʒ'" << it->second.product->getName() << "'�Ѵӹ��ﳵ��ȫ�Ƴ���" << std::endl;
        items.erase(it);
    }
    else {
        std::cout << "���ﳵ��δ�ҵ���ƷID " << productID << "��ִ�����������" << std::endl;
    }
}


void ShoppingCart::clearCart() {
    if (!items.empty()) { // ��չ��ﳵ
        items.clear();
        std::cout << "���ﳵ����ա�" << std::endl;
    }
    else {
        std::cout << "���ﳵ��Ϊ�ա�" << std::endl;
    }
}

const std::map<std::string, CartItem>& ShoppingCart::getItems() const {
    return items; // ���ع��ﳵ��Ŀ���������ã�
}

CartItem* ShoppingCart::getItem(const std::string& productID) {
    auto it = items.find(productID);
    if (it != items.end()) {
        return &(it->second); // ������Ŀָ��
    }
    return nullptr; // δ�ҵ����ؿ�ָ��
}


double ShoppingCart::getTotalValue() const {
    double total = 0.0;
    for (const auto& pair : items) {
        total += pair.second.getSubtotal(); // �ۼ�������Ŀ�����ܼ�
    }
    return total; // �����ܼ�ֵ
}

bool ShoppingCart::isEmpty() const {
    return items.empty(); // �жϹ��ﳵ�Ƿ�Ϊ��
}

void ShoppingCart::displayCart() const {
    if (items.empty()) { // ���ﳵΪ��ʱ��ʾ
        std::cout << "��Ĺ��ﳵΪ�ա�" << std::endl;
        return;
    }
    std::cout << "\n--- ��Ĺ��ﳵ ---" << std::endl;
    for (const auto& pair : items) {
        pair.second.display(); // ��ʾÿ����Ŀ����
    }
    std::cout << "--------------------------" << std::endl;
    std::cout << "���ﳵ�ܼ�ֵ��$" << std::fixed << std::setprecision(2) << getTotalValue() << std::endl; // ��ʾ�ܼ�ֵ����λС����
    std::cout << "--------------------------" << std::endl;
}