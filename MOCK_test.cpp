#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream> 
#include <string>

using namespace std;
using ::testing::_;
using ::testing::NiceMock;
class IDBconnection
{
    public:
    IDBconnection() {};
    virtual ~IDBconnection() {};
    virtual bool open(const string& login, const string& password)  = 0;
    virtual bool close() = 0;
    virtual bool exacQuery(const string& query, string& answer) = 0;
};

class MockDBconnection : public IDBconnection
{
    public:
    MOCK_METHOD(bool, open, (const string& login, const string& password), (override));
    MOCK_METHOD(bool, close, (), (override));
    MOCK_METHOD(bool, exacQuery, (const string& query, string& answer), (override));
};

class RealConnection: public IDBconnection
{
public:
    RealConnection() {};
    virtual ~RealConnection() {};
    virtual bool open(const string& login, const string& password) 
    {
        return true;
    }
    virtual bool close()
    {
        return true;
    }
    virtual bool exacQuery(const string& query, string& answer)
    {
        return true;
    }

};

class ClassThatUsesDB
{
    private:
        IDBconnection* m_DBC;
        string m_login;
        string m_password;
        
        
    public:  
        string m_query;
        string m_result;

        ClassThatUsesDB(IDBconnection* DBC, string& login, string& password): m_DBC(DBC), m_login(login), m_password(password)
        {
            if (!m_DBC) cout << "Error of connection" << endl;
            else cout << "Connection - ok" << endl;
        }
        ~ClassThatUsesDB()
        {
            m_DBC->close();
        }

        bool openConnection(IDBconnection* DBC)
        {
           assert(DBC);
           return DBC->open(m_login, m_password);
        }        

        bool closeConnection(IDBconnection* DBC)
        {
            return DBC->close();
        }
        bool useConnection(IDBconnection* DBC)
        {
            return DBC->exacQuery(m_query, m_result);
        }
};

class TestSuite : public ::testing::Test
{
protected:
    void SetUp()
    {
        TestDBC = new RealConnection();
    }

    void TearDown()
    {
        delete TestDBC;
    }

    IDBconnection* TestDBC;
};

TEST_F(TestSuite, test1)
{
    MockDBconnection MDBC; 
    string login = "login" ;
    string password = "password" ;
    ClassThatUsesDB user (&MDBC, login, password);
    EXPECT_CALL(MDBC, open(_, _)).Times(::testing::AtLeast(1))
                                 .WillOnce(testing::Return(true));
    EXPECT_CALL(MDBC, close).Times(::testing::AtLeast(2))
                            .WillRepeatedly(testing::Return(true));                            
    bool Open = user.openConnection(&MDBC);
    ASSERT_TRUE(Open);
    bool Close = user.closeConnection(&MDBC);
    ASSERT_TRUE(Close);
}

TEST_F(TestSuite, test2)
{
    MockDBconnection MDBC; 
    string login = "login";
    string password = "password";
    ClassThatUsesDB user (&MDBC, login, password);
    EXPECT_CALL(MDBC, open).Times(::testing::AtLeast(1))
                           .WillOnce(testing::Return(false));
    EXPECT_CALL(MDBC, close).Times(::testing::AtLeast(1));
    bool res = user.openConnection(&MDBC);               
    ASSERT_FALSE(res);   
}

TEST_F(TestSuite, test3)
{
    NiceMock<MockDBconnection>MDBC;
    string login = "login";
    string password = "password";
    ClassThatUsesDB user(&MDBC, login, password);  
    user.m_query = "query";
    user.m_result = "";  

    EXPECT_CALL(MDBC, exacQuery(user.m_query, user.m_result))
                           .Times(::testing::AtLeast(1))
                           .WillOnce(testing::Return(true));    
    bool res = user.useConnection(&MDBC);
    ASSERT_TRUE(res); 
   
}

TEST_F(TestSuite, test4)
{
   
    NiceMock<MockDBconnection>MDBC;
    string login = "login";
    string password = "password";
    ClassThatUsesDB user(&MDBC, login, password);  
    user.m_query = "query";
    user.m_result = "";  

    EXPECT_CALL(MDBC, exacQuery(user.m_query, user.m_result))
                           .Times(::testing::AtLeast(1))
                           .WillOnce(testing::Return(false));    
    bool res = user.useConnection(&MDBC);
    ASSERT_FALSE(res); 
   
}


int main(int argc, char* argv[])
{    
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}