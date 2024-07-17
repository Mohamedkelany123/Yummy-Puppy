#include "UpdatingProvisions.h"

PSQLJoinQueryIterator* UpdatingProvisions::aggregator(string _closure_date_string,string start_fiscal_year,string end_fiscal_year,string end_date){
    string query_end_date = end_fiscal_year; 
    if (end_date != ""){
        query_end_date = end_date;
    }
    string start_date_string = "'" + start_fiscal_year + "'";
    string end_date_string = "'" + end_date + "'";
    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {   
            new tms_app_bond_primitive_orm("main",false,true,-1,{"fundingfacility_ptr_id"}),
            new tms_app_loaninstallmentfundingrequest_primitive_orm("main",false,true,-1,{"installment_id","loan_id","funding_facility_id"}),
            new loan_app_installment_primitive_orm("main",false,true,-1,{"id","loan_id"}),
            new loan_app_loan_primitive_orm("main",false,true,-1,{"id","customer_id","merchant_id"}),
        },
        {
            {{{"tms_app_bond","fundingfacility_ptr_id"},{"tms_app_loaninstallmentfundingrequest","funding_facility_id"}},JOIN_TYPE::inner},
            {{{"tms_app_loaninstallmentfundingrequest","installment_id"},{"loan_app_installment","id"}},JOIN_TYPE::right},
            {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},JOIN_TYPE::inner}
        });

    psqlQueryJoin->filter(
        ANDOperator 
        (
            // new UnaryOperator("loan_app_loan.id",eq,30),
            new UnaryOperator("loan_app_loan.last_closing_day",lte,_closure_date_string),
            new UnaryOperator("loan_app_loan.loan_booking_day",lte,_closure_date_string),
            new UnaryOperator("loan_app_loan.status_id",nin,"12,13")
            // new UnaryOperator("loan_app_loan.closure_status",eq,ledger_status::UPDATE_PROVISION-1)
        )
    );
    psqlQueryJoin->setAggregates({{"loan_app_installment", {"loan_id",1}}, {"tms_app_loaninstallmentfundingrequest", {"funding_facility_id",1}}});
    psqlQueryJoin->setOrderBy("loan_app_installment.id asc");

    psqlQueryJoin->addExtraFromField("(WITH parent_account AS (\
        SELECT id \
        FROM ledger_account \
        WHERE name = 'Loans receivable (gross)'\
    ),\
    child_accounts AS (\
        SELECT id \
        FROM ledger_account \ 
        WHERE parent_id = (SELECT id FROM parent_account) \
    )\
    SELECT SUM(amount) \
    FROM ledger_amount la inner join ledger_entry le on la.entry_id = le.id   \
    WHERE account_id IN ( \
        SELECT id \
        FROM child_accounts \
        UNION ALL \
        SELECT id \
        FROM parent_account \
    ) \
    AND loan_id = loan_app_installment.loan_id and \
    la.bond_id is null \
    and le.entry_date between " + start_date_string + " and " + end_date_string + ")","loans_rec_balance_on");

    psqlQueryJoin->addExtraFromField("(WITH parent_account AS (\
        SELECT id \
        FROM ledger_account \
        WHERE name = 'Loans receivable (gross)'\
    ),\
    child_accounts AS (\
        SELECT id \
        FROM ledger_account \ 
        WHERE parent_id = (SELECT id FROM parent_account) \
    )\
    SELECT SUM(amount) \
    FROM ledger_amount la inner join ledger_entry le on la.entry_id = le.id   \
    WHERE account_id IN ( \
        SELECT id \
        FROM child_accounts \
        UNION ALL \
        SELECT id \
        FROM parent_account \
    ) \
    AND loan_id = loan_app_installment.loan_id and \
    la.bond_id is not null and la.bond_id = tms_app_loaninstallmentfundingrequest.funding_facility_id \
    and le.entry_date between " + start_date_string + " and " + end_date_string + ")","loans_rec_balance_off");

    psqlQueryJoin->addExtraFromField("(WITH parent_account AS (\
        SELECT id \
        FROM ledger_account \
        WHERE name = 'Long-term loans receivable (net)'\
    ),\
    child_accounts AS (\
        SELECT id \
        FROM ledger_account \ 
        WHERE parent_id = (SELECT id FROM parent_account) \
    )\
    SELECT SUM(amount) \
    FROM ledger_amount la inner join ledger_entry le on la.entry_id = le.id   \
    WHERE account_id IN ( \
        SELECT id \
        FROM child_accounts \
        UNION ALL \
        SELECT id \
        FROM parent_account \
    ) \
    AND loan_id = loan_app_installment.loan_id and \
    la.bond_id is null \
    and le.entry_date between " + start_date_string + " and " + end_date_string + ")","long_term_balance_on");

    psqlQueryJoin->addExtraFromField("(WITH parent_account AS (\
        SELECT id \
        FROM ledger_account \
        WHERE name = 'Long-term loans receivable (net)'\
    ),\
    child_accounts AS (\
        SELECT id \
        FROM ledger_account \ 
        WHERE parent_id = (SELECT id FROM parent_account) \
    )\
    SELECT SUM(amount) \
    FROM ledger_amount la inner join ledger_entry le on la.entry_id = le.id   \
    WHERE account_id IN ( \
        SELECT id \
        FROM child_accounts \
        UNION ALL \
        SELECT id \
        FROM parent_account \
    ) \
    AND loan_id = loan_app_installment.loan_id and \
    la.bond_id = tms_app_loaninstallmentfundingrequest.funding_facility_id \
    and le.entry_date between " + start_date_string + " and " + end_date_string + ")","long_term_balance_off");

    psqlQueryJoin->addExtraFromField("(WITH parent_account AS (\
        SELECT id \
        FROM ledger_account \
        WHERE name = 'Impairment provisions, On-balance sheet'\
    ),\
    child_accounts AS (\
        SELECT id \
        FROM ledger_account \ 
        WHERE parent_id = (SELECT id FROM parent_account) \
    )\
    SELECT SUM(amount) \
    FROM ledger_amount la inner join ledger_entry le on la.entry_id = le.id   \
    WHERE account_id IN ( \
        SELECT id \
        FROM child_accounts \
        UNION ALL \
        SELECT id \
        FROM parent_account \
    ) \
    AND loan_id = loan_app_installment.loan_id and \
    la.bond_id is null \
    and le.entry_date between " + start_date_string + " and " + end_date_string + ")","impairment_provisions_balance_on");

    psqlQueryJoin->addExtraFromField("(WITH parent_account AS (\
        SELECT id \
        FROM ledger_account \
        WHERE name = 'Impairment provisions, On-balance sheet'\
    ),\
    child_accounts AS (\
        SELECT id \
        FROM ledger_account \ 
        WHERE parent_id = (SELECT id FROM parent_account) \
    )\
    SELECT SUM(amount) \
    FROM ledger_amount la inner join ledger_entry le on la.entry_id = le.id   \
    WHERE account_id IN ( \
        SELECT id \
        FROM child_accounts \
        UNION ALL \
        SELECT id \
        FROM parent_account \
    ) \
    AND loan_id = loan_app_installment.loan_id and \
    la.bond_id = tms_app_loaninstallmentfundingrequest.funding_facility_id \ 
    and le.entry_date between " + start_date_string + " and " + end_date_string + ")","impairment_provisions_balance_off");

    psqlQueryJoin->addExtraFromField("(select percentage from loan_app_provision lap where status_id = (select status_id from loan_app_loanstatushistroy lal where loan_id= loan_app_loan.id and day <= " + end_date_string + " and status_type = 1 order by id desc limit 1))","history_provision_percentage");
    psqlQueryJoin->addExtraFromField("(select percentage from loan_app_provision where status_id = loan_app_loan.fra_status_id)","loan_provision_percentage");
    // psqlJoinQuery->addExtraFromField("(select onbalance from tms_app_loaninstallmentfundingrequest tal where tal.id = tms_app_loaninstallmentfundingrequest.id)","on_balance");
    // psqlJoinQuery->addExtraFromField("(select funding_facility_id from tms_app_loaninstallmentfundingrequest tal2 where tal2.funding_facility_id=tms_app_loaninstallmentfundingrequest.funding_facility_id and tal2.loan_id = loan_app_loan.id limit 1)","funding_facility_id");
    // vector<pair<string, string>> distinct_map = {
    //     {"loan_app_loan", "id"},
    //     {"loan_app_loan","merchant_id"},
    //     {"loan_app_loan","customer_id"},
    //     {"tms_app_loaninstallmentfundingrequest" ,"funding_facility_id"},
    // };
    // psqlJoinQuery->setDistinct(distinct_map);

    // cout << psqlJoinQuery;

    return psqlQueryJoin;
}

