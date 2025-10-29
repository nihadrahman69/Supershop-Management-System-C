#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <time.h>

#define Max_Password_Length 50
#define Max_Name_Length 100
#define Max_Cart_Items 100
#define Max_Transaction_History 100
#define User_File "user_data.txt"
#define Inventory_File "inventory.txt"
#define Transaction_File "transaction.txt"
#define VAT_RATE 0.15

#define GREEN_TEXT "\033[0;32m"
#define RESET_TEXT "\033[0m"
#define RED_TEXT "\033[31m"

int loggedIn = 0;
char currentUsername[Max_Name_Length];
int cartItemCount = 0;
int isAdmin = 0;

typedef struct {
    char product_number[20];
    char item_name[Max_Name_Length];
    float price;
    int quantity;
} Product;

typedef struct {
    Product product;
    int quantity;
} CartItem;

typedef struct {
    char transaction_code[20];
    CartItem cart[Max_Cart_Items];
    int cart_size;
    float total_amount;
    time_t transaction_time;
} Transaction;

void createNewUser();
int logInUser();
void logOutUser();
void getMaskedPassword(char *password);
void saveInventory();
void displayInventory(Product inventory[], int inventory_size);
int addtocart(CartItem cart[], int *cart_size, Product inventory[], int inventory_size, char *product_number, int quantity);
int removeItemFromCart(CartItem cart[], int *cart_size, Product inventory[], int inventory_size, char *product_number);
void generateTransactionCode(char *transaction_code);
float calculateTotalAmount(CartItem cart[], int cart_size);
void refund(const char *filename, Product inventory[], int inventory_size, char *transaction_code);
void addStock(Product inventory[], int *inventory_size);
void createSalesReport(Product inventory[], int inventory_size);
int loadInventory(const char *filename, Product inventory[]);
void viewCart(CartItem cart[], int cart_size);
void checkout(CartItem cart[], int *cart_size, Product inventory[], int inventory_size, Transaction transactions[], int *transaction_count, const char *currentUsername);
void checkStockLevel(Product inventory[], int inventory_size);
void checkPurchaseHistory();
void clearScreen();
void displayHeader();
void printAsciiLogo();

