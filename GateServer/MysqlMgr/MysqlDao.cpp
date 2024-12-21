#include "MysqlDao.h"
#include <iostream>
#include "assert.h"

MysqlDao::MysqlDao() 
{
    auto & cfg = ConfigMgr::getInstance();
    const auto& host = cfg["Mysql"]["Host"];
    const auto& port = cfg["Mysql"]["Port"];
    const auto& pwd = cfg["Mysql"]["Passwd"];
    const auto& schema = cfg["Mysql"]["Schema"];
    const auto& user = cfg["Mysql"]["User"];
    pool_.reset(new MySqlPool(host+":"+port, user, pwd,schema, 5));
}

MysqlDao::~MysqlDao() 
{
    pool_->Close();
}

bool MysqlDao::RegisterUser(std::string const &email, std::string const &pwd, int& userNumber) 
{
    // 初始化输出参数
    userNumber = -1;

    // 从连接池获取数据库连接
    auto conn = pool_->getConnection();
    if (conn == nullptr) {
        return false;
    }

    // 确保数据库连接在退出时归还连接池
    Defer defer([this, &conn] {
        pool_->returnConnection(std::move(conn));
    });

    try {
        // 调用存储过程
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "CALL register_user(?, ?, @out_user_number, @out_error_code);"));
        stmt->setString(1, email);
        stmt->setString(2, pwd);
        stmt->execute();

        // 获取输出参数
        std::unique_ptr<sql::Statement> stmtOut(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmtOut->executeQuery(
            "SELECT @out_user_number AS user_number, @out_error_code AS error_code"));

        if (res->next()) {
            userNumber = res->getInt("user_number");
            int errorCode = res->getInt("error_code");
            
            if (errorCode == 0) {
                // 成功插入
                return true;
            } else {
                userNumber = errorCode;
                return false;
            }
        }
    } catch (const sql::SQLException& e) {
        userNumber = 1;
    }
    return false;
}

bool MysqlDao::isUserExistByEmail(std::string const &email, int& errorCode) {
    
    errorCode = -1;
    // 从连接池获取数据库连接
    auto conn = pool_->getConnection();
    if (conn == nullptr) {
        return false;
    }

    Defer defer([this, &conn] {
        pool_->returnConnection(std::move(conn));
    });

    try {
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "SELECT COUNT(*) FROM users WHERE email = ?"));

        // 设置参数
        stmt->setString(1, email);

        // 执行查询
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        if (res->next()) {
            int count = res->getInt(1);
            if(count > 0)   errorCode = 1;
            else errorCode = 0;
            return true;
        }
    } catch (const sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
    errorCode = 1;
    return false;
}

bool MysqlDao::ResetPasswdByEmail(std::string const &email,
                                  std::string const &pwd, int &errorCode)
{
    errorCode = -1;
    // 从连接池获取数据库连接
    auto conn = pool_->getConnection();
    if (conn == nullptr) {
        return false;
    }

    Defer defer([this, &conn] {
        pool_->returnConnection(std::move(conn));
    });
    
    try
    {
        // 调用存储过程
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "CALL reset_passwd_by_email(?, ?, @out_error_code);")
        );
        stmt->setString(1, email);
        stmt->setString(2, pwd);
        stmt->execute();

        // 获取输出参数
        std::unique_ptr<sql::Statement> stmtOut(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmtOut->executeQuery(
            "SELECT @out_error_code AS error_code"));

        if (res->next()) {
            errorCode = res->getInt("error_code");
            return true;
        }
    } catch (const sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
    errorCode = 1;
    return false;
}

bool MysqlDao::VerifyUser(std::string const &email, std::string const &pwd,
                        bool &isValid, int &errorCode, unsigned int& uid) 
{
    isValid = false;
    errorCode = -1;
    uid = 0;
    // 从连接池获取数据库连接
    auto conn = pool_->getConnection();
    if (conn == nullptr) {
        return false;
    }

    Defer defer([this, &conn] {
        pool_->returnConnection(std::move(conn));
    });
    try
    {
        // 调用存储过程
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "CALL verify_user(?, ?, @out_is_valid, @out_error_code, @out_uid);"));
        stmt->setString(1, email);
        stmt->setString(2, pwd);
        stmt->execute();

        // 获取存储过程输出参数
        std::unique_ptr<sql::Statement> stmtOut(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmtOut->executeQuery(
            "SELECT @out_is_valid AS is_valid, @out_error_code AS error_code, @out_uid AS uid"));
        
        if (res->next()) {
            isValid = res->getInt("is_valid") == 1;
            errorCode = res->getInt("error_code");
            uid = res->getUInt("uid");
            return true;
        }
    }
    catch (const sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
    errorCode = 1;
    return false;   
}

// void testRegisterUser(MysqlDao* userDao)
// {
//     int errorCode;
//     std::cout << "------ Test Case 1: Successful Registration ------" << std::endl;
//     int userNumber = userDao->RegisterUser("newuser1@example.com", "password123", errorCode);
//     assert(errorCode == 0);
//     assert(userNumber > 0);
//     std::cout << "Test Case 1 Passed: User registered successfully. User Number: " << userNumber << std::endl;

//     std::cout << "------ Test Case 2: Email Already Exists ------" << std::endl;
//     userNumber = userDao->RegisterUser("newuser1@example.com", "password123", errorCode);
//     assert(errorCode == 2);
//     std::cout << "Test Case 2 Passed: Duplicate email handled correctly." << std::endl;

//     std::cout << "------ Test Case 3: Invalid Input (Empty Email) ------" << std::endl;
//     userNumber = userDao->RegisterUser("", "password123", errorCode);
//     assert(errorCode == 1);
//     std::cout << "Test Case 3 Passed: Invalid email detected as expected." << std::endl;

//     std::cout << "------ Test Case 4: SQL Exception Simulation ------" << std::endl;
//     // // 使用错误的数据库信息模拟异常情况
//     // UserDao* invalidDao = new UserDaoImpl("tcp://invalid_host", DB_USER, DB_PASS, DB_NAME);
//     // userNumber = invalidDao->RegisterUser("erroruser@example.com", "password123", errorCode);
//     // assert(errorCode == 1);
//     // std::cout << "Test Case 4 Passed: SQL Exception handled correctly." << std::endl;

//     // 释放资源
//     delete userDao;
//     //delete invalidDao;

//     std::cout << "All Test Cases Passed!" << std::endl;
// }

// int main()
// {
//     MysqlDao mysqldao;
//     testRegisterUser(&mysqldao);
//     return 0;
// }