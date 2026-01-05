
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
#include "Helper_functions.h"
#include "my_exceptions.h"

using namespace std;


class BasicBankingAccount {
protected:
    string Bank;  //Bank Name
    int AccountNumber;
    int AccountPin;
    string AccountTitle; //Account Holder Name
    double AccountBalance;

public:

    BasicBankingAccount(const string& bank, const int& Acc_No,const string& AccountHolder ,const double& initial_balance, const int& pin){
        Bank = bank;
        AccountNumber = Acc_No;
        AccountTitle = AccountHolder;
        AccountBalance =initial_balance;
        AccountPin = pin;
    }

     tuple<string, string, int> getBankInfo() const {

      return make_tuple(Bank, AccountTitle, AccountPin);
   
     }

    bool AccountExists(int id){
        try {
            if(AccountNumber != id){
                throw AccountNotFoundException();
                return false;
            }
            else if (AccountNumber == id){
                return true;
            }
        }
        catch (const exception& except) {
            cerr << except.what() << endl;
        }
        return true;
    }

    void updatePin(const int& pin) {
        AccountPin = pin;
    }

    int checkPin() const {
        return AccountPin;
    }

    void Deposit(const double& deposit) {
        try {
            if (deposit < 0) {
                throw NegativeAmountException();
            }
            else {
                AccountBalance = AccountBalance + deposit;
            }
        }
        catch (const exception& except) {
            cerr << except.what() << endl;
        }

    }

    virtual double checkBalance(){
        return AccountBalance;
    }

    int getAcc_No() const {
        return AccountNumber;
    }

    virtual void Withdraw(const double& amount) {
        try {
            if ( amount < 0 ){
                throw NegativeAmountException();
            }
            else if ( amount > AccountBalance ){
                throw InsufficientFundsException();
            }

            else {
                AccountBalance = AccountBalance - amount;
                cout << "Amount Withdrawn successfully"<< endl;
            }

        }
        catch (const exception& except) {
            cerr << except.what() << endl;
        }

    }

    virtual string serialize() {
    // Format: TYPE,Bank,AccNo,Holder,Balance,Pin
    return "BASIC," + Bank + "," + to_string(AccountNumber) + "," + 
           AccountTitle + "," + to_string(AccountBalance) + "," + to_string(AccountPin);
    }
   

    friend void BankTransfer( BasicBankingAccount& x, BasicBankingAccount& y, const double& amount);

};
void BankTransfer( BasicBankingAccount& x, BasicBankingAccount& y, const double& amount){
    x.Withdraw(amount);
    y.Deposit(amount);
}



class CurrentAccount : public BasicBankingAccount {
protected:
    double overdraft_fee;

public:
    CurrentAccount (const string& bank,const int& Acc_No, const string& AccountHolder, const double& initial_balance, const int& pin) : BasicBankingAccount(bank, Acc_No, AccountHolder, initial_balance, pin) {}

    void Withdraw(const double& amount){
        try{
            if( amount < 0){
                throw NegativeAmountException();
            }
            else if (amount > AccountBalance){
                double overdraft = amount - AccountBalance ;
                overdraft_fee = overdraft * 0.15;

                AccountBalance =   AccountBalance - amount - overdraft_fee;

                cout << "Amount Transaction succesfull.\n Overdraft Penalty= $" << overdraft_fee << "\nAccount Balance After Overdraft penalty: $ " << AccountBalance <<endl;
            }
            else {
                AccountBalance = AccountBalance - amount;
                cout << " Amount Transaction successfull.\n Remaining Balance: $" << AccountBalance << endl;
            }

        }
        catch(const exception& except){
            cerr << except.what() << endl;
        }

    }

    void setOverdraftFee(const double& fee){ // new function to set overdraft fee in serialization
        overdraft_fee = fee;
    }
     
    virtual string serialize() override {
    // Returns basic info: Type|BankName|AccNo|Holder|Balance|Pin
    return "CURRENT," + Bank + "," + to_string(AccountNumber) + "," + 
           AccountTitle + "," + to_string(AccountBalance) + "," + to_string(AccountPin) + "," + to_string(overdraft_fee);
    }

};

class SavingsAccount : public BasicBankingAccount {
protected:
    const double interest_rate = 10.0;
    double Time_Period; //in years
    string Last_interestcycle_Date;

public:
    SavingsAccount ( const string& bank,const int& Acc_No, const string& AccountHolder, const double& initial_balance, const int& pin) : BasicBankingAccount(bank, Acc_No, AccountHolder, initial_balance, pin) {
        Last_interestcycle_Date = getCurrentDate();
    }


    void SavingsManager(){
        double savings;
        savings = (AccountBalance * interest_rate * Time_Period) / 100;
        AccountBalance = AccountBalance + savings;
    }

    void SetTime(const double& Period){ //in years
        Time_Period = Period;
    }

    int getTime(){ // new function to get time period for serialization
        return Time_Period;
    }

    void PeriodOfInterest(){ // new function  implementedto check if time period has passed for interest calculation

        string Current_Date = getCurrentDate();
        double diff_days = String_TO_Double(Current_Date) - String_TO_Double(Last_interestcycle_Date);
        double days_in_one_period = Time_Period * 365.0;

        if (days_in_one_period <= 0) return; // Avoid division by zero or negative periods

        int cycles_missed = (int)(diff_days / days_in_one_period);
        
        // add interest for each interest cycle missed
        if (cycles_missed > 0) {
            for (int i = 0; i < cycles_missed; i++) {
                SavingsManager();
            }
            Last_interestcycle_Date = Current_Date;
        }
    }

