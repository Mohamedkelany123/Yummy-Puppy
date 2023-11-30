from database_utils import SQLUtilsService
from colors import *
import polars as pl
import pandas as pd
from polars.testing import assert_frame_equal


class DueToOverDue:
    
    def __init__(self, connection_c, connection_python,closure_before_running_date):
        self.connection_c = connection_c
        self.connection_python = connection_python
        self.closure_before_running_date = closure_before_running_date
        self.color_options = color_options


    """
        DUE TO OVERDUE TESTS:
            1-NLI [new_lms_installment_extention] Payment status changed to 0 
            2-Changes LMS Closure Status Due To Over Due 1->2
            3-Creates entry for Installment status payment history entries
            4-Create late fees object -> Excluding [amount, created_at, updated_at]  
    """

    """
        Late Fees Amount  -> In django the late fees are calculated twice. 1st time while creating the installment extension, second 
        time is when creating the late fee. Both ways are different. In C we get the late fees from the installment extension. So the 
        values differ sometimes within a range of 0.01->0.72 EGP, Ramy Told me to keep using the same way which is getting the value 
        from installment extension because its more accurate as it uses decimal.

        --> WHAT I DID IS CHANGED IN DJANGO CODE TO SET THE LATE FEES AMOUNT WITH THE INSTALLMENTEXTENTION LATEFEES AMOUNT IN ORDER TO 
            BE ABLE TO TEST CORRECTLY
    """

     #1-NLI Payment status changed to 0 (COUNT)
    def installment_extention(self, payment_status):
        try:
            print_colored("Test 1 :: Compare Count Of Installments Extentions with payment status = 0", self.color_options.YELLOW ,bold=True)
            query = f"SELECT COUNT(*) FROM new_lms_installmentextension WHERE payment_status = {payment_status}"

            # C++
            data_c = SQLUtilsService.execute_query(self.connection_c, query)
            # print_colored(f"Results from DB_c (Payment Status = {payment_status}):", self.color_options.BLUE ,bold=True)
            # print(data_c)

            # PYTHON
            data_python = SQLUtilsService.execute_query(self.connection_python, query)
            # print_colored(f"Results from DB_python (Payment Status = {payment_status}):",self.color_options.BLUE,  bold=True)
            # print(data_python)

            # Compare the results
            if data_c == data_python:
                print_colored(f"PASS -> Installments Extension to (payment status){payment_status}",color=self.color_options.GREEN , bold=True)
                # Continue with the next steps
            else:
                print_colored("FAIL -> There is a problem in the count between both databases.", self.color_options.RED, bold=True)
                print_colored("DB_c result:", data_c)
                print("DB_python result:", data_python)

        except Exception as e:
                print_colored(f"An error occurred: {e}", color='red', bold=True)

    #2-Changes LMS Closure Status Due To Over Due 1->2
    def due_to_due_overdue_closure_status(self):
        try:
            print_colored("Test 2 :: Changes LMS Closure Status Due To Over Due 1->2", self.color_options.YELLOW ,bold=True)

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
            print_colored("PASS -> Update loan closure status -undue to -due ",color=self.color_options.GREEN , bold=True)

        except Exception as e:
                print_colored(f"An error occurred: {e}", self.color_options.RED, bold=True)


    #3-Creates entry for Installment status payment history entries 
    def installment_payment_history_entries(self):
        try:
            print_colored("Test 3 :: Creates entry for Installment status payment history", self.color_options.YELLOW ,bold=True)
            # print_colored("Test 3 :: DINT FORGET TO CHECK THE CREATED_AT AND UPDATED_AT DATES IF THEY ARE CREATED CORRECTLY", self.color_options.RED ,bold=True)


            query = f"select status, installment_extension_id , coalesce(order_id,-1) as order_id  , \"day\" from new_lms_installmentpaymentstatushistory nli where day > '{self.closure_before_running_date}' order by  installment_extension_id desc,  \"day\" desc, status desc"

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



    #4-Create late fees object
    def late_fees_objects(self):
        try:
            print_colored("Test 4 :: Create late fees object", self.color_options.YELLOW ,bold=True)
            # print_colored("Test 4 :: DONT FORGET TO CHECK THE CREATED_AT AND UPDATED_AT DATES IF THEY ARE CREATED CORRECTLY", self.color_options.RED ,bold=True)

            query = f"""select *
                        from new_lms_installmentlatefees nli 
                        where day > '{self.closure_before_running_date}' order by installment_extension_id  desc, \"day\" desc, installment_status_id desc
                    """
            # C++
            data_c = SQLUtilsService.execute_query(self.connection_c, query)
            # print_colored("Results from DB_c(Installment Payment History):", self.color_options.BLUE, bold=True)
            # print(data_c[0])

            # PYTHON
            data_python = SQLUtilsService.execute_query(self.connection_python, query)
            # print_colored("Results from DB_python(Installment Payment History):", self.color_options.BLUE, bold=True)
            # print(data_python[0])


            # Remove "created_at" and "updated_at" from the data
            excluded_columns = ['created_at', 'updated_at', 'id']
            data_c_filtered = [ {k: v for k, v in row.items() if k not in excluded_columns} for row in data_c]
            data_python_filtered = [ {k: v for k, v in row.items() if k not in excluded_columns} for row in data_python]
            print_colored("----------> FILTERED DATA", self.color_options.YELLOW, bold=True)

            df_c = pl.DataFrame(data_c_filtered)
            df_python = pl.DataFrame(data_python_filtered)
            # df_python.write_csv("python.csv")

            # Compare the results
            assert_frame_equal(df_c, df_python)
            print_colored("PASS -> Data of Installment Payment History", color=self.color_options.GREEN, bold=True)

        except Exception as e:
            print_colored(f"An error occurred: {e}", color='red', bold=True)


    def test_due_to_overdue(self):
        self.installment_extention(0)
        self.due_to_due_overdue_closure_status()
        self.installment_payment_history_entries()
        self.late_fees_objects()