int main() {
    Product inventory[100];
    CartItem cart[Max_Cart_Items];
    Transaction transactions[Max_Transaction_History];
    int inventory_size = 0;
    int cart_size = 0;
    int transaction_count = 0;

    inventory_size = loadInventory("inventory.txt", inventory);

    char choice;
    clearScreen();
    printAsciiLogo();

    printf(GREEN_TEXT "Welcome to the Supershop Management System!\n" RESET_TEXT);

    while(1) {
        if (loggedIn) {
            printf(GREEN_TEXT "\nYou are logged in as %s.\n" RESET_TEXT, currentUsername);

            if (isAdmin) {
                printf(GREEN_TEXT "A. View Inventory\nB. Add Stock\nC. Check Stock Level\nD. View Purchase History\nE. Create Sales Report\nF. Logout (sign out)\nG. Exit\n" RESET_TEXT);
            } else {
                printf(GREEN_TEXT "1. View Product Categories\n2. Add product to cart\n3. View Cart\n4. Checkout\n5. Request Refund\n6. Logout \n7. About Us\n8. Exit\n" RESET_TEXT);
            }
        } else {
            printf(GREEN_TEXT "\nWhat would you like to do?\n1. Register (create a new account)\n2. Login (sign in)\n3. About Us\n4. Exit\n" RESET_TEXT);
        }

        printf(GREEN_TEXT "Enter your choice: " RESET_TEXT);
        scanf(" %c", &choice);
        choice = toupper(choice);

        clearScreen();
        displayHeader(); 

        if (choice == '1' && !loggedIn) {
            createNewUser();
        } else if (choice == '2' && !loggedIn) {
            if (logInUser()) {
                printf(GREEN_TEXT "Login successful! Welcome, %s!\n" RESET_TEXT, currentUsername);
            } else {
                printf(GREEN_TEXT "Incorrect username or password. Please try again.\n" RESET_TEXT);
            }
        } else if (choice == '1' && loggedIn) {
            displayInventory(inventory, inventory_size);
        }else if ((choice == '3' && !loggedIn) || (choice == '7' && loggedIn)) {
            printf(GREEN_TEXT "\t******************************************************\n" RESET_TEXT);
            printf(GREEN_TEXT "\t*                                                    *\n" RESET_TEXT);
            printf(GREEN_TEXT "\t*               Welcome to Our Website!              *\n" RESET_TEXT);
            printf(GREEN_TEXT "\t*                                                    *\n" RESET_TEXT);
            printf(GREEN_TEXT "\t*      We are a team of passionate individuals       *\n" RESET_TEXT);
            printf(GREEN_TEXT "\t*         dedicated to delivering excellence.        *\n" RESET_TEXT);
            printf(GREEN_TEXT "\t*    Our mission is to provide top-notch solutions   *\n" RESET_TEXT);
            printf(GREEN_TEXT "\t*              tailored to your needs.               *\n" RESET_TEXT);
            printf(GREEN_TEXT "\t*   Innovation, quality, and customer satisfaction   *\n" RESET_TEXT);
            printf(GREEN_TEXT "\t*              drive everything we do.               *\n" RESET_TEXT);
            printf(GREEN_TEXT "\t*    Thank you for being a part of our journey.      *\n" RESET_TEXT);
            printf(GREEN_TEXT "\t*       Let's build something great together!        *\n" RESET_TEXT);
            printf(GREEN_TEXT "\t*                                                    *\n" RESET_TEXT);
            printf(GREEN_TEXT "\t******************************************************\n" RESET_TEXT);

            printf(GREEN_TEXT "\n\t******************************************************\n" RESET_TEXT);
            printf(GREEN_TEXT "\t*               --- Developer Team ---               *\n" RESET_TEXT);
            printf(GREEN_TEXT "\t*                                                    *\n" RESET_TEXT);
            printf(GREEN_TEXT "\t*     1. Jayanto Sarker (ID: 232-35-561)             *\n" RESET_TEXT);
            printf(GREEN_TEXT "\t*     2. Nafisa Hasan (ID: 232-35-564)               *\n" RESET_TEXT);
            printf(GREEN_TEXT "\t*     3. Busra Ahammad Jui (ID: 232-35-625)          *\n" RESET_TEXT);
            printf(GREEN_TEXT "\t*     4. Nihad Rahman Rawdra (ID: 232-35-520)        *\n" RESET_TEXT);
            printf(GREEN_TEXT "\t*                                                    *\n" RESET_TEXT);
            printf(GREEN_TEXT "\t******************************************************\n" RESET_TEXT);


        }else if (choice == '2' && loggedIn) {
            char product_number[20];
            int quantity;

            displayInventory(inventory, inventory_size);
            printf(GREEN_TEXT "Enter the product number to add to cart: " RESET_TEXT);
            scanf("%s", product_number);

            printf(GREEN_TEXT "Enter the quantity: " RESET_TEXT);
            scanf("%d", &quantity);

            if (addtocart(cart, &cart_size, inventory, inventory_size, product_number, quantity)) {
                printf(GREEN_TEXT "Item successfully added to the cart.\n" RESET_TEXT);
            } else {
                printf(GREEN_TEXT "Failed to add item to the cart.\n" RESET_TEXT);
            }
        } else if (choice == '3' && loggedIn) {
            viewCart(cart, cart_size);
        } else if (choice == '4' && loggedIn) {
            checkout(cart, &cart_size, inventory, inventory_size, transactions, &transaction_count, currentUsername);
        } else if (choice == '5' && loggedIn) {
            char transaction_code[20]; 
            printf(GREEN_TEXT "Enter the transaction code for the refund: " RESET_TEXT);
            scanf("%s", transaction_code);
            refund("transaction.txt", inventory, inventory_size, transaction_code);
        } else if ((choice == '6' || choice == 'F' || choice == 'f') && loggedIn) {
            logOutUser();
        } else if (choice == '8' || choice == 'G' || (choice == '4' && !loggedIn)) {
            printf(GREEN_TEXT "Goodbye!\n" RESET_TEXT);
            break;
        } else if (choice == 'A' && loggedIn && isAdmin) {
            displayInventory(inventory, inventory_size);
        } else if (choice == 'B' && loggedIn && isAdmin) {
            addStock(inventory, &inventory_size);
        } else if (choice == 'C' && loggedIn && isAdmin) {
            checkStockLevel(inventory, inventory_size);
        } else if (choice == 'D' && loggedIn && isAdmin) {
            checkPurchaseHistory("product_number");
        } else if (choice == 'E' && loggedIn && isAdmin) {
            createSalesReport(inventory, inventory_size);
        } else {
            printf(GREEN_TEXT "Invalid option. Please try again.\n" RESET_TEXT);
        }
    }

    return 0;
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}


