
#include "DBConnection.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <regex>
#include <string>
#include <ctime>
#include <cmath>
#include <cctype>
#include <vector>
#include <algorithm>
#include <fstream>
#include <conio.h>

using namespace std;

// =============== GLOBAL SESSION VARIABLES ===============
string currentEmployeeID;       // Logged-in user's ID
string currentUserRole;         // Role: Admin / Manager / Employee


// =============== GLOBAL INPUT/OUTPUT HELPERS ===============
void showBackReminder() {
    cout << "\n\t\t\t\t\tPress '<' anytime to go back. You will be asked to confirm.\n";
}

bool confirmBack() {
    char confirm;
    cout << "\n\t\t\t\t\tAre you sure you want to go back? (Y/N): ";
    cin >> confirm;
    return (toupper(confirm) == 'Y');
}

bool getInputOrBack(string& input) {
    cin >> input;
    if (input == "<") {
        char confirm;
        cout << "\n\t\t\t\t\tAre you sure you want to go back? (Y/N): ";
        cin >> confirm;
        return (toupper(confirm) != 'Y') && getInputOrBack(input);
    }
    return true;
}

bool getLineOrBack(string& input) {
    getline(cin, input);
    if (input == "<") {
        char confirm;
        cout << "\n\t\t\t\t\tAre you sure you want to go back? (Y/N): ";
        cin >> confirm;
        cin.ignore();
        return (toupper(confirm) != 'Y') && getLineOrBack(input);
    }
    return true;
}

bool getIntOrBack(int& value) {
    string input;
    cin >> input;
    if (input == "<") {
        char confirm;
        cout << "\n\t\t\t\t\tAre you sure you want to go back? (Y/N): ";
        cin >> confirm;
        return (toupper(confirm) != 'Y') && getIntOrBack(value);
    }
    try {
        value = stoi(input);
        return true;
    }
    catch (...) {
        cout << "\t\t\t\t\tInvalid number format.\n";
        return false;
    }
}


// =============== STRING SANITIZATION ===============
string escapeQuotes(const string& input) {
    string escaped;
    for (char c : input) {
        if (c == '\'') escaped += "\\'";
        else if (c == '"') escaped += "\"\"";
        else escaped += c;
    }
    return escaped;
}


// =============== FORMAT VALIDATION ===============
bool isValidDateFormat(const string& date) {
    regex dateRegex(R"(^\d{4}-\d{2}-\d{2}$)");
    return regex_match(date, dateRegex);
}

bool isValidTimeFormat(const string& time) {
    regex timeRegex(R"(^([01]\d|2[0-3]):([0-5]\d)$)");
    return regex_match(time, timeRegex);
}

bool isValidEmail(const string& email) {
    regex pattern(R"(^[\w\.-]+@[\w\.-]+\.\w{2,}$)");
    return regex_match(email, pattern);
}

bool isValidPhone(const string& phone) {
    regex pattern(R"(^\d{10,15}$)");
    return regex_match(phone, pattern);
}

bool isValidPassword(const string& password) {
    const regex passwordPattern(
        "^(?=.*[a-z])"         // At least one lowercase letter
        "(?=.*[A-Z])"          // At least one uppercase letter
        "(?=.*\\d)"            // At least one digit
        "(?=.*[@#$%^&+=!])"    // At least one special character
        ".{8,}$"               // Minimum length of 8 characters
    );
    return regex_match(password, passwordPattern);
}


// =============== WORKDAY VALIDATION ===============
bool isNonWorkingDay(const string& date) {
    // Check if weekend
    string dayQuery = "SELECT DAYOFWEEK('" + date + "')";
    if (mysql_query(conn, dayQuery.c_str()) == 0) {
        MYSQL_RES* dayRes = mysql_store_result(conn);
        MYSQL_ROW dayRow = mysql_fetch_row(dayRes);
        if (dayRow && dayRow[0]) {
            int dayOfWeek = stoi(dayRow[0]);
            mysql_free_result(dayRes);
            if (dayOfWeek == 1 || dayOfWeek == 7) return true;  // Sunday or Saturday
        }
    }

    // Check if public holiday
    string holidayQuery = "SELECT * FROM Public_Holiday WHERE date = '" + date + "'";
    if (mysql_query(conn, holidayQuery.c_str()) == 0) {
        MYSQL_RES* holidayRes = mysql_store_result(conn);
        bool isPH = mysql_num_rows(holidayRes) > 0;
        mysql_free_result(holidayRes);
        if (isPH) return true;
    }

    return false;
}


// ===================== AUTHENTICATION =====================
void login();                              // Login for all users
void registerUser();                       // Registration form
void displayWelcomeScreen();               // Welcome splash screen


// ===================== ROLE-BASED MODULES =================
void adminModule();                        // Admin main menu
void managerModule();                      // Manager main menu
void employeeModule();                     // Employee main menu


// ========================= ADMIN ==========================
void manageEmployees();                    // View/Edit/Delete Employees
void editEmployee();                       // Edit employee profile
void manageDepartments();                  // Department creation & editing
void manageLeave();                        // Admin leave features               
void adminViewAllLeaveLogs();              // View all leave approvals
void manageHolidays();                     // Create/edit holiday dates
void changePassword();                     // Admin change user password
void resetLeaveBalance();                  // Reset leave annually
void adminReports();                       // Admin reports menu
void adminManageLeaveRequests();           // View/edit all leave requests
void adminViewLeaveBalances();


// ======================== MANAGER =========================
void managerManageLeave();                 // Manager leave options
void applyLeaveOnBehalf();                 // Apply leave for employee
void viewPendingLeave();                   // Manager view pending requests
void updateLeaveStatus();                  // Approve/reject leave
void viewTeamAttendance();                 // View team attendance
void viewFrequentHalfDays();               // View frequent half-day staff
void assignStaffToDepartment();            // Assign to department
void viewLeaveLog();                       // View leave history (team)
void editLeaveRequest();                   // Edit pending team leave
void deleteLeaveRequest();                 // Delete team leave request
void DeptviewLeaveBalance();               // View leave balance (dept only)
void viewTeamLeaveLog();                   // Detailed leave log by team
void exportTeamLeaveSummary();             // Export team leave CSV
void managerReports();                     // Manager reports menu


// ======================== EMPLOYEE ========================
void clockIn();                            // Clock-in time
void clockOut();                           // Clock-out time
void applyLeave();                         // Submit leave request
void viewLeaveHistory();                   // View personal leave records
void viewLeaveBalance();                   // See current leave balance
void editMyProfile();                      // Edit personal details
void viewAttendanceHistory();              // View clock in/out log


// ========================= REPORTS ========================
void generateReports();                    // (Optional general report)
void exportAttendance();                   // Export attendance to CSV
void exportLeaveSummary();                 // Export leave summary CSV
void exportLeaveLogToCSV();                // Export full leave log
void exportLeaveDataToCSV();               // Clean formatted CSV
void viewAttendanceReport();               // All attendance (admin)
void grantBonusLeave();                    // Bonus leave for OT staff
void generateLeaveStats();                 // Monthly usage, mode, variance

struct EmployeeProfile {
    string EmployeeID;
    string Name;
    string Email;
    string ContactNumber;
    string Role;
};

int main() {
    conn = initializeConnection();
    if (!conn) return 1;

    displayWelcomeScreen();

    bool autoCommit = mysql_autocommit(conn, 1);
    if (autoCommit) {
        cout << "\t\t\t\t\tAuto-commit is enabled." << endl;
    }
    else {
        cout << "\t\t\t\t\tAuto-commit is disabled." << endl;
    }

    string choiceStr;
    int choice;

    while (true) {
        system("cls");
        showBackReminder();
        cout << "\n\n\n";
        cout << "\t\t\t\t\t                                 M A I N   M E N U                               \n";
        cout << "\t\t\t\t\t________________________________________________________________________________\n";
        cout << "\t\t\t\t\t|                                                                              |\n";
        cout << "\t\t\t\t\t|               1. LOGIN           2. REGISTER          3. EXIT                |\n";
        cout << "\t\t\t\t\t|______________________________________________________________________________|\n";
        cout << "\t\t\t\t\tSelect: ";
        cin >> choiceStr;

        if (choiceStr == "<") {
            char confirm;
            cout << "\n\t\t\t\t\tAre you sure you want to go back to welcome screen? (Y/N): ";
            cin >> confirm;
            if (toupper(confirm) == 'Y') {
                system("cls");
                displayWelcomeScreen();
                continue;
            }
        }

        try {
            choice = stoi(choiceStr);
        }
        catch (...) {
            cout << "\t\t\t\t\tInvalid input. Try again.\n";
            system("pause");
            continue;
        }

        switch (choice) {
        case 1:
            login();
            break;
        case 2:
            registerUser();
            break;
        case 3:
            cout << "\t\t\t\t\tExiting program...\n";
            mysql_close(conn);
            return 0;
        default:
            cout << "\t\t\t\t\tInvalid choice. Please try again.\n";
            system("pause");
        }
    }
}

// Function to display the "WELCOME"
void displayWelcomeScreen() {
    system("cls");
    cout << "\n\n\n\n\n";

    cout << "\t\t\t\t\t                                                                                             \n";
    cout << "\t\t\t\t\t   [x]         [x]   [x][x][x]   [x]         [x][x]   [x][x]    [x]       [x]   [x][x][x]    \n";
    cout << "\t\t\t\t\t   [x]         [x]   [x]         [x]        [x]      [x]  [x]   [x][x] [x][x]   [x]          \n";
    cout << "\t\t\t\t\t   [x]  [x]    [x]   [x][x][x]   [x]        [x]      [x]  [x]   [x] [x][x][x]   [x][x][x]    \n";
    cout << "\t\t\t\t\t   [x] [x][x]  [x]   [x]         [x]        [x]      [x]  [x]   [x]  [x]  [x]   [x]          \n";
    cout << "\t\t\t\t\t   [x][x]   [x][x]   [x][x][x]   [x][x][x]   [x][x]   [x][x]    [x]       [x]   [x][x][x]    \n";

    cout << "\t\t\t\t\t                                                                                             \n";
    cout << "\t\t\t\t\t                                   WELCOME TO                                                \n";
    cout << "\t\t\t\t\t                     ATTENDANCE AND LEAVE MANAGEMENT SYSTEM                                  \n";

    cout << "\n\n";
    cout << "\t\t\t\t\t   Press Enter to continue..." << endl;

    cin.get();  // Wait for Enter
    cin.ignore(); // Clear buffer
}



// Login function
void login() {
    string username, password;
    system("cls");

    cout << "\n\n\n\n";
    cout << "\t\t\t\t\t___________________________________________________________________\n";
    cout << "\t\t\t\t\t|                                                                 |\n";
    cout << "\t\t\t\t\t|                           L O G I N                             |\n";
    cout << "\t\t\t\t\t|_________________________________________________________________|\n";
    cout << "\t\t\t\t\tEnter '<' to cancel and go back to Main Menu\n\n";

    cout << "\t\t\t\t\tEnter Username: ";
    if (!getInputOrBack(username)) return;

    // Password Input with Masking
    cout << "\t\t\t\t\tPassword (e.g., P@ssw0rd123!): ";
    char ch;
    password = "";
    while (true) {
        ch = _getch();
        if (ch == 13) break;  // Enter key
        else if (ch == 8 && !password.empty()) {
            cout << "\b \b";
            password.pop_back();
        }
        else if (isprint(ch)) {
            password += ch;
            cout << '*';
        }
    }
    cout << endl;

    // Query user credentials
    string query =
        "SELECT employee_id, user_type "
        "FROM Employee "
        "WHERE username='" + username + "' AND password='" + password + "'";

    qstate = mysql_query(conn, query.c_str());

    if (!qstate) {
        res = mysql_store_result(conn);
        if (mysql_num_rows(res) > 0) {
            row = mysql_fetch_row(res);
            if (row && row[0] && row[1]) {
                currentEmployeeID = row[0];
                currentUserRole = row[1] ? row[1] : "";

                cout << "\t\t\t\t\tLogin successful! Role: " << currentUserRole << endl;
                system("pause");

                // Normalize role string (trim spaces + lowercase)
                currentUserRole.erase(remove_if(currentUserRole.begin(), currentUserRole.end(), ::isspace), currentUserRole.end());
                transform(currentUserRole.begin(), currentUserRole.end(), currentUserRole.begin(), ::tolower);

                if (currentUserRole == "admin") {
                    adminModule();
                }
                else if (currentUserRole == "manager") {
                    managerModule();
                }
                else if (currentUserRole == "staff") {
                    cout << "\t\t\t\t\tRedirecting to EMPLOYEE MODULE..." << endl;
                    system("pause");
                    employeeModule();
                }
                else {
                    cout << "\t\t\t\t\tUnknown role: [" << currentUserRole << "]\n";
                    system("pause");
                }
            }
            else {
                cout << "\t\t\t\t\tInvalid user data format.\n";
                system("pause");
            }
        }
        else {
            cout << "\t\t\t\t\tInvalid username or password. Please try again.\n";
            system("pause");
        }
        mysql_free_result(res);
    }
    else {
        cout << "\t\t\t\t\tError: " << mysql_error(conn) << endl;
        system("pause");
    }
}


// RegisterUser function
void registerUser() {
    string username, password, role, name, email, phoneNo;

    system("cls");
    showBackReminder();
    cout << "\n\n\n\n";
    cout << "\t\t\t\t\t______________________________________________________________________________\n";
    cout << "\t\t\t\t\t|                                                                             |\n";
    cout << "\t\t\t\t\t|                           R E G I S T R A T I O N                           |\n";
    cout << "\t\t\t\t\t|_____________________________________________________________________________|\n";
    cout << "\t\t\t\t\tEnter '<' anytime to go back to Main Menu.\n\n";

    cout << "\t\t\t\t\tEnter Username: ";
    if (!getInputOrBack(username)) return;

    do {
        cout << "\t\t\t\t\tPassword (e.g., P@ssw0rd123!): ";
        char ch;
        password = "";
        while (true) {
            ch = _getch();
            if (ch == 13) break; // ENTER
            else if (ch == 8 && !password.empty()) { // BACKSPACE
                cout << "\b \b";
                password.pop_back();
            }
            else if (isprint(ch)) {
                password += ch;
                cout << '*';
            }
        }
        cout << endl;

        if (password == "<") return;

        if (!isValidPassword(password)) {
            cout << "\n\t\t\t\t\tPassword must contain:\n"
                << "\t\t\t\t\t- At least 1 uppercase letter\n"
                << "\t\t\t\t\t- At least 1 lowercase letter\n"
                << "\t\t\t\t\t- At least 1 number\n"
                << "\t\t\t\t\t- At least 1 special character (!@#$%^&*)\n"
                << "\t\t\t\t\t- Minimum length: 8 characters\n";
        }
    } while (!isValidPassword(password));

    cout << "\t\t\t\t\tRole (admin/manager/staff): ";
    if (!getInputOrBack(role)) return;
    transform(role.begin(), role.end(), role.begin(), ::tolower);
    if (role == "employee") role = "staff";

    if (role != "admin" && role != "manager" && role != "staff") {
        cout << "\t\t\t\t\tInvalid role entered.\n";
        system("pause");
        return;
    }

    cin.ignore();
    cout << "\t\t\t\t\tFull Name: ";
    if (!getLineOrBack(name)) return;

    cout << "\t\t\t\t\tEmail: ";
    if (!getLineOrBack(email)) return;

    cout << "\t\t\t\t\tPhone No: ";
    if (!getLineOrBack(phoneNo)) return;

    string deptID;
    if (role == "manager") {
        cout << "\t\t\t\t\tDepartment ID (e.g., DEP001): ";
        if (!getLineOrBack(deptID)) return;
    }

    regex emailPattern(".+@.+\\..+");
    if (!regex_match(email, emailPattern)) {
        cout << "\t\t\t\t\tInvalid email format.\n";
        system("pause");
        return;
    }

    char confirm;
    cout << "\n\t\t\t\t\tDo you want to proceed with registration? (Y/N): ";
    cin >> confirm;
    if (toupper(confirm) != 'Y') {
        cout << "\t\t\t\t\tRegistration cancelled.\n";
        system("pause");
        return;
    }

    cout << "\t\t\t\t\tRegistered as role: [" << role << "]\n";

    string query;
    if (role == "manager") {
        query = "INSERT INTO Employee(username, password, employee_name, contact_number, email, user_type, department_id) VALUES('" +
            username + "', '" + password + "', '" + name + "', '" + phoneNo + "', '" + email + "', '" + role + "', '" + deptID + "')";
    }
    else {
        query = "INSERT INTO Employee(username, password, employee_name, contact_number, email, user_type) VALUES('" +
            username + "', '" + password + "', '" + name + "', '" + phoneNo + "', '" + email + "', '" + role + "')";
    }

    qstate = mysql_query(conn, query.c_str());
    if (!qstate) {
        cout << "\t\t\t\t\tRegistration successful!\n";

        string getIDQuery = "SELECT employee_id FROM Employee WHERE username='" + username + "'";
        qstate = mysql_query(conn, getIDQuery.c_str());
        if (!qstate) {
            res = mysql_store_result(conn);
            if ((row = mysql_fetch_row(res))) {
                string newEmpID = row[0];
                mysql_free_result(res);

                if (role == "admin") {
                    string adminInsert = "INSERT INTO Admin(employee_id, admin_level, created_by) VALUES('" + newEmpID + "', 'standard', '" + currentEmployeeID + "')";
                    mysql_query(conn, adminInsert.c_str());
                }
                else if (role == "manager") {
                    string mgrInsert = "INSERT INTO Manager(employee_id, department_id, manager_name) VALUES('" + newEmpID + "', '" + deptID + "', '" + name + "')";
                    mysql_query(conn, mgrInsert.c_str());
                }

                string balanceQuery =
                    "INSERT INTO Leave_Balance (employee_id, leave_type_id, year, total_days, used_days, remaining_days) "
                    "SELECT '" + newEmpID + "', leave_type_id, YEAR(CURDATE()), max_days, 0, max_days "
                    "FROM Leave_Type "
                    "WHERE NOT EXISTS ("
                    "  SELECT * FROM Leave_Balance "
                    "  WHERE employee_id='" + newEmpID + "' "
                    "    AND leave_type_id = Leave_Type.leave_type_id "
                    "    AND year = YEAR(CURDATE())"
                    ")";
                mysql_query(conn, balanceQuery.c_str());
            }
        }
    }
    else {
        cout << "\t\t\t\t\tError: " << mysql_error(conn) << endl;
    }

    system("pause");
}



// Function to mask password input
string getPasswordInput() {
    string password;
    char ch;

    // Print initial tab alignment for input line
    cout << "\t\t\t\t\t";

    while ((ch = _getch()) != '\r') { // Enter key
        if (ch == '\b') { // Backspace
            if (!password.empty()) {
                // Move back, print space, move back again
                cout << "\b \b";
                password.pop_back();
            }
        }
        else {
            password += ch;
            cout << '*'; // Print masked character
        }
    }
    cout << endl;
    return password;
}

// admin_module.cpp 
void adminModule() {
    string input;

    while (true) {
        system("cls");
        showBackReminder();

        cout << "\n\n\n";
        cout << "\t\t\t\t\t_____________________________________________________________\n";
        cout << "\t\t\t\t\t|                                                           |\n";
        cout << "\t\t\t\t\t|                    A D M I N   M E N U                    |\n";
        cout << "\t\t\t\t\t|___________________________________________________________|\n";
        cout << "\t\t\t\t\t|                                                           |\n";
        cout << "\t\t\t\t\t|   1. Manage Employees                                     |\n";
        cout << "\t\t\t\t\t|   2. Manage Leave                                         |\n";
        cout << "\t\t\t\t\t|   3. Manage Departments                                   |\n";
        cout << "\t\t\t\t\t|   4. Manage Holidays                                      |\n";
        cout << "\t\t\t\t\t|   5. Reports                                              |\n";
        cout << "\t\t\t\t\t|   6. Logout                                               |\n";
        cout << "\t\t\t\t\t|   <. Exit                                                 |\n";
        cout << "\t\t\t\t\t|___________________________________________________________|\n";
        cout << "\t\t\t\t\tEnter your choice: ";
        cin >> input;

        if (input == "<") {
            if (confirmBack()) return;
            else continue;
        }

        int choice;
        try {
            choice = stoi(input);
        }
        catch (...) {
            cout << "\t\t\t\t\tInvalid input. Try again.\n";
            system("pause");
            continue;
        }

        switch (choice) {
        case 1: manageEmployees(); break;
        case 2: manageLeave(); break;
        case 3: manageDepartments(); break;
        case 4: manageHolidays(); break;
        case 5: adminReports(); break;
        case 6: {
            char confirm;
            cout << "\t\t\t\t\tAre you sure you want to logout? (Y/N): ";
            cin >> confirm;
            if (toupper(confirm) == 'Y') {
                login();
                return;
            }
            else {
                cout << "\t\t\t\t\tLogout cancelled.\n";
                system("pause");
            }
            break;
        }
        default:
            cout << "\t\t\t\t\tInvalid choice. Try again.\n";
            system("pause");
        }
    }
}



