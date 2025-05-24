#pragma once
#ifndef PRODUCTMANAGER_H
#define PRODUCTMANAGER_H

#include "Products.h"
#include <vector>
#include <string>
#include <algorithm> // ����std::sort, std::remove_if
#include <map>       // ���ڲ�Ʒ���ͺ�����ۿ�

class ProductManager {
private:
    std::vector<Product*> products;          // �洢���в�Ʒ��ָ��
    const std::string filename = "products.txt"; // ��Ʒ���ݴ洢���ļ���
    int nextProductID;                       // ��������Ψһ��ƷID�ļ�����

    // �洢�ض��̼Ҷ�ĳ���Ʒ���õ��ۿ۰ٷֱ�
    // �����̼��û�����ֵ��������������ͣ�ֵ���ۿ۰ٷֱȣ���ӳ��
    std::map<std::string, std::map<std::string, double>> activeCategoryDiscounts;

    void loadProductsFromFile();              // ���ļ����ز�Ʒ����
    void saveProductsToFile() const;          // ����Ʒ���ݱ��浽�ļ�
    std::string generateNewProductID();       // �����µ�Ψһ��ƷID

public:
    ProductManager();                         // ���캯��
    ~ProductManager();                        // ��������

    // ����²�Ʒ
    // ���������ơ�������ԭ�ۡ���桢�����̼��û�������Ʒ����
    bool addProduct(const std::string& name, const std::string& description, double originalPrice,
        int stock, const std::string& ownerMerchantUsername, const std::string& productType);

    Product* findProductByID(const std::string& id); // ����ID���Ҳ�Ʒ
    std::vector<Product*> getAllProducts() const;    // ��ȡ���в�Ʒ
    std::vector<Product*> getProductsByMerchant(const std::string& merchantUsername) const; // ��ȡ�ض��̼ҵ����в�Ʒ
    // ������Ʒ��searchBy������ѡ��"name"�����ƣ�"type"�����ͣ�"merchant"���̼ң�
    std::vector<Product*> searchProducts(const std::string& searchTerm, const std::string& searchBy) const;

    // ɾ����Ʒ���̼�ֻ��ɾ���Լ��Ĳ�Ʒ��
    bool removeProduct(const std::string& productID, const std::string& merchantUsername);
    void updateProduct(Product* product);     // ���²�Ʒ��Ϣ������

    void persistChanges();                    // ��ʽ�����ı��浽�ļ�

    std::vector<std::string> getAvailableProductTypes() const; // ��ȡ���п��õĲ�Ʒ����

    // ����ۿ۹����·���
    void applyCategoryDiscount(const std::string& merchantUsername, const std::string& categoryType, double discountPercentage); // Ӧ������ۿ�
    double getActiveCategoryDiscount(const std::string& merchantUsername, const std::string& categoryType) const; // ��ȡ�����ۿ�
};

#endif // PRODUCTMANAGER_H