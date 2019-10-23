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
                "Email VARCHAR NOT NULL UNIQUE,"
                "PhotoPath     VARCHAR);";
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

bool Database::insertUser(User user) {
    bool retValue=false;
    openConnection();
    std::string sql="INSERT INTO User VALUES(NULL,'"+user.getUsername().toStdString()+"', '"+user.getPassword().toStdString()+"', '"+user.getName().toStdString()+"','"+user.getSurname().toStdString()+"','"+user.getEmail().toStdString()+"','path');";
    char* messaggeError;
    int exitCode = sqlite3_exec(DBConnection, sql.c_str(), NULL, 0, &messaggeError);
    if (exitCode != SQLITE_OK) {
        std::cerr << "Error Insert" << std::endl;
        sqlite3_free(messaggeError);
    }
    else{
        std::cout << "User inserted successfully!" << std::endl;
        retValue=true;
    }

    sqlite3_close(this->DBConnection);
    return  retValue;
}

bool Database::queryUserPass(std::string username, std::string password, User& user) {
    openConnection();
    bool retValue= false;
    std::string sql="SELECT * FROM User WHERE Username='"+username+"' AND Password='"+password+"';";
    const char* zTail = nullptr;
    const char** pzTail = &zTail;
    sqlite3_stmt* statement = nullptr;
    int retval = sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1,&statement,pzTail);
    if (retval != SQLITE_OK) {
        std::cerr << "prepare: " << sqlite3_errstr(retval) << '\n';
        return retval;
    }
    const int num_columns = sqlite3_column_count(statement);
    retval = sqlite3_step(statement);
    if (retval == SQLITE_ROW) {// There is another row after this one: read this row
        User temp(reinterpret_cast<const char*>(sqlite3_column_text(statement, 6)),reinterpret_cast<const char*>(sqlite3_column_text(statement, 1)), reinterpret_cast<const char*>(sqlite3_column_text(statement, 3)), reinterpret_cast<const char*>(sqlite3_column_text(statement, 4)), reinterpret_cast<const char*>(sqlite3_column_text(statement, 5)));
        user=temp;
        /*for (int i = 0; i < num_columns; ++i) {
            const unsigned char* row_element =sqlite3_column_text(statement, i);
            std::cout << row_element << '\n';
        }*/
        std::cout << '\n';
        retValue=true;
    }
    else if (retval == SQLITE_DONE) {
        // no more rows: exit
        std::cout << "Done\n";
    }
    else {
        std::cerr << "step: " << sqlite3_errstr(retval) << '\n';
    }
    retval = sqlite3_finalize(statement);
    if (retval != SQLITE_OK)
        std::cerr << "finalize: " <<sqlite3_errstr(retval) << '\n';

    sqlite3_close(this->DBConnection);
    return retValue;

}

bool Database::updateUser(User user){
    bool retValue=false;
    openConnection();
    std::string sql="UPDATE User set Password = '"+user.getPassword().toStdString()+"', Name ='"+user.getName().toStdString()
            +"' , Surname='"+user.getSurname().toStdString()+"', Email='"+user.getEmail().toStdString()+"' where Username='"+user.getUsername().toStdString()+"'; ";
    char* messaggeError;
    int exitCode = sqlite3_exec(DBConnection, sql.c_str(), NULL, 0, &messaggeError);
    if (exitCode != SQLITE_OK) {
        std::cerr << "Error in Update user" << std::endl;
        sqlite3_free(messaggeError);
    }
    else{
        std::cout << "User updated successfully!" << std::endl;
        retValue=true;
    }

    sqlite3_close(this->DBConnection);
    return retValue;
}

bool Database::deleteUser(std::string username) {
    bool retValue=false;
    openConnection();
    char* messaggeError;
    std::string sql="DELETE FROM User WHERE Username = '"+username+"';";
    int exitCode = sqlite3_exec(DBConnection, sql.c_str(), NULL, 0, &messaggeError);
    if (exitCode != SQLITE_OK) {
        std::cerr << "Error DELETE" << std::endl;
        sqlite3_free(messaggeError);
    }
    else{
        std::cout << "Record deleted Successfully!" << std::endl;
        retValue=true;
    }

    sqlite3_close(this->DBConnection);
    return retValue;
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