// === EMPLOYEE MANAGEMENT ===
void manageEmployees() {
    string input;
    while (true) {
        system("cls");
        showBackReminder();
        cout << "\n\n\n";
        cout << "\t\t\t\t\t___________________________________________________________\n";
        cout << "\t\t\t\t\t|                                                         |\n";
        cout << "\t\t\t\t\t|             M A N A G E   E M P L O Y E E S             |\n";
        cout << "\t\t\t\t\t|_________________________________________________________|\n";
        cout << "\t\t\t\t\t|                                                         |\n";
        cout << "\t\t\t\t\t|   1. View All Employees                                 |\n";
        cout << "\t\t\t\t\t|   2. Add New Employee                                   |\n";
        cout << "\t\t\t\t\t|   3. Edit Employee Profile                              |\n";
        cout << "\t\t\t\t\t|   4. Delete Employee                                    |\n";
        cout << "\t\t\t\t\t|   5. Back to Admin Menu                                 |\n";
        cout << "\t\t\t\t\t|   <. Cancel / Go Back                                   |\n";
        cout << "\t\t\t\t\t|_________________________________________________________|\n";
        cout << "\t\t\t\t\tChoose: ";
        cin >> input;
        if (input == "<") {
            if (confirmBack()) return;
            else continue;
        }
        // 👇 CLEAR BAD INPUT
        cin.clear();
        cin.ignore(10000, '\n');
        int opt;
        try { opt = stoi(input); }
        catch (...) {
            cout << "\t\t\t\t\tInvalid input.\n";
            system("pause");
            continue;
        }
        string query;
        switch (opt) {
        case 1:
            query = "SELECT employee_id, employee_name, email, contact_number, user_type FROM Employee WHERE is_active=1";
            qstate = mysql_query(conn, query.c_str());

            if (!qstate) {
                res = mysql_store_result(conn);

                cout << left
                    << setw(12) << "ID"
                    << setw(25) << "Name"
                    << setw(30) << "Email"
                    << setw(15) << "Contact"
                    << setw(10) << "Role" << endl;

                cout << string(92, '-') << endl;

                while ((row = mysql_fetch_row(res))) {
                    cout << setw(12) << (row[0] ? row[0] : "-")
                        << setw(25) << (row[1] ? row[1] : "-")
                        << setw(30) << (row[2] ? row[2] : "-")
                        << setw(15) << (row[3] ? row[3] : "-")
                        << setw(10) << (row[4] ? row[4] : "-") << endl;
                }

                mysql_free_result(res);
                system("pause"); // <-- Add this line
            }
            else {
                cout << "\t\tQuery Error: " << mysql_error(conn) << endl;
                system("pause");
            }
            break;

        case 2: {

            string name, email, phone, username, password, role;
            showBackReminder();
            cout << "\t\t\t\t\tEnter Full Name (< to cancel): "; cin.ignore(); getline(cin, name);
            if (!getLineOrBack(name)) return;
            cout << "\t\t\t\t\tEmail: "; cin >> email;
            if (email == "<") return;
            cout << "\t\t\t\t\tContact No: "; cin >> phone;
            if (phone == "<") return;
            cout << "\t\t\t\t\tUsername: "; cin >> username;
            if (username == "<") return;
            cout << "\t\t\t\t\tPassword: "; cin >> password;
            if (password == "<") return;
            cout << "\t\t\t\t\tRole (admin/manager/staff): "; cin >> role;
            if (role == "<") return;
            if (role != "admin" && role != "manager" && role != "staff") {
                cout << "\t\t\t\t\tInvalid role. Must be admin, manager, or staff.\n";
                system("pause");
                return;
            }
            char confirm;
            cout << "\n\t\t\t\t\tConfirm adding this employee? (Y/N): ";
            cin >> confirm;
            if (toupper(confirm) != 'Y') {
                cout << "\t\t\t\t\tAdd employee cancelled.\n";
                system("pause");
                return;
            }
            query = "INSERT INTO Employee(username, employee_name, contact_number, email, password, user_type) VALUES('"
                + username + "', '" + name + "', '" + phone + "', '" + email + "', '" + password + "', '" + role + "')";

            qstate = mysql_query(conn, query.c_str());
            if (!qstate) {
                cout << "\t\tEmployee added successfully!\n";

                // Get last inserted employee_id
                query = "SELECT employee_id FROM Employee ORDER BY employee_id DESC LIMIT 1";
                qstate = mysql_query(conn, query.c_str());
                if (!qstate) {
                    res = mysql_store_result(conn);
                    if ((row = mysql_fetch_row(res))) {
                        string newEmpID = row[0];

                        // === Insert into admin if admin
                        if (role == "admin") {
                            string adminInsert = "INSERT INTO admin (employee_id, admin_level) VALUES('" + newEmpID + "', 'standard')";
                            qstate = mysql_query(conn, adminInsert.c_str());
                            if (!qstate) cout << "\t\t\t\t\tAdmin privileges granted.\n";
                            else cout << "\t\t\t\t\tAdmin insert error: " << mysql_error(conn) << endl;
                        }

                        // === Insert into manager if manager
                        else if (role == "manager") {
                            // List departments
                            cout << "\t\tAvailable Departments:\n";
                            string dquery = "SELECT department_id, department_name FROM Department";
                            if (mysql_query(conn, dquery.c_str()) == 0) {
                                res = mysql_store_result(conn);
                                while ((row = mysql_fetch_row(res))) {
                                    cout << "\t\t\t\t\t" << row[0] << " - " << row[1] << endl;
                                }
                                mysql_free_result(res);
                            }

                            string deptID;
                            cout << "\t\t\t\t\tEnter Department ID for manager: ";
                            cin >> deptID;

                            string mgrInsert = "INSERT INTO manager (employee_id, manager_name, department_id) "
                                "SELECT '" + newEmpID + "', employee_name, '" + deptID + "' FROM employee WHERE employee_id = '" + newEmpID + "'";
                            qstate = mysql_query(conn, mgrInsert.c_str());
                            if (!qstate) cout << "\t\t\t\t\tManager privileges granted.\n";
                            else cout << "\t\t\t\t\tManager insert error: " << mysql_error(conn) << endl;
                        }

                        // === Leave balance should be initialized for all roles
                        string lbQuery = "INSERT INTO leave_balance (employee_id, leave_type_id, total_days, used_days, remaining_days, year) "
                            "SELECT '" + newEmpID + "', leave_type_id, max_days, 0, max_days, YEAR(CURDATE()) FROM leave_type";
                        qstate = mysql_query(conn, lbQuery.c_str());
                        if (!qstate)
                            cout << "\t\t\t\t\tLeave balances initialized.\n";
                        else
                            cout << "\t\t\t\t\tLeave balance error: " << mysql_error(conn) << endl;
                    }
                    mysql_free_result(res);
                }

            }
            else {
                cout << "\t\t\t\t\tError: " << mysql_error(conn) << endl;
            }
            system("pause");
            break;
        }

        case 3: editEmployee(); break;

        case 4: {
            string empID;
            showBackReminder();
            system("cls");

            cout << "\n\t\t\t\t\t[Current Active Employees]\n";
            string q = "SELECT employee_id, employee_name FROM Employee WHERE is_active=1";
            if (mysql_query(conn, q.c_str()) == 0) {
                res = mysql_store_result(conn);
                while ((row = mysql_fetch_row(res))) {
                    cout << "\t\t\t\t\t" << row[0] << " - " << row[1] << "\n";
                }
                mysql_free_result(res);
            }
            else {
                cout << "\t\t\t\t\tError retrieving employees: " << mysql_error(conn) << endl;
                system("pause");
                return;
            }

            cout << "\t\t\t\t\tEnter Employee ID to delete (< to cancel): ";
            cin >> empID;
            if (empID == "<") return;

            if (empID == currentEmployeeID) {
                cout << "\t\t\t\t\tYou cannot delete your own account while logged in.\n";
                system("pause");
                return;
            }

            // Check if employee exists
            string checkQuery = "SELECT employee_name FROM Employee WHERE employee_id = '" + empID + "'";
            if (mysql_query(conn, checkQuery.c_str()) != 0) {
                cout << "\t\t\t\t\tError: " << mysql_error(conn) << endl;
                system("pause");
                return;
            }

            res = mysql_store_result(conn);
            if (!res || mysql_num_rows(res) == 0) {
                cout << "\t\t\t\t\tEmployee not found.\n";
                mysql_free_result(res);
                system("pause");
                return;
            }

            row = mysql_fetch_row(res);
            string empName = row[0];
            mysql_free_result(res);

            // Confirm
            char confirm;
            cout << "\n\t\t\t\t\tAre you sure you want to PERMANENTLY delete " << empName << "? (Y/N): ";
            cin >> confirm;
            if (toupper(confirm) != 'Y') {
                cout << "\t\t\t\t\tDeletion cancelled.\n";
                system("pause");
                return;
            }

            // Delete employee and related records
            string deleteQuery =
                "UPDATE Employee "
                "SET is_active = 0 "
                "WHERE employee_id = '" + empID + "'";
            if (mysql_query(conn, deleteQuery.c_str()) == 0)
                cout << "\t\t\t\t\tEmployee deactivated successfully.\n";
            else
                cout << "\t\t\t\t\tDeletion failed: " << mysql_error(conn) << endl;

            system("pause");
            break;
        }


        case 5: return; // back to adminModule
        default:
            cout << "\t\t\t\t\tInvalid choice. Try again.\n";
            system("pause");
        }
    }
}

void editEmployee() {
    string empID;
    system("cls");
    showBackReminder();

    cout << "\n\t\t\t\t\t=========== CURRENT ACTIVE EMPLOYEES ===========\n";

    string q = "SELECT employee_id, employee_name, email, contact_number, user_type FROM Employee WHERE is_active=1";
    mysql_query(conn, q.c_str());
    res = mysql_store_result(conn);

    while ((row = mysql_fetch_row(res))) {
        cout << "\t\t\t\t\t" << row[0] << " - " << row[1] << " | " << row[2] << " | " << row[3] << " | " << row[4] << "\n";
    }
    mysql_free_result(res);

    cout << "\n\t\t\t\t\tEnter Employee ID to edit (< to cancel): ";
    cin >> empID;
    if (empID == "<") return;

    q = "SELECT employee_name, email, contact_number, user_type FROM Employee WHERE employee_id = '" + empID + "' AND is_active = 1";
    if (mysql_query(conn, q.c_str()) != 0) {
        cout << "\t\t\t\t\tQuery error: " << mysql_error(conn) << endl;
        system("pause");
        return;
    }

    res = mysql_store_result(conn);
    if (!res || mysql_num_rows(res) == 0) {
        cout << "\t\t\t\t\tEmployee not found.\n";
        mysql_free_result(res);
        system("pause");
        return;
    }

    row = mysql_fetch_row(res);
    string currName = row[0], currEmail = row[1], currPhone = row[2], currRole = row[3];
    mysql_free_result(res);

    string newName = currName, newEmail = currEmail, newPhone = currPhone, newRole = currRole;
    int choice;

    do {
        system("cls");
        showBackReminder();
        cout << "\n\t\t\t\t\t_____________________________________________________\n";
        cout << "\t\t\t\t\t|                                                   |\n";
        cout << "\t\t\t\t\t|               E D I T   E M P L O Y E E            |\n";
        cout << "\t\t\t\t\t|___________________________________________________|\n";
        cout << "\t\t\t\t\t|   1. Name   : " << setw(35) << left << newName << "|\n";
        cout << "\t\t\t\t\t|   2. Email  : " << setw(35) << left << newEmail << "|\n";
        cout << "\t\t\t\t\t|   3. Contact: " << setw(35) << left << newPhone << "|\n";
        cout << "\t\t\t\t\t|   4. Role   : " << setw(35) << left << newRole << "|\n";
        cout << "\t\t\t\t\t|   5. Save changes                                  |\n";
        cout << "\t\t\t\t\t|   6. Cancel                                        |\n";
        cout << "\t\t\t\t\t|___________________________________________________|\n";
        cout << "\t\t\t\t\tChoose field to edit: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            cout << "\t\t\t\t\tEnter new name: ";
            if (!getLineOrBack(newName)) return;
            break;
        case 2:
            do {
                cout << "\t\t\t\t\tEnter new email: ";
                if (!getLineOrBack(newEmail)) return;
                regex emailPattern(".+@.+\\..+");
                if (!regex_match(newEmail, emailPattern)) {
                    cout << "\t\t\t\t\tInvalid email format. Try again.\n";
                }
                else break;
            } while (true);
            break;
        case 3:
            cout << "\t\t\t\t\tEnter new contact number: ";
            if (!getLineOrBack(newPhone)) return;
            break;
        case 4:
            do {
                cout << "\t\t\t\t\tEnter new role (admin/manager/staff): ";
                if (!getLineOrBack(newRole)) return;
                if (newRole != "admin" && newRole != "manager" && newRole != "staff") {
                    cout << "\t\t\t\t\tInvalid role. Try again.\n";
                }
                else break;
            } while (true);
            break;
        case 5:
            break;
        case 6:
            cout << "\t\t\t\t\tEdit cancelled.\n";
            system("pause");
            return;
        default:
            cout << "\t\t\t\t\tInvalid option.\n";
            system("pause");
            break;
        }
    } while (choice != 5);

    char confirm;
    cout << "\n\t\t\t\t\tSave changes? (Y/N): ";
    cin >> confirm;
    if (toupper(confirm) != 'Y') {
        cout << "\t\t\t\t\tUpdate cancelled.\n";
        system("pause");
        return;
    }

    string updateQuery = "UPDATE Employee SET employee_name='" + newName +
        "', email='" + newEmail +
        "', contact_number='" + newPhone +
        "', user_type='" + newRole +
        "' WHERE employee_id='" + empID + "'";

    if (mysql_query(conn, updateQuery.c_str()) == 0)
        cout << "\n\t\t\t\t\tUpdate successful.\n";
    else
        cout << "\n\t\t\t\t\tUpdate failed: " << mysql_error(conn) << endl;

    system("pause");
}

void showEmployeeList() {
    string q = "SELECT employee_id, employee_name, user_type FROM Employee WHERE is_active=1";

    if (mysql_query(conn, q.c_str()) != 0) {
        cout << "\t\t\t\t\tQuery failed: " << mysql_error(conn) << "\n";
        system("pause");
        return;
    }

    res = mysql_store_result(conn);
    if (!res) {
        cout << "\t\t\t\t\tFailed to retrieve result set.\n";
        system("pause");
        return;
    }

    system("cls");
    showBackReminder();

    cout << "\n\t\t\t\t\t________________________________________________________\n";
    cout << "\t\t\t\t\t|                                                      |\n";
    cout << "\t\t\t\t\t|            L I S T   O F   A C T I V E   S T A F F    |\n";
    cout << "\t\t\t\t\t|______________________________________________________|\n\n";

    cout << left
        << "\t\t\t\t\t" << setw(12) << "ID"
        << setw(25) << "Name"
        << setw(10) << "Role" << "\n";
    cout << "\t\t\t\t\t" << string(50, '-') << "\n";

    while ((row = mysql_fetch_row(res))) {
        cout << "\t\t\t\t\t"
            << setw(12) << (row[0] ? row[0] : "-")
            << setw(25) << (row[1] ? row[1] : "-")
            << setw(10) << (row[2] ? row[2] : "-") << "\n";
    }

    mysql_free_result(res);

    system("pause");
}

