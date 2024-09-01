#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>

using namespace std;

class Product {
public:
    int id;
    string name;
    string category;
    double price;
    int stock;

    // Default constructor
    Product() : id(0), name(""), category(""), price(0.0), stock(0) {}

    // Parameterized constructor
    Product(int id, const string& name, const string& category, double price, int stock) {
        this->id = id;
        this->name = name;
        this->category = category;
        this->price = price;
        this->stock = stock;
    }
};

class Order {
public:
    int orderId;
    string username;
    vector<string> productNames;
    double totalPrice;

    // Constructor with initialization list
    Order(int orderId, const string& username, const vector<string>& productNames, double totalPrice) 
        : orderId(orderId), username(username), productNames(productNames), totalPrice(totalPrice) {}
};

unordered_map<int, Product> productCatalogById;
unordered_map<string, Product> productCatalogByName;
vector<Order> orders;
int orderCounter = 1;

// Print header with title
void printHeader(const string& title) {
    cout << string(40, '=') << endl;
    cout << title << endl;
    cout << string(40, '=') << endl;
}

// Print title at the start of the program
void printTitle() {
    cout << string(40, '=') << endl;
    cout << "  Welcome to the Simple E-commerce System" << endl;
    cout << string(40, '=') << endl;
}

// Load products from file
void loadProducts() {
    ifstream inFile("products.excel");
    if (!inFile) {
        cerr << "Error opening products file!" << endl;
        return;
    }

    int id, stock;
    double price;
    string name, category, line;

    while (getline(inFile, line)) {
        stringstream ss(line);
        string idStr, priceStr, stockStr;

        // Parse each line
        getline(ss, idStr, ',');
        getline(ss, name, ',');
        getline(ss, category, ',');
        getline(ss, priceStr, ',');
        getline(ss, stockStr);

        // Convert to appropriate types
        try {
            id = stoi(idStr);
            price = stod(priceStr);
            stock = stoi(stockStr);

            Product p(id, name, category, price, stock);
            productCatalogById[id] = p;
            productCatalogByName[name] = p;
        } catch (const exception& e) {
            cerr << "Error parsing product data: " << e.what() << endl;
            cerr << "Line: " << line << endl;
        }
    }
}

// Save products to file
void saveProducts() {
    ofstream outFile("products.txt");
    if (!outFile) {
        cerr << "Error opening products file for writing!" << endl;
        return;
    }

    for (const auto& p : productCatalogById) {
        outFile << p.second.id << "," << p.second.name << "," << p.second.category << "," << p.second.price << "," << p.second.stock << "\n";
    }
}

// Add a new product
void addProduct() {
    printHeader("Add Product");

    int id, stock;
    double price;
    string name, category;

    cout << "Enter ID: ";
    while (!(cin >> id)) {
        cin.clear(); // Clear the error flag
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
        cout << "Invalid input. Please enter a numeric ID: ";
    }
    cin.ignore(); // Ignore leftover newline character

    cout << "Enter Name: ";
    cin.ignore(); // Ignore leftover newline character
    getline(cin, name);

    cout << "Enter Category: ";
    getline(cin, category);

    cout << "Enter Price: ";
    while (!(cin >> price) || price < 0) {
        cin.clear(); // Clear the error flag
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
        cout << "Invalid input. Please enter a positive numeric price: ";
    }

    cout << "Enter Stock: ";
    while (!(cin >> stock) || stock < 0) {
        cin.clear(); // Clear the error flag
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
        cout << "Invalid input. Please enter a non-negative numeric stock quantity: ";
    }

    Product p(id, name, category, price, stock);
    productCatalogById[id] = p;
    productCatalogByName[name] = p;

    cout << "Product added successfully!" << endl;
}

// Add a product to the cart with specified quantity
void addToCart(vector<pair<string, int>>& cart) {  // Update cart to hold pairs of product names and quantities
    printHeader("Add to Cart");

    string name;
    int quantity;

    cout << "Enter product name: ";
    cin.ignore(); // Ignore leftover newline character
    getline(cin, name);

    if (productCatalogByName.count(name)) {
        cout << "Enter quantity: ";
        while (!(cin >> quantity) || quantity <= 0) {
            cin.clear(); // Clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
            cout << "Invalid input. Please enter a positive numeric quantity: ";
        }

        Product& product = productCatalogByName[name];
        if (product.stock >= quantity) {
            cart.push_back(make_pair(name, quantity));  // Store both name and quantity in the cart
            cout << "Added " << quantity << " of " << name << " to cart." << endl;  // Confirm addition to the cart
        } else {
            cout << "Sorry, only " << product.stock << " units of " << name << " are available in stock." << endl;
        }
    } else {
        cout << "Product not found!" << endl;
    }
}

// Checkout and clear the cart
void checkout(vector<pair<string, int>>& cart, const string& username) {  // Update cart to hold pairs of product names and quantities
    if (cart.empty()) {
        cout << "Your cart is empty! Add products to the cart before checking out." << endl;
        return;
    }

    printHeader("Checkout");
    double total = 0;
    vector<string> productsToRemove;

    for (const auto& item : cart) {
        const string& name = item.first;
        int quantity = item.second;

        if (productCatalogByName.count(name)) {
            Product& product = productCatalogByName[name];
            if (product.stock >= quantity) {
                total += product.price * quantity;
                product.stock -= quantity;
                productsToRemove.push_back(name);
            } else {
                cout << "Sorry, only " << product.stock << " units of " << name << " are available. Unable to fulfill order for " << quantity << " units." << endl;
            }
        } else {
            cout << "Product " << name << " not found in the catalog!" << endl;
        }
    }

    if (!productsToRemove.empty()) {
        orders.push_back(Order(orderCounter++, username, productsToRemove, total));
    }

    cart.clear();
    cout << "Checkout complete. Total: $" << total << endl;
}

// Display the main menu
void mainMenu() {
    printHeader("Main Menu");
    cout << "1. Add Product\n2. Add to Cart\n3. Checkout\n0. Exit\n";
    cout << string(20, '-') << endl;
    cout << "Enter your choice: ";
}

// Main function to run the program
int main() {
    printTitle();
    // Load initial data
    loadProducts();

    vector<pair<string, int>> cart;  // Updated cart to hold pairs of product names and quantities
    string currentUser = "Guest";  // Simplified for this example
    int choice;

    do {
        mainMenu();
        while (!(cin >> choice) || choice < 0 || choice > 3) {
            cin.clear(); // Clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
            cout << "Invalid choice. Please enter a number between 0 and 3: ";
        }
        cin.ignore(); // Ignore leftover newline character

        switch (choice) {
            case 1:
                addProduct();
                saveProducts();
                break;
            case 2:
                addToCart(cart);  // Pass the updated cart
                break;
            case 3:
                checkout(cart, currentUser);  // Pass the updated cart
                saveProducts();
                break;
            case 0:
                saveProducts();
                cout << "Exiting program." << endl;
                break;
        }
    } while (choice != 0);

    return 0;
}