loan_app_loan_primitive_orm_iterator* UpdatingProvisions::aggregator_onbalance(string _closure_date_string,string start_fiscal_year,string end_fiscal_year,string end_date){
    string query_end_date = end_fiscal_year; 
    if (end_date != ""){
        query_end_date = end_date;
    }
    string start_date_string = "'" + start_fiscal_year + "'";
    string end_date_string = "'" + end_date + "'";
    loan_app_loan_primitive_orm_iterator * psqlJoinQuery = new loan_app_loan_primitive_orm_iterator("main");

    psqlJoinQuery->filter(
        ANDOperator(
            new UnaryOperator("loan_app_loan.id",in,"82185,82188,82195,82196,82198,82210,82231,82234,82236,82239,82245,82265,82266,82267,82271,82279,82280,82284,82290,82292,82296,82302,82309,82313,82319,82322,82325,82328,82329,82337,82340,82343,82361,82365,82368,82369,82371,82372,82376,82378,82380,82386,82391,82395,82398,82402,82407,82411,82415,82418,82426,82429,82432,82437,82442,82446,82448,82449,82455,82458,82460,82461,82467,82470,82472,82473,82475,82479,82481,82482,82485,82486,82493,82495,82499,82511,82512,82513,82514,82516,82519,82523,82525,82527,82532,82534,82537,82538,82544,82546,82548,82555,82556,82570,82575,82577,82595,82597,82603,82606,82608,82610,82611,82613,82614,82616,82619,82621,82623,82624,82631,82640,82647,82651,82653,82656,82663,82671,82672,82678,82680,82685,82689,82690,82693,82696,82697,82701,82704,82708,82710,82712,82715,82716,82719,82722,82727,82728,82731,82737,82741,82742,82745,82755,82756,82759,82762,82763,82777,82778,82779,82780,82783,82790,82793,82795,82809,82811,82812,82813,82816,82817,82819,82822,82825,82827,82829,82835,82838,82839,82844,82845,82849,82850,82852,82855,82868,82877,82880,82882,82885,82890,82895,82898,82901,82902,82903,82906,82909,82916,82919,82922,82926,82930,82934,82938,82939,82941,82943,82948,82949,82952,82955,82958,82964,82967,82969,82980,82981,82986,82988,82989,82994,82997,83001,83009,83011,83013,83014,83018,83022,83023,83027,83028,83030,83032,83034,83035,83036,83038,83039,83045,83047,83048,83055,83061,83064,83073,83084,83087,83089,83090,83110,83114,83115,83121,83122,83123,83132,83135,83136,83140,83146,83155,83158,83163,83164,83166,83174,83175,83177,83178,83183,83184,83185,83186,83188,83192,83193,83201,83202,83205,83226,83230,83240,83242,83243,83245,83248,83257,83264,83266,83273,83274,83279,83285,83286,83288,83290,83291,83295,83297,83299,83306,83308,83309,83312,83314,83316,83318,83319,83324,83326,83329,83336,83338,83340,83342,83343,83361,83363,83377,83378,83379,83384,83386,83392,83393,83395,83398,83402,83405,83409,83415,83428,83431,83432,83433,83434,83440,83449,83459,83460,83464,83465,83467,83472,83474,83481,83484,83488,83496,83499,83500,83502,83503,83513,83517,83521,83522,83523,83524,83525,83544,83545,83550,83553,83557,83558,83559,83562,83563,83565,83567,83569,83577,83587,83589,83590,83600,83602,83605,83611,83619,83628,83629,83633,83636,83638,83640,83645,83646,83649,83650,83655,83668,83669,83677,83679,83680,83687,83691,83696,83697,83709,83713,83715,83719,83725,83726,83727,83729,83734,83738,83739,83746,83754,83759,83764,83770,83772,83773,83774,83779,83780,83781,83783,83784,83788,83789,83791,83795,83798,83803,83805,83806,83809,83812,83817,83824,83825,83827,83829,83833,83834,83835,83845,83847,83852,83853,83858,83859,83862,83865,83866,83872,83874,83885,83892,83898,83902,83915,83921,83922,83925,83927,83936,83940,83954,83955,83958,83959,83967,83970,83973,83975,83980,83982,83984,83985,83988,83989,83990,83991,83993,83995,83997,84001,84002,84004,84005,84011,84012,84013,84019,84020,84021,84023,84027,84029,84041,84044,84045,84050,84051,84052,84053,84057,84060,84062"),
            new UnaryOperator("loan_app_loan.last_closing_day",lt,_closure_date_string),
            new UnaryOperator("loan_app_loan.loan_booking_day",lte,_closure_date_string),
            new UnaryOperator("loan_app_loan.status_id",nin,"12,13")
            // new UnaryOperator("loan_app_loan.closure_status",eq,ledger_status::UPDATE_PROVISION-1)
        )
    );

    psqlJoinQuery->addExtraFromField("(WITH parent_account AS (\
        SELECT id \
        FROM ledger_account \
        WHERE name = 'Loans receivable (gross)'\
    ),\
    child_accounts AS (\
        SELECT id \
        FROM ledger_account \ 
        WHERE parent_id = (SELECT id FROM parent_account) \
    )\
    SELECT SUM(CASE WHEN lac.type='AS' OR lac.type='EX' THEN -1*amount_local ELSE amount_local END) \
    FROM ledger_amount la inner join ledger_entry le on la.entry_id = le.id inner join ledger_account lac on la.account_id =  lac.id  \
    WHERE account_id IN ( \
        SELECT id \
        FROM child_accounts \
        UNION ALL \
        SELECT id \
        FROM parent_account \
    ) \
    AND loan_id = loan_app_loan.id and \
    la.bond_id is null \
    and le.entry_date between " + start_date_string + " and " + end_date_string + ")","loans_rec_balance");

    psqlJoinQuery->addExtraFromField("(WITH parent_account AS (\
        SELECT id \
        FROM ledger_account \
        WHERE name = 'Long-term loans receivable (net)'\
    ),\
    child_accounts AS (\
        SELECT id \
        FROM ledger_account \ 
        WHERE parent_id = (SELECT id FROM parent_account) \
    )\
    SELECT SUM(CASE WHEN lac.type='AS' OR lac.type='EX' THEN -1*amount_local ELSE amount_local END) \
    FROM ledger_amount la inner join ledger_entry le on la.entry_id = le.id inner join ledger_account lac on la.account_id =  lac.id  \
    WHERE account_id IN ( \
        SELECT id \
        FROM child_accounts \
        UNION ALL \
        SELECT id \
        FROM parent_account \
    ) \
    AND loan_id = loan_app_loan.id and \
    la.bond_id is null \
    and le.entry_date between " + start_date_string + " and " + end_date_string + ")","long_term_balance");

    psqlJoinQuery->addExtraFromField("(WITH parent_account AS (\
        SELECT id \
        FROM ledger_account \
        WHERE name = 'Impairment provisions, On-balance sheet'\
    ),\
    child_accounts AS (\
        SELECT id \
        FROM ledger_account \ 
        WHERE parent_id = (SELECT id FROM parent_account) \
    )\
    SELECT SUM(CASE WHEN lac.type='AS' OR lac.type='EX' THEN -1*amount_local ELSE amount_local END) \
    FROM ledger_amount la inner join ledger_entry le on la.entry_id = le.id inner join ledger_account lac on la.account_id =  lac.id  \
    WHERE account_id IN ( \
        SELECT id \
        FROM child_accounts \
        UNION ALL \
        SELECT id \
        FROM parent_account \
    ) \
    AND loan_id = loan_app_loan.id and \
    la.bond_id is null \
    and le.entry_date between " + start_date_string + " and " + end_date_string + ")","impairment_provisions_balance");

    psqlJoinQuery->addExtraFromField("(select percentage from loan_app_provision lap where status_id = (select status_id from loan_app_loanstatushistroy lal where loan_id= loan_app_loan.id and day <= " + end_date_string + " and status_type = 1 order by id desc limit 1))","history_provision_percentage");
    psqlJoinQuery->addExtraFromField("(select percentage from loan_app_provision where status_id = loan_app_loan.fra_status_id)","loan_provision_percentage");

    cout << psqlJoinQuery;

    return psqlJoinQuery;
}

