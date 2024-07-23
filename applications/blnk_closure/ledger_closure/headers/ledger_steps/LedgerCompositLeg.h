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
        pair <ledger_amount_primitive_orm*,ledger_amount_primitive_orm*> * leg;
        int bond_id; 
    public:
        LedgerCompositLeg();
        int getLegId();
        void setAmount (float _amount);
        pair <ledger_amount_primitive_orm*,ledger_amount_primitive_orm*>* build(TemplateLeg * _template,  LedgerAmount * _ledger_amount,ledger_entry_primitive_orm * entry, int _cache_partition_number=-1);
        void buildLeg(TemplateLeg * _template,  LedgerAmount * _ledger_amount, ledger_amount_primitive_orm * _leg_side, bool _is_debit);
        void validateEntry(TemplateLeg * _template,  LedgerAmount * _ledger_amount); 
        int getBondId(int _installment_id);
        pair <ledger_amount_primitive_orm *,ledger_amount_primitive_orm *> * getLedgerCompositeLeg ();
        ~LedgerCompositLeg();

};

#endif