// === DEPARTMENT MANAGEMENT ===
void manageDepartments() {
    string input;
    while (true) {
        system("cls");
        showBackReminder();
        cout << "\n\n\n";
        cout << "\t\t\t\t\t________________________________________________________\n";
        cout << "\t\t\t\t\t|                                                      |\n";
        cout << "\t\t\t\t\t|            M A N A G E   D E P A R T M E N T S        |\n";
        cout << "\t\t\t\t\t|______________________________________________________|\n";
        cout << "\t\t\t\t\t|                                                      |\n";
        cout << "\t\t\t\t\t|   1. View Departments                                |\n";
        cout << "\t\t\t\t\t|   2. Add Department                                  |\n";
        cout << "\t\t\t\t\t|   3. Edit Department                                 |\n";
        cout << "\t\t\t\t\t|   4. Delete Department                               |\n";
        cout << "\t\t\t\t\t|   5. Back to Admin Menu                              |\n";
        cout << "\t\t\t\t\t|   <. Cancel / Go Back                                |\n";
        cout << "\t\t\t\t\t|______________________________________________________|\n";
        cout << "\t\t\t\t\tChoose: ";
        cin >> input;

        if (input == "<") {
            if (confirmBack()) return;
            else continue;
        }

        int opt;
        try { opt = stoi(input); }
        catch (...) {
            cout << "\t\t\t\t\tInvalid input.\n";
            system("pause");
            continue;
        }

        string query;
        switch (opt) {
        case 1: {
            query = "SELECT department_id, department_name FROM Department";
            qstate = mysql_query(conn, query.c_str());
            if (!qstate) {
                res = mysql_store_result(conn);
                cout << "\n\t\t\t\t\t========== DEPARTMENTS ==========\n";
                cout << left << "\t\t\t\t\t" << setw(10) << "Dept ID" << setw(30) << "Name" << endl;
                cout << "\t\t\t\t\t" << string(40, '-') << endl;
                while ((row = mysql_fetch_row(res))) {
                    cout << "\t\t\t\t\t" << setw(10) << row[0] << setw(30) << row[1] << endl;
                }
                mysql_free_result(res);
            }
            else {
                cout << "\t\t\t\t\tQuery Error: " << mysql_error(conn) << endl;
            }
            break;
        }

        case 2: {
            showBackReminder();
            string name;
            cout << "\t\t\t\t\tEnter Department Name (< to cancel): ";
            cin.ignore();
            getline(cin, name);
            if (name == "<") break;

            char confirm;
            cout << "\t\t\t\t\tConfirm adding this department? (Y/N): ";
            cin >> confirm;
            if (toupper(confirm) != 'Y') {
                cout << "\t\t\t\t\tAdd department cancelled.\n";
                break;
            }

            query = "INSERT INTO Department(department_name) VALUES('" + name + "')";
            qstate = mysql_query(conn, query.c_str());
            if (!qstate)
                cout << "\t\t\t\t\tDepartment added successfully.\n";
            else
                cout << "\t\t\t\t\tError: " << mysql_error(conn) << endl;
            break;
        }

        case 3: {
            system("cls");
            showBackReminder();

            string q = "SELECT department_id, department_name, description FROM Department";
            if (mysql_query(conn, q.c_str()) == 0) {
                res = mysql_store_result(conn);
                cout << "\n\t\t\t\t\t========= Existing Departments =========\n";
                while ((row = mysql_fetch_row(res))) {
                    cout << "\t\t\t\t\t" << row[0] << " - " << row[1] << " | " << (row[2] ? row[2] : "-") << "\n";
                }
                mysql_free_result(res);
            }
            else {
                cout << "\t\t\t\t\tError: " << mysql_error(conn) << endl;
                break;
            }

            string deptID;
            cout << "\n\t\t\t\t\tEnter Department ID to edit (< to cancel): ";
            cin >> deptID;
            if (deptID == "<") break;

            query = "SELECT department_name, description FROM Department WHERE department_id = '" + deptID + "'";
            if (mysql_query(conn, query.c_str()) != 0) {
                cout << "\t\t\t\t\tQuery Error: " << mysql_error(conn) << endl;
                break;
            }

            res = mysql_store_result(conn);
            if (!res || mysql_num_rows(res) == 0) {
                cout << "\t\t\t\t\tDepartment not found.\n";
                mysql_free_result(res);
                break;
            }

            row = mysql_fetch_row(res);
            string currName = row[0], currDesc = row[1] ? row[1] : "";
            mysql_free_result(res);

            string newName = currName, newDesc = currDesc;
            int choice;

            do {
                system("cls");
                showBackReminder();
                cout << "\n\t\t\t\t\t--- Editing Department: " << deptID << " ---\n";
                cout << "\t\t\t\t\t1. Name       : " << newName << "\n";
                cout << "\t\t\t\t\t2. Description: " << newDesc << "\n";
                cout << "\t\t\t\t\t3. Save changes\n";
                cout << "\t\t\t\t\t4. Cancel\n";
                cout << "\t\t\t\t\tChoose field to edit: ";
                cin >> choice;
                cin.ignore();

                switch (choice) {
                case 1:
                    cout << "\t\t\t\t\tEnter new department name: ";
                    getline(cin, newName);
                    break;
                case 2:
                    cout << "\t\t\t\t\tEnter new description: ";
                    getline(cin, newDesc);
                    break;
                case 3:
                    break;
                case 4:
                    cout << "\t\t\t\t\tEdit cancelled.\n";
                    goto end_edit;
                default:
                    cout << "\t\t\t\t\tInvalid option.\n";
                    break;
                }
            } while (choice != 3);

            char confirm;
            cout << "\n\t\t\t\t\tConfirm update? (Y/N): ";
            cin >> confirm;
            if (toupper(confirm) != 'Y') {
                cout << "\t\t\t\t\tUpdate cancelled.\n";
                break;
            }

            query = "UPDATE Department SET department_name='" + newName + "', description='" + newDesc +
                "' WHERE department_id='" + deptID + "'";
            if (mysql_query(conn, query.c_str()) == 0)
                cout << "\t\t\t\t\tDepartment updated successfully.\n";
            else
                cout << "\t\t\t\t\tUpdate failed: " << mysql_error(conn) << endl;

        end_edit:
            break;
        }

        case 4: {
            string q = "SELECT department_id, department_name FROM Department";
            mysql_query(conn, q.c_str());
            res = mysql_store_result(conn);

            cout << "\n\t\t\t\t\t========= EXISTING DEPARTMENTS =========\n";
            while ((row = mysql_fetch_row(res))) {
                cout << "\t\t\t\t\t" << row[0] << " - " << row[1] << endl;
            }
            mysql_free_result(res);

            string deptID;
            cout << "\t\t\t\t\tEnter Department ID to delete: ";
            cin >> deptID;

            char confirm;
            cout << "\t\t\t\t\tAre you sure you want to delete this department? (Y/N): ";
            cin >> confirm;
            if (toupper(confirm) != 'Y') {
                cout << "\t\t\t\t\tDeletion cancelled.\n";
                break;
            }

            // Check if employees are assigned to this department
            string checkQuery = "SELECT COUNT(*) FROM Employee WHERE department_id = '" + deptID + "'";
            qstate = mysql_query(conn, checkQuery.c_str());
            if (!qstate) {
                res = mysql_store_result(conn);
                row = mysql_fetch_row(res);
                int count = row ? stoi(row[0]) : 0;
                mysql_free_result(res);

                if (count > 0) {
                    cout << "\t\t\t\t\tCannot delete. Department has assigned employees.\n";
                    break;
                }
            }

            // Proceed to delete department
            query = "DELETE FROM Department WHERE department_id = '" + deptID + "'";
            if (mysql_query(conn, query.c_str()) == 0)
                cout << "\t\t\t\t\tDepartment deleted successfully.\n";
            else
                cout << "\t\t\t\t\tError: " << mysql_error(conn) << endl;

            break;
        }

        case 5:
            return;

        default:
            cout << "\t\t\t\t\tInvalid choice. Try again.\n";
        }


        system("pause");
    }
}

void manageLeave() {
    string input;

    while (true) {
        system("cls");
        showBackReminder();

        cout << "\n\n\n";
        cout << "\t\t\t\t\t______________________________________________________________\n";
        cout << "\t\t\t\t\t|                                                            |\n";
        cout << "\t\t\t\t\t|                  M A N A G E   L E A V E                    |\n";
        cout << "\t\t\t\t\t|____________________________________________________________|\n";
        cout << "\t\t\t\t\t|                                                            |\n";
        cout << "\t\t\t\t\t|  1. Manage Leave Requests   (Approve/Reject)               |\n";
        cout << "\t\t\t\t\t|  2. View All Leave Logs     (Read)                         |\n";
        cout << "\t\t\t\t\t|  3. Edit Leave Request      (Update)                       |\n";
        cout << "\t\t\t\t\t|  4. Delete Leave Request    (Delete)                       |\n";
        cout << "\t\t\t\t\t|  5. View Leave Balances                                     |\n";
        cout << "\t\t\t\t\t|  6. Reset Leave Balances                                    |\n";
        cout << "\t\t\t\t\t|  7. Back to Previous Menu                                   |\n";
        cout << "\t\t\t\t\t|  <. Cancel / Go Back                                        |\n";
        cout << "\t\t\t\t\t|____________________________________________________________|\n";
        cout << "\t\t\t\t\tChoose: ";
        cin >> input;

        if (input == "<") {
            if (confirmBack()) return;
            else continue;
        }

        int opt;
        try {
            opt = stoi(input);
        }
        catch (...) {
            cout << "\t\t\t\t\tInvalid input.\n";
            system("pause");
            continue;
        }

        switch (opt) {
        case 1: adminManageLeaveRequests(); break;
        case 2: adminViewAllLeaveLogs(); break;
        case 3: editLeaveRequest(); break;
        case 4: deleteLeaveRequest(); break;
        case 5: adminViewLeaveBalances(); break;
        case 6: resetLeaveBalance(); break;
        case 7: return;
        default:
            cout << "\t\t\t\t\tInvalid option.\n";
            system("pause");
        }

        system("pause");
    }
}

// === LEAVE MANAGEMENT ===
void manageLeaveRequest() {
    system("cls");
    cout << "\n\n\n";
    cout << "\t\t\t\t\t__________________________________________________________\n";
    cout << "\t\t\t\t\t|                                                        |\n";
    cout << "\t\t\t\t\t|       P E N D I N G   L E A V E   R E Q U E S T S       |\n";
    cout << "\t\t\t\t\t|            ( D E P A R T M E N T   O N L Y )            |\n";
    cout << "\t\t\t\t\t|________________________________________________________|\n";

    // Get manager's department
    string managerDeptID;
    string deptQuery = "SELECT department_id FROM Manager WHERE employee_id = '" + currentEmployeeID + "'";
    if (mysql_query(conn, deptQuery.c_str()) == 0) {
        res = mysql_store_result(conn);
        if ((row = mysql_fetch_row(res))) {
            managerDeptID = row[0];
        }
        mysql_free_result(res);
    }

    string query =
        "SELECT LR.leave_request_id, LR.employee_id, LR.leave_type_id, "
        "LR.start_date, LR.end_date, LR.reason, LR.status "
        "FROM Leave_Request LR "
        "JOIN Employee E ON LR.employee_id = E.employee_id "
        "WHERE LR.status = 'Pending' AND E.department_id = '" + managerDeptID + "'";

    qstate = mysql_query(conn, query.c_str());
    if (qstate) {
        cout << "\n\t\t\t\t\tError fetching data: " << mysql_error(conn) << endl;
        system("pause");
        return;
    }

    res = mysql_store_result(conn);
    if (mysql_num_rows(res) == 0) {
        cout << "\n\t\t\t\t\tNo pending requests in your department.\n";
        mysql_free_result(res);
        system("pause");
        return;
    }

    while ((row = mysql_fetch_row(res))) {
        cout << "\n\t\t\t\t\tRequest ID: " << row[0]
            << " | Emp ID: " << row[1]
            << " | Type: " << row[2]
            << " | " << row[3] << " to " << row[4];
        cout << "\n\t\t\t\t\tReason: " << row[5]
            << " | Status: " << row[6] << "\n";
    }
    mysql_free_result(res);

    string reqID, decision;
    cout << "\n\t\t\t\t\tEnter Request ID to update (< to cancel): ";
    cin >> reqID;
    if (reqID == "<") return;

    cout << "\t\t\t\t\tApprove or Reject? (type exactly): ";
    cin >> decision;
    if (decision != "Approve" && decision != "Reject") {
        cout << "\t\t\t\t\tInvalid input. Only 'Approve' or 'Reject' allowed.\n";
        system("pause");
        return;
    }

    char confirm;
    cout << "\t\t\t\t\tYou are about to set this leave request to [" << decision << "]. Confirm? (Y/N): ";
    cin >> confirm;
    if (toupper(confirm) != 'Y') {
        cout << "\t\t\t\t\tUpdate cancelled.\n";
        system("pause");
        return;
    }

    if (decision == "Approve") {
        query = "SELECT employee_id, leave_type_id, DATEDIFF(end_date, start_date) + 1 "
            "FROM Leave_Request WHERE leave_request_id = '" + reqID + "'";
        if (mysql_query(conn, query.c_str()) == 0) {
            res = mysql_store_result(conn);
            if ((row = mysql_fetch_row(res))) {
                string empID = row[0];
                string leaveTypeID = row[1];
                int leaveDays = stoi(row[2]);
                mysql_free_result(res);

                string updateLB = "UPDATE leave_balance SET "
                    "used_days = used_days + " + to_string(leaveDays) + ", "
                    "remaining_days = remaining_days - " + to_string(leaveDays) + " "
                    "WHERE employee_id = '" + empID + "' AND leave_type_id = '" + leaveTypeID + "'";
                if (mysql_query(conn, updateLB.c_str()) != 0) {
                    cout << "\t\t\t\t\tFailed to update leave balance: " << mysql_error(conn) << "\n";
                    system("pause");
                    return;
                }
            }
            else {
                mysql_free_result(res);
                cout << "\t\t\t\t\tRequest not found for balance update.\n";
                system("pause");
                return;
            }
        }
        else {
            cout << "\t\t\t\t\tQuery error: " << mysql_error(conn) << "\n";
            system("pause");
            return;
        }
    }

    query = "UPDATE Leave_Request SET status='" + decision + "', approved_by='" + currentEmployeeID + "' "
        "WHERE leave_request_id='" + reqID + "'";
    if (mysql_query(conn, query.c_str()) == 0)
        cout << "\t\t\t\t\tRequest updated successfully.\n";
    else
        cout << "\t\t\t\t\tError: " << mysql_error(conn) << endl;

    system("pause");
}


void editLeaveRequest() {
    system("cls");
    showBackReminder();

    cout << "\n\n\n";
    cout << "\t\t\t\t\t________________________________________________________\n";
    cout << "\t\t\t\t\t|                                                      |\n";
    cout << "\t\t\t\t\t|              E D I T   L E A V E   R E Q U E S T      |\n";
    cout << "\t\t\t\t\t|______________________________________________________|\n";

    string q = "SELECT leave_request_id, employee_id, leave_type_id, start_date, end_date, reason, status FROM Leave_Request";
    if (mysql_query(conn, q.c_str()) != 0) {
        cout << "\t\t\t\t\tQuery error: " << mysql_error(conn) << "\n";
        system("pause");
        return;
    }

    res = mysql_store_result(conn);
    if (!res || mysql_num_rows(res) == 0) {
        cout << "\t\t\t\t\tNo leave requests found.\n";
        mysql_free_result(res);
        system("pause");
        return;
    }

    while ((row = mysql_fetch_row(res))) {
        cout << "" << row[0] << " | Emp: " << row[1]
            << " | Type: " << row[2]
            << " | From: " << row[3] << " To: " << row[4]
            << " | Reason: " << row[5]
            << " | Status: " << row[6] << "\n";
    }
    mysql_free_result(res);

    string reqID;
    cout << "\n\t\t\t\t\tEnter Request ID to edit (< to cancel): ";
    cin >> reqID;
    if (reqID == "<") return;

    q = "SELECT leave_type_id, start_date, end_date, reason, status FROM Leave_Request WHERE leave_request_id='" + reqID + "'";
    if (mysql_query(conn, q.c_str()) != 0 || !(res = mysql_store_result(conn)) || mysql_num_rows(res) == 0) {
        cout << "\t\t\t\t\tRequest not found.\n";
        if (res) mysql_free_result(res);
        system("pause");
        return;
    }

    row = mysql_fetch_row(res);
    string currType = row[0], currStart = row[1], currEnd = row[2], currReason = row[3], currStatus = row[4];
    mysql_free_result(res);

    string newType = currType, newStart = currStart, newEnd = currEnd, newReason = currReason, newStatus = currStatus;
    int choice;

    do {
        system("cls");
        showBackReminder();

        cout << "\n\t\t\t\t\t------------- EDITING REQUEST ID [" << reqID << "] -------------\n";
        cout << "\t\t\t\t\t1. Leave Type ID : " << newType << "\n";
        cout << "\t\t\t\t\t2. Start Date    : " << newStart << "\n";
        cout << "\t\t\t\t\t3. End Date      : " << newEnd << "\n";
        cout << "\t\t\t\t\t4. Reason        : " << newReason << "\n";
        cout << "\t\t\t\t\t5. Status        : " << newStatus << "\n";
        cout << "\t\t\t\t\t6. Save Changes\n";
        cout << "\t\t\t\t\t7. Cancel Edit\n";
        cout << "\t\t\t\t\tChoose: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            cout << "\t\t\t\t\tEnter new Leave Type ID: ";
            getline(cin, newType);
            break;
        case 2:
            cout << "\t\t\t\t\tEnter new Start Date (YYYY-MM-DD): ";
            getline(cin, newStart);
            break;
        case 3:
            cout << "\t\t\t\t\tEnter new End Date (YYYY-MM-DD): ";
            getline(cin, newEnd);
            break;
        case 4:
            cout << "\t\t\t\t\tEnter new Reason: ";
            getline(cin, newReason);
            break;
        case 5:
            cout << "\t\t\t\t\tEnter new Status (Pending / Approved / Rejected): ";
            getline(cin, newStatus);
            break;
        case 6: break;
        case 7:
            cout << "\t\t\t\t\tEdit cancelled.\n";
            system("pause");
            return;
        default:
            cout << "\t\t\t\t\tInvalid choice.\n";
            system("pause");
        }
    } while (choice != 6);

    char confirm;
    cout << "\n\t\t\t\t\tSave changes? (Y/N): ";
    cin >> confirm;
    if (toupper(confirm) != 'Y') {
        cout << "\t\t\t\t\tUpdate cancelled.\n";
        system("pause");
        return;
    }

    string updateQuery = "UPDATE Leave_Request SET "
        "leave_type_id='" + newType + "', "
        "start_date='" + newStart + "', "
        "end_date='" + newEnd + "', "
        "reason='" + newReason + "', "
        "status='" + newStatus + "' "
        "WHERE leave_request_id='" + reqID + "'";

    if (mysql_query(conn, updateQuery.c_str()) == 0)
        cout << "\t\t\t\t\tLeave request updated successfully.\n";
    else
        cout << "\t\t\t\t\tError updating request: " << mysql_error(conn) << "\n";

    system("pause");
}

void deleteLeaveRequest() {
    system("cls");
    showBackReminder();

    cout << "\n\n\n";
    cout << "\t\t\t\t\t________________________________________________________\n";
    cout << "\t\t\t\t\t|                                                      |\n";
    cout << "\t\t\t\t\t|              D E L E T E   L E A V E   R E Q U E S T  |\n";
    cout << "\t\t\t\t\t|______________________________________________________|\n";

    string q = "SELECT leave_request_id, employee_id, start_date, end_date, status FROM Leave_Request";
    if (mysql_query(conn, q.c_str()) != 0) {
        cout << "\t\t\t\t\tQuery error: " << mysql_error(conn) << "\n";
        system("pause");
        return;
    }

    res = mysql_store_result(conn);
    if (!res || mysql_num_rows(res) == 0) {
        cout << "\t\t\t\t\tNo leave requests found.\n";
        if (res) mysql_free_result(res);
        system("pause");
        return;
    }

    while ((row = mysql_fetch_row(res))) {
        cout << "\t\t\t\t\t" << row[0]
            << " | Emp: " << row[1]
            << " | " << row[2] << " to " << row[3]
            << " | Status: " << row[4] << "\n";
    }
    mysql_free_result(res);

    string reqID;
    cout << "\n\t\t\t\t\tEnter Request ID to delete (< to cancel): ";
    cin >> reqID;
    if (reqID == "<") return;

    q = "SELECT * FROM Leave_Request WHERE leave_request_id='" + reqID + "'";
    if (mysql_query(conn, q.c_str()) != 0 || !(res = mysql_store_result(conn)) || mysql_num_rows(res) == 0) {
        cout << "\t\t\t\t\tLeave request not found.\n";
        if (res) mysql_free_result(res);
        system("pause");
        return;
    }

    row = mysql_fetch_row(res);
    cout << "\n\t\t\t\t\tYou are about to delete:\n"
        << "\t\t\t\t\tID     : " << row[0] << "\n"
        << "\t\t\t\t\tEmp    : " << row[1] << "\n"
        << "\t\t\t\t\tType   : " << row[2] << "\n"
        << "\t\t\t\t\tFrom   : " << row[3] << " To: " << row[4] << "\n"
        << "\t\t\t\t\tReason : " << row[5] << "\n"
        << "\t\t\t\t\tStatus : " << row[6] << "\n";
    mysql_free_result(res);

    char confirm;
    cout << "\n\t\t\t\t\tConfirm permanent delete? (Y/N): ";
    cin >> confirm;
    if (toupper(confirm) != 'Y') {
        cout << "\t\t\t\t\tDeletion cancelled.\n";
        system("pause");
        return;
    }

    string del = "DELETE FROM Leave_Request WHERE leave_request_id='" + reqID + "'";
    if (mysql_query(conn, del.c_str()) == 0)
        cout << "\t\t\t\t\tDeleted successfully.\n";
    else
        cout << "\t\t\t\t\tError: " << mysql_error(conn) << "\n";

    system("pause");
}

void adminViewAllLeaveLogs() {
    system("cls");
    showBackReminder();

    cout << "\n\n\n\n";
    cout << "\t\t\t\t\t===========================================================\n";
    cout << "\t\t\t\t\t|               A L L   L E A V E   L O G S                |\n";
    cout << "\t\t\t\t\t|               (Approved Requests - All Users)           |\n";
    cout << "\t\t\t\t\t===========================================================\n";

    string query =
        "SELECT LL.employee_id, E.employee_name, LT.type_name, LL.days_taken, "
        "LL.approved_by, LL.approved_date "
        "FROM Leave_Log LL "
        "JOIN Employee E ON LL.employee_id = E.employee_id "
        "JOIN Leave_Type LT ON LL.leave_type_id = LT.leave_type_id "
        "ORDER BY LL.approved_date DESC";

    qstate = mysql_query(conn, query.c_str());
    if (!qstate) {
        res = mysql_store_result(conn);

        cout << left
            << "\t\t\t\t\t" << setw(12) << "Emp ID"
            << setw(25) << "Name"
            << setw(18) << "Leave Type"
            << setw(10) << "Days"
            << setw(15) << "Approved By"
            << setw(15) << "Date" << "\n";
        cout << "\t\t\t\t\t" << string(95, '-') << "\n";

        while ((row = mysql_fetch_row(res))) {
            cout << "\t\t\t\t\t"
                << setw(12) << (row[0] ? row[0] : "-")
                << setw(25) << (row[1] ? row[1] : "-")
                << setw(18) << (row[2] ? row[2] : "-")
                << setw(10) << (row[3] ? row[3] : "-")
                << setw(15) << (row[4] ? row[4] : "-")
                << setw(15) << (row[5] ? row[5] : "-") << "\n";
        }

        mysql_free_result(res);
    }
    else {
        cout << "\t\t\t\t\tError: " << mysql_error(conn) << endl;
    }

    system("pause");
}

