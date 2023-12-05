from database_utils import SQLUtilsService
from colors import *
import polars as pl
import pandas as pd
from polars.testing import assert_frame_equal


class UpdateLoanStatus:
    
    def __init__(self, connection_c, connection_python,database_copy_date, closure_run_date):
        self.connection_c = connection_c
        self.connection_python = connection_python
        self.database_copy_date = database_copy_date
        self.color_options = color_options
        self.closure_run_date = closure_run_date


    """
        UPDATE LOAN STATUS TESTS:
            1-Create 2 installemnt status history [FRA-BLNK]
            2-Create 2 Loan Status History [FRA-BLNK]
            3-Udate Status For Installment
            4-Update Status For Loan
            5-Installments payment status history
    """

    #1-Create 2 installemnt status history [FRA-BLNK]
    def installment_status_history_entries(self):
        try:
            print_colored("Test 1 :: Create 2 installemnt status history [FRA-BLNK]", self.color_options.YELLOW ,bold=True)


            query = f"""
                SELECT "day" , status_type , installment_id , previous_status_id , status_id  
                FROM new_lms_installmentstatushistory nli
                WHERE DATE(nli.created_at) >= {self.closure_run_date}
                order by "day" desc, installment_id desc, status_id desc
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
            print_colored("Test 1 :: Create 2 installemnt status history [FRA-BLNK]", self.color_options.YELLOW ,bold=True)


            query = f"""
                select "day" , status_type , loan_id , previous_status_id , status_id , reversal_order_id  
                from loan_app_loanstatushistroy lal 
                where "day" > '{self.database_copy_date}'
                order by "day" desc, loan_id desc, status_id desc
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


    #4-Update Status For Loan
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
            print_colored("PASS -> ALL LOAN STATUS ARE EQUAL", color=self.color_options.GREEN, bold=True)

        except Exception as e:
            print_colored(f"An error occurred: {e}", color='red', bold=True)


    def test_update_loan_status(self):
        # self.installment_status_history_entries()
        self.update_loan_status()