void displayHeader() {
    printf("\n");
    printf(RED_TEXT"\t\t\t#######################\n");
    printf(RED_TEXT"\t\t\t##                   ##\n");
    printf(RED_TEXT"\t\t\t##      NJ MART      ##\n");
    printf(RED_TEXT"\t\t\t##                   ##\n");
    printf(RED_TEXT"\t\t\t#######################\n");
    printf("\n");
}

void printAsciiLogo() {
    printf(RED_TEXT"\t\t\t.__   __.        __     .___  ___.      ___      .______      .___________.\n");
    printf(RED_TEXT"\t\t\t|  \\ |  |       |  |    |   \\/   |     /   \\     |   _  \\     |           |\n");
    printf(RED_TEXT"\t\t\t|   \\|  |       |  |    |  \\  /  |    /  ^  \\    |  |_)  |    `---|  |----`\n");
    printf(RED_TEXT"\t\t\t|  . `  | .--.  |  |    |  |\\/|  |   /  /_\\  \\   |      /         |  |     \n");
    printf(RED_TEXT"\t\t\t|  |\\   | |  `--'  |    |  |  |  |  /  _____  \\  |  |\\  \\----.    |  |     \n");
    printf(RED_TEXT"\t\t\t|__| \\__|  \\______/     |__|  |__| /__/     \\__\\ | _| `._____|    |__|     \n");
    printf("\n\n\n");
}  

void createNewUser() {
    FILE *file = fopen(User_File, "a");
    if (file == NULL) {
        printf(GREEN_TEXT "Error opening file.\n" RESET_TEXT);
        return;
    }

    char newUsername[Max_Name_Length];
    char newPassword[Max_Password_Length];

    printf(GREEN_TEXT "Enter a new username: " RESET_TEXT);
    scanf("%s", newUsername);
    getchar();
    printf(GREEN_TEXT "Enter a new password: " RESET_TEXT);
    getMaskedPassword(newPassword);

    clearScreen();
    displayHeader(); 

    fprintf(file, "%s %s\n", newUsername, newPassword);
    fclose(file);
    printf(GREEN_TEXT "\nRegistration successful!\n" RESET_TEXT);
    
}

