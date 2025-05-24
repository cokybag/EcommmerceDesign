#ifndef PRODUCTMANAGER_H
#define PRODUCTMANAGER_H

#include "Products.h"
#include <vector>
#include <string>
#include <algorithm> // ����std::sort, std::remove_if
#include <map>       // ���ڴ洢��Ʒ���ͺ�����ۿ�

class ProductManager {
private:
    std::vector<Product*> products;          // �洢���в�Ʒ��ָ���б�
    const std::string filename = "products.txt"; // �洢��Ʒ���ݵ��ļ�����Ĭ��"products.txt"��
    int nextProductID;                        // ��������Ψһ��ƷID�ļ�����

    // �洢�̼�����ۿ۹���
    // �����̼��û�����ֵ��������������ͣ�ֵ���ۿ۰ٷֱȣ���ӳ��
    std::map<std::string, std::map<std::string, double>> activeCategoryDiscounts;

    void loadProductsFromFile();              // ���ļ����ز�Ʒ���ݣ�˽�з�����
    void saveProductsToFile() const;          // ����Ʒ���ݱ��浽�ļ���˽�з�����
    std::string generateNewProductID();       // ����Ψһ��ƷID��˽�з�����

public:
    ProductManager();                         // ���캯������ʼ��ʱ�����ļ����ݣ�
    ~ProductManager();                        // �����������ͷ��ڴ棩

    // ����²�Ʒ
    // ���������ơ�������ԭ�ۡ���桢�����̼��û�������Ʒ���ͣ�Book/Food/Clothing��
    bool addProduct(const std::string& name, const std::string& description, double originalPrice,
        int stock, const std::string& ownerMerchantUsername, const std::string& productType);

    Product* findProductByID(const std::string& id); // ����ID���Ҳ�Ʒ������ָ�룬δ�ҵ�����nullptr��
    std::vector<Product*> getAllProducts() const;    // ��ȡ���в�Ʒ�б�������
    std::vector<Product*> getProductsByMerchant(const std::string& merchantUsername) const; // ��ȡָ���̼ҵĲ�Ʒ�б�

    // ������Ʒ��֧�ְ����ơ����͡��̼�������
    // searchBy������ѡֵ��"name"�����ƣ���"type"�����ͣ���"merchant"���̼ң�
    std::vector<Product*> searchProducts(const std::string& searchTerm, const std::string& searchBy) const;

    // ɾ����Ʒ���������̼�ɾ�����в�Ʒ��
    // ��������ƷID�������̼��û���������֤����Ȩ��
    bool removeProduct(const std::string& productID, const std::string& merchantUsername);

    void updateProduct(Product* product); // ���²�Ʒ��Ϣ�󱣴棨ͨ�ø��·�����
    void persistChanges(); // ��ʽ�����ݳ־û����ļ�������saveProductsToFile��

    // ��ȡ������Ч��Ʒ���ͣ�ȥ�غ���б���["Book", "Food", "Clothing"]��
    std::vector<std::string> getAvailableProductTypes() const;

    // ����ۿ۹�����
    void applyCategoryDiscount(const std::string& merchantUsername, const std::string& categoryType, double discountPercentage); // Ӧ��/��������ۿ�
    double getActiveCategoryDiscount(const std::string& merchantUsername, const std::string& categoryType) const; // ��ȡ��ǰ��Ч�ۿ��ʣ����򷵻�0��
};

#endif // PRODUCTMANAGER_H