#include <HTTPResponse.h>


HTTPResponse::HTTPResponse(HTTPResponseHeader * _httpResponseHeader)
{
    httpResponseHeader = _httpResponseHeader;
}
void HTTPResponse::setBody(string _body)
{
    body = _body;
}
void HTTPResponse::setBody(json _body)
{
    body = _body.dump(4);
}
HTTPResponse::~HTTPResponse()
{


}
