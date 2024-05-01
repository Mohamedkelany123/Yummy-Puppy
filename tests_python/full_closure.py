from database_utils import SQLUtilsService
from colors import *
import polars as pl
import pandas as pd
from polars.testing import assert_frame_equal
import tqdm

from slack_notify import send_slack_message
from datetime import datetime


class FullClosure:
    
    def __init__(self, connection_c, connection_python):
        self.connection_c = connection_c
        self.connection_python = connection_python
        self.color_options = color_options

        self.webhook_url = "https://hooks.slack.com/services/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"
        self.current_date = datetime.now().strftime("%Y/%m/%d")



    def test(self):
        send_slack_message(self.webhook_url, f"*STARTING LMS CLOSURE STATUS TESTS: {str(self.current_date)}* ", "#0000FF")
        self.wallet()
        self.wallet_transaction()
        self.loan_order()
        self.loan_order_header()
        self.loan_app_loan()
        self.installment_extentions()
        self.installment()
        self.late_fees()
        self.installment_payment_status_history()
        self.installment_status_history()
        self.loan_status_history()
        send_slack_message(self.webhook_url, f"*FINISHED LMS CLOSURE STATUS TESTS: {str(self.current_date)}* ", "#0000FF")



    """
        Tables To Test:

            1-Installment Extention Excluding[ext_created_at , ext_updated_at]
            2-Loan App Loan Excluding[created_at, updated_at]
            3-Installment Excluding[created_at, updated_at]
            4-Installment Late Fees Exclude-ID-(Step-4 Marginalization Creates Late Fees So We cant Compare the [ids] as C++ runs using Multithreads)
            5-Installment Payment Status History Exclude
            6-Installment Status History 
            7-Loan Status History
            8- Wallet
            9- Wallet Transaction
            10- Loan Order
            11- Loan Order Header
    """


    #1-Installment Extention [ext_created_at , ext_updated_at]
    def installment_extentions(self):
        print_colored("Test 1 :: Installment Extention ", self.color_options.YELLOW ,bold=True)
        query = f"""
                    select *
                    from 
                        new_lms_installmentextension nli 
                    order by 
                        installment_ptr_id  desc
                """
        excluded_columns = ['ext_created_at', 'ext_updated_at','early_paid_at', 'interest_paid_at' ,'principal_paid_at', 'extra_interest_paid_at','interest_order_id', 'principal_order_id', 'early_order_id', 'extra_interest_order_id']
        temp = self.exec(query, excluded_columns, "Installment Extention")
        if temp:
            send_slack_message(self.webhook_url, "PASS -> Installment Extention ", "#00FF00")

    #2-Loan App Loan Exclude[created_at , updated_at]
    def loan_app_loan(self):
        print_colored("Test 2 :: Loan App Loan ", self.color_options.YELLOW ,bold=True)
        query = f"""
                    select *
                    from 
                        loan_app_loan lal 
                    order by 
                        id desc
                """
        excluded_columns = ['created_at', 'updated_at']



        temp = self.exec(query, excluded_columns,  "Loan App Loan")
        if temp:
            send_slack_message(self.webhook_url, "PASS -> Loan App Loan ", "#00FF00")



    #3-Installment Exclude[created_at , updated_at]
    def installment(self):
        print_colored("Test 3 :: Installment ", self.color_options.YELLOW ,bold=True)
        query = f"""
                    select * 
                    from 
                        loan_app_installment lai 
                    order by 
                        id desc
                """
        excluded_columns = ['created_at', 'updated_at']
        temp = self.exec(query, excluded_columns, "Installment")
        if temp:
            send_slack_message(self.webhook_url, "PASS -> Installment ", "#00FF00")


    #4-Installment Late Fees Exclude[id, created_at , updated_at]-(Marginalization Creates Late Fees So We cant Compare the ids as C++ runs using Multithreads)
    def late_fees(self):
        print_colored("Test 4 :: Installment Late Fees ", self.color_options.YELLOW ,bold=True)
        query = f"""
                select *
                from 
                    new_lms_installmentlatefees nli 
                order by 
                    installment_extension_id  desc, "day" desc, installment_status_id desc
                """
        excluded_columns = ['id', 'created_at', 'updated_at', 'paid_at']
        temp = self.exec(query, excluded_columns, "Installment Late Fees")
        if temp:
            send_slack_message(self.webhook_url, "PASS -> Installment Late Fees ", "#00FF00")



    #5-Installment Payment Status History Exclude[id, created_at , updated_at]
    def installment_payment_status_history(self):
        print_colored("Test 5 :: Installment Payment Status History ", self.color_options.YELLOW ,bold=True)
        query = f"""
                select * 
                from 
                    new_lms_installmentpaymentstatushistory nli 
                order by  
                    installment_extension_id desc,  "day" desc, status desc
                """
        excluded_columns = ['id', 'created_at', 'updated_at', 'order_id']

        temp = self.exec(query, excluded_columns,"Installment Payment Status History")
        if temp:
            send_slack_message(self.webhook_url, "PASS -> Installment Payment Status History ", "#00FF00")

    #6-Installment Status History 
    def installment_status_history(self):
        print_colored("Test 6 :: Installment Status History ", self.color_options.YELLOW ,bold=True)
        query = f"""
                SELECT *  
                FROM 
                    new_lms_installmentstatushistory nli
                order by 
                    installment_id desc, "day" desc, status_type desc, status_id desc
                """
        excluded_columns = ['id', 'created_at', 'updated_at']
        temp = self.exec(query, excluded_columns, "Installment Status History")
        if temp:
            send_slack_message(self.webhook_url, "PASS -> Installment Status History ", "#00FF00")


    #7-Loan Status History
    def loan_status_history(self):
        print_colored("Test 7 :: Loan Status History ", self.color_options.YELLOW ,bold=True)
        query = f"""
                select *  
                from 
                    loan_app_loanstatushistroy lal 
                order by
                    loan_id desc,previous_status_id desc, "day" desc, status_id desc, status_type desc
                """
        excluded_columns = ['id', 'created_at', 'updated_at']
        temp = self.exec(query, excluded_columns, "Loan Status History")
        if temp:
            send_slack_message(self.webhook_url, "PASS -> Loan Status History ", "#00FF00")

    #8- Wallet
    def wallet(self):
        print_colored("Test 8 :: Wallet ", self.color_options.YELLOW ,bold=True)
        query = f"""
                    select * 
                    from new_lms_customerwallet nlc 
                    order by id 
                """
        excluded_columns = ['created_at', 'updated_at']
        temp = self.exec(query, excluded_columns, "Wallet")
        if temp:
            send_slack_message(self.webhook_url, "PASS -> Wallet ", "#00FF00")

    #9- Wallet Transaction
    def wallet_transaction(self):
        print_colored("Test 9 :: Wallet Transaction ", self.color_options.YELLOW ,bold=True)
        query = f"""
                    select * 
                    from new_lms_customerwallettransaction nlc 
                    order by customer_wallet_id , amount  
                """
        excluded_columns = ['id', 'created_at', 'updated_at', 'order_id']
        temp = self.exec(query, excluded_columns, "Wallet Transaction")
        if temp:
            send_slack_message(self.webhook_url, "PASS -> Wallet Transaction ", "#00FF00")

    #10- Loan Order
    def loan_order(self):
        print_colored("Test 10 :: Loan Order ", self.color_options.YELLOW ,bold=True)
        query = f"""
                    select * 
                    from payments_loanorder pl 
                    where created_at::date > '2024-04-01'
                    order by customer_id , amount, status , payment_method_id, payment_method_code, payment_ledger_entry_id, loan_id
                """
        excluded_columns = ['id', 'created_at', 'updated_at', 'processed_at', 'paid_at', 'installments', 'loan_order_header_id']
        temp = self.exec(query, excluded_columns, "Loan Order")
        if temp:
            send_slack_message(self.webhook_url, "PASS -> Loan Order ", "#00FF00")

    #11- Loan Order Header
    def loan_order_header(self):
        print_colored("Test 11 :: Loan Order Header ", self.color_options.YELLOW ,bold=True)
        query = f"""
                    select * 
                    from payments_loanorderheader pl 
                    where created_at::date > '2024-01-01'
                    order by customer_id , total_amount , payment_ledger_entry_id , adjustment_amount
                """
        excluded_columns = ['id', 'created_at', 'updated_at']
        temp = self.exec(query, excluded_columns, "Loan Order Header")
        if temp:
            send_slack_message(self.webhook_url, "PASS -> Loan Order Header ", "#00FF00")











