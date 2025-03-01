#ifndef MYSQL_DATABASE_HH
#define MYSQL_DATABASE_HH

#include <string>
#include <vector>

struct st_mysql;
struct st_mysql_res;

class Database
{
public:
    class ConnectionData
    {
    public:
        /// Constructor.
        ConnectionData(const std::string& host, const std::string& user, const std::string& password, const std::string& database);

        /// Copy constructor.
        ConnectionData(const ConnectionData& other);

        /// Move constructor.
        ConnectionData(ConnectionData&& other);

        /// Copy assignment operator.
        ConnectionData& operator=(const ConnectionData& other);

        /// Move assignment operator.
        ConnectionData& operator=(ConnectionData&& other);

    public:
        /// Get connection host.
        std::string getHost()
        {
            return m_host;
        }

        /// Get connection username.
        std::string getUser()
        {
            return m_user;
        }

        /// Get connection password.
        std::string getPassword()
        {
            return m_password;
        }

        /// Get database name.
        std::string getDatabase()
        {
            return m_database;
        }

    private:
        std::string m_host, m_user, m_password, m_database;
    };

    class ResultSet
    {
    public:
        /// Constructor.
        ResultSet(st_mysql_res* result);

        /// Destructor.
        ~ResultSet();

        /// Get data of the query.
        std::vector<std::string> getRow();

        /// Get the contents of the query as a string.
        std::string asString();

    private:
        st_mysql_res* m_result;
    };

    /// Constructor.
    Database(const ConnectionData& connectionData);

    /// Destructor.
    ~Database();

    /// Deleted copy constructor.
    Database(const Database& other) = delete;

    /// Move constructor.
    Database(Database&& other);

    /// Deleted copy assignment operator.
    Database& operator=(const Database& other) = delete;

    /// Move assignment operator.
    Database& operator=(Database&& other);

    /// For query operations that don't return anything.
    /// -> INSERT, UPDATE, DELETE, etc.
    bool executeNonQuery(const std::string& query);

    /// For queries that return results, like SELECT.
    ResultSet executeQuery(const std::string& query);

private:
    st_mysql* m_connection;
    ConnectionData m_connectionData;
};

#endif