// === HOLIDAY MANAGEMENT ===
void manageHolidays() {
    while (true) {
        system("cls");
        showBackReminder();
        cout << "\n\n\n\n";
        cout << "\t\t\t\t\t_________________________________________________________\n";
        cout << "\t\t\t\t\t|                                                       |\n";
        cout << "\t\t\t\t\t|         M A N A G E   H O L I D A Y S   M E N U        |\n";
        cout << "\t\t\t\t\t|_______________________________________________________|\n";
        cout << "\t\t\t\t\t|                                                       |\n";
        cout << "\t\t\t\t\t|   [1] View All Holidays                               |\n";
        cout << "\t\t\t\t\t|   [2] Add Holiday                                     |\n";
        cout << "\t\t\t\t\t|   [3] Edit Holiday                                    |\n";
        cout << "\t\t\t\t\t|   [4] Delete Holiday                                  |\n";
        cout << "\t\t\t\t\t|   [5] Back to Admin Menu                              |\n";
        cout << "\t\t\t\t\t|   [<] Cancel / Go Back                                |\n";
        cout << "\t\t\t\t\t|_______________________________________________________|\n";

        string input;
        cout << "\t\t\t\t\tChoose: "; cin >> input;
        if (input == "<") {
            if (confirmBack()) return;
            else continue;
        }

        int opt;
        try { opt = stoi(input); }
        catch (...) {
            cout << "\t\t\t\t\tInvalid input.\n"; system("pause"); continue;
        }

        string query;
        switch (opt) {
        case 1: {
            query = "SELECT holiday_id, holiday_name, holiday_date FROM Holiday";
            if (mysql_query(conn, query.c_str()) == 0) {
                res = mysql_store_result(conn);

                cout << "\n\t\t\t\t\t_________________________________________________________\n";
                cout << "\t\t\t\t\t|                                                       |\n";
                cout << "\t\t\t\t\t|                  H O L I D A Y   L I S T               |\n";
                cout << "\t\t\t\t\t|_______________________________________________________|\n";

                while ((row = mysql_fetch_row(res))) {
                    cout << "\t\t\t\t\tID: " << row[0]
                        << " | Name: " << row[1]
                        << " | Date: " << row[2] << "\n";
                }

                mysql_free_result(res);
            }
            else {
                cout << "\t\t\t\t\tError: " << mysql_error(conn) << endl;
            }
            break;
        }
        case 2: {
            cin.ignore();
            string name, date;

            cout << "\n\t\t\t\t\t_________________________________________________________\n";
            cout << "\t\t\t\t\t|                                                       |\n";
            cout << "\t\t\t\t\t|                 A D D   N E W   H O L I D A Y          |\n";
            cout << "\t\t\t\t\t|_______________________________________________________|\n";

            cout << "\t\t\t\t\tEnter Holiday Name (< to cancel): ";
            getline(cin, name);
            if (name == "<") break;

            cout << "\t\t\t\t\tEnter Date (YYYY-MM-DD) (< to cancel): ";
            getline(cin, date);
            if (date == "<") break;

            if (!isValidDateFormat(date)) {
                cout << "\t\t\t\t\tInvalid date format.\n"; break;
            }

            char confirm;
            cout << "\t\t\t\t\tConfirm add holiday? (Y/N): ";
            cin >> confirm;
            if (toupper(confirm) != 'Y') break;

            query = "INSERT INTO Holiday(holiday_name, holiday_date) VALUES('" + name + "', '" + date + "')";
            if (mysql_query(conn, query.c_str()) == 0)
                cout << "\t\t\t\t\tHoliday added successfully.\n";
            else
                cout << "\t\t\t\t\tError: " << mysql_error(conn) << "\n";
            break;
        }

        case 3: {
            system("cls");
            showBackReminder();
            query = "SELECT holiday_id, holiday_name, holiday_date, description FROM Holiday";

            if (mysql_query(conn, query.c_str()) == 0) {
                res = mysql_store_result(conn);
                cout << "\n\t\t\t\t\t================ EXISTING HOLIDAYS ================\n";
                cout << "\t\t\t\t\t| ID   | Name               | Date       | Desc   |\n";
                cout << "\t\t\t\t\t---------------------------------------------------\n";
                while ((row = mysql_fetch_row(res))) {
                    cout << "\t\t\t\t\t" << setw(6) << row[0]
                        << " " << setw(18) << row[1]
                        << " " << setw(12) << row[2]
                        << " " << (row[3] ? row[3] : "-") << "\n";
                }
                mysql_free_result(res);
            }
            else {
                cout << "\t\t\t\t\tError: " << mysql_error(conn) << "\n";
                break;
            }

            string holidayID;
            cout << "\n\t\t\t\t\tEnter Holiday ID to edit (< to cancel): ";
            cin >> holidayID;
            if (holidayID == "<") break;

            query = "SELECT holiday_name, holiday_date, description FROM Holiday WHERE holiday_id='" + holidayID + "'";
            if (mysql_query(conn, query.c_str()) != 0 || !(res = mysql_store_result(conn)) || mysql_num_rows(res) == 0) {
                cout << "\t\t\t\t\tHoliday not found.\n";
                if (res) mysql_free_result(res);
                break;
            }

            row = mysql_fetch_row(res);
            string name = row[0], date = row[1], desc = row[2] ? row[2] : "";
            mysql_free_result(res);

            string newName = name, newDate = date, newDesc = desc;
            int choice;
            do {
                system("cls");
                cout << "\n\t\t\t\t\t_______________________________________________\n";
                cout << "\t\t\t\t\t|             EDITING HOLIDAY: " << holidayID << "              |\n";
                cout << "\t\t\t\t\t|_____________________________________________|\n";
                cout << "\t\t\t\t\t| 1. Name        : " << newName << "\n";
                cout << "\t\t\t\t\t| 2. Date        : " << newDate << "\n";
                cout << "\t\t\t\t\t| 3. Description : " << newDesc << "\n";
                cout << "\t\t\t\t\t| 4. Save Changes                            |\n";
                cout << "\t\t\t\t\t| 5. Cancel                                  |\n";
                cout << "\t\t\t\t\t|_____________________________________________|\n";
                cout << "\t\t\t\t\tChoose: ";
                cin >> choice;
                cin.ignore();

                switch (choice) {
                case 1:
                    cout << "\t\t\t\t\tNew Name: ";
                    getline(cin, newName);
                    break;
                case 2:
                    cout << "\t\t\t\t\tNew Date (YYYY-MM-DD): ";
                    getline(cin, newDate);
                    if (!isValidDateFormat(newDate)) {
                        cout << "\t\t\t\t\tInvalid date format.\n";
                        system("pause");
                        newDate = date;
                    }
                    break;
                case 3:
                    cout << "\t\t\t\t\tNew Description: ";
                    getline(cin, newDesc);
                    break;
                case 4:
                    break;
                case 5:
                    cout << "\t\t\t\t\tEdit cancelled.\n";
                    system("pause");
                    goto skip_edit;
                default:
                    cout << "\t\t\t\t\tInvalid option.\n";
                    system("pause");
                }
            } while (choice != 4);

            char confirm;
            cout << "\n\t\t\t\t\tConfirm update? (Y/N): ";
            cin >> confirm;
            if (toupper(confirm) != 'Y') break;

            query = "UPDATE Holiday SET holiday_name='" + newName + "', holiday_date='" + newDate +
                "', description='" + newDesc + "' WHERE holiday_id='" + holidayID + "'";

            if (mysql_query(conn, query.c_str()) == 0)
                cout << "\t\t\t\t\tUpdated successfully.\n";
            else
                cout << "\t\t\t\t\tError: " << mysql_error(conn) << "\n";

        skip_edit:
            break;
        }

        case 4: {
            query = "SELECT holiday_id, holiday_name, holiday_date FROM Holiday";
            mysql_query(conn, query.c_str());
            res = mysql_store_result(conn);
            cout << "\n\t\t\t\t\t================== HOLIDAYS ==================\n";
            cout << "\t\t\t\t\t| ID   | Name                       | Date    |\n";
            cout << "\t\t\t\t\t---------------------------------------------\n";

            while ((row = mysql_fetch_row(res))) {
                cout << "\t\t\t\t\t| " << setw(4) << row[0]
                    << " | " << setw(25) << row[1]
                    << " | " << setw(8) << row[2] << " |\n";
            }
            mysql_free_result(res);
            cout << "\t\t\t\t\t---------------------------------------------\n";

            string id;
            cout << "\n\t\t\t\t\tEnter Holiday ID to delete (< to cancel): ";
            cin >> id;
            if (id == "<") break;

            char confirm;
            cout << "\t\t\t\t\tAre you sure you want to delete this holiday? (Y/N): ";
            cin >> confirm;
            if (toupper(confirm) != 'Y') break;

            query = "DELETE FROM Holiday WHERE holiday_id='" + id + "'";
            if (mysql_query(conn, query.c_str()) == 0)
                cout << "\t\t\t\t\tDeleted successfully.\n";
            else
                cout << "\t\t\t\t\tError: " << mysql_error(conn) << "\n";
            break;
        }

        case 5:
            return;

        default:
            cout << "\t\t\t\t\tInvalid choice.\n";
        }
        system("pause");
    }
}
void adminReports() {
    string input;
    while (true) {
        system("cls");
        showBackReminder();

        cout << "\n\n\n\n";
        cout << "\t\t\t\t\t_____________________________________________________________\n";
        cout << "\t\t\t\t\t|                         ADMIN REPORTS                     |\n";
        cout << "\t\t\t\t\t|___________________________________________________________|\n";
        cout << "\t\t\t\t\t|  1. View Full Attendance Report                           |\n";
        cout << "\t\t\t\t\t|  2. View Leave Log (All Employees)                        |\n";
        cout << "\t\t\t\t\t|  3. Export Attendance to File                             |\n";
        cout << "\t\t\t\t\t|  4. Export Leave Summary                                  |\n";
        cout << "\t\t\t\t\t|  5. Grant Bonus Leave (Overtime ≥ 10)                     |\n";
        cout << "\t\t\t\t\t|  6. View Monthly Leave Statistics                         |\n";
        cout << "\t\t\t\t\t|  7. Export Full Leave Log to CSV                          |\n";
        cout << "\t\t\t\t\t|  8. Back                                                  |\n";
        cout << "\t\t\t\t\t|___________________________________________________________|\n";
        cout << "\t\t\t\t\tChoose: ";
        cin >> input;

        if (input == "<" || input == "8") {
            if (confirmBack()) return;
            else continue;
        }
        int opt;
        try { opt = stoi(input); }
        catch (...) {
            cout << "\t\t\t\t\tInvalid input. Please enter a number.\n";
            system("pause");
            continue;
        }

        switch (opt) {
        case 1:
            viewAttendanceReport();  // Show all attendance from DB
            break;
        case 2:
            adminViewAllLeaveLogs();          // Show all leave requests
            break;
        case 3:
            exportAttendance();      // Write attendance data to .txt/.csv
            break;
        case 4:
            exportLeaveSummary();    // Write leave summary per employee
            break;
        case 5:
            grantBonusLeave();           // New
            break;
        case 6:
            generateLeaveStats();        // New
            break;
        case 7:
            exportLeaveDataToCSV();      // New
            break;
        default:
            cout << "\t\t\t\t\tInvalid option.\n";
            system("pause");
        }
    }
}
void viewAttendanceReport() {
    system("cls");
    showBackReminder();

    string query =
        "SELECT A.attendance_id, A.employee_id, E.employee_name, "
        "A.clock_in_time, A.clock_out_time, A.attendance_date "
        "FROM Attendance A "
        "JOIN Employee E ON A.employee_id = E.employee_id "
        "ORDER BY A.attendance_date DESC, A.clock_in_time ASC";


    if (mysql_query(conn, query.c_str()) == 0) {
        res = mysql_store_result(conn);

        if (res && mysql_num_rows(res) > 0) {
            cout << "\n\t\t==============================================\n";
            cout << "\t\t|           ATTENDANCE REPORT                |\n";
            cout << "\t\t==============================================\n";

            cout << left
                 << setw(10) << "ID"
                 << setw(12) << "Emp ID"
                 << setw(20) << "Name"
                 << setw(20) << "Clock In"
                 << setw(20) << "Clock Out"
                 << setw(15) << "Date" << "\n";
            cout << string(97, '-') << "\n";

            while ((row = mysql_fetch_row(res))) {
                cout << setw(10) << (row[0] ? row[0] : "-")
                     << setw(12) << (row[1] ? row[1] : "-")
                     << setw(20) << (row[2] ? row[2] : "-")
                     << setw(20) << (row[3] ? row[3] : "-")
                     << setw(20) << (row[4] ? row[4] : "-")
                     << setw(15) << (row[5] ? row[5] : "-") << "\n";
            }
        } else {
            cout << "\n\t\tNo attendance records found.\n";
        }

        mysql_free_result(res);
    } else {
        cout << "\n\t\tError: " << mysql_error(conn) << "\n";
    }

    system("pause");
}

void viewFrequentHalfDays() {
    system("cls");
    showBackReminder();

    cout << "\n\n\n\n";
    cout << "\t\t\t\t\t===================================================\n";
    cout << "\t\t\t\t\t|     F R E Q U E N T   H A L F - D A Y   A L E R T S     |\n";
    cout << "\t\t\t\t\t===================================================\n";

    string query =
        "SELECT A.employee_id, E.employee_name, COUNT(*) AS half_days "
        "FROM Attendance A "
        "JOIN Employee E ON A.employee_id = E.employee_id "
        "WHERE A.status = 'Half Day' "
        "AND MONTH(A.attendance_date) = MONTH(CURDATE()) "
        "AND YEAR(A.attendance_date) = YEAR(CURDATE()) "
        "GROUP BY A.employee_id "
        "HAVING half_days >= 3";

    qstate = mysql_query(conn, query.c_str());

    if (!qstate) {
        res = mysql_store_result(conn);

        if (mysql_num_rows(res) == 0) {
            cout << "\n\t\t\t\t\tNo employees with 3 or more half days this month.\n";
        }
        else {
            cout << "\n\t\t\t\t\tEmployees with frequent half days:\n\n";
            cout << left
                << "\t\t\t\t\t" << setw(15) << "Employee ID"
                << setw(25) << "Employee Name"
                << setw(15) << "Half Days" << "\n";
            cout << "\t\t\t\t\t" << string(55, '-') << "\n";

            while ((row = mysql_fetch_row(res))) {
                cout << "\t\t\t\t\t"
                    << setw(15) << (row[0] ? row[0] : "-")
                    << setw(25) << (row[1] ? row[1] : "-")
                    << setw(15) << (row[2] ? row[2] : "-") << "\n";
            }
        }

        mysql_free_result(res);
    }
    else {
        cout << "\t\t\t\t\tError: " << mysql_error(conn) << "\n";
    }
    system("pause");
}



void exportLeaveLogToCSV() {
    system("cls");
    showBackReminder();
    cout << "\n\t\t\t\t\t========= EXPORT LEAVE LOG =========\n";

    string query =
        "SELECT LL.employee_id, E.employee_name, LL.leave_type_id, LT.type_name, "
        "LL.days_taken, LL.approved_by, LL.approved_date "
        "FROM Leave_Log LL "
        "JOIN Employee E ON LL.employee_id = E.employee_id "
        "JOIN Leave_Type LT ON LL.leave_type_id = LT.leave_type_id "
        "ORDER BY LL.approved_date DESC";

    qstate = mysql_query(conn, query.c_str());
    if (!qstate) {
        res = mysql_store_result(conn);

        // Generate filename with timestamp
        time_t now = time(0);
        tm local;
        localtime_s(&local, &now);
        char filename[100];
        strftime(filename, sizeof(filename), "leave_log_%Y%m%d_%H%M%S.csv", &local);
        ofstream csvFile(filename);

        // Write CSV header
        csvFile << "Employee ID,Employee Name,Leave Type ID,Leave Type Name,"
            "Days Taken,Approved By,Approval Date\n";

        // Write data rows
        while ((row = mysql_fetch_row(res))) {
            for (int i = 0; i < 7; ++i) {
                csvFile << (row[i] ? row[i] : "") << (i < 6 ? "," : "\n");
            }
        }

        csvFile.close();
        mysql_free_result(res);
        cout << "\n\t\t\t\t\tLeave Log exported to: " << filename << "\n";
    }
    else {
        cout << "\t\t\t\t\tMySQL error: " << mysql_error(conn) << "\n";
    }

    system("pause");
}

// === REPORTS ===
void generateReports() {
    system("cls");
    showBackReminder();
    cout << "\n\n\n\n";
    cout << "\t\t\t\t\t==========================================\n";
    cout << "\t\t\t\t\t|         R E P O R T   M E N U           |\n";
    cout << "\t\t\t\t\t==========================================\n";
    cout << "\t\t\t\t\t|  1. Attendance Summary Report           |\n";
    cout << "\t\t\t\t\t|  2. Leave Summary Report                |\n";
    cout << "\t\t\t\t\t|  3. Export Full Leave Log               |\n";
    cout << "\t\t\t\t\t|  <. Back to Previous Menu               |\n";
    cout << "\t\t\t\t\t==========================================\n";

    string input;
    cout << "\t\t\t\t\tChoose: ";
    cin >> input;

    if (input == "<") {
        if (confirmBack()) return;
        else return generateReports();
    }

    int opt;
    try {
        opt = stoi(input);
    }
    catch (...) {
        cout << "\t\t\t\t\tInvalid input. Returning...\n";
        system("pause");
        return;
    }

    string query;
    switch (opt) {
    case 1:
        query = "SELECT employee_id, COUNT(*) AS total_days, "
            "SUM(CASE WHEN status='Present' THEN 1 ELSE 0 END) AS total_present "
            "FROM Attendance GROUP BY employee_id";
        break;
    case 2:
        query = "SELECT employee_id, COUNT(*) AS total_leaves, "
            "SUM(CASE WHEN status='Approved' THEN 1 ELSE 0 END) AS approved "
            "FROM Leave_Request GROUP BY employee_id";
        break;
    case 3:
        exportLeaveLogToCSV();
        return;
    default:
        cout << "\t\t\t\t\tInvalid option.\n";
        system("pause");
        return;
    }

    char confirm;
    cout << "\n\t\t\t\t\tExport this report to CSV? (Y/N): ";
    cin >> confirm;
    if (toupper(confirm) != 'Y') {
        cout << "\t\t\t\t\tExport cancelled.\n";
        system("pause");
        return;
    }

    qstate = mysql_query(conn, query.c_str());
    if (!qstate) {
        res = mysql_store_result(conn);
        time_t now = time(0);
        tm local;
        localtime_s(&local, &now);

        char filename[50];
        strftime(filename, sizeof(filename), "report_%Y%m%d_%H%M%S.csv", &local);
        ofstream csvFile(filename);

        int num_fields = mysql_num_fields(res);
        MYSQL_FIELD* fields = mysql_fetch_fields(res);

        // CSV Header
        for (int i = 0; i < num_fields; ++i) {
            csvFile << fields[i].name;
            if (i < num_fields - 1) csvFile << ",";
        }
        csvFile << endl;

        // CSV Rows
        while ((row = mysql_fetch_row(res))) {
            for (int i = 0; i < num_fields; ++i) {
                string field = row[i] ? row[i] : "";
                csvFile << escapeQuotes(field).c_str();
                if (i < num_fields - 1) csvFile << ",";
            }
            csvFile << endl;
        }

        csvFile.close();
        mysql_free_result(res);
        cout << "\n\t\t\t\t\tReport exported successfully to '" << filename << "'\n";
    }
    else {
        cout << "\t\t\t\t\tQuery error: " << mysql_error(conn) << "\n";
    }

    system("pause");
}

