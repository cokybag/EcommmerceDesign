// (main.cpp ��һ������֮ǰ�İ����ļ��͸�������)
#include <iostream>
#include <string>
#include <vector>
#include <limits> 
#include <algorithm> 
#include <iomanip> 
#include <set>

#include "UserManager.h"
#include "ProductManager.h"
// User.h �� Product.h ͨ�� UserManager.h �� ProductManager.h ��������

// �˵�������ǰ������
void showMainMenu(UserManager& um, ProductManager& pm, User*& currentUser);
void showConsumerMenu(UserManager& um, ProductManager& pm, User*& currentUser);
void showMerchantMenu(UserManager& um, ProductManager& pm, User*& currentUser);

// ��׳���븨������
template <typename T>
T getValidatedInput(const std::string& prompt) {
    T value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.good()) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ��ջ�����
            return value;
        }
        else {
            std::cout << "������Ч�������ԡ�" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

std::string getLineInput(const std::string& prompt, bool allowEmpty = false) {
    std::string value;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, value);
        if (allowEmpty || !value.empty()) {
            return value;
        }
        std::cout << "���벻��Ϊ�գ������ԡ�" << std::endl;
    }
}


// --- �û���ز��� --- 
void handleRegister(UserManager& um) {
    std::cout << "\n--- �û�ע�� ---" << std::endl;
    std::string uname = getLineInput("�������û�����");
    if (um.isUsernameTaken(uname)) {
        std::cout << "�û����ѱ�ռ�ã��볢�������û�����" << std::endl;
        return;
    }

    std::string pwd1, pwd2;
    do {
        pwd1 = getLineInput("���������루����Ϊ�գ���");
        pwd2 = getLineInput("��ȷ�����룺");
        if (pwd1 != pwd2) {
            std::cout << "���벻һ�£������ԡ�" << std::endl;
        }
    } while (pwd1 != pwd2);

    std::string typeChoice;
    std::cout << "ע�����ͣ�1�������� ��2���̼ң�";
    std::getline(std::cin, typeChoice);

    std::string accountType;
    if (typeChoice == "1") accountType = "Consumer";
    else if (typeChoice == "2") accountType = "Merchant";
    else {
        std::cout << "ѡ����Ч��ע��ʧ�ܡ�" << std::endl;
        return;
    }

    if (um.registerUser(uname, pwd1, accountType)) {
        std::cout << "ע��ɹ�������Ϊ " << accountType << "��" << std::endl;
    }
    else {
        std::cout << "ע��ʧ�ܣ��û��������ѱ�ռ�û�������Ч��" << std::endl;
    }
}

void handleLogin(UserManager& um, User*& currentUser) {
    std::cout << "\n--- �û���¼ ---" << std::endl;
    if (currentUser) {
        std::cout << "�ѵ�¼���û���" << currentUser->getUsername() << std::endl;
        return;
    }
    std::string uname = getLineInput("�������û�����");
    std::string pwd = getLineInput("���������룺");

    currentUser = um.loginUser(uname, pwd);
    if (currentUser) {
        std::cout << "��¼�ɹ�����ӭ " << currentUser->getUsername() << "��" << std::endl;
    }
    else {
        std::cout << "��¼ʧ�ܣ��û������������" << std::endl;
    }
}

void handleLogout(User*& currentUser, UserManager& um) {
    if (currentUser) {
        std::cout << "�����˳� " << currentUser->getUsername() << "��" << std::endl;
        um.persistChanges();
        currentUser = nullptr;
    }
    else {
        std::cout << "δ��¼��" << std::endl;
    }
}

void handleChangePassword(UserManager& um, User* currentUser) {
    if (!currentUser) {
        std::cout << "�������ȵ�¼�����޸����롣" << std::endl;
        return;
    }
    std::cout << "\n--- �޸����� ---" << std::endl;
    std::string oldPwd = getLineInput("�����뵱ǰ���룺");
    if (!currentUser->checkPassword(oldPwd)) {
        std::cout << "��ǰ�������" << std::endl;
        return;
    }

    std::string pwd1, pwd2;
    do {
        pwd1 = getLineInput("�����������루����Ϊ�գ���");
        pwd2 = getLineInput("��ȷ�������룺");
        if (pwd1 != pwd2) {
            std::cout << "�����벻һ�£������ԡ�" << std::endl;
        }
    } while (pwd1 != pwd2);

    currentUser->setPassword(pwd1);
    um.persistChanges();
    std::cout << "�����޸ĳɹ���" << std::endl;
}

