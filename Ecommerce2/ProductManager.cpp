#include "ProductManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm> 
#include <set> 
#include "utils.h"
//extern std::vector<std::string> split(const std::string& s, char delimiter);

// ��Ʒ���������캯������ʼ����ƷID�����������ļ����ز�Ʒ
ProductManager::ProductManager() : nextProductID(1) {
    loadProductsFromFile();
    if (!products.empty()) {
        for (const auto* p : products) {
            try {
                if (p->getID().length() > 1 && p->getID()[0] == 'P') {
                    int idNum = std::stoi(p->getID().substr(1));
                    nextProductID = std::max(nextProductID, idNum + 1);
                }
            }
            catch (const std::exception& e) {
                // �����쳣�����־�Ĭ
            }
        }
    }
}

// ���������������Ʒ�ڴ沢��ղ�Ʒ�б�
ProductManager::~ProductManager() {
    for (Product* product : products) {
        delete product;
    }
    products.clear();
}

// �����µĲ�ƷID����ʽ��P+���֣�
std::string ProductManager::generateNewProductID() {
    return "P" + std::to_string(nextProductID++);
}

// ���ļ����ز�Ʒ����
void ProductManager::loadProductsFromFile() {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        return; // �ļ������ڻ��޷��򿪣�ֱ�ӷ���
    }

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        // ��ǰ��ʽ��ID,����,����,ԭ��,�ۼ�,�ܿ��,�̼�,���� (��������Ԥ�������Ϊ7���ֶ�)
        std::vector<std::string> tokens = split(line, ',');
        // ������Ԥ����棬�˴�ӦΪtokens.size() == 9 (ID,����,����,ԭ��,�ۼ�,�ܿ��,Ԥ�����,�̼�,����)
        // Ŀǰ����8���ֶΣ�ID,����,����,ԭ��,�ۼ�,���,�̼�,����
        if (tokens.size() == 8) {
            try {
                std::string id = tokens[0];
                std::string name = tokens[1];
                std::replace(name.begin(), name.end(), ';', ','); // ��ԭ���滻�Ķ���
                std::string desc = tokens[2];
                std::replace(desc.begin(), desc.end(), ';', ','); // ��ԭ���滻�Ķ���
                double origP = std::stod(tokens[3]);
                double saleP = std::stod(tokens[4]);
                int totalStockVal = std::stoi(tokens[5]); // �����ܿ��
                // int reservedStockVal = 0; // ��������Ԥ�������Ĭ��Ϊ0
                // if (tokens.size() == 9) reservedStockVal = std::stoi(tokens[6]); // ������Ԥ�����
                std::string owner = tokens[6]; // ������Ԥ����������������
                std::string type = tokens[7];  // ������Ԥ����������������

                // ������Ӧ���͵Ĳ�Ʒ����
                Product* newProd = nullptr;
                if (type == "Book") {
                    // ���ܿ��ֵ���ݸ����캯��
                    newProd = new Book(id, name, desc, origP, saleP, totalStockVal, owner);
                }
                else if (type == "Food") {
                    newProd = new Food(id, name, desc, origP, saleP, totalStockVal, owner);
                }
                else if (type == "Clothing") {
                    newProd = new Clothing(id, name, desc, origP, saleP, totalStockVal, owner);
                }
                else {
                    std::cerr << "���棺�ļ��в�Ʒ'" << name << "'������'" << type << "'δ֪�������ò�Ʒ��" << std::endl;
                    continue;
                }

                // �����ļ�����Ԥ�������������Ԥ�����ķ�����
                // if (newProd && tokens.size() == 9) { /* newProd->setReservedStock_after_creation(reservedStockVal); */ }
                // ĿǰԤ������ʼ��Ϊ0������OrderManager���ض���ʱ����

                if (newProd) products.push_back(newProd);

            }
            catch (const std::invalid_argument& ia) {
                std::cerr << "�����ļ��в�Ʒ����ֵʱ����: " << ia.what() << " ������: " << line << std::endl;
            }
            catch (const std::out_of_range& oor) {
                std::cerr << "�ļ��в�Ʒ����ֵ������Χ��������: " << line << std::endl;
            }
        }
        else {
            std::cerr << "���棺��Ʒ�ļ��е��и�ʽ��������8���ֶΣ�: " << line << "���������С�" << std::endl;
        }
    }
    inFile.close();
}

