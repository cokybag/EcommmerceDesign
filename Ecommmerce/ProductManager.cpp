#include "ProductManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm> // ����std::max, std::sort, std::unique, std::transform
#include <set> // ���ڻ�ȡΨһ��Ʒ����

// ����split����ȫ�ֿ��û��ڹ���util.h�ж���
// ��δ���壬��ȷ������ʵ�֣���������UserManager.cpp�򹤾��ļ���
extern std::vector<std::string> split(const std::string& s, char delimiter);

// ��Ʒ���������캯��
ProductManager::ProductManager() : nextProductID(1) {
    loadProductsFromFile(); // ���ļ��������в�Ʒ����
    if (!products.empty()) { // �������Ѽ��صĲ�Ʒ
        for (const auto* p : products) { // �������в�Ʒ
            try {
                // ������ƷID������ID��ʽΪ"P����"����"P123"��
                if (p->getID().length() > 1 && p->getID()[0] == 'P') {
                    int idNum = std::stoi(p->getID().substr(1)); // ��ȡ���ֲ���
                    // ������һ������ID��ȡ���ֵ+1��
                    nextProductID = std::max(nextProductID, idNum + 1);
                }
            }
            catch (const std::exception& e) {
                // ���Խ������󣨾�Ĭ���棬���жϳ���
                // std::cerr << "Warning: Could not parse product ID " << p->getID() << std::endl;
            }
        }
    }
}

// �����������ͷ����ж�̬����Ĳ�Ʒ����
ProductManager::~ProductManager() {
    for (Product* product : products) {
        delete product; // �ͷŵ�����Ʒ����
    }
    products.clear(); // ��ղ�Ʒ�б�
}

// �����µĲ�ƷID����ʽΪ"P+��������"����"P1"��"P2"��
std::string ProductManager::generateNewProductID() {
    return "P" + std::to_string(nextProductID++); // ����������ת��Ϊ�ַ���
}

// ���ļ����ز�Ʒ���ݣ�CSV��ʽ��
void ProductManager::loadProductsFromFile() {
    std::ifstream inFile(filename); // �򿪲�Ʒ�����ļ�
    if (!inFile.is_open()) { // ���ļ���ʧ�ܣ�ֱ�ӷ���
        return;
    }

    std::string line;
    while (std::getline(inFile, line)) { // ���ж�ȡ�ļ�
        if (line.empty()) continue; // ��������

        // �����ŷָ�������Ϊ�ֶ��б�
        std::vector<std::string> tokens = split(line, ',');
        if (tokens.size() == 8) { // ��֤�ֶ������Ƿ����CSV��ʽ��8���ֶΣ�
            try {
                // �������ֶ�ֵ
                std::string id = tokens[0]; // ��ƷID
                std::string name = tokens[1]; // ��Ʒ���ƣ��滻�ֺ�Ϊ���ţ���ԭԭʼ���ݣ�
                std::replace(name.begin(), name.end(), ';', ',');
                std::string desc = tokens[2]; // ��Ʒ������ͬ�ϣ�
                std::replace(desc.begin(), desc.end(), ';', ',');
                double origP = std::stod(tokens[3]); // ԭ��
                double saleP = std::stod(tokens[4]); // �ۼ�
                int stock = std::stoi(tokens[5]); // ���
                std::string owner = tokens[6]; // �����̼��û���
                std::string type = tokens[7]; // ��Ʒ���ͣ�Book/Food/Clothing��

                Product* newProd = nullptr; // ��ʼ����Ʒָ��

                // �������ʹ��������Ʒ����
                if (type == "Book") {
                    newProd = new Book(id, name, desc, origP, saleP, stock, owner);
                }
                else if (type == "Food") {
                    newProd = new Food(id, name, desc, origP, saleP, stock, owner);
                }
                else if (type == "Clothing") {
                    newProd = new Clothing(id, name, desc, origP, saleP, stock, owner);
                }
                else { // δ֪���ͣ�����������
                    std::cerr << "Warning: Unknown product type '" << type << "' for product '" << name << "' in file. Skipping." << std::endl;
                    continue;
                }

                if (newProd) products.push_back(newProd); // ����Ч��Ʒ��ӵ��б�

            }
            catch (const std::invalid_argument& ia) { // ������ֵ��������
                std::cerr << "Error parsing numeric value for a product in file: " << ia.what() << " Line: " << line << std::endl;
            }
            catch (const std::out_of_range& oor) { // ������ֵ��Χ����
                std::cerr << "Numeric value out of range for a product in file. Line: " << line << std::endl;
            }
        }
        else { // �ֶ�������������������
            std::cerr << "Warning: Malformed line in product file: " << line << ". Skipping." << std::endl;
        }
    }
    inFile.close(); // �ر��ļ�
}

