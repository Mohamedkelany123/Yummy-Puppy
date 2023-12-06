import psycopg2
from undue_to_due import UndueToDue
from due_to_overdue import DueToOverDue
from update_loan_status import UpdateLoanStatus
from database_utils import SQLUtilsService
from marginalization import Marginalization
from colors import *



def main():

    #Put it manually -(Date = DATABASE DUPLICATION DATE)
    database_copy_date = '2023-11-25'
    closure_run_date  = '2023-12-03'

    # #--------------------------------------------------UNDUE TO DUE--------------------------------------------------
    # print_colored("---------------------------------> UNDUE TO DUE ", color_options.PURPLE, bold=True)
    # DB_c_url = "postgres://postgres:postgres@192.168.65.203/1_undue_to_due_c" 
    # DB_python = "postgres://postgres:postgres@192.168.65.203/1_undue_to_due_django"

    # connection_c = SQLUtilsService.connect_to_database(DB_c_url)
    # connection_python = SQLUtilsService.connect_to_database(DB_python)

    # undue_to_due = UndueToDue(connection_c, connection_python, database_copy_date)
    # undue_to_due.test_undue_to_due()

    # connection_c.close()
    # connection_python.close()

    # #--------------------------------------------------Due TO overdue--------------------------------------------------
    # print_colored("---------------------------------> DUE TO OVERDUE ", color_options.PURPLE, bold=True)

    # DB_c_url = "postgres://postgres:postgres@192.168.65.203/2_due_to_overdue_c" 
    # DB_python = "postgres://postgres:postgres@192.168.65.203/2_due_to_overdue_django"

    # connection_c = SQLUtilsService.connect_to_database(DB_c_url)
    # connection_python = SQLUtilsService.connect_to_database(DB_python)


    # due_to_overdue = DueToOverDue(connection_c, connection_python, database_copy_date)
    # due_to_overdue.test_due_to_overdue()

    # connection_c.close()
    # connection_python.close()

    # #--------------------------------------------------UPDATE LOAN STATUS---------------------------------------------
    # print_colored("---------------------------------> UPDATE LOAN STATUS", color_options.PURPLE, bold=True)
    # DB_c_url = "postgres://postgres:postgres@192.168.65.203/3_update_loan_status_c" 
    # DB_python = "postgres://postgres:postgres@192.168.65.203/3_update_loan_status_django"

    # connection_c = SQLUtilsService.connect_to_database(DB_c_url)
    # connection_python = SQLUtilsService.connect_to_database(DB_python)


    # update_loan_status = UpdateLoanStatus(connection_c, connection_python, database_copy_date, closure_run_date)
    # update_loan_status.test_update_loan_status()

    # connection_c.close()
    # connection_python.close()

    #--------------------------------------------------MARGINALIZATION---------------------------------------------
    print_colored("---------------------------------> MARGINALIZATION", color_options.PURPLE, bold=True)
    DB_c_url = "postgres://postgres:postgres@192.168.65.203/4_4_4_marginalization_step_1_django" 
    DB_python = "postgres://postgres:postgres@192.168.65.203/4_4_marginalization_step_1_django"

    connection_c = SQLUtilsService.connect_to_database(DB_c_url)
    connection_python = SQLUtilsService.connect_to_database(DB_python)

    marginalization = Marginalization(connection_c, connection_python, database_copy_date)
    marginalization.test_marginalization()

    connection_c.close()
    connection_python.close()



if __name__ == "__main__":
    main()

   