#include "ForestTree.h"
#include "Transaction.h"  // Required for complete definition of Transaction
#include <fstream>
#include <iostream>
#include <stdexcept>

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

// Build the tree from a file
void ForestTree::buildFromFile(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    int accountNumber;
    string description;
    double initialBalance;

    while (file >> accountNumber >> description >> initialBalance) {
        addAccount(accountNumber, description, initialBalance);
    }

    file.close();
}

// Add an account
void ForestTree::addAccount(int accountNumber, const string& description, double initialBalance) {
    if (accountMap.find(accountNumber) != accountMap.end()) {
        cerr << "Account already exists: " << accountNumber << endl;
        return;
    }

    Account* newAccount = new Account(accountNumber, description, initialBalance);

    if (!root) {
        root = newAccount;
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

// Print the entire tree
void ForestTree::printTree(const string& filename) {
    ofstream file(filename);
    if (!file) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    for (const auto& [accountNumber, account] : accountMap) {
        file << *account << endl;
    }

    file.close();
}

// Print account details
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

    file << *(it->second) << endl;

    file.close();
}
