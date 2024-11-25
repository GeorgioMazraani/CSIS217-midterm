#ifndef FORESTTREE_H
#define FORESTTREE_H

/**-- ForestTree.h ----------------------------------------------------------
  This header file defines the ForestTree class for managing a hierarchical
  structure of accounts, where each account can contain multiple transactions.

  Basic operations:
    Constructor:         Constructs an empty ForestTree object.
    Destructor:          Cleans up memory used by the tree and its accounts.
    initialize:          Clears the tree, removing all accounts and transactions.
    buildFromFile:       Builds the ForestTree structure by reading from a file.
    addAccount:          Adds a new account to the tree.
    removeAccount:       Removes an account from the tree by its number.
    addTransaction:      Adds a transaction to a specific account.
    removeTransaction:   Removes a transaction from a specific account by ID.
    searchAccount:       Searches for an account in the tree by its number.
    printTree:           Prints the entire tree structure to a file.

  Helper functions:
    deleteTree:          Recursively deletes an account and its resources.
    findParentNumber:    Determines the parent account number based on the current account number.
    cleanDescription:    Cleans and formats account descriptions.
    printTreeRecursive:  Recursively prints the ForestTree structure to a file.

  Class Invariant:
    1. The `root` pointer is null if the tree is empty or points to the root of the account hierarchy.
    2. The `accountMap` provides a quick lookup for accounts using their unique numbers.
    3. Each account can contain multiple transactions and may optionally have a parent account.
--------------------------------------------------------------------------**/

#include <iostream>
#include <string>
#include <map>
#include "Account.h"
#include "Transaction.h"

using namespace std;

class ForestTree {
private:
    Account *root; // Root of the tree (or null if the tree is empty)
    map<int, Account *> accountMap; // Map for quick account lookup by account number

    /***** Helper Functions *****/
    /*------------------------------------------------------------------------
      Recursively deletes an account and its associated memory.

      Precondition:  A valid pointer to an account node is provided.
      Post-condition: The node and its associated memory are freed.
    -----------------------------------------------------------------------*/
    void deleteTree(Account *node);

    /*------------------------------------------------------------------------
      Determines the parent account number based on the current account number.

      Precondition:  A valid account number is provided.
      Post-condition: Returns the parent account number, or 0 if there is no parent.
    -----------------------------------------------------------------------*/
    int findParentNumber(int accountNumber) const;

    /*------------------------------------------------------------------------
      Cleans and formats an account description by removing unnecessary
      characters and trimming leading or trailing whitespace.

      Precondition:  A valid string reference is provided.
      Post-condition: Returns the cleaned and formatted string.
    -----------------------------------------------------------------------*/
    string cleanDescription(const string &desc) const;

    /*------------------------------------------------------------------------
      Recursively writes the ForestTree structure to a file.

      Precondition:  A valid account pointer, file stream, and indentation level are provided.
      Post-condition: The tree structure is recursively written to the file in a readable format.
    -----------------------------------------------------------------------*/
    void printTreeRecursive(Account *account, ofstream &file, int indent);

public:
    /***** Constructor *****/
    /*------------------------------------------------------------------------
      Constructs an empty ForestTree object.

      Precondition:  None.
      Post-condition: A ForestTree object is created with no accounts.
    -----------------------------------------------------------------------*/
    ForestTree();

    /***** Destructor *****/
    /*------------------------------------------------------------------------
      Cleans up memory used by the tree and its accounts.

      Precondition:  None.
      Post-condition: All accounts in the tree are deleted, and memory is freed.
    -----------------------------------------------------------------------*/
    ~ForestTree();

    /***** Initialize *****/
    /*------------------------------------------------------------------------
      Clears the tree, removing all accounts and transactions.

      Precondition:  None.
      Post-condition: The tree is empty, and root is reset to nullptr.
    -----------------------------------------------------------------------*/
    void initialize();

    /***** Build from File *****/
    /*------------------------------------------------------------------------
      Builds the ForestTree structure by reading account data from a file.

      Precondition:  A valid filename with properly formatted account and
                     transaction data is provided.
      Post-condition: The tree is populated with accounts and transactions based
                      on the file data.
    -----------------------------------------------------------------------*/
    void buildFromFile(const string &filename);

    /***** Add Account *****/
    /*------------------------------------------------------------------------
      Adds a new account to the ForestTree.

      Precondition:  A unique account number, description, and initial balance are provided.
      Post-condition: The new account is added to the tree, mapped in `accountMap`,
                      and associated with its parent account (if applicable).
    -----------------------------------------------------------------------*/
    void addAccount(int accountNumber, const string &description, double initialBalance);

    /***** Remove Account *****/
    /*------------------------------------------------------------------------
      Removes an account from the ForestTree by its account number.

      Precondition:  A valid account number is provided.
      Post-condition: The account is removed from the tree and `accountMap` if it exists.
    -----------------------------------------------------------------------*/
    void removeAccount(int accountNumber);

    /***** Add Transaction *****/
    /*------------------------------------------------------------------------
      Adds a transaction to a specific account in the ForestTree.

      Precondition:  A valid account number and a Transaction object are provided.
      Post-condition: The transaction is added to the specified account.
    -----------------------------------------------------------------------*/
    void addTransaction(int accountNumber, const Transaction &transaction);

    /***** Remove Transaction *****/
    /*------------------------------------------------------------------------
      Removes a transaction from a specific account by its transaction ID.

      Precondition:  A valid account number and transaction ID are provided.
      Post-condition: The transaction is removed from the account if it exists.
    -----------------------------------------------------------------------*/
    void removeTransaction(int accountNumber, int transactionID);

    /***** Search Account *****/
    /*------------------------------------------------------------------------
      Searches for an account in the ForestTree by its account number.

      Precondition:  A valid account number is provided.
      Post-condition: Returns a pointer to the account if found, or nullptr if not.
    -----------------------------------------------------------------------*/
    Account *searchAccount(int accountNumber);

    /***** Print Tree *****/
    /*------------------------------------------------------------------------
      Prints the entire ForestTree structure to a file.

      Precondition:  A valid filename is provided.
      Post-condition: The tree structure is written to the file in a readable format.
    -----------------------------------------------------------------------*/
    void printTree(const string &filename);
};

#endif // FORESTTREE_H
