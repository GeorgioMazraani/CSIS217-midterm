#include "Transaction.h"
#include "Account.h"

using namespace std;

int Transaction::nextTransactionID = 1;

// Constructor: Initializes a Transaction object with auto-incrementing ID, amount, type, and related account
Transaction::Transaction(double amt, char dc, const string &account)
        : transactionID(nextTransactionID++), amount(amt), debitOrCredit(dc), relatedAccount(account) {
    // Validate the transaction type
    if (dc != 'D' && dc != 'C') {
        throw invalid_argument("Invalid transaction type. Use 'D' for Debit or 'C' for Credit.");
    }
}

// Returns the transaction ID
int Transaction::getTransactionID() const {
    return transactionID;
}

// Returns the transaction amount
double Transaction::getAmount() const {
    return amount;
}

// Returns the transaction type ('D' for Debit, 'C' for Credit)
char Transaction::getDebitOrCredit() const {
    return debitOrCredit;
}

// Returns the related account associated with this transaction
const string &Transaction::getRelatedAccount() const {
    return relatedAccount;
}

// Sets the transaction amount
void Transaction::setAmount(double amt) {
    amount = amt;
}

// Sets the transaction type ('D' or 'C')
void Transaction::setDebitOrCredit(char dc) {
    debitOrCredit = dc;
}

// Sets the related account for the transaction
void Transaction::setRelatedAccount(const string &account) {
    relatedAccount = account;
}

// Applies the transaction to the given account and its parent accounts
void Transaction::applyTransaction(Account *account) const {
    if (!account) return;

    // Adjust the balance based on transaction type
    if (debitOrCredit == 'D') {
        account->updateBalance(amount); // Add for Debit
    } else if (debitOrCredit == 'C') {
        account->updateBalance(-amount); // Subtract for Credit
    }

    // Recursively apply the transaction to the parent account
    applyTransaction(account->getParent());
}

// Checks if the transaction is valid for the given account
bool Transaction::isValid(const Account *account) const {
    // Ensure that a Credit transaction doesn't result in a negative balance
    if (debitOrCredit == 'C' && account->getBalance() < amount) {
        return false;
    }
    return true;
}

// Overloaded input operator: Reads transaction details from the input stream
istream &operator>>(istream &in, Transaction &transaction) {
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

// Overloaded output operator: Writes transaction details to the output stream
ostream &operator<<(ostream &out, const Transaction &transaction) {
    out << "Transaction ID: " << transaction.transactionID << "\n"
        << "Amount: " << transaction.amount << "\n"
        << "Type: " << (transaction.debitOrCredit == 'D' ? "Debit" : "Credit") << "\n"
        << "Related Account: " << transaction.relatedAccount;
    return out;
}