void handleBalanceManagement(UserManager& um, User* currentUser) {
    if (!currentUser) {
        std::cout << "�������ȵ�¼���ܹ�����" << std::endl;
        return;
    }
    std::cout << "\n--- ������ ---" << std::endl;
    std::cout << "��ǰ��$" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
    std::cout << "1. ��ֵ���" << std::endl;
    std::cout << "2. �鿴������ʾ��" << std::endl;
    // ����1��ͨ��ͨ������չʾ"Consume"���֡�
    // ����Ҫ������1���Ƴ���ֱ�ӹ����ܣ�
    // ��˱�Ҫʱ������ͨ�õ�����ѡ�����"����"��
    // ���������ֵ�Ͳ鿴���ܼ�����������������
    // Ϊ��������˴���������ֵ�Ͳ鿴���ܡ�
    // ������ȷչʾ"����"�������"����"ѡ�
    std::cout << "0. ����" << std::endl;

    std::string choiceStr = getLineInput("��ѡ��");
    int choice = -1;
    try {
        choice = std::stoi(choiceStr);
    }
    catch (const std::exception& e) {
        std::cout << "������Ч��" << std::endl;
        return;
    }

    if (choice == 1) {
        double amount = getValidatedInput<double>("�������ֵ��$");
        if (amount <= 0) {
            std::cout << "��ֵ������Ϊ������" << std::endl;
            return;
        }
        currentUser->deposit(amount);
        um.persistChanges();
        std::cout << "��ֵ�ɹ�������$" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
    }
    else if (choice == 0) {
        return;
    }
    else if (choice != 2) {
        std::cout << "ѡ����Ч��" << std::endl;
    }
}
// --- �û���ز������� ---


// --- ��Ʒ��ز�����ͨ�ã� ---
void displayProducts(const std::vector<Product*>& productList) {
    if (productList.empty()) {
        std::cout << "û�п���ʾ����Ʒ��" << std::endl;
        return;
    }
    std::cout << "\n--- ������Ʒ ---" << std::endl;
    for (const auto* product : productList) {
        product->displayDetails();
    }
}

void handleDisplayAllProducts(ProductManager& pm) {
    displayProducts(pm.getAllProducts());
}

void handleSearchProducts(ProductManager& pm) {
    std::cout << "\n--- ������Ʒ ---" << std::endl;
    std::cout << "����������1�����ƣ���2�����ͣ���3���̼ң�";
    std::string choiceStr = getLineInput("");
    int choice = -1;
    try {
        choice = std::stoi(choiceStr);
    }
    catch (const std::exception& e) {
        std::cout << "������Ч��" << std::endl;
        return;
    }

    std::string searchTerm = getLineInput("�����������ʣ�");
    std::string searchBy;

    if (choice == 1) searchBy = "name";
    else if (choice == 2) searchBy = "type";
    else if (choice == 3) searchBy = "merchant";
    else {
        std::cout << "����������Ч��" << std::endl;
        return;
    }

    std::vector<Product*> results = pm.searchProducts(searchTerm, searchBy);
    if (results.empty()) {
        std::cout << "δ�ҵ�������������Ʒ��" << std::endl;
    }
    else {
        std::cout << "--- ������� ---" << std::endl;
        displayProducts(results);
    }
}

// --- �������ض����� ---
// ��������1��Ҫ���Ƴ���handlePurchaseProduct����
/*
void handlePurchaseProduct(UserManager& um, ProductManager& pm, User* consumerUser) {
    // ... �������������Ƴ� ...
}
*/

