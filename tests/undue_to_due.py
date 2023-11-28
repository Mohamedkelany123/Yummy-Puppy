from database_utils import SQLUtilsService
from colors import *
import polars as pl
import pandas as pd
from polars.testing import assert_frame_equal

class UndueToDue:
    
    def __init__(self, connection_c, connection_python,closure_before_running_date):
        self.connection_c = connection_c
        self.connection_python = connection_python
        self.closure_before_running_date = closure_before_running_date
        self.color_options = color_options


    """
        UNDUE TO DUE TESTS:
            1-Installments Extension from (payment status) 5->4 - Compare Count Of Installments Extentions with payment status = 4
            2-Update LMS closure status -undue to -due [FAILED]
            3-Compare Payments Status history (COUNT)
            3.1-Compare The Actual Payments History Status Entries
    """


    #1-Installments Extension from (payment status) 5->4 (COUNT)
    def installment_extention(self, payment_status):
        try:
            print_colored("Test 1 :: Compare Count Of Installments Extentions with payment status = 4", self.color_options.YELLOW ,bold=True)
            query = f"SELECT COUNT(*) FROM new_lms_installmentextension WHERE payment_status = {payment_status}"

            # C++
            data_c = SQLUtilsService.execute_query(self.connection_c, query)
            print_colored("Results from DB_c (Payment Status = 4):", self.color_options.BLUE ,bold=True)
            print(data_c)

            # PYTHON
            data_python = SQLUtilsService.execute_query(self.connection_python, query)
            print_colored("Results from DB_python (Payment Status = 4):",self.color_options.BLUE,  bold=True)
            print(data_python)




            # Compare the results
            if data_c == data_python:
                print_colored("PASS -> Installments Extension from (payment status) 5->4",color=self.color_options.GREEN , bold=True)
                # Continue with the next steps
            else:
                print_colored("FAIL -> There is a problem in the count between both databases.", self.color_options.RED, bold=True)
                print("DB_c result:", data_c)
                print("DB_python result:", data_python)

        except Exception as e:
                print_colored(f"An error occurred: {e}", color='red', bold=True)


    #2-Update loan closure status -undue to -due
    def undue_to_due_lms_closure_status(self):
        try:
            print_colored("Test 2 :: Update LMS closure status -undue to -due", self.color_options.YELLOW ,bold=True)

            query = f"select lms_closure_status, count(*) from loan_app_loan group by lms_closure_status "

            # C++
            data_c = SQLUtilsService.execute_query(self.connection_c, query)
            print_colored("Results from DB_c (Undue To Due):", self.color_options.BLUE ,bold=True)
            print(data_c)

            # PYTHON
            data_python = SQLUtilsService.execute_query(self.connection_python, query)
            print_colored("Results from DB_python (Undue To Due):",self.color_options.BLUE,  bold=True)
            print(data_python)


            df_c = pl.DataFrame(data_c)
            df_python = pl.DataFrame(data_python)
            
            assert_frame_equal(df_c, df_python)
            print_colored("PASS -> Update loan closure status -undue to -due ",color=self.color_options.GREEN , bold=True)

        except Exception as e:
                print_colored(f"An error occurred: {e}", self.color_options.RED, bold=True)

    #3-Compare Payments Status history (COUNT)
    def installment_payment_history_count(self):
        try:
            print_colored("Test 3 :: Compare Payments Status history (COUNT) ", self.color_options.YELLOW ,bold=True)

            query = f"select count(*) from new_lms_installmentpaymentstatushistory nli"

            #C++
            data_c = SQLUtilsService.execute_query(self.connection_c, query)
            print_colored("Results from DB_c(Installment Payment History):", self.color_options.BLUE, bold=True)
            print(data_c[0])

            #PYTHON
            data_python = SQLUtilsService.execute_query(self.connection_python, query)
            print_colored("Results from DB_python(Installment Payment History):", self.color_options.BLUE, bold=True)
            print(data_python[0])

            # Compare the results
            if data_c == data_python:
                print_colored("PASS -> Count for Installment Payment History",color=self.color_options.GREEN , bold=True)
            else:
                print_colored("FAIL -> There is a problem in the count between both databases.", self.color_options.RED, bold=True)
                print("DB_c result:", data_c)
                print("DB_python result:", data_python)
        except Exception as e:
            print_colored(f"An error occurred: {e}", self.color_options.RED, bold=True)

    #3.1-Compare The Actual Payments History Status Entries
    def installment_payment_history_data(self):
        try:
            print_colored("Test 4 :: Compare The Actual Payments History Status Entries", self.color_options.YELLOW ,bold=True)

            query = f"select status, installment_extension_id , coalesce(order_id,-1) as order_id  , \"day\" from new_lms_installmentpaymentstatushistory nli where day >= '{self.closure_before_running_date}' order by  installment_extension_id desc,  \"day\" desc, status desc"

            # C++
            data_c = SQLUtilsService.execute_query(self.connection_c, query)
            print_colored("Results from DB_c(Installment Payment History):", self.color_options.BLUE, bold=True)
            print(data_c[0])

            # PYTHON
            data_python = SQLUtilsService.execute_query(self.connection_python, query)
            print_colored("Results from DB_python(Installment Payment History):", self.color_options.BLUE, bold=True)
            print(data_python[0])

            df_c = pl.DataFrame(data_c)
            df_python = pl.DataFrame(data_python)

            # Compare the results
            assert_frame_equal(df_c, df_python)
            print_colored("PASS -> Data of Installment Payment History", color=self.color_options.GREEN, bold=True)

        except Exception as e:
            print_colored(f"An error occurred: {e}", color='red', bold=True)



        


    def test_undue_to_due(self):
        self.installment_extention(4)
        self.undue_to_due_lms_closure_status()
        self.installment_payment_history_count()
        self.installment_payment_history_data()


    


