#include "ForestTree.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

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

// Determines the parent account number based on the current account number
int ForestTree::findParentNumber(int accountNumber) const {
    string numStr = to_string(accountNumber);

    // If the account number is single-digit or invalid, return 0 (no parent)
    if (numStr.length() <= 1) {
        return 0;
    }

    // For five-digit accounts, trim one digit to find the parent
    if (numStr.length() == 5) {
        return stoi(numStr.substr(0, 4));
    }

    // For other lengths, trim the last digit
    return stoi(numStr.substr(0, numStr.length() - 1));
}


// Cleans account description by removing unwanted characters and trimming spaces
string ForestTree::cleanDescription(const string &desc) const {
    if (desc.empty()) {
        return "";
    }

    string cleaned = desc;

    // Trim leading whitespace
    size_t start = cleaned.find_first_not_of(" \t");
    if (start != string::npos) {
        cleaned = cleaned.substr(start);
    }

    // Trim trailing whitespace
    size_t end = cleaned.find_last_not_of(" \t");
    if (end != string::npos) {
        cleaned = cleaned.substr(0, end + 1);
    }

    return cleaned;
}


// Builds the chart of accounts by reading from a file
void ForestTree::buildFromFile(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + filename);
    }

    string line, description;
    int accountNumber = 0;
    double balance = 0.0;
    bool readingDescription = false;

    while (getline(file, line)) {
        if (line.empty()) continue; // Skip empty lines

        istringstream iss(line);
        string firstToken;
        iss >> firstToken;

        if (isdigit(firstToken[0])) { // Starts with a digit (account number)
            if (readingDescription) { // Finalize the previous account
                description = cleanDescription(description);
                try {
                    if (accountMap.find(accountNumber) == accountMap.end()) {
                        addAccount(accountNumber, description, balance);
                    }
                } catch (const exception &e) {
                    cerr << "Error adding account " << accountNumber << ": " << e.what() << endl;
                }
                description.clear();
            }

            // Parse account number and description/balance
            try {
                accountNumber = stoi(firstToken);
            } catch (const exception &) {
                cerr << "Error parsing account number: " << line << endl;
                continue;
            }

            // Extract the remaining line for description/balance
            getline(iss, line);
            size_t lastSpace = line.find_last_of(' ');

            if (lastSpace != string::npos) {
                string potentialBalance = line.substr(lastSpace + 1);
                try {
                    balance = stod(potentialBalance); // Parse balance
                    description = line.substr(0, lastSpace);
                } catch (const exception &) {
                    description = line; // Treat entire line as description
                    balance = 0.0;
                }
            } else {
                description = line;
                balance = 0.0;
            }

            readingDescription = true;
        } else if (line.find("Transaction ID:") != string::npos) { // Transaction lines
            while (line.find("Transaction ID:") != string::npos) {
                try {
                    // Extract transaction details
                    size_t idPos = line.find("Transaction ID:");
                    size_t amountPos = line.find("Amount:");
                    size_t typePos = line.find("Type:");

                    if (idPos != string::npos && amountPos != string::npos && typePos != string::npos) {
                        string idStr = line.substr(idPos + 14, amountPos - (idPos + 14));
                        idStr.erase(idStr.find_last_not_of(" ,:\n\r\t") + 1); // Trim trailing characters
                        idStr.erase(0, idStr.find_first_not_of(" ,:\n\r\t")); // Trim leading characters

                        int transactionID = stoi(idStr); // Convert to integer

                        string amountStr = line.substr(amountPos + 7, typePos - (amountPos + 7));
                        amountStr.erase(amountStr.find_last_not_of(" \n\r\t") + 1); // Trim trailing spaces
                        amountStr.erase(0, amountStr.find_first_not_of(" \n\r\t")); // Trim leading spaces

                        double transactionAmount = stod(amountStr); // Convert to double

                        string typeStr = line.substr(typePos + 5);
                        typeStr.erase(typeStr.find_last_not_of(" \n\r\t") + 1); // Trim trailing spaces
                        typeStr.erase(0, typeStr.find_first_not_of(" \n\r\t")); // Trim leading spaces

                        char transactionType = (typeStr == "Debit") ? 'D' : (typeStr == "Credit" ? 'C' : '\0');

                        if (transactionType == '\0') {
                            throw invalid_argument("Invalid transaction type: " + typeStr);
                        }

                        // Ensure the account exists before adding the transaction
                        Account *account = searchAccount(accountNumber);
                        if (!account) {
                            addAccount(accountNumber, description, 0);
                            account = searchAccount(accountNumber);
                        }

                        // Add the transaction
                        Transaction transaction(transactionAmount, transactionType);
                        transaction.setTransactionID(transactionID);
                        account->addTransaction(transaction.getAmount(), transaction.getDebitOrCredit());
                    } else {
                        cerr << "Malformed transaction line: " << line << endl;
                    }
                } catch (const exception &e) {
                    cerr << "Error parsing transaction for account " << accountNumber << ": " << e.what() << endl;
                }

                if (!getline(file, line)) break; // Read the next line if available
            }

            // Check if the next line starts a new account
            if (!line.empty() && isdigit(line[0])) {
                continue; // Process the next account line in the main loop
            }
        } else if (readingDescription) { // Append additional description lines
            description += " " + line;
        }
    }

    // Add the last account if still reading
    if (readingDescription) {
        description = cleanDescription(description);
        try {
            if (accountMap.find(accountNumber) == accountMap.end()) {
                addAccount(accountNumber, description, balance);
            }
        } catch (const exception &e) {
            cerr << "Error adding account " << accountNumber << ": " << e.what() << endl;
        }
    }
}