// --- �̼��ض����� ---
void handleAddProduct(ProductManager& pm, User* merchantUser) {
    if (!merchantUser || merchantUser->getUserType() != "Merchant") {
        std::cout << "���󣺱������̼���ݵ�¼��" << std::endl;
        return;
    }
    Merchant* merchant = static_cast<Merchant*>(merchantUser);

    std::cout << "\n--- �������Ʒ ---" << std::endl;
    std::string name = getLineInput("��������Ʒ���ƣ�");
    std::string description = getLineInput("��������Ʒ������");
    double originalPrice = -1.0;
    while (originalPrice < 0) {
        originalPrice = getValidatedInput<double>("��������Ʒԭ�ۣ�$");
        if (originalPrice < 0) std::cout << "�۸���Ϊ������" << std::endl;
    }
    int stock = -1;
    while (stock < 0) {
        stock = getValidatedInput<int>("��������Ʒ���������");
        if (stock < 0) std::cout << "��治��Ϊ������" << std::endl;
    }

    std::string productTypeChoice;
    std::string productType; // ������ʵ������"Book"��"Food"��"Clothing"
    while (true) {
        std::cout << "ѡ����Ʒ�������ʵ������\n1. �鼮\n2. ʳƷ\n3. ��װ\n��ѡ��";
        std::getline(std::cin, productTypeChoice);
        if (productTypeChoice == "1") { productType = "Book"; break; }
        if (productTypeChoice == "2") { productType = "Food"; break; }
        if (productTypeChoice == "3") { productType = "Clothing"; break; }
        std::cout << "ѡ����Ч����ѡ��1��2��3��Ϊ������Ʒ���" << std::endl;
    }

    // ��ѡ�������̼�ָ������ϸ�������ַ�����������Ҫ����
    // ��C++����Ϊ��������֮һ��
    // ��������1��productTypeΪ"Book"��"Food"��"Clothing"���㹻��

    if (pm.addProduct(name, description, originalPrice, stock, merchant->getUsername(), productType)) {
        std::cout << "��Ʒ'" << name << "'��ӳɹ���" << std::endl;
        // �ۿ�Ӧ�ý���pm.addProduct�ڲ��������й�����ڣ�
    }
    else {
        std::cout << "�����Ʒʧ�ܡ�" << std::endl;
    }
}