    double checkBalance() override {
           
        PeriodOfInterest(); // check and add interest before returning balance
        return AccountBalance;
    } 

    void Withdraw(const double& amount) {

        PeriodOfInterest(); // check and add interest before withdrawal

        try {
            if ( amount < 0 ){
                throw NegativeAmountException();
            }
            else if ( amount > AccountBalance ){
                throw InsufficientFundsException();
            }

            else {
                AccountBalance = AccountBalance - amount;
                cout << "Amount Withdrawn successfully";
            }

        }
        catch (const exception& except) {
            cerr << except.what() << endl;
        }

    }


    void LastInterestDate(const string& date){ // new function to set last interest date in serialization
        Last_interestcycle_Date = date;
    }


    string serialize() override {
    // Format: SAVINGS,Bank,AccNo,Holder,Balance,Pin,Time,Date
    return "SAVINGS," + Bank + "," + to_string(AccountNumber) + "," + 
           AccountTitle + "," + to_string(AccountBalance) + "," + to_string(AccountPin) + "," + 
           to_string(Time_Period) + "," + Last_interestcycle_Date;
    }

};

class Fixed_Deposit : public BasicBankingAccount {
protected:
    int TimePeriod;
    string MaturityDate;
    double interest_rate = 10.0;
    bool EarlyWithdraw = false;


public:
    Fixed_Deposit( const string& bank, const int& Acc_No, const string& AccountHolder, const double& initial_balance, const int& pin,const int& TimePeriod): BasicBankingAccount( Bank, Acc_No, AccountHolder, initial_balance, pin)  {
       MaturityDate = GetDueDate(TimePeriod); 
       this->TimePeriod = TimePeriod;
    }

    void setMaturityDate(const string& date){
        MaturityDate = date;
    }

    double CalculatePenalty(){    
        EarlyWithdraw = Compare_dates(getCurrentDate(), MaturityDate);
        
        double penalty;
        if(EarlyWithdraw == true){
            penalty = TimePeriod * interest_rate;
            EarlyWithdraw = false; // reset after calculation
            return penalty ;}

        
        penalty = 0;
        return penalty;
    }

    void Withdraw(const double& amount) {
        double Early_Withdrawal_Penalty = CalculatePenalty();
        try {
            if ( amount < 0 ){
                throw NegativeAmountException();
            }
            else if ( amount > AccountBalance ){
                throw InsufficientFundsException();
            }

            else {
                AccountBalance = AccountBalance - amount - Early_Withdrawal_Penalty;
                if (EarlyWithdraw == true){
                    cout << "Amount Withdrawn successfully.\n Penalty imposed for Early Withdrawal: " << Early_Withdrawal_Penalty << "\nRemaining Balance after Withdrawal and Penalty: " << AccountBalance << endl;}
                else{cout << "Amount Withdrawn successfully." << "\nRemaining Balance after Withdrawal and Penalty: " << AccountBalance << endl;}
            }

        }
        catch (const exception& except) {
            cerr << except.what() << endl;
        }

    }

    string serialize() override {
    // Format: FIXED_DEPOSIT,Bank,AccNo,Holder,Balance,Pin,Time,Date
    return "FIXED_DEPOSIT," + Bank + "," + to_string(AccountNumber) + "," + 
           AccountTitle + "," + to_string(AccountBalance) + "," + to_string(AccountPin) + "," + 
           to_string(TimePeriod) + "," + MaturityDate;
        }
    


};

struct Loans {

    int loan_id;
    double value = 0.0;
    double interest, overdue;
    bool paid = false;
    std::string Due_Date , Payment_Date;
    std::string Taken_Date;
    bool On_Time = false;
    //int late_payments = 0; false logic sorry


};

 Loans Initialize_Loan (int id,double val,double interesting,int period){ //Loan initialization for logging
  
    Loans lone; // dummy loan to make loan record

    lone.loan_id = id; //Dont know how im calculating id, if using fuction just going to pass it here later on 
    
    // setting interest and loan amount
    lone.value = val;
    lone.interest = interesting;

    // setting dates
    lone.Taken_Date = getCurrentDate();
    lone.Due_Date = GetDueDate(period);
    

    cout << "Loan Taken on: " << lone.Taken_Date << "\n Due Date: " << lone.Due_Date << endl; //Debuugging

    lone.Payment_Date = "0";
    lone.On_Time = true;

    cout << "Loan Initialized Successfully"<< endl; //Debugging

    return lone;
    };

 bool Loan_Payment(Loans& return_loan){
    

    return_loan.Payment_Date = getCurrentDate(); // logging current date as payment date

    if (Compare_dates(return_loan.Payment_Date, return_loan.Due_Date)) {
        return_loan.On_Time = true;
        return_loan.paid = true;
        return true;
       }
    else {
        return_loan.On_Time = false;
        return_loan.paid = true;
        return false;
    }


 }
    
 string serialize_loan(const Loans& loan) {
    // Format: LoanID,Value,Interest,Paid,DueDate,PaymentDate,TakenDate,OnTime
    return to_string(loan.loan_id) + "," + 
           to_string(loan.value) + "," + 
           to_string(loan.interest) + "," + 
           (loan.paid ? "1" : "0") + "," + 
           loan.Due_Date + "," + 
           loan.Payment_Date + "," + 
           loan.Taken_Date + "," + 
           (loan.On_Time ? "1" : "0");


 }




