#include <ThorSerialize/Traits.h>
#include <ThorSerialize/SerUtil.h>
#include <ThorSerialize/JsonThor.h>
#include <fstream>
#include <iostream>
using namespace std;

class TransactionObject
{
    public:
    int         payment_id;
    std::string payment_date;
    std::string url;
    std::string title;
    int         sell_id;
    std::string last_update;
    int         inventory_id;
    std::string amount;
    int         item_id;
    int         buyer_id;
};

ThorsAnvil_MakeTrait(TransactionObject, payment_id, payment_date, url, title, sell_id, last_update, inventory_id, amount, item_id, buyer_id);

class TransactionLog
{
    public:
        std::vector<TransactionObject>  transaction;
};

ThorsAnvil_MakeTrait(TransactionLog, transaction);

class Transactions
{
    public:
        TransactionLog transactions;
};

ThorsAnvil_MakeTrait(Transactions, transactions);

int main()
{
    std::ifstream   file("/Users/kmsobh/blnk/orm_c++/data.json");
    Transactions    jsonData;
    file >> ThorsAnvil::Serialize::jsonImporter(jsonData);

    cout << jsonData.transactions.transaction[0].payment_id << "\n";
    jsonData.transactions.transaction[1].payment_id = 100;
    std::ostringstream ss;
    ss << ThorsAnvil::Serialize::jsonExporter(jsonData);
    string s = ss.str();
    cout << s << endl;
}
