from database_utils import SQLUtilsService
from colors import *
import polars as pl
import pandas as pd
from polars.testing import assert_frame_equal
import tqdm


class FullClosure:
    
    def __init__(self, connection_c, connection_python,database_copy_date):
        self.connection_c = connection_c
        self.connection_python = connection_python
        self.database_copy_date = database_copy_date
        self.color_options = color_options

    def test(self):
        self.installment_extentions()
        self.loan_app_loan()
        self.installment()
        self.late_fees()
        self.installment_payment_status_history()
        self.installment_status_history()
        self.loan_status_history()


    """
        Tables To Test:

            1-Installment Extention Excluding[ext_created_at , ext_updated_at]
    [FAILED]2-Loan App Loan Excluding[created_at, updated_at]
            3-Installment Excluding[created_at, updated_at]
            4-Installment Late Fees Exclude-ID-(Step-4 Marginalization Creates Late Fees So We cant Compare the [ids] as C++ runs using Multithreads)
            5-Installment Payment Status History Exclude
            6-Installment Status History 
            7-Loan Status History
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
        excluded_columns = ['ext_created_at', 'ext_updated_at']
        self.exec(query, excluded_columns)

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
        # excluded_columns = ['created_at', 'updated_at', "last_lms_closing_day"]
        excluded_columns = ['created_at', 'updated_at']
        # excluded_columns = ['created_at', 'updated_at']

        self.exec(query, excluded_columns)


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
        self.exec(query, excluded_columns)



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
        excluded_columns = ['id', 'created_at', 'updated_at']
        self.exec(query, excluded_columns)



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
        excluded_columns = ['id', 'created_at', 'updated_at']

        self.exec(query, excluded_columns)


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
        self.exec(query, excluded_columns)



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
        self.exec(query, excluded_columns)










#---------------------------------------------------------------------------------------------------------------------------------------------------#
    def exec(self, query ,excluded_columns=None):
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
                raise ValueError(f"Data LENGTHS ARE NOT EQUAL")

            # counter = 0

            for i in tqdm.tqdm(range(len(data_c_filtered)), desc="Comparing Data"):
                        if str(data_c_filtered[i]) != str(data_python_filtered[i]):
                            # counter += 1
                            print_colored("Data C++:", self.color_options.RED, bold=True)
                            print_colored(str(data_c_filtered[i]), self.color_options.BLUE, bold=True)
                            print_colored("Data Python:", self.color_options.RED, bold=True)
                            print_colored(str(data_python_filtered[i]), self.color_options.CYAN, bold=True)
                            print(f"Data mismatch at index {i}")
                            raise ValueError(f"Data mismatch at index {i}")
            # print("--------------------------------------------------COUNTER:", counter)
            print_colored("-----------------------------------PASS---------------------------------", color=self.color_options.GREEN, bold=True)

        except Exception as e:
            print_colored(f"FAIL ---> An error occurred: {e}", color_options.RED, bold=True)


