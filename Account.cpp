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

void Account::addTransaction(Transaction* transaction) {
    if (!transaction->isValid(this)) {
        throw invalid_argument("Transaction is invalid: Insufficient balance for credit transaction.");
    }

    // Add the transaction to this account
    transactions.push_back(transaction);

    // Calculate the adjustment amount
    double adjustment = (transaction->getDebitOrCredit() == 'D') ? transaction->getAmount() : -transaction->getAmount();

    // Update the balance of this account and propagate to parents
    Account* current = this;
    while (current) {
        current->updateBalance(adjustment);
        current = current->getParent();
    }
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

ostream& operator<<(ostream& out, const Account& account) {
    out << setw(6) << account.accountNumber << " "
        << setw(30) << account.description.substr(0, 30) << " "
        << fixed << setprecision(2) << account.balance << "\n";

    for (const Transaction* transaction : account.transactions) {
        out << "  " << *transaction << "\n";
    }
    return out;
}
// Copy Constructor
Account::Account(const Account& other)
        : accountNumber(other.accountNumber),
          description(other.description),
          balance(other.balance),
          parent(other.parent)
{
    for (const auto& t : other.transactions) {
        transactions.push_back(new Transaction(*t)); // Deep copy of transactions
    }
}

// Assignment Operator
Account& Account::operator=(const Account& other) {
    if (this != &other) {
        accountNumber = other.accountNumber;
        description = other.description;
        balance = other.balance;
        parent = other.parent;

        // Clear old transactions
        for (auto t : transactions) {
            delete t;
        }
        transactions.clear();

        // Deep copy transactions from the other account
        for (const auto& t : other.transactions) {
            transactions.push_back(new Transaction(*t));
        }
    }
    return *this;
}
void Account::saveToFile(const string& filename) const {
    ofstream file(filename, ios::app);
    if (!file) {
        throw runtime_error("Failed to open file for saving.");
    }

    file << setw(6) << accountNumber << " "
         << std::quoted(description) << " "
         << fixed << setprecision(2) << balance << "\n";

    for (const Transaction* transaction : transactions) {
        file << "  " << *transaction << "\n";
    }
}