#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include "User_class.h" // Includes your classes and helper functions

using namespace std;

// --- TEST FUNCTION: Runs automatically to verify logic ---
void RunAutomatedTest() {
    cout << "============================================" << endl;
    cout << "      RUNNING AUTOMATED SYSTEM TEST" << endl;
    cout << "============================================" << endl;

    string dbFile = "BankDatabase.txt";
    
    // 1. SETUP: Create a Dummy User
    cout << "[TEST 1] Creating User 'Alice'..." << endl;
    User alice("Alice", 1234, 101, 50000); // Name, Pin, ID, Salary

    // 2. FEATURE: Add Accounts (Polymorphism)
    cout << "[TEST 2] Opening Accounts..." << endl;
    
    // Savings Account
    auto savings = make_shared<SavingsAccount>("HBL", 111, "Alice", 10000.0, 1234);
    savings->SetTime(2.0); // 2 Years period
    alice.OpenAccount(savings);

    // Current Account
    auto current = make_shared<CurrentAccount>("Meezan", 222, "Alice", 5000.0, 1234);
    alice.OpenAccount(current);

    // 3. FEATURE: Loans & Credit Score
    cout << "[TEST 3] Taking a Loan..." << endl;
    if (alice.CanGetLoan()) {
        cout << "Loan Initialized Started" << endl; //Debugging
        Loans myLoan = Initialize_Loan(1, 50000, 10.0, 12); // ID, Val, Interest, Paid, Period
        cout << "Loan Initialized Ended" << endl; //Debugging
        alice.AddLoan(myLoan);
        cout << " -> Loan Added. New Credit Score: " << alice.GetCredit() << endl;
    }

    // 4. FEATURE: Saving to File
    cout << "[TEST 4] Saving to File..." << endl;
    vector<User> allUsers;
    allUsers.push_back(alice);
    SaveAllUsers(allUsers);
    cout << " -> Data Encrypted and Saved." << endl;

    // 5. FEATURE: Loading from File
    cout << "[TEST 5] Clearing Memory & Reloading..." << endl;
    allUsers.clear(); // Simulate restarting app
    LoadAllUsers(allUsers);

    // 6. VERIFICATION
    if (allUsers.empty()) {
        cout << "[FAILED] No users loaded!" << endl;
        return;
    }

    User& loadedAlice = allUsers[0];
    cout << " -> Loaded User: " << loadedAlice.GetUserName() << endl;
    cout << " -> ID: " << get<0>(loadedAlice.getCredentials()) << endl; // Testing Tuple
    
    // Check Accounts
    vector<shared_ptr<BasicBankingAccount>> banks = loadedAlice.getBank();
    cout << " -> Accounts Restored: " << banks.size() << endl;
    
    for(auto& acc : banks) {
        // dynamic_cast checks if the polymorphism was preserved
        if (dynamic_cast<SavingsAccount*>(acc.get())) {
            cout << "    - Found Savings Account: Balance " << acc->checkBalance() << endl;
        } else if (dynamic_cast<CurrentAccount*>(acc.get())) {
            cout << "    - Found Current Account: Balance " << acc->checkBalance() << endl;
        }
    }

    // Check Loans
    vector<Loans> history = loadedAlice.getLoanHistory();
    cout << " -> Loans Restored: " << history.size() << endl;
    cout << " -> Credit Score Recalculated: " << loadedAlice.GetCredit() << endl;

    cout << "============================================" << endl;
    cout << "           TEST COMPLETED SUCCESSFULLY      " << endl;
    cout << "============================================" << endl << endl;
}

// --- MAIN MENU ---
int main() {
    srand(time(0)); // Seed random number generator

    // Uncomment this line to run the auto-test every time you start
    RunAutomatedTest(); 

    
}


bool checkMinimum(const double& minimum ,const double& balance){
    try {
        if ( balance < 0){
            throw NegativeAmountException();
        }
        else if ( balance < minimum){
            throw MinimumBalance();
        }
    }
    catch(const exception& except){
        cerr << except.what() << endl;
    }
    if ( balance > minimum ){
        return true;
    }
    else { return false;}

}