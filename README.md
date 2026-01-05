# 🏦 Bank Management System

A comprehensive, GUI-based Banking Application developed in **C++** using the **Raylib** graphics library. This project demonstrates advanced **Object-Oriented Programming (OOP)** principles, featuring a complete user authentication system, polymorphic account management, loan processing with credit score logic, and persistent data storage.

**Course:** Object Oriented Programming (CS-217)  
**Authors:** Gaurav Khatwani & Ibrahim Irfan

---

## 🚀 Key Features

### 👤 User Management
* **Authentication:** Secure Login and Registration system.
* **Profile & Stats:** Dashboard view of financial status and dynamic Credit Score calculation.
* **Data Persistence:** Custom file serialization ensures all User, Account, and Loan data is saved and encrypted in `BankDatabase.txt`.

### 💰 Account Services
* **Multiple Account Types:**
    * **Savings:** Earns interest over time.
    * **Current:** Supports overdraft facilities.
    * **Fixed Deposit:** Time-locked deposits with maturity logic.
* **Transactions:** seamless Deposit, Withdrawal, and Fund Transfer capabilities.
* **Smart Validation:** Checks for minimum balance, valid IDs, and sufficient funds before processing.

### 📝 Loan Management
* **Credit Logic:** System evaluates user salary and repayment history to determine loan eligibility.
* **Lifecycle:** Apply for loans, track due dates, and make payments.
* **Penalties:** Late payments negatively impact the user's internal Credit Score.

---

## 🛠️ Tech Stack

* **Language:** C++ (C++17 recommended)
* **GUI Library:** [Raylib](https://www.raylib.com/) (with RayGui for UI controls)
* **Compiler:** MinGW / G++
* **Key Libraries:** `vector`, `memory` (Smart Pointers), `fstream`, `string`.

---

## 🧩 OOP Concepts Implemented

This project serves as a practical implementation of core OOP pillars:

1.  **Encapsulation:** Data hiding is enforced using `private` and `protected` members within `User` and `Account` classes.
2.  **Inheritance:**
    * Base Class: `BasicBankingAccount`
    * Derived Classes: `SavingsAccount`, `CurrentAccount`, `Fixed_Deposit`
3.  **Polymorphism:**
    * **Runtime Polymorphism:** Virtual functions are used for account-specific behaviors (e.g., specific withdrawal rules).
    * **Smart Pointers:** `std::vector<std::shared_ptr<BasicBankingAccount>>` allows storing different account types in a single container.
4.  **Exception Handling:** Custom exception classes (e.g., `InsufficientFundsException`) ensure robust error management without crashing.
5.  **Abstraction:** Complex file parsing and encryption logic is hidden behind helper functions.

---

## ⚙️ Installation & Build Instructions

### Prerequisites
1.  A C++ Compiler (MinGW/G++).

### Build Command
Navigate to the project folder in your terminal and run the following command:

```bash
g++ Displaytester.cpp -o BankApp.exe -L. -lraylib -lgdi32 -lwinmm
```
Once compiled run the app via the following command:
```bash
./BankApp.exe