int logInUser() {
    FILE *file = fopen(User_File, "r");
    if (file == NULL) {
        printf(GREEN_TEXT "Error opening file. Please register first.\n" RESET_TEXT);
        return 0;
    }

    char enteredUsername[Max_Name_Length];
    char enteredPassword[Max_Password_Length];
    char storedUsername[Max_Name_Length];
    char storedPassword[Max_Password_Length];

    printf(GREEN_TEXT "Enter your username: " RESET_TEXT);
    scanf("%s", enteredUsername);
    getchar();
    printf(GREEN_TEXT "Enter your password: " RESET_TEXT);
    getMaskedPassword(enteredPassword);

    clearScreen();
    displayHeader(); 

    if (strcmp(enteredUsername, "admin") == 0 && strcmp(enteredPassword, "admin") == 0) {
        isAdmin = 1; 
        strcpy(currentUsername, enteredUsername);
        loggedIn = 1;
        fclose(file);
        return 1;
    }

    while (fscanf(file, "%s %s", storedUsername, storedPassword) != EOF) {
        if (strcmp(enteredUsername, storedUsername) == 0 && strcmp(enteredPassword, storedPassword) == 0) {
            loggedIn = 1;
            strcpy(currentUsername, enteredUsername);
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

void logOutUser() {
    clearScreen();
    displayHeader(); 
    printf(GREEN_TEXT "Logging out %s...\n" RESET_TEXT, currentUsername);
    loggedIn = 0;
    currentUsername[0] = '\0';
    cartItemCount = 0;
}

void getMaskedPassword(char *password) {
    int index = 0;
    char character;

    while ((character = getch()) != '\n' && character != '\r') {
        if (character == '\b' && index > 0) {
            printf("\b \b");
            index--;
        } else if (character != '\b' && index < Max_Password_Length - 1) {
            password[index++] = character;
            printf("*");
        }
    }
    password[index] = '\0';
    printf("\n");
}

int loadInventory(const char *filename, Product inventory[]) {
    FILE *file = fopen(filename, "r");
    int i = 0;

    if (file == NULL) {
        printf(GREEN_TEXT"Error opening file. Returning empty inventory.\n"RESET_TEXT);
        return 0;
    }

    while (fscanf(file, "%19[^,], %99[^,], %f, %d\n", inventory[i].product_number, inventory[i].item_name,
                  &inventory[i].price, &inventory[i].quantity) == 4) {
        i++;
    }

    fclose(file);
    return i;
}

void saveInventory(const char *filename, Product inventory[], int size) {
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        printf(GREEN_TEXT"Error opening file for writing.\n"RESET_TEXT);
        return;
    }

    for (int i = 0; i < size; i++) {
        fprintf(file, "%s, %s, %.2f, %d\n", inventory[i].product_number, inventory[i].item_name,
                inventory[i].price, inventory[i].quantity);
    }

    fclose(file);
}

void displayInventory(Product inventory[], int size) {
    printf(GREEN_TEXT"-----------------------------------------------------------\n"RESET_TEXT);
    printf(GREEN_TEXT"| Product Number | Item Name           | Price  | Quantity |\n"RESET_TEXT);
    printf(GREEN_TEXT"-----------------------------------------------------------\n"RESET_TEXT);

    for (int i = 0; i < size; i++) {
        printf("| %-15s | %-18s | %-6.2f | %-8d |\n", inventory[i].product_number, inventory[i].item_name,
               inventory[i].price, inventory[i].quantity);
    }

    printf(GREEN_TEXT"-----------------------------------------------------------\n"RESET_TEXT);
}

int addtocart(CartItem cart[], int *cart_size, Product inventory[], int inventory_size, char *product_number, int quantity) {
    for (int i = 0; i < inventory_size; i++) {
        if (strcmp(inventory[i].product_number, product_number) == 0) {
            if (inventory[i].quantity >= quantity) {
                int product_found_in_cart = 0;
                for (int j = 0; j < *cart_size; j++) {
                    if (strcmp(cart[j].product.product_number, product_number) == 0) {
                        cart[j].quantity += quantity;  
                        product_found_in_cart = 1;
                        break;
                    }
                }

                if (!product_found_in_cart) {
                    cart[*cart_size].product = inventory[i];
                    cart[*cart_size].quantity = quantity;
                    (*cart_size)++;
                }

                inventory[i].quantity -= quantity;
                return 1; 
            } else {
                printf(GREEN_TEXT"Not enough stock available for this item.\n"RESET_TEXT);
                return 0;
            }
        }
    }

    printf(GREEN_TEXT"Product not found in inventory.\n"RESET_TEXT);
    return 0;
}

void viewCart(CartItem cart[], int cart_size) {
    if (cart_size == 0) {
        printf(GREEN_TEXT"Your cart is empty.\n"RESET_TEXT);
        return;
    }

    printf(GREEN_TEXT"-----------------------------------------------------------\n"RESET_TEXT);
    printf(GREEN_TEXT"| Product Number | Item Name           | Price  | Quantity |\n"RESET_TEXT);
    printf(GREEN_TEXT"-----------------------------------------------------------\n"RESET_TEXT);

    for (int i = 0; i < cart_size; i++) {
        printf("| %-15s | %-18s | %-6.2f | %-8d |\n", cart[i].product.product_number, cart[i].product.item_name,
               cart[i].product.price, cart[i].quantity);
    }

    printf(GREEN_TEXT"-----------------------------------------------------------\n"RESET_TEXT);
}

int removeItemFromCart(CartItem cart[], int *cart_size, Product inventory[], int inventory_size, char *product_number) {
    for (int i = 0; i < *cart_size; i++) {
        if (strcmp(cart[i].product.product_number, product_number) == 0) {
            for (int j = 0; j < inventory_size; j++) {
                if (strcmp(inventory[j].product_number, product_number) == 0) {
                    inventory[j].quantity += cart[i].quantity;
                    break;
                }
            }

            for (int k = i; k < *cart_size - 1; k++) {
                cart[k] = cart[k + 1];
            }

            (*cart_size)--;
            return 1;
        }
    }

    printf(GREEN_TEXT"Product not found in the cart.\n"RESET_TEXT);
    return 0;
}

void generateTransactionCode(char *transaction_code) {
    srand(time(NULL)); 
    snprintf(transaction_code, 20, "TXN%ld", time(NULL));  
}

float calculateTotalAmount(CartItem cart[], int cart_size) {
    float total = 0.0;
    for (int i = 0; i < cart_size; i++) {
        total += cart[i].product.price * cart[i].quantity;
    }
    return total;
}

void checkout(CartItem cart[], int *cart_size, Product inventory[], int inventory_size, Transaction transactions[], int *transaction_count, const char *currentUsername) {
    if (*cart_size == 0) {
        printf(GREEN_TEXT "Your cart is empty. Add items before checking out.\n" RESET_TEXT);
        return;
    }

    float total = 0;

    for (int i = 0; i < *cart_size; i++) {
        total += cart[i].product.price * cart[i].quantity;
    }
    
    float vatAmount = total * VAT_RATE;
    float finalTotal = total + vatAmount;

    char transaction_code[20];
    generateTransactionCode(transaction_code);

    FILE *file = fopen(Transaction_File, "a");
    if (file == NULL) {
        printf(RED_TEXT "Error opening transaction file.\n" RESET_TEXT);
        return;
    }

    fprintf(file, "%s,%s,%.2f,%ld\n", transaction_code, currentUsername, finalTotal, time(NULL));
    for (int i = 0; i < *cart_size; i++) {
        fprintf(file, "%s,%s,%.2f,%d\n", cart[i].product.product_number, cart[i].product.item_name, cart[i].product.price, cart[i].quantity);
    }
    fclose(file);
    
    int paymentMethod;
    printf(GREEN_TEXT "\nChoose Payment Method:\n" RESET_TEXT);
    printf(GREEN_TEXT "1. Cash\n" RESET_TEXT);
    printf(GREEN_TEXT "2. Credit Card\n" RESET_TEXT);
    printf(GREEN_TEXT "3. Mobile Banking (bKash/Nagad)\n" RESET_TEXT);
    printf(GREEN_TEXT "Enter payment option: " RESET_TEXT);
    scanf("%d", &paymentMethod);

    printf(GREEN_TEXT "\n--- Receipt ---\n" RESET_TEXT);
    printf(GREEN_TEXT "Customer: %s\n" RESET_TEXT, currentUsername);
    printf(GREEN_TEXT "Transaction code: %s\n" RESET_TEXT, transaction_code);
    printf(GREEN_TEXT "Items:\n" RESET_TEXT);
    for (int i = 0; i < *cart_size; i++) {
        printf(GREEN_TEXT "%d. %s - $%.2f x %d = $%.2f\n" RESET_TEXT,
            i+1, cart[i].product.item_name, cart[i].product.price,
            cart[i].quantity, cart[i].product.price * cart[i].quantity);
    }
    printf(GREEN_TEXT "Subtotal: $%.2f\n" RESET_TEXT, total);
    printf(GREEN_TEXT "VAT (15%%): $%.2f\n" RESET_TEXT, vatAmount);
    printf(GREEN_TEXT "Total Amount: $%.2f\n" RESET_TEXT, finalTotal);

    switch (paymentMethod) {
        case 1:
            printf(GREEN_TEXT "Payment Method: Cash\n" RESET_TEXT);
            break;
        case 2:
            printf(GREEN_TEXT "Payment Method: Credit Card\n" RESET_TEXT);
            break;
        case 3:
            printf(GREEN_TEXT "Payment Method: Mobile Banking (bKash/Nagad)\n" RESET_TEXT);
            break;
        default:
            printf(RED_TEXT "Invalid payment option. Please select a valid method.\n" RESET_TEXT);
            return;
    }

    *cart_size = 0;
}


void refund(const char *filename, Product inventory[], int inventory_size, char *transaction_code) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    Transaction transaction;
    char line[256];
    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "TXN", 3) == 0) {
            // Parse transaction header
            sscanf(line, "%[^,],%*[^,],%f,%*d", 
                   transaction.transaction_code, 
                   &transaction.total_amount);
            transaction.cart_size = 0;

            // Check if this is the transaction to refund
            if (strcmp(transaction.transaction_code, transaction_code) == 0) {
                found = 1;
                printf(GREEN_TEXT"Refunding transaction with code: %s\n"RESET_TEXT, transaction_code);

                // Read cart items for this transaction
                while (fgets(line, sizeof(line), file) && strncmp(line, "TXN", 3) != 0) {
                    CartItem *item = &transaction.cart[transaction.cart_size];
                    sscanf(line, "%[^,],%[^,],%f,%d", 
                           item->product.product_number, 
                           item->product.item_name, 
                           &item->product.price, 
                           &item->quantity);
                    transaction.cart_size++;
                }

                // Update inventory
                for (int i = 0; i < transaction.cart_size; i++) {
                    for (int j = 0; j < inventory_size; j++) {
                        if (strcmp(inventory[j].product_number, transaction.cart[i].product.product_number) == 0) {
                            inventory[j].quantity += transaction.cart[i].quantity;
                            break;
                        }
                    }
                }

                printf(GREEN_TEXT"Refund successful.\n"RESET_TEXT);
                fclose(file);
                return;
            }
        }
    }

    fclose(file);
    if (!found) {
        printf(GREEN_TEXT"Transaction code not found.\n"RESET_TEXT);
    }
}

