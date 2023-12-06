from database_utils import SQLUtilsService
from colors import *
import polars as pl
import pandas as pd
from polars.testing import assert_frame_equal


class Marginalization:
    
    def __init__(self, connection_c, connection_python,database_copy_date):
        self.connection_c = connection_c
        self.connection_python = connection_python
        self.database_copy_date = database_copy_date
        self.color_options = color_options



    """
        Marginalization tests:
        
        1- LMS Closure status 3->4
        2-  Step-1 -> 
                Installment Extention:
                    -is_partially_marginalized
                    -partial_marginalization_date
            Step-2 ->
                Installment Extention:
                    - is_marginalized 
                    -marginalization_date
        3- Step-3 ->
                Installment Late Fees:
                    -is_marginalized
                    -marginalization_date
    """


    #1- LMS Closure status 3->4
    def update_loan_status_closure_status(self):
        try:
            print_colored("Test 1 :: Changes LMS Closure Status Due To Over Due 3->4", self.color_options.YELLOW ,bold=True)

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

    """
        2-  Step-1 -> 
                Installment Extention:
                    -is_partially_marginalized
                    -partial_marginalization_date
            Step-2 ->  [FAILED]
                Installment Extention:
                    - is_marginalized 
                    -marginalization_date
    """
    def update_installment_extention_partial(self):
        try:
            print_colored("Test 2 :: Step-1 Partial Step-2 Full", self.color_options.YELLOW ,bold=True)

            query = f"""
                SELECT
                    installment_ptr_id,
                    is_partially_marginalized,
                    COALESCE(partial_marginalization_date, '2001-11-30') AS partial_marginalization_date,
                    is_marginalized,
                    COALESCE(marginalization_date, '2001-11-30') AS marginalization_date
                FROM
                    new_lms_installmentextension nli
                ORDER BY
                    installment_ptr_id DESC;
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
            print_colored("PASS -> Step-1 Partial Step-2 Full",color=self.color_options.GREEN , bold=True)

        except Exception as e:
                print_colored(f"An error occurred: {e}", self.color_options.RED, bold=True)
                

    """
        3- Step-3 ->
        Installment Late Fees:
            -is_marginalized
            -marginalization_date
    """
    def update_late_fees(self):
        try:
            print_colored("Test 3 :: Step-3 Late Fees", self.color_options.YELLOW ,bold=True)

            query = f"""
                select 
                    is_marginalized, 
                    COALESCE(marginalization_date, '2001-11-30') AS marginalization_date
                from 
                    new_lms_installmentlatefees nli
                order by id desc   
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
            print_colored("PASS -> Step-3 Late Fees",color=self.color_options.GREEN , bold=True)

        except Exception as e:
                print_colored(f"An error occurred: {e}", self.color_options.RED, bold=True)

    def test_marginalization(self):
        self.update_loan_status_closure_status()
        self.update_installment_extention_partial()
        self.update_late_fees()