PSQLJoinQueryIterator* UpdatingProvisions::aggregator_offbalance(string _closure_date_string,string start_fiscal_year,string end_fiscal_year,string end_date){
    string query_end_date = end_fiscal_year; 
    if (end_date != ""){
        query_end_date = end_date;
    }
    string start_date_string = "'" + start_fiscal_year + "'";
    string end_date_string = "'" + end_date + "'";
    PSQLJoinQueryIterator * psqlJoinQuery = new PSQLJoinQueryIterator("main",
    {
        new tms_app_bond_primitive_orm("main"),
        new tms_app_loanfundingrequest_primitive_orm("main"),
        new loan_app_loan_primitive_orm("main")
        },
    {
        {{"tms_app_bond","fundingfacility_ptr_id"},{"tms_app_loanfundingrequest","funding_facility_id"}},
        {{"tms_app_loanfundingrequest","loan_id"},{"loan_app_loan","id"}}
    }
    );

    psqlJoinQuery->filter(
        ANDOperator(
            new UnaryOperator("loan_app_loan.id",in,"82185,82188,82195,82196,82198,82210,82231,82234,82236,82239,82245,82265,82266,82267,82271,82279,82280,82284,82290,82292,82296,82302,82309,82313,82319,82322,82325,82328,82329,82337,82340,82343,82361,82365,82368,82369,82371,82372,82376,82378,82380,82386,82391,82395,82398,82402,82407,82411,82415,82418,82426,82429,82432,82437,82442,82446,82448,82449,82455,82458,82460,82461,82467,82470,82472,82473,82475,82479,82481,82482,82485,82486,82493,82495,82499,82511,82512,82513,82514,82516,82519,82523,82525,82527,82532,82534,82537,82538,82544,82546,82548,82555,82556,82570,82575,82577,82595,82597,82603,82606,82608,82610,82611,82613,82614,82616,82619,82621,82623,82624,82631,82640,82647,82651,82653,82656,82663,82671,82672,82678,82680,82685,82689,82690,82693,82696,82697,82701,82704,82708,82710,82712,82715,82716,82719,82722,82727,82728,82731,82737,82741,82742,82745,82755,82756,82759,82762,82763,82777,82778,82779,82780,82783,82790,82793,82795,82809,82811,82812,82813,82816,82817,82819,82822,82825,82827,82829,82835,82838,82839,82844,82845,82849,82850,82852,82855,82868,82877,82880,82882,82885,82890,82895,82898,82901,82902,82903,82906,82909,82916,82919,82922,82926,82930,82934,82938,82939,82941,82943,82948,82949,82952,82955,82958,82964,82967,82969,82980,82981,82986,82988,82989,82994,82997,83001,83009,83011,83013,83014,83018,83022,83023,83027,83028,83030,83032,83034,83035,83036,83038,83039,83045,83047,83048,83055,83061,83064,83073,83084,83087,83089,83090,83110,83114,83115,83121,83122,83123,83132,83135,83136,83140,83146,83155,83158,83163,83164,83166,83174,83175,83177,83178,83183,83184,83185,83186,83188,83192,83193,83201,83202,83205,83226,83230,83240,83242,83243,83245,83248,83257,83264,83266,83273,83274,83279,83285,83286,83288,83290,83291,83295,83297,83299,83306,83308,83309,83312,83314,83316,83318,83319,83324,83326,83329,83336,83338,83340,83342,83343,83361,83363,83377,83378,83379,83384,83386,83392,83393,83395,83398,83402,83405,83409,83415,83428,83431,83432,83433,83434,83440,83449,83459,83460,83464,83465,83467,83472,83474,83481,83484,83488,83496,83499,83500,83502,83503,83513,83517,83521,83522,83523,83524,83525,83544,83545,83550,83553,83557,83558,83559,83562,83563,83565,83567,83569,83577,83587,83589,83590,83600,83602,83605,83611,83619,83628,83629,83633,83636,83638,83640,83645,83646,83649,83650,83655,83668,83669,83677,83679,83680,83687,83691,83696,83697,83709,83713,83715,83719,83725,83726,83727,83729,83734,83738,83739,83746,83754,83759,83764,83770,83772,83773,83774,83779,83780,83781,83783,83784,83788,83789,83791,83795,83798,83803,83805,83806,83809,83812,83817,83824,83825,83827,83829,83833,83834,83835,83845,83847,83852,83853,83858,83859,83862,83865,83866,83872,83874,83885,83892,83898,83902,83915,83921,83922,83925,83927,83936,83940,83954,83955,83958,83959,83967,83970,83973,83975,83980,83982,83984,83985,83988,83989,83990,83991,83993,83995,83997,84001,84002,84004,84005,84011,84012,84013,84019,84020,84021,84023,84027,84029,84041,84044,84045,84050,84051,84052,84053,84057,84060,84062"),
            new UnaryOperator("loan_app_loan.last_closing_day",lt,_closure_date_string),
            new UnaryOperator("loan_app_loan.loan_booking_day",lte,_closure_date_string),
            new UnaryOperator("loan_app_loan.status_id",nin,"12,13")
            // new UnaryOperator("loan_app_loan.closure_status",eq,ledger_status::UPDATE_PROVISION-1)
        )
    );

    psqlJoinQuery->addExtraFromField("(WITH parent_account AS (\
        SELECT id \
        FROM ledger_account \
        WHERE name = 'Loans receivable (gross)'\
    ),\
    child_accounts AS (\
        SELECT id \
        FROM ledger_account \ 
        WHERE parent_id = (SELECT id FROM parent_account) \
    )\
    SELECT SUM(CASE WHEN lac.type='AS' OR lac.type='EX' THEN -1*amount_local ELSE amount_local END) \
    FROM ledger_amount la inner join ledger_entry le on la.entry_id = le.id inner join ledger_account lac on la.account_id =  lac.id  \
    WHERE account_id IN ( \
        SELECT id \
        FROM child_accounts \
        UNION ALL \
        SELECT id \
        FROM parent_account \
    ) \
    AND loan_id = loan_app_loan.id and \
    la.bond_id = tms_app_loanfundingrequest.funding_facility_id \
    and le.entry_date between " + start_date_string + " and " + end_date_string + ")","loans_rec_balance");

    psqlJoinQuery->addExtraFromField("(WITH parent_account AS (\
        SELECT id \
        FROM ledger_account \
        WHERE name = 'Long-term loans receivable (net)'\
    ),\
    child_accounts AS (\
        SELECT id \
        FROM ledger_account \ 
        WHERE parent_id = (SELECT id FROM parent_account) \
    )\
    SELECT SUM(CASE WHEN lac.type='AS' OR lac.type='EX' THEN -1*amount_local ELSE amount_local END) \
    FROM ledger_amount la inner join ledger_entry le on la.entry_id = le.id inner join ledger_account lac on la.account_id =  lac.id  \
    WHERE account_id IN ( \
        SELECT id \
        FROM child_accounts \
        UNION ALL \
        SELECT id \
        FROM parent_account \
    ) \
    AND loan_id = loan_app_loan.id and \
    la.bond_id = tms_app_loanfundingrequest.funding_facility_id \
    and le.entry_date between " + start_date_string + " and " + end_date_string + ")","long_term_balance");

    psqlJoinQuery->addExtraFromField("(WITH parent_account AS (\
        SELECT id \
        FROM ledger_account \
        WHERE name = 'Impairment provisions, On-balance sheet'\
    ),\
    child_accounts AS (\
        SELECT id \
        FROM ledger_account \ 
        WHERE parent_id = (SELECT id FROM parent_account) \
    )\
    SELECT SUM(CASE WHEN lac.type='AS' OR lac.type='EX' THEN -1*amount_local ELSE amount_local END) \
    FROM ledger_amount la inner join ledger_entry le on la.entry_id = le.id inner join ledger_account lac on la.account_id =  lac.id  \
    WHERE account_id IN ( \
        SELECT id \
        FROM child_accounts \
        UNION ALL \
        SELECT id \
        FROM parent_account \
    ) \
    AND loan_id = loan_app_loan.id and \
    la.bond_id = tms_app_loanfundingrequest.funding_facility_id \
    and le.entry_date between " + start_date_string + " and " + end_date_string + ")","impairment_provisions_balance");

    psqlJoinQuery->addExtraFromField("(select percentage from loan_app_provision lap where status_id = (select status_id from loan_app_loanstatushistroy lal where loan_id= loan_app_loan.id and day <= " + end_date_string + " and status_type = 1 order by id desc limit 1))","history_provision_percentage");
    psqlJoinQuery->addExtraFromField("(select percentage from loan_app_provision where status_id = loan_app_loan.fra_status_id)","loan_provision_percentage");

    cout << psqlJoinQuery;

    return psqlJoinQuery;
}

