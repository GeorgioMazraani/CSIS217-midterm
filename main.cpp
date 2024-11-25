#include <iostream>
#include <fstream>
#include <limits>

#include "ForestTree.h"

using namespace std;

// Predefined file paths for loading and saving account data
const string ORIGINAL_FILE = "C:\\Users\\lucas\\CLionProjects\\untitled\\accountswithspace.txt";
const string UPDATED_FILE = "C:\\Users\\lucas\\CLionProjects\\untitled\\accountswithspace2.txt";

// Displays the main menu to the user
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

// Main function: Entry point of the program
int main() {
    ForestTree forestTree; // Create the ForestTree instance to manage accounts
    int choice; // Variable to store user menu selection

    // Load accounts data from the UPDATED_FILE if it exists, otherwise from ORIGINAL_FILE
    ifstream updatedFile(UPDATED_FILE);
    if (updatedFile) {
        try {
            forestTree.buildFromFile(UPDATED_FILE);
            cout << "Accounts successfully loaded from " << UPDATED_FILE << "." << endl;
        } catch (const exception &e) {
            cerr << "Error loading updated accounts: " << e.what() << endl;
            return 1; // Exit if the updated file cannot be loaded
        }
    } else {
        try {
            forestTree.buildFromFile(ORIGINAL_FILE);
            cout << "Accounts successfully loaded from " << ORIGINAL_FILE << "." << endl;
        } catch (const exception &e) {
            cerr << "Error loading original accounts: " << e.what() << endl;
            return 1; // Exit if the original file cannot be loaded
        }
    }

    // Main menu loop
    do {
        displayMenu();
        cout << "Enter your choice: ";

        while (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number (1-7): \n ";
            displayMenu();
        }

        switch (choice) {
            case 1: { // Add a new account
                int accountNumber;
                std::string description;
                double initialBalance;

                cout << "Enter account number: ";
                if (!(cin >> accountNumber)) { // Validate numeric input
                    cout << "Error: Invalid account number. Please enter a valid number between 1 and 5 digits." << endl;
                    cin.clear(); // Clear the error flag
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the invalid input
                    break;
                }

                if (accountNumber < 1 || accountNumber > 99999) { // Validate range
                    cout << "Error: Invalid account number. Account number must be between 1 and 5 digits." << endl;
                    break;
                }

                cout << "Enter description: ";
                cin.ignore(); // Clear the newline character left in the buffer
                getline(cin, description);

                cout << "Enter initial balance: ";
                if (!(cin >> initialBalance)) { // Validate numeric input for initial balance
                    cout << "Error: Invalid initial balance. Please enter a valid number." << endl;
                    cin.clear(); // Clear the error flag
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the invalid input
                    break;
                }

                try {
                    forestTree.addAccount(accountNumber, description, initialBalance);
                    cout << "Account added successfully." << endl;
                } catch (const std::exception &e) {
                    cout << "Error adding account: " << e.what() << endl;
                }

                break;
            }


            case 2: { // Remove an account
                int accountNumber;

                cout << "Enter the account number to remove: ";
                cin >> accountNumber;

                try {
                    forestTree.removeAccount(accountNumber);
                    cout << "Account removed successfully." << endl;
                } catch (const std::exception &e) {
                    cout << "Error removing account: " << e.what() << endl;
                }
                break;
            }

            case 3: { // Add a transaction to an account
                int accountNumber;
                double amount;
                char type;

                cout << "Enter account number: ";
                cin >> accountNumber;

                cout << "Enter transaction amount: ";
                if (!(cin >> amount)) { // Validate numeric input for amount
                    cout << "Error: Invalid transaction amount. Please enter a number." << endl;
                    cin.clear(); // Clear the error flag
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore invalid input
                    break;
                }

                cout << "Enter type ('D' for Debit, 'C' for Credit): ";
                cin >> type;

                // Convert type to uppercase to handle case-insensitivity
                type = toupper(type);

                // Validate the transaction type
                if (type != 'D' && type != 'C') {
                    cout << "Error: Invalid transaction type. Please enter 'D' for Debit or 'C' for Credit." << endl;
                    break;
                }

                try {
                    // Create a new transaction and automatically assign an ID
                    forestTree.addTransaction(accountNumber, Transaction(amount, type));
                    cout << "Transaction added successfully." << endl;
                } catch (const std::exception &e) {
                    cout << "Error: " << e.what() << endl;
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
                } catch (const std::exception &e) {
                    cout << "Error: " << e.what() << endl; // Display the error message using cout
                }
                break;
            }

            case 5: { // Search for an account by number
                int accountNumber;

                cout << "Enter the account number to search for: ";
                cin >> accountNumber;
                while (!(cin >> accountNumber)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter a valid account number: ";
                }

                Account *account = forestTree.searchAccount(accountNumber);
                if (account) {
                    cout << "Account found:" << endl;
                    cout << *account << endl;
                } else {
                    cout << "Account not found." << endl;
                }
                break;
            }
            case 6: { // Print the chart of accounts to a temporary file and display it
                try {
                    string tempFile = "current_chart_of_accounts.txt";
                    forestTree.printTree(tempFile);
                    cout << "Current state of the chart of accounts saved to " << tempFile << "." << endl;

                    ifstream file(tempFile);
                    if (file) {
                        cout << file.rdbuf(); // Display file contents to the console
                        file.close();
                    }
                } catch (const exception &e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
            }
            case 7: { // Save changes to UPDATED_FILE and exit
                try {
                    forestTree.printTree(UPDATED_FILE);
                    cout << "All changes saved to " << UPDATED_FILE << ". Goodbye!" << endl;
                } catch (const exception &e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
            }
            default: // Invalid menu choice
                cout << "Invalid choice. Please enter a valid option." << endl;

        }
    } while (choice != 7); // Exit when the user selects option 7

    return 0;
}