#---------------------------------------------------------------------------------------------------------------------------------------------------#
    def exec(self, query ,excluded_columns=None, tableName = ""):
        try:
            print_colored("EXCLUDE:", self.color_options.CYAN, bold=True)
            for excluded_column in excluded_columns:
                print_colored(f"[{excluded_column}]",self.color_options.CYAN, bold=True)
            print_colored("---------------------------", self.color_options.CYAN, bold=True)

            # C++
            data_c = SQLUtilsService.execute_query(self.connection_c, query)
            print_colored("----------> DATA C++ FETCHED ", self.color_options.CYAN, bold=True)


            # PYTHON
            data_python = SQLUtilsService.execute_query(self.connection_python, query)

            print_colored("----------> DATA Python FETCHED", self.color_options.CYAN, bold=True)

            if excluded_columns:
                print_colored("----------> FILTERING DATA", self.color_options.CYAN, bold=True)
                data_c_filtered = []
                for row in tqdm.tqdm(data_c, desc="Filtering C++ Data"):
                    filtered_row = {k: v for k, v in row.items() if k not in excluded_columns}
                    data_c_filtered.append(filtered_row)

                data_python_filtered = []
                for row in tqdm.tqdm(data_python, desc="Filtering Python Data"):
                    filtered_row = {k: v for k, v in row.items() if k not in excluded_columns}
                    data_python_filtered.append(filtered_row)
            else:
                data_c_filtered = data_c
                data_python_filtered = data_python

            if len(data_c_filtered) != len(data_python_filtered):
                send_slack_message(self.webhook_url, f"FAILED -> {tableName} ", "#FF0000")
                raise ValueError(f"Data LENGTHS ARE NOT EQUAL")

            counter = 0

            for i in tqdm.tqdm(range(len(data_c_filtered)), desc="Comparing Data"):
                        if str(data_c_filtered[i]) != str(data_python_filtered[i]):
                            counter += 1
                            if counter < 10:
                                print_colored("Data C++:", self.color_options.RED, bold=True)
                                print_colored(str(data_c_filtered[i]), self.color_options.BLUE, bold=True)
                                print_colored("Data Python:", self.color_options.RED, bold=True)
                                print_colored(str(data_python_filtered[i]), self.color_options.CYAN, bold=True)
                                print(f"Data mismatch at index {i}")


                            # send_slack_message(self.webhook_url, f"FAILED -> {tableName} ", "#FF0000")
                            # raise ValueError(f"Data mismatch at index {i}")
            # print("--------------------------------------------------COUNTER:", counter)
            if counter > 0:
                print_colored(str(counter), color=self.color_options.RED, bold=True)
                print_colored("-----------------------------------FAILED---------------------------------", color=self.color_options.RED, bold=True)
                send_slack_message(self.webhook_url, f"FAILED -> {tableName} ", "#FF0000")
                return False
            else:
                print_colored(str(counter), color=self.color_options.GREEN, bold=True)
                print_colored("-----------------------------------PASS---------------------------------", color=self.color_options.GREEN, bold=True)
                return True

        except Exception as e:
            print_colored(f"FAIL ---> An error occurred: {e}", color_options.RED, bold=True)


