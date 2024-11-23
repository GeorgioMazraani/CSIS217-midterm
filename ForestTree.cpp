#include "ForestTree.h"
#include "Transaction.h"  // Required for complete definition of Transaction
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <cctype>
#include <cstdlib>

#include <iomanip>
using namespace std;

// Constructor
ForestTree::ForestTree() : root(nullptr) {}

// Destructor
ForestTree::~ForestTree() {
    deleteTree(root);
}

// Recursively delete the tree
void ForestTree::deleteTree(Account* node) {
    if (!node) return;

    for (Transaction* transaction : node->getTransactions()) {
        node->removeTransaction(transaction->getTransactionID());
    }
    delete node;
    accountMap.clear(); // Ensure the map is cleared after deleting accounts
}


// Initialize the tree
void ForestTree::initialize() {
    deleteTree(root);
    root = nullptr;
    accountMap.clear();
}



// Helper function to check if a string is a valid double
bool isDouble(const std::string& token) {
    char* end = nullptr;
    strtod(token.c_str(), &end);
    return end != token.c_str() && *end == '\0';
}
void ForestTree::buildFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || std::isspace(line[0])) {
            // Skip empty lines or lines that start with whitespace
            continue;
        }

        std::istringstream ss(line);
        std::string token;
        int accountNumber = 0;
        double initialBalance = 0.0;
        std::string description;
        bool validLine = true;

        // Extract and validate account number
        if (!(ss >> accountNumber) || accountNumber < 1 || accountNumber > 99999) {
            std::cerr << "Error parsing account number in line: " << line << std::endl;
            validLine = false;
        }

        if (!validLine) continue; // Skip invalid lines

        // Extract description and balance
        while (ss >> token) {
            if (isDouble(token)) {
                initialBalance = std::stod(token);
                break;
            } else {
                if (!description.empty()) {
                    description += " ";
                }
                description += token;
            }
        }

        if (description.empty() || initialBalance == 0.0) {
            std::cerr << "Error parsing description or balance in line: " << line << std::endl;
            continue;
        }

        try {
            addAccount(accountNumber, description, initialBalance);
        } catch (const std::exception& e) {
            std::cerr << "Error adding account: " << e.what() << std::endl;
        }
    }

    file.close();
    std::cout << "Finished building accounts from file: " << filename << std::endl;
}


void ForestTree::addAccount(int accountNumber, const string& description, double initialBalance) {
    if (accountMap.find(accountNumber) != accountMap.end()) {
        cerr << "Account already exists: " << accountNumber << endl;
        return;
    }

    Account* newAccount = new Account(accountNumber, description, initialBalance);

    // Find the parent account based on the most significant digits
    int parentNumber = accountNumber / 10; // Assume parent is derived by removing the last digit
    auto parentIt = accountMap.find(parentNumber);
    if (parentIt != accountMap.end()) {
        newAccount->setParent(parentIt->second); // Set the parent
    } else if (!root) {
        root = newAccount; // Set as root if no parent and tree is empty
    }

    accountMap[accountNumber] = newAccount;
}



// Remove an account
void ForestTree::removeAccount(int accountNumber) {
    auto it = accountMap.find(accountNumber);
    if (it == accountMap.end()) {
        cerr << "Account not found: " << accountNumber << endl;
        return;
    }

    // Recursively remove all child accounts
    for (auto& [childNumber, childAccount] : accountMap) {
        if (childAccount->getParent() == it->second) {
            removeAccount(childNumber);
        }
    }

    delete it->second; // Delete the account
    accountMap.erase(it); // Remove from the map
}


// Add a transaction
void ForestTree::addTransaction(int accountNumber, const Transaction& transaction) {
    auto it = accountMap.find(accountNumber);
    if (it == accountMap.end()) {
        cerr << "Account not found: " << accountNumber << endl;
        return;
    }

    if (!transaction.isValid(it->second)) {
        cerr << "Invalid transaction: Insufficient balance." << endl;
        return; // Do not allocate memory for invalid transactions
    }

    Transaction* newTransaction = new Transaction(transaction.getTransactionID(),
                                                  transaction.getAmount(),
                                                  transaction.getDebitOrCredit(),
                                                  transaction.getRelatedAccount());
    it->second->addTransaction(newTransaction);
}


// Remove a transaction
void ForestTree::removeTransaction(int accountNumber, int transactionID) {
    auto it = accountMap.find(accountNumber);
    if (it == accountMap.end()) {
        cerr << "Account not found: " << accountNumber << endl;
        return;
    }

    it->second->removeTransaction(transactionID);
}

// Search for an account
Account* ForestTree::searchAccount(int accountNumber) {
    auto it = accountMap.find(accountNumber);
    return (it != accountMap.end()) ? it->second : nullptr;
}


void ForestTree::printAccountDetails(int accountNumber, const string& filename) {
    auto it = accountMap.find(accountNumber);
    if (it == accountMap.end()) {
        cerr << "Account not found: " << accountNumber << endl;
        return;
    }

    ofstream file(filename);
    if (!file) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    Account* account = it->second;
    file << "Account Number: " << account->getAccountNumber() << "\n"
         << "Description: " << account->getDescription().substr(0, 10) << "\n"
         << "Balance: " << fixed << setprecision(2) << account->getBalance() << "\n";

    file << "Transactions:\n";
    for (const Transaction* transaction : account->getTransactions()) {
        file << "  - " << *transaction << "\n";
    }

    // Add subaccount details recursively
    for (const auto& [subAccountNumber, subAccount] : accountMap) {
        if (to_string(subAccountNumber).find(to_string(accountNumber)) == 0 && subAccountNumber != accountNumber) {
            file << "  Subaccount: " << subAccountNumber << " - Balance: " << subAccount->getBalance() << "\n";
        }
    }

    file.close();
}
void ForestTree::printTreeRecursive(Account* account, ofstream& file, int indent = 0) {
    if (!account) return;

    // Print account details with proper indentation
    file << string(indent, ' ')
         << account->getAccountNumber() << " "
         << account->getDescription() << " "
         << fixed << setprecision(2) << account->getBalance() << "\n";

    // Print transactions under this account
    for (const Transaction* transaction : account->getTransactions()) {
        file << string(indent + 4, ' ') << "Transaction ID: " << transaction->getTransactionID() << "\n"
             << string(indent + 4, ' ') << "Amount: " << transaction->getAmount() << "\n"
             << string(indent + 4, ' ') << "Type: " << (transaction->getDebitOrCredit() == 'D' ? "Debit" : "Credit") << "\n"
             << string(indent + 4, ' ') << "Related Account: " << transaction->getRelatedAccount() << "\n";
    }

    // Print subaccounts recursively
    for (const auto& [subAccountNumber, subAccount] : accountMap) {
        if (subAccount->getParent() == account) {
            printTreeRecursive(subAccount, file, indent + 4);
        }
    }
}


void ForestTree::printTree(const string& filename) {
    ofstream file(filename);
    if (!file) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    if (root) {
        printTreeRecursive(root, file);
    }

    file.close();
}
