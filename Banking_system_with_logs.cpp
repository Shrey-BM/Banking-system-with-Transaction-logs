/*
              BANKING SYSTEM WITH TRANSACTION HISTORY

   - Every account has a unique account number (1001, 1002, etc.).
     We use this number to perform all banking operations.
   - A phone number can be linked to only one active account.
     Before creating a new account, we check if the phone number
     is already registered. Two customers can have the same name.
   - The customer's name is stored and displayed whenever needed.

   DSA CONCEPTS USED

   1. Vector:
      Stores all the accounts.
      We can directly access an account using its account number.
      This makes searching for an account O(1).
      If an account is deleted, its position is set to nullptr.
      Account numbers are never reused.

   2. Linked List:
      Each account has its own linked list to store its transactions.
      We use head and tail pointers to keep track of the first
      and last transaction.

   3. Hash Table:
      Stores phone numbers along with their account numbers.
      It helps us check whether a phone number is already registered
      when someone tries to open an account.
      This lookup takes O(1) time on average.
      If an account is deleted, its phone number is also removed
      from the hash table, so it can be used again.
      Collisions are handled using chaining.

   4. Pair:
      Each entry in the hash table stores two values together:
      the phone number and its corresponding account number.
*/

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
using namespace std;

// ---------- structures ----------
struct Transaction {
    string type;          // OPENING, DEPOSIT, WITHDRAW, TRANSFER TO, TRANSFER FROM
    double amount;
    int otherAcc;         // the other account in a transfer (-1 if none)
    double balance;       // balance after this transaction
};

struct Node {             
    Transaction data;
    Node* next;
};

struct Account {
    int accNo;
    string name;          // user name
    string phone;         // user phone number
    double balance;       // user balance
    Node* head;           // first (oldest) transaction   
    Node* tail;           // last (newest) transaction
};

const int FIRST_ACC = 1001;
vector<Account*> accounts;      // vector to store all the accounts

/*
Find  account by  account number or index
Since the account number starts from 1001, we subtract FIRST_ACC
to get its position in the vector.
*/
Account* getAccount(int accNo) {
    int i = accNo - FIRST_ACC;
    if (i < 0 || i >= (int)accounts.size()) return nullptr;
    return accounts[i];
}

/* 
Converts the phone number into an index for the hash table. 
We multiply the current hash value by 31 and add the next digit.
Modulo SIZE keeps the index within the table's range.
*/
const int SIZE = 10007;          
vector<pair<string, int>> table[SIZE];

int hashFn(const string& phone) {
    int h = 0;
    for (char c : phone){
     h = (h * 31 + c) % SIZE;
    }
    return h;
}

/*
Checks whether this phone number is already present in the hash table.
We only search inside the bucket given by the hash function.
If we find the phone number, we return its account number.
Otherwise, -1 means that the phone number is not registered.
*/
int findByPhone(const string& phone) {      
    for (auto& p : table[hashFn(phone)]){
    if(p.first==phone) return p.second;
    }
    return -1;
}

void addTransaction(Account* a, const string& type, double amount, int otherAcc = -1) {
    Node* n = new Node{{type, amount, otherAcc, a->balance}, nullptr};
    if (a->head == nullptr) {
    a->head = a->tail = n;
    }
    else {      
    a->tail->next = n; a->tail = n; 
    }
}

// Bank operations

// Opens a new account after checking the given details.
void openAccount(const string& name, const string& phone, double amount) {
    if(phone.size() != 10){ // Phone number should contain exactly 10 digits.
        cout << "Phone number must have 10 digits\n"; return; 
    }  
    if(findByPhone(phone) != -1){  // A phone number cannot be used for two active accounts.
        cout << "This phone number already has an account\n"; 
        return; 
    }
    if(amount < 0){ // Amount cannot be negative.
        cout << "Amount cannot be negative\n"; 
        return;
    }
    // Generate a new account number. Since the account number starts from 1001, we add the number of accounts to it.
    int accNo = FIRST_ACC + accounts.size();    // The account number is the next available number.
    Account * a = new Account{accNo, name, phone, amount, nullptr, nullptr}; // Create a new account.
    accounts.push_back(a);  // Add the new account to the vector.
    table[hashFn(phone)].push_back({phone, accNo});  // Add the phone number and account number to the hash table.
    addTransaction(a, "OPENING ", amount);            // Add the opening transaction to the account.
    cout << "Account opened. Account number: " << accNo << "\n";
}

