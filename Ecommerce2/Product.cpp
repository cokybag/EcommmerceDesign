#include "Products.h"
#include <iomanip> // ����std::fixed��std::setprecision���̶�С��λ�������
#include <sstream> // �����ַ���������
#include <algorithm> // ����std::replace���ַ����滻��

// Product ��ʵ��
Product::Product(std::string id, std::string n, std::string desc, double origP, double saleP, int initialStock, std::string owner, std::string type)
    : productID(id), name(n), description(desc), originalPrice(origP), currentSalePrice(saleP),
    totalStock(initialStock), reservedStock(0), // ��ʼ��Ԥ�����Ϊ0
    ownerMerchantUsername(owner), productType(type) {
}

double Product::getPrice() const {
    return currentSalePrice; // ���ص�ǰ���ۼ�
}

std::string Product::getID() const { return productID; }
std::string Product::getName() const { return name; }
std::string Product::getDescription() const { return description; }
double Product::getOriginalPrice() const { return originalPrice; }
double Product::getCurrentSalePrice() const { return currentSalePrice; }
int Product::getTotalStock() const { return totalStock; } // ��ȡ�ܿ��
int Product::getReservedStock() const { return reservedStock; } // ��ȡԤ�����

int Product::getAvailableStock() const {
    return totalStock - reservedStock; // ���ÿ�� = �ܿ�� - Ԥ�����
}

std::string Product::getOwnerMerchantUsername() const { return ownerMerchantUsername; }
std::string Product::getProductType() const { return productType; }


void Product::setOriginalPrice(double price) {
    if (price >= 0) originalPrice = price; // ȷ��ԭ�۷Ǹ�
    if (currentSalePrice > originalPrice) currentSalePrice = originalPrice; // �ۼ۲�����ԭ��
}

void Product::setCurrentSalePrice(double price) {
    if (price >= 0 && price <= originalPrice) { // �ۼ��ں���Χ�ڣ�0 �� �ۼ� �� ԭ�ۣ�
        currentSalePrice = price;
    }
    else if (price > originalPrice) { // �ۼ۳���ԭ��ʱ���Զ���Ϊԭ��
        currentSalePrice = originalPrice;
    }
    // �����ۼ۲�������ѡ����־��¼����Ϊ0���˴��ݲ�����
}

void Product::setTotalStock(int newTotalStock) { // ��������setStock���ܿ�����ã�
    if (newTotalStock >= 0) {
        // ȷ���ܿ�治������Ԥ�����
        if (newTotalStock < reservedStock) {
            std::cerr << "���棺��Ʒ" << productID << "���ܿ������Ϊ" << newTotalStock << "������Ԥ�����" << reservedStock << "���Զ�����ΪԤ�����ֵ��" << std::endl;
            totalStock = reservedStock;
        }
        else {
            totalStock = newTotalStock;
        }
    }
}

void Product::setDescription(const std::string& desc) {
    description = desc; // ���²�Ʒ����
}
void Product::setName(const std::string& newName) {
    name = newName; // ���²�Ʒ����
}

// ���Ԥ������
bool Product::reserveStock(int quantity) {
    if (quantity <= 0) return false; // ����Ԥ����������
    if (getAvailableStock() >= quantity) { // ���ÿ���㹻ʱִ��Ԥ��
        reservedStock += quantity;
        return true;
    }
    return false; // ��治�㣬Ԥ��ʧ��
}

bool Product::releaseReservedStock(int quantity) {
    if (quantity <= 0) return false;
    if (reservedStock >= quantity) { // �����������ͷ�Ԥ�����
        reservedStock -= quantity;
        return true;
    }
    // �����ͷų���Ԥ�����Ŀ��ʱ���ͷ�ȫ��Ԥ������¼����
    std::cerr << "���棺��Ʒ" << productID << "�����ͷ�" << quantity << "��Ԥ����棬����ǰ��Ԥ��" << reservedStock << "�������ͷ�ȫ��Ԥ����档" << std::endl;
    reservedStock = 0;
    return false; // ��ʾ���ֳɹ�������쳣
}

