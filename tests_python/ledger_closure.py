from database_utils import SQLUtilsService
from colors import *
import polars as pl
import pandas as pd
from polars.testing import assert_frame_equal
import tqdm

from slack_notify import send_slack_message
from datetime import datetime

class LedgerClosure:
    
    def __init__(self, connection_c, connection_python):
        self.connection_c = connection_c
        self.connection_python = connection_python
        self.color_options = color_options

        self.webhook_url = "https://hooks.slack.com/services/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"
        self.current_date = datetime.now().strftime("%Y-%m-%d")


    def test(self):
        # send_slack_message(self.webhook_url, f"*STARTING LMS CLOSURE STATUS TESTS: {str(self.current_date)}* ", "#0000FF")
        self.ledger_amount()
        self.ledger_entry()
        # send_slack_message(self.webhook_url, f"*FINISHED LMS CLOSURE STATUS TESTS: {str(self.current_date)}* ", "#0000FF")



    #1- ledger amount
    def ledger_amount(self):
        print_colored("Test 1:: Ledger Amount ", self.color_options.YELLOW ,bold=True)
        query = f"""
                    select * from ledger_amount la 
                    where created_at::date >= '{self.current_date}' 
                    order by loan_id desc, amount desc, leg_temple_id desc, account_id, cashier_id, customer_id desc
                """
        excluded_columns = ['id', 'created_at', 'updated_at', 'entry_id']
        compare_amount_attributes = ['amount', 'amount_local']
        temp = self.exec(query, excluded_columns, "Ledger Amount", compare_amount_attributes)
        # if temp:
        #     send_slack_message(self.webhook_url, "PASS -> Ledger Amount ", "#00FF00")


    #2- ledger entries
    def ledger_entry(self):
        print_colored("Test 2 :: Ledger Entry ", self.color_options.YELLOW ,bold=True)
        query = f"""
                    select * from ledger_entry le 
                    where created_at::date >= '{self.current_date}'
                    order by entry_date  desc, template_id  desc
                """
        excluded_columns = [ 'id', 'created_at', 'updated_at', 'description']
        compare_amount_attributes = []
        temp = self.exec(query, excluded_columns, "Ledger Entry", compare_amount_attributes)
        # if temp:
        #     send_slack_message(self.webhook_url, "PASS -> Ledger Entry ", "#00FF00")






#---------------------------------------------------------------------------------------------------------------------------------------------------#
    def exec(self, query ,excluded_columns=None, tableName = "", compare_amount_attributes=[]):
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
            if len(compare_amount_attributes) > 0:
                data_c, data_python = self.compare_amounts(data_c=data_c, data_python=data_python, columns_to_check=compare_amount_attributes)

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

            print ("length of c is ", len(data_c_filtered)," \n length of django is ", len(data_python_filtered))
            if len(data_c_filtered) != len(data_python_filtered):
                # send_slack_message(self.webhook_url, f"FAILED -> {tableName} ", "#FF0000")
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
                # send_slack_message(self.webhook_url, f"FAILED -> {tableName} ", "#FF0000")
                return False
            else:
                print_colored(str(counter), color=self.color_options.GREEN, bold=True)
                print_colored("-----------------------------------PASS---------------------------------", color=self.color_options.GREEN, bold=True)
                return True

        except Exception as e:
            print_colored(f"FAIL ---> An error occurred: {e}", color_options.RED, bold=True)



    def compare_amounts(self, data_c, data_python, columns_to_check=None):
        """
        Compares specified columns between two datasets and removes the entry if the difference is not more than 0.1.

        :param data_c: List of dictionaries representing data fetched from C++ database.
        :param data_python: List of dictionaries representing data fetched from Python database.
        :param columns_to_check: List of column names to check for differences.
        :return: Filtered data_c and data_python lists.
        """
        print("Inside compare amounts.............")
        if not data_c or not data_python:
            return data_c, data_python

        if columns_to_check is None:
            columns_to_check = []

        # Iterate over the data to compare specified column values
        filtered_data_c = []
        filtered_data_python = []

        for row_c, row_python in zip(data_c, data_python):
            keep_row = False
            for column in columns_to_check:
                if column in row_c and column in row_python:
                    if float(abs(row_c[column] - row_python[column])) > float(0.01):
                        keep_row = True
                        break
            
            if keep_row:
                filtered_data_c.append(row_c)
                filtered_data_python.append(row_python)
            else:
                for column in columns_to_check:
                    row_c.pop(column, None)
                    row_python.pop(column, None)
                filtered_data_c.append(row_c)
                filtered_data_python.append(row_python)

        return filtered_data_c, filtered_data_python