// ����Ʒ���ݱ��浽�ļ���CSV��ʽ��
void ProductManager::saveProductsToFile() const {
    std::ofstream outFile(filename); // ���ļ�����д��
    if (!outFile.is_open()) { // ����ʧ�ܣ�������󲢷���
        std::cerr << "Error: Could not open product file for writing: " << filename << std::endl;
        return;
    }
    for (const Product* product : products) { // �������в�Ʒ
        product->serialize(outFile); // ���ò�Ʒ�����л�����д������
        outFile << std::endl; // ÿ�����ݺ���
    }
    outFile.close(); // �ر��ļ�
}

// �־û����ģ����浽�ļ���
void ProductManager::persistChanges() {
    saveProductsToFile(); // ���ñ��淽��
}

// ����²�Ʒ
bool ProductManager::addProduct(const std::string& name, const std::string& description, double originalPrice,
    int stock, const std::string& ownerMerchantUsername, const std::string& productType) {
    std::string newID = generateNewProductID(); // ������ID
    Product* newProd = nullptr; // ��ʼ����Ʒָ��

    // �������ʹ��������Ʒ���󣨳�ʼ�ۼ۵���ԭ�ۣ�
    if (productType == "Book") {
        newProd = new Book(newID, name, description, originalPrice, originalPrice, stock, ownerMerchantUsername);
    }
    else if (productType == "Food") {
        newProd = new Food(newID, name, description, originalPrice, originalPrice, stock, ownerMerchantUsername);
    }
    else if (productType == "Clothing") {
        newProd = new Clothing(newID, name, description, originalPrice, originalPrice, stock, ownerMerchantUsername);
    }
    else { // ��֧�ֵ����ͣ�������󲢻���ID������
        std::cout << "Unsupported product category. Please choose Book, Food, or Clothing." << std::endl;
        nextProductID--; // ����ID����
        return false;
    }

    // �����̼Һ�����Ƿ�����Ч���ۿ۹���
    double discountPercent = getActiveCategoryDiscount(ownerMerchantUsername, productType);
    if (discountPercent > 0.0) { // ��������Ч�ۿ�
        // �����ۺ�۲����²�Ʒ�ۼ�
        double newSalePrice = newProd->getOriginalPrice() * (1.0 - (discountPercent / 100.0));
        newProd->setCurrentSalePrice(newSalePrice);
        // �����ʾ��Ϣ
        std::cout << "Info: Product '" << name << "' automatically discounted by " << discountPercent << "% upon adding." << std::endl;
    }

    products.push_back(newProd); // ��Ӳ�Ʒ���б�
    saveProductsToFile(); // ���浽�ļ�
    return true;
}

// ����ID���Ҳ�Ʒ������ָ�룬δ�ҵ��򷵻�nullptr��
Product* ProductManager::findProductByID(const std::string& id) {
    // ʹ��lambda���ʽ���б��в���ƥ��ID�Ĳ�Ʒ
    auto it = std::find_if(products.begin(), products.end(),
        [&](const Product* p) { return p->getID() == id; });
    if (it != products.end()) { // �ҵ��򷵻�ָ��
        return *it;
    }
    return nullptr; // δ�ҵ�����nullptr
}

// ��ȡ���в�Ʒ�б�������
std::vector<Product*> ProductManager::getAllProducts() const {
    return products; // ���ز�Ʒ�б���
}

// ��ȡָ���̼ҵ����в�Ʒ
std::vector<Product*> ProductManager::getProductsByMerchant(const std::string& merchantUsername) const {
    std::vector<Product*> result; // ����б�
    for (Product* p : products) { // �������в�Ʒ
        if (p->getOwnerMerchantUsername() == merchantUsername) { // ƥ���̼��û���
            result.push_back(p); // ��ӵ�����б�
        }
    }
    return result; // ���ؽ���б�
}

// ������Ʒ��֧�ְ����ơ����͡��̼������������ִ�Сд��
std::vector<Product*> ProductManager::searchProducts(const std::string& searchTerm, const std::string& searchBy) const {
    std::vector<Product*> results; // ����б�
    std::string termLower = searchTerm; // ת��������ΪСд
    std::transform(termLower.begin(), termLower.end(), termLower.begin(),
        [](unsigned char c) { return std::tolower(c); }); // ת���������ַ�תСд

    for (Product* p : products) { // �������в�Ʒ
        bool match = false; // ƥ���־

        // ������������ִ�в�ͬƥ���߼�
        if (searchBy == "name") { // ����������
            std::string nameLower = p->getName(); // ��ȡ��Ʒ���Ʋ�תСд
            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(),
                [](unsigned char c) { return std::tolower(c); });
            // ����Ƿ���������ʣ������ִ�Сд��
            if (nameLower.find(termLower) != std::string::npos) {
                match = true;
            }
        }
        else if (searchBy == "type") { // ����������
            std::string typeLower = p->getProductType(); // ��ȡ��Ʒ���Ͳ�תСд
            std::transform(typeLower.begin(), typeLower.end(), typeLower.begin(),
                [](unsigned char c) { return std::tolower(c); });
            if (typeLower.find(termLower) != std::string::npos) {
                match = true;
            }
        }
        else if (searchBy == "merchant") { // ���̼�����
            std::string merchantLower = p->getOwnerMerchantUsername(); // ��ȡ�̼�����תСд
            std::transform(merchantLower.begin(), merchantLower.end(), merchantLower.begin(),
                [](unsigned char c) { return std::tolower(c); });
            if (merchantLower.find(termLower) != std::string::npos) {
                match = true;
            }
        }

        if (match) { // ��ƥ��ɹ�����ӵ�����б�
            results.push_back(p);
        }
    }
    return results; // �����������
}

