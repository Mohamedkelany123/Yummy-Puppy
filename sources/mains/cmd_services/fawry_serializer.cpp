#include <ThorSerialize/Traits.h>
#include <ThorSerialize/SerUtil.h>
#include <ThorSerialize/JsonThor.h>
#include <fstream>
#include <iostream>
using namespace std;


class KeyValue
{
    private:
        string key;
        string value;    
        friend struct ThorsAnvil::Serialize::Traits<KeyValue>;
    public:
        KeyValue(){}
        void setKey (string _key){key=_key;}
        string getKey () {return key;} 
        void setValue (string _value){value=_value;}
        string getValue () {return value;} 
        ~KeyValue(){}
};

ThorsAnvil_MakeTrait(KeyValue, key, value);

class POSTFawryInquiryInput
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
        
        friend struct ThorsAnvil::Serialize::Traits<POSTFawryInquiryInput>;
};

ThorsAnvil_MakeTrait(POSTFawryInquiryInput, msgCode, sender, receiver, custLang, clientDt, rqUID, asyncRqUID, terminalID, billingAcct, bankID, billTypeCode, deliveryMethod, signature, extraBillingAccts,  customProperties);


class FawryInquiry
{
    public:
        POSTFawryInquiryInput postFawryInquiryInput;
};

ThorsAnvil_MakeTrait(FawryInquiry,postFawryInquiryInput);

int main()
{
    std::ifstream   file("/Users/kmsobh/blnk/orm_c++/fawry.json");
    FawryInquiry    jsonData;
    
    file >> ThorsAnvil::Serialize::jsonImporter(jsonData);

    cout << jsonData.postFawryInquiryInput.msgCode << "\n";
    cout << jsonData.postFawryInquiryInput.customProperties[0].getKey() << "\n";
    cout << jsonData.postFawryInquiryInput.customProperties[0].getValue() << "\n";
    jsonData.postFawryInquiryInput.customProperties[0].setValue("01001091779");
    // jsonData.transactions.transaction[1].payment_id = 100;
    std::ostringstream ss;
    ss << ThorsAnvil::Serialize::jsonExporter(jsonData);
    string s = ss.str();
    cout << s << endl;
}