// ����Ʒ���ݱ��浽�ļ�
void ProductManager::saveProductsToFile() const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "�����޷��򿪲�Ʒ�ļ�����д��: " << filename << std::endl;
        return;
    }
    for (const Product* product : products) {
        product->serialize(outFile); // Product::serialize����д���ܿ��
        outFile << std::endl;
    }
    outFile.close();
}

// �־û����и��ģ����浽�ļ���
void ProductManager::persistChanges() {
    saveProductsToFile();
}

// ����²�Ʒ
bool ProductManager::addProduct(const std::string& name, const std::string& description, double originalPrice,
    int initialTotalStock, const std::string& ownerMerchantUsername, const std::string& productType) { // ����������
    std::string newID = generateNewProductID();
    Product* newProd = nullptr;

    // ���ݲ�Ʒ���ʹ�����Ӧ����
    if (productType == "Book") {
        newProd = new Book(newID, name, description, originalPrice, originalPrice, initialTotalStock, ownerMerchantUsername);
    }
    else if (productType == "Food") {
        newProd = new Food(newID, name, description, originalPrice, originalPrice, initialTotalStock, ownerMerchantUsername);
    }
    else if (productType == "Clothing") {
        newProd = new Clothing(newID, name, description, originalPrice, originalPrice, initialTotalStock, ownerMerchantUsername);
    }
    else {
        std::cout << "��֧�ֵĲ�Ʒ�����ѡ���鼮��ʳƷ���װ��" << std::endl;
        nextProductID--; // ����ID������
        return false;
    }

    // Ӧ������ۿۣ�����У�
    double discountPercent = getActiveCategoryDiscount(ownerMerchantUsername, productType);
    if (discountPercent > 0.0) {
        double newSalePrice = newProd->getOriginalPrice() * (1.0 - (discountPercent / 100.0));
        newProd->setCurrentSalePrice(newSalePrice);
        std::cout << "��Ϣ����Ʒ'" << name << "'���ʱ�Զ�Ӧ��" << discountPercent << "%���ۿۡ�" << std::endl;
    }

    products.push_back(newProd);
    saveProductsToFile();
    return true;
}

// ����ID���Ҳ�Ʒ����const�汾��
Product* ProductManager::findProductByID(const std::string& id) {
    auto it = std::find_if(products.begin(), products.end(),
        [&](const Product* p) { return p->getID() == id; });
    if (it != products.end()) {
        return *it;
    }
    return nullptr;
}

// ��ȡ���в�Ʒ
std::vector<Product*> ProductManager::getAllProducts() const {
    return products; // ����ָ�������ĸ���
}

// ��ȡ�ض��̼ҵ����в�Ʒ
std::vector<Product*> ProductManager::getProductsByMerchant(const std::string& merchantUsername) const {
    std::vector<Product*> result;
    for (Product* p : products) { // ���products�洢Product*����˴�����������洢const Product*����resultҲӦΪconst Product*
        if (p->getOwnerMerchantUsername() == merchantUsername) {
            result.push_back(p);
        }
    }
    return result;
}

// ������Ʒ��֧�ְ����ơ����͡��̼�������
std::vector<Product*> ProductManager::searchProducts(const std::string& searchTerm, const std::string& searchBy) const {
    std::vector<Product*> results;
    std::string termLower = searchTerm;
    // ��������ת��ΪСд
    std::transform(termLower.begin(), termLower.end(), termLower.begin(),
        [](unsigned char c) { return std::tolower(c); });

    for (Product* p : products) {
        bool match = false;
        if (searchBy == "name") {
            std::string nameLower = p->getName();
            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(),
                [](unsigned char c) { return std::tolower(c); });
            if (nameLower.find(termLower) != std::string::npos) {
                match = true;
            }
        }
        else if (searchBy == "type") {
            std::string typeLower = p->getProductType();
            std::transform(typeLower.begin(), typeLower.end(), typeLower.begin(),
                [](unsigned char c) { return std::tolower(c); });
            if (typeLower.find(termLower) != std::string::npos) {
                match = true;
            }
        }
        else if (searchBy == "merchant") {
            std::string merchantLower = p->getOwnerMerchantUsername();
            std::transform(merchantLower.begin(), merchantLower.end(), merchantLower.begin(),
                [](unsigned char c) { return std::tolower(c); });
            if (merchantLower.find(termLower) != std::string::npos) {
                match = true;
            }
        }
        // �ɸ�����Ҫ��Ӹ�����������

        if (match) {
            results.push_back(p);
        }
    }
    return results;
}

