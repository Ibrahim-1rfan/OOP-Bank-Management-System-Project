#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <functional>
#include <cstdlib>
#include <ctime>
#include <sstream>
using namespace std;






bool Compare_dates(string date1, string date2){

 if (date1 > date2) {
        return false;
    } 
 else {
        return true;
    }


};


string GetDueDate(int Period){ 
    time_t now = time(0);
    tm *ltm = localtime(&now);


    int x = 1900 + ltm->tm_year + Period/12;
    string year = to_string(x);

    int y;
    y = (1 + ltm->tm_mon) + Period%12;
   
   
    string Month; 
    if((y)/10 == 0){
        Month = "0" + to_string(y);
    }
    else { Month = to_string(y);}


    string Day;
    if((ltm->tm_mday)/10 == 0){
        Day = "0" + to_string(ltm->tm_mday);
    }
    else {  Day = to_string(ltm->tm_mday);}


    string Date =  year + "-" + Month +"-"+ Day ;
    return Date;
}



std::string getCurrentDate() {
    std::time_t t = std::time(nullptr); // Get current time in seconds since epoch
    std::tm tm = *std::localtime(&t); // Convert to local time structure

    // Format the date into a string (YYYY-MM-DD format)
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}



// Function to set Account Number based on account type
// srand(time(0)) should be seeded in main function only once

int setAccountNo(const int& choose){
    
    int randomPart = (100 + rand() % 900) * 1000 + (100 + rand() % 900);

    int prefix = (110 + choose) * 1000000;

    int AccountNo = prefix + randomPart;

    cout << "Your Account No. is: " << AccountNo << endl;
    return AccountNo;
}


double String_TO_Double(const string& Date){
    if (Date.empty() || Date == "0") return 0.0; // Safety check!

    string s = Date;
    s.erase(remove(s.begin(), s.end(), '-'), s.end());
    s.erase(remove(s.begin(), s.end(), '/'), s.end());

    // Safety check for length (YYYYMMDD = 8 chars)
    if (s.length() < 8) return 0.0; 

    // 2. Setup time structure (Year, Month, Day)
    struct tm t = {0};
    try {
        t.tm_year = stoi(s.substr(0, 4)) - 1900; // mktime expects years since 1900
        t.tm_mon  = stoi(s.substr(4, 2)) - 1;    // mktime expects months 0-11
        t.tm_mday = stoi(s.substr(6, 2));        // Day
    } catch (...) {
        return 0.0;
    }

    // 3. Convert to seconds -> divide by 86400 -> return Days
    return mktime(&t) / 86400.0;
}




// Simple XOR Encryption/Decryption Function
// The Key can be any char, e.g., 'K' or 'X' or even a complex string.
const char SECRET_KEY = 129; 

string EncryptDecrypt(string data) {
    // XOR Encryption: 
    // If you Input "ABC" -> Output might be "%#@"
    // If you Input "%#@" -> Output becomes "ABC"
    
    for (int i = 0; i < data.size(); i++) {
        data[i] = data[i] ^ SECRET_KEY; 
    }
    return data;
}





vector<string> Split(string str, char delimiter) {
    vector<string> tokens;
    string token;
    stringstream ss(str);
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}



// Testing for helper functions
// int main(){


//     std::cout << "Current date: " << getCurrentDate() << std::endl;
//     return 0;

// }