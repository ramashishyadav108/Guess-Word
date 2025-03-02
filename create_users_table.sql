CREATE DATABASE IF NOT EXISTS word_game_db;
USE word_game_db;

CREATE TABLE IF NOT EXISTS users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,
    high_score INT DEFAULT 0
);