void changePassword() {
    system("cls");
    showBackReminder();
    string oldPass, newPass, confirmPass;

    cout << "\n\n\n\n";
    cout << "\t\t\t\t\t=====================================\n";
    cout << "\t\t\t\t\t|         CHANGE PASSWORD           |\n";
    cout << "\t\t\t\t\t=====================================\n";
    cout << "\t\t\t\t\tCurrent User: " << currentEmployeeID << "\n";

    // Old password
    cout << "\t\t\t\t\tOld Password (< to cancel): ";
    oldPass = getPasswordInput();
    if (oldPass == "<") return;

    // New password input
    cout << "\t\t\t\t\tNew Password (< to cancel): ";
    newPass = getPasswordInput();
    if (newPass == "<") return;

    // Confirm password
    cout << "\t\t\t\t\tConfirm New Password (< to cancel): ";
    confirmPass = getPasswordInput();
    if (confirmPass == "<") return;

    if (newPass != confirmPass) {
        cout << "\t\t\t\t\tPasswords do not match.\n";
        system("pause");
        return;
    }

    if (!isValidPassword(newPass)) {
        cout << "\n\t\t\t\t\tNew password must contain:\n"
            << "\t\t\t\t\t- At least 8 characters\n"
            << "\t\t\t\t\t- At least 1 uppercase letter\n"
            << "\t\t\t\t\t- At least 1 lowercase letter\n"
            << "\t\t\t\t\t- At least 1 number\n"
            << "\t\t\t\t\t- At least 1 special character (!@#$%^&*)\n";
        system("pause");
        return;
    }

    char confirm;
    cout << "\n\t\t\t\t\tAre you sure you want to change your password? (Y/N): ";
    cin >> confirm;
    if (toupper(confirm) != 'Y') {
        cout << "\t\t\t\t\tPassword change cancelled.\n";
        system("pause");
        return;
    }

    oldPass = escapeQuotes(oldPass);
    newPass = escapeQuotes(newPass);

    string query = "UPDATE Employee SET password='" + newPass + "' "
        "WHERE employee_id='" + currentEmployeeID + "' AND password='" + oldPass + "'";

    qstate = mysql_query(conn, query.c_str());

    if (!qstate && mysql_affected_rows(conn) > 0)
        cout << "\t\t\t\t\tPassword changed successfully.\n";
    else
        cout << "\t\t\t\t\tFailed to change password. Is the old password correct?\n";

    system("pause");
}

// === Reset Leave Balance ===
void resetLeaveBalance() {
    system("cls");
    showBackReminder();
    string empID, typeID;

    cout << "\n\n\n\n";
    cout << "\t\t\t\t\t=========================================\n";
    cout << "\t\t\t\t\t|          RESET LEAVE BALANCE          |\n";
    cout << "\t\t\t\t\t=========================================\n";

    // List employees
    cout << "\n\t\t\t\t\t[Active Employees]\n";
    string empListQuery = "SELECT employee_id, employee_name FROM Employee WHERE is_active = 1";
    if (mysql_query(conn, empListQuery.c_str()) == 0) {
        res = mysql_store_result(conn);
        while ((row = mysql_fetch_row(res)))
            cout << "\t\t\t\t\t" << row[0] << " - " << row[1] << "\n";
        mysql_free_result(res);
    }

    cout << "\n\t\t\t\t\tEnter Employee ID (< to cancel): ";
    cin >> empID;
    if (empID == "<") return;

    // List leave types
    cout << "\n\t\t\t\t\t[Available Leave Types]\n";
    string ltQuery = "SELECT leave_type_id, type_name FROM Leave_Type";
    if (mysql_query(conn, ltQuery.c_str()) == 0) {
        res = mysql_store_result(conn);
        while ((row = mysql_fetch_row(res)))
            cout << "\t\t\t\t\t" << row[0] << " - " << row[1] << "\n";
        mysql_free_result(res);
    }

    cout << "\n\t\t\t\t\tEnter Leave Type ID (< to cancel): ";
    cin >> typeID;
    if (typeID == "<") return;

    // Show current leave balance
    string query = "SELECT total_days, used_days, remaining_days FROM Leave_Balance "
        "WHERE employee_id = '" + empID + "' AND leave_type_id = '" + typeID + "'";
    qstate = mysql_query(conn, query.c_str());

    if (qstate || !(res = mysql_store_result(conn)) || mysql_num_rows(res) == 0) {
        cout << "\t\t\t\t\tNo such leave balance record found.\n";
        system("pause");
        return;
    }

    row = mysql_fetch_row(res);
    cout << "\t\t\t\t\tCurrent Balance -> Total: " << row[0]
        << ", Used: " << row[1]
        << ", Remaining: " << row[2] << "\n";
    mysql_free_result(res);

    // Fetch max days from leave_type
    query = "SELECT max_days FROM Leave_Type WHERE leave_type_id = '" + typeID + "'";
    int maxDays = 30; // fallback default
    if (mysql_query(conn, query.c_str()) == 0) {
        res = mysql_store_result(conn);
        if ((row = mysql_fetch_row(res))) maxDays = stoi(row[0]);
        mysql_free_result(res);
    }

    // Confirm
    char confirm;
    cout << "\n\t\t\t\t\tReset leave balance to " << maxDays << " days? (Y/N): ";
    cin >> confirm;
    if (toupper(confirm) != 'Y') {
        cout << "\t\t\t\t\tReset cancelled.\n";
        system("pause");
        return;
    }

    query = "UPDATE Leave_Balance SET total_days = " + to_string(maxDays) +
        ", used_days = 0, remaining_days = " + to_string(maxDays) +
        " WHERE employee_id = '" + empID + "' AND leave_type_id = '" + typeID + "'";

    if (mysql_query(conn, query.c_str()) == 0)
        cout << "\t\t\t\t\tLeave balance successfully reset.\n";
    else
        cout << "\t\t\t\t\tError: " << mysql_error(conn) << "\n";

    system("pause");
}

void viewAllLeaveLogs() {
    string managerDeptID = "";

    // Step 1: Get manager's department based on currentEmployeeID
    string deptQuery = "SELECT department_id FROM Manager WHERE employee_id = '" + currentEmployeeID + "'";
    if (mysql_query(conn, deptQuery.c_str()) == 0) {
        res = mysql_store_result(conn);
        if ((row = mysql_fetch_row(res))) {
            managerDeptID = row[0];
        }
        mysql_free_result(res);
    }

    system("cls");
    showBackReminder();
    cout << "\n\n\n\n";
    cout << "\t\t\t\t\t=============================================\n";
    cout << "\t\t\t\t\t|        ALL LEAVE APPROVAL LOGS (DEPT)     |\n";
    cout << "\t\t\t\t\t=============================================\n\n";

    // Step 2: Query logs for manager's department
    string query =
        "SELECT LL.employee_id, E.employee_name, LT.type_name, LL.days_taken, "
        "LL.approved_by, LL.approved_date "
        "FROM Leave_Log LL "
        "JOIN Employee E ON LL.employee_id = E.employee_id "
        "JOIN Leave_Type LT ON LL.leave_type_id = LT.leave_type_id "
        "WHERE E.department_id = '" + managerDeptID + "' "
        "ORDER BY LL.approved_date DESC";

    qstate = mysql_query(conn, query.c_str());

    if (!qstate) {
        res = mysql_store_result(conn);

        cout << left
            << setw(12) << "Emp ID"
            << setw(25) << "Name"
            << setw(18) << "Leave Type"
            << setw(10) << "Days"
            << setw(15) << "Approved By"
            << setw(15) << "Date" << "\n";

        cout << "\t\t\t\t\t" << string(95, '-') << "\n";

        while ((row = mysql_fetch_row(res))) {
            cout << setw(12) << (row[0] ? row[0] : "-")
                << setw(25) << (row[1] ? row[1] : "-")
                << setw(18) << (row[2] ? row[2] : "-")
                << setw(10) << (row[3] ? row[3] : "-")
                << setw(15) << (row[4] ? row[4] : "-")
                << setw(15) << (row[5] ? row[5] : "-") << "\n";
        }

        mysql_free_result(res);
    }
    else {
        cout << "\n\t\t\t\t\tError: " << mysql_error(conn) << endl;
    }

    system("pause");
}




// manager_module.cpp - Manager functionality
// Global scope (e.g., top of manager_module.cpp)
string managerDeptID = "";

// managerModule implementation
void managerModule() {
    string input;

    while (true) {
        system("cls");
        showBackReminder();
        string deptName = "Unknown";

        // Fetch department name for current manager
// Fetch department ID and name for current manager
        string deptQuery =
            "SELECT M.department_id, D.department_name "
            "FROM Manager M "
            "JOIN Department D ON M.department_id = D.department_id "
            "WHERE M.employee_id = '" + currentEmployeeID + "'";

        if (mysql_query(conn, deptQuery.c_str()) == 0) {
            res = mysql_store_result(conn);
            if ((row = mysql_fetch_row(res))) {
                managerDeptID = row[0];  // store dept_id
                deptName = row[1];       // display dept name
            }
            mysql_free_result(res);
        }

        cout << "\n\n\n\n";
        cout << "\t\t\t\t\t__________________________________________________________\n";
        cout << "\t\t\t\t\t|                                                        |\n";
        cout << "\t\t\t\t\t|                   M A N A G E R   M E N U              |\n";
        cout << "\t\t\t\t\t|________________________________________________________|\n";
        cout << "\t\t\t\t\t| Department: " << setw(44) << left << deptName << "|\n";
        cout << "\t\t\t\t\t|________________________________________________________|\n";
        cout << "\t\t\t\t\t|                                                        |\n";
        cout << "\t\t\t\t\t| 1. Manage Leave                                        |\n";
        cout << "\t\t\t\t\t| 2. Assign Staff to Department                          |\n";
        cout << "\t\t\t\t\t| 3. Reports (Team View)                                 |\n";
        cout << "\t\t\t\t\t| 4. My Employee Features                                |\n";
        cout << "\t\t\t\t\t| 5. Logout                                              |\n";
        cout << "\t\t\t\t\t| <. Back to Main Menu                                   |\n";
        cout << "\t\t\t\t\t|________________________________________________________|\n";
        cout << "\t\t\t\t\tEnter your choice: ";
        cin >> input;

        if (input == "<") {
            if (confirmBack()) return;
            else continue;
        }

        int choice;
        try {
            choice = stoi(input);
        }
        catch (...) {
            cout << "\t\t\t\t\tInvalid input. Try again.\n";
            system("pause");
            continue;
        }

        switch (choice) {
        case 1:
            managerManageLeave();  // Your manager's leave features
            break;
        case 2:
            assignStaffToDepartment();  // Assign employees to department
            break;
        case 3:
            managerReports();  // NEW: reports with dept-restricted access
            break;
        case 4:
            employeeModule();  // Manager accesses employee-level features
            break;
        case 5: {
            char confirm;
            cout << "\t\t\t\t\tAre you sure you want to logout? (Y/N): ";
            cin >> confirm;
            if (toupper(confirm) == 'Y') {
                login();
                return;
            }
            else {
                cout << "\t\t\t\t\tLogout cancelled.\n";
                system("pause");
            }
            break;
        }
        default:
            cout << "\t\t\t\t\tInvalid option. Try again.\n";
            system("pause");
        }
    }
}


void managerReports() {
    string input;
    while (true) {
        system("cls");
        showBackReminder();
        cout << "\n\n\n\n";
        cout << "\t\t\t\t\t__________________________________________________________\n";
        cout << "\t\t\t\t\t|                                                        |\n";
        cout << "\t\t\t\t\t|              M A N A G E R   R E P O R T S              |\n";
        cout << "\t\t\t\t\t|________________________________________________________|\n";
        cout << "\t\t\t\t\t|                                                        |\n";
        cout << "\t\t\t\t\t|   1. View Team Attendance Summary                      |\n";
        cout << "\t\t\t\t\t|   2. View Team Leave Log                               |\n";
        cout << "\t\t\t\t\t|   3. Export Team Leave Summary                         |\n";
        cout << "\t\t\t\t\t|   4. Back                                              |\n";
        cout << "\t\t\t\t\t|   <. Cancel / Go Back                                  |\n";
        cout << "\t\t\t\t\t|________________________________________________________|\n";
        cout << "\t\t\t\t\tChoose: ";
        cin >> input;

        if (input == "<" || input == "4") {
            if (confirmBack()) return;
            else continue;
        }

        int opt;
        try {
            opt = stoi(input);
        }
        catch (...) {
            cout << "\t\t\t\t\tInvalid input.\n";
            system("pause");
            continue;
        }

        switch (opt) {
        case 1:
            viewTeamAttendance(); // Show attendance for employees under this manager
            break;
        case 2:
            viewTeamLeaveLog(); // Should internally filter by department/manager if needed
            break;
        case 3:
            exportTeamLeaveSummary(); // Optional export version (provided below too)
            break;
        default:
            cout << "\t\t\t\t\tInvalid option.\n";
            system("pause");
        }
    }
}
void viewTeamLeaveLog() {
    system("cls");
    showBackReminder();
    cout << "\n\n\n\n";
    cout << "\t\t\t\t\t=============================================\n";
    cout << "\t\t\t\t\t|         TEAM LEAVE LOG (BY DEPARTMENT)    |\n";
    cout << "\t\t\t\t\t=============================================\n";

    string query =
        "SELECT LL.employee_id, E.employee_name, LT.type_name, LL.days_taken, "
        "LL.approved_by, LL.approved_date "
        "FROM Leave_Log LL "
        "JOIN Employee E ON LL.employee_id = E.employee_id "
        "JOIN Leave_Type LT ON LL.leave_type_id = LT.leave_type_id "
        "WHERE E.department_id = (SELECT department_id FROM Manager WHERE employee_id = '" + currentEmployeeID + "') "
        "ORDER BY LL.approved_date DESC";

    qstate = mysql_query(conn, query.c_str());
    if (!qstate) {
        res = mysql_store_result(conn);

        cout << left
            << setw(12) << "\n\tEmp ID"
            << setw(22) << "Employee Name"
            << setw(18) << "Leave Type"
            << setw(14) << "Days Taken"
            << setw(20) << "Approved By"
            << setw(16) << "Approved Date" << endl;
        cout << "\t" << string(96, '-') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << "\t" << setw(12) << row[0]
                << setw(22) << row[1]
                << setw(18) << row[2]
                << setw(14) << row[3]
                << setw(20) << row[4]
                << setw(16) << row[5] << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "\t\t\t\t\tError fetching log: " << mysql_error(conn) << endl;
    }

    cout << "\n\t\t\t\t\t< Back\t\t\t\t\t[ Confirm ]\n";
    system("pause");
}
void exportTeamLeaveSummary() {
    system("cls");
    showBackReminder();
    ofstream file("team_leave_summary.csv");
    file << "Employee ID,Employee Name,Leave Type,Days Taken,Approved By,Approved Date\n";

    string query =
        "SELECT LL.employee_id, E.employee_name, LT.type_name, LL.days_taken, "
        "LL.approved_by, LL.approved_date "
        "FROM Leave_Log LL "
        "JOIN Employee E ON LL.employee_id = E.employee_id "
        "JOIN Leave_Type LT ON LL.leave_type_id = LT.leave_type_id "
        "WHERE E.department_id = (SELECT department_id FROM Manager WHERE employee_id = '" + currentEmployeeID + "') "
        "ORDER BY LL.approved_date DESC";

    qstate = mysql_query(conn, query.c_str());
    if (!qstate) {
        res = mysql_store_result(conn);
        while ((row = mysql_fetch_row(res))) {
            for (int i = 0; i < 6; i++) {
                file << "\"" << row[i] << "\"";
                if (i < 5) file << ",";
            }
            file << "\n";
        }
        mysql_free_result(res);
        cout << "\n\t\t\t\t\tExported to 'team_leave_summary.csv'\n";
    }
    else {
        cout << "\n\t\t\t\t\tExport failed: " << mysql_error(conn) << endl;
    }

    file.close();
    cout << "\n\t\t\t\t\t< Back\t\t\t\t\t[ Confirm ]\n";
    system("pause");
}


void managerManageLeave() {
    string input;

    while (true) {
        system("cls");
        showBackReminder();

        cout << "\n\n\n\n";
        cout << "\t\t\t\t\t__________________________________________________________\n";
        cout << "\t\t\t\t\t|                                                        |\n";
        cout << "\t\t\t\t\t|          M A N A G E R :  M A N A G E   L E A V E       |\n";
        cout << "\t\t\t\t\t|________________________________________________________|\n";
        cout << "\t\t\t\t\t|                                                        |\n";
        cout << "\t\t\t\t\t|   1. Manage Leave Requests (Department Only)           |\n";
        cout << "\t\t\t\t\t|   2. Apply Leave on Behalf                             |\n";
        cout << "\t\t\t\t\t|   3. View Leave Logs (Department Only)                 |\n";
        cout << "\t\t\t\t\t|   4. View Leave Balances (Department Only)             |\n";
        cout << "\t\t\t\t\t|   5. Back                                              |\n";
        cout << "\t\t\t\t\t|   <. Back to Manager Menu                              |\n";
        cout << "\t\t\t\t\t|________________________________________________________|\n";
        cout << "\t\t\t\t\tChoose: ";
        cin >> input;

        if (input == "<") {
            if (confirmBack()) return;
            else continue;
        }

        int opt;
        try {
            opt = stoi(input);
        }
        catch (...) {
            cout << "\t\t\t\t\tInvalid input.\n";
            system("pause");
            continue;
        }

        switch (opt) {
        case 1:
            manageLeaveRequest();     // Filter by managerDeptID inside function
            break;
        case 2:
            applyLeaveOnBehalf();     // Optional: restrict by managerDeptID
            break;
        case 3:
            viewAllLeaveLogs();       // Filter logs using managerDeptID JOIN
            break;
        case 4:
            DeptviewLeaveBalance();       // Show balances from managerDeptID
            break;
        case 5:
            return;                   // Go back to previous menu
        default:
            cout << "\t\t\t\t\tInvalid option.\n";
            system("pause");
        }
    }
}

