#include <iostream>
#include <fstream>
#include "ForestTree.h"

using namespace std;

// Predefined file paths
const string ORIGINAL_FILE = "C://Users//User//OneDrive//Desktop//uob//5th semester 2024//ads//CSIS217-midterm//accountswithspace.txt";
const string UPDATED_FILE = "C://Users//User//OneDrive//Desktop//uob//5th semester 2024//ads//CSIS217-midterm//updated_accounts.txt";

// Function to display the menu
void displayMenu() {
    cout << "\n======================================" << endl;
    cout << " Lebanese Chart of Accounts - Menu    " << endl;
    cout << "======================================" << endl;
    cout << "1. Add a new account" << endl;
    cout << "2. Remove an account" << endl;
    cout << "3. Add a transaction to an account" << endl;
    cout << "4. Remove a transaction from an account" << endl;
    cout << "5. Search for an account by number" << endl;
    cout << "6. Print the chart of accounts (current state)" << endl;
    cout << "7. Exit and save changes to a new file" << endl;
    cout << "======================================" << endl;
}

// Main function
int main() {
    ForestTree forestTree; // Create the ForestTree instance
    int choice;

    // Automatically load from UPDATED_FILE if it exists, otherwise load ORIGINAL_FILE
    ifstream updatedFile(UPDATED_FILE);
    if (updatedFile) {
        try {
            forestTree.buildFromFile(UPDATED_FILE);
            cout << "Accounts successfully loaded from " << UPDATED_FILE << "." << endl;
        } catch (const exception& e) {
            cerr << "Error loading updated accounts: " << e.what() << endl;
            return 1; // Exit if updated file cannot be loaded
        }
    } else {
        try {
            forestTree.buildFromFile(ORIGINAL_FILE);
            cout << "Accounts successfully loaded from " << ORIGINAL_FILE << "." << endl;
        } catch (const exception& e) {
            cerr << "Error loading original accounts: " << e.what() << endl;
            return 1; // Exit if original file cannot be loaded
        }
    }

    do {
        displayMenu();
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: { // Add a new account
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
            case 2: { // Remove an account
                int accountNumber;
                cout << "Enter the account number to remove: ";
                cin >> accountNumber;

                forestTree.removeAccount(accountNumber);
                cout << "Account removed successfully." << endl;
                break;
            }
            case 3: { // Add a transaction to an account
                int accountNumber, transactionID;
                double amount;
                char type;

                cout << "Enter account number: ";
                cin >> accountNumber;
                cout << "Enter transaction ID: ";
                cin >> transactionID;
                cout << "Enter transaction amount: ";
                cin >> amount;
                cout << "Enter type ('D' for Debit, 'C' for Credit): ";
                cin >> type;

                try {
                    Transaction transaction(transactionID, amount, type, "");
                    forestTree.addTransaction(accountNumber, transaction);
                    cout << "Transaction added successfully." << endl;
                } catch (const exception& e) {
                    cerr << "Error: " << e.what() << endl;
                }
                break;
            }
            case 4: { // Remove a transaction from an account
                int accountNumber, transactionID;

                cout << "Enter account number: ";
                cin >> accountNumber;
                cout << "Enter transaction ID: ";
                cin >> transactionID;

                try {
                    forestTree.removeTransaction(accountNumber, transactionID);
                    cout << "Transaction removed successfully." << endl;
                } catch (const exception& e) {
                    cerr << "Error: " << e.what() << endl;
                }
                break;
            }
            case 5: { // Search for an account
                int accountNumber;

                cout << "Enter the account number to search for: ";
                cin >> accountNumber;

                Account* account = forestTree.searchAccount(accountNumber);
                if (account) {
                    cout << "Account found:" << endl;
                    cout << *account << endl;
                } else {
                    cout << "Account not found." << endl;
                }
                break;
            }
            case 6: { // Print the chart of accounts
                try {
                    string tempFile = "current_chart_of_accounts.txt";
                    forestTree.printTree(tempFile);
                    cout << "Current state of the chart of accounts saved to " << tempFile << "." << endl;

                    ifstream file(tempFile);
                    if (file) {
                        cout << file.rdbuf(); // Print file content to console
                        file.close();
                    }
                } catch (const exception& e) {
                    cerr << "Error: " << e.what() << endl;
                }
                break;
            }
            case 7: { // Save changes to a new file and exit
                try {
                    forestTree.printTree(UPDATED_FILE);
                    cout << "All changes saved to " << UPDATED_FILE << ". Goodbye!" << endl;
                } catch (const exception& e) {
                    cerr << "Error: " << e.what() << endl;
                }
                break;
            }
            default:
                cout << "Invalid choice. Please enter a valid option." << endl;
        }
    } while (choice != 7);

    return 0;
}
