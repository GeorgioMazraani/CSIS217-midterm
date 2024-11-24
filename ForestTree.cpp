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
    bool isNumeric(const std::string& str) {
        return !str.empty() && str.find_first_not_of("0123456789") == std::string::npos;
    }

    std::string cleanDescription(const std::string& desc) {
        std::string cleaned = desc;
        // Remove quotes and parentheses
        cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), '"'), cleaned.end());
        cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), '('), cleaned.end());
        cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), ')'), cleaned.end());

        // Trim spaces
        while (!cleaned.empty() && std::isspace(cleaned.front())) cleaned.erase(0, 1);
        while (!cleaned.empty() && std::isspace(cleaned.back())) cleaned.pop_back();

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
    std::vector<std::string> lines;

    // First, read all lines and clean them
    while (std::getline(file, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }

    // Process lines
    for (size_t i = 0; i < lines.size(); i++) {
        // Split the line into tokens
        std::istringstream iss(lines[i]);
        std::string firstToken;
        iss >> firstToken;

        if (!isNumeric(firstToken)) continue;

        int accountNumber = std::stoi(firstToken);
        std::string description;
        double balance = 0.0;
        bool foundBalance = false;

        // Read the entire line into a vector of tokens
        std::vector<std::string> tokens;
        std::string token;
        while (iss >> token) {
            tokens.push_back(token);
        }

        // The last token should be the balance
        if (!tokens.empty()) {
            try {
                balance = std::stod(tokens.back());
                tokens.pop_back(); // Remove the balance from tokens
                foundBalance = true;
            } catch (...) {
                // Last token wasn't a number, need to look ahead
            }
        }

        // If we haven't found the balance, look ahead to subsequent lines
        if (!foundBalance) {
            size_t j = i + 1;
            while (j < lines.size()) {
                std::istringstream nextIss(lines[j]);
                std::string nextFirstToken;
                nextIss >> nextFirstToken;

                if (isNumeric(nextFirstToken)) break;

                // Add the entire line to tokens
                while (nextIss >> token) {
                    tokens.push_back(token);
                }

                // Check if the last token is a number
                if (!tokens.empty()) {
                    try {
                        balance = std::stod(tokens.back());
                        tokens.pop_back(); // Remove the balance
                        foundBalance = true;
                        break;
                    } catch (...) {
                        // Continue to next line
                    }
                }
                j++;
            }
            i = j - 1;  // Update the outer loop counter
        }

        // Join all remaining tokens to form the description
        description = "";
        for (const auto& tok : tokens) {
            if (!description.empty()) description += " ";
            description += tok;
        }

        // Clean up the description and add the account
        description = cleanDescription(description);
        try {
            if (!description.empty()) {
                addAccount(accountNumber, description, balance);
            }
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
    }
    // Remove business unit identifiers (A, B) from account numbers
    else {
        size_t len = accountStr.length();
        if (len > 5) {
            accountNumber = std::stoi(accountStr.substr(0, 5));
        }
    }

    if (accountNumber <= 0 || accountNumber > 99999) {
        throw std::invalid_argument("Account number must be between 1 and 99999");
    }

    if (accountMap.find(accountNumber) != accountMap.end()) {
        // Update existing account instead of throwing an error
        Account* existingAccount = accountMap[accountNumber];
        existingAccount->updateBalance(initialBalance);
        return;
    }

    Account* newAccount = new Account(accountNumber, description, initialBalance);

    int parentNumber = findParentNumber(accountNumber);
    if (parentNumber > 0) {
        auto parentIt = accountMap.find(parentNumber);
        if (parentIt != accountMap.end()) {
            newAccount->setParent(parentIt->second);
        }
    }

    accountMap[accountNumber] = newAccount;
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
         << std::setw(30) << std::left << account->getDescription().substr(0, 30)
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