UpdatingProvisions::UpdatingProvisions(map<string,PSQLAbstractORM*>*_orms,string start_date_input, string end_date_input){
    lal_orm = ORM(loan_app_loan,_orms);
    tal_orm = ORM(tms_app_loanfundingrequest, _orms);

    cout << "LOAN ID : " << lal_orm->get_id() << endl;
    cout << "MERCHANT ID: " << lal_orm->get_merchant_id() << endl;
    PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,_orms);
    long_term_balance = gorm->toFloat("long_term_balance");
    cout << "LONG TERM BALANCE : ";
    cout << long_term_balance << endl;
    loans_rec_balance = gorm->toFloat("loans_rec_balance");
    cout << "Loans receivable BALANCE : ";
    cout << loans_rec_balance << endl;
    impairment_provisions_balance = gorm->toFloat("impairment_provisions_balance");
    cout << "IMP prov BALANCE : ";
    cout << impairment_provisions_balance << endl;
    history_provision_percentage = gorm->toInt("history_provision_percentage");
    cout << "HISTORY PERCENTAGE " << gorm->get("history_provision_percentage");
    loan_provision_percentage = gorm->toInt("loan_provision_percentage");
    cout << "LOAN PERCENTAGE " << gorm->get("loan_provision_percentage");
    on_balance = gorm->toBool("on_balance");
    cout << "BOND ID : " << tal_orm->get_funding_facility_id() << endl;
    on_balance = false;
    start_date = start_date_input;
    end_date = end_date_input;
    missing_provisions = 0;
    this->calculateMissingProvisions();
}

