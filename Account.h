#ifndef ACCOUNT_H
#define ACCOUNT_H

/**-- Account.h --------------------------------------------------------------
  This header file defines an Account class for representing financial
  accounts, including support for transactions and hierarchical parent-child
  relationships between accounts.

  Basic operations:
    Constructor:         Constructs an Account with a unique account number,
                         description, and optional initial balance.
    Copy Constructor:    Creates a deep copy of an existing Account object.
    Assignment Operator: Assigns one Account object to another, performing
                         a deep copy.
    Destructor:          Cleans up memory used by the Account and its transactions.
    Getters:             Provides access to account attributes, such as
                         account number, description, balance, parent account,
                         and associated transactions.
    Setters:             Allows modification of account relationships (e.g., parent).
    updateBalance:       Updates the account's balance by a specified amount.
    addTransaction:      Adds a new transaction to the account and propagates
                         balance adjustments to parent accounts.
    removeTransaction:   Removes a transaction from the account by ID and
                         adjusts balances for the account and parent accounts.
    Overloaded Operators: Implements input and output stream operations for Accounts.
    saveToFile:          Saves account details to a file.

  Helper functions:
    validateAccountNumber: Validates the account number format.

  Class Invariant:
    1. Each account has a unique account number.
    2. Transactions are stored as pointers to allow dynamic allocation.
    3. The parent pointer is either null or points to a valid Account object.
    4. The balance reflects the sum of the initial balance and all transaction amounts.
    5. The nextTransactionID ensures all transactions for an account have unique IDs.
----------------------------------------------------------------------------**/

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Transaction; // Forward declaration

class Account {
private:
    int accountNumber;                    // Unique account number
    string description;                   // Account description
    double balance;                       // Current account balance
    vector<Transaction *> transactions;   // List of transactions for the account
    Account *parent;                      // Pointer to the parent account (if any)
    int nextTransactionID;                // Tracks the next transaction ID for this account

    /***** Helper Function *****/
    /*------------------------------------------------------------------------
      Validates the account number to ensure it meets predefined criteria.

      Precondition:  A valid integer account number.
      Post-condition: Throws an exception if the account number is invalid.
    -----------------------------------------------------------------------*/
    void validateAccountNumber(int accountNumber);

public:
    /***** Constructor *****/
    /*------------------------------------------------------------------------
      Constructs an Account object with a unique account number, description,
      and an optional initial balance.

      Precondition:  A unique account number and description are provided.
                     Initial balance defaults to 0.0 if not specified.
      Post-condition: An Account object is created with the specified attributes.
    -----------------------------------------------------------------------*/
    Account(int accountNumber, const string &description, double initialBalance = 0.0);

    /***** Copy Constructor *****/
    /*------------------------------------------------------------------------
      Creates a deep copy of an existing Account object, including transactions.

      Precondition:  An Account object is provided.
      Post-condition: A new Account object is created as a copy of the given Account,
                      with a deep copy of its transactions.
    -----------------------------------------------------------------------*/
    Account(const Account &other);

    /***** Assignment Operator *****/
    /*------------------------------------------------------------------------
      Assigns one Account object to another, performing a deep copy of attributes
      and transactions.

      Precondition:  An existing Account object is provided.
      Post-condition: The current Account object is updated with the values
                      from the provided Account.
    -----------------------------------------------------------------------*/
    Account &operator=(const Account &other);

    /***** Destructor *****/
    /*------------------------------------------------------------------------
      Cleans up memory used by the Account and its associated transactions.

      Precondition:  None.
      Post-condition: The Account object and its associated transactions are deleted.
    -----------------------------------------------------------------------*/
    ~Account();

    /***** Getters *****/
    /*------------------------------------------------------------------------
      Provides access to the attributes of the Account.

      Precondition:  None.
      Post-condition: Returns the requested attribute's value.
    -----------------------------------------------------------------------*/
    int getAccountNumber() const;

    const string &getDescription() const;

    double getBalance() const;

    Account *getParent() const;

    const vector<Transaction *> &getTransactions() const;

    /***** Setters *****/
    /*------------------------------------------------------------------------
      Allows modification of the parent account relationship.

      Precondition:  A valid pointer to an Account object or nullptr.
      Post-condition: Updates the parent account of the current Account.
    -----------------------------------------------------------------------*/
    void setParent(Account *parentAccount);

    /***** Update Balance *****/
    /*------------------------------------------------------------------------
      Updates the account balance by adding or subtracting a specified amount.

      Precondition:  A valid numeric amount (positive or negative).
      Post-condition: The account balance is updated accordingly.
    -----------------------------------------------------------------------*/
    void updateBalance(double amount);

    /***** Transaction Management *****/
    /*------------------------------------------------------------------------
      Adds a transaction to the account and propagates balance adjustments
      to this account and its parent accounts.

      Precondition:  A valid numeric amount and transaction type ('D' or 'C') are provided.
      Post-condition: The transaction is added to the account's transaction list,
                      and balances are adjusted accordingly.
    -----------------------------------------------------------------------*/
    void addTransaction(double amount, char debitOrCredit);

    /*------------------------------------------------------------------------
      Removes a transaction from the account by its ID and adjusts balances
      for this account and parent accounts.

      Precondition:  A valid transaction ID is provided.
      Post-condition: The transaction is removed from the account, and balances
                      are adjusted accordingly.
    -----------------------------------------------------------------------*/
    void removeTransaction(int transactionID);

    /***** Overloaded Input and Output Operators *****/
    /*------------------------------------------------------------------------
      Implements input and output stream operations for Account objects.

      Precondition:  A valid input or output stream is provided.
      Post-condition: Account data is read from or written to the stream.

      Note: Input requires account number, description, and initial balance.
    -----------------------------------------------------------------------*/
    friend istream &operator>>(istream &in, Account &account);

    friend ostream &operator<<(ostream &out, const Account &account);

};

#endif // ACCOUNT_H
