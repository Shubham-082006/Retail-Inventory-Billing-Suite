#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>

using namespace std;

class Item {
protected:
    int id;
    string name;
    double price;
    int quantity;

public:
    Item(int i, string n, double p, int q) : id(i), name(n), price(p), quantity(q) {}
    virtual ~Item() {} 

    int getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }

    void reduceQuantity(int amt) { quantity -= amt; }
    void addQuantity(int amt) { quantity += amt; }

    virtual string getType() const { return "General"; }
    
    virtual void display() const {
        cout << left << setw(5) << id 
             << setw(20) << name 
             << setw(10) << fixed << setprecision(2) << price 
             << setw(10) << quantity 
             << setw(15) << getType() << endl;
    }

    virtual string toFileData() const {
        return to_string(id) + "," + name + "," + to_string(price) + "," + to_string(quantity) + ",General,N/A";
    }
};

class PerishableItem : public Item {
private:
    string expiryDate;

public:
    PerishableItem(int i, string n, double p, int q, string exp) 
        : Item(i, n, p, q), expiryDate(exp) {}

    string getType() const override { return "Perishable"; }

    void display() const override {
        cout << left << setw(5) << id 
             << setw(20) << name 
             << setw(10) << fixed << setprecision(2) << price 
             << setw(10) << quantity 
             << setw(15) << getType() 
             << " (Exp: " << expiryDate << ")" << endl;
    }

    string toFileData() const override {
        return to_string(id) + "," + name + "," + to_string(price) + "," + to_string(quantity) + ",Perishable," + expiryDate;
    }
};

class ElectronicsItem : public Item {
private:
    int warrantyMonths;

public:
    ElectronicsItem(int i, string n, double p, int q, int warranty) 
        : Item(i, n, p, q), warrantyMonths(warranty) {}

    string getType() const override { return "Electronics"; }

    void display() const override {
        cout << left << setw(5) << id 
             << setw(20) << name 
             << setw(10) << fixed << setprecision(2) << price 
             << setw(10) << quantity 
             << setw(15) << getType() 
             << " (" << warrantyMonths << "M Warranty)" << endl;
    }

    string toFileData() const override {
        return to_string(id) + "," + name + "," + to_string(price) + "," + to_string(quantity) + ",Electronics," + to_string(warrantyMonths);
    }
};

class InventoryManager {
private:
    vector<Item*> inventory;
    const string fileName = "inventory_db.txt";

    void clearInventoryMemory() {
        for (Item* item : inventory) {
            delete item;
        }
        inventory.clear();
    }

public:
    InventoryManager() { loadFromFile(); }
    ~InventoryManager() { clearInventoryMemory(); }

    void saveToFile() {
        ofstream outFile(fileName);
        if (!outFile) return;
        for (const Item* item : inventory) {
            outFile << item->toFileData() << "\n";
        }
        outFile.close();
    }

