#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>
#include <fstream>
#include <iostream> // ����Ǳ�ڵĵ���/��ʾ����

class Product {
protected:
    std::string productID;          // ��ƷID��Ψһ��ʶ��
    std::string name;               // ��Ʒ����
    std::string description;        // ��Ʒ����
    double originalPrice;           // ԭ��
    double currentSalePrice;        // ��Ʒ�ۿۼۣ���ǰ�ۼۣ�
    int stock;                      // �������
    std::string ownerMerchantUsername; // �����̼��û���
    std::string productType;        // ��Ʒ���ͣ���"Book", "Food", "Clothing"��

public:
    // ���캯��
    Product(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner, std::string type);
    virtual ~Product() = default;   // ������������֧�ֶ�̬��

    // ��ȡ�۸���麯�������ص�ǰ�ۼۣ�
    virtual double getPrice() const;

    // ��ȡ����Getters��
    std::string getID() const;              // ��ȡ��ƷID
    std::string getName() const;            // ��ȡ��Ʒ����
    std::string getDescription() const;     // ��ȡ��Ʒ����
    double getOriginalPrice() const;        // ��ȡԭ��
    double getCurrentSalePrice() const;     // ��ȡ��ǰ�ۼۣ��ۿۼۣ�
    int getStock() const;                   // ��ȡ�������
    std::string getOwnerMerchantUsername() const; // ��ȡ�����̼��û���
    std::string getProductType() const;     // ��ȡ��Ʒ����

    // ��������Setters����Ҫ���̼�ʹ�ã�
    void setOriginalPrice(double price);     // ����ԭ�ۣ����ܴ����ۼ۵�����
    void setCurrentSalePrice(double price);  // ���õ�ǰ�ۼۣ������ۿ۹���
    void setStock(int newStock);             // ���ÿ������
    void setDescription(const std::string& desc); // ���²�Ʒ����
    void setName(const std::string& newName); // ���²�Ʒ����

    // ��ʾ��Ʒ������麯������������д��
    virtual void displayDetails() const;

    // �ļ��洢��أ����л������������������д��
    virtual void serialize(std::ostream& os) const;
    // �����л���ProductManager�������ڻ�����ʵ�֣�
};

// ʾ�����ࣺ�鼮��
class Book : public Product {
public:
    // ���캯�������û��๹�캯����ָ������Ϊ"Book"��
    Book(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner);
    // �̳�getPrice()���������߼��������ͬ��������д��
    void displayDetails() const override; // ��д��ʾ���鷽��
    void serialize(std::ostream& os) const override; // ��д���л�����
};

// ʾ�����ࣺʳƷ��
class Food : public Product {
public:
    Food(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner);
    void displayDetails() const override;
    void serialize(std::ostream& os) const override;
};

// ʾ�����ࣺ��װ��
class Clothing : public Product {
public:
    Clothing(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner);
    void displayDetails() const override;
    void serialize(std::ostream& os) const override;
};


#endif // PRODUCT_H