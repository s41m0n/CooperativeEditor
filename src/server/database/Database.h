//
// Created by francesco on 15/10/19.
//

#ifndef COOPERATIVEEDITOR_DATABASE_H
#define COOPERATIVEEDITOR_DATABASE_H

#include <sqlite3.h>
#include <string>
#include <src/common/User.h>


class Database{
private:
    // The connection object
    sqlite3* DBConnection;

    //The constructor (private because is a singleton class)
    Database();

    //Delete the copy constructor
    Database(const Database&)=delete;

    //Delete the overloading of = operator
    void operator=(const Database&)=delete;

    //Method to create User table in the database
    void createTableUser();

    //Method to check if the User table has been already created
    bool checkTable();

    //Method to open a connection to the database
    int openConnection();

    //Callback method used to check if the User table exists
    static int callbackCount (void* data, int colNum, char**dataRow, char** colName);

    //Destructor
    ~Database();
public:

    //Static method to get the singleton instance
    static Database& getInstance(){
        static Database DBinstance;
        return DBinstance;
    }

    //Method to insert a new user in the database
    bool insertUser(User user);

    //Method to check the user password
    bool queryUserPass(std::string username, std::string password, User& user);

    //Method to update the user attributes
    bool updateUser(User user);

    //Method to delete an user
    bool deleteUser(std::string username);

};



#endif //COOPERATIVEEDITOR_DATABASE_H
