#include "database.hh"

#include <iostream>

int main()
{
    Database::ConnectionData connectionData("0.0.0.0", "root", "my_password", "my_database");

    Database db(connectionData);
    db.executeNonQuery("CREATE TABLE IF NOT EXISTS cpp (id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(255))");
    if (db.executeNonQuery("INSERT INTO cpp (name) VALUES ('Moika')"))
    {
        std::cout << "Query executed successfully" << std::endl;
    }

    auto result = db.executeQuery("SELECT * FROM cpp;");
    std::cout << result.asString() << std::endl;
}
