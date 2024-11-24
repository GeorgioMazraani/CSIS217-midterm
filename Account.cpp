#include "Account.h"
#include "Transaction.h"
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

// Constructor: Initializes an Account with account number, description, and initial balance
Account::Account(int accountNumber, const string &description, double initialBalance)
        : accountNumber(accountNumber), description(description), balance(initialBalance), parent(nullptr) {
    validateAccountNumber(accountNumber); // Ensure the account number is valid
}

// Destructor: Releases memory for all transactions associated with this account
Account::~Account() {
    for (Transaction *transaction : transactions) {
        delete transaction;
    }
}

// Validates the account number to ensure it is within the acceptable range
void Account::validateAccountNumber(int accountNumber) {
    if (accountNumber <= 0 || accountNumber > 999999) {
        throw invalid_argument("Account number must be between 1 and 99999.");
    }
}

// Returns the account number
int Account::getAccountNumber() const {
    return accountNumber;
}

// Returns the account description
const string &Account::getDescription() const {
    return description;
}

// Returns the account balance
double Account::getBalance() const {
    return balance;
}

// Returns the parent account, if any
Account *Account::getParent() const {
    return parent;
}

// Returns a reference to the list of transactions associated with this account
const vector<Transaction *> &Account::getTransactions() const {
    return transactions;
}

// Sets the parent account for this account
void Account::setParent(Account *parentAccount) {
    parent = parentAccount;
}

// Updates the balance by adding the specified amount
void Account::updateBalance(double amount) {
    balance += amount;
}

// Adds a transaction to the account and updates balances for this account and its parent accounts
void Account::addTransaction(double amount, char debitOrCredit, const std::string &relatedAccount) {
    // Validate the transaction type
    if (debitOrCredit != 'D' && debitOrCredit != 'C') {
        throw std::invalid_argument("Invalid transaction type. Use 'D' for Debit or 'C' for Credit.");
    }

    // Create a new transaction with auto-incrementing ID
    Transaction *transaction = new Transaction(amount, debitOrCredit, relatedAccount);

    // Validate the transaction
    if (!transaction->isValid(this)) {
        delete transaction; // Clean up if invalid
        throw std::invalid_argument("Transaction is invalid: Insufficient balance for credit transaction.");
    }

    // Add the transaction to the list
    transactions.push_back(transaction);

    // Calculate the adjustment based on the transaction type
    double adjustment = (debitOrCredit == 'D') ? amount : -amount;

    // Propagate the balance adjustment to this account and its parent accounts
    Account *current = this;
    while (current) {
        current->updateBalance(adjustment);
        current = current->getParent();
    }
}


// Removes a transaction by its ID and adjusts balances accordingly
void Account::removeTransaction(int transactionID) {
    auto it = find_if(transactions.begin(), transactions.end(),
                      [transactionID](Transaction *t) { return t->getTransactionID() == transactionID; });

    if (it != transactions.end()) {
        // Reverse the balance adjustment caused by this transaction
        double adjustment = ((*it)->getDebitOrCredit() == 'D') ? -(*it)->getAmount() : (*it)->getAmount();
        updateBalance(adjustment);

        // Propagate the reverse adjustment to parent accounts
        Account *current = parent;
        while (current) {
            current->updateBalance(adjustment);
            current = current->getParent();
        }

        // Delete the transaction and remove it from the list
        delete *it;
        transactions.erase(it);
    } else {
        throw invalid_argument("Transaction not found.");
    }
}

// Overloaded input operator: Reads account details from the input stream
istream &operator>>(istream &in, Account &account) {
    cout << "Enter Account Number: ";
    in >> account.accountNumber;
    cout << "Enter Description: ";
    in.ignore(); // Ignore any trailing newline
    getline(in, account.description);
    cout << "Enter Initial Balance: ";
    in >> account.balance;
    return in;
}

// Overloaded output operator: Writes account details to the output stream
// Overloaded output operator: Writes account details to the output stream
ostream &operator<<(ostream &out, const Account &account) {
    // Format the account number and description
    out << account.accountNumber << " ";

    // Ensure the description is padded or truncated to a fixed width
    std::string description = account.description.substr(0, 80); // Truncate if too long
    if (description.length() < 80) {  // Pad with spaces if too short
        description.append(80 - description.length(), ' ');
    }
    out << description;

    // Format the balance with fixed precision
    out << " " << std::fixed << std::setprecision(2) << account.balance << "\n";

    // Output the transactions
    for (const Transaction *transaction : account.transactions) {
        out << "\t" << *transaction << "\n"; // Assuming Transaction has operator<< defined
    }

    return out;
}



// Copy constructor: Creates a deep copy of the given Account object
Account::Account(const Account &other)
        : accountNumber(other.accountNumber),
          description(other.description),
          balance(other.balance),
          parent(other.parent) {
    for (const auto &t : other.transactions) {
        transactions.push_back(new Transaction(*t)); // Deep copy of transactions
    }
}

// Assignment operator: Assigns the content of one Account object to another
Account &Account::operator=(const Account &other) {
    if (this != &other) {
        accountNumber = other.accountNumber;
        description = other.description;
        balance = other.balance;
        parent = other.parent;

        // Clear existing transactions
        for (auto t : transactions) {
            delete t;
        }
        transactions.clear();

        // Deep copy transactions from the other account
        for (const auto &t : other.transactions) {
            transactions.push_back(new Transaction(*t));
        }
    }
    return *this;
}

// Saves the account details and its transactions to a file
void Account::saveToFile(const string &filename) const {
    ofstream file(filename, ios::app); // Open the file in append mode
    if (!file) {
        throw runtime_error("Failed to open file for saving.");
    }

    file << setw(6) << accountNumber << " "
         << std::quoted(description) << " "
         << fixed << setprecision(2) << balance << "\n";

    for (const Transaction *transaction : transactions) {
        file << "  " << *transaction << "\n";
    }
}
