#include <IScoreNidInquiry.h>


IScoreNidInquiry::IScoreNidInquiry(ekyc_app_nidlog_primitive_orm * _orms, float _inquiry_fee)
{
    nid_orm = _orms;
    inquiry_fee = _inquiry_fee;
}

ekyc_app_nidlog_primitive_orm_iterator* IScoreNidInquiry(string _closure_date_string){
    ekyc_app_nidlog_primitive_orm_iterator * nid_logs = new ekyc_app_nidlog_primitive_orm_iterator("main");

    nid_logs->filter(
        ANDOperator(
            new UnaryOperator("ekyc_app_nidlog.status",eq,1),
            new UnaryOperator("ekyc_app_nidlog.nid_expense_ledger_entry",isnull,"",true),
            new UnaryOperator("ekyc_app_nidlog.created_at",lte,_closure_date_string)
        )
    );

    return nid_logs;
}