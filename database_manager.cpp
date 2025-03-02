#include "database_manager.hpp"

DatabaseManager::DatabaseManager()
{
    try
    {
        driver = sql::mysql::get_mysql_driver_instance();

        // Connect without specifying a schema
        con.reset(driver->connect("tcp://127.0.0.1:3306", "project", "password09"));

        // Create database if it doesn't exist
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        stmt->execute("CREATE DATABASE IF NOT EXISTS word_game_db");

        // Select the database
        con->setSchema("word_game_db");

        stmt->execute("CREATE TABLE IF NOT EXISTS users ("
                      "id INT AUTO_INCREMENT PRIMARY KEY,"
                      "username VARCHAR(50) UNIQUE NOT NULL,"
                      "password VARCHAR(255) NOT NULL,"
                      "high_score INT DEFAULT 0)");
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "SQL Exception: " << e.what() << std::endl;
    }
}

DatabaseManager::~DatabaseManager() = default;

bool DatabaseManager::registerUser(const std::string &username, const std::string &password)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
            "INSERT INTO users (username, password) VALUES (?, ?)"));
        pstmt->setString(1, username);
        pstmt->setString(2, password);
        pstmt->execute();
        return true;
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "SQL Exception: " << e.what() << std::endl;
        return false;
    }
}

bool DatabaseManager::loginUser(const std::string &username, const std::string &password)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
            "SELECT * FROM users WHERE username = ? AND password = ?"));
        pstmt->setString(1, username);
        pstmt->setString(2, password);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        return res->next();
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "SQL Exception: " << e.what() << std::endl;
        return false;
    }
}

int DatabaseManager::getHighScore(const std::string &username)
{
    int highScore = 0;
    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
            "SELECT high_score FROM users WHERE username = ?"));
        pstmt->setString(1, username);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next())
        {
            highScore = res->getInt("high_score");
        }
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "SQL Exception: " << e.what() << std::endl;
    }
    return highScore;
}

void DatabaseManager::updateHighScore(const std::string &username, int newHighScore)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
            "UPDATE users SET high_score = ? WHERE username = ?"));
        pstmt->setInt(1, newHighScore);
        pstmt->setString(2, username);
        pstmt->executeUpdate();
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "SQL Exception: " << e.what() << std::endl;
    }
}
