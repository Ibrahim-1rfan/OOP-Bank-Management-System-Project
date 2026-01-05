#include "raylib.h"

// IMPORTANT: Define this BEFORE including raygui.h
#define RAYGUI_IMPLEMENTATION 
#include "raygui.h" 

#include "User_class.h" 
#include <vector>
#include <string>
#include <cstring> // For strcpy
#include <iostream>

using namespace std;

int main() {
    // 1. Setup Window
    InitWindow(1000, 700, "Bank Management System"); // Made window slightly larger
    SetTargetFPS(60);

    // 2. Load Data
    vector<User> SystemUsers;
    LoadAllUsers(SystemUsers);

    // 3. App State Variables
    enum Screen { LOGIN, REGISTER, DASHBOARD, LOAN_MENU, ACCOUNT_MENU, ACCOUNT_VIEW, PROFILE_MENU };
    Screen currentScreen = LOGIN;

    // Track Current User & Selected Account
    int currentUserIndex = -1; 
    shared_ptr<BasicBankingAccount> selectedAccount = nullptr; // <--- SOLVES YOUR STORAGE ISSUE

    // --- BUFFERS & UI STATE ---
    
    // Login
    char idBuffer[64] = "";
    char pinBuffer[64] = "";
    bool idEdit = false, pinEdit = false;
    char message[128] = "Welcome";

    // Registration
    char nameBuffer[64] = "";
    char salaryBuffer[64] = "";
    bool nameEdit = false, salaryEdit = false;

    // Open Account (In Account Menu)
    int newAccType = 0; // 0=Savings, 1=Current, 2=Fixed
    bool newAccTypeEdit = false;
    char newBank[64] = "HBL";
    char newInitial[64] = "1000";
    char newPin[64] = "1234";
    char newPeriod[64] = "1"; // Years (for Savings/Fixed)
    bool newBankEdit = false, newInitialEdit = false, newPinEdit = false, newPeriodEdit = false;

    // Transaction (In Account View)
    char amountBuffer[64] = "";
    char targetAccBuffer[64] = "";
    bool amountEdit = false, targetAccEdit = false;


    //Loan Menu
    char loanAmount[64] = "";
    char loanPeriod[64] = ""; // Should be in months
    bool loanAmountEdit = false;
    bool loanPeriodEdit = false;

    // --- MAIN LOOP ---
    while (!WindowShouldClose()) {
        
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // ====================================================================
        // SCREEN 1: LOGIN
        // ====================================================================
        if (currentScreen == LOGIN) {
            DrawText("BANK LOGIN", 400, 150, 30, DARKGRAY);

            GuiGroupBox((Rectangle){350, 200, 300, 250}, "CREDENTIALS");
            
            GuiLabel((Rectangle){370, 230, 100, 30}, "User ID:");
            if (GuiTextBox((Rectangle){370, 250, 260, 30}, idBuffer, 64, idEdit)) idEdit = !idEdit;

            GuiLabel((Rectangle){370, 290, 100, 30}, "PIN:");
            if (GuiTextBox((Rectangle){370, 310, 260, 30}, pinBuffer, 64, pinEdit)) pinEdit = !pinEdit;

            if (GuiButton((Rectangle){370, 370, 260, 40}, "LOGIN")) {
                try {
                    int id = stoi(idBuffer);
                    int pin = stoi(pinBuffer);
                    bool found = false;
                    for (size_t i = 0; i < SystemUsers.size(); i++) {
                        auto [uID, uPin] = SystemUsers[i].getCredentials();
                        if (uID == id && uPin == pin) {
                            currentUserIndex = i; // Save the index
                            currentScreen = DASHBOARD;
                            snprintf(message, 128, "Welcome %s", SystemUsers[i].GetUserName().c_str());
                            found = true;
                            break;
                        }
                    }
                    if (!found) snprintf(message, 128, "User not found!");
                } catch (...) { snprintf(message, 128, "Invalid numbers"); }
            }


            // REGISTER BUTTON (New Feature)
            DrawText("Don't have an account?", 370, 410, 10, GRAY);
            if (GuiButton((Rectangle){370, 430, 260, 30}, "REGISTER NEW USER")) {
                currentScreen = REGISTER;
                snprintf(message, 128, "Create New ID");
                // Clear buffers
                strcpy(nameBuffer, ""); strcpy(salaryBuffer, "");
                strcpy(idBuffer, ""); strcpy(pinBuffer, "");
            }


            DrawText(message, 370, 460, 20, RED);
        }


        // ====================================================================
        // SCREEN 1.5: REGISTER (New Screen)
        // ====================================================================
        else if (currentScreen == REGISTER) {
            DrawText("NEW USER REGISTRATION", 320, 100, 30, DARKGRAY);

            GuiGroupBox((Rectangle){300, 150, 400, 350}, "Details");

            GuiLabel((Rectangle){320, 170, 100, 20}, "Name:");
            if (GuiTextBox((Rectangle){320, 190, 360, 30}, nameBuffer, 64, nameEdit)) nameEdit = !nameEdit;

            GuiLabel((Rectangle){320, 230, 100, 20}, "Desired ID:");
            if (GuiTextBox((Rectangle){320, 250, 360, 30}, idBuffer, 64, idEdit)) idEdit = !idEdit;

            GuiLabel((Rectangle){320, 290, 100, 20}, "PIN:");
            if (GuiTextBox((Rectangle){320, 310, 360, 30}, pinBuffer, 64, pinEdit)) pinEdit = !pinEdit;

            GuiLabel((Rectangle){320, 350, 100, 20}, "Salary:");
            if (GuiTextBox((Rectangle){320, 370, 360, 30}, salaryBuffer, 64, salaryEdit)) salaryEdit = !salaryEdit;

            if (GuiButton((Rectangle){320, 430, 360, 40}, "CREATE USER")) {
                try {
                    string name = nameBuffer;
                    int id = stoi(idBuffer);
                    int pin = stoi(pinBuffer);
                    double sal = stod(salaryBuffer);

                    // Create User
                    User newUser(name, pin, id, sal);
                    
                    // Add default account so they aren't empty
                    auto acc = make_shared<SavingsAccount>("DefaultBank", id+100, name, 0, pin);
                    newUser.OpenAccount(acc);

                    SystemUsers.push_back(newUser);
                    SaveAllUsers(SystemUsers); // Save immediately
                    
                    snprintf(message, 128, "User Created! Please Login.");
                    currentScreen = LOGIN;
                } catch (...) { snprintf(message, 128, "Invalid Input Data"); }
            }
            
            if (GuiButton((Rectangle){320, 480, 100, 30}, "BACK")) currentScreen = LOGIN;
        }

        // ====================================================================
        // SCREEN 2: DASHBOARD
        // ====================================================================
        else if (currentScreen == DASHBOARD) {
            User& user = SystemUsers[currentUserIndex];
            
            DrawText("DASHBOARD", 400, 50, 40, DARKBLUE);
            DrawText(("User: " + user.GetUserName()).c_str(), 50, 50, 20, GRAY);
            DrawText(message, 400, 100, 20, GREEN);

            // Menu Buttons


            if (GuiButton((Rectangle){350, 200, 300, 60}, "MANAGE ACCOUNTS")) {
                currentScreen = ACCOUNT_MENU;
                snprintf(message, 128, "Select an Account");
            }
            if (GuiButton((Rectangle){350, 280, 300, 60}, "LOAN CENTER")) {
                currentScreen = LOAN_MENU; // Ensure you have this enum logic
                snprintf(message, 128, "Loan Management");
            }

            if (GuiButton((Rectangle){350, 360, 300, 60}, "PROFILE")) {
                currentScreen = PROFILE_MENU;
                // Update stats when entering profile
                user.RecalculateLateOverdue(); 
            }

            if (GuiButton((Rectangle){350, 550, 300, 40}, "LOGOUT")) {
                currentUserIndex = -1;
                currentScreen = LOGIN;
            }
        }

        // ====================================================================
        // SCREEN 3: ACCOUNT MENU (Select + Open New)
        // ====================================================================
        else if (currentScreen == ACCOUNT_MENU) {
            User& user = SystemUsers[currentUserIndex];
            
            DrawText("MY ACCOUNTS", 50, 30, 30, DARKGRAY);

            // --- LEFT PANEL: LIST OF ACCOUNTS ---
            GuiGroupBox((Rectangle){50, 80, 400, 500}, "Select Account to View");
            
            int yPos = 110;
            vector<shared_ptr<BasicBankingAccount>> accounts = user.getBank();

            if (accounts.empty()) DrawText("No Accounts Open", 70, 150, 20, LIGHTGRAY);

            for (auto& acc : accounts) {
                // Determine Type for Label
                string typeLabel = "Account"; 
                // Using dynamic_cast to check type safely
                if (dynamic_pointer_cast<SavingsAccount>(acc)) typeLabel = "[SAVINGS]";
                else if (dynamic_pointer_cast<CurrentAccount>(acc)) typeLabel = "[CURRENT]";
                else if (dynamic_pointer_cast<Fixed_Deposit>(acc)) typeLabel = "[FIXED]";

                // Draw Button for each account
                string btnText = typeLabel + " - " + get<1>(acc->getBankInfo()) + " (" + get<0>(acc->getBankInfo()) + ")";
                
                if (GuiButton((Rectangle){70, (float)yPos, 360, 40}, btnText.c_str())) {
                    selectedAccount = acc; // <--- CAPTURE POINTER
                    currentScreen = ACCOUNT_VIEW; // Go to View Screen
                    snprintf(message, 128, "Viewing Account: %d", acc->getAcc_No());
                    // Clear buffer for transactions
                    strcpy(amountBuffer, "");
                }
                yPos += 50;
            }

            // --- RIGHT PANEL: OPEN NEW ACCOUNT ---
            GuiGroupBox((Rectangle){500, 80, 400, 500}, "Open New Account");
            
            GuiLabel((Rectangle){520, 110, 100, 20}, "Bank Name:");
            if (GuiTextBox((Rectangle){520, 130, 300, 30}, newBank, 64, newBankEdit)) newBankEdit = !newBankEdit;

            GuiLabel((Rectangle){520, 170, 100, 20}, "Initial Deposit:");
            if (GuiTextBox((Rectangle){520, 190, 300, 30}, newInitial, 64, newInitialEdit)) newInitialEdit = !newInitialEdit;

            GuiLabel((Rectangle){520, 230, 100, 20}, "PIN Code:");
            if (GuiTextBox((Rectangle){520, 250, 300, 30}, newPin, 64, newPinEdit)) newPinEdit = !newPinEdit;

            // Type Selector
            GuiLabel((Rectangle){520, 300, 100, 20}, "Account Type:");
            if (GuiDropdownBox((Rectangle){520, 320, 300, 30}, "Savings;Current;Fixed Deposit", &newAccType, newAccTypeEdit)) {
                newAccTypeEdit = !newAccTypeEdit;
            }

            // Conditional Input: Time Period (Only for Savings/Fixed)
            if (newAccType == 0 || newAccType == 2) {
                GuiLabel((Rectangle){520, 360, 200, 20}, "Period (Years):");
                if (GuiTextBox((Rectangle){520, 380, 300, 30}, newPeriod, 64, newPeriodEdit)) newPeriodEdit = !newPeriodEdit;
            }

            if (GuiButton((Rectangle){550, 450, 240, 40}, "CREATE ACCOUNT")) {
                try {
                    double initBal = stod(newInitial);
                    int pin = stoi(newPin);
                    int accNo = rand() % 90000 + 10000; // Simple Random ID
                    string name = user.GetUserName();
                    double period = stod(newPeriod);

                    if (newAccType == 0) { // SAVINGS
                        auto acc = make_shared<SavingsAccount>(newBank, accNo, name, initBal, pin);
                        acc->SetTime(period);
                        user.OpenAccount(acc);
                    }
                    else if (newAccType == 1) { // CURRENT
                        auto acc = make_shared<CurrentAccount>(newBank, accNo, name, initBal, pin);
                        user.OpenAccount(acc);
                    }
                    else if (newAccType == 2) { // FIXED
                        auto acc = make_shared<Fixed_Deposit>(newBank, accNo, name, initBal, pin, (int)period);
                        user.OpenAccount(acc);
                    }
                    snprintf(message, 128, "Account Created Successfully!");
                } catch (...) { snprintf(message, 128, "Invalid Input for Account"); }
            }

            // Back Button
            if (GuiButton((Rectangle){50, 600, 100, 40}, "BACK")) currentScreen = DASHBOARD;
        }

        // ====================================================================
        // SCREEN 4: ACCOUNT VIEW (Specific Operations)
        // ====================================================================
        else if (currentScreen == ACCOUNT_VIEW) {
            
            if (selectedAccount == nullptr) {
                currentScreen = ACCOUNT_MENU; // Safety check
            } else {
                User& user = SystemUsers[currentUserIndex];

                // 1. DISPLAY HEADER INFO
                DrawText("ACCOUNT DETAILS", 350, 30, 30, DARKGRAY);
                
                char header[256];
                snprintf(header, 256, "Bank: %s | Account #: %d", get<0>(selectedAccount->getBankInfo()).c_str(), selectedAccount->getAcc_No());
                DrawText(header, 50, 80, 20, DARKBLUE);
                
                snprintf(header, 256, "Balance: %.2f", selectedAccount->checkBalance());
                DrawText(header, 50, 110, 40, GREEN);

                // 2. POLYMORPHISM DISPLAY (Show extra details based on type)
                // We attempt to cast the pointer. If it works (not null), it's that type.
                if (auto sav = dynamic_pointer_cast<SavingsAccount>(selectedAccount)) {
                    DrawText("[Savings Account Features Active]", 50, 160, 20, ORANGE);
                    DrawText(("Interest Period: " + to_string(sav->getTime()) + " Years").c_str(), 50, 190, 20, GRAY);
                }
                else if (auto curr = dynamic_pointer_cast<CurrentAccount>(selectedAccount)) {
                    DrawText("[Current Account Features Active]", 50, 160, 20, ORANGE);
                    DrawText("Overdraft Facility Available", 50, 190, 20, GRAY);
                }
                else if (auto fix = dynamic_pointer_cast<Fixed_Deposit>(selectedAccount)) {
                     DrawText("[Fixed Deposit - Early Withdraw Penalty Applies]", 50, 160, 20, RED);
                }

                // 3. ACTIONS PANEL
                GuiGroupBox((Rectangle){450, 150, 400, 400}, "Transactions");

                GuiLabel((Rectangle){470, 180, 100, 20}, "Amount:");
                if (GuiTextBox((Rectangle){470, 200, 300, 30}, amountBuffer, 64, amountEdit)) amountEdit = !amountEdit;

                if (GuiButton((Rectangle){470, 250, 140, 40}, "DEPOSIT")) {
                    try {
                        double amt = stod(amountBuffer);
                        selectedAccount->Deposit(amt);
                        snprintf(message, 128, "Deposited %.2f", amt);
                    } catch (...) { snprintf(message, 128, "Invalid Amount"); }
                }

                if (GuiButton((Rectangle){630, 250, 140, 40}, "WITHDRAW")) {
                    try {
                        double amt = stod(amountBuffer);
                        selectedAccount->Withdraw(amt);
                        snprintf(message, 128, "Withdrawn %.2f", amt);
                    } catch (const std::exception& e) { snprintf(message, 128, "%s", e.what()); } 
                    catch (...) { snprintf(message, 128, "Transaction Failed"); }
                }

                // Transfer Section
                GuiLabel((Rectangle){470, 320, 200, 20}, "Transfer To (Acc ID):");
                if (GuiTextBox((Rectangle){470, 340, 300, 30}, targetAccBuffer, 64, targetAccEdit)) targetAccEdit = !targetAccEdit;

                if (GuiButton((Rectangle){470, 390, 300, 40}, "TRANSFER")) {
                    try {
                        int targetID = stoi(targetAccBuffer);
                        double amt = stod(amountBuffer);
                        bool found = false;
                        
                        // SEARCH ALL USERS FOR TARGET
                        // (Realistically you might want to search only this user's accounts, 
                        // but usually transfers are global. This loops EVERYONE.)
                        for (auto& u : SystemUsers) {
                            for (auto& targetBank : u.getBank()) {
                                if (targetBank->getAcc_No() == targetID) {
                                    // Use your helper function logic here
                                    BankTransfer(*selectedAccount, *targetBank, amt);
                                    snprintf(message, 128, "Sent %.2f to ID %d", amt, targetID);
                                    found = true;
                                    break;
                                }
                            }
                            if(found) break;
                        }
                        if (!found) snprintf(message, 128, "Target Account Not Found");
                    } catch (const std::exception& e) { snprintf(message, 128, "Error: %s", e.what()); }
                }

                DrawText(message, 450, 560, 20, BLUE);

                // Back Button
                if (GuiButton((Rectangle){50, 600, 100, 40}, "BACK")) {
                    selectedAccount = nullptr; // Unselect logic
                    currentScreen = ACCOUNT_MENU;
                }
            }
        }

        // ====================================================================
        // SCREEN 5: PROFILE MENU
        // ====================================================================
        else if (currentScreen == PROFILE_MENU) {
            User& user = SystemUsers[currentUserIndex];
            // --- PROFILE VIEW SCREEN ---

            DrawText("USER PROFILE", 20, 20, 20, DARKGRAY);
            DrawText(("Name: " + user.GetUserName()).c_str(), 20, 60, 20, BLACK);
            DrawText(("Citizen ID: " + to_string(get<0>(user.getCredentials()))).c_str(), 20, 90, 20, BLACK);
            DrawText(("Salary: " + to_string(user.CheckSalary())).c_str(), 20, 120, 20, BLACK);
            DrawText(("Credit Score: " + to_string(user.GetCredit())).c_str(), 20, 150, 20, BLACK);

            GuiGroupBox((Rectangle){20, 180, 760, 340}, "CHANGE PIN");  
            if (GuiTextBox((Rectangle){ 470, 190, 280, 30 }, newPin, 64, pinEdit)) {
                pinEdit = !pinEdit;
            }

            if (GuiButton((Rectangle){ 20, 530, 100, 30 }, "Change PIN")) {
                   try {
                        int pin = stoi(newPin);
                        user.updatePin(pin);
                        strcpy(message, "PIN Updated Successfully!");
                        // Clear buffer
                        strcpy(newPin, "");
                    } catch (...) {
                        strcpy(message, "Invalid Input");
                    }    
            }

            
            
            if (GuiButton((Rectangle){ 20, 530, 100, 30 }, "BACK")) {
                currentScreen = DASHBOARD; // Go back to main menu
                strcpy(message, "Dashboard");
            }


        }


        // ====================================================================
        // SCREEN 6: LOAN MENU
        // ====================================================================

        else if (currentScreen == LOAN_MENU) {
        User& use = SystemUsers[currentUserIndex];
        

        DrawText("LOAN MANAGEMENT", 20, 20, 20, DARKGRAY);

        // ---------------------------------------------------------
        // NEW LOAN FORM (Right Side)
        // ---------------------------------------------------------
        GuiGroupBox((Rectangle){ 450, 60, 320, 300 }, "APPLY FOR NEW LOAN");  // Container box

        DrawText("Amount:", 470, 100, 10, GRAY);
        if (GuiTextBox((Rectangle){ 470, 120, 280, 30 }, loanAmount, 64, loanAmountEdit)) {
            loanAmountEdit = !loanAmountEdit;
        }

        DrawText("Period (Months):", 470, 170, 10, GRAY);
        if (GuiTextBox((Rectangle){ 470, 190, 280, 30 }, loanPeriod, 64, loanPeriodEdit)) {
            loanPeriodEdit = !loanPeriodEdit;
        }

        if (GuiButton((Rectangle){ 470, 250, 280, 40 }, "TAKE LOAN")) {
            try {
                double amount = stod(loanAmount);
                int period = stoi(loanPeriod);
                
                // Use your logic functions
                if (use.CanGetLoan()) {
                    // Initialize_Loan(ID, Value, Interest, Paid, Period)
                    // Note: Generate a random ID or use history size + 1
                    Loans l = Initialize_Loan(use.getLoanHistory().size() + 1, amount, 10.0, period);
                    use.AddLoan(l);
                    
                    strcpy(message, "Loan Approved!");
                    // Clear buffers
                    strcpy(loanAmount, "");
                    strcpy(loanPeriod, "");
                } else {
                    strcpy(message, "Credit Score too low!");
                }
            } catch (...) {
                strcpy(message, "Invalid Input");
            }
        }

        // ---------------------------------------------------------
        // HISTORY LIST (Left Side)
        // ---------------------------------------------------------
        GuiGroupBox((Rectangle){ 20, 60, 400, 450 }, "LOAN HISTORY");
        
        int startY = 90;
        vector<Loans> history = use.getLoanHistory();

        if (history.empty()) {
            DrawText("No Loan History Found.", 40, 100, 20, LIGHTGRAY);
        } 
        else {
            // Header
            DrawText("ID      Amount       Due-Date       Status", 30, 75, 10, DARKGRAY);

            for (auto& loan : history) {
                // Build string: "1   5000   20240101"
                string status = loan.paid ? " PAID" : " ACTIVE";
                Color statusColor = loan.paid ? GREEN : RED;
                
                // Display Info
                DrawText(to_string(loan.loan_id).c_str(), 30, startY, 20, BLACK);
                DrawText(to_string((int)loan.value).c_str(), 80, startY, 20, BLACK);
                DrawText(loan.Due_Date.c_str(), 160, startY, 20, BLACK);
                DrawText(status.c_str(), 280, startY, 20, statusColor);

                // "PAY" Button (Only if not paid)
                if (!loan.paid) {
                    if (GuiButton((Rectangle){ 350, (float)startY, 50, 20 }, "    PAY")) {
                        // PAY LOGIC
                        // 1. Find the actual loan in the user object and update it
                        // Since 'loan' is a copy or reference from getLoanHistory(), 
                        // you might need a function in User class like 'PayLoan(id)'
                        // For now, let's assume you add a PayLoan function to User:
                        
                        bool success = use.updateLoans(loan.loan_id); 
                        if(success) strcpy(message, "Loan Paid!");
                        else strcpy(message, "Insufficient Funds!");
                    }
                }

                startY += 30; // Move down for next row
            }
        }

        // ---------------------------------------------------------
        // NAVIGATION
        // ---------------------------------------------------------
        DrawText(message, 450, 400, 20, BLUE); // Status messages

        if (GuiButton((Rectangle){ 20, 530, 100, 30 }, "BACK")) {
            currentScreen = DASHBOARD; // Go back to main menu
            strcpy(message, "Dashboard");
        }
        }








        
        EndDrawing();
    }

    // Cleanup
    SaveAllUsers(SystemUsers); // Auto-save on exit
    CloseWindow();
    return 0;
}