void applyLeaveOnBehalf() {
    system("cls");
    showBackReminder();
    string empID, leaveTypeID, startDate, endDate, reason;

    cout << "\n\n\n\n";
    cout << "\t\t\t\t\t===================================================\n";
    cout << "\t\t\t\t\t|           A P P L Y   L E A V E   ( O B )       |\n";
    cout << "\t\t\t\t\t===================================================\n";
    cout << "\t\t\t\t\t|  OB = On Behalf (Only your department staff)    |\n";
    cout << "\t\t\t\t\t|  You may type < anytime to cancel the process.  |\n";
    cout << "\t\t\t\t\t---------------------------------------------------\n";

    // Step 1: Show eligible employees in manager's department
    string query =
        "SELECT employee_id, employee_name FROM Employee "
        "WHERE department_id = '" + managerDeptID + "' AND employee_id != '" + currentEmployeeID + "'";

    qstate = mysql_query(conn, query.c_str());
    if (!qstate) {
        res = mysql_store_result(conn);
        if (mysql_num_rows(res) == 0) {
            cout << "\n\t\t\t\t\tNo employees found in your department.\n";
            mysql_free_result(res);
            system("pause");
            return;
        }

        cout << "\n\t\t\t\t\tEmployees in Your Department:\n";
        while ((row = mysql_fetch_row(res))) {
            cout << "\t\t\t\t\t- " << row[0] << " (" << row[1] << ")\n";
        }
        mysql_free_result(res);
    }
    else {
        cout << "\n\t\t\t\t\tError retrieving employee list: " << mysql_error(conn) << endl;
        system("pause");
        return;
    }

    // Step 2: Select employee ID
    cout << "\n\t\t\t\t\tEnter Employee ID to apply for: ";
    cin >> empID;
    if (empID == "<") return;

    // Step 3: Show leave types
    query = "SELECT leave_type_id, type_name FROM Leave_Type";
    qstate = mysql_query(conn, query.c_str());

    if (!qstate) {
        res = mysql_store_result(conn);
        cout << "\n\t\t\t\t\tAvailable Leave Types:\n";
        while ((row = mysql_fetch_row(res))) {
            cout << "\t\t\t\t\t- ID: " << row[0] << " (" << row[1] << ")\n";
        }
        mysql_free_result(res);
    }
    else {
        cout << "\n\t\t\t\t\tError fetching leave types: " << mysql_error(conn) << endl;
        system("pause");
        return;
    }

    // Step 4: Input leave type
    cout << "\n\t\t\t\t\tLeave Type ID: ";
    cin >> leaveTypeID;
    if (leaveTypeID == "<") return;

    // Step 5: Input start date
    do {
        cout << "\t\t\t\t\tStart Date (YYYY-MM-DD): ";
        cin >> startDate;
        if (startDate == "<") return;
        if (!isValidDateFormat(startDate)) {
            cout << "\t\t\t\t\tInvalid format. Use YYYY-MM-DD.\n";
        }
        else if (isNonWorkingDay(startDate)) {
            cout << "\t\t\t\t\tLeave cannot start on weekend/public holiday.\n";
            startDate = "";
        }
    } while (!isValidDateFormat(startDate) || startDate.empty());

    // Step 6: Input end date
    do {
        cout << "\t\t\t\t\tEnd Date (YYYY-MM-DD): ";
        cin >> endDate;
        if (endDate == "<") return;
        if (!isValidDateFormat(endDate)) {
            cout << "\t\t\t\t\tInvalid format. Use YYYY-MM-DD.\n";
        }
        else if (endDate < startDate) {
            cout << "\t\t\t\t\tEnd date must be after or equal to start date.\n";
        }
        else if (isNonWorkingDay(endDate)) {
            cout << "\t\t\t\t\tLeave cannot end on weekend/public holiday.\n";
            endDate = "";
        }
    } while (!isValidDateFormat(endDate) || endDate < startDate || endDate.empty());

    // Step 7: Reason
    cin.ignore();
    cout << "\t\t\t\t\tReason: ";
    getline(cin, reason);
    if (reason == "<") return;

    // Step 8: Confirm
    string confirm;
    cout << "\n\t\t\t\t\tSubmit this leave request? (Y/N or < to cancel): ";
    cin >> confirm;
    if (confirm == "<" || toupper(confirm[0]) != 'Y') {
        cout << "\t\t\t\t\tLeave application cancelled.\n";
        system("pause");
        return;
    }

    // Step 9: Insert leave request
    query =
        "INSERT INTO Leave_Request(employee_id, leave_type_id, start_date, end_date, reason, status) VALUES('" +
        empID + "', '" + leaveTypeID + "', '" + startDate + "', '" + endDate + "', '" + reason + "', 'Pending')";

    qstate = mysql_query(conn, query.c_str());
    if (!qstate)
        cout << "\t\t\t\t\tLeave request submitted on behalf of " << empID << ".\n";
    else
        cout << "\t\t\t\t\tError: " << mysql_error(conn) << endl;


    system("pause");
}
void viewPendingLeave() {
    system("cls");
    showBackReminder();

    cout << "\n\n\n\n";
    cout << "\t\t\t\t\t=====================================================\n";
    cout << "\t\t\t\t\t|           P E N D I N G   L E A V E   R E Q U E S T S           |\n";
    cout << "\t\t\t\t\t=====================================================\n";

    string query =
        "SELECT leave_request_id, employee_id, start_date, end_date, reason "
        "FROM Leave_Request WHERE status='Pending'";

    qstate = mysql_query(conn, query.c_str());
    if (!qstate) {
        res = mysql_store_result(conn);

        if (mysql_num_rows(res) == 0) {
            cout << "\t\t\t\t\tNo pending leave requests found.\n";
        }
        else {
            cout << left
                << "\t\t\t\t\t" << setw(15) << "Request ID"
                << setw(15) << "Employee ID"
                << setw(15) << "Start Date"
                << setw(15) << "End Date"
                << setw(30) << "Reason" << "\n";
            cout << "\t\t\t\t\t" << string(90, '-') << "\n";

            while ((row = mysql_fetch_row(res))) {
                cout << "\t\t\t\t\t"
                    << setw(15) << (row[0] ? row[0] : "-")
                    << setw(15) << (row[1] ? row[1] : "-")
                    << setw(15) << (row[2] ? row[2] : "-")
                    << setw(15) << (row[3] ? row[3] : "-")
                    << setw(30) << (row[4] ? row[4] : "-") << "\n";
            }
        }

        mysql_free_result(res);
    }
    else {
        cout << "\t\t\t\t\tError: " << mysql_error(conn) << "\n";
    }

    system("pause");
}

void updateLeaveStatus() {
    system("cls");
    showBackReminder();
    string reqID, decision;

    cout << "\n\n\n\n";
    cout << "\t\t\t\t\t==================================================\n";
    cout << "\t\t\t\t\t|         A P P R O V E / R E J E C T   L E A V E         |\n";
    cout << "\t\t\t\t\t==================================================\n";

    cout << "\t\t\t\t\tEnter Leave Request ID: ";
    cin >> reqID;
    cout << "\t\t\t\t\tEnter decision (Approved / Rejected): ";
    cin >> decision;

    if (decision != "Approved" && decision != "Rejected") {
        cout << "\t\t\t\t\tInvalid decision. Use Approved or Rejected.\n";
        system("pause");
        return;
    }

    char confirm;
    cout << "\t\t\t\t\tYou are about to set this leave request to [" << decision << "]. Confirm? (Y/N): ";
    cin >> confirm;
    if (toupper(confirm) != 'Y') {
        cout << "\t\t\t\t\tUpdate cancelled.\n";
        system("pause");
        return;
    }

    string query = "UPDATE Leave_Request SET status='" + decision +
        "', approved_by='" + currentEmployeeID +
        "' WHERE leave_request_id='" + reqID + "'";
    qstate = mysql_query(conn, query.c_str());

    if (qstate != 0) {
        cout << "\t\t\t\t\tError: " << mysql_error(conn) << endl;
        system("pause");
        return;
    }

    cout << "\t\t\t\t\tLeave request updated successfully.\n";

    if (decision == "Approved") {
        string fetchQuery = "SELECT employee_id, leave_type_id, start_date, end_date "
            "FROM Leave_Request WHERE leave_request_id='" + reqID + "'";
        if (!mysql_query(conn, fetchQuery.c_str())) {
            res = mysql_store_result(conn);
            if ((row = mysql_fetch_row(res))) {
                string empID = row[0], typeID = row[1], startDate = row[2], endDate = row[3];

                string daysDiffQuery = "SELECT DATEDIFF('" + endDate + "', '" + startDate + "') + 1";
                if (!mysql_query(conn, daysDiffQuery.c_str())) {
                    MYSQL_RES* res2 = mysql_store_result(conn);
                    MYSQL_ROW row2 = mysql_fetch_row(res2);
                    if (row2 && row2[0]) {
                        int daysTaken = stoi(row2[0]);

                        string checkBalance = "SELECT remaining_days FROM Leave_Balance "
                            "WHERE employee_id='" + empID +
                            "' AND leave_type_id='" + typeID +
                            "' AND year=YEAR(CURDATE())";
                        if (!mysql_query(conn, checkBalance.c_str())) {
                            MYSQL_RES* res3 = mysql_store_result(conn);
                            MYSQL_ROW row3 = mysql_fetch_row(res3);
                            if (row3 && row3[0]) {
                                int remaining = stoi(row3[0]);
                                if (remaining < daysTaken) {
                                    cout << "\t\t\t\t\tNot enough leave balance. Approval reverted.\n";
                                    mysql_free_result(res3);
                                    mysql_free_result(res2);
                                    mysql_free_result(res);
                                    system("pause");
                                    return;
                                }
                            }
                            mysql_free_result(res3);
                        }

                        string updateBalance = "UPDATE Leave_Balance SET "
                            "used_days = used_days + " + to_string(daysTaken) +
                            ", remaining_days = remaining_days - " + to_string(daysTaken) +
                            " WHERE employee_id='" + empID + "' "
                            "AND leave_type_id='" + typeID + "' "
                            "AND year=YEAR(CURDATE())";
                        mysql_query(conn, updateBalance.c_str());

                        string logQuery = "INSERT INTO Leave_Log(employee_id, leave_type_id, days_taken, approved_by, approved_date) "
                            "VALUES('" + empID + "', '" + typeID + "', " + to_string(daysTaken) + ", '" +
                            currentEmployeeID + "', CURDATE())";
                        mysql_query(conn, logQuery.c_str());
                    }
                    mysql_free_result(res2);
                }
            }
            mysql_free_result(res);
        }
    }

    system("pause");
}

void exportAttendance() {
    system("cls");
    showBackReminder();
    cout << "\n\n\n";
    cout << "\t\t\t\t\t===========================================\n";
    cout << "\t\t\t\t\t|     E X P O R T   A T T E N D A N C E     |\n";
    cout << "\t\t\t\t\t===========================================\n";

    ofstream out("attendance_report.txt");
    string query = "SELECT attendance_id, employee_id, attendance_date, clock_in_time, clock_out_time, status, working_hours, remarks FROM Attendance";

    if (mysql_query(conn, query.c_str()) == 0) {
        res = mysql_store_result(conn);
        out << "Attendance Report\n=========================\n";
        out << left << setw(12) << "Attn ID"
            << setw(12) << "Emp ID"
            << setw(15) << "Date"
            << setw(20) << "Clock In"
            << setw(20) << "Clock Out"
            << setw(12) << "Status"
            << setw(10) << "Hours"
            << setw(20) << "Remarks" << "\n";
        out << string(120, '-') << "\n";

        while ((row = mysql_fetch_row(res))) {
            out << left << setw(12) << (row[0] ? row[0] : "")
                << setw(12) << (row[1] ? row[1] : "")
                << setw(15) << (row[2] ? row[2] : "")
                << setw(20) << (row[3] ? row[3] : "")
                << setw(20) << (row[4] ? row[4] : "")
                << setw(12) << (row[5] ? row[5] : "")
                << setw(10) << (row[6] ? row[6] : "")
                << setw(20) << (row[7] ? row[7] : "") << "\n";
        }

        mysql_free_result(res);
        out.close();
        cout << "\n\t\t\t\t\tAttendance report saved to 'attendance_report.txt'\n";
    }
    else {
        cout << "\t\t\t\t\tExport failed: " << mysql_error(conn) << "\n";
    }
    system("pause");
}

void exportLeaveSummary() {
    system("cls");
    showBackReminder();
    cout << "\n\n\n";
    cout << "\t\t\t\t\t===========================================\n";
    cout << "\t\t\t\t\t|       E X P O R T   L E A V E   S U M M A R Y      |\n";
    cout << "\t\t\t\t\t===========================================\n";

    ofstream out("leave_summary.txt");
    string query = "SELECT employee_id, leave_type_id, total_days, used_days, remaining_days, year FROM leave_balance";

    if (mysql_query(conn, query.c_str()) == 0) {
        res = mysql_store_result(conn);
        out << "Leave Summary Report\n==============================\n";
        out << left << setw(12) << "Emp ID"
            << setw(14) << "Leave Type"
            << setw(10) << "Total"
            << setw(10) << "Used"
            << setw(12) << "Remaining"
            << setw(8) << "Year" << "\n";
        out << string(70, '-') << "\n";

        while ((row = mysql_fetch_row(res))) {
            out << left << setw(12) << (row[0] ? row[0] : "")
                << setw(14) << (row[1] ? row[1] : "")
                << setw(10) << (row[2] ? row[2] : "")
                << setw(10) << (row[3] ? row[3] : "")
                << setw(12) << (row[4] ? row[4] : "")
                << setw(8) << (row[5] ? row[5] : "") << "\n";
        }

        mysql_free_result(res);
        out.close();
        cout << "\n\t\t\t\t\tLeave summary saved to 'leave_summary.txt'\n";
    }
    else {
        cout << "\t\t\t\t\tExport failed: " << mysql_error(conn) << "\n";
    }

    system("pause");
}

void viewTeamAttendance() {
    system("cls");
    showBackReminder();

    cout << "\n\n\n\n";
    cout << "\t\t\t\t\t===========================================================\n";
    cout << "\t\t\t\t\t|              T E A M   A T T E N D A N C E              |\n";
    cout << "\t\t\t\t\t===========================================================\n";
    cout << "\t\t\t\t\t| 1. View Today's Attendance                              |\n";
    cout << "\t\t\t\t\t| 2. View Last 20 Records                                 |\n";
    cout << "\t\t\t\t\t| 3. Export Team Attendance to CSV                        |\n";
    cout << "\t\t\t\t\t| <. Back                                                 |\n";
    cout << "\t\t\t\t\t===========================================================\n";
    cout << "\t\t\t\t\tChoose: ";

    string input;
    cin >> input;
    if (input == "<") return;

    string query;
    if (input == "1") {
        query =
            "SELECT A.employee_id, E.employee_name, A.attendance_date, A.status, A.working_hours "
            "FROM Attendance A "
            "JOIN Employee E ON A.employee_id = E.employee_id "
            "WHERE E.department_id = '" + managerDeptID + "' "
            "AND A.attendance_date = CURDATE() "
            "ORDER BY A.employee_id";
    }
    else if (input == "2") {
        query =
            "SELECT A.employee_id, E.employee_name, A.attendance_date, A.status, A.working_hours "
            "FROM Attendance A "
            "JOIN Employee E ON A.employee_id = E.employee_id "
            "WHERE E.department_id = '" + managerDeptID + "' "
            "ORDER BY A.attendance_date DESC LIMIT 20";
    }
    else if (input == "3") {
        string exportQuery =
            "SELECT A.employee_id, E.employee_name, A.attendance_date, A.status, A.working_hours "
            "FROM Attendance A "
            "JOIN Employee E ON A.employee_id = E.employee_id "
            "WHERE E.department_id = '" + managerDeptID + "' "
            "ORDER BY A.attendance_date DESC";

        if (mysql_query(conn, exportQuery.c_str()) == 0) {
            res = mysql_store_result(conn);

            time_t now = time(0);
            tm local;
            localtime_s(&local, &now);

            char filename[100];
            strftime(filename, sizeof(filename), "team_attendance_%Y%m%d_%H%M%S.csv", &local);
            ofstream csvFile(filename);

            csvFile << "Employee ID,Employee Name,Date,Status,Working Hours\n";
            while ((row = mysql_fetch_row(res))) {
                for (int i = 0; i < 5; i++) {
                    csvFile << (row[i] ? row[i] : "") << (i < 4 ? "," : "\n");
                }
            }

            csvFile.close();
            mysql_free_result(res);

            cout << "\n\t\t\t\t\tExported successfully to '" << filename << "'\n";
        }
        else {
            cout << "\t\t\t\t\tExport error: " << mysql_error(conn) << "\n";
        }

        cout << "\n\t\t\t\t\t< Back\t\t\t\t\t[ Confirm ]\n";
        system("pause");
        return;
    }
    else {
        cout << "\t\t\t\t\tInvalid option.\n";
        system("pause");
        return;
    }

    qstate = mysql_query(conn, query.c_str());
    if (!qstate) {
        res = mysql_store_result(conn);
        cout << "\n\t\t\t\t\t================== ATTENDANCE RECORD ==================\n";
        cout << left
            << "\t\t\t\t\t" << setw(12) << "Emp ID"
            << setw(22) << "Name"
            << setw(15) << "Date"
            << setw(15) << "Status"
            << setw(10) << "Hours" << "\n";
        cout << "\t\t\t\t\t" << string(80, '-') << "\n";

        while ((row = mysql_fetch_row(res))) {
            cout << "\t\t\t\t\t"
                << setw(12) << (row[0] ? row[0] : "-")
                << setw(22) << (row[1] ? row[1] : "-")
                << setw(15) << (row[2] ? row[2] : "-")
                << setw(15) << (row[3] ? row[3] : "-")
                << setw(10) << (row[4] ? row[4] : "-") << "\n";
        }

        mysql_free_result(res);
    }
    else {
        cout << "\t\t\t\t\tQuery Error: " << mysql_error(conn) << "\n";
    }

    system("pause");
}
void assignStaffToDepartment() {
    system("cls");
    showBackReminder();

    cout << "\n\n\n";
    cout << "\t\t\t\t\t===================================================\n";
    cout << "\t\t\t\t\t|        A S S I G N   S T A F F   T O   D E P T   |\n";
    cout << "\t\t\t\t\t===================================================\n";

    string empID, deptID;

    // View unassigned staff
    string query = "SELECT employee_id, employee_name FROM Employee WHERE user_type='staff' AND department_id IS NULL";
    qstate = mysql_query(conn, query.c_str());

    if (!qstate) {
        res = mysql_store_result(conn);
        int count = mysql_num_rows(res);
        if (count == 0) {
            cout << "\n\t\t\t\t\tAll staff are already assigned to a department.\n";
            system("pause");
            return;
        }

        cout << "\n\t\t\t\t\t----------- Unassigned Staff -----------\n";
        cout << "\t\t\t\t\t" << left << setw(15) << "Employee ID" << setw(25) << "Name" << "\n";
        cout << "\t\t\t\t\t" << string(40, '-') << "\n";
        while ((row = mysql_fetch_row(res))) {
            cout << "\t\t\t\t\t" << setw(15) << row[0] << setw(25) << row[1] << "\n";
        }
        mysql_free_result(res);
    }
    else {
        cout << "\n\t\t\t\t\tError: " << mysql_error(conn) << "\n";
        return;
    }

    // List departments
    cout << "\n\t\t\t\t\t----------- Available Departments -----------\n";
    query = "SELECT department_id, department_name FROM Department";
    qstate = mysql_query(conn, query.c_str());
    if (!qstate) {
        res = mysql_store_result(conn);
        while ((row = mysql_fetch_row(res))) {
            cout << "\t\t\t\t\t" << row[0] << " - " << row[1] << "\n";
        }
        mysql_free_result(res);
    }
    else {
        cout << "\t\t\t\t\tError loading departments.\n";
    }

    // Input
    cout << "\n\t\t\t\t\tEnter Employee ID to assign (< to cancel): ";
    cin >> empID;
    if (empID == "<") return;

    cout << "\t\t\t\t\tEnter Department ID (e.g., DEP001) (< to cancel): ";
    cin >> deptID;
    if (deptID == "<") return;

    // Check if staff is already assigned
    query = "SELECT department_id FROM Employee WHERE employee_id = '" + empID + "'";
    qstate = mysql_query(conn, query.c_str());
    if (!qstate) {
        res = mysql_store_result(conn);
        if ((row = mysql_fetch_row(res)) && row[0] != NULL) {
            cout << "\n\t\t\t\t\tStaff is already assigned to department " << row[0] << ".\n";
            mysql_free_result(res);
            system("pause");
            return;
        }
        mysql_free_result(res);
    }

    // Check if department exists
    query = "SELECT * FROM Department WHERE department_id = '" + deptID + "'";
    qstate = mysql_query(conn, query.c_str());
    res = mysql_store_result(conn);
    if (!res || mysql_num_rows(res) == 0) {
        cout << "\n\t\t\t\t\t Department not found.\n";
        if (res) mysql_free_result(res);
        system("pause");
        return;
    }
    mysql_free_result(res);

    // Assign staff to department
    query = "UPDATE Employee SET department_id='" + deptID + "' WHERE employee_id='" + empID + "' AND user_type='staff'";
    qstate = mysql_query(conn, query.c_str());

    if (!qstate)
        cout << "\n\t\t\t\t\tStaff assigned successfully to department " << deptID << ".\n";
    else
        cout << "\n\t\t\t\t\tError: " << mysql_error(conn) << "\n";

    system("pause");
}

// Show today's clock-in/out status and attendance summary
void showTodayAttendanceStatus() {
    system("cls");
    showBackReminder();

    cout << "\n\n";
    cout << "\t\t\t\t\t=============================================\n";
    cout << "\t\t\t\t\t|       T O D A Y ' S   A T T E N D A N C E  |\n";
    cout << "\t\t\t\t\t=============================================\n";

    string query = "SELECT status, clock_in_time, clock_out_time FROM Attendance "
        "WHERE employee_id = '" + currentEmployeeID + "' AND attendance_date = CURDATE()";

    qstate = mysql_query(conn, query.c_str());
    if (!qstate) {
        res = mysql_store_result(conn);
        if ((row = mysql_fetch_row(res))) {
            cout << "\n\t\t\t\t\tStatus     : " << (row[0] ? row[0] : "-") << "\n";
            cout << "\t\t\t\t\tClock In   : " << (row[1] ? row[1] : "-") << "\n";
            cout << "\t\t\t\t\tClock Out  : " << (row[2] ? row[2] : "-") << "\n";
        }
        else {
            cout << "\n\t\t\t\t\tNo attendance record for today yet.\n";
        }
        mysql_free_result(res);
    }
    else {
        cout << "\n\t\t\t\t\tError fetching today's attendance: " << mysql_error(conn) << "\n";
    }
    system("pause");
}

