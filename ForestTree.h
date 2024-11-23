#ifndef FORESTTREE_H
#define FORESTTREE_H

/**-- ForestTree.h ----------------------------------------------------------
  This header file defines a ForestTree class for managing a hierarchical
  structure of accounts, with each account containing transactions.

  Basic operations:
    Constructor:         Constructs an empty ForestTree object.
    Destructor:          Cleans up memory used by the tree and its accounts.
    initialize:          Initializes an empty ForestTree.
    buildFromFile:       Builds the ForestTree structure from a file.
    addAccount:          Adds an account to the tree.
    removeAccount:       Removes an account from the tree by its number.
    addTransaction:      Adds a transaction to a specific account.
    removeTransaction:   Removes a transaction from a specific account.
    searchAccount:       Searches for an account in the tree by its number.
    printTree:           Prints the entire tree to a file.
    printAccountDetails: Prints details of a specific account to a file.

  Helper functions:
    deleteTree:          Recursively deletes all nodes in the tree.

  Class Invariant:
    1. The root pointer points to the root of the account hierarchy or is null if the tree is empty.
    2. The accountMap provides a quick lookup for accounts using their unique numbers.
    3. Each account can contain multiple transactions.
--------------------------------------------------------------------------**/

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Account.h"
#include "Transaction.h"  // Include the full definition of Transaction

class ForestTree {
private:
    Account *root; // Root of the tree
    std::map<int, Account *> accountMap; // Map for quick account lookup by number

    /***** Helper Functions *****/
    /*------------------------------------------------------------------------
      Recursively deletes all accounts in the tree.

      Precondition:  A valid pointer to the root node or a subtree node.
      Post-condition: All nodes in the subtree are deleted, and memory is freed.
    -----------------------------------------------------------------------*/
    void deleteTree(Account *node);

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
      Initializes an empty ForestTree.

      Precondition:  None.
      Post-condition: The tree is cleared, removing all accounts and transactions.
    -----------------------------------------------------------------------*/
    void initialize();

    /***** Build from File *****/
    /*------------------------------------------------------------------------
      Builds the ForestTree structure by reading account data from a file.

      Precondition:  A valid filename with properly formatted account data is provided.
      Post-condition: The tree is populated with accounts based on the file data.
    -----------------------------------------------------------------------*/
    void buildFromFile(const std::string &filename);

    /***** Add Account *****/
    /*------------------------------------------------------------------------
      Adds a new account to the ForestTree.

      Precondition:  A unique account number, description, and initial balance are provided.
      Post-condition: The new account is added to the tree and mapped in accountMap.
    -----------------------------------------------------------------------*/
    void addAccount(int accountNumber, const std::string &description, double initialBalance);

    /***** Remove Account *****/
    /*------------------------------------------------------------------------
      Removes an account from the ForestTree by its account number.

      Precondition:  A valid account number is provided.
      Post-condition: The account is removed from the tree and accountMap if it exists.
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
    void printTree(const std::string &filename);

    /***** Print Account Details *****/
    /*------------------------------------------------------------------------
      Prints the details of a specific account to a file.

      Precondition:  A valid account number and filename are provided.
      Post-condition: The account details are written to the file in a readable format.
    -----------------------------------------------------------------------*/
    void printAccountDetails(int accountNumber, const std::string &filename);

    /***** Print Tree Recursive *****/
    /*------------------------------------------------------------------------
      Recursively prints the ForestTree structure to a file.

      Precondition:  A valid account pointer, file stream, and indentation level are provided.
      Post-condition: The tree structure is recursively written to the file.
    -----------------------------------------------------------------------*/
    void printTreeRecursive(Account *account, std::ofstream &file, int indent);
};

#endif // FORESTTREE_H
