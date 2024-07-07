#include <IScoreNidInquiry.h>


IScoreNidInquiry::IScoreNidInquiry(map<string,PSQLAbstractORM *> * _orms, float _inquiry_fee)
{
    nid_orm = ORM(ekyc_app_nidlog,_orms);
    onb_orm = ORM(ekyc_app_onboardingsession,_orms);
    cout << "MERCHANT STAFF ID IS : ";
    cout << onb_orm->get_merchant_staff_id() << endl;
    PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,_orms);
    if(gorm->get("merchant_id") == ""){
        cout << "MERCHANT ID 2 IS :";

        merchantID =gorm->toInt("merchant_id2");
        cout << merchantID << endl;
    }
    else{
        merchantID =gorm->toInt("merchant_id");
        cout << gorm->toInt("merchant_id") << endl;
    }
    
    std::stringstream ss2(gorm->get("customer_id"));
    cout << "CUSTOMER ID IS :";
    cout << gorm->get("customer_id") << endl;
    if (gorm->get("customer_id") == ""){
        customerID = NULL;
    }
    ss2 >> customerID;
    cout << "Stored customer id is";
    cout << customerID;

    inquiryFee = _inquiry_fee;
}

PSQLJoinQueryIterator* IScoreNidInquiry::aggregator(string _closure_date_string){
    PSQLJoinQueryIterator * nidLogsQuery = new PSQLJoinQueryIterator("main",
    {new ekyc_app_nidlog_primitive_orm("main"), new ekyc_app_onboardingsession_primitive_orm("main")},
    {{{"ekyc_app_nidlog","onboarding_session_id"},{"ekyc_app_onboardingsession","id"}}}
    );

    nidLogsQuery->filter(
        ANDOperator(
            new UnaryOperator("ekyc_app_nidlog.id",in,"109005,  \
109006, \
109007, \
109008, \
109009, \
109010, \
109011, \
109013, \
109014, \
109015, \
109016, \
109017, \
109018, \
109019, \
109020, \
109021, \
109022, \
109023, \
109024, \
109025, \
109026, \
109027, \
109028, \
109029, \
109030, \
109033, \
109034, \
109035, \
109036, \
109037, \
109038, \
109039, \
109040, \
109041, \
109042, \
109043, \
109044, \
109045, \
109046, \
109047, \
109048, \
109049, \
109050, \
109051, \
109052, \
109053, \
109054, \
109055, \
109056, \
109057, \
109058, \
109059, \
109060, \
109061, \
109062, \
109063, \
109064, \
109065, \
109066, \
109067, \
109068, \
109069, \
109070, \
109071, \
109072, \
109073, \
109074, \
109075, \
109076, \
109077, \
109078, \
109079, \
109080, \
109081, \
109082, \
109083, \
109084, \
109085, \
109086, \
109087, \
109088, \
109089, \
109090, \
109091, \
109092, \
109093, \
109094, \
109095, \
109096, \
109097, \
109098, \
109099, \
109100, \
109101, \
109102, \
109103, \
109104, \
109105, \
109106, \
109107, \
109108, \
109109, \
109110, \
109111, \
109112, \
109113, \
109114, \
109115, \
109116, \
109117, \
109118, \
109119, \
109120, \
109121, \
109122, \
109124, \
109127, \
109128, \
109129, \
109131, \
109132, \
109133, \
109134, \
109135, \
109136, \
109138, \
109139, \
109140, \
109141, \
109142, \
109143, \
109144, \
109145, \
109146, \
109147, \
109148, \
109149, \
109150, \
109151, \
109152, \
109153, \
109154, \
109155, \
109156, \
109157, \
109158, \
109159, \
109160, \
109161, \
109162, \
109163, \
109164, \
109165, \
109166, \
109167, \
109168, \
109169, \
109170, \
109171, \
109172, \
109173, \
109174, \
109175, \
109176, \
109177, \
109178, \
109179, \
109180, \
109181, \
109182, \
109183, \
109184, \
109185, \
109186, \
109187, \
109188, \
109189, \
109190, \
109191, \
109192, \
109193, \
109194, \
109195, \
109196, \
109198, \
109199, \
109200, \
109201, \
109202, \
109203, \
109204, \
109205, \
109206, \
109207, \
109208, \
109209, \
109210, \
109211, \
109212, \
109213, \
109214, \
109215, \
109216, \
109217, \
109218, \
109219, \
109220, \
109221, \
109222, \
109223, \
109224, \
109226, \
109227, \
109228, \
109229, \
109230, \
109231, \
109232, \
109233, \
109234, \
109235, \
109236, \
109237, \
109238, \
109239, \
109240, \
109241, \
109242, \
109243, \
109244, \
109245, \
109246, \
109247, \
109248, \
109249, \
109250, \
109251, \
109252, \
109253, \
109254, \
109255, \
109256, \
109257, \
109258, \
109259, \
109260, \
109261, \
109262, \
109263, \
109264, \
109265, \
109266, \
109267, \
109268, \
109269, \
109270, \
109271, \
109272, \
109273, \
109274, \
109275, \
109276, \
109277, \
109278, \
109279, \
109280, \
109281, \
109282, \
109283, \
109284, \
109285, \
109286, \
109287, \
109288, \
109289, \
109290, \
109291, \
109292, \
109293, \
109294, \
109295, \
109296, \
109297, \
109298, \
109299, \
109300, \
109301, \
109302, \
109303, \
109304, \
109305, \
109306, \
109307, \
109308, \
109309, \
109310, \
109311, \
109312, \
109313, \
109314, \
109315, \
109316, \
109317, \
109318, \
109319, \
109320, \
109321, \
109322, \
109323, \
109324, \
109325, \
109326, \
109327, \
109328, \
109329, \
109330, \
109331, \
109332, \
109333, \
109334, \
109335, \
109336, \
109337, \
109338, \
109339, \
109340, \
109341, \
109342, \
109343, \
109344, \
109345, \
109346, \
109347, \
109348, \
109349, \
109350, \
109351, \
109352, \
109353, \
109354, \
109355, \
109356, \
109357, \
109358, \
109359, \
109360, \
109361, \
109362, \
109363, \
109364, \
109365, \
109366, \
109367, \
109368, \
109369, \
109370, \
109371, \
109372, \
109373, \
109374, \
109375, \
109376, \
109377, \
109378, \
109379, \
109380, \
109381, \
109382, \
109383, \
109384, \
109385, \
109386, \
109387, \
109388, \
109389, \
109390, \
109391, \
109392, \
109393, \
109394, \
109395, \
109396, \
109397, \
109398, \
109399, \
109400, \
109401, \
109402, \
109403, \
109404, \
109405, \
109406, \
109407, \
109408, \
109409, \
109410, \
109411, \
109412, \
109413, \
109414, \
109415, \
109416, \
109417, \
109418, \
109419, \
109420, \
109421, \
109422, \
109423, \
109424, \
109425, \
109426, \
109427, \
109428, \
109429, \
109430, \
109431, \
109432, \
109433, \
109434, \
109436, \
109437, \
109438, \
109439, \
109440, \
109441, \
109442, \
109443, \
109444, \
109445, \
109446, \
109447, \
109448, \
109449, \
109450, \
109451, \
109452, \
109453, \
109454, \
109455, \
109456, \
109457, \
109458, \
109459, \
109460, \
109461, \
109462, \
109463, \
109464, \
109465, \
109466, \
109467, \
109468, \
109469, \
109470, \
109471, \
109472, \
109473, \
109474, \
109475, \
109476, \
109477, \
109478, \
109479, \
109480, \
109481, \
109482, \
109483, \
109484, \
109485, \
109486, \
109487, \
109488, \
109489, \
109490, \
109491, \
109492, \
109493, \
109494, \
109495, \
109496, \
109497, \
109498, \
109499, \
109500, \
109503, \
109504, \
109505, \
109506, \
109507, \
109508, \
109509, \
109510, \
109511, \
109512, \
109513, \
109514, \
109515, \
109516"),
            new UnaryOperator("ekyc_app_nidlog.status",eq,1),
            new UnaryOperator("ekyc_app_nidlog.nid_expense_ledger_entry_id",isnull,"",true),
            new UnaryOperator("ekyc_app_nidlog.created_at",lte,_closure_date_string)
            // new UnaryOperator("loan_app_loan.closure_status",eq,ledger_status::NID_ISCORE-1)
        )
    );
    string query_closure_date = "'" + _closure_date_string + "'"; 

    nidLogsQuery->addExtraFromField("(select merchant_id from crm_app_merchantstaffhistory cam where cam.staff_id=ekyc_app_onboardingsession.merchant_staff_id and cam.created_at <= " + query_closure_date + " order by id desc limit 1)","merchant_id");
    nidLogsQuery->addExtraFromField("(select id from crm_app_customer cac where cac.phone_number = ekyc_app_onboardingsession.phone_number limit 1)","customer_id");
    nidLogsQuery->addExtraFromField("(select merchant_id from crm_app_merchantstaff cam where ekyc_app_onboardingsession.merchant_staff_id = cam.user_ptr_id limit 1)","merchant_id2");
    return nidLogsQuery;
}

