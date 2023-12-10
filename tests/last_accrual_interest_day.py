from database_utils import SQLUtilsService
from colors import *
import polars as pl
import pandas as pd
from polars.testing import assert_frame_equal


class LastAccrualInterestDay:
    
    def __init__(self, connection_c, connection_python,database_copy_date):
        self.connection_c = connection_c
        self.connection_python = connection_python
        self.database_copy_date = database_copy_date
        self.color_options = color_options


    """
        Last Accrual Interest Date:

            1- LMS Closure Status 5->6
            2- Loan last_accrued_interest_day, lms_closure_status

    """


    #1-LMS Closure status 5->6
    def update_loan_status_closure_status(self):
        try:
            print_colored("Test 1 :: Changes LMS Closure status 5->6", self.color_options.YELLOW ,bold=True)

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



    #2- Loan last_accrued_interest_day, lms_closure_status
    def update_loan(self):
        try:
            print_colored("Test 2 :: Loan last_accrued_interest_day, lms_closure_status", self.color_options.YELLOW ,bold=True)

            query = f"""
                select 
                    id,
                    last_accrued_interest_day
                from 
                    loan_app_loan lal 
                order by 
                    id desc
            """

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
            print_colored("PASS -> Loan last_accrued_interest_day, lms_closure_status ",color=self.color_options.GREEN , bold=True)

        except Exception as e:
                print_colored(f"An error occurred: {e}", self.color_options.RED, bold=True)
    


    # def execute(self, query):
        


    def test_last_accrual_interest_day(self):
        self.update_loan_status_closure_status()
        self.update_loan()