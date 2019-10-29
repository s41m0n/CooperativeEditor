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
    std::string sql= "CREATE TABLE User ("
                "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                "Username VARCHAR NOT NULL UNIQUE,"
                "Password VARCHAR NOT NULL,"
                "Name VARCHAR,"
                "Surname VARCHAR,"
                "Email VARCHAR NOT NULL UNIQUE,"
                "PhotoPath     VARCHAR);";
    char* messaggeError;
    int exitCode = sqlite3_exec(this->DBConnection, sql.c_str(), nullptr, nullptr, &messaggeError);
    if (exitCode != SQLITE_OK)
        sqlite3_free(messaggeError);
}

bool Database::checkTable() {
    bool ret=false;
    char* messaggeError;
    std::string sql="SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='User';";
    int count;
    int exitCode = sqlite3_exec(this->DBConnection, sql.c_str(), callbackCount, &count, &messaggeError);
    if (exitCode != SQLITE_OK) {
        sqlite3_free(messaggeError);
    }
    else {
        if (count == 0)
            ret=false;

        if (count == 1)
            ret=true;
    }
    return ret;
}

int Database::openConnection() {
    int exitCode=0;
    exitCode = sqlite3_open("EditorDB.db", &DBConnection);

    if (exitCode)
        return (-1);

    else
        return 0;
}

bool Database::insertUser(User& user) {
    bool retValue=false;
    openConnection();
    std::string sql="INSERT INTO User VALUES(NULL,?,?,?,?,?,'path');";
    const char* zTail = nullptr;
    const char** pzTail = &zTail;
    sqlite3_stmt* statement = nullptr;
    int exitCode = sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1,&statement,pzTail);
    if (exitCode != SQLITE_OK)
        return  retValue;

    sqlite3_bind_text(statement, 1, user.getUsername().toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, user.getPassword().toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, user.getName().toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4, user.getSurname().toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 5, user.getEmail().toStdString().c_str(), -1, SQLITE_TRANSIENT);

    exitCode = sqlite3_step(statement);
    if(exitCode !=SQLITE_DONE)
        return retValue;

    User temp(user.getIcon(), user.getUsername(),user.getName(), user.getSurname(), user.getEmail());
    user=std::move(temp);
    retValue=true;

    sqlite3_finalize(statement);
    sqlite3_close(this->DBConnection);
    return  retValue;
}

bool Database::queryUserPass(User& user) {
    openConnection();
    bool retValue= false;
    std::string sql="SELECT * FROM User WHERE Username=? AND Password=?;";
    const char* zTail = nullptr;
    const char** pzTail = &zTail;
    sqlite3_stmt* statement = nullptr;
    int retval = sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1,&statement,pzTail);
    sqlite3_bind_text(statement, 1, user.getUsername().toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, user.getPassword().toStdString().c_str(), -1, SQLITE_TRANSIENT);
    if (retval != SQLITE_OK)
        return retval;

    retval = sqlite3_step(statement);
    if (retval == SQLITE_ROW) {
        // There is another row after this one: read this row
        User temp(reinterpret_cast<const char *>(sqlite3_column_text(statement, 6)),
                  reinterpret_cast<const char *>(sqlite3_column_text(statement, 1)),
                  reinterpret_cast<const char *>(sqlite3_column_text(statement, 3)),
                  reinterpret_cast<const char *>(sqlite3_column_text(statement, 4)),
                  reinterpret_cast<const char *>(sqlite3_column_text(statement, 5)));
        user = std::move(temp);
        retValue = true;
    }

    sqlite3_finalize(statement);
    sqlite3_close(this->DBConnection);
    return retValue;

}

bool Database::updateUser(User& user){
    bool retValue=false;
    openConnection();
    std::string sql="UPDATE User set Password = ?, Name= ?, Surname = ?, Email = ? where Username = ?;";
    const char* zTail = nullptr;
    const char** pzTail = &zTail;
    sqlite3_stmt* statement = nullptr;
    int exitCode = sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1,&statement,pzTail);
    if (exitCode != SQLITE_OK)
        return  retValue;

    sqlite3_bind_text(statement, 5, user.getUsername().toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 1, user.getPassword().toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, user.getName().toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, user.getSurname().toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4, user.getEmail().toStdString().c_str(), -1, SQLITE_TRANSIENT);

    exitCode = sqlite3_step(statement);
    if(exitCode !=SQLITE_DONE)
        return retValue;

    User temp(user.getIcon(), user.getUsername(),user.getName(), user.getSurname(), user.getEmail());
    user=std::move(temp);
    retValue=true;

    sqlite3_finalize(statement);
    sqlite3_close(this->DBConnection);
    return  retValue;
}

bool Database::deleteUser(User user) {
    bool retValue=false;
    openConnection();
    char* messaggeError;
    std::string sql="DELETE FROM User WHERE Username = '"+user.getUsername().toStdString()+"';";
    int exitCode = sqlite3_exec(DBConnection, sql.c_str(), nullptr, nullptr, &messaggeError);
    if (exitCode != SQLITE_OK)
        sqlite3_free(messaggeError);

    else
        retValue=true;

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

