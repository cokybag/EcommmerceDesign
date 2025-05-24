#include "Products.h"
#include <iomanip> // ����std::fixed��std::setprecision
#include <sstream> // �����ַ�����
#include <algorithm>

// ��Ʒ�๹�캯��
Product::Product(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner, std::string type)
    : productID(id), name(n), description(desc), originalPrice(origP), currentSalePrice(saleP), stock(s), ownerMerchantUsername(owner), productType(type) {
}

// ��ȡ��Ʒ��ǰ�۸�
double Product::getPrice() const {
    return currentSalePrice;
}

// ��ȡ��ƷID
std::string Product::getID() const { return productID; }
// ��ȡ��Ʒ����
std::string Product::getName() const { return name; }
// ��ȡ��Ʒ����
std::string Product::getDescription() const { return description; }
// ��ȡ��Ʒԭ��
double Product::getOriginalPrice() const { return originalPrice; }
// ��ȡ��Ʒ��ǰ�ۼ�
double Product::getCurrentSalePrice() const { return currentSalePrice; }
// ��ȡ��Ʒ���
int Product::getStock() const { return stock; }
// ��ȡ��Ʒ�����̼��û���
std::string Product::getOwnerMerchantUsername() const { return ownerMerchantUsername; }
// ��ȡ��Ʒ����
std::string Product::getProductType() const { return productType; }

// ���ò�Ʒԭ��
// ����¼۸���Ч���Ǹ����������ԭ��
// �����ǰ�ۼ۸�����ԭ�ۣ����ۼ۵���Ϊԭ��
void Product::setOriginalPrice(double price) {
    if (price >= 0) originalPrice = price;
    // �����ǰ�ۼ۸���ԭ�ۣ�������Ҫ������ǰ�ۼ�
    if (currentSalePrice > originalPrice) currentSalePrice = originalPrice;
}

// ���ò�Ʒ��ǰ�ۼ�
// ȷ���ۼ۲�����0�Ҳ�����ԭ��
void Product::setCurrentSalePrice(double price) {
    if (price >= 0 && price <= originalPrice) { // �ۿۼ۲��ܸ���ԭ��
        currentSalePrice = price;
    }
    else if (price > originalPrice) {
        currentSalePrice = originalPrice; // ����Ϊԭ��
    }
}

// ���ò�Ʒ���������ȷ�����Ǹ���
void Product::setStock(int newStock) {
    if (newStock >= 0) stock = newStock;
}

// ���²�Ʒ����
void Product::setDescription(const std::string& desc) {
    description = desc;
}

// ���²�Ʒ����
void Product::setName(const std::string& newName) {
    name = newName;
}

// ��ʾ��Ʒ��ϸ��Ϣ
void Product::displayDetails() const {
    std::cout << "-------------------------------------\n"
        << "ID: " << productID << "\n"
        << "����: " << name << "\n"
        << "����: " << productType << "\n"
        << "����: " << description << "\n"
        << "ԭ��: $" << std::fixed << std::setprecision(2) << originalPrice << "\n"
        << "��ǰ�۸�: $" << std::fixed << std::setprecision(2) << currentSalePrice << "\n"
        << "���: " << stock << "\n"
        << "�̼�: " << ownerMerchantUsername << "\n"
        << "-------------------------------------" << std::endl;
}

// ����Ʒ��Ϣ���л�ΪCSV��ʽ
// �滻���ƺ������еĶ��ţ���ֹ�ƻ�CSV��ʽ
void Product::serialize(std::ostream& os) const {
    // �滻���ƺ������еĶ��ţ������ƻ�CSV��ʽ
    std::string safeName = name;
    std::replace(safeName.begin(), safeName.end(), ',', ';');
    std::string safeDesc = description;
    std::replace(safeDesc.begin(), safeDesc.end(), ',', ';');

    os << productID << ","
        << safeName << ","
        << safeDesc << ","
        << originalPrice << ","
        << currentSalePrice << ","
        << stock << ","
        << ownerMerchantUsername << ","
        << productType;
}

// --- �鼮�� ---
// �鼮�๹�캯��
Book::Book(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner)
    : Product(id, n, desc, origP, saleP, s, owner, "Book") {
}

// ��ʾ�鼮��ϸ��Ϣ
void Book::displayDetails() const {
    // std::cout << "--- �鼮���� ---" << std::endl;
    Product::displayDetails();
    // ������鼮�ض���Ϣ�������ߡ�ISBN��
}

// ���л��鼮��Ϣ
void Book::serialize(std::ostream& os) const {
    Product::serialize(os); // ����д�����������Ϣ
}

// --- ʳƷ�� ---
// ʳƷ�๹�캯��
Food::Food(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner)
    : Product(id, n, desc, origP, saleP, s, owner, "Food") {
}

// ��ʾʳƷ��ϸ��Ϣ
void Food::displayDetails() const {
    // std::cout << "--- ʳƷ���� ---" << std::endl;
    Product::displayDetails();
    // �����ʳƷ�ض���Ϣ���籣���ڵ�
}

// ���л�ʳƷ��Ϣ
void Food::serialize(std::ostream& os) const {
    Product::serialize(os);
}

// --- ��װ�� ---
// ��װ�๹�캯��
Clothing::Clothing(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner)
    : Product(id, n, desc, origP, saleP, s, owner, "Clothing") {
}

// ��ʾ��װ��ϸ��Ϣ
void Clothing::displayDetails() const {
    // std::cout << "--- ��װ���� ---" << std::endl;
    Product::displayDetails();
    // ����ӷ�װ�ض���Ϣ����ߴ硢��ɫ��
}

// ���л���װ��Ϣ
void Clothing::serialize(std::ostream& os) const {
    Product::serialize(os);
}