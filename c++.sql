--  '2022-07-01'
with loan_on_day_status as (
    select id as loan_id,
    	(
            select status_id
            from loan_app_loanstatushistroy
            where day <= {'date':'end_date'} and status_type = 0 and loan_app_loanstatushistroy.loan_id = loan_app_loan.id
            order by loan_id, updated_at desc
            limit 1
        ) as blnk_status,
        (
            select status_id
            from loan_app_loanstatushistroy
            where day <= {'date':'end_date'} and status_type = 1 and loan_app_loanstatushistroy.loan_id = loan_app_loan.id
            order by loan_id, updated_at desc
            limit 1
        ) as fra_status,
        (
            select day
            from loan_app_loanstatushistroy
            where day <= {'date':'end_date'} and status_type = 0 and loan_app_loanstatushistroy.loan_id = loan_app_loan.id
            order by loan_id, updated_at desc
            limit 1
        ) as status_day
        from loan_app_loan
        order by loan_id
),
 bv_loans as (
	select distinct loan_id, talifr.funding_facility_id as bond_id
    from tms_app_loaninstallmentfundingrequest talifr
    inner join tms_app_fundingfacility tmsff on tmsff.id = talifr.funding_facility_id
    where tmsff.funding_source_id = 7
    union 
    select talmb.loan_id, talmb.bond_id
	from tms_app_loanmerchantbatch talmb
	where talmb.bond_id = 16 
),

bv_off_bal_installments as (
	select lai.id as installment, lai.loan_id , bv.bond_id, lai.day, taf.contract_start_date::date as contract_start_date 
	from loan_app_installment lai 
	inner join bv_loans bv on bv.loan_id = lai.loan_id 
	inner join tms_app_loaninstallmentfundingrequest talifr on talifr.installment_id = lai.id
	inner join tms_app_fundingfacility taf on taf.id = talifr.funding_facility_id
	where talifr.onbalance = false and assignment_day <= {'date':'end_date'}

),
latest_payment_status as (
    select nli.id, installment_extension_id as installment_id, status as payment_status, day
    from new_lms_installmentpaymentstatushistory nli
    where day <= {'date':'end_date'}
    --where installment_extension_id = 161515
    order by nli.installment_extension_id--, nli.day desc, nli.id desc
),

--paid_late_fees as (
--	select nlilf.installment_extension_id ,sum(nlilf.amount) as amount, nlilf.paid_at::date as paid_at, bvi.bond_id
--	from new_lms_installmentlatefees nlilf
--	inner join bv_off_bal_installments bvi on bvi.installment = nlilf.installment_extension_id 
--	where nlilf.is_paid = true and nlilf.paid_at::date >= bvi.contract_start_date
--	group by nlilf.installment_extension_id , nlilf.paid_at::date, bvi.bond_id
--)

order_headers as (
    select coalesce(income_fee,0) as income_fee, coalesce(expense_fee,0) as expense_fee,
    (select id from payments_loanorder where payments_loanorder.loan_order_header_id=payments_loanorderheader.id
    and status=1 order by id limit 1) as first_order_id
    from payments_loanorderheader
)
,
late_fees_details as (
     select installment_extension_id, paid_at::date, sum(amount) as lf_paid
 from new_lms_installmentlatefees nlilf 
 group by installment_extension_id, paid_at::date
),
payment_details as (
	select  nli.installment_ptr_id "Installment ID", bvi.bond_id,
	(case when nli.installment_ptr_id in (select cast(jsonb_array_elements(plo.installments -> 'interest_paid' -> 'installments') as int)) then nli.actual_interest_paid else 0 end)
	as interest_paid,
	(case when nli.installment_ptr_id in (select cast(jsonb_array_elements(plo.installments -> 'principal_paid' -> 'installments') as int)) then lai.principal_expected else 0 end)
	as principal_paid,
    (case when nli.installment_ptr_id in (select cast(jsonb_array_elements(plo.installments -> 'early_fees_paid' -> 'installments') as int)) then nli.early_fee_amount else 0 end)
    as early_fee_paid,
    (case when nli.installment_ptr_id in (select cast(jsonb_array_elements(plo.installments -> 'extra_interest_ids' -> 'installments') as int)) then nli.first_installment_interest_adjustment else 0 end)
     first_installment_interest_adjustment, 
     (case when nli.installment_ptr_id in (select cast(jsonb_array_elements(plo.installments -> 'late_fees_paid' -> 'installments') as int)) then late_fees_details.lf_paid else 0 end)
     late_fees_paid,
   
     plo.paid_at::date "Payment date",
    lal.id "Loan ID"
	from new_lms_installmentextension nli 
	inner join loan_app_installment lai on lai.id = nli.installment_ptr_id 
	inner join loan_app_loan lal on lal.id = lai.loan_id 
	inner join payments_loanorder plo on plo.loan_id = lal.id  
	inner join bv_off_bal_installments bvi on bvi.installment = lai.id 
    left join late_fees_details on late_fees_details.installment_extension_id = lai.id and plo.paid_at::date = late_fees_details.paid_at::date
    where plo.status=1 and plo.paid_at::date >= {'date':'start_date'}  and plo.paid_at::date <= {'date':'end_date'} and plo.paid_at::date >= bvi.contract_start_date
    
)

select "Loan ID", bond_id, sum(principal_paid) as principal_paid, sum(interest_paid+first_installment_interest_adjustment) as interest_paid, sum(early_fee_paid) as early_fee_paid, sum(late_fees_paid) late_fees_paid
from payment_details 
where 1=1 [[and bond_id={'number':'bond'}]] 
--and "Loan ID" = 57830
group by  "Loan ID", bond_id
order by "Loan ID"


