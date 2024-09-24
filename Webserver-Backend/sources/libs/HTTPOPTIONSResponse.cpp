#include "HTTPOPTIONSResponse.h"

// Constructor: Calling the base class constructor
HTTPOPTIONSResponse::HTTPOPTIONSResponse(TCPSocket * _tcpSocket) : HTTPResponse(_tcpSocket){
    this->setHeaderValue("Access-Control-Allow-Origin", "*");
    this->setHeaderValue("Access-Control-Allow-Methods", "*");
    this->setHeaderValue("Access-control-allow-credentials", "true");
    this->setHeaderValue("Access-Control-Allow-Headers", "Content-Type");
}

HTTPOPTIONSResponse::~HTTPOPTIONSResponse() // Destructor
{
}
