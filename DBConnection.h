#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <mysql.h>
#include <iostream>

// Database connection variables
extern MYSQL* conn;
extern MYSQL_ROW row;
extern MYSQL_RES* res;
extern int qstate;

// Function to initialize database connection
MYSQL* initializeConnection();

#endif