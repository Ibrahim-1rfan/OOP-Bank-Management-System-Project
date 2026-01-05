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
using namespace std;

double minimum = 5000;

class NegativeAmountException : public std::exception {
public:
    const char* what() const throw() {
        return "The amount entered is negative\n";
    }
};

class InsufficientFundsException : public std::exception {
public:
    const char* what() const throw() {
        return "The amount cannot be withdrawn due to insufficient Account Balance\n";
    }
};

class MinimumBalance : public std::exception {
public:
    const char* what() const throw() {
        return "Initial Balance Deposit should be greater than Minimum Required Amount. \n Cannot open account. Please Enter correct Amount\n ";
    }
};

class AccountNotFoundException : public std::exception {
public:
    const char* what() const throw() {
        return "Invalid AccountID given\n";
    }
};

class UserNotFoundException : public std::exception {
public:
    const char* what() const throw() {
        return "Invalid UserID given\n";
    }
};
