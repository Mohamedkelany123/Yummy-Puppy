import psycopg2
from undue_to_due import UndueToDue
from due_to_overdue import DueToOverDue
from database_utils import SQLUtilsService
from colors import *



def main():

    #Put it manually -(Date = 1 Day Before database date)
    closure_before_running_date = '2023-11-24'

    #--------------------------------------------------UNDUE TO DUE--------------------------------------------------
    print_colored("---------------------------------> UNDUE TO DUE ", color_options.PURPLE, bold=True)
    DB_c_url = "postgres://postgres:postgres@192.168.65.203/undue_to_due_c" 
    DB_python = "postgres://postgres:postgres@192.168.65.203/undue_to_due_django"

    connection_c = SQLUtilsService.connect_to_database(DB_c_url)
    connection_python = SQLUtilsService.connect_to_database(DB_python)

    undue_to_due = UndueToDue(connection_c, connection_python, closure_before_running_date)
    undue_to_due.test_undue_to_due()

    connection_c.close()
    connection_python.close()


    #--------------------------------------------------Due TO overdue--------------------------------------------------
    # print_colored("---------------------------------> DUE TO OVERDUE ", color_options.PURPLE, bold=True)

    # DB_c_url = "postgres://postgres:postgres@192.168.65.203/test2_due_to_overdue_c" 
    # DB_python = "postgres://postgres:postgres@192.168.65.203/test2_due_to_overdue_django"

    # connection_c = SQLUtilsService.connect_to_database(DB_c_url)
    # connection_python = SQLUtilsService.connect_to_database(DB_python)


    # due_to_overdue = DueToOverDue(connection_c, connection_python, closure_before_running_date)
    # due_to_overdue.test_due_to_overdue()

    # connection_c.close()
    # connection_python.close()





if __name__ == "__main__":
    main()

   