UpdatingProvisions::UpdatingProvisions(loan_app_loan_primitive_orm * loan,string start_date_input,string end_date_input){
    lal_orm = loan;
    cout << "LOAN ID : " << lal_orm->get_id() << endl;
    cout << "MERCHANT ID: " << lal_orm->get_merchant_id() << endl;
    long_term_balance = lal_orm->getExtraToFloat("long_term_balance");
    cout << "LONG TERM BALANCE : ";
    cout << long_term_balance << endl;
    loans_rec_balance = lal_orm->getExtraToFloat("loans_rec_balance");
    cout << "Loans receivable BALANCE : ";
    cout << loans_rec_balance << endl;
    impairment_provisions_balance = lal_orm->getExtraToFloat("impairment_provisions_balance");
    cout << "IMP prov BALANCE : ";
    cout << impairment_provisions_balance << endl;
    history_provision_percentage = lal_orm->getExtraToFloat("history_provision_percentage");
    cout << "HISTORY PERCENTAGE " << history_provision_percentage << endl;;
    loan_provision_percentage = lal_orm->getExtraToFloat("loan_provision_percentage");
    cout << "LOAN PERCENTAGE " << loan_provision_percentage << endl;;
    start_date = start_date_input;
    end_date = end_date_input;
    missing_provisions = 0;
    on_balance = true;
    this->calculateMissingProvisions();
}

