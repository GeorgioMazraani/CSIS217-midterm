#include <iostream>
#include "ForestTree.h"

using namespace std;

// Function to display the menu
void displayMenu() {
    cout << "======================================" << endl;
    cout << " Lebanese Chart of Accounts - Menu    " << endl;
    cout << "======================================" << endl;
    cout << "1. Load accounts from the existing file" << endl;
    cout << "2. Add an account" << endl;
    cout << "3. Remove an account" << endl;
    cout << "4. Add a transaction" << endl;
    cout << "5. Remove a transaction" << endl;
    cout << "6. Search for an account" << endl;
    cout << "7. Print account details to a file" << endl;
    cout << "8. Print the entire tree to a file" << endl;
    cout << "9. Exit" << endl;
    cout << "======================================" << endl;
}

int main() {
    ForestTree forestTree;
    int choice;
    const string filename = "C://Users//User//OneDrive//Desktop//uob//5th semester 2024//ads//CSIS217-midterm//accountswithspace.txt"; // Fixed file name

    do {
        displayMenu();
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                // Load accounts from the fixed file
                forestTree.buildFromFile(filename);
                cout << "Accounts loaded successfully from " << filename << "." << endl;
                break;
            }
            case 2: {
                int accountNumber;
                string description;
                double initialBalance;
                cout << "Enter account number: ";
                cin >> accountNumber;
                cout << "Enter description: ";
                cin.ignore();
                getline(cin, description);
                cout << "Enter initial balance: ";
                cin >> initialBalance;
                forestTree.addAccount(accountNumber, description, initialBalance);
                cout << "Account added successfully." << endl;
                break;
            }
            case 3: {
                int accountNumber;
                cout << "Enter the account number to remove: ";
                cin >> accountNumber;
                forestTree.removeAccount(accountNumber);
                cout << "Account removed successfully." << endl;
                break;
            }
            case 4: {
                int accountNumber, transactionID;
                double amount;
                char type;
                string relatedAccount;
                cout << "Enter account number: ";
                cin >> accountNumber;
                cout << "Enter transaction ID: ";
                cin >> transactionID;
                cout << "Enter amount: ";
                cin >> amount;
                cout << "Enter type (D for Debit, C for Credit): ";
                cin >> type;
                Transaction transaction(transactionID, amount, type, "");
                forestTree.addTransaction(accountNumber, transaction);
                cout << "Transaction added successfully." << endl;
                break;
            }
            case 5: {
                int accountNumber, transactionID;
                cout << "Enter account number: ";
                cin >> accountNumber;
                cout << "Enter transaction ID: ";
                cin >> transactionID;
                forestTree.removeTransaction(accountNumber, transactionID);
                cout << "Transaction removed successfully." << endl;
                break;
            }
            case 6: {
                int accountNumber;
                cout << "Enter the account number to search for: ";
                cin >> accountNumber;
                Account* account = forestTree.searchAccount(accountNumber);
                if (account) {
                    cout << "Account found: " << endl;
                    cout << *account << endl;
                } else {
                    cout << "Account not found." << endl;
                }
                break;
            }
            case 7: {
                int accountNumber;
                string outputFilename;
                cout << "Enter the account number: ";
                cin >> accountNumber;
                cout << "Enter the output filename: ";
                cin >> outputFilename;
                forestTree.printAccountDetails(accountNumber, outputFilename);
                cout << "Account details printed to " << outputFilename << "." << endl;
                break;
            }
            case 8: {
                string outputFilename;
                cout << "Enter the filename to print the tree: ";
                cin >> outputFilename;
                forestTree.printTree(outputFilename);
                cout << "Tree printed to " << outputFilename << "." << endl;
                break;
            }
            case 9:
                cout << "Exiting the program. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 9);

    return 0;
}
