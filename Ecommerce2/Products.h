#pragma once
#ifndef PRODUCTS_H
#define PRODUCTS_H

#include <string>
#include <iostream>
#include <fstream> // ����serialize�е�ostream

class Product {
protected:
    std::string productID;           // ��ƷID
    std::string name;                // ��Ʒ����
    std::string description;         // ��Ʒ����
    double originalPrice;            // ԭ��
    double currentSalePrice;         // ��ǰ���ۼ�
    int totalStock;                  // �ܿ�棨��'stock'������Ϊ'totalStock'����ȷ���壩
    int reservedStock;               // �£�Ϊ��������Ԥ���Ŀ��
    std::string ownerMerchantUsername; // �����̼��û���
    std::string productType;         // ��Ʒ���ͣ���"Book", "Food", "Electronics", "SciFiBook"��

public:
    // ���캯��
    Product(std::string id, std::string n, std::string desc, double origP, double saleP, int initialStock, std::string owner, std::string type);
    virtual ~Product() = default;    // ����������

    virtual double getPrice() const; // ʵ�ʷ���currentSalePrice

    // ��ȡ������
    std::string getID() const;
    std::string getName() const;
    std::string getDescription() const;
    double getOriginalPrice() const;
    double getCurrentSalePrice() const;
    int getTotalStock() const;       // ��ȡ��ʵ����
    int getReservedStock() const;    // ��ȡΪ����Ԥ���Ŀ��
    int getAvailableStock() const;   // ��ȡ���ÿ�棨�ܿ�� - Ԥ����棩
    std::string getOwnerMerchantUsername() const;
    std::string getProductType() const;

    // ����������
    void setOriginalPrice(double price);
    void setCurrentSalePrice(double price);
    void setTotalStock(int newTotalStock); // �滻setStock
    void setDescription(const std::string& desc);
    void setName(const std::string& newName);

    // �������Ԥ������
    bool reserveStock(int quantity);   // �ɹ�����true
    bool releaseReservedStock(int quantity); // �ɹ�����true
    bool confirmSaleFromReserved(int quantity); // �����ܿ���Ԥ�����

    virtual void displayDetails() const; // ��ʾ��Ʒ����
    virtual void serialize(std::ostream& os) const; // ���л�Ϊ�����
};

// �鼮�� - �̳���Product
class Book : public Product {
public:
    Book(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner);
    void displayDetails() const override; // ��д��ʾ���鷽��
    void serialize(std::ostream& os) const override; // ��д���л�����
    // ���getPriceֻ�Ƿ���currentSalePrice����������д
};

// ʳƷ�� - �̳���Product
class Food : public Product {
public:
    Food(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner);
    void displayDetails() const override; // ��д��ʾ���鷽��
    void serialize(std::ostream& os) const override; // ��д���л�����
};

// ��װ�� - �̳���Product
class Clothing : public Product {
public:
    Clothing(std::string id, std::string n, std::string desc, double origP, double saleP, int s, std::string owner);
    void displayDetails() const override; // ��д��ʾ���鷽��
    void serialize(std::ostream& os) const override; // ��д���л�����
};

#endif // PRODUCTS_H