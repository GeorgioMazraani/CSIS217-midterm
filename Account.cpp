#include "Account.h"
#include "Transaction.h"
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>


using namespace std;

// Constructor
Account::Account(int accountNumber, const string& description, double initialBalance)
        : accountNumber(accountNumber), description(description), balance(initialBalance), parent(nullptr) {
    validateAccountNumber(accountNumber);
}

// Destructor
Account::~Account() {
    for (Transaction* transaction : transactions) {
        delete transaction;
    }
}

// Validate account number
void Account::validateAccountNumber(int accountNumber) {
    if (accountNumber <= 0 || accountNumber > 99999) {
        throw invalid_argument("Account number must be between 1 and 99999.");
    }
}

// Getters
int Account::getAccountNumber() const { return accountNumber; }
const string& Account::getDescription() const { return description; }
double Account::getBalance() const { return balance; }
Account* Account::getParent() const { return parent; }
const vector<Transaction*>& Account::getTransactions() const { return transactions; }

// Setters
void Account::setParent(Account* parentAccount) { parent = parentAccount; }

// Update balance
void Account::updateBalance(double amount) { balance += amount; }

// Add a transaction
void Account::addTransaction(Transaction* transaction) {
    if (!transaction->isValid(this)) {
        throw invalid_argument("Transaction is invalid: Insufficient balance for credit transaction.");
    }
    transactions.push_back(transaction);
    transaction->applyTransaction(this); // Apply recursively to parent accounts
}

// Remove a transaction
void Account::removeTransaction(int transactionID) {
    auto it = find_if(transactions.begin(), transactions.end(),
                      [transactionID](Transaction* t) { return t->getTransactionID() == transactionID; });

    if (it != transactions.end()) {
        double adjustment = ((*it)->getDebitOrCredit() == 'D' ? -(*it)->getAmount() : (*it)->getAmount());
        updateBalance(adjustment);

        // Apply the adjustment recursively to parent accounts
        Account* current = parent;
        while (current) {
            current->updateBalance(adjustment);
            current = current->getParent();
        }

        delete *it;
        transactions.erase(it);
    } else {
        throw invalid_argument("Transaction not found.");
    }
}

// Print a detailed report
void Account::printReport() const {
    cout << "Account Number: " << accountNumber << "\n"
         << "Description: " << description << "\n"
         << "Balance: " << fixed << setprecision(2) << balance << "\n"
         << "Transactions:\n";

    for (const Transaction* transaction : transactions) {
        cout << *transaction << "\n";
    }
}

// Save account to a file
void Account::saveToFile(const string& filename) const {
    ofstream file(filename);
    if (!file) {
        throw runtime_error("Failed to open file for saving.");
    }

    file << accountNumber << "\n" << description << "\n" << balance << "\n";
    for (const Transaction* transaction : transactions) {
        file << *transaction << "\n";
    }
    file.close();
}

// Load account from a file
void Account::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file) {
        throw runtime_error("Failed to open file for loading.");
    }

    transactions.clear(); // Clear existing transactions

    file >> accountNumber;
    file.ignore();
    getline(file, description);
    file >> balance;

    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            stringstream ss(line);
            int id;
            double amt;
            char dc;
            string account;

            // Extract transaction details
            ss >> id >> amt >> dc >> account;
            Transaction* transaction = new Transaction(id, amt, dc, account);
            transactions.push_back(transaction);
        }
    }

    file.close();
}

// Overloaded input operator
istream& operator>>(istream& in, Account& account) {
    cout << "Enter Account Number: ";
    in >> account.accountNumber;
    cout << "Enter Description: ";
    in.ignore();
    getline(in, account.description);
    cout << "Enter Initial Balance: ";
    in >> account.balance;
    return in;
}

// Overloaded output operator
ostream& operator<<(ostream& out, const Account& account) {
    out << "Account Number: " << account.accountNumber << "\n"
        << "Description: " << account.description << "\n"
        << "Balance: " << fixed << setprecision(2) << account.balance << "\n"
        << "Transactions:\n";

    for (const Transaction* transaction : account.transactions) {
        out << *transaction << "\n";
    }
    return out;
}