void handleManageMyProducts(ProductManager& pm, User* merchantUser) {
    if (!merchantUser || merchantUser->getUserType() != "Merchant") {
        std::cout << "���󣺱������̼���ݵ�¼��" << std::endl;
        return;
    }
    Merchant* merchant = static_cast<Merchant*>(merchantUser);

    std::cout << "\n--- �����ҵ���Ʒ ---" << std::endl;
    std::vector<Product*> myProducts = pm.getProductsByMerchant(merchant->getUsername());
    if (myProducts.empty()) {
        std::cout << "����δ����κ���Ʒ��" << std::endl;
        return;
    }
    displayProducts(myProducts);

    std::string productID = getLineInput("������Ҫ�������ƷID����0ȡ������");
    if (productID == "0") return;

    Product* product = pm.findProductByID(productID);
    if (!product || product->getOwnerMerchantUsername() != merchant->getUsername()) {
        std::cout << "��ƷIDδ�ҵ�������ӵ�и���Ʒ��" << std::endl;
        return;
    }

    std::cout << "\n������Ʒ��" << product->getName() << "��ID��" << product->getID() << "��" << std::endl;
    std::cout << "1. ���¼۸�ԭ��&�ۼۣ�" << std::endl;
    std::cout << "2. ���¿��" << std::endl;
    std::cout << "3. �����ۿۣ����´���Ʒ���ۼۣ�" << std::endl;
    std::cout << "4. ɾ����Ʒ" << std::endl;
    std::cout << "5. ��������" << std::endl;
    std::cout << "6. ��������" << std::endl;
    std::cout << "0. ����" << std::endl;

    std::string choiceStr = getLineInput("��ѡ��");
    int choice = -1;
    try {
        choice = std::stoi(choiceStr);
    }
    catch (const std::exception& e) {
        std::cout << "������Ч��" << std::endl;
        return;
    }

    bool changed = false;
    switch (choice) {
    case 1: {
        double newOrigPrice = -1.0;
        while (newOrigPrice < 0) {
            newOrigPrice = getValidatedInput<double>("��������ԭ�ۣ�$");
            if (newOrigPrice < 0) std::cout << "�۸���Ϊ������\n";
        }
        product->setOriginalPrice(newOrigPrice);
        double newSalePrice = -1.0;
        while (newSalePrice < 0 || newSalePrice > newOrigPrice) {
            newSalePrice = getValidatedInput<double>("���������ۼۣ����ܳ���ԭ�ۣ���$");
            if (newSalePrice < 0) std::cout << "�۸���Ϊ������\n";
            if (newSalePrice > newOrigPrice) std::cout << "�ۼ۲��ܳ���ԭ�ۡ�\n";
        }
        product->setCurrentSalePrice(newSalePrice);
        changed = true;
        break;
    }
    case 2: {
        int newStock = -1;
        while (newStock < 0) {
            newStock = getValidatedInput<int>("�������¿��������");
            if (newStock < 0) std::cout << "��治��Ϊ������\n";
        }
        product->setStock(newStock);
        changed = true;
        break;
    }
    case 3: {
        double discountPercent = -1.0;
        while (discountPercent < 0 || discountPercent > 100) {
            discountPercent = getValidatedInput<double>("���������Ʒ���ۿ۰ٷֱȣ���10��ʾ9�ۣ���");
            if (discountPercent < 0 || discountPercent > 100) std::cout << "�ۿ۱�����0��100֮�䡣\n";
        }
        double newSalePrice = product->getOriginalPrice() * (1.0 - (discountPercent / 100.0));
        product->setCurrentSalePrice(newSalePrice);
        std::cout << "����Ʒ���ۼ�������Ϊ��$" << std::fixed << std::setprecision(2) << product->getCurrentSalePrice() << std::endl;
        changed = true;
        break;
    }
    case 4: {
        std::string confirm = getLineInput("ȷ��ɾ������Ʒ����y/n����");
        if (confirm == "y" || confirm == "Y") {
            if (pm.removeProduct(product->getID(), merchant->getUsername())) {
                std::cout << "��Ʒ��ɾ����" << std::endl;
            }
            else {
                std::cout << "ɾ����Ʒʧ�ܡ�" << std::endl;
            }
            return;
        }
        break;
    }
    case 5: {
        std::string newDesc = getLineInput("��������������");
        product->setDescription(newDesc);
        changed = true;
        break;
    }
    case 6: {
        std::string newName = getLineInput("�����������ƣ�");
        product->setName(newName);
        changed = true;
        break;
    }
    case 0:
        return;
    default:
        std::cout << "ѡ����Ч��" << std::endl;
        break;
    }

    if (changed) {
        pm.updateProduct(product);
        std::cout << "��Ʒ�����Ѹ��¡�" << std::endl;
    }
}

