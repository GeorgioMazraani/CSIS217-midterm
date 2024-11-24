#include "ForestTree.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

// Constructor
ForestTree::ForestTree() : root(nullptr) {}

// Destructor
ForestTree::~ForestTree() {
    for (auto& pair : accountMap) {
        deleteTree(pair.second);
    }
}

// Helper to recursively delete accounts
void ForestTree::deleteTree(Account* node) {
    if (node) {
        delete node;
    }
}

// Initialize an empty ForestTree
void ForestTree::initialize() {
    for (auto& pair : accountMap) {
        deleteTree(pair.second);
    }
    accountMap.clear();
    root = nullptr;
}

// Helper functions for parsing
namespace {

    std::string cleanDescription(const std::string& desc) {
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



    int findParentNumber(int accountNumber) {
        std::string numStr = std::to_string(accountNumber);
        if (numStr.length() <= 1) return 0;
        return std::stoi(numStr.substr(0, numStr.length() - 1));
    }
}

// Build the chart of accounts from file
void ForestTree::buildFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue; // Skip empty lines

        std::istringstream iss(line);
        std::string firstToken;
        iss >> firstToken;

        // Directly validate if firstToken can be converted to an integer
        int accountNumber;
        try {
            accountNumber = std::stoi(firstToken); // Attempt to convert to integer
        } catch (const std::exception&) {
            continue; // Skip lines where the first token is not a valid number
        }

        std::string description;
        double balance = 0.0;

        // Extract the rest of the line
        std::string remainingLine;
        std::getline(iss, remainingLine);

        // Split the line to identify the balance
        size_t lastSpace = remainingLine.find_last_of(' ');

        if (lastSpace != std::string::npos) {
            std::string potentialBalance = remainingLine.substr(lastSpace + 1);
            try {
                // Attempt to parse the balance as a number
                balance = std::stod(potentialBalance);
                description = remainingLine.substr(0, lastSpace); // The rest is the description
            } catch (const std::exception&) {
                // If parsing fails, treat the entire line as description
                description = remainingLine;
            }
        } else {
            description = remainingLine; // No balance, treat everything as description
        }

        // Clean the description to remove unwanted characters
        description = cleanDescription(description);

        // Add the account to the tree
        try {
            addAccount(accountNumber, description, balance);
        } catch (const std::exception& e) {
            std::cerr << "Error adding account " << accountNumber << ": " << e.what() << std::endl;
        }
    }
}



void ForestTree::addAccount(int accountNumber, const std::string& description, double initialBalance) {
    std::string accountStr = std::to_string(accountNumber);

    // Handle special cases for account numbers with business unit identifiers
    if (accountStr.find("181") == 0 || accountStr.find("188") == 0) {
        accountNumber = std::stoi(accountStr.substr(0, 3));
    } else if (accountStr.length() > 5) { // Remove extra identifiers if length exceeds 5
        accountNumber = std::stoi(accountStr.substr(0, 5));
    }

    // Validate account number range
    if (accountNumber <= 0 || accountNumber > 999999) {
        std::cerr << "Error: Account number must be between 1 and 999999.\n";
        return; // Skip adding the account
    }

    // Check for duplicate account
    if (accountMap.find(accountNumber) != accountMap.end()) {
        std::cerr << "Warning: Account number " << accountNumber << " already exists. Skipping addition.\n";
        return; // Skip adding the account
    }

    // Create a new account
    Account* newAccount = new Account(accountNumber, description, initialBalance);

    // Find and set parent account if applicable
    int parentNumber = findParentNumber(accountNumber);
    if (parentNumber > 0) {
        auto parentIt = accountMap.find(parentNumber);
        if (parentIt != accountMap.end()) {
            newAccount->setParent(parentIt->second);
        }
    }

    // Add new account to the map
    accountMap[accountNumber] = newAccount;
    std::cout << "Added account number " << accountNumber << " successfully.\n";
}


void ForestTree::removeAccount(int accountNumber) {
    auto it = accountMap.find(accountNumber);
    if (it == accountMap.end()) {
        throw std::invalid_argument("Account not found");
    }

    Account* account = it->second;
    accountMap.erase(it);
    delete account;
}

void ForestTree::addTransaction(int accountNumber, const Transaction& transaction) {
    Account* account = searchAccount(accountNumber);
    if (!account) {
        throw std::invalid_argument("Account not found");
    }

    account->addTransaction(new Transaction(transaction));
}

void ForestTree::removeTransaction(int accountNumber, int transactionID) {
    Account* account = searchAccount(accountNumber);
    if (!account) {
        throw std::invalid_argument("Account not found");
    }

    account->removeTransaction(transactionID);
}

Account* ForestTree::searchAccount(int accountNumber) {
    auto it = accountMap.find(accountNumber);
    return (it != accountMap.end()) ? it->second : nullptr;
}

void ForestTree::printTree(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filename);
    }

    for (const auto& pair : accountMap) {
        if (!pair.second->getParent()) {  // Start with root level accounts
            printTreeRecursive(pair.second, file, 0);
        }
    }
}

void ForestTree::printTreeRecursive(Account* account, std::ofstream& file, int indent) {
    if (!account) return;

    file << std::string(indent * 2, ' ')
         << account->getAccountNumber() << " "
         << std::setw(90) << std::left << account->getDescription().substr(0, 90)
         << std::fixed << std::setprecision(2) << account->getBalance() << std::endl;

    // Find and print children
    for (const auto& pair : accountMap) {
        Account* childAccount = pair.second;
        if (childAccount->getParent() == account) {
            printTreeRecursive(childAccount, file, indent + 1);
        }
    }
}

void ForestTree::printAccountDetails(int accountNumber, const std::string& filename) {
    Account* account = searchAccount(accountNumber);
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

    const auto& transactions = account->getTransactions();
    for (const Transaction* transaction : transactions) {
        file << "  " << *transaction << "\n";
    }
}