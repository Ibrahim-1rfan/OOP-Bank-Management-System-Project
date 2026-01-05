#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <stdexcept>
#include <typeinfo>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <functional>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <tuple>
#include <memory> // For unique_ptr or shared_ptr
#include "classes.h"

using namespace std;

class User {
protected:

    vector<shared_ptr<BasicBankingAccount>> Banks;  // changed to shared ptr to manage memory better
    string User_Name;
    int User_Pin;
    int Citizen_Identity;
    double Salary;
    vector <Loans> Loan_History;
    double CreditScore,overdue; // added overdue to calculate credit score, minimum credit should be 450 and maximum 750
    bool loan_taken = false; // renamed to avoid confusion
    int late_payments = 0;

public:



   

    User( const string& Name, const int& Pin, const int& ID, const double& pay) {

        User_Name = Name;
        Citizen_Identity = ID;
        User_Pin = Pin;
        Salary = pay;
        CreditScoreCalculator(); 

    }

    void copyUser( User copyuser){
      auto [userId, userPin] = copyuser.getCredentials();
      User_Name = copyuser.GetUserName();
      Citizen_Identity = userId;
      User_Pin= userPin;
      Salary = copyuser.CheckSalary();   
      Banks = copyuser.getBank();
      CopyLoanHistory(copyuser.getLoanHistory());
      RecalculateLateOverdue();
    }

    void AddLoan(const Loans& loan){
        cout << "Adding Loan to User's Loan History..." << endl; //Debugging
        Loan_History.push_back(loan);
        loan_taken = true;
        overdue = AverageOverDue();
        CreditScoreCalculator();   // updating credit score on adding loan

    }

    void CopyLoanHistory(vector<Loans> loan ){  
        Loan_History = loan;
        loan_taken = true;
    }

    string GetUserName(){
        return User_Name;
    }

    double CheckSalary(){
        return Salary;
    }

    double GetCredit(){
        return CreditScore;
    }

    vector<Loans> getLoanHistory(){
        return Loan_History;
    }

    bool UserExist(const int& id) const{
        try {
            if(Citizen_Identity != id){
                throw UserNotFoundException();
                return false;
            }
            else if (Citizen_Identity == id){
                return true;
            }
        }
        catch (const exception& except) {
            cerr << except.what() << endl;
        }
        return true;
    }

    void updatePin(const int& pin){
        User_Pin = pin;
    }

    void updateSalary(const double& raise){
        Salary = raise;
        CreditScoreCalculator();  // updating credit score on salary change

    }

    bool updateLoans(const int& Loan_Id){


        auto it = find_if(Loan_History.begin(), Loan_History.end(), 
                            [Loan_Id](const Loans& loan) { 
                                 return loan.loan_id == Loan_Id; }
                        );
        
         if(Loan_Payment(*it)){
            cout << "Loan succesfully paid."<<endl;
            return true;
         }

         else{
            late_payments++;
            cout << "Loan succesfully paid late. Charged overdue fee."<<endl;
            return false;
         }
        


    }

    void RecalculateLateOverdue() {

        late_payments = 0;
        loan_taken = !Loan_History.empty();
        
        for (const auto& loan : Loan_History) {
            if (!loan.On_Time) {
                late_payments++;
            }
        }

        overdue = AverageOverDue();

        CreditScoreCalculator();
    }

    vector<shared_ptr<BasicBankingAccount>> getBank(){
        return Banks;
    }

    //TODO: implement tuple return for ID and Pin in main login function
    tuple<int, int> getCredentials() const {

    return make_tuple(Citizen_Identity, User_Pin);
   
  }

    void updateBank(vector<shared_ptr<BasicBankingAccount>> x){ //why am i replacing the whole bank vector ???
        Banks = x;
    }

    void CreditScoreCalculator() {   //? Logic: Penalize late payments by 10*average delay points per late payment, reward each loan taken with 5 points, reward salary brackets with base points
     

        double BaseScore = 500; // Default for < 100k
    
        if (Salary >= 1000000)      BaseScore = 700;
        else if (Salary >= 350000)  BaseScore = 600;
        else if (Salary >= 100000)  BaseScore = 550;

        //for rewarding loans taken
        int reward = (loan_taken) ? 20 : 0; // If loan exists, add 20, else 0

        // Final Credit Score Calculation
        CreditScore = BaseScore + reward - (late_payments * overdue * 10) + (5 * Loan_History.size());

        //Final credit score should be between 300 and 850
        if (CreditScore > 850) CreditScore = 850;
        if (CreditScore < 300) CreditScore = 300;

    }