LedgerAmount* UpdatingProvisions::_init_ledger_amount(){
    LedgerAmount * lg = new LedgerAmount();
    lg->setCustomerId(lal_orm->get_customer_id());
    lg->setLoanId(lal_orm->get_id());
    lg->setMerchantId(lal_orm->get_merchant_id());
    if(!on_balance){
        lg->setBondId(funding_facility_id);
    }
    return lg;
}

void UpdatingProvisions::update_step(){
    PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator(
            new UnaryOperator ("loan_app_loan.id",ne,"14312"),
            //TODO: Change To update status comparing to the closure status of the step before it not gt 0
            new UnaryOperator ("loan_app_loan.closure_status",gte,0)
        ),
        {{"closure_status",to_string(ledger_status::UPDATE_PROVISION)}}
        );
        psqlUpdateQuery.update();  
}

void UpdatingProvisions::setupLedgerClosureService(LedgerClosureService * ledgerClosureService)
{
    ledgerClosureService->addHandler("Increasing provisions ",_increasing_provisions);
    ledgerClosureService->addHandler("Decreasing provisions ",_decreasing_provisions);
}

float UpdatingProvisions::get_missing_provisions(){
    return missing_provisions;
}

void UpdatingProvisions::set_missing_provisions(float x){
    missing_provisions = x;
}