// Deposits money into an account.
void deposit(int accNo, double amount) {
    // Get the account by its account number.
    Account* a = getAccount(accNo);
    if (a == nullptr){ // If the account is not found, print an error message.
        cout << "Account not found\n"; 
        return; 
    }
    if (amount <= 0){ // Amount cannot be negative.
        cout << "Amount must be positive\n"; 
        return;
    }
    // Add the amount to the account balance.
    a->balance += amount;
    addTransaction(a, "DEPOSIT ", amount);
    cout << "Deposited. New balance: Rs." << a->balance << "\n"; // Print the new balance.
}

// Withdraws money from an account.
void withdraw(int accNo, double amount) {
    // Get the account by its account number.
    Account* a = getAccount(accNo);
    if(a == nullptr){ // If the account is not found, print an error message.
        cout << "Account not found\n"; 
        return; 
    }
    if(amount <= 0){ // Amount cannot be negative.
        cout << "  Amount must be positive\n"; 
        return; 
    }
    if(a->balance < amount){ // Insufficient balance.               
        cout << "  Insufficient balance\n";
        return; 
    }
    // Subtract the amount from the account balance.
    a->balance -= amount;
    addTransaction(a, "WITHDRAW ", amount); // Add the withdrawal transaction to the account.
    cout << "Withdrawn. New balance: Rs." << a->balance << "\n";
}

// Transfers money from one account to another.
void transfer(int fromAcc, int toAcc, double amount) {
    // Get the accounts by their account numbers.
    Account* from = getAccount(fromAcc);
    Account* to = getAccount(toAcc);
    if(from == nullptr || to == nullptr){ // If the accounts are not found, print an error message.
        cout << "Account not found\n";
        return; 
    }
    if(fromAcc == toAcc){ // Cannot transfer to the same account.
        cout << "Cannot transfer to the same account\n"; 
        return; 
    }
    if(amount <= 0){ // Amount cannot be negative.
        cout << "Amount must be positive\n";
        return; 
    }
    if(from->balance < amount){ // Insufficient balance.
        cout << "Insufficient balance\n"; 
        return; 
    }
    // Subtract the amount from the sender's account balance.       
    from->balance -= amount;
    to->balance += amount;   // Add the amount to the receiver's account balance.
    addTransaction(from, "TRANSFER TO ", amount, toAcc);      // sender's history
    addTransaction(to, "TRANSFER FROM ", amount, fromAcc);    // receiver's history
    cout << "Transferred Rs." << amount << "to account " << toAcc << "\n";
}

// Displays the account details.
void showAccount(int accNo) {
    // Get the account by its account number.
    Account* a = getAccount(accNo);
    if (a == nullptr){ // If the account is not found, print an error message.
        cout << "  Account not found\n";
        return;
    }
    // Print the account details.
    cout << "Account: " << a->accNo << "| Name: " << a->name << "| Phone: " << a->phone << "| Balance: Rs." << a->balance << "\n";
}

// Displays the transaction history of an account.
void showHistory(int accNo) {
    // Get the account by its account number.
    Account* a = getAccount(accNo);
    if (a == nullptr){ // If the account is not found, print an error message.
        cout << "Account not found\n"; 
        return;
    }
    // Print the transaction history.       
    cout << "Transaction history of account" << a->accNo << ":\n";
    // Walk the linked list and print each transaction.
    int i = 1;
    for (Node* cur = a->head; cur != nullptr; cur = cur->next) {  
        const Transaction& t = cur->data;                          
        cout << "  " << i++ << ". " << t.type;
        if (t.otherAcc != -1) cout << " " << t.otherAcc;  // If the transaction is a transfer, print the other account number.
        cout << "Rs." << t.amount << " Balance: Rs." << t.balance << "\n";
    }
}

