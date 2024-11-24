#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <iostream>
#include <string>

using namespace std;

class Account; // Forward declaration

class Transaction {
private:
    int transactionID;          // Unique identifier for the transaction
    double amount;              // Transaction amount
    char debitOrCredit;         // 'D' for Debit, 'C' for Credit
    string relatedAccount;      // Account this transaction is related to

public:
    // Constructor
    Transaction(int id, double amt, char dc, const string& account);

    // Getters
    int getTransactionID() const;
    double getAmount() const;
    char getDebitOrCredit() const;
    const string& getRelatedAccount() const;

    // Setters
    void setAmount(double amt);
    void setDebitOrCredit(char dc);
    void setRelatedAccount(const string& account);

    // Apply transaction to an account and its parents
    void applyTransaction(Account* account) const;

    // Validate transaction feasibility
    bool isValid(const Account* account) const;

    // Overloaded operators
    friend istream& operator>>(istream& in, Transaction& transaction);
    friend ostream& operator<<(ostream& out, const Transaction& transaction);
};

#endif // TRANSACTION_H