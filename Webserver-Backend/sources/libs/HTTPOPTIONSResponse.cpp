#include "HTTPOPTIONSResponse.h"

// Constructor: Calling the base class constructor
HTTPOPTIONSResponse::HTTPOPTIONSResponse(TCPSocket * _tcpSocket) : HTTPResponse(_tcpSocket){
    this->setHeaderValue("Access-Control-Allow-Origin", "*");
    this->setHeaderValue("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    this->setHeaderValue("Access-Control-Allow-Headers", "Content-Type");
}

HTTPOPTIONSResponse::~HTTPOPTIONSResponse() // Destructor
{
}