// ɾ����Ʒ������֤�Ƿ�Ϊ�̼����в�Ʒ��
bool ProductManager::removeProduct(const std::string& productID, const std::string& merchantUsername) {
    // ����ƥ��ID�Ĳ�Ʒ
    auto it = std::find_if(products.begin(), products.end(),
        [&](const Product* p) { return p->getID() == productID; });

    if (it != products.end()) { // �ҵ���Ʒ
        if ((*it)->getOwnerMerchantUsername() == merchantUsername) { // ��֤���ڵ�ǰ�̼�
            delete* it; // �ͷ��ڴ�
            products.erase(it); // ���б��Ƴ�
            saveProductsToFile(); // �������
            return true;
        }
        else { // ���̼����в�Ʒ���ܾ�ɾ��
            std::cout << "Error: You can only remove your own products." << std::endl;
            return false;
        }
    }
    // δ�ҵ���Ʒ
    std::cout << "Error: Product ID not found." << std::endl;
    return false;
}

// ���²�Ʒ��Ϣ�����浽�ļ���
void ProductManager::updateProduct(Product* product) {
    if (product) { // ����Ʒָ����Ч
        saveProductsToFile(); // ���±������в�Ʒ���ݣ��������л���
    }
}

// ��ȡ���п��ò�Ʒ���ͣ�ȥ�غ���б�
std::vector<std::string> ProductManager::getAvailableProductTypes() const {
    std::set<std::string> typesSet; // ʹ��set�Զ�ȥ��
    for (const auto* p : products) { // �������в�Ʒ
        typesSet.insert(p->getProductType()); // ������͵�set
    }
    return std::vector<std::string>(typesSet.begin(), typesSet.end()); // ת��Ϊ�б���
}

// Ӧ������ۿۣ������̼�ĳ����Ʒ���ۿ۹��򣬲��������в�Ʒ��
void ProductManager::applyCategoryDiscount(const std::string& merchantUsername, const std::string& categoryType, double discountPercentage) {
    if (discountPercentage < 0 || discountPercentage > 100) { // ��֤�ۿ۷�Χ
        std::cout << "Error: Invalid discount percentage. Must be between 0 and 100." << std::endl;
        return;
    }

    // �洢�ۿ۹��򣨹����̼�-���-�ۿ��ʣ�
    activeCategoryDiscounts[merchantUsername][categoryType] = discountPercentage;

    // Ӧ���ۿ۵����в�Ʒ
    int count = 0; // ��¼��Ӱ��Ĳ�Ʒ����
    for (Product* p : products) {
        // ƥ���̼Һ����
        if (p->getOwnerMerchantUsername() == merchantUsername && p->getProductType() == categoryType) {
            // �����ۺ�۲�����
            double newSalePrice = p->getOriginalPrice() * (1.0 - (discountPercentage / 100.0));
            p->setCurrentSalePrice(newSalePrice);
            count++; // ������һ
        }
    }

    if (count > 0) { // ���в�Ʒ������
        saveProductsToFile(); // �������
        std::cout << count << " of your products in category '" << categoryType
            << "' have been discounted by " << discountPercentage << "%." << std::endl; // ����ɹ���Ϣ
    }
    else { // �����в�Ʒƥ�䣬�������ѱ��棨������δ����ӵĲ�Ʒ��
        std::cout << "You have no products in category '" << categoryType << "' to apply discount to, but the rule is set for future additions." << std::endl;
    }
}

// ��ȡ��Ч����ۿۣ������ۿ��ʣ����򷵻�0��
double ProductManager::getActiveCategoryDiscount(const std::string& merchantUsername, const std::string& categoryType) const {
    // �����̼ҵ��ۿ۹���
    auto merchIt = activeCategoryDiscounts.find(merchantUsername);
    if (merchIt != activeCategoryDiscounts.end()) { // �̼Ҵ����ۿ۹���
        auto catIt = merchIt->second.find(categoryType); // ���������ۿ۹���
        if (catIt != merchIt->second.end()) { // �������ۿ۹���
            return catIt->second; // �����ۿ���
        }
    }
    return 0.0; // ����Ч�ۿ�
}