bool Product::confirmSaleFromReserved(int quantity) {
    if (quantity <= 0) return false;
    if (reservedStock >= quantity && totalStock >= quantity) { // Ԥ�������ܿ����㹻ʱȷ������
        totalStock -= quantity; // �����ܿ��
        reservedStock -= quantity; // ����Ԥ�����
        if (reservedStock < 0) reservedStock = 0; // ��ֹԤ�����Ϊ������Ӧ�ɵ����߼���֤���˴���Ϊ�����Դ���
        return true;
    }
    return false; // ��治�㣬����ȷ��ʧ�ܣ������߼��²�Ӧ������
}


void Product::displayDetails() const {
    std::cout << "-------------------------------------\n"
        << "ID: " << productID << "\n" // ��ƷID
        << "Name: " << name << "\n" // ��Ʒ����
        << "Type: " << productType << "\n" // ��Ʒ����
        << "Description: " << description << "\n" // ��Ʒ����
        << "Original Price: $" << std::fixed << std::setprecision(2) << originalPrice << "\n" // ԭ�ۣ�������λС����
        << "Current Price: $" << std::fixed << std::setprecision(2) << currentSalePrice << "\n" // ��ǰ�ۼۣ�������λС����
        << "Total Stock: " << totalStock << "\n" // �ܿ��
        << "Reserved Stock: " << reservedStock << "\n" // Ԥ�����
        << "Available Stock: " << getAvailableStock() << "\n" // ���ÿ��
        << "Sold by: " << ownerMerchantUsername << "\n" // �����̼�
        << "-------------------------------------" << std::endl;
}

void Product::serialize(std::ostream& os) const {
    std::string safeName = name;
    std::replace(safeName.begin(), safeName.end(), ',', ';'); // �滻�����еĶ���Ϊ�ֺţ�����CSV�������⣩
    std::string safeDesc = description;
    std::replace(safeDesc.begin(), safeDesc.end(), ',', ';'); // �滻�����еĶ���Ϊ�ֺ�

    os << productID << "," // ��ƷID
        << safeName << "," // �����Ĳ�Ʒ����
        << safeDesc << "," // �����Ĳ�Ʒ����
        << originalPrice << "," // ԭ��
        << currentSalePrice << "," // ��ǰ�ۼ�
        << totalStock << "," // �ܿ�棨Ԥ����治���棬����ʱͨ�������ؽ���
        << ownerMerchantUsername << "," // �̼��û���
        << productType; // ��Ʒ����
    // ע�⣺Ԥ����棨reservedStock��δ���浽�ļ�����ͨ�������������¼���
}

// --- �鼮�� ---
Book::Book(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner)
    : Product(id, n, desc, origP, saleP, s, owner, "Book") { // ���͹̶�Ϊ"Book"
}

void Book::displayDetails() const {
    Product::displayDetails(); // ���û����������ʾ�߼�
}
void Book::serialize(std::ostream& os) const {
    Product::serialize(os); // ���û�������л��߼�
}

// --- ʳƷ�� ---
Food::Food(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner)
    : Product(id, n, desc, origP, saleP, s, owner, "Food") { // ���͹̶�Ϊ"Food"
}

void Food::displayDetails() const {
    Product::displayDetails();
}
void Food::serialize(std::ostream& os) const {
    Product::serialize(os);
}

// --- ��װ�� ---
Clothing::Clothing(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner)
    : Product(id, n, desc, origP, saleP, s, owner, "Clothing") { // ���͹̶�Ϊ"Clothing"
}

void Clothing::displayDetails() const {
    Product::displayDetails();
}
void Clothing::serialize(std::ostream& os) const {
    Product::serialize(os);
}