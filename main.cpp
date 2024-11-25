#include <iostream>
#include <fstream>
#include <limits>

#include "ForestTree.h"

using namespace std;

// Predefined file paths for loading and saving account data
const string ORIGINAL_FILE = "C:\\Users\\User\\OneDrive\\Desktop\\uob\\CSIS217-midterm\\CSIS217-midterm\\accountswithspace.txt";
const string UPDATED_FILE = "C:\\Users\\User\\OneDrive\\Desktop\\uob\\CSIS217-midterm\\CSIS217-midterm\\accountswithspace2.txt";

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

                // Attempt to read an integer
                if (cin >> accountNumber) {
                    // Check if the number is within the valid range
                    if (accountNumber >= 0 && accountNumber <= 99999) {
                        try {
                            // Attempt to remove the account
                            forestTree.removeAccount(accountNumber);
                            cout << "Account removed successfully." << endl;
                        } catch (const std::exception &e) {
                            // Handle any exceptions thrown during removal
                            cout << "Error removing account: " << e.what() << endl;
                        }
                    } else {
                        cout << "Error: Account number must be between 0 and 99999." << endl;
                    }
                } else {
                    // Clear the error state and ignore invalid input
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Error: Invalid input. Please enter a numeric account number." << endl;
                }

                break;
            }

            case 3: { // Add a transaction to an account
                int accountNumber;
                double amount;
                string typeInput;
                char type;

                cout << "Enter account number: ";

                // Attempt to read an integer for the account number
                if (cin >> accountNumber) {
                    // Validate the account number range
                    if (accountNumber >= 0 && accountNumber <= 99999) {
                        cout << "Enter transaction amount: ";

                        // Validate numeric input for the amount
                        if (cin >> amount) {
                            cout << "Enter type ('D' for Debit, 'C' for Credit): ";
                            cin >> typeInput;

                            // Convert input to lowercase for easier comparison
                            for (auto &c : typeInput) c = tolower(c);

                            // Validate and process the transaction type
                            if (typeInput == "d" || typeInput == "debit") {
                                type = 'D';
                            } else if (typeInput == "c" || typeInput == "credit") {
                                type = 'C';
                            } else {
                                cout << "Error: Invalid transaction type. Please enter 'D' for Debit, 'C' for Credit, or the full words 'Debit' or 'Credit'." << endl;
                                break;
                            }

                            try {
                                // Create a new transaction and automatically assign an ID
                                forestTree.addTransaction(accountNumber, Transaction(amount, type));
                                cout << "Transaction added successfully." << endl;
                            } catch (const std::exception &e) {
                                // Handle exceptions and display the error message
                                cout << "Error: " << e.what() << endl;
                            }
                        } else {
                            // Handle invalid input for the transaction amount
                            cin.clear();
                            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            cout << "Error: Invalid transaction amount. Please enter a number." << endl;
                        }
                    } else {
                        cout << "Error: Account number must be between 0 and 99999." << endl;
                    }
                } else {
                    // Handle invalid input for the account number
                    cin.clear();
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    cout << "Error: Invalid input. Please enter a numeric account number." << endl;
                }

                break;
            }



            case 4: { // Remove a transaction from an account
                int accountNumber, transactionID;

                cout << "Enter account number: ";

                // Attempt to read an integer for the account number
                if (cin >> accountNumber) {
                    // Validate the account number range
                    if (accountNumber >= 0 && accountNumber <= 99999) {
                        cout << "Enter transaction ID: ";

                        // Attempt to read an integer for the transaction ID
                        if (cin >> transactionID) {
                            try {
                                // Attempt to remove the transaction
                                forestTree.removeTransaction(accountNumber, transactionID);
                                cout << "Transaction removed successfully." << endl;
                            } catch (const std::exception &e) {
                                // Handle exceptions and display the error message
                                cout << "Error: " << e.what() << endl;
                            }
                        } else {
                            // Handle invalid input for the transaction ID
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Error: Invalid input. Please enter a numeric transaction ID." << endl;
                        }
                    } else {
                        cout << "Error: Account number must be between 0 and 99999." << endl;
                    }
                } else {
                    // Handle invalid input for the account number
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Error: Invalid input. Please enter a numeric account number." << endl;
                }

                break;
            }


            case 5: { // Search for an account by number
                int accountNumber;
                cout << "Enter the account number to search for: ";

                // Attempt to read an integer
                if (cin >> accountNumber) {
                    // Check if the number is within the valid range
                    if (accountNumber >= 0 && accountNumber <= 99999) {
                        // Perform the account search
                        Account *account = forestTree.searchAccount(accountNumber);
                        if (account) {
                            cout << "Account found:" << endl;
                            cout << *account << endl;
                        } else {
                            cout << "Account not found." << endl;
                        }
                    } else {
                        cout << "Error: Account number must be between 0 and 99999." << endl;
                    }
                } else {
                    // Clear the error state and ignore invalid input
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Error: Invalid input. Please enter a numeric account number." << endl;
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
