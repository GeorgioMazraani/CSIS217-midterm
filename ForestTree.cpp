#include "ForestTree.h"
#include "Transaction.h"  // Required for complete definition of Transaction
#include <fstream>
#include <iostream>
#include <stdexcept>

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
        node->removeTransaction(transaction->getTransactionID()); // Use -> for pointers
    }

    delete node;
}

// Initialize the tree
void ForestTree::initialize() {
    deleteTree(root);
    root = nullptr;
    accountMap.clear();
}
void ForestTree::buildFromFile(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        int accountNumber;
        string description;
        double initialBalance;

        if (line.empty() || isspace(line[0])) {
            // Skip blank lines or lines that don't start with an account number
            continue;
        }

        ss >> accountNumber;
        ss.ignore(); // Skip whitespace
        getline(ss, description, ' '); // Adjust to capture full description
        ss >> initialBalance;

        try {
            addAccount(accountNumber, description, initialBalance);
        } catch (const exception& e) {
            cerr << "Error adding account: " << e.what() << endl;
        }
    }

    file.close();
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

    delete it->second;
    accountMap.erase(it);
}

// Add a transaction
void ForestTree::addTransaction(int accountNumber, const Transaction& transaction) {
    auto it = accountMap.find(accountNumber);
    if (it == accountMap.end()) {
        cerr << "Account not found: " << accountNumber << endl;
        return;
    }

    // Create a new Transaction dynamically and pass it
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

    file << string(indent, ' ') << account->getAccountNumber() << " "
         << account->getDescription() << " "
         << fixed << setprecision(2) << account->getBalance() << "\n";

    for (const Transaction* transaction : account->getTransactions()) {
        file << string(indent + 2, ' ') << *transaction << "\n";
    }

    // Print subaccounts
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
