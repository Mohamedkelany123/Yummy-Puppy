#include <LedgerClosureService.h>

LedgerClosureService::LedgerClosureService(LedgerClosureStep * _ledgerClosureStep)
{
    ledgerClosureStep = _ledgerClosureStep;
}

void LedgerClosureService::addHandler (string legName, funcPtr func)
{
    funcMap[legName] = func;
}

map <string,LedgerAmount> LedgerClosureService::inference ()
{
    map <string,LedgerAmount> la ;
    for (auto f : funcMap)
    {
        la[f.first] = (f.second)(ledgerClosureStep);
    }
    return la;
}