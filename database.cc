#include "database.hh"

#include <mysql/mysql.h>

#include <cstdint>
#include <sstream>
#include <iostream>

Database::ConnectionData::ConnectionData(const std::string& host, const std::string& user,
                                         const std::string& password, const std::string& database)
    : m_host(host),
      m_user(user),
      m_password(password),
      m_database(database)
{
}

Database::ConnectionData::ConnectionData(const ConnectionData& other)
    : m_host(other.m_host),
      m_user(other.m_user),
      m_password(other.m_password),
      m_database(other.m_database)
{
}

Database::ConnectionData::ConnectionData(ConnectionData&& other)
    : m_host(std::move(other.m_host)),
      m_user(std::move(other.m_user)),
      m_password(std::move(other.m_password)),
      m_database(std::move(other.m_database))
{
}

Database::ConnectionData& Database::ConnectionData::operator=(const ConnectionData& other)
{
    if (this != &other)
    {
        m_host = other.m_host;
        m_user = other.m_user;
        m_password = other.m_password;
        m_database = other.m_database;
    }
    return *this;
}

Database::ConnectionData& Database::ConnectionData::operator=(ConnectionData&& other)
{
    if (this != &other)
    {
        m_host = std::move(other.m_host);
        m_user = std::move(other.m_user);
        m_password = std::move(other.m_password);
        m_database = std::move(other.m_database);
    }
    return *this;
}

Database::ResultSet::ResultSet(st_mysql_res* result)
    : m_result(result)
{
}

Database::ResultSet::~ResultSet()
{
    if (m_result)
    {
        mysql_free_result(m_result);
    }
}

std::vector<std::string> Database::ResultSet::getRow()
{
    auto row = mysql_fetch_row(m_result);
    std::vector<std::string> rowData;

    if (row)
    {
        uint32_t numFields = mysql_num_fields(m_result);
        rowData.reserve(numFields);
        for (uint32_t i = 0; i < numFields; ++i)
        {
            rowData.push_back(row[i] ? row[i] : "NULL");
        }
    }

    return rowData;
}

std::string Database::ResultSet::asString()
{
    std::stringstream ss;
    while (true)
    {
        auto row = getRow();
        if (row.empty())
        {
            break;
        }
        for (const auto& value : row)
        {
            ss << value << " ";
        }
        ss << "\n";
    }
    return ss.str();
}

Database::Database(const Database::ConnectionData& connectionData)
    : m_connectionData(connectionData)
{
    m_connection = mysql_init(nullptr);

    if (!m_connection)
    {
        std::cerr << "Error initializing MySQL connection" << std::endl;
        return;
    }

    if (mysql_real_connect(m_connection, m_connectionData.getHost().c_str(),
                           m_connectionData.getUser().c_str(), m_connectionData.getPassword().c_str(),
                           m_connectionData.getDatabase().c_str(), 0, nullptr, 0) == nullptr)
    {
        mysql_close(m_connection);
        m_connection = nullptr;
        std::cout << "MySQL connection closed!" << std::endl;
    }
}

Database::Database(Database&& other)
    : m_connection(std::move(other.m_connection)),
      m_connectionData(std::move(other.m_connectionData))
{
}

Database::~Database()
{
    if (m_connection)
    {
        mysql_close(m_connection);
        std::cout << "MySQL connection closed!" << std::endl;
    }
}

Database& Database::operator=(Database&& other)
{
    if (this != &other)
    {
        m_connection = std::move(other.m_connection);
        m_connectionData = std::move(other.m_connectionData);
    }
    return *this;
}

bool Database::executeNonQuery(const std::string& query)
{
    if (!m_connection)
    {
        std::cerr << "Not connected to MySQL server" << std::endl;
        return false;
    }

    if (mysql_query(m_connection, query.c_str()))
    {
        std::cerr << "Error executing SQL query: " <<  mysql_error(m_connection) << std::endl;;
        return false;
    }
    return true;
}

Database::ResultSet Database::executeQuery(const std::string& query)
{
    if (!m_connection)
    {
        std::cerr << "Not connected to MySQL server" << std::endl;
        return ResultSet(nullptr);
    }

    if (mysql_query(m_connection, query.c_str()))
    {
        std::cerr<< "Error executing SQL query: " <<  mysql_error(m_connection) << std::endl;
        return ResultSet(nullptr);
    }

    st_mysql_res* result = mysql_store_result(m_connection);
    if (!result)
    {
        std::cerr << "Error storing result set: " <<  mysql_error(m_connection) << std::endl;
        return ResultSet(nullptr);
    }

    return ResultSet(result);
}

