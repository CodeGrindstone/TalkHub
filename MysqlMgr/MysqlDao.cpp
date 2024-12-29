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

bool MysqlDao::RegisterUser(std::string const &email, std::string const &pwd, int& errorCode) 
{
    // 从连接池获取数据库连接
    auto conn = pool_->getConnection();
    if (conn == nullptr) {
        errorCode = -1;
        return false;
    }

    // 确保数据库连接在退出时归还连接池
    Defer defer([this, &conn] {
        pool_->returnConnection(std::move(conn));
    });

    try {
        // 调用存储过程
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "CALL register_user(?, ?, @out_error_code);"));
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

bool MysqlDao::isUserExistByEmail(std::string const &email, int& errorCode) {
    // 从连接池获取数据库连接
    auto conn = pool_->getConnection();
    if (conn == nullptr) {
        errorCode = -1;
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
    // 从连接池获取数据库连接
    auto conn = pool_->getConnection();
    if (conn == nullptr) {
        errorCode = -1;
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

bool MysqlDao::VerifyUser(std::string const &email, std::string const &pwd,bool &isValid, int &errorCode, 
                        int &uid, std::string &name, std::string &sex, std::string &icon, std::string &bio) 
{
    isValid = false;
    // 从连接池获取数据库连接
    auto conn = pool_->getConnection();
    if (conn == nullptr) {
        errorCode = -1;
        return false;
    }

    Defer defer([this, &conn] {
        pool_->returnConnection(std::move(conn));
    });
    try
    {
        // 调用存储过程
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "CALL verify_user_by_email(?, ?, @out_is_valid, @out_error_code, @out_uid, @out_name, @out_sex, @out_profile_picture, @out_bio);"));
        
        stmt->setString(1, email);
        stmt->setString(2, pwd);
        stmt->execute();

        // 获取存储过程输出参数
        std::unique_ptr<sql::Statement> stmtOut(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmtOut->executeQuery(
            "SELECT @out_is_valid AS is_valid, @out_error_code AS error_code, @out_uid AS uid, @out_name AS name, @out_sex AS sex, @out_profile_picture AS profile_picture, @out_bio AS bio"
        ));
        
        if (res->next()) {
            isValid = res->getInt("is_valid") == 1;
            errorCode = res->getInt("error_code");
            uid = res->getUInt("uid");
            name = res->getString("name");
            sex = res->getString("sex");
            icon = res->getString("profile_picture");
            bio = res->getString("bio");
            return true;
        }
    }
    catch (const sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
    errorCode = 1;
    return false;   
}

bool MysqlDao::SearchUserByUid(int search_uid, std::string &name,
                               std::string &sex, std::string &icon,
                               std::string &desc, int &errorCode) {
    auto conn = pool_->getConnection();
    if (conn == nullptr) {
        errorCode = -1;
        return false;
    }

    Defer defer([this, &conn] {
        pool_->returnConnection(std::move(conn));
    });

    try
    {
        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement("CALL GetUserInfoByUid(?, @username, @gender, @profile_picture, @bio, @error_code)")
        );
        stmt->setInt(1, search_uid);

        // 执行存储过程
        stmt->execute();

        // 获取存储过程的输出参数
        std::unique_ptr<sql::Statement> queryStmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(
            queryStmt->executeQuery(
                "SELECT @username AS username,        "
                "@gender AS gender,                   "
                "@profile_picture AS profile_picture, "
                "@bio AS bio,                         "
                "@error_code AS error_code            "
            )
        );

        if (res->next()) {
            name = res->getString("username");
            sex = res->getString("gender");
            icon = res->getString("profile_picture");
            desc = res->getString("bio");
            errorCode = res->getInt("error_code");
            return true;
        }
    }
    catch (const sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
    errorCode = 1;
    return false;
}

bool MysqlDao::Applytoaddfriends(int uid, int peer_id,
                                 std::string const &remark_name,
                                 std::string const &request_message,
                                 int &errorCode) 
{
    auto conn = pool_->getConnection();
    if (conn == nullptr) {
        errorCode = -1;
        return false;
    }

    Defer defer([this, &conn] {
        pool_->returnConnection(std::move(conn));
    });

    try
    {
        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement("CALL insert_request_friend(?, ?, ?, ?, @error_code)")
        );

        stmt->setInt(1, uid);
        stmt->setInt(2, peer_id);
        stmt->setString(3, remark_name);
        stmt->setString(4, request_message);

        // 执行存储过程
        stmt->execute();

        // 获取存储过程的输出参数
        std::unique_ptr<sql::Statement> queryStmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(
            queryStmt->executeQuery("SELECT @error_code AS error_code")
        );

        if (res->next()) {
            errorCode = res->getInt("error_code");
            return true;
        }
    }
    catch (const sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
    errorCode = 1;
    return false;     
}

bool MysqlDao::AuthFriendReq(int requester_uid, int receiver_uid, int &errorCode) 
{
    auto conn = pool_->getConnection();
    if (conn == nullptr) {
        errorCode = -1;
        return false;
    }

    Defer defer([this, &conn] {
        pool_->returnConnection(std::move(conn));
    });

    try
    {
        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement("CALL auth_friend_req(?, ?, @error_code)")
        );

        stmt->setInt(1, requester_uid);
        stmt->setInt(2, receiver_uid);

        // 执行存储过程
        stmt->execute();

        // 获取存储过程的输出参数
        std::unique_ptr<sql::Statement> queryStmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(
            queryStmt->executeQuery("SELECT @error_code AS error_code")
        );

        if (res->next()) {
            errorCode = res->getInt("error_code");
            return true;
        }
    }
    catch (const sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
    errorCode = 1;
    return false; 
}

bool MysqlDao::GetUserBaseInfo(int uid, std::string& name, 
                                std::string &email, std::string &gender,
                                std::string &icon, std::string &bio,
                                int &errorCode) 
{
    auto conn = pool_->getConnection();
    if (conn == nullptr) {
        errorCode = -1;
        return false;
    }

    Defer defer([this, &conn] {
        pool_->returnConnection(std::move(conn));
    });

    try
    {
        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement("CALL get_user_base_info(?, @error_code, @name, @email, @gender, @profile_picture, @bio)")
        );

        stmt->setInt(1, uid);

        // 执行存储过程
        stmt->execute();

        // 获取存储过程的输出参数
        std::unique_ptr<sql::Statement> queryStmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(
            queryStmt->executeQuery(
                "SELECT @error_code AS error_code, @name AS name, @email AS email, @gender AS gender, @profile_picture AS profile_picture, @bio AS bio"    
            ));

        if (res->next()) {
            errorCode = res->getInt("error_code");
            name = res->getString("name");
            email = res->getString("email");
            gender = res->getString("gender"),
            icon = res->getString("profile_picture");
            bio = res->getString("bio");
            if(errorCode == 2)
                return false;
            return true;
        }
    }
    catch (const sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
    errorCode = 1;
    return false; 
}

bool MysqlDao::GetReceivedFriendReq(int uid, std::vector<int> &request_uids,
                                    std::vector<std::string> &request_messages,
                                    std::vector<std::string> &status,
                                    int &errorCode) 
                                    
{
    errorCode = 0;
    auto conn = pool_->getConnection();
    if (conn == nullptr) {
        errorCode = -1;
        return false;
    }

    Defer defer([this, &conn] {
        pool_->returnConnection(std::move(conn));
    });

    try
    {
        // 准备存储过程调用，包含一个输入参数和一个 OUT 参数
        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement("CALL get_received_friend_request(?, @error_code)")
        );
        
        // 设置输入参数
        stmt->setInt(1, uid);

        // 执行存储过程
        stmt->execute();

        // 清空之前的数据
        request_uids.clear();
        request_messages.clear();
        status.clear();
        errorCode = 1; // 默认错误码为 1

        // 处理存储过程返回的所有结果集
        bool hasResultSet = true;
        while (hasResultSet) {
            std::unique_ptr<sql::ResultSet> res(stmt->getResultSet());
            if (res) {
                // 假设第一个结果集是 friend request 数据
                while (res->next()) {
                    request_uids.push_back(res->getInt("requester_id"));
                    request_messages.push_back(res->getString("request_message"));
                    status.push_back(res->getString("status"));
                }
            }
            // 检查是否有更多的结果集
            hasResultSet = stmt->getMoreResults();
        }

        // 确保所有结果集都已处理完毕，避免 "Commands out of sync" 错误

        // 获取 OUT 参数 @error_code
        std::unique_ptr<sql::Statement> stmtOut(conn->createStatement());
        std::unique_ptr<sql::ResultSet> resOut(stmtOut->executeQuery("SELECT @error_code AS error_code"));

        if (resOut->next()) {
            errorCode = resOut->getInt("error_code");
            std::cout << "Error Code: " << errorCode << std::endl;
        } else {
            // 如果没有返回 @error_code，则设为 1
            errorCode = 1; 
            std::cout << "No Error Code returned. Set to default: " << errorCode << std::endl;
        }

        return true;

    }
    catch (const sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
    errorCode = 1;
    return false; 
}

bool MysqlDao::GetUserFriend(int uid, std::vector<int>& friends_uid,
                             int &errorCode) 
{
    errorCode = 0;
    auto conn = pool_->getConnection();
    if (conn == nullptr) {
        errorCode = -1;
        return false;
    }

    Defer defer([this, &conn] {
        pool_->returnConnection(std::move(conn));
    });

    try
    {
        // 准备存储过程调用，包含一个输入参数和一个 OUT 参数
        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement("CALL get_user_friend(?, @error_code)")
        );
        
        // 设置输入参数
        stmt->setInt(1, uid);

        // 执行存储过程
        stmt->execute();

        // 清空之前的数据
        friends_uid.clear();

        // 处理存储过程返回的所有结果集
        bool hasResultSet = true;
        while (hasResultSet) {
            std::unique_ptr<sql::ResultSet> res(stmt->getResultSet());
            if (res) {
                // 假设第一个结果集是 friend request 数据
                while (res->next()) {
                    int uid = res->getInt("friend_id");
                    friends_uid.push_back(uid);
                }
            }
            // 检查是否有更多的结果集
            hasResultSet = stmt->getMoreResults();
        }

        // 确保所有结果集都已处理完毕，避免 "Commands out of sync" 错误

        // 获取 OUT 参数 @error_code
        std::unique_ptr<sql::Statement> stmtOut(conn->createStatement());
        std::unique_ptr<sql::ResultSet> resOut(stmtOut->executeQuery("SELECT @error_code AS error_code"));

        if (resOut->next()) {
            errorCode = resOut->getInt("error_code");
            std::cout << "Error Code: " << errorCode << std::endl;
        } else {
            // 如果没有返回 @error_code，则设为 1
            errorCode = 1; 
            std::cout << "No Error Code returned. Set to default: " << errorCode << std::endl;
        }

        return true;

    }
    catch (const sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
    errorCode = 1;
    return false; 
}

bool MysqlDao::SendPrivateMes(int fromUid, int toUid, std::string const &message, int &errorCode) 
{
    auto conn = pool_->getConnection();
    if (conn == nullptr) {
        errorCode = -1;
        return false;
    }

    Defer defer([this, &conn] {
        pool_->returnConnection(std::move(conn));
    });

    try
    {
        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement("CALL save_private_message(?, ?, ?, ?, @error_code)")
        );

        stmt->setInt(1, fromUid);
        stmt->setInt(2, toUid);
        stmt->setString(3, "text");
        stmt->setString(4, message);

        // 执行存储过程
        stmt->execute();

        // 获取存储过程的输出参数
        std::unique_ptr<sql::Statement> queryStmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(
            queryStmt->executeQuery(
                "SELECT @error_code AS error_code"    
            ));

        if (res->next()) {
            errorCode = res->getInt("error_code");
            return true;
        }
    }
    catch (const sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
    errorCode = 1;
    return false; 
}

bool MysqlDao::GetUserSendMes(int uid, std::vector<int> &receiver_id,
                              std::vector<std::string> &contents,
                              int &errorCode) 
{
    auto conn = pool_->getConnection();
    if (conn == nullptr) {
        errorCode = -1;
        return false;
    }

    Defer defer([this, &conn] {
        pool_->returnConnection(std::move(conn));
    });

    try
    {
        // 使用 std::unique_ptr 管理 PreparedStatement 的生命周期
        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement(
                "SELECT * "
                "FROM messages "
                "WHERE sender_id = ?;"
            )
        );


        stmt->setInt(1, uid);

        // 执行存储过程
        stmt->execute();

        // 执行查询
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        // 遍历结果集
        while (res->next()) {
            receiver_id.push_back(res->getInt("receiver_id"));
            contents.push_back(res->getString("content"));
        }
        return true;
    }
    catch (const sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
    errorCode = 1;
    return false; 
}

bool MysqlDao::GetUserRecvMes(int uid, std::vector<int> &sender_id,
                              std::vector<std::string> &contents,
                              int &errorCode) 
{
    auto conn = pool_->getConnection();
    if (conn == nullptr) {
        errorCode = -1;
        return false;
    }

    Defer defer([this, &conn] {
        pool_->returnConnection(std::move(conn));
    });

    try
    {
        // 使用 std::unique_ptr 管理 PreparedStatement 的生命周期
        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement(
                "SELECT * "
                "FROM messages "
                "WHERE receiver_id = ?;"
            )
        );


        stmt->setInt(1, uid);

        // 执行存储过程
        stmt->execute();

        // 执行查询
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        // 遍历结果集
        while (res->next()) {
            sender_id.push_back(res->getInt("sender_id"));
            contents.push_back(res->getString("content"));
        }
        return true;
    }
    catch (const sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
    errorCode = 1;
    return false; 
}
