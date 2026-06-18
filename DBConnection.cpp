#include "DBConnection.h"
using namespace std;

// Initialize database connection variables
MYSQL* conn = nullptr;
MYSQL_ROW row = nullptr;
MYSQL_RES* res = nullptr;
int qstate = 0;

MYSQL* initializeConnection() {
    conn = mysql_init(0);
    if (!conn) {
        cerr << "MySQL initialization failed" <<endl;
        return nullptr;
    }

    conn = mysql_real_connect(conn, "localhost", "root", "", "attendance_leave", 3306, NULL, 0);

    if (conn) {
        cout << "Database connected successfully." <<endl;
    }
    else {
        cerr << "Database connection failed: " << mysql_error(conn) << endl;
    }

    return conn;
}