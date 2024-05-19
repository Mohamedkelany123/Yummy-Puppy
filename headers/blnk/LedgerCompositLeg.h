#ifndef LEDGER_COMPOSIT_LEG_H
#define LEDGER_COMPOSIT_LEG_H

#include <common.h>
#include <common_orm.h>
#include<LedgerAmount.h>
#include<TemplateLeg.h>



class LedgerCompositLeg
{
    private:
        int leg_id;
        std::pair <ledger_amount_primitive_orm*,ledger_amount_primitive_orm*> leg;
        int bond_id; 
    public:
        LedgerCompositLeg(){}
        void setAmount (float _amount);
        std::pair <ledger_amount_primitive_orm*,ledger_amount_primitive_orm*>* build(TemplateLeg * _template,  LedgerAmount * _ledger_amount,ledger_entry_primitive_orm * entry);
        void buildLeg(TemplateLeg * _template,  LedgerAmount * _ledger_amount, ledger_amount_primitive_orm * _leg_side, bool _is_debit);
        void validateEntry(TemplateLeg * _template,  LedgerAmount * _ledger_amount); 
        int getBondId(int _installment_id);

        std::pair <ledger_amount_primitive_orm *,ledger_amount_primitive_orm *> getLedgerCompositeLeg ();
        ~LedgerCompositLeg(){}

};

#endif