// Adds an account to the ForestTree
void ForestTree::addAccount(int accountNumber, const string &description, double initialBalance) {
    try {

        // Validate the account number range (1 to 5 digits)
        if (accountNumber < 1 || accountNumber > 99999) {
            throw invalid_argument("Invalid account number: " + to_string(accountNumber) +
                                   ". Account number must be between 1 and 5 digits.");
        }
        // Check if the account already exists
        if (accountMap.find(accountNumber) != accountMap.end()) {
            throw invalid_argument("Account number already exists: " + to_string(accountNumber));
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
    } catch (const exception &e) {
        // Catch any errors and rethrow to be handled in the calling function
        throw;
    }
}


// Removes an account by its number
void ForestTree::removeAccount(int accountNumber) {
    auto it = accountMap.find(accountNumber);
    if (it == accountMap.end()) {
        throw invalid_argument("Account not found");
    }

    delete it->second;
    accountMap.erase(it);
}

// Adds a transaction to an account
void ForestTree::addTransaction(int accountNumber, const Transaction &transaction) {
    // Search for the account by its number
    Account *account = searchAccount(accountNumber);
    if (!account) {
        throw invalid_argument("Account not found");
    }

    // Delegate the transaction details to the account's addTransaction method
    account->addTransaction(transaction.getAmount(), transaction.getDebitOrCredit());
}


// Removes a transaction from an account
void ForestTree::removeTransaction(int accountNumber, int transactionID) {
    Account *account = searchAccount(accountNumber);
    if (!account) {
        throw invalid_argument("Account not found");
    }

    try {
        account->removeTransaction(transactionID);
    } catch (const exception &e) {
        throw invalid_argument("Transaction not found for the given account.");
    }
}


// Searches for an account by its number
Account *ForestTree::searchAccount(int accountNumber) {
    auto it = accountMap.find(accountNumber);
    return (it != accountMap.end()) ? it->second : nullptr;
}

// Prints the entire ForestTree to a file
void ForestTree::printTree(const string &filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open file for writing: " + filename);
    }

    for (const auto &pair: accountMap) {
        if (!pair.second->getParent()) {
            printTreeRecursive(pair.second, file, 0);
        }
    }
}

void ForestTree::printTreeRecursive(Account *account, ofstream &file, int indent) {
    if (!account) return;

    // Print account details with indentation
    file << string(indent * 2, ' ')
         << account->getAccountNumber() << " "
         << setw(30) << left << account->getDescription() << " "
         << fixed << setprecision(2) << account->getBalance() << "\n";

    // Print transactions for this account
    for (const Transaction *transaction: account->getTransactions()) {
        file << string((indent + 1) * 2, ' ')  // Indent transactions more than account
             << "Transaction ID: " << transaction->getTransactionID() << ", "
             << "Amount: " << transaction->getAmount() << ", "
             << "Type: " << (transaction->getDebitOrCredit() == 'D' ? "Debit" : "Credit") << "\n";
    }

    // Add a blank line after transactions for better readability
    if (!account->getTransactions().empty()) {
        file << "\n";
    }

    // Recursively print child accounts
    for (const auto &pair: accountMap) {
        if (pair.second->getParent() == account) {
            printTreeRecursive(pair.second, file, indent + 1);
        }
    }
}



