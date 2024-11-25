#ifndef TRANSACTION_H
#define TRANSACTION_H

/**-- Transaction.h ----------------------------------------------------------
  This header file defines a Transaction class for representing financial
  transactions associated with accounts. Each transaction includes details
  about the amount, type (debit or credit), and optionally, related account information.

  Basic operations:
    Constructor:          Constructs a Transaction object with the specified
                          amount and type.
    Getters:              Provides access to transaction attributes such as
                          transaction ID, amount, and type.
    Setters:              Allows modification of transaction attributes.
    setTransactionID:     Assigns a transaction ID (used by accounts to assign
                          sequential IDs to their transactions).
    applyTransaction:     Applies the transaction to a given account and its
                          parent accounts.
    isValid:              Validates the feasibility of applying the transaction
                          to a specific account.
    Overloaded Operators: Implements input and output stream operations for
                          Transaction objects.

  Class Invariant:
    1. Each transaction has a unique transaction ID, assigned by the account.
    2. The transaction type is represented by 'D' (Debit) or 'C' (Credit).
    3. The transaction amount must be a valid numeric value.
----------------------------------------------------------------------------**/

#include <iostream>
#include <string>

using namespace std;

class Account; // Forward declaration

class Transaction {
private:
    int transactionID;          // Unique identifier for the transaction (assigned by the account)
    double amount;              // Transaction amount
    char debitOrCredit;         // 'D' for Debit, 'C' for Credit
    string relatedAccount;      // Account this transaction is related to (optional)

public:
    /***** Constructor *****/
    /*------------------------------------------------------------------------
      Constructs a Transaction object with the specified amount and type.

      Precondition:  A valid amount and transaction type ('D' or 'C') are provided.
      Post-condition: A Transaction object is created with the specified attributes.
                      The transaction ID is set separately by the account.
    -----------------------------------------------------------------------*/
    Transaction(double amt, char dc);

    /***** Getters *****/
    /*------------------------------------------------------------------------
      Provides access to the transaction attributes.

      Precondition:  None.
      Post-condition: Returns the requested attribute's value.
    -----------------------------------------------------------------------*/
    int getTransactionID() const;

    double getAmount() const;

    char getDebitOrCredit() const;

    /***** Transaction ID Management *****/
    /*------------------------------------------------------------------------
      Sets the transaction ID for the transaction. This allows accounts to
      assign sequential IDs to their transactions.

      Precondition:  A valid transaction ID is provided.
      Post-condition: The transaction ID is updated.
    -----------------------------------------------------------------------*/
    void setTransactionID(int id);

    /***** Apply Transaction *****/
    /*------------------------------------------------------------------------
      Applies the transaction to a specified account and its parent accounts.

      Precondition:  A valid pointer to an Account object is provided.
      Post-condition: The transaction amount is added or subtracted from
                      the account's balance, depending on the type.
    -----------------------------------------------------------------------*/
    void applyTransaction(Account *account) const;

    /***** Validate Transaction *****/
    /*------------------------------------------------------------------------
      Validates whether the transaction can be applied to the specified account.

      Precondition:  A valid pointer to an Account object is provided.
      Post-condition: Returns true if the transaction is feasible; otherwise,
                      returns false (e.g., for Credit transactions with insufficient balance).
    -----------------------------------------------------------------------*/
    bool isValid(const Account *account) const;

    /***** Overloaded Operators *****/
    /*------------------------------------------------------------------------
      Implements input and output stream operations for Transaction objects.

      Precondition:  A valid input or output stream is provided.
      Post-condition: Transaction data is read from or written to the stream.
    -----------------------------------------------------------------------*/
    friend istream &operator>>(istream &in, Transaction &transaction);

    friend ostream &operator<<(ostream &out, const Transaction &transaction);
};

#endif // TRANSACTION_H