// Main employee module menu
void employeeModule() {
    string input;
    while (true) {
        system("cls");
        showBackReminder();

        showTodayAttendanceStatus(); // auto-show attendance for today

        cout << "\n\n";
        cout << "\t\t\t\t\t==================================================\n";
        cout << "\t\t\t\t\t|               E M P L O Y E E   M E N U         |\n";
        cout << "\t\t\t\t\t==================================================\n";
        cout << "\t\t\t\t\t|  1. Clock In                                    |\n";
        cout << "\t\t\t\t\t|  2. Clock Out                                   |\n";
        cout << "\t\t\t\t\t|  3. Apply for Leave                             |\n";
        cout << "\t\t\t\t\t|  4. View Leave History                          |\n";
        cout << "\t\t\t\t\t|  5. View Leave Balance                          |\n";
        cout << "\t\t\t\t\t|  6. View Attendance History                     |\n";
        cout << "\t\t\t\t\t|  7. Edit My Profile                             |\n";
        cout << "\t\t\t\t\t|  8. View Leave Log                              |\n";
        cout << "\t\t\t\t\t|  9. Logout                                      |\n";
        cout << "\t\t\t\t\t|  <. Exit                                        |\n";
        cout << "\t\t\t\t\t==================================================\n";
        cout << "\t\t\t\t\tEnter your choice: ";
        cin >> input;

        if (input == "<") {
            if (confirmBack()) return; // go back to login loop
            else continue;
        }

        int choice;
        try {
            choice = stoi(input);
        }
        catch (...) {
            cout << "\t\t\t\t\tInvalid input. Try again.\n";
            system("pause");
            continue;
        }

        switch (choice) {
        case 1: clockIn(); break;
        case 2: clockOut(); break;
        case 3: applyLeave(); break;
        case 4: viewLeaveHistory(); break;
        case 5: viewLeaveBalance(); break;
        case 6: viewAttendanceHistory(); break;
        case 7: editMyProfile(); break;
        case 8: viewLeaveLog(); break;
        case 9: {
            char confirm;
            cout << "\t\t\t\t\tAre you sure you want to logout? (Y/N): ";
            cin >> confirm;
            if (toupper(confirm) == 'Y') {
                login(); // return to login screen
                return;
            }
            else {
                cout << "\t\t\t\t\tLogout cancelled.\n";
                system("pause");
            }
            break;
        }
        default:
            cout << "\t\t\t\t\tInvalid choice. Try again.\n";
            system("pause");
        }
    }
}

void clockIn() {
    system("cls");
    showBackReminder();
    time_t now = time(0);
    tm localtm{};
    localtime_s(&localtm, &now);

    char datetimeStr[20];
    strftime(datetimeStr, sizeof(datetimeStr), "%Y-%m-%d %H:%M:%S", &localtm);

    cout << "\n\n";
    cout << "\t\t\t\t\t=====================================\n";
    cout << "\t\t\t\t\t|             C L O C K   I N        |\n";
    cout << "\t\t\t\t\t=====================================\n";
    cout << "\t\t\t\t\t|  Current Time: " << datetimeStr << "  |\n";
    cout << "\t\t\t\t\t|  (Type < at any prompt to cancel)  |\n";
    cout << "\t\t\t\t\t=====================================\n";

    string query = "SELECT * FROM Attendance WHERE employee_id='" + currentEmployeeID + "' AND attendance_date = CURDATE()";
    qstate = mysql_query(conn, query.c_str());

    if (!qstate) {
        res = mysql_store_result(conn);
        if (mysql_num_rows(res) > 0) {
            cout << "\n\t\t\t\t\tYou have already clocked in today.\n";
        }
        else {
            string confirm;
            cout << "\n\t\t\t\t\tDo you want to clock in now? (Y/N or < to cancel): ";
            cin >> confirm;

            if (confirm == "<" || toupper(confirm[0]) != 'Y') {
                cout << "\t\t\t\t\tClock-in cancelled.\n";
                system("pause");
                return;
            }

            string insertQuery = "INSERT INTO Attendance(employee_id, attendance_date, clock_in_time, status) "
                "VALUES('" + currentEmployeeID + "', CURDATE(), NOW(), 'Present')";
            qstate = mysql_query(conn, insertQuery.c_str());

            if (!qstate)
                cout << "\t\t\t\t\tClocked in successfully at " << datetimeStr << ".\n";
            else
                cout << "\t\t\t\t\tError: " << mysql_error(conn) << endl;
        }

        if (res) mysql_free_result(res);
    }
    else {
        cout << "\t\t\t\t\tError: " << mysql_error(conn) << endl;
    }

    system("pause");
}

void clockOut() {
    system("cls");
    showBackReminder();
    time_t now = time(0);
    tm localtm{};
    localtime_s(&localtm, &now);

    char datetimeStr[20];
    strftime(datetimeStr, sizeof(datetimeStr), "%Y-%m-%d %H:%M:%S", &localtm);

    cout << "\n\n";
    cout << "\t\t\t\t\t=====================================\n";
    cout << "\t\t\t\t\t|            C L O C K   O U T       |\n";
    cout << "\t\t\t\t\t=====================================\n";
    cout << "\t\t\t\t\t|  Current Time: " << datetimeStr << " |\n";
    cout << "\t\t\t\t\t|  (Type < at any prompt to cancel)  |\n";
    cout << "\t\t\t\t\t=====================================\n";

    string query = "SELECT attendance_id, clock_in_time, clock_out_time "
        "FROM Attendance WHERE employee_id='" + currentEmployeeID + "' AND attendance_date = CURDATE()";
    qstate = mysql_query(conn, query.c_str());

    if (!qstate) {
        res = mysql_store_result(conn);
        if (mysql_num_rows(res) > 0) {
            row = mysql_fetch_row(res);
            if (row[1] && !row[2]) {
                string clockInTime = row[1];
                string attendance_id = row[0];

                string durationQuery = "SELECT TIMESTAMPDIFF(MINUTE, '" + clockInTime + "', NOW())";
                int minutesWorked = 0;

                if (!mysql_query(conn, durationQuery.c_str())) {
                    MYSQL_RES* res2 = mysql_store_result(conn);
                    MYSQL_ROW row2 = mysql_fetch_row(res2);
                    if (row2 && row2[0]) {
                        minutesWorked = stoi(row2[0]);
                    }
                    mysql_free_result(res2);
                }

                string status;
                string remarks;
                if (minutesWorked < 240) {
                    status = "Half Day";
                    remarks = "Worked less than 4 hours";
                    cout << "\t\t\t\t\t Warning: You worked less than 4 hours. Marked as Half Day.\n";

                    string insertLog = "INSERT INTO Half_Day_Log (employee_id, date, reason) "
                        "VALUES ('" + currentEmployeeID + "', CURDATE(), 'Less than 4 hours')";
                    mysql_query(conn, insertLog.c_str());

                }
                else if (minutesWorked > 540) {
                    status = "Overtime";
                    remarks = "Worked more than 9 hours";
                    cout << "\t\t\t\t\tGreat! You worked overtime today.\n";

                }
                else {
                    status = "Present";
                    remarks = "Normal working day";
                }

                string confirm;
                cout << "\t\t\t\t\tConfirm clock-out now? (Y/N or < to cancel): ";
                cin >> confirm;
                if (confirm == "<" || toupper(confirm[0]) != 'Y') {
                    cout << "\t\t\t\t\tClock-out cancelled.\n";
                    system("pause");
                    return;
                }

                ostringstream oss;
                oss << fixed << setprecision(2) << (minutesWorked / 60.0);
                string hoursStr = oss.str();

                string updateQuery = "UPDATE Attendance SET clock_out_time = NOW(), status = '" + status +
                    "', working_hours = " + hoursStr +
                    ", remarks = '" + remarks +
                    "' WHERE attendance_id='" + attendance_id + "'";

                qstate = mysql_query(conn, updateQuery.c_str());

                if (!qstate) {
                    cout << "\t\t\t\t\tClocked out successfully at " << datetimeStr << " (" << status << ").\n";
                }
                else {
                    cout << "\t\t\t\t\tUpdate Error: " << mysql_error(conn) << endl;
                }

            }
            else {
                cout << "\t\t\t\t\t You have already clocked out today.\n";
            }
        }
        else {
            cout << "\t\t\t\t\tNo clock-in found. Please clock in first.\n";
        }
        mysql_free_result(res);
    }
    else {
        cout << "\t\t\t\t\tMySQL Error: " << mysql_error(conn) << endl;
    }

    system("pause");
}

void applyLeave() {
    system("cls");
    showBackReminder();
    string leaveTypeID, startDate, endDate, reason;

    cout << "\n\n";
    cout << "\t\t\t\t\t=====================================\n";
    cout << "\t\t\t\t\t|        L E A V E   A P P L Y       |\n";
    cout << "\t\t\t\t\t=====================================\n";
    cout << "\t\t\t\t\t  Employee: " << setw(30) << left << currentEmployeeID << "\n";
    cout << "\t\t\t\t\t  (Type < at any prompt to cancel)  \n";
    cout << "\t\t\t\t\t=====================================\n";

    // Step 1: Show available leave types
    string query = "SELECT leave_type_id, type_name FROM Leave_Type";
    qstate = mysql_query(conn, query.c_str());

    if (!qstate) {
        res = mysql_store_result(conn);
        int count = 0;
        cout << "\n\t\t\t\t\tAvailable Leave Types:\n";
        while ((row = mysql_fetch_row(res))) {
            cout << "\t\t\t\t\t- ID: " << row[0] << " (" << row[1] << ")\n";
            count++;
        }
        mysql_free_result(res);
        if (count == 0) {
            cout << "\t\t\t\t\tNo leave types found. Please contact admin.\n";
            system("pause");
            return;
        }
    }
    else {
        cout << "\t\t\t\t\tError fetching leave types: " << mysql_error(conn) << endl;
        system("pause");
        return;
    }

    // Step 2: Input leave type
    cout << "\n\t\t\t\t\tLeave Type ID: ";
    cin >> leaveTypeID;
    if (leaveTypeID == "<") return;

    // Step 3: Input and validate start date
    do {
        cout << "\t\t\t\t\tStart Date (YYYY-MM-DD): ";
        cin >> startDate;
        if (startDate == "<") return;
        if (!isValidDateFormat(startDate)) {
            cout << "\t\t\t\t\tInvalid format. Use YYYY-MM-DD.\n";
        }
        else if (isNonWorkingDay(startDate)) {
            cout << "\t\t\t\t\tCannot start on weekend/public holiday.\n";
            startDate = "";
        }
    } while (!isValidDateFormat(startDate) || startDate.empty());

    // Step 4: Input and validate end date
    do {
        cout << "\t\t\t\t\tEnd Date (YYYY-MM-DD): ";
        cin >> endDate;
        if (endDate == "<") return;
        if (!isValidDateFormat(endDate)) {
            cout << "\t\t\t\t\tInvalid format. Use YYYY-MM-DD.\n";
        }
        else if (endDate < startDate) {
            cout << "\t\t\t\t\tEnd date must be after or equal to start date.\n";
        }
        else if (isNonWorkingDay(endDate)) {
            cout << "\t\t\t\t\tCannot end on weekend/public holiday.\n";
            endDate = "";
        }
    } while (!isValidDateFormat(endDate) || endDate < startDate || endDate.empty());

    // Step 5: Input reason
    cin.ignore();
    cout << "\t\t\t\t\tReason: ";
    getline(cin, reason);
    if (reason == "<") return;

    // Step 6: Confirm
    string confirm;
    cout << "\n\t\t\t\t\tSubmit this leave request? (Y/N or < to cancel): ";
    cin >> confirm;
    if (confirm == "<" || toupper(confirm[0]) != 'Y') {
        cout << "\t\t\t\t\tLeave application cancelled.\n";
        system("pause");
        return;
    }

    // Step 7: Insert leave request
    query = "INSERT INTO Leave_Request(employee_id, leave_type_id, start_date, end_date, reason, status) VALUES('" +
        currentEmployeeID + "', '" + leaveTypeID + "', '" + startDate + "', '" + endDate + "', '" + reason + "', 'Pending')";

    qstate = mysql_query(conn, query.c_str());
    if (!qstate)
        cout << "\n\t\t\t\t\tLeave request submitted successfully.\n";
    else
        cout << "\n\t\t\t\t\tError: " << mysql_error(conn) << endl;

    system("pause");
}

void viewLeaveHistory() {
    system("cls");
    showBackReminder();

    cout << "\n\n";
    cout << "\t\t\t\t\t===============================================================================\n";
    cout << "\t\t\t\t\t|                             L E A V E   H I S T O R Y                       |\n";
    cout << "\t\t\t\t\t|-----------------------------------------------------------------------------|\n";
    cout << "\t\t\t\t\t|  Employee ID : " << setw(40) << left << currentEmployeeID << "|\n";
    cout << "\t\t\t\t\t==============================================================================\n";

    string query =
        "SELECT lr.leave_request_id, lt.type_name, lr.start_date, lr.end_date, lr.status "
        "FROM Leave_Request lr JOIN Leave_Type lt ON lr.leave_type_id = lt.leave_type_id "
        "WHERE lr.employee_id='" + currentEmployeeID + "'";

    qstate = mysql_query(conn, query.c_str());

    if (!qstate) {
        res = mysql_store_result(conn);
        if (mysql_num_rows(res) == 0) {
            cout << "\n\t\t\t\t\t|  No leave records found.                                                 |\n";
            cout << "\t\t\t\t\t==============================================================================\n";
        }
        else {
            cout << "\n\t\t\t\t\t| " << left
                << setw(13) << "Request ID"
                << setw(18) << "Leave Type"
                << setw(15) << "Start Date"
                << setw(15) << "End Date"
                << setw(12) << "Status" << " |\n";
            cout << "\t\t\t\t\t|" << string(58, '-') << "|\n";

            while ((row = mysql_fetch_row(res))) {
                cout << "\t\t\t\t\t| "
                    << setw(13) << (row[0] ? row[0] : "-")
                    << setw(18) << (row[1] ? row[1] : "-")
                    << setw(15) << (row[2] ? row[2] : "-")
                    << setw(15) << (row[3] ? row[3] : "-")
                    << setw(12) << (row[4] ? row[4] : "-") << " |\n";
            }

            cout << "\t\t\t\t\t===============================================================================\n";
        }
        mysql_free_result(res);
    }
    else {
        cout << "\t\t\t\t\t|Error: " << mysql_error(conn) << "\n";
        cout << "\t\t\t\t\t==============================================================================\n";
    }

    system("pause");
}

void editMyProfile() {
    system("cls");
    showBackReminder();

    string query = "SELECT employee_name, email, contact_number, password FROM Employee WHERE employee_id = '" + currentEmployeeID + "'";
    qstate = mysql_query(conn, query.c_str());

    if (qstate || !(res = mysql_store_result(conn)) || mysql_num_rows(res) == 0) {
        cout << "\t\t\t\t\tError fetching profile: " << mysql_error(conn) << endl;
        system("pause");
        return;
    }

    row = mysql_fetch_row(res);
    string name = row[0], email = row[1], phone = row[2], password = row[3];
    mysql_free_result(res);

    string newName = name, newEmail = email, newPhone = phone, newPass = password;

    int choice;
    do {
        system("cls");
        showBackReminder();

        cout << "\n\n";
        cout << "\t\t\t\t\t====================================================================\n";
        cout << "\t\t\t\t\t|                  EDIT MY PROFILE                                 |\n";
        cout << "\t\t\t\t\t|------------------------------------------------------------------|\n";
        cout << "\t\t\t\t\t|  1. Name           : " << setw(40) << left << newName << "|\n";
        cout << "\t\t\t\t\t|  2. Email          : " << setw(40) << left << newEmail << "|\n";
        cout << "\t\t\t\t\t|  3. Contact Number : " << setw(40) << left << newPhone << "|\n";
        cout << "\t\t\t\t\t|  4. Password       : ********                                    |\n";
        cout << "\t\t\t\t\t|  5. Save Changes                                                 |\n";
        cout << "\t\t\t\t\t|  6. Cancel                                                       |\n";
        cout << "\t\t\t\t\t====================================================================\n";
        cout << "\t\t\t\t\tSelect field to edit: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            cout << "\t\t\t\t\tEnter new name: ";
            if (!getLineOrBack(newName)) return;
            break;

        case 2:
            do {
                cout << "\t\t\t\t\tEnter new email: ";
                if (!getLineOrBack(newEmail)) return;
                if (!isValidEmail(newEmail)) {
                    cout << "\t\t\t\t\tInvalid email format. Try again.\n";
                }
            } while (!isValidEmail(newEmail));
            break;

        case 3:
            do {
                cout << "\t\t\t\t\tEnter new contact number: ";
                if (!getLineOrBack(newPhone)) return;
                if (!isValidPhone(newPhone)) {
                    cout << "\t\t\t\t\tInvalid phone number. Use 10–15 digits.\n";
                }
            } while (!isValidPhone(newPhone));
            break;

        case 4: {
            string tempPass, confirmPass;
            do {
                cout << "\t\t\t\t\tEnter new password: ";
                if (!getLineOrBack(tempPass)) return;

                cout << "\t\t\t\t\tRe-enter to confirm: ";
                if (!getLineOrBack(confirmPass)) return;

                if (tempPass != confirmPass) {
                    cout << "\t\t\t\t\tPasswords do not match. Try again.\n";
                }
            } while (tempPass != confirmPass);

            newPass = tempPass;
            break;
        }

        case 5:
            break;

        case 6:
            cout << "\t\t\t\t\tEdit cancelled.\n";
            system("pause");
            return;

        default:
            cout << "\t\t\t\t\tInvalid option. Try again.\n";
            system("pause");
        }
    } while (choice != 5);

    char confirm;
    cout << "\n\t\t\t\t\tAre you sure you want to save changes? (Y/N): ";
    cin >> confirm;
    if (toupper(confirm) != 'Y') {
        cout << "\t\t\t\t\tUpdate cancelled.\n";
        system("pause");
        return;
    }

    query = "UPDATE Employee SET employee_name='" + newName + "', email='" + newEmail +
        "', contact_number='" + newPhone + "', password='" + newPass +
        "' WHERE employee_id='" + currentEmployeeID + "'";

    if (mysql_query(conn, query.c_str()) == 0)
        cout << "\t\t\t\t\tProfile updated successfully.\n";
    else
        cout << "\t\t\t\t\tError updating profile: " << mysql_error(conn) << endl;

    system("pause");
}

void viewLeaveBalance() {
    system("cls");
    showBackReminder();

    cout << "\n\n";
    cout << "\t\t\t\t\t==============================================================================\n";
    cout << "\t\t\t\t\t|                            LEAVE BALANCE SUMMARY                           |\n";
    cout << "\t\t\t\t\t==============================================================================\n";
    cout << "\t\t\t\t\t|  Employee ID: " << setw(37) << left << currentEmployeeID << "|\n";
    cout << "\t\t\t\t\t|  (Type < to return anytime)                  |\n";
    cout << "\t\t\t\t\t------------------------------------------------------------------------------\n";
    cout << "\t\t\t\t\t|  Leave Type                       | Total | Used | Remain                  |\n";
    cout << "\t\t\t\t\t------------------------------------------------------------------------------\n";

    string query = "SELECT LT.type_name, LB.total_days, LB.used_days, LB.remaining_days "
        "FROM Leave_Balance LB "
        "JOIN Leave_Type LT ON LB.leave_type_id = LT.leave_type_id "
        "WHERE LB.employee_id = '" + currentEmployeeID + "'";

    qstate = mysql_query(conn, query.c_str());

    if (!qstate) {
        res = mysql_store_result(conn);

        if (mysql_num_rows(res) == 0) {
            cout << "\t\t\t\t\t|  No leave balance records found.                         |\n";
        }
        else {
            while ((row = mysql_fetch_row(res))) {
                cout << "\t\t\t\t\t|  " << setw(30) << left << (row[0] ? row[0] : "-")
                    << "| " << setw(5) << (row[1] ? row[1] : "0")
                    << "| " << setw(5) << (row[2] ? row[2] : "0")
                    << "| " << setw(6) << (row[3] ? row[3] : "0") << "|\n";
            }
        }

        cout << "\t\t\t\t\t-------------------------------------------------------------------------\n";
        mysql_free_result(res);
    }
    else {
        cout << "\t\t\t\t\tError: " << mysql_error(conn) << endl;
    }

    system("pause");
}

