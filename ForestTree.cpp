#include "ForestTree.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

// Constructor for initializing an empty ForestTree
ForestTree::ForestTree() : root(nullptr) {}

// Destructor for cleaning up the ForestTree and releasing memory
ForestTree::~ForestTree() {
    for (auto &pair: accountMap) {
        deleteTree(pair.second);
    }
}

// Helper function to recursively delete accounts
void ForestTree::deleteTree(Account *node) {
    if (node) {
        delete node;
    }
}

// Initializes an empty ForestTree by clearing all accounts and resetting root
void ForestTree::initialize() {
    for (auto &pair: accountMap) {
        deleteTree(pair.second);
    }
    accountMap.clear();
    root = nullptr;
}

// Helper namespace for utility functions used in ForestTree operations
namespace {
    // Checks if a string is numeric
    bool isNumeric(const std::string &str) {
        return !str.empty() && str.find_first_not_of("0123456789") == std::string::npos;
    }

    // Cleans account description by removing unwanted characters and trimming spaces
    std::string cleanDescription(const std::string &desc) {
        std::string cleaned = desc;
        cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), '"'), cleaned.end());
        cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), '('), cleaned.end());
        cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), ')'), cleaned.end());
        while (!cleaned.empty() && std::isspace(cleaned.front())) cleaned.erase(0, 1);
        while (!cleaned.empty() && std::isspace(cleaned.back())) cleaned.pop_back();
        return cleaned;
    }

    // Determines the parent account number based on the current account number
    int findParentNumber(int accountNumber) {
        std::string numStr = std::to_string(accountNumber);
        if (numStr.length() <= 1) return 0;
        return std::stoi(numStr.substr(0, numStr.length() - 1));
    }
}

// Builds the chart of accounts by reading from a file
void ForestTree::buildFromFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::string line;
    std::vector<std::string> lines;

    // Read and clean lines from the file
    while (std::getline(file, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }

    // Process each line to build the accounts
    for (size_t i = 0; i < lines.size(); i++) {
        std::istringstream iss(lines[i]);
        std::string firstToken;
        iss >> firstToken;

        if (!isNumeric(firstToken)) continue;

        int accountNumber = std::stoi(firstToken);
        std::string description;
        double balance = 0.0;
        bool foundBalance = false;

        // Extract description and balance
        std::string word;
        while (iss >> word) {
            try {
                balance = std::stod(word);
                foundBalance = true;
                break;
            } catch (...) {
                if (!description.empty()) description += " ";
                description += word;
            }
        }

        // If no balance found, look at subsequent lines
        if (!foundBalance) {
            size_t j = i + 1;
            while (j < lines.size()) {
                std::istringstream nextIss(lines[j]);
                std::string nextFirstToken;
                nextIss >> nextFirstToken;

                if (isNumeric(nextFirstToken)) break;

                std::string remainingLine;
                std::getline(nextIss, remainingLine);

                std::istringstream remainingStream(remainingLine);
                while (remainingStream >> word) {
                    try {
                        balance = std::stod(word);
                        foundBalance = true;
                        break;
                    } catch (...) {
                        if (!description.empty()) description += " ";
                        description += word;
                    }
                }

                if (foundBalance) break;
                j++;
            }
            i = j - 1; // Adjust the loop counter
        }

        // Add the cleaned account to the tree
        description = cleanDescription(description);
        try {
            addAccount(accountNumber, description, balance);
        } catch (const std::exception &e) {
            std::cerr << "Error adding account " << accountNumber << ": " << e.what() << std::endl;
        }
    }
}

// Adds an account to the ForestTree
void ForestTree::addAccount(int accountNumber, const std::string &description, double initialBalance) {
    if (accountMap.find(accountNumber) != accountMap.end()) {
        Account *existingAccount = accountMap[accountNumber];
        existingAccount->updateBalance(initialBalance);
        return;
    }

    Account *newAccount = new Account(accountNumber, description, initialBalance);
    int parentNumber = findParentNumber(accountNumber);

    if (parentNumber > 0 && accountMap.find(parentNumber) != accountMap.end()) {
        newAccount->setParent(accountMap[parentNumber]);
    }

    accountMap[accountNumber] = newAccount;
}

// Removes an account by its number
void ForestTree::removeAccount(int accountNumber) {
    auto it = accountMap.find(accountNumber);
    if (it == accountMap.end()) {
        throw std::invalid_argument("Account not found");
    }

    delete it->second;
    accountMap.erase(it);
}

// Adds a transaction to an account
void ForestTree::addTransaction(int accountNumber, const Transaction &transaction) {
    Account *account = searchAccount(accountNumber);
    if (!account) {
        throw std::invalid_argument("Account not found");
    }

    account->addTransaction(new Transaction(transaction));
}

// Removes a transaction from an account
void ForestTree::removeTransaction(int accountNumber, int transactionID) {
    Account *account = searchAccount(accountNumber);
    if (!account) {
        throw std::invalid_argument("Account not found");
    }

    account->removeTransaction(transactionID);
}

// Searches for an account by its number
Account *ForestTree::searchAccount(int accountNumber) {
    auto it = accountMap.find(accountNumber);
    return (it != accountMap.end()) ? it->second : nullptr;
}

// Prints the entire ForestTree to a file
void ForestTree::printTree(const std::string &filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filename);
    }

    for (const auto &pair: accountMap) {
        if (!pair.second->getParent()) {
            printTreeRecursive(pair.second, file, 0);
        }
    }
}

// Recursively prints an account and its children
void ForestTree::printTreeRecursive(Account *account, std::ofstream &file, int indent) {
    if (!account) return;

    file << std::string(indent * 2, ' ')
         << account->getAccountNumber() << " "
         << std::setw(30) << std::left << account->getDescription()
         << std::fixed << std::setprecision(2) << account->getBalance() << "\n";

    for (const auto &pair: accountMap) {
        if (pair.second->getParent() == account) {
            printTreeRecursive(pair.second, file, indent + 1);
        }
    }
}

// Prints the details of a specific account to a file
void ForestTree::printAccountDetails(int accountNumber, const std::string &filename) {
    Account *account = searchAccount(accountNumber);
    if (!account) {
        throw std::invalid_argument("Account not found");
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filename);
    }

    file << "Account Details:\n"
         << "Number: " << account->getAccountNumber() << "\n"
         << "Description: " << account->getDescription() << "\n"
         << "Balance: " << std::fixed << std::setprecision(2) << account->getBalance() << "\n\n"
         << "Transactions:\n";

    for (const Transaction *transaction: account->getTransactions()) {
        file << "  " << *transaction << "\n";
    }
}