void checkStockLevel(Product inventory[], int inventory_size) {
    char product_number[20]; 
    printf(GREEN_TEXT "Enter product number: " RESET_TEXT);
    scanf("%s", product_number);
    for (int i = 0; i < inventory_size; i++) {
        if (strcmp(inventory[i].product_number, product_number) == 0) {
            printf(GREEN_TEXT"Product %s - %s: Stock level is %d\n"RESET_TEXT, inventory[i].product_number, inventory[i].item_name, inventory[i].quantity);
            return;
        }
    }
    printf(GREEN_TEXT"Product %s not found in inventory.\n"RESET_TEXT, product_number);
}

void addStock(Product inventory[], int *inventory_size) {
    char product_number[20];
    int quantity;
    
    printf("Enter the product number: ");
    scanf("%s", product_number);
    printf("Enter the quantity to add: ");
    scanf("%d", &quantity);
    
    for (int i = 0; i < *inventory_size; i++) {
        if (strcmp(inventory[i].product_number, product_number) == 0) {
            inventory[i].quantity += quantity;
            printf("Stock updated. New quantity: %d\n", inventory[i].quantity);
            return;
        }
    }
    
    printf("Product not found in inventory.\n");
}

void checkPurchaseHistory() {
    char product_number[20];

    printf(GREEN_TEXT "Enter the product number to check purchase history: " RESET_TEXT);
    scanf("%19s", product_number); 

    printf(GREEN_TEXT "Purchase history for Product %s:\n" RESET_TEXT, product_number);
    printf(GREEN_TEXT "-----------------------------------------------------------\n" RESET_TEXT);
    printf(GREEN_TEXT "| Transaction Code | Item Name           | Quantity | Total Amount |\n" RESET_TEXT);
    printf(GREEN_TEXT "-----------------------------------------------------------\n" RESET_TEXT);

    FILE *file = fopen(Transaction_File, "r");
    if (file == NULL) {
        printf(RED_TEXT "Error opening transaction file.\n" RESET_TEXT);
        return;
    }

    char line[256];
    char current_transaction_code[20] = "";
    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "TXN", 3) == 0) {
            sscanf(line, "%19[^,]", current_transaction_code);
            continue; 
        }

        char productNum[20], itemName[Max_Name_Length];
        float price;
        int quantity;

        if (sscanf(line, "%19[^,],%99[^,],%f,%d", productNum, itemName, &price, &quantity) == 4) {
            if (strcmp(productNum, product_number) == 0) {
                printf(GREEN_TEXT "| %-17s | %-18s | %-8d | %-12.2f |\n" RESET_TEXT,
                       current_transaction_code, itemName, quantity, price * quantity);
                found = 1;
            }
        }
    }

    fclose(file);

    if (!found) {
        printf(GREEN_TEXT "No purchase history found for Product %s.\n" RESET_TEXT, product_number);
    }

    printf(GREEN_TEXT "-----------------------------------------------------------\n" RESET_TEXT);
}





