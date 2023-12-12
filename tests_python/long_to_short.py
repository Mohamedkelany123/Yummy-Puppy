from database_utils import SQLUtilsService
from colors import *
import polars as pl
import pandas as pd
from polars.testing import assert_frame_equal


class LongToShort:
    
    def __init__(self, connection_c, connection_python,database_copy_date):
        self.connection_c = connection_c
        self.connection_python = connection_python
        self.database_copy_date = database_copy_date
        self.color_options = color_options


    """
        Long To Short Tests:
            
            1-LMS Closure status 4->5
            2-Installment Extention is_long_term -> FALSE (Installment in 12 months)
    """


    #1-LMS Closure status 4->5
    def update_loan_status_closure_status(self):
        try:
            print_colored("Test 1 :: Changes LMS Closure status 4->5", self.color_options.YELLOW ,bold=True)

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
    

    #2-Installment Extention is_long_term -> false (Installment in 12 months)
    def update_installment_extention_is_long_term(self):
        try:
            print_colored("Test 2 :: Installment Extention is_long_term -> FALSE (Installment in 12 months)", self.color_options.YELLOW ,bold=True)

            query = f"""
                select 
                    installment_ptr_id , 
                    is_long_term  
                from 
                    new_lms_installmentextension nli 
                order by 
                    installment_ptr_id desc
            """

            # C++
            data_c = SQLUtilsService.execute_query(self.connection_c, query)
            # print_colored("Results from DB_c (Due To OverDue):", self.color_options.BLUE ,bold=True)
            # print(data_c[0]['installment_ptr_id'] )

            # PYTHON
            data_python = SQLUtilsService.execute_query(self.connection_python, query)
            # print_colored("Results from DB_python (Due To OverDue):",self.color_options.BLUE,  bold=True)
            # print(data_python[0]['is_long_term'])


            # for i in range(len(data_c)):
            #     if(data_c[i]['is_long_term'] != data_python[i]['is_long_term']):
            #         print(data_c[i]['installment_ptr_id'])

            df_c = pl.DataFrame(data_c)
            df_python = pl.DataFrame(data_python)
            
            assert_frame_equal(df_c, df_python)
            print_colored("PASS -> Installment Extention is_long_term  ",color=self.color_options.GREEN , bold=True)

        except Exception as e:
                print_colored(f"An error occurred: {e}", self.color_options.RED, bold=True)






    def test_long_to_short(self):
        self.update_loan_status_closure_status()
        self.update_installment_extention_is_long_term()