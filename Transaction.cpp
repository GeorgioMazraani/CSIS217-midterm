#include "Transaction.h"
#include "Account.h"

using namespace std;

// Constructor: Initializes a Transaction object with amount, type, and related account
Transaction::Transaction(double amt, char dc)
        : transactionID(0), amount(amt), debitOrCredit(dc) {
    // Validate the transaction type
    if (dc != 'D' && dc != 'C') {
        throw invalid_argument("Invalid transaction type. Use 'D' for Debit or 'C' for Credit.");
    }
}

// Returns the transaction ID
int Transaction::getTransactionID() const {
    return transactionID;
}

// Sets the transaction ID (used to assign IDs specific to each account)
void Transaction::setTransactionID(int id) {
    transactionID = id;
}

// Returns the transaction amount
double Transaction::getAmount() const {
    return amount;
}

// Returns the transaction type ('D' for Debit, 'C' for Credit)
char Transaction::getDebitOrCredit() const {
    return debitOrCredit;
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
    out <<"\n"<< "- Transaction ID: " << transaction.transactionID << "\n"
        << "- Amount: " << transaction.amount << "\n"
        << "- Type: " << (transaction.debitOrCredit == 'D' ? "Debit" : "Credit");
    return out;
}