// �Ƴ���Ʒ���̼�ֻ���Ƴ��Լ��Ĳ�Ʒ���Ҳ�Ʒ������Ԥ����棩
bool ProductManager::removeProduct(const std::string& productID, const std::string& merchantUsername) {
    auto it = std::find_if(products.begin(), products.end(),
        [&](const Product* p) { return p->getID() == productID; });

    if (it != products.end()) {
        // ��飺�����Ʒ��Ԥ����棨�����������������Ƴ�
        if ((*it)->getReservedStock() > 0) {
            std::cout << "���󣺲�Ʒ'" << (*it)->getName() << "'��" << (*it)->getReservedStock()
                << "����Ԥ�����ڴ����������޷��Ƴ���" << std::endl;
            std::cout << "��ȷ�������漰�˲�Ʒ�Ķ�������ɻ�ȡ����" << std::endl;
            return false;
        }
        if ((*it)->getOwnerMerchantUsername() == merchantUsername) {
            delete* it;
            products.erase(it);
            saveProductsToFile();
            return true;
        }
        else {
            std::cout << "������ֻ���Ƴ��Լ��Ĳ�Ʒ��" << std::endl;
            return false;
        }
    }
    std::cout << "����δ�ҵ��ò�ƷID��" << std::endl;
    return false;
}

// ���²�Ʒ��Ϣ��ʵ�ʽ������ļ����߼��򻯰棩
void ProductManager::updateProduct(Product* product) {
    if (product) {
        saveProductsToFile();
    }
}

// ��ȡ���п��õĲ�Ʒ����
std::vector<std::string> ProductManager::getAvailableProductTypes() const {
    std::set<std::string> typesSet;
    for (const auto* p : products) {
        typesSet.insert(p->getProductType());
    }
    return std::vector<std::string>(typesSet.begin(), typesSet.end());
}

// Ӧ������ۿۣ����ض��̼ҵ��ض�����Ʒ��
void ProductManager::applyCategoryDiscount(const std::string& merchantUsername, const std::string& categoryType, double discountPercentage) {
    if (discountPercentage < 0 || discountPercentage > 100) {
        std::cout << "������Ч���ۿ۰ٷֱȡ�������0��100֮�䡣" << std::endl;
        return;
    }
    activeCategoryDiscounts[merchantUsername][categoryType] = discountPercentage;
    int count = 0;
    for (Product* p : products) {
        if (p->getOwnerMerchantUsername() == merchantUsername && p->getProductType() == categoryType) {
            double newSalePrice = p->getOriginalPrice() * (1.0 - (discountPercentage / 100.0));
            p->setCurrentSalePrice(newSalePrice);
            count++;
        }
    }
    if (count > 0) {
        saveProductsToFile();
        std::cout << "�������Ϊ'" << categoryType
            << "'��" << count << "����Ʒ��Ӧ��" << discountPercentage << "%���ۿۡ�" << std::endl;
    }
    else {
        std::cout << "��û�����Ϊ'" << categoryType << "'�Ĳ�Ʒ��Ӧ���ۿۣ����˹��������ã���Ӧ����δ����ӵĲ�Ʒ��" << std::endl;
    }
}

// ��ȡ�ض��̼ҵ��ض����ǰ�ۿ۰ٷֱ�
double ProductManager::getActiveCategoryDiscount(const std::string& merchantUsername, const std::string& categoryType) const {
    auto merchIt = activeCategoryDiscounts.find(merchantUsername);
    if (merchIt != activeCategoryDiscounts.end()) {
        auto catIt = merchIt->second.find(categoryType);
        if (catIt != merchIt->second.end()) {
            return catIt->second;
        }
    }
    return 0.0; // Ĭ�����ۿ�
}