// Deletes an account from the system.  
void removePhone(const string& phone) {          // Removes the phone number from the hash table.
    vector<pair<string, int>>& bucket = table[hashFn(phone)];
    for (int i = 0; i < (int)bucket.size(); i++) {
        if (bucket[i].first == phone){
            bucket[i] = bucket.back();           // Moves the last entry into this spot
            bucket.pop_back();
            return;
        }
    }
}

// Deletes an account from the system.
void freeAccount(Account* a) {             
    Node* cur = a->head;
    while (cur != nullptr) {
        Node* next = cur->next;    // Stores the next node.
        delete cur;                // Deletes the current node.
        cur = next;                // Moves to the next node.
    }
    delete a;                      // Deletes the account.
}  

// Closes an account after checking the given details.
void deleteAccount(int accNo) {
    Account* a = getAccount(accNo);
    if (a == nullptr) {
        cout << "Account not found\n";
        return;
    }
    // Print the closing balance.
    cout << "Closing balance Rs." << a->balance << " returned to the customer\n";
    removePhone(a->phone);  // Removes the phone number from the hash table.
    freeAccount(a);         // Deletes the account.
    accounts[accNo - FIRST_ACC] = nullptr;       // Slot stays empty, number is never reused
    cout << "Account " << accNo << "deleted\n";  // Prints the deleted account number.  
}

// Main function
int main() {
    cout << fixed << setprecision(2);

    int choice, acc, acc2;
    string name, phone;
    double amount;
    bool running = true;

    while (running) {   // Runs until the user closes the portal.
        cout << "\n===== BANK MENU =====\n"
             << "1. Open account\n"
             << "2. Deposit\n"
             << "3. Withdraw\n"
             << "4. Transfer\n"
             << "5. Find account\n"
             << "6. Transaction history\n"
             << "7. Delete account\n"
             << "8. Close portal\n"
             << "Choice: ";
        if(!(cin >> choice)) break;  // If the user enters an invalid choice, break the loop.

        switch(choice) {
        case 1:
            cout << "Name: ";   getline(cin >> ws, name);  // Reads the name.
            cout << "Phone: ";  cin >> phone;
            cout << "Amount: "; cin >> amount;  // Reads the amount.            
            openAccount(name, phone, amount);
            break;  // Opens a new account.     
        case 2:
            cout << "Account number: "; cin >> acc;
            cout << "Amount: ";         cin >> amount;  // Reads the amount.          
            deposit(acc, amount);
            break;  // Deposits money into an account.
        case 3:
            cout << "Account number: "; cin >> acc;
            cout << "Amount: ";         cin >> amount;  // Reads the amount.          
            withdraw(acc, amount);
            break;  // Withdraws money from an account.
        case 4:
            cout << "From account: "; cin >> acc;
            cout << "To account: ";   cin >> acc2;
            cout << "Amount: ";       cin >> amount;
            transfer(acc, acc2, amount);
            break;
        case 5:
            cout << "Account number: "; cin >> acc;
            showAccount(acc);
            break;
        case 6:
            cout << "Account number: "; cin >> acc;
            showHistory(acc);
            break;
        case 7: {
            char confirm;
            cout << "Account number: "; cin >> acc;
            cout << "Are you sure? (y/n): "; cin >> confirm;
            if (confirm == 'y' || confirm == 'Y') deleteAccount(acc);   
            else cout << "  Cancelled\n";
            break;
        }
        case 8:
            running = false;
            break;
        default:
            cout << "Invalid choice\n";
        }
    }

    // Free memory of all remaining accounts (deleted ones are already nullptr)
    for (Account* a : accounts)
        if (a != nullptr) freeAccount(a);

    cout << "Portal closed\n";
    return 0;
}
