#ifndef CSIS217_PROJECT_FORESTTREE_H
#define CSIS217_PROJECT_FORESTTREE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Account.h"
#include "Transaction.h"  // Include the full definition of Transaction

class ForestTree {
private:
    Account* root; // Root of the tree
    std::map<int, Account*> accountMap; // Map for quick account lookup by number

    // Helper to recursively delete accounts
    void deleteTree(Account* node);

public:
    // Constructor
    ForestTree();

    // Destructor
    ~ForestTree();

    // Initialize an empty ForestTree
    void initialize();

    // Build the chart of accounts by reading from a file
    void buildFromFile(const std::string& filename);

    // Add an account
    void addAccount(int accountNumber, const std::string& description, double initialBalance);

    // Remove an account
    void removeAccount(int accountNumber);

    // Add a transaction
    void addTransaction(int accountNumber, const Transaction& transaction);

    // Remove a transaction
    void removeTransaction(int accountNumber, int transactionID);

    // Search for an account
    Account* searchAccount(int accountNumber);

    // Print the entire tree
    void printTree(const std::string& filename);

    // Print account details
    void printAccountDetails(int accountNumber, const std::string& filename);
};

#endif // CSIS217_PROJECT_FORESTTREE_H