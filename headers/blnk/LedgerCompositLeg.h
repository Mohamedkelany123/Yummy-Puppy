#include <common.h>
#include<LedgerAmount.h>
#include<ledger_amount_primitive_orm.h>


class LedgerCompositLeg
{
    private:
        int leg_id;
        std::pair <ledger_amount_primitive_orm*,ledger_amount_primitive_orm*> leg;
    public:
        LedgerCompositLeg(){}
        void setAmount (float _amount);
        bool build(TemplateLeg * _template, LedgerAmount * _ledger_amount);
        std::pair <ledger_amount_primitive_orm *,ledger_amount_primitive_orm *> getLedgerCompositeLeg ();
        ~LedgerCompositLeg(){}

};