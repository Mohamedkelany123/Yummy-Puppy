#include <JWTMiddleware.h>
#include <ostream>
#include <PSQLConnection.h>
#include <PSQLQuery.h>

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

        if ( strncasecmp(jwt.c_str(),"Bearer",strlen("Bearer")) ==0 )
        {
            const char * s = jwt.c_str() + strlen("Bearer")+1;
            jwt = s;

        }
        cout << jwt << endl;


        // Poco::JWT::Token token;
        // token.setType("JWT");
        // token.setSubject("1234567890");
        // token.payload().set("name", std::string("John Doe"));
        // token.setIssuedAt(Poco::Timestamp());


        string jwt_key = "wnia2ie9-(c2_)4g%ck%bw6lyfjtdgf@imcg*xe*n!uo%1^&0%";
        Poco::JWT::Signer signer(jwt_key);
        Token token = signer.verify(jwt);
        //std::ostream out;
        std::stringstream out;
        token.payload().stringify(out);
        std::ostringstream ss;
        ss << out.rdbuf();
        cout << "Token: "<< ss.str() << endl;
        string user_id =  token.payload().get("user_id");
        cout << "user_id: "<< user_id << endl;
        string hostname = "192.168.65.216";
        int port  = 5432;
        string username = "development";
        string password = "5k6MLFM9CLN3bD1";
        string auth_table = "auth_app_user";
        string database = "django_ostaz_14082024_ml";
        string username_id_field = "id";
        PSQLConnection * psqlConnection = new PSQLConnection(hostname,port,database,username,password);
        string query = "select * from "+auth_table+" where "+username_id_field+" = "+user_id;
        PSQLQuery * psqlQuery= new PSQLQuery(psqlConnection,query);
        if ( psqlQuery->hasResults() && psqlQuery->fetchNextRow())
        {
            cout << "username: " << psqlQuery->getValue("username") << endl;;
            cout << "phone_number: " << psqlQuery->getValue("phone_number") << endl;;
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
        delete (psqlConnection);
    }
    else 
    {

    }

    cout << "This is JWTMiddleware::run()" << endl;
    return true;
}
Middleware *JWTMiddleware::clone()
{
    return (Middleware *)new JWTMiddleware();
}

JWTMiddleware::~JWTMiddleware()
{
}

extern "C" Middleware *create_object() // extern "c" not garbling function names
{
    return new JWTMiddleware();
}
