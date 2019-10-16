//
// Created by francesco on 15/10/19.
//

#ifndef COOPERATIVEEDITOR_DATABASE_H
#define COOPERATIVEEDITOR_DATABASE_H

#include <sqlite3.h>
#include <string>


class Database{
private:
    sqlite3* DBConnection;

    void createTableUser();
    bool checkTable();
    int openConnection();
    static int callback(void *data, int colNum, char **dataRow, char **colName);
    static int callbackCount (void* data, int colNum, char**dataRow, char** colName);
public:
    Database();
    void insertUser(std::string username, std::string password, std::string name, std::string surname, std::string email);
    void queryUserPass(std::string username, std::string password);
    void updateUser(std::string username, std::string password, std::string name, std::string surname, std::string email);
    void deleteUser(std::string username);
    //void run();
    ~Database();
};



#endif //COOPERATIVEEDITOR_DATABASE_H
