#include <PSQLInitMiddleware.h>
#include <PSQLController.h>


PSQLInitMiddleware::PSQLInitMiddleware() : Middleware("PSQLINIT")
{
}

bool PSQLInitMiddleware::run(HTTPRequest *_req, HTTPResponse *_res)
{
    return false;
}
Middleware *PSQLInitMiddleware::clone()
{
    // Middleware * jwtMiddleware =  new JWTMiddleware();
    // jwtMiddleware->init(this->getParams());
    // return jwtMiddleware;
    return  NULL;
}

void PSQLInitMiddleware::setupDataSources()
{
    json dataSourceData = getParamValue("data_sources"); 
     for (auto datasource : dataSourceData) {
        cout << "Creating DataSource: " << datasource["name"] << endl;
        bool connect = psqlController.addDataSource(datasource["name"],datasource["hostname"],datasource["port"],datasource["database"],datasource["username"],datasource["password"]);

    // int port  = connectionData["port"];
    // string hostname = connectionData["hostname"];
    // string username = connectionData["username"];
    // string password = connectionData["password"];
    // string database = connectionData["database"];
    // connection = new PSQLConnection(hostname,port,database,username,password);
    }
}

void PSQLInitMiddleware::init(json initData)
{
    setParams(initData);
    setupDataSources();
}

PSQLInitMiddleware::~PSQLInitMiddleware()
{
}

extern "C" Middleware *create_object(PSQLControllerMaster * psqlControllerMaster) // extern "c" not garbling function names
{
    psqlController.initialize(psqlControllerMaster);
    return new PSQLInitMiddleware();
}