    bool CanGetLoan(){
        if(CreditScore < 450){
            cout << "Credit score too low. Not eligible for loan"<< endl;
            return false;
        }
        return true;
    }


    int LoanInterestCalculator( const double& amount){
        
        double LoanInterest;

        if (amount<=100000)                              LoanInterest = 15;
        else if (amount>100000 && amount <= 500000 )     LoanInterest = 7;
        else if (amount>500000)                          LoanInterest = 3.125;

        return LoanInterest;

    }

    bool checkLoanTaken(){
        return loan_taken;
    }
 
    double AverageOverDue(){

     double Total_Overdue = 0 , Average_Overdue = 0;
        for(auto it = Loan_History.begin(); it != Loan_History.end(); ++it ) {
            if(it->On_Time == false){
                double overdue = String_TO_Double(it->Payment_Date) - String_TO_Double(it->Due_Date);
                Total_Overdue = Total_Overdue + overdue;
            }
        }

        if (late_payments > 0)     Average_Overdue = Total_Overdue/ late_payments;
       
        return Average_Overdue;
    }

  void OpenAccount(shared_ptr<BasicBankingAccount> BankAccount){
        Banks.push_back(BankAccount);
    }


  // Inside User Class
string serializeUser() {
    string data = "";

    // 1. User Basic Info (Level 1: Pipes)
    data += to_string(Citizen_Identity) + "|";
    data += to_string(User_Pin) + "|";
    data += User_Name + "|";
    data += to_string(Salary) + "|";

    // 2. Serialize Bank Accounts Vector (Level 2: Braces & Semicolons)
    data += "{"; 
    for (size_t i = 0; i < Banks.size(); i++) {
        data += Banks[i]->serialize(); // Call the account's function
        if (i != Banks.size() - 1) {
            data += ";"; // Separate accounts with Semicolon
        }
    }
    data += "}|"; // End of Bank Vector

    // 3. Serialize Loans Vector
    data += "{";
    for (size_t i = 0; i < Loan_History.size(); i++) {
        data += serialize_loan(Loan_History[i]);
        if (i != Loan_History.size() - 1) {
            data += ";";
        }
    }
    data += "}"; // End of Loan Vector

    return data;
}
};


void SaveAllUsers(const vector<User>& allUsers) {
    ofstream file("BankDatabase.txt", ios::trunc); // Overwrite file

    for (User u : allUsers) {
        // 1. Get the massive one-liner
        string cleanLine = u.serializeUser();

        // 2. Encrypt it (So it looks like garbage)
        string encryptedLine = EncryptDecrypt(cleanLine);

        // 3. Write to file
        file << encryptedLine << endl;
    }
    file.close();
}


