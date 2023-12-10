from database_utils import SQLUtilsService
from colors import *
import polars as pl
import pandas as pd
from polars.testing import assert_frame_equal


class UpdateLoanStatus:
    
    def __init__(self, connection_c, connection_python,database_copy_date):
        self.connection_c = connection_c
        self.connection_python = connection_python
        self.database_copy_date = database_copy_date
        self.color_options = color_options


    """
        UPDATE LOAN STATUS TESTS:
            1-Create 2 installemnt status history [FRA-BLNK]
            2-Create 2 Loan Status History [FRA-BLNK]
            3-Update payment_status, status_id, fra_status For Installment Extentions
            4-Update Status For Loan
            5-Installments payment status history
            #6-Changes LMS Closure Status 2->3
    """

    #1-Create 2 installemnt status history [FRA-BLNK]
    def installment_status_history_entries(self):
        try:
            print_colored("Test 1 :: Create 2 installemnt status history [FRA-BLNK]", self.color_options.YELLOW ,bold=True)


            query = f"""
                SELECT "day" , status_type , installment_id , previous_status_id , status_id  
                FROM new_lms_installmentstatushistory nli
                where DATE(created_at) > '{self.database_copy_date}'
                order by installment_id desc, "day" desc, status_type desc
            """

            # C++
            data_c = SQLUtilsService.execute_query(self.connection_c, query)
            # print_colored("Results from DB_c(Installment Payment History):", self.color_options.BLUE, bold=True)
            # print(data_c[0])

            # PYTHON
            data_python = SQLUtilsService.execute_query(self.connection_python, query)
            # print_colored("Results from DB_python(Installment Payment History):", self.color_options.BLUE, bold=True)
            # print(data_python[0])

            df_c = pl.DataFrame(data_c)
            df_python = pl.DataFrame(data_python)
            # df_python.write_csv("python.csv")

            # Compare the results
            assert_frame_equal(df_c, df_python)
            print_colored("PASS -> Data of Installment Payment History", color=self.color_options.GREEN, bold=True)

        except Exception as e:
            print_colored(f"An error occurred: {e}", color='red', bold=True)


    #2-Create 2 Loan Status History [FRA-BLNK]
    def loan_status_history_entries(self):
        try:
            print_colored("Test 2 :: Create 2 Loan status history [FRA-BLNK]", self.color_options.YELLOW ,bold=True)

            query = f"""
                select "day" , status_type , loan_id , previous_status_id , status_id , reversal_order_id  
                from loan_app_loanstatushistroy lal 
                where DATE(created_at) > '{self.database_copy_date}'
                order by  loan_id desc,previous_status_id desc, "day" desc, status_type desc
            """

            # C++
            data_c = SQLUtilsService.execute_query(self.connection_c, query)
            # print_colored("Results from DB_c(Installment Payment History):", self.color_options.BLUE, bold=True)
            # print(data_c[0])

            # PYTHON
            data_python = SQLUtilsService.execute_query(self.connection_python, query)
            # print_colored("Results from DB_python(Installment Payment History):", self.color_options.BLUE, bold=True)
            # print(data_python[0])

            df_c = pl.DataFrame(data_c)
            df_python = pl.DataFrame(data_python)
            # df_python.write_csv("python.csv")

            # Compare the results
            assert_frame_equal(df_c, df_python)
            print_colored("PASS -> Data of Loan Status History", color=self.color_options.GREEN, bold=True)

        except Exception as e:
            print_colored(f"An error occurred: {e}", color='red', bold=True)



    #3-Update payment_status, status_id, fra_status For Installment Extentions
    def update_installmentextention_entries(self):
        try:
            print_colored("Test 3 :: Update payment_status, status_id, fra_status For Installment Extentions", self.color_options.YELLOW ,bold=True)

            query = f"""
                select status_id , fra_status_id , payment_status 
                from new_lms_installmentextension nli 
                order by installment_ptr_id  desc 
            """

            # C++
            data_c = SQLUtilsService.execute_query(self.connection_c, query)
            # print_colored("Results from DB_c(Installment Payment History):", self.color_options.BLUE, bold=True)
            # print(data_c[0])

            # PYTHON
            data_python = SQLUtilsService.execute_query(self.connection_python, query)
            # print_colored("Results from DB_python(Installment Payment History):", self.color_options.BLUE, bold=True)
            # print(data_python[0])

            df_c = pl.DataFrame(data_c)
            df_python = pl.DataFrame(data_python)
            # df_python.write_csv("python.csv")

            # Compare the results
            assert_frame_equal(df_c, df_python)
            print_colored("PASS -> Update payment_status, status_id, fra_status For Installment Extentions", color=self.color_options.GREEN, bold=True)

        except Exception as e:
            print_colored(f"An error occurred: {e}", color='red', bold=True)


    #4-Update Loan status id and fra status id
    def update_loan_status(self):
        try:
            print_colored("Test 4 :: Update Status For Loan", self.color_options.YELLOW ,bold=True)

            query = f"""
                select status_id, fra_status_id from loan_app_loan lal order by id desc 
            """

            # C++
            data_c = SQLUtilsService.execute_query(self.connection_c, query)
            # print_colored("Results from DB_c(Installment Payment History):", self.color_options.BLUE, bold=True)
            # print(data_c[0])

            # PYTHON
            data_python = SQLUtilsService.execute_query(self.connection_python, query)
            # print_colored("Results from DB_python(Installment Payment History):", self.color_options.BLUE, bold=True)
            # print(data_python[0])

            df_c = pl.DataFrame(data_c)
            df_python = pl.DataFrame(data_python)
            # df_python.write_csv("python.csv")

            # Compare the results
            assert_frame_equal(df_c, df_python)
            print_colored("PASS -> All Loan status, fra-status are equal", color=self.color_options.GREEN, bold=True)

        except Exception as e:
            print_colored(f"An error occurred: {e}", color='red', bold=True)


    #5-Installments payment status history
    def create_installment_payment_status_history(self):
        try:
            print_colored("Test 5 :: Installments payment status history", self.color_options.YELLOW ,bold=True)

            # query = f"""
            #     select status  from new_lms_installmentpaymentstatushistory nli 
            #     where "day" > '2023-11-20'
            #     order by installment_extension_id desc
            # """

            query = f"""select status, installment_extension_id , "day" , status
                        from new_lms_installmentpaymentstatushistory nli 
                        where day > '{self.database_copy_date}' 
                        order by  installment_extension_id desc,  "day" desc, status desc
                    """


            # C++
            data_c = SQLUtilsService.execute_query(self.connection_c, query)
            # print_colored("Results from DB_c(Installment Payment History):", self.color_options.BLUE, bold=True)
            # print(data_c[0])

            # PYTHON
            data_python = SQLUtilsService.execute_query(self.connection_python, query)
            # print_colored("Results from DB_python(Installment Payment History):", self.color_options.BLUE, bold=True)
            # print(data_python[0])

            df_c = pl.DataFrame(data_c)
            df_python = pl.DataFrame(data_python)
            # df_python.write_csv("python.csv")

            # Compare the results
            assert_frame_equal(df_c, df_python)
            print_colored("PASS -> Installments payment status history", color=self.color_options.GREEN, bold=True)

        except Exception as e:
            print_colored(f"An error occurred: {e}", color='red', bold=True)



    #6-Changes LMS Closure Status 2->3
    def update_loan_status_closure_status(self):
        try:
            print_colored("Test 6 :: Changes LMS Closure Status Due To Over Due 2->3", self.color_options.YELLOW ,bold=True)

            query = f"select lms_closure_status, count(*) from loan_app_loan group by lms_closure_status "

            # C++
            data_c = SQLUtilsService.execute_query(self.connection_c, query)
            # print_colored("Results from DB_c (Due To OverDue):", self.color_options.BLUE ,bold=True)
            # print(data_c)

            # PYTHON
            data_python = SQLUtilsService.execute_query(self.connection_python, query)
            # print_colored("Results from DB_python (Due To OverDue):",self.color_options.BLUE,  bold=True)
            # print(data_python)


            df_c = pl.DataFrame(data_c)
            df_python = pl.DataFrame(data_python)
            
            assert_frame_equal(df_c, df_python)
            print_colored("PASS -> LMS Closure Status Updated ",color=self.color_options.GREEN , bold=True)

        except Exception as e:
                print_colored(f"An error occurred: {e}", self.color_options.RED, bold=True)

    def test_update_loan_status(self):
        self.installment_status_history_entries()
        self.loan_status_history_entries()
        self.update_installmentextention_entries()
        self.update_loan_status()
        self.create_installment_payment_status_history()
        self.update_loan_status_closure_status()
