#include <JWTMiddleware.h>
// #include <ostream>


JWTMiddleware::JWTMiddleware() : Middleware("JWT")
{
}

bool JWTMiddleware::run(HTTPRequest *_req, HTTPResponse *_res)
{
    if (_req != nullptr)
    {
        string str = _req->getHeaderValue("Accept");
        string jwt = _req->getHeaderValue("Authorization");
        cout << jwt << endl;
        pair<string, bool> tokenSuccess = verifyToken(jwt);
        cout << "tokenSuccess.first: " << tokenSuccess.first << endl;
        bool isVerified = verifyUser(tokenSuccess.first);
        if(isVerified == true){
            injectUserData(_req, {{"userID", tokenSuccess.first}});
            return true;
        }
        return false;

        cout << "IS Successful: " << tokenSuccess.second << endl;
    }
    else 
    {

    }

    cout << "This is JWTMiddleware::run()" << endl;
    return true;
}
Middleware *JWTMiddleware::clone()
{
    Middleware * jwtMiddleware =  new JWTMiddleware();
    jwtMiddleware->init(this->getParams());
    return jwtMiddleware;
}

void JWTMiddleware::connectDatabase()
{
    json connectionData = getParamValue("auth_db"); 
    int port  = connectionData["port"];
    string hostname = connectionData["hostname"];
    string username = connectionData["username"];
    string password = connectionData["password"];
    string database = connectionData["database"];
    connection = new PSQLConnection(hostname,port,database,username,password);
}

void JWTMiddleware::init(json initData)
{
    setParams(initData);
    connectDatabase();
}

PSQLConnection * JWTMiddleware::getDatabaseConnection()
{
    if (!connection->isAlive())
        cout << "Database connection is not alive" << endl;
    return connection;
}


pair<string, bool> JWTMiddleware::verifyToken(string authToken)
{

    try {
        string secretKey = getParamValue("SECRET");

        string tokenString ;
        if ( strncasecmp(authToken.c_str(),"Bearer",strlen("Bearer")) ==0 )
        {
            const char * s = authToken.c_str() + strlen("Bearer")+1;
            tokenString = s;
        }else{
            return {"", false};
        }

        string jwt(tokenString);

        Signer signer(secretKey);
        Token token = signer.verify(jwt);

        string userID =  token.payload().get("user_id");

        // bool isVerified = verifyUser(userID);

        // if(isVerified == true){
            return {userID, true};
        // }
        // cout << "Cannot verify Token, User ID Might be invalid" << endl;
        // return {"", false};
    }
    catch (Poco::Exception& exc) {
        std::cerr << "Error: " << exc.displayText() << std::endl;
        return {"", false};
    }

  

    // return false;
}

bool JWTMiddleware::verifyUser(string userID){
    json connectionData = getParamValue("auth_db");
    string authTable = connectionData["table"];
    string usernameIDField = connectionData["username_id_field"]; 

    string query = "select * from "+authTable+" where "+usernameIDField+" = "+userID;
    PSQLQuery * psqlQuery= new PSQLQuery(connection,query);
    if ( psqlQuery->hasResults() && psqlQuery->fetchNextRow())
    {
        cout << "username: " << psqlQuery->getValue("username") << endl;
        cout << "phone_number: " << psqlQuery->getValue("phone_number") << endl;
        delete (psqlQuery);
        return true;
    }
    else
    {
        cout << "Error cannot find authentication user" << endl;
        delete (psqlQuery);
        return false;
    }
}

void JWTMiddleware::injectUserData(HTTPRequest* _req, map<string, string> data)
{
    _req->addContext("JWT", data);
}

JWTMiddleware::~JWTMiddleware()
{
}

extern "C" Middleware *create_object() // extern "c" not garbling function names
{
    return new JWTMiddleware();
}