void LoadAllUsers(vector<User>& systemUsers) {
    ifstream file("BankDatabase.txt");
    
    // Check if file opened successfully
    if (!file.is_open()) {
        cerr << "Warning: Could not open BankDatabase.txt. Starting with empty database." << endl;
        return;
    }

    string line;

    while (getline(file, line)) { // Reads ONE USER per loop
        if(line.empty()) continue;

        // 1. Decrypt the line
        string raw = EncryptDecrypt(line); 
        // raw format: "101|1234|Ali|50000|{SAVINGS,HBL...;CURRENT...}|{LOAN,1...;LOAN,2...}"

        // 2. Split Main Parts by PIPE '|'
        vector<string> parts = Split(raw, '|');
        
        // Safety Check: Ensure we have at least the basic user parts
        if (parts.size() < 6) continue; 

        int id = stoi(parts[0]);
        int pin = stoi(parts[1]);
        string name = parts[2];
        double salary = stod(parts[3]);

        // Create the User Object
        User newUser(name, pin, id, salary);

        // ---------------------------------------------------------
        // 3. Parse Accounts (The 5th part, index 4)
        // ---------------------------------------------------------
        string accountSection = parts[4]; 
        
        // Remove surrounding '{' and '}'
        if (accountSection.size() >= 2 && accountSection.front() == '{' && accountSection.back() == '}') {
            accountSection = accountSection.substr(1, accountSection.size() - 2);
            
            if (!accountSection.empty()) {
                // Split by SEMICOLON ';' to get individual accounts
                vector<string> accountList = Split(accountSection, ';');

                for (string accStr : accountList) {
                    // Split by COMMA ',' to get account details
                    vector<string> details = Split(accStr, ',');
                    if (details.empty()) continue;

                    string type = details[0];
                    string bankName = details[1];
                    int accNo = stoi(details[2]);
                    string holder = details[3];
                    double balance = stod(details[4]);
                    int pin = stoi(details[5]);

                    if (type == "SAVINGS") {
                        // Format: SAVINGS,Bank,AccNo,Holder,Balance,Pin,Time,Date
                        auto sav = make_shared<SavingsAccount>(bankName, accNo, holder, balance, pin);
                        
                        // Set extra details
                        if (details.size() > 6) sav->SetTime(stod(details[6]));
                        // Note: You might need a setter for Last_interestcycle_Date if you want to restore it exactly
                        if (details.size() > 7) sav->LastInterestDate(details[7]);
                        newUser.OpenAccount(sav);
                    }
                    else if (type == "CURRENT") {
                        // Format: CURRENT,Bank,AccNo,Holder,Balance,Pin,Overdraft
                        auto curr = make_shared<CurrentAccount>(bankName, accNo, holder, balance, pin);
                        // Note: Overdraft fee is protected and has no setter, so we cannot restore the specific fee amount without modifying the class.
                        if (details.size() > 6) curr->setOverdraftFee(stod(details[6]));
                        newUser.OpenAccount(curr);
                    }
                    else if (type == "FIXED_DEPOSIT") {
                        // Format: FIXED_DEPOSIT,Bank,AccNo,Holder,Balance,Pin,Time,Date
                        int timePeriod = stoi(details[6]); 
                        // Note: This constructor recalculates MaturityDate based on TODAY. 
                        auto fixed = make_shared<Fixed_Deposit>(bankName, accNo, holder, balance, pin, timePeriod);
                        // Set Maturity Date
                        if (details.size() > 7) fixed->setMaturityDate(details[7]);

                        newUser.OpenAccount(fixed);
                    }
                    else if (type == "BASIC") {
                         auto basic = make_shared<BasicBankingAccount>(bankName, accNo, holder, balance, pin);
                         newUser.OpenAccount(basic);
                    }
                }
            }
        }

        // ---------------------------------------------------------
        // 4. Parse Loans (The 6th part, index 5)
        // ---------------------------------------------------------
        string loanSection = parts[5];
        vector<Loans> tempLoanHistory;

        // Remove surrounding '{' and '}'
        if (loanSection.size() >= 2 && loanSection.front() == '{' && loanSection.back() == '}') {
            loanSection = loanSection.substr(1, loanSection.size() - 2);

            if (!loanSection.empty()) {
                vector<string> loanList = Split(loanSection, ';');

                for (string loanStr : loanList) {
                    vector<string> lData = Split(loanStr, ',');
                    
                    // Format: LoanID,Value,Interest,Paid,DueDate,PaymentDate,TakenDate,OnTime
                    if (lData.size() >= 8) {
                        Loans l;
                        l.loan_id = stoi(lData[0]);
                        l.value = stod(lData[1]);
                        l.interest = stod(lData[2]);
                        l.paid = (lData[3] == "1");
                        l.Due_Date = lData[4];
                        l.Payment_Date = lData[5];
                        l.Taken_Date = lData[6];
                        l.On_Time = (lData[7] == "1");

                        tempLoanHistory.push_back(l);
                    }
                }
            }
        }

        // Add the restored loans to the user
        if (!tempLoanHistory.empty()) {
            newUser.CopyLoanHistory(tempLoanHistory);
        }

        newUser.RecalculateLateOverdue();
        
        // Add fully built user to system
        systemUsers.push_back(newUser);
    }
    
    file.close();
}