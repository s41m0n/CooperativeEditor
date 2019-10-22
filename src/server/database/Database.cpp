//
// Created by francesco on 15/10/19.
//

#include "Database.h"
#include <iostream>

Database::Database() {
    openConnection();
    if(!checkTable())
        createTableUser();
    sqlite3_close(this->DBConnection);
}

void Database::createTableUser() {
    std::cout<<"CREATE TABLE CALLED\n";
    std::string sql= "CREATE TABLE User ("
                "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                "Username VARCHAR NOT NULL UNIQUE,"
                "Password VARCHAR NOT NULL,"
                "Name VARCHAR,"
                "Surname VARCHAR,"
                "Email VARCHAR NOT NULL UNIQUE);";
    char* messaggeError;
    int exitCode = sqlite3_exec(this->DBConnection, sql.c_str(), NULL, 0, &messaggeError);
    if (exitCode != SQLITE_OK) {
        std::cerr << "Error Create Table" << std::endl;
        sqlite3_free(messaggeError);
    }
    else
        std::cout << "Table created Successfully" << std::endl;
}

bool Database::checkTable() {
    bool ret=false;
    char* messaggeError;
    std::string sql="SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='User';";
    int count;
    int exitCode = sqlite3_exec(this->DBConnection, sql.c_str(), callbackCount, &count, &messaggeError);
    if (exitCode != SQLITE_OK) {
        std::cerr << "Error CheckTable" << std::endl;
        sqlite3_free(messaggeError);
    }
    else {
        if (count == 0) {
            std::cout << "Table User not created yet" << std::endl;
            ret=false;
        }
        if (count == 1) {
            std::cout << "Table User already exists" << std::endl;
            ret=true;
        }
    }
    return ret;
}

int Database::openConnection() {
    int exitCode=0;
    exitCode = sqlite3_open("EditorDB.db", &DBConnection);

    if (exitCode) {
        std::cerr << "Error open DB " << sqlite3_errmsg(DBConnection) << std::endl;
        return (-1);
    }
    else {
        std::cout << "Opened Database Successfully!" << std::endl;
        return 0;
    }

}

void Database::insertUser(std::string username, std::string password, std::string name,
                          std::string surname, std::string email) {
    openConnection();
    std::string sql="INSERT INTO User VALUES(NULL,'"+username+"', '"+password+"', '"+name+"','"+surname+"','"+email+"');";
    char* messaggeError;
    int exitCode = sqlite3_exec(DBConnection, sql.c_str(), NULL, 0, &messaggeError);
    if (exitCode != SQLITE_OK) {
        std::cerr << "Error Insert" << std::endl;
        sqlite3_free(messaggeError);
    }
    else
        std::cout << "User inserted successfully!" << std::endl;
    sqlite3_close(this->DBConnection);
}

void Database::queryUserPass(std::string username, std::string password) {
    std::string query = "SELECT Password FROM User WHERE Username='"+username+"';";
    sqlite3_exec(DBConnection, query.c_str(),callback, &password, NULL);
}

void Database::updateUser(std::string username, std::string password, std::string name,
                          std::string surname, std::string email) {
    openConnection();
    std::string sql="UPDATE User set Password = '"+password+"', Name ='"+name+"' , Surname='"+surname+"', Email='"+email+"' where Username='"+username+"'; ";
    char* messaggeError;
    int exitCode = sqlite3_exec(DBConnection, sql.c_str(), NULL, 0, &messaggeError);
    if (exitCode != SQLITE_OK) {
        std::cerr << "Error in Update user" << std::endl;
        sqlite3_free(messaggeError);
    }
    else
        std::cout << "User updated successfully!" << std::endl;
    sqlite3_close(this->DBConnection);
}

void Database::deleteUser(std::string username) {
    openConnection();
    char* messaggeError;
    std::string sql="DELETE FROM User WHERE Username = '"+username+"';";
    int exitCode = sqlite3_exec(DBConnection, sql.c_str(), NULL, 0, &messaggeError);
    if (exitCode != SQLITE_OK) {
        std::cerr << "Error DELETE" << std::endl;
        sqlite3_free(messaggeError);
    }
    else
        std::cout << "Record deleted Successfully!" << std::endl;
    sqlite3_close(this->DBConnection);
}

int Database::callback(void *data, int colNum, char **dataRow, char **colName) {
    int idx=0;
    std::cout<<"There are"<<colNum<<" column(s)\n";

    for (idx = 0; idx < colNum; idx++) {
        printf("The data in column \"%s\" is: %s\n", colName[idx], dataRow[idx]);
    }

    printf("\n");

    return 0;
}

int Database::callbackCount (void* data, int colNum, char**dataRow, char** colName){
    if (colNum == 1 && dataRow) {
        *static_cast<int*>(data) = atoi(dataRow[0]);
        return 0;
    }
    return 1;
}

Database:: ~Database(){
    sqlite3_close(this->DBConnection);
}

