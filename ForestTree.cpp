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

        // If the account number is single-digit or invalid, return 0 (no parent)
        if (numStr.length() <= 1) {
            return 0;
        }

        // For five-digit accounts, trim one digit to find the parent
        if (numStr.length() == 5) {
            return std::stoi(numStr.substr(0, 4));
        }

        // For other lengths, trim the last digit
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

        if (std::isdigit(firstToken[0])) { // Starts with a digit (account number)
            if (readingDescription) { // Finalize the previous account
                description = cleanDescription(description);
                try {
                    addAccount(accountNumber, description, balance);
                } catch (const std::exception &e) {
                    std::cerr << "Error adding account " << accountNumber << ": " << e.what() << std::endl;
                }
                description.clear();
            }

            // Parse account number and description/balance
            try {
                accountNumber = std::stoi(firstToken);
            } catch (const std::exception &) {
                std::cerr << "Error parsing account number: " << line << std::endl;
                continue;
            }

            // Extract the remaining line for description/balance
            std::getline(iss, line);
            size_t lastSpace = line.find_last_of(' ');

            if (lastSpace != std::string::npos) {
                std::string potentialBalance = line.substr(lastSpace + 1);
                try {
                    balance = std::stod(potentialBalance); // Parse balance
                    description = line.substr(0, lastSpace);
                } catch (const std::exception &) {
                    description = line; // Treat entire line as description
                    balance = 0.0;
                }
            } else {
                description = line;
                balance = 0.0;
            }

            readingDescription = true;
        } else if (line.find("Transaction ID:") != std::string::npos) { // Transaction line
            try {
                // Debug: Print the full line being processed
                std::cout << "Processing line: " << line << std::endl;

                // Extract positions of keys
                size_t idPos = line.find("Transaction ID:");
                size_t amountPos = line.find("Amount:");
                size_t typePos = line.find("Type:");

                // Debug: Print positions of keys
                std::cout << "idPos: " << idPos << ", amountPos: " << amountPos << ", typePos: " << typePos << std::endl;

                // Ensure all keys exist in the line
                if (idPos != std::string::npos && amountPos != std::string::npos && typePos != std::string::npos) {
                    // Parse Transaction ID
                    std::string idStr = line.substr(idPos + 14, amountPos - (idPos + 14)); // Extract substring
                    idStr.erase(idStr.find_last_not_of(" ,:\n\r\t") + 1);                  // Trim unwanted characters
                    idStr.erase(0, idStr.find_first_not_of(" ,:\n\r\t"));                 // Trim leading characters

                    // Debug: Print extracted Transaction ID after cleanup
                    std::cout << "Extracted Transaction ID: " << idStr << std::endl;

                    int transactionID = std::stoi(idStr); // Convert to integer

                    // Parse Transaction Amount
                    std::string amountStr = line.substr(amountPos + 7, typePos - (amountPos + 7));
                    amountStr.erase(amountStr.find_last_not_of(" \n\r\t") + 1); // Trim trailing spaces
                    amountStr.erase(0, amountStr.find_first_not_of(" \n\r\t")); // Trim leading spaces

                    // Debug: Print extracted Amount
                    std::cout << "Extracted Amount: " << amountStr << std::endl;

                    double transactionAmount = std::stod(amountStr); // Convert to double

                    // Parse Transaction Type
                    std::string typeStr = line.substr(typePos + 5);
                    typeStr.erase(typeStr.find_last_not_of(" \n\r\t") + 1); // Trim trailing spaces
                    typeStr.erase(0, typeStr.find_first_not_of(" \n\r\t")); // Trim leading spaces

                    // Debug: Print extracted Type
                    std::cout << "Extracted Type: " << typeStr << std::endl;

                    char transactionType = (typeStr == "Debit") ? 'D' : (typeStr == "Credit" ? 'C' : '\0');

                    // Validate Transaction Type
                    if (transactionType == '\0') {
                        throw std::invalid_argument("Invalid transaction type: " + typeStr);
                    }

                    // Debug: Print parsed transaction details
                    std::cout << "Transaction ID: " << transactionID
                              << ", Amount: " << transactionAmount
                              << ", Type: " << transactionType << std::endl;

                    // Add transaction to the account
                    Account *account = searchAccount(138);
                    if (account) {
                        Transaction transaction(transactionAmount, transactionType);
                        transaction.setTransactionID(transactionID);
                        account->addTransaction(transaction.getAmount(), transaction.getDebitOrCredit());
                        std::cout << "Transaction added to account: " << accountNumber << std::endl;
                    } else {
                        std::cerr << "Account not found for transaction: " << line << std::endl;
                    }
                } else {
                    std::cerr << "Malformed transaction line: " << line << std::endl;
                }
            } catch (const std::exception &e) {
                std::cerr << "Error parsing transaction for account " << accountNumber << ": " << e.what() << std::endl;
            }
        }

        else if (readingDescription) { // Append additional description lines
            description += " " + line;
        }
    }

    // Add the last account if still reading
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
    // Search for the account by its number
    Account *account = searchAccount(accountNumber);
    if (!account) {
        throw std::invalid_argument("Account not found");
    }

    // Delegate the transaction details to the account's addTransaction method
    account->addTransaction(transaction.getAmount(), transaction.getDebitOrCredit());
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

    // Print account details with indentation
    file << std::string(indent * 2, ' ')
         << account->getAccountNumber() << " "
         << std::setw(30) << std::left << account->getDescription() << " "
         << std::fixed << std::setprecision(2) << account->getBalance() << "\n";

    // Print transactions for this account
    for (const Transaction *transaction: account->getTransactions()) {
        file << std::string((indent + 1) * 2, ' ')  // Indent transactions more than account
             << "Transaction ID: " << transaction->getTransactionID() << ", "
             << "Amount: " << transaction->getAmount() << ", "
             << "Type: " << (transaction->getDebitOrCredit() == 'D' ? "Debit" : "Credit") << "\n";

    }

    // Recursively print child accounts
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
