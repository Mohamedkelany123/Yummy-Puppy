#include <EndpointClient.h>
#include <ThorSerialize/Traits.h>
#include <ThorSerialize/SerUtil.h>
#include <ThorSerialize/JsonThor.h>
#include <EndpointService.h>
class AbstractSerializer
{
    private:
    public:
        AbstractSerializer(){}
        virtual void serialize (string ss) = 0;
        virtual string deserialize () = 0;
        virtual ~AbstractSerializer(){}
};

class KeyValue: public AbstractSerializer
{
    private:
        string key;
        string value;    
        friend struct ThorsAnvil::Serialize::Traits<KeyValue>;
    public:
        KeyValue(): AbstractSerializer(){}
        void setKey (string _key){key=_key;}
        string getKey () {return key;} 
        void setValue (string _value){value=_value;}
        string getValue () {return value;} 
        void serialize (string ss) {}
        string deserialize () {return "";}
        ~KeyValue(){}
};

ThorsAnvil_MakeTrait(KeyValue, key, value);

class POSTFawryInquiryInput: public AbstractSerializer
{
    private:
        string msgCode;
        string sender;
        string receiver;
        string custLang;
        // string ClientDt          types.BlnkTime `json:"clientDt,omitempty" valid:"Required"`
        string clientDt;
        string rqUID;
        string asyncRqUID;
        string terminalID;
        string billingAcct;
        string bankID;
        string billTypeCode;
        string deliveryMethod;
        string signature;
        vector<KeyValue>  extraBillingAccts;
        vector<KeyValue>  customProperties;
    public:
        POSTFawryInquiryInput() : AbstractSerializer(){}
        void serialize (string ss) {}
        string deserialize () {return "";}
        string getMSGCode () {return msgCode;}
        vector<KeyValue> & getCustomProperties() {return customProperties;}
        ~POSTFawryInquiryInput(){} 


        friend struct ThorsAnvil::Serialize::Traits<POSTFawryInquiryInput>;
};

ThorsAnvil_MakeTrait(POSTFawryInquiryInput, msgCode, sender, receiver, custLang, clientDt, rqUID, asyncRqUID, terminalID, billingAcct, bankID, billTypeCode, deliveryMethod, signature, extraBillingAccts,  customProperties);


#define   SERIALIZER_TEMPLATE     void serialize (string str) { \
            std::stringstream json_string (str); \
            json_string >> ThorsAnvil::Serialize::jsonImporter((*this)); \
        }

#define   DESERIALIZER_TEMPLATE     string deserialize () { \
            std::ostringstream ss; \
            ss << ThorsAnvil::Serialize::jsonExporter((*this)); \
            return ss.str(); \
        }


class FawryInquiry: public AbstractSerializer
{
    private:
        POSTFawryInquiryInput postFawryInquiryInput;
    public:
        FawryInquiry() : AbstractSerializer(){}
        SERIALIZER_TEMPLATE
        DESERIALIZER_TEMPLATE
        POSTFawryInquiryInput & getPOSTFawryInquiryInput() {return postFawryInquiryInput;} 
        ~FawryInquiry(){} 
    friend struct ThorsAnvil::Serialize::Traits<FawryInquiry>;


};

ThorsAnvil_MakeTrait(FawryInquiry,postFawryInquiryInput);



class POSTFawryInquiryOutput: public AbstractSerializer
{
    private:
        string msgCode;
    public:
        POSTFawryInquiryOutput() : AbstractSerializer(){}
        void serialize (string ss) {}
        string deserialize () {return "";}
        string getMSGCode () {return msgCode;}
        void setMSGCode (string _msgCode) {msgCode=_msgCode;}
        ~POSTFawryInquiryOutput(){} 


        friend struct ThorsAnvil::Serialize::Traits<POSTFawryInquiryOutput>;
};

ThorsAnvil_MakeTrait(POSTFawryInquiryOutput, msgCode);


class FawryInquiryResponse: public AbstractSerializer
{
    private:
        POSTFawryInquiryOutput postFawryInquiryOutput;
    public:
        FawryInquiryResponse() : AbstractSerializer(){}
        SERIALIZER_TEMPLATE
        DESERIALIZER_TEMPLATE
        POSTFawryInquiryOutput & getPOSTFawryInquiryOutput() {return postFawryInquiryOutput;} 
        ~FawryInquiryResponse(){} 
    friend struct ThorsAnvil::Serialize::Traits<FawryInquiryResponse>;

};

ThorsAnvil_MakeTrait(FawryInquiryResponse,postFawryInquiryOutput);


int main (int argc,char ** argv)
{

    string http_body = R""""(
{
    "postFawryInquiryInput":{
        "msgCode":"AHMED",
        "sender":"AHMED",
        "receiver":"AHMED",
        "custLang":"AHMED",
        "clientDt":"2022-01-15T13:24:11.329471",
        "rqUID":"AHMED",
        "asyncRqUID":"AHMED",
        "terminalId":"AHMED",
        "billingAcct":"01155689007",
        "bankId":"AHMED",
        "billTypeCode":"76586",
        "deliveryMethod":"AHMED",
        "signature":"JTYDWQR0mPY9c6FwOq7/Kh1WPbOdpxQlALw5fPzl8Mk=",
        "extraBillingAccts":[],
        "customProperties":[
            {
                "key": "PosSerialNumber",
                "value": "1170648539"
            },
            {
                "key": "BCR_VERSION",
                "value": "1.7.119.146"
            },
            {
                "key": "KeyToken",
                "value": "fdb3bf60c8ce4c4a"
            },
            {
                "key": "GWTK",
                "value": "ocY5ucFwh5qSXUA32xWlYBxCM6bGmKPybNMqU39nbFGV4JMFxiMYyr5kTDeZNyiZ"
            }
        ]
    }
}
    )"""";

    // FawryInquiry * fawryInquiry  = new FawryInquiry();
    // fawryInquiry->serialize(http_body);
    EndpointClient <FawryInquiry,FawryInquiryResponse> * endpointClient =new EndpointClient <FawryInquiry,FawryInquiryResponse> ();
    endpointClient->setInputSerializer(http_body);
    FawryInquiryResponse * fawryInquiryResponse = endpointClient->fetch("http://localhost:8080/testing");
    cout << fawryInquiryResponse->deserialize()<< endl;
    delete (endpointClient);
    // delete (fawryInquiry);


    return 0;
}