void UpdatingProvisions::calculateMissingProvisions(){
    double expected_provisions = 0;
    double loan_marginalized_balance = loans_rec_balance + long_term_balance;
    cout << "LOAN MARG BALANCE = " << loan_marginalized_balance << endl;
    if(end_date != ""){
        expected_provisions = std::floor(((static_cast<double>(history_provision_percentage) / 100.00) * loan_marginalized_balance)*100) / 100;
        cout << "CALCULATING EXPECTED PROVISIONS HISTORY : (" << history_provision_percentage << "/ 100) * " << loan_marginalized_balance << endl;
        cout << "HISTORY EXPECTED PROVISIONS = " << expected_provisions << endl;
    }
    else{
        expected_provisions = (loan_provision_percentage / 100) * loan_marginalized_balance;
    }
    expected_provisions = ROUND(expected_provisions);
    cout << "EXPECTED PROVISIONS AFTER ROUNDING: " << expected_provisions << endl;
    missing_provisions =  ROUND((expected_provisions - std::abs(impairment_provisions_balance)));
    if(missing_provisions <= 0.01 && missing_provisions >= -0.01){
        missing_provisions = 0.0;
    }
    cout << "MISSING PROVISIONS = " << missing_provisions << endl;
}

LedgerAmount * UpdatingProvisions::_increasing_provisions(LedgerClosureStep * updatingProvisionsStep){
    float missing_provisions = ((UpdatingProvisions*)updatingProvisionsStep)->get_missing_provisions();
    cout << "MISSING PROVISIONS : " << missing_provisions << endl;
    if(missing_provisions > 0.0){
        cout << "-----------INCREASING PROVISIONS WITH " << missing_provisions << endl;
        LedgerAmount * la = ((UpdatingProvisions*)updatingProvisionsStep)->_init_ledger_amount();
        la->setAmount(missing_provisions);
        return la;
    }
    else{
        LedgerAmount * la = ((UpdatingProvisions*)updatingProvisionsStep)->_init_ledger_amount();
        la->setAmount(0.0);
        return la;
    }
}

LedgerAmount * UpdatingProvisions::_decreasing_provisions(LedgerClosureStep * updatingProvisionsStep){
    float missing_provisions = ((UpdatingProvisions*)updatingProvisionsStep)->get_missing_provisions();
    cout << "GOT MISSING PROVISIONS WITH : " << missing_provisions << endl;
    if(missing_provisions < 0.0){
        cout << "-----------DECREASING PROVISIONS WITH " << missing_provisions << endl;
        LedgerAmount * la = ((UpdatingProvisions*)updatingProvisionsStep)->_init_ledger_amount();
        la->setAmount(missing_provisions);
        return la;
    }
    else{
        LedgerAmount * la = ((UpdatingProvisions*)updatingProvisionsStep)->_init_ledger_amount();
        la->setAmount(0.0);
        return la;
    }
}

UpdatingProvisions::~UpdatingProvisions(){}

