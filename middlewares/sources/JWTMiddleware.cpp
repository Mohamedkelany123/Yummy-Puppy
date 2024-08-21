#include <JWTMiddleware.h>
#include <ostream>


JWTMiddleware::JWTMiddleware() : Middleware("JWT")
{
    connectDatabase();
}


bool JWTMiddleware::run(HTTPRequest *_req, HTTPResponse *_res)
{
    if (_req != nullptr)
    {
        string str = _req->getHeaderValue("Accept");
        string jwt = _req->getHeaderValue("Authorization");
        cout << jwt << endl;

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
        else
        {
            cout << "Error cannot find authentication user" << endl;;
        }
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

void JWTMiddleware::connectDatabase()
{
    string hostname = getParamValue("hostname");
    int port  = getParamValue("port");
    string username = getParamValue("username");
    string password = getParamValue("password");
    string database = getParamValue("database");
    connection = new PSQLConnection(hostname,port,database,username,password);
}

PSQLConnection * JWTMiddleware::getDatabaseConnection()
{
    if (!connection->isAlive())
        cout << "Database connection is not alive" << endl;
    return connection;
}


bool JWTMiddleware::verifyToken(string authToken)
{

    try {
        std::string tokenString = "your.jwt.token.here";
        std::string secretKey = "your-secret-key";

        // Parse the JWT token
        Poco::JWT::Token token;
        Poco::JWT::Token::parse(tokenString, token);

        // Create a Signer (for HS256 in this case)
        Poco::JWT::Signer::Ptr pSigner = Poco::JWT::SignerFactory::createSigner(Poco::JWT::SignerFactory::HS256, secretKey);

        // Create a JWTValidator
        Poco::JWT::JWTValidator validator;

        // Verify the token using the Signer
        if (validator.verify(pSigner, token)) {
            std::cout << "JWT is valid!" << std::endl;
        } else {
            std::cout << "JWT is invalid!" << std::endl;
        }
    }
    catch (Poco::Exception& exc) {
        std::cerr << "Error: " << exc.displayText() << std::endl;
    }



    string authTable = getParamValue("auth_table");
    string usernameIDField = getParamValue("username_id_field");   

    return false;
}

void JWTMiddleware::injectUserData()
{
}

JWTMiddleware::~JWTMiddleware()
{
}

extern "C" Middleware *create_object() // extern "c" not garbling function names
{
    return new JWTMiddleware();
}
