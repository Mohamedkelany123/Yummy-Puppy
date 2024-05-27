#include <LedgerClosureService.h>

LedgerClosureService::LedgerClosureService(LedgerClosureStep * _ledgerClosureStep)
{
    la = new map <string,LedgerAmount *>();
    ledgerClosureStep = _ledgerClosureStep;
}

void LedgerClosureService::addHandler (string legName, funcPtr func)
{
    funcMap[legName] = func;
}

map <string,LedgerAmount*> * LedgerClosureService::inference ()
{

    for (auto f : funcMap)
    {
        cout << "FUNCMAP NAME:" << f.first << endl;
        LedgerAmount * temp = (f.second)(ledgerClosureStep);
        if(temp->getAmount() != 0)
            (*la)[f.first] = temp;
    }
    return la;
}

LedgerClosureService::~LedgerClosureService()
{
    for (auto l : (*la))
        delete (l.second);

    delete (la);
}