void DeptviewLeaveBalance() {
    system("cls");
    showBackReminder();

    cout << "\n\n\n\n";
    cout << "\t\t\t\t\t===========================================================\n";
    cout << "\t\t\t\t\t|           DEPARTMENT LEAVE BALANCE SUMMARY             |\n";
    cout << "\t\t\t\t\t===========================================================\n";
    cout << "\t\t\t\t\t|  Department ID: " << setw(42) << left << managerDeptID << "|\n";
    cout << "\t\t\t\t\t|  (Type < to return anytime)                            |\n";
    cout << "\t\t\t\t\t-----------------------------------------------------------\n";
    cout << "\t\t\t\t\t| Emp ID   | Name                   | Type         | Tot | Used | Rem |\n";
    cout << "\t\t\t\t\t-----------------------------------------------------------------------\n";

    string query =
        "SELECT LB.employee_id, E.employee_name, LT.type_name, "
        "LB.total_days, LB.used_days, LB.remaining_days "
        "FROM Leave_Balance LB "
        "JOIN Leave_Type LT ON LB.leave_type_id = LT.leave_type_id "
        "JOIN Employee E ON LB.employee_id = E.employee_id "
        "WHERE E.department_id = '" + managerDeptID + "' "
        "ORDER BY LB.employee_id, LT.type_name";

    qstate = mysql_query(conn, query.c_str());

    if (!qstate) {
        res = mysql_store_result(conn);
        if (mysql_num_rows(res) == 0) {
            cout << "\t\t\t\t\t| No leave balance records found for this department.     |\n";
        }
        else {
            while ((row = mysql_fetch_row(res))) {
                cout << "\t\t\t\t\t| " << setw(8) << (row[0] ? row[0] : "-")
                    << " | " << setw(22) << (row[1] ? row[1] : "-")
                    << " | " << setw(12) << (row[2] ? row[2] : "-")
                    << " | " << setw(3) << (row[3] ? row[3] : "0")
                    << " | " << setw(4) << (row[4] ? row[4] : "0")
                    << " | " << setw(4) << (row[5] ? row[5] : "0") << " |\n";
            }
        }
        mysql_free_result(res);
    }
    else {
        cout << "\t\t\t\t\tError: " << mysql_error(conn) << endl;
    }

    cout << "\t\t\t\t\t-----------------------------------------------------------------------\n";
    system("pause");
}

void viewAttendanceHistory() {
    system("cls");
    showBackReminder();

    cout << "\n\t\t\t\t\t==============================================================================\n";
    cout << "\t\t\t\t\t                                ATTENDANCE HISTORY                           \n";
    cout << "\t\t\t\t\t==============================================================================\n";
    cout << "\t\t\t\t\t  Employee ID: " << setw(40) << left << currentEmployeeID << "\n";
    cout << "\t\t\t\t\t  (Showing latest 30 records. Type < to return)   \n";
    cout << "\t\t\t\t\t--------------------------------------------------------------------------------\n";

    string query = "SELECT attendance_date, clock_in_time, clock_out_time, status, working_hours "
        "FROM Attendance "
        "WHERE employee_id = '" + currentEmployeeID + "' "
        "ORDER BY attendance_date DESC LIMIT 30";

    qstate = mysql_query(conn, query.c_str());

    if (!qstate) {
        res = mysql_store_result(conn);

        if (mysql_num_rows(res) == 0) {
            cout << "\t\t\t\t\t  No attendance records found.                    \n";
        }
        else {
            cout << "\t\t\t\t\t " << left
                << setw(12) << "Date"
                << setw(20) << "Clock In"
                << setw(20) << "Clock Out"
                << setw(12) << "Status"
                << setw(6) << "Hours" << " \n";
            cout << "\t\t\t\t\t--------------------------------------------------------------------------------\n";

            while ((row = mysql_fetch_row(res))) {
                cout << "\t\t\t\t\t "
                    << setw(12) << (row[0] ? row[0] : "-")
                    << setw(20) << (row[1] ? row[1] : "-")
                    << setw(20) << (row[2] ? row[2] : "-")
                    << setw(12) << (row[3] ? row[3] : "-")
                    << setw(6) << (row[4] ? row[4] : "-") << " \n";
            }
        }

        mysql_free_result(res);
    }
    else {
        cout << "\t\t\t\t\tError: " << mysql_error(conn) << " \n";
    }

    cout << "\t\t\t\t\t--------------------------------------------------------------------------------\n";
    system("pause");
}

void viewLeaveLog() {
    system("cls");
    showBackReminder();

    cout << "\n\t\t\t\t\t==============================================\n";
    cout << "\t\t\t\t\t|                 LEAVE LOG                  |\n";
    cout << "\t\t\t\t\t==============================================\n";
    cout << "\t\t\t\t\t  Employee ID: " << setw(35) << left << currentEmployeeID << "\n";
    cout << "\t\t\t\t\t----------------------------------------------\n";

    string query =
        "SELECT LL.leave_type_id, LT.type_name, LL.days_taken, LL.approved_by, LL.approved_date "
        "FROM Leave_Log LL "
        "JOIN Leave_Type LT ON LL.leave_type_id = LT.leave_type_id "
        "WHERE LL.employee_id = '" + currentEmployeeID + "' "
        "ORDER BY LL.approved_date DESC";

    qstate = mysql_query(conn, query.c_str());
    if (!qstate) {
        res = mysql_store_result(conn);

        if (mysql_num_rows(res) == 0) {
            cout << "\t\t\t\t\t  No leave logs found.                       \n";
        }
        else {
            cout << "\t\t\t\t\t " << left
                << setw(10) << "Type ID"
                << setw(22) << "Leave Type"
                << setw(8) << "Days"
                << setw(18) << "Approved By"
                << setw(12) << "Date" << " \n";
            cout << "\t\t\t\t\t----------------------------------------------\n";

            while ((row = mysql_fetch_row(res))) {
                cout << "\t\t\t\t\t "
                    << setw(10) << (row[0] ? row[0] : "-")
                    << setw(22) << (row[1] ? row[1] : "-")
                    << setw(8) << (row[2] ? row[2] : "-")
                    << setw(18) << (row[3] ? row[3] : "-")
                    << setw(12) << (row[4] ? row[4] : "-") << " |\n";
            }
        }

        mysql_free_result(res);
    }
    else {
        cout << "\t\t\t\t\tError: " << mysql_error(conn) << " \n";
    }

    cout << "\t\t\t\t\t--------------------------------------------------------------\n";
    system("pause");
}

void grantBonusLeave() {
    system("cls");
    showBackReminder();
    cout << "\n\n\n\n";
    cout << "\t\t======================================================\n";
    cout << "\t\t|   YEAR-END BONUS LEAVE PROCESSING (OVERTIME)       |\n";
    cout << "\t\t======================================================\n";

    string query =
        "SELECT employee_id, COUNT(*) as overtime_count "
        "FROM Attendance "
        "WHERE status = 'Overtime' AND YEAR(attendance_date) = YEAR(CURDATE()) "
        "GROUP BY employee_id "
        "HAVING overtime_count >= 10";

    qstate = mysql_query(conn, query.c_str());

    if (!qstate) {
        res = mysql_store_result(conn);
        bool found = false;

        while ((row = mysql_fetch_row(res))) {
            found = true;
            string empID = row[0];

            // Insert bonus leave for next year if not exists, else increment
            string insert =
                "INSERT INTO Leave_Balance (employee_id, leave_type_id, total_days, used_days, remaining_days, year) "
                "SELECT '" + empID + "', leave_type_id, 2, 0, 2, YEAR(CURDATE()) + 1 "
                "FROM Leave_Balance "
                "WHERE employee_id = '" + empID + "' LIMIT 1 "
                "ON DUPLICATE KEY UPDATE total_days = total_days + 2, remaining_days = remaining_days + 2";

            if (mysql_query(conn, insert.c_str()) == 0)
                cout << "\t\t+ Bonus Leave Granted to " << empID << " for Next Year.\n";
            else
                cout << "\t\tError granting bonus to " << empID << ": " << mysql_error(conn) << "\n";
        }

        if (!found) {
            cout << "\n\t\tNo employees qualified for bonus leave this year.\n";
        }

        mysql_free_result(res);
    }
    else {
        cout << "\t\tError: " << mysql_error(conn) << endl;
    }

    system("pause");
}



void generateLeaveStats() {
    system("cls");
    showBackReminder();
    cout << "\n\n\n\n";
    cout << "\t\t\t\t\t=========================================\n";
    cout << "\t\t\t\t\t|       LEAVE USAGE STATISTICS REPORT   |\n";
    cout << "\t\t\t\t\t=========================================\n";

    vector<int> monthlyCounts(13, 0);

    string query =
        "SELECT MONTH(start_date) AS Month, COUNT(*) AS LeaveCount "
        "FROM Leave_Request "
        "WHERE status = 'Approved' "
        "GROUP BY MONTH(start_date)";

    qstate = mysql_query(conn, query.c_str());
    int total = 0, modeMonth = 0, modeValue = 0;

    if (!qstate) {
        res = mysql_store_result(conn);

        cout << "\n\t\t\t\t\tMonth\t\tTotal Approved Leaves\n";
        while ((row = mysql_fetch_row(res))) {
            int month = atoi(row[0]);
            int count = atoi(row[1]);
            monthlyCounts[month] = count;
            total += count;

            if (count > modeValue) {
                modeValue = count;
                modeMonth = month;
            }

            cout << "\t\t\t\t\t" << setw(2) << month << "\t\t\t" << count << "\n";
        }

        double mean = total / 12.0, variance = 0;
        for (int i = 1; i <= 12; i++) {
            variance += pow(monthlyCounts[i] - mean, 2);
        }
        variance /= 12;

        cout << "\n\t\t\t\t\tMost Frequent Month (Mode) : " << modeMonth << endl;
        cout << "\t\t\t\t\tVariance in Leave Counts    : " << fixed << setprecision(2) << variance << endl;

        mysql_free_result(res);
    }
    else {
        cout << "\t\t\t\t\tError: " << mysql_error(conn) << endl;
    }

    system("pause");
}

void exportLeaveDataToCSV() {
    system("cls");
    showBackReminder();
    cout << "\n\n\n\n";
    cout << "\t\t\t\t\t=============================================\n";
    cout << "\t\t\t\t\t|         EXPORT LEAVE DATA TO CSV FILE     |\n";
    cout << "\t\t\t\t\t=============================================\n";

    ofstream file("leave_report.csv");
    file << "Employee ID,Employee Name,Leave Type,Used Days,Month,Year\n";

    string query =
        "SELECT E.employee_id, E.employee_name, LT.type_name, "
        "DATEDIFF(LR.end_date, LR.start_date) + 1 AS used_days, "
        "MONTH(LR.start_date) AS month, YEAR(LR.start_date) AS year "
        "FROM Leave_Request LR "
        "JOIN Employee E ON LR.employee_id = E.employee_id "
        "JOIN Leave_Type LT ON LR.leave_type_id = LT.leave_type_id "
        "WHERE LR.status = 'Approved'";

    qstate = mysql_query(conn, query.c_str());

    if (!qstate) {
        res = mysql_store_result(conn);
        while ((row = mysql_fetch_row(res))) {
            for (int i = 0; i < 6; i++) {
                file << "\"" << row[i] << "\"";
                if (i < 5) file << ",";
            }
            file << "\n";
        }

        cout << "\t\t\t\t\tLeave report exported to 'leave_report.csv'.\n";
        mysql_free_result(res);
    }
    else {
        cout << "\t\t\t\t\tExport failed: " << mysql_error(conn) << endl;
    }

    file.close();
    system("pause");
}

void adminManageLeaveRequests() {
    while (true) {
        system("cls");
        cout << "\n\n\n";
        cout << "\t\t============================================\n";
        cout << "\t\t        ADMIN: MANAGE LEAVE REQUESTS        \n";
        cout << "\t\t============================================\n";
        cout << "\t\t1. View All Pending Requests\n";
        cout << "\t\t2. Approve / Reject Leave Request\n";
        cout << "\t\t3. < Back [ Confirm ]\n";
        cout << "\n\t\tEnter your choice: ";
        string choice;
        cin >> choice;

        if (choice == "1") {
            system("cls");
            cout << "\n\n\n";
            cout << "\t\t=========== PENDING LEAVE REQUESTS ==========\n";

            string query = "SELECT leave_request_id, employee_id, leave_type_id, start_date, end_date, reason, status "
                "FROM leave_request WHERE status = 'Pending'";
            qstate = mysql_query(conn, query.c_str());

            if (qstate == 0) {
                res = mysql_store_result(conn);
                int count = 0;
                while ((row = mysql_fetch_row(res))) {
                    cout << "\n\t\tRequest ID : " << row[0]
                        << "\n\t\tEmployee ID: " << row[1]
                        << "\n\t\tLeave Type : " << row[2]
                        << "\n\t\tStart Date : " << row[3]
                        << "\n\t\tEnd Date   : " << row[4]
                        << "\n\t\tReason     : " << row[5]
                        << "\n\t\tStatus     : " << row[6] << "\n";
                    cout << "\t\t--------------------------------------------\n";
                    count++;
                }

                if (count == 0) {
                    cout << "\n\t\tNo pending requests found.\n";
                }

                mysql_free_result(res);
            }
            else {
                cout << "\n\t\tError retrieving leave requests: " << mysql_error(conn) << endl;
            }

            system("pause");
        }

        else if (choice == "2") {
            system("cls");
            cout << "\n\n\n";
            cout << "\t\t=========== PENDING LEAVE REQUESTS ==========\n";

            // Show all pending first
            string query = "SELECT leave_request_id, employee_id, leave_type_id, start_date, end_date, reason "
                "FROM leave_request WHERE status = 'Pending'";
            qstate = mysql_query(conn, query.c_str());

            if (qstate == 0) {
                res = mysql_store_result(conn);
                int count = 0;
                while ((row = mysql_fetch_row(res))) {
                    cout << "\n\t\tRequest ID : " << row[0]
                        << "\n\t\tEmployee ID: " << row[1]
                        << "\n\t\tLeave Type : " << row[2]
                        << "\n\t\tStart Date : " << row[3]
                        << "\n\t\tEnd Date   : " << row[4]
                        << "\n\t\tReason     : " << row[5] << "\n";
                    cout << "\t\t--------------------------------------------\n";
                    count++;
                }

                if (count == 0) {
                    cout << "\n\t\tNo pending requests to process.\n";
                    mysql_free_result(res);
                    system("pause");
                    continue;
                }

                mysql_free_result(res);
            }
            else {
                cout << "\n\t\tError retrieving pending requests: " << mysql_error(conn) << endl;
                system("pause");
                continue;
            }

            string requestId;
            cout << "\n\t\tEnter Request ID to Approve/Reject (or '<' to go back): ";
            cin >> requestId;
            if (requestId == "<") {
                char confirm;
                cout << "\n\t\tAre you sure you want to go back? (Y/N): ";
                cin >> confirm;
                if (toupper(confirm) == 'Y') continue;
                else continue; // Redisplay the list
            }

            string fetchQuery = "SELECT employee_id, leave_type_id, start_date, end_date, reason "
                "FROM leave_request WHERE leave_request_id = '" + requestId + "' AND status = 'Pending'";
            qstate = mysql_query(conn, fetchQuery.c_str());

            if (qstate == 0) {
                res = mysql_store_result(conn);
                if ((row = mysql_fetch_row(res))) {
                    string empId = row[0];
                    string leaveType = row[1];
                    string startDate = row[2];
                    string endDate = row[3];
                    string reason = row[4];

                    cout << "\n\t\tSelected Request Details:\n";
                    cout << "\t\tEmployee ID : " << empId << endl;
                    cout << "\t\tLeave Type  : " << leaveType << endl;
                    cout << "\t\tStart Date  : " << startDate << endl;
                    cout << "\t\tEnd Date    : " << endDate << endl;
                    cout << "\t\tReason      : " << reason << endl;

                    char decision;
                    cout << "\n\t\tApprove or Reject this request? (A/R): ";
                    cin >> decision;

                    string status = (toupper(decision) == 'A') ? "Approved" : "Rejected";

                    string update = "UPDATE leave_request SET status = '" + status + "', "
                        "approved_by = '" + currentEmployeeID + "', "
                        "approved_date = CURDATE() "
                        "WHERE leave_request_id = '" + requestId + "'";

                    qstate = mysql_query(conn, update.c_str());
                    if (!qstate)
                        cout << "\n\t\tRequest successfully " << status << ".\n";
                    else
                        cout << "\n\t\tFailed to update request: " << mysql_error(conn) << endl;
                }
                else {
                    cout << "\n\t\tRequest not found or already processed.\n";
                }

                mysql_free_result(res);
            }
            else {
                cout << "\n\t\tQuery error: " << mysql_error(conn) << endl;
            }

            system("pause");
        }

        else if (choice == "3" || choice == "<") {
            char confirm;
            cout << "\n\t\tAre you sure you want to go back? (Y/N): ";
            cin >> confirm;
            if (toupper(confirm) == 'Y') return;
        }

        else {
            cout << "\n\t\tInvalid option. Please try again.\n";
            system("pause");
        }
    }
}
void adminViewLeaveBalances() {
    while (true) {
        system("cls");
        cout << "\n\n\n";
        cout << "\t\t============================================\n";
        cout << "\t\t          ADMIN: VIEW LEAVE BALANCES        \n";
        cout << "\t\t============================================\n";

        string query =
            "SELECT E.employee_id, E.employee_name, L.leave_type_id, LT.type_name, "
            "L.total_days, L.used_days, L.remaining_days, L.year "
            "FROM leave_balance L "
            "JOIN employee E ON L.employee_id = E.employee_id "
            "JOIN leave_type LT ON L.leave_type_id = LT.leave_type_id "
            "ORDER BY E.employee_id, L.leave_type_id";

        qstate = mysql_query(conn, query.c_str());

        if (qstate == 0) {
            res = mysql_store_result(conn);
            string lastEmpID = "";
            bool hasResults = false;

            while ((row = mysql_fetch_row(res))) {
                hasResults = true;
                string empID = row[0];
                string empName = row[1];

                if (empID != lastEmpID) {
                    cout << "\n\t--------------------------------------------------------\n";
                    cout << "\tEmployee ID : " << empID << "\n";
                    cout << "\tEmployee Name: " << empName << "\n";
                    cout << "\t--------------------------------------------------------\n";
                    cout << "\tLeave Type     | Total | Used | Remaining | Year\n";
                    cout << "\t--------------------------------------------------------\n";
                    lastEmpID = empID;
                }

                cout << "\t" << setw(15) << left << row[3]
                    << " | " << setw(5) << row[4]
                    << " | " << setw(5) << row[5]
                    << " | " << setw(9) << row[6]
                    << " | " << row[7] << "\n";
            }

            if (!hasResults) {
                cout << "\n\t\tNo leave balances found.\n";
            }

            mysql_free_result(res);
        }
        else {
            cout << "\n\t\tQuery Error: " << mysql_error(conn) << endl;
        }

        char back;
        cout << "\n\t\t< Back [ Confirm ] - Type '<' to return: ";
        cin >> back;
        if (back == '<') {
            char confirm;
            cout << "\n\t\tAre you sure you want to go back? (Y/N): ";
            cin >> confirm;
            if (toupper(confirm) == 'Y') return;
        }
    }
}