void handleDiscountCategory(ProductManager& pm, User* merchantUser) {
    if (!merchantUser || merchantUser->getUserType() != "Merchant") {
        std::cout << "���󣺱������̼���ݵ�¼��" << std::endl;
        return;
    }
    Merchant* merchant = static_cast<Merchant*>(merchantUser);

    std::cout << "\n--- �����������Ʒ�ۿۣ����������Ʒ�� ---" << std::endl;

    std::vector<Product*> myProducts = pm.getProductsByMerchant(merchant->getUsername());
    if (myProducts.empty()) {
        std::cout << "��û�п������ۿ۵���Ʒ��" << std::endl;
        return;
    }

    std::set<std::string> myTypesSet;
    for (const auto* p : myProducts) {
        myTypesSet.insert(p->getProductType());
    }
    if (myTypesSet.empty() && pm.getAvailableProductTypes().empty()) { // ����̼����ͺ�ƽ̨��������
        std::cout << "δ�ҵ�������Ʒ��ƽ̨�ϵ���Ʒ���" << std::endl;
        return;
    }

    std::cout << "����ǰ����Ʒ���";
    if (myTypesSet.empty()) std::cout << "�ޡ� ";
    for (const auto& type : myTypesSet) {
        std::cout << type << " ";
    }
    std::cout << "\n����ƽ̨��� ";
    std::vector<std::string> allTypes = pm.getAvailableProductTypes();
    if (allTypes.empty()) std::cout << "�ޡ�";
    for (const auto& type : allTypes) {
        std::cout << type << " ";
    }
    std::cout << std::endl;


    std::string categoryToDiscount = getLineInput("������ҪӦ��/�����ۿ۵���Ʒ�����Book��Food��Clothing���� ");
    // ��֤����ַ�������ѡ���������κ��ַ�������δ�����ͣ�
    // Ϊ��������˴�����Ϊ��֪����֮һ����ProductManager�ɴ����κ��ַ�����
    bool platformHasCategory = false;
    for (const auto& type : allTypes) {
        if (type == categoryToDiscount) {
            platformHasCategory = true;
            break;
        }
    }
    // ���Ҫ�ϸ����ƣ��ɼ����ѡ����Ƿ�Ϊ�������֮һ��
    if (categoryToDiscount != "Book" && categoryToDiscount != "Food" && categoryToDiscount != "Clothing" && !platformHasCategory) {
        std::cout << "���棺'" << categoryToDiscount << "'���Ǳ�׼��������� "
            << "������ƻ���Ӵ�����Ʒ���Կ�Ϊ�������ۿ۹���" << std::endl;
    }


    double discountPercent = -1.0;
    while (discountPercent < 0 || discountPercent > 100) {
        discountPercent = getValidatedInput<double>("�������ۿ۰ٷֱȣ�0-100��0��ʾ�Ƴ��ۿۣ��� ");
        if (discountPercent < 0 || discountPercent > 100) std::cout << "�ۿ۱�����0��100֮�䡣\n";
    }

    // ʹ���µ�ProductManager����
    pm.applyCategoryDiscount(merchant->getUsername(), categoryToDiscount, discountPercent);
    // pm.applyCategoryDiscount��������ӡ��Ϣ��������ġ�
    // �˴��������pm.persistChanges()����ΪapplyCategoryDiscount�ѵ���saveProductsToFile��
}


// --- �˵����� ---
void showMainMenu(UserManager& um, ProductManager& pm, User*& currentUser) {
    std::string choiceStr;
    int choice = -1;

    while (true) {
        std::cout << "\n========= ��������ƽ̨ =========" << std::endl;
        if (currentUser) {
            std::cout << "��ǰ��¼��" << currentUser->getUsername()
                << " (" << currentUser->getUserType()
                << ") | ��$" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
        }
        else {
            std::cout << "��ӭ���ÿͣ�" << std::endl;
        }
        std::cout << "---------------------------------------" << std::endl;
        std::cout << "1. ��ʾ������Ʒ" << std::endl;
        std::cout << "2. ������Ʒ" << std::endl;
        if (!currentUser) {
            std::cout << "3. ע��" << std::endl;
            std::cout << "4. ��¼" << std::endl;
        }
        else {
            std::cout << "3. �ҵ��˻�ѡ��" << std::endl;
            std::cout << "4. �˳���¼" << std::endl;
        }
        std::cout << "0. �˳�" << std::endl;
        std::cout << "---------------------------------------" << std::endl;

        choiceStr = getLineInput("��ѡ��");
        try {
            choice = std::stoi(choiceStr);
        }
        catch (const std::exception& e) {
            choice = -1;
        }

        switch (choice) {
        case 1: handleDisplayAllProducts(pm); break;
        case 2: handleSearchProducts(pm); break;
        case 3:
            if (!currentUser) handleRegister(um);
            else {
                if (currentUser->getUserType() == "Consumer") {
                    showConsumerMenu(um, pm, currentUser);
                }
                else if (currentUser->getUserType() == "Merchant") {
                    showMerchantMenu(um, pm, currentUser);
                }
            }
            break;
        case 4:
            if (!currentUser) handleLogin(um, currentUser);
            else handleLogout(currentUser, um);
            break;
        case 0:
            std::cout << "�����˳�ƽ̨���ټ���" << std::endl;
            if (currentUser) um.persistChanges();
            pm.persistChanges();
            return;
        default:
            std::cout << "ѡ����Ч�������ԡ�" << std::endl;
        }
    }
}