    void loadFromFile() {
        clearInventoryMemory();
        ifstream inFile(fileName);
        if (!inFile) return;

        string line;
        while (getline(inFile, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string idStr, name, priceStr, qtyStr, type, extraAttr;

            getline(ss, idStr, ',');
            getline(ss, name, ',');
            getline(ss, priceStr, ',');
            getline(ss, qtyStr, ',');
            getline(ss, type, ',');
            getline(ss, extraAttr, ',');

            int id = stoi(idStr);
            double price = stod(priceStr);
            int qty = stoi(qtyStr);

            if (type == "Perishable") {
                inventory.push_back(new PerishableItem(id, name, price, qty, extraAttr));
            } else if (type == "Electronics") {
                inventory.push_back(new ElectronicsItem(id, name, price, qty, stoi(extraAttr)));
            } else {
                inventory.push_back(new Item(id, name, price, qty));
            }
        }
        inFile.close();
    }

    void addItem() {
        int id, qty, choice;
        string name;
        double price;

        cout << "\n--- Add New Product ---\n";
        cout << "Enter Item ID: "; cin >> id;
        
        for (const auto* item : inventory) {
            if (item->getId() == id) {
                cout << "Error: Item ID already exists!\n";
                return;
            }
        }
        
        cin.ignore();
        cout << "Enter Item Name: "; getline(cin, name);
        cout << "Enter Price: "; cin >> price;
        cout << "Enter Quantity: "; cin >> qty;
        cout << "Select Category (1: General, 2: Perishable, 3: Electronics): "; cin >> choice;

        if (choice == 2) {
            string expDate;
            cout << "Enter Expiry Date (DD-MM-YYYY): "; cin >> expDate;
            inventory.push_back(new PerishableItem(id, name, price, qty, expDate));
        } else if (choice == 3) {
            int warranty;
            cout << "Enter Warranty Period (in months): "; cin >> warranty;
            inventory.push_back(new ElectronicsItem(id, name, price, qty, warranty));
        } else {
            inventory.push_back(new Item(id, name, price, qty));
        }
        
        saveToFile();
        cout << "Product successfully added to system database!\n";
    }

    void viewInventory() const {
        if (inventory.empty()) {
            cout << "\nThe system inventory store is currently empty.\n";
            return;
        }
        cout << "\n====================================================================\n";
        cout << left << setw(5) << "ID" << setw(20) << "Name" << setw(10) << "Price" << setw(10) << "Qty" << setw(15) << "Category" << "Extra Specs\n";
        cout << "====================================================================\n";
        for (const auto* item : inventory) {
            item->display();
        }
        cout << "====================================================================\n";
    }

    void generateBill() {
        if (inventory.empty()) {
            cout << "\nInventory is empty. Cannot generate a bill.\n";
            return;
        }

        int id, qty;
        double totalBill = 0.0;
        vector<pair<Item*, int>> checkoutCart;

        cout << "\n--- Instant Checkout Billing Mode ---\n";
        while (true) {
            cout << "Enter Item ID to purchase (-1 to finalize order): "; cin >> id;
            if (id == -1) break;

            Item* targetItem = nullptr;
            for (auto* item : inventory) {
                if (item->getId() == id) {
                    targetItem = item;
                    break;
                }
            }

            if (!targetItem) {
                cout << "Product code not registered. Try again.\n";
                continue;
            }

            cout << "Available Stock: " << targetItem->getQuantity() << ". Enter units to buy: "; cin >> qty;
            if (qty > targetItem->getQuantity() || qty <= 0) {
                cout << "Invalid quantity amount requested.\n";
                continue;
            }

            targetItem->reduceQuantity(qty);
            checkoutCart.push_back({targetItem, qty});
        }

        if (checkoutCart.empty()) {
            cout << "Cart is empty. Order cancelled.\n";
            return;
        }

        cout << "\n=================== CUSTOMER INVOICE ===================\n";
        cout << left << setw(15) << "Item Name" << setw(10) << "Unit Price" << setw(10) << "Qty Sold" << "Subtotal\n";
        cout << "--------------------------------------------------------\n";
        for (const auto& entry : checkoutCart) {
            double cost = entry.first->getPrice() * entry.second;
            totalBill += cost;
            cout << left << setw(15) << entry.first->getName()
                 << setw(10) << entry.first->getPrice()
                 << setw(10) << entry.second
                 << cost << "\n";
        }
        cout << "--------------------------------------------------------\n";
        cout << "GRAND TOTAL TO PAY: INR " << fixed << setprecision(2) << totalBill << "\n";
        cout << "========================================================\n";

        saveToFile();
    }
};

int main() {
    InventoryManager appStore;
    int userChoice;

    while (true) {
        cout << "\n====================================\n";
        cout << "  RETAIL INVENTORY & BILLING SUITE  \n";
        cout << "====================================\n";
        cout << "1. View Complete Stock Inventory\n";
        cout << "2. Add New Product Line\n";
        cout << "3. Open Customer Counter (New Bill)\n";
        cout << "4. Terminate Program Engine\n";
        cout << "Select option: "; 
        cin >> userChoice;

        switch (userChoice) {
            case 1: appStore.viewInventory(); break;
            case 2: appStore.addItem(); break;
            case 3: appStore.generateBill(); break;
            case 4: 
                cout << "\nSaving databases... Shutting down securely.\n"; 
                return 0;
            default: cout << "Invalid Input. Try again.\n";
        }
    }
}
