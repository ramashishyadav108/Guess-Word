#ifndef DATABASE_MANAGER_HPP
#define DATABASE_MANAGER_HPP

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/statement.h>
#include <string>
#include <memory>
#include <iostream>

class DatabaseManager
{
private:
    sql::mysql::MySQL_Driver *driver;
    std::unique_ptr<sql::Connection> con;

public:
    DatabaseManager();
    ~DatabaseManager();
    bool registerUser(const std::string &username, const std::string &password);
    bool loginUser(const std::string &username, const std::string &password);
    int getHighScore(const std::string &username);
    void updateHighScore(const std::string &username, int newHighScore);
};

#endif // DATABASE_MANAGER_HPP