void showConsumerMenu(UserManager& um, ProductManager& pm, User*& currentUser) {
    std::string choiceStr;
    int choice = -1;

    while (currentUser && currentUser->getUserType() == "Consumer") {
        std::cout << "\n--- �����߲˵� (" << currentUser->getUsername() << ") ---" << std::endl;
        std::cout << "��$" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
        std::cout << "1. �鿴������Ʒ" << std::endl;
        std::cout << "2. ������Ʒ" << std::endl;
        // std::cout << "3. ������Ʒ" << std::endl; // ���Ƴ�
        std::cout << "3. �������" << std::endl;     // ���±��
        std::cout << "4. �޸�����" << std::endl;    // ���±��
        std::cout << "0. �������˵����˳���¼��" << std::endl;

        choiceStr = getLineInput("��ѡ��");
        try {
            choice = std::stoi(choiceStr);
        }
        catch (const std::exception& e) {
            choice = -1;
        }

        switch (choice) {
        case 1: handleDisplayAllProducts(pm); break;
        case 2: handleSearchProducts(pm); break;
            // case 3: handlePurchaseProduct(um, pm, currentUser); break; // ���Ƴ�
        case 3: handleBalanceManagement(um, currentUser); break; // ��Ϊcase 3
        case 4: handleChangePassword(um, currentUser); break;    // ��Ϊcase 4
        case 0:
            handleLogout(currentUser, um);
            return;
        default:
            std::cout << "ѡ����Ч�������ԡ�" << std::endl;
        }
    }
    if (!currentUser) {
        return;
    }
}

void showMerchantMenu(UserManager& um, ProductManager& pm, User*& currentUser) {
    std::string choiceStr;
    int choice = -1;

    while (currentUser && currentUser->getUserType() == "Merchant") {
        std::cout << "\n--- �̼Ҳ˵� (" << currentUser->getUsername() << ") ---" << std::endl;
        std::cout << "��$" << std::fixed << std::setprecision(2) << currentUser->getBalance() << std::endl;
        std::cout << "1. �������Ʒ" << std::endl;
        std::cout << "2. �����ҵ���Ʒ" << std::endl;
        std::cout << "3. ���������/�����ۿۣ�����ҵ���Ʒ��" << std::endl; // �ѳ�������
        std::cout << "4. �鿴����ƽ̨��Ʒ" << std::endl;
        std::cout << "5. ����ƽ̨��Ʒ" << std::endl;
        std::cout << "6. �������" << std::endl;
        std::cout << "7. �޸�����" << std::endl;
        std::cout << "0. �������˵����˳���¼��" << std::endl;

        choiceStr = getLineInput("��ѡ��");
        try {
            choice = std::stoi(choiceStr);
        }
        catch (const std::exception& e) {
            choice = -1;
        }

        switch (choice) {
        case 1: handleAddProduct(pm, currentUser); break;
        case 2: handleManageMyProducts(pm, currentUser); break;
        case 3: handleDiscountCategory(pm, currentUser); break;
        case 4: handleDisplayAllProducts(pm); break;
        case 5: handleSearchProducts(pm); break;
        case 6: handleBalanceManagement(um, currentUser); break;
        case 7: handleChangePassword(um, currentUser); break;
        case 0:
            handleLogout(currentUser, um);
            return;
        default:
            std::cout << "ѡ����Ч�������ԡ�" << std::endl;
        }
    }
    if (!currentUser) {
        return;
    }
}


// --- ������ ---
int main() {
    UserManager userManager;
    ProductManager productManager;

    User* currentUser = nullptr;

    showMainMenu(userManager, productManager, currentUser);

    // �ڴ����˵��˳��͵ǳ�ʱ�����PersistChanges��
    return 0;
}

// ȷ��split�����Ѷ��塣�������UserManager.cpp���Ҳ��ڹ���ͷ�ļ��У�
// ������Ҫ�ڴ˴����¶����������ʵ�ù����ļ�����������
// Ϊ����������������UserManager.cpp�У�������Ҫ�ڴ˴������������ƶ��䶨�塣
// ���������ã����磬���UserManager.cpp�ѱ��벢���ӣ���'split'�����ڷǾ�̬ʱ���ã���
// Ϊ��ȫ����������ڵ�����util.h��util.cpp�ж�����������main��ʹ���Ϊ��̬���֡�
// ������Ŀ�ṹ����ProductManager.cpp��ʹ��'extern std::vector<std::string> split(...);'
// ������UserManager.cpp�������ڼ��ṩ�����ǳ���������