LedgerAmount* IScoreNidInquiry::_init_ledger_amount(){
    LedgerAmount * lg = new LedgerAmount();
    lg->setMerchantId(merchantID);
    lg->setCashierId(onb_orm->get_merchant_staff_id());
    if(customerID != NULL){
        lg->setCustomerId(customerID);
    }

    return lg;
}

void IScoreNidInquiry::update_step(){
    PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator(
            new UnaryOperator ("loan_app_loan.id",ne,"14312"),
            //TODO: Change To update status comparing to the closure status of the step before it not gt 0
            new UnaryOperator ("loan_app_loan.closure_status",gte,0)
        ),
        {{"closure_status",to_string(ledger_status::NID_ISCORE)}}
        );
        psqlUpdateQuery.update();  
}

void IScoreNidInquiry::setupLedgerClosureService(LedgerClosureService * ledgerClosureService)
{
    ledgerClosureService->addHandler("iScore NID inquiry fee expense",_calculate_inquiry_fee);
}

void IScoreNidInquiry::stampORMs(ledger_entry_primitive_orm * entry){
    // map<string,LedgerCompositLeg*>::iterator it = leg_amounts->begin();
    // ledger_amount_primitive_orm* first_leg_amount = it->second->getLedgerCompositeLeg()->first;
    // if (first_leg_amount != NULL){
    nid_orm->setUpdateRefernce("nid_expense_ledger_entry_id",entry);
    // }
    // else cout << "ERROR in fetching first leg of the entry " << endl;
}

float IScoreNidInquiry::get_inquiry_fee(){
    return inquiryFee;
}

LedgerAmount * IScoreNidInquiry::_calculate_inquiry_fee(LedgerClosureStep *iScoreNidInquiry){
    LedgerAmount * la = ((IScoreNidInquiry*)iScoreNidInquiry)->_init_ledger_amount();
    la->setAmount(((IScoreNidInquiry*)iScoreNidInquiry)->get_inquiry_fee());
    cout << la << endl;

    return la;
}

IScoreNidInquiry::~IScoreNidInquiry(){}



