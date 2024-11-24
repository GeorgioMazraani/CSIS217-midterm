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
    // Determines the parent account number based on the current account number
    int findParentNumber(int accountNumber) {
        std::string numStr = std::to_string(accountNumber);
        if (numStr.length() <= 1) return 0;
        return std::stoi(numStr.substr(0, numStr.length() - 1));
    }

    // Checks if a string is numeric
    bool isNumeric(const std::string &str) {
        return !str.empty() && str.find_first_not_of("0123456789") == std::string::npos;
    }

    // Cleans account description by removing unwanted characters and trimming spaces
    std::string cleanDescription(const std::string &desc) {
        // Handle empty strings
        if (desc.empty()) {
            return "";
        }

        std::string cleaned = desc;

        // Trim leading whitespace
        size_t start = cleaned.find_first_not_of(" \t");
        if (start != std::string::npos) {
            cleaned = cleaned.substr(start);
        }

        // Trim trailing whitespace
        size_t end = cleaned.find_last_not_of(" \t");
        if (end != std::string::npos) {
            cleaned = cleaned.substr(0, end + 1);
        }

        // Ensure no internal changes to valid characters
        return cleaned;
    }


}

// Builds the chart of accounts by reading from a file
void ForestTree::buildFromFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::string line, description;
    int accountNumber = 0;
    double balance = 0.0;
    bool readingDescription = false;

    while (std::getline(file, line)) {
        if (line.empty()) continue; // Skip empty lines

        std::istringstream iss(line);
        std::string firstToken;
        iss >> firstToken;

        // Check if the first token is numeric (account number)
        if (std::isdigit(firstToken[0])) {
            // If already reading a description, finalize the previous account
            if (readingDescription) {
                description = cleanDescription(description);
                try {
                    addAccount(accountNumber, description, balance);
                } catch (const std::exception &e) {
                    std::cerr << "Error adding account " << accountNumber << ": " << e.what() << std::endl;
                }
                description.clear();
            }

            // Parse the account number
            try {
                accountNumber = std::stoi(firstToken);
            } catch (const std::exception &) {
                std::cerr << "Error parsing account number in line: " << line << std::endl;
                continue;
            }

            // Extract the rest of the line
            std::getline(iss, line);
            size_t lastSpace = line.find_last_of(' ');

            if (lastSpace != std::string::npos) {
                std::string potentialBalance = line.substr(lastSpace + 1);
                try {
                    // Attempt to parse the balance
                    balance = std::stod(potentialBalance);
                    description = line.substr(0, lastSpace); // Description is the remaining part
                } catch (const std::exception &) {
                    // Treat the entire line as description if parsing balance fails
                    description = line;
                    balance = 0.0;
                }
            } else {
                description = line;
                balance = 0.0;
            }

            readingDescription = true;
        } else if (readingDescription) {
            // If line doesn't start with a number, append it to the description
            description += " " + line;
        }
    }

    // Add the last account if reading description
    if (readingDescription) {
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
    // Check if the account already exists
    if (accountMap.find(accountNumber) != accountMap.end()) {
        throw std::invalid_argument("Account number already exists: " + std::to_string(accountNumber));
    }

    // Create a new account
    Account *newAccount = new Account(accountNumber, description, initialBalance);
    int parentNumber = findParentNumber(accountNumber);

    // Set the parent account if it exists
    if (parentNumber > 0 && accountMap.find(parentNumber) != accountMap.end()) {
        newAccount->setParent(accountMap[parentNumber]);
    }

    // Add the new account to the map
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
         << std::setw(30) << std::left << account->getDescription() << " "
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