void createSalesReport(Product inventory[], int inventory_size) {
    FILE *file = fopen(Transaction_File, "r");
    if (file == NULL) {
        printf(RED_TEXT "Error opening transaction file.\n" RESET_TEXT);
        return;
    }

    float sales_totals[inventory_size];
    int units_sold[inventory_size];
    memset(sales_totals, 0, sizeof(sales_totals));
    memset(units_sold, 0, sizeof(units_sold));

    char line[256];

    while (fgets(line, sizeof(line), file)) {
        if (strchr(line, ',') != NULL && strchr(line, 'T') == line) {
            continue;
        }

        char productNum[20], itemName[Max_Name_Length];
        float price;
        int quantity;

        if (sscanf(line, "%19[^,],%99[^,],%f,%d", productNum, itemName, &price, &quantity) == 4) {
            for (int i = 0; i < inventory_size; i++) {
                if (strcmp(productNum, inventory[i].product_number) == 0) {
                    sales_totals[i] += price * quantity;  
                    units_sold[i] += quantity;           
                    break;
                }
            }
        }
    }

    fclose(file);

    printf(GREEN_TEXT "\nSales Report:\n" RESET_TEXT);
    printf(GREEN_TEXT "-----------------------------------------------------------\n" RESET_TEXT);
    printf(GREEN_TEXT "| Product Number | Item Name           | Total Sales | Units Sold |\n" RESET_TEXT);
    printf(GREEN_TEXT "-----------------------------------------------------------\n" RESET_TEXT);

    for (int i = 0; i < inventory_size; i++) {
        if (sales_totals[i] > 0) {
            printf(GREEN_TEXT "| %-15s | %-18s | %-12.2f | %-10d |\n" RESET_TEXT,
                   inventory[i].product_number, inventory[i].item_name, sales_totals[i], units_sold[i]);
        }
    }

    printf(GREEN_TEXT "-----------------------------------------------------------\n" RESET_TEXT);
}

