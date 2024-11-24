#ifndef TRANSACTION_H
#define TRANSACTION_H

/**-- Transaction.h ----------------------------------------------------------
  This header file defines a Transaction class for representing financial
  transactions associated with accounts. Each transaction includes details
  about the amount, type (debit or credit), and related account information.

  Basic operations:
    Constructor:          Constructs a Transaction object with a unique ID,
                          amount, type, and related account.
    Getters:              Provides access to transaction attributes such as
                          transaction ID, amount, type, and related account.
    Setters:              Allows modification of transaction attributes.
    applyTransaction:     Applies the transaction to a given account and its
                          parent accounts.
    isValid:              Validates the feasibility of applying the transaction
                          to a specific account.
    Overloaded Operators: Implements input and output stream operations for
                          Transaction objects.

  Class Invariant:
    1. Each transaction has a unique transaction ID.
    2. The transaction type is represented by 'D' (Debit) or 'C' (Credit).
    3. The related account is represented by its name or identifier as a string.
----------------------------------------------------------------------------**/

#include <iostream>
#include <string>

using namespace std;

class Account; // Forward declaration

class Transaction {
private:
    static int nextTransactionID; // Static variable to track the next transaction ID
    int transactionID;          // Unique identifier for the transaction
    double amount;              // Transaction amount
    char debitOrCredit;         // 'D' for Debit, 'C' for Credit
    string relatedAccount;      // Account this transaction is related to

public:
    /***** Constructor *****/
    /*------------------------------------------------------------------------
      Constructs a Transaction object with the specified attributes.

      Precondition:  A valid ID, amount, transaction type ('D' or 'C'), and
                     related account are provided.
      Post-condition: A Transaction object is created with the specified
                      attributes.
    -----------------------------------------------------------------------*/

    Transaction(double amt, char dc, const std::string &account);

    /***** Getters *****/
    /*------------------------------------------------------------------------
      Provides access to the transaction attributes.

      Precondition:  None.
      Post-condition: Returns the requested attribute's value.
    -----------------------------------------------------------------------*/
    int getTransactionID() const;

    double getAmount() const;

    char getDebitOrCredit() const;

    const string &getRelatedAccount() const;

    /***** Setters *****/
    /*------------------------------------------------------------------------
      Allows modification of the transaction attributes.

      Precondition:  Valid values are provided for the attributes.
      Post-condition: The specified attribute is updated with the new value.
    -----------------------------------------------------------------------*/
    void setAmount(double amt);

    void setDebitOrCredit(char dc);

    void setRelatedAccount(const string &account);

    /***** Apply Transaction *****/
    /*------------------------------------------------------------------------
      Applies the transaction to a specified account and its parent accounts.

      Precondition:  A valid pointer to an Account object.
      Post-condition: The transaction amount is added or subtracted from
                      the account's balance, depending on the type.
    -----------------------------------------------------------------------*/
    void applyTransaction(Account *account) const;

    /***** Validate Transaction *****/
    /*------------------------------------------------------------------------
      Validates whether the transaction can be applied to the specified account.

      Precondition:  A valid pointer to an Account object.
      Post-condition: Returns true if the transaction is feasible; otherwise,
                      returns false.
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
