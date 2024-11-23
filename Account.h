#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <iostream>
#include <string>
#include <vector>

class Transaction; // Forward declaration

class Account {
private:
    int accountNumber;                    // Unique account number
    std::string description;              // Account description
    double balance;                       // Current account balance
    std::vector<Transaction*> transactions; // Use pointers to avoid incomplete type issues
    Account* parent;                      // Pointer to the parent account (if any)

    // Helper function to validate account number
    void validateAccountNumber(int accountNumber);

public:
    // Constructor
    Account(int accountNumber, const std::string& description, double initialBalance = 0.0);

    Account(const Account& other);

    Account& operator=(const Account& other);

    // Destructor
    ~Account();

    // Getters
    int getAccountNumber() const;
    const std::string& getDescription() const;
    double getBalance() const;
    Account* getParent() const;
    const std::vector<Transaction*>& getTransactions() const;

    // Setters
    void setParent(Account* parentAccount);

    // Update balance
    void updateBalance(double amount);

    // Transaction management
    void addTransaction(Transaction* transaction);
    void removeTransaction(int transactionID);

    // Overloaded input and output operators
    friend std::istream& operator>>(std::istream& in, Account& account);
    friend std::ostream& operator<<(std::ostream& out, const Account& account);

    void saveToFile(const std::string &filename) const;
};

#endif // ACCOUNT_H
