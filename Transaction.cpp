#include "Transaction.h"
#include "Account.h"
using namespace std;


// Constructor
Transaction::Transaction(int id, double amt, char dc, const string& account)
        : transactionID(id), amount(amt), debitOrCredit(dc), relatedAccount(account) {
    if (dc != 'D' && dc != 'C') {
        throw invalid_argument("Invalid transaction type. Use 'D' for Debit or 'C' for Credit.");
    }
}

// Getters
int Transaction::getTransactionID() const { return transactionID; }
double Transaction::getAmount() const { return amount; }
char Transaction::getDebitOrCredit() const { return debitOrCredit; }
const string& Transaction::getRelatedAccount() const { return relatedAccount; }

// Setters
void Transaction::setAmount(double amt) { amount = amt; }
void Transaction::setDebitOrCredit(char dc) { debitOrCredit = dc; }
void Transaction::setRelatedAccount(const string& account) { relatedAccount = account; }

// Apply transaction
void Transaction::applyTransaction(Account* account) const {
    if (!account) return;

    if (debitOrCredit == 'D') {
        account->updateBalance(amount);
    } else if (debitOrCredit == 'C') {
        account->updateBalance(-amount);
    }

    applyTransaction(account->getParent());
}

// Validate transaction
bool Transaction::isValid(const Account* account) const {
    if (debitOrCredit == 'C' && account->getBalance() < amount) {
        return false;
    }
    return true;
}

// Input and output operators
istream& operator>>(istream& in, Transaction& transaction) {
    cout << "Enter Transaction ID: ";
    in >> transaction.transactionID;
    cout << "Enter Amount: ";
    in >> transaction.amount;
    cout << "Enter Debit or Credit (D/C): ";
    in >> transaction.debitOrCredit;
    cout << "Enter Related Account: ";
    in >> transaction.relatedAccount;
    return in;
}

ostream& operator<<(ostream& out, const Transaction& transaction) {
    out << "Transaction ID: " << transaction.transactionID << "\n"
        << "Amount: " << transaction.amount << "\n"
        << "Type: " << (transaction.debitOrCredit == 'D' ? "Debit" : "Credit") << "\n"
        << "Related Account: " << transaction.relatedAccount;
    return out;
}
