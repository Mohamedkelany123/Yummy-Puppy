from undue_to_due import UndueToDue
from due_to_overdue import DueToOverDue
from update_loan_status import UpdateLoanStatus
from database_utils import SQLUtilsService
from long_to_short import LongToShort
from last_accrual_interest_day import LastAccrualInterestDay
from marginalization import Marginalization
from full_closure import FullClosure
from colors import *



def main():

    #PUT IT MANUALLY-(Date = DATABASE DUPLICATION DATE) 
    #FOR ALL EXCEPT FULL CLOSURE
    database_copy_date = '2023-11-25'
    
    #------------------------------------------------------------------------------------------------------------------------------------------------------------#
    #------------------------------------------------------------------------------------------------------------------------------------------------------------#
    #------------------------------------------------------------------------------------------------------------------------------------------------------------#
    #--------------------------------------------------Full Closure Tests------------------------------------------
    

    #TOOK 24 MINUTES TO TEST ALL 7 TABLES
    print_colored("--------------------------------->FULL CLOSURE TESTS ", color_options.PURPLE, bold=True)
    DB_c_url = "postgres://postgres:postgres@192.168.65.203/kemosparc_2" 
    DB_python = "postgres://postgres:postgres@192.168.65.203/7_full_closure_django"
    
    print_colored("Database 1: ", color_options.CYAN, bold=True)
    print_colored(DB_c_url, color_options.PURPLE, bold=True)
    print_colored("Database 2: ", color_options.CYAN, bold=True)
    print_colored(DB_python, color_options.PURPLE, bold=True)
    print_colored("------------------------------------------------------ ", color_options.PURPLE, bold=True)

    connection_c = SQLUtilsService.connect_to_database(DB_c_url)
    connection_python = SQLUtilsService.connect_to_database(DB_python)

    full_closure = FullClosure(connection_c, connection_python, database_copy_date)
    full_closure.test()

    connection_c.close()
    connection_python.close()

    #------------------------------------------------------------------------------------------------------------------------------------------------------------#
    #------------------------------------------------------------------------------------------------------------------------------------------------------------#
    #------------------------------------------------------------------------------------------------------------------------------------------------------------#
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

    # # #--------------------------------------------------UPDATE LOAN STATUS---------------------------------------------
    # print_colored("---------------------------------> UPDATE LOAN STATUS", color_options.PURPLE, bold=True)
    # DB_c_url = "postgres://postgres:postgres@192.168.65.203/3_update_loan_status_c" 
    # DB_python = "postgres://postgres:postgres@192.168.65.203/3_update_loan_status_django"

    # connection_c = SQLUtilsService.connect_to_database(DB_c_url)
    # connection_python = SQLUtilsService.connect_to_database(DB_python)

    # update_loan_status = UpdateLoanStatus(connection_c, connection_python, database_copy_date)
    # update_loan_status.test_update_loan_status()

    # connection_c.close()
    # connection_python.close()


    # #--------------------------------------------------MARGINALIZATION---------------------------------------------
    # print_colored("---------------------------------> MARGINALIZATION", color_options.PURPLE, bold=True)
    # DB_c_url = "postgres://postgres:postgres@192.168.65.203/4_marginalization_c" 
    # DB_python = "postgres://postgres:postgres@192.168.65.203/4_marginalization_django"


    # connection_c = SQLUtilsService.connect_to_database(DB_c_url)
    # connection_python = SQLUtilsService.connect_to_database(DB_python)

    # marginalization = Marginalization(connection_c, connection_python, database_copy_date)
    # marginalization.test_marginalization()

    # connection_c.close()
    # connection_python.close()

    # #--------------------------------------------------Long To Short---------------------------------------------
    # print_colored("---------------------------------> Long To Short", color_options.PURPLE, bold=True)
    # DB_c_url = "postgres://postgres:postgres@192.168.65.203/5_long_to_short_c" 
    # DB_python = "postgres://postgres:postgres@192.168.65.203/5_long_to_short_django"


    # connection_c = SQLUtilsService.connect_to_database(DB_c_url)
    # connection_python = SQLUtilsService.connect_to_database(DB_python)

    # long_to_short = LongToShort(connection_c, connection_python, database_copy_date)
    # long_to_short.test_long_to_short()

    # connection_c.close()
    # connection_python.close()
   
   
    # #--------------------------------------------------Last Accrual Interest Date------------------------------------
    # print_colored("---------------------------------> Last Accrual Interest Date", color_options.PURPLE, bold=True)
    # DB_c_url = "postgres://postgres:postgres@192.168.65.203/6_last_accrual_interest_date_c" 
    # DB_python = "postgres://postgres:postgres@192.168.65.203/6_last_accrual_interest_date_django"


    # connection_c = SQLUtilsService.connect_to_database(DB_c_url)
    # connection_python = SQLUtilsService.connect_to_database(DB_python)

    # last_accrual_interest_day = LastAccrualInterestDay(connection_c, connection_python, database_copy_date)
    # last_accrual_interest_day.test_last_accrual_interest_day()

    # connection_c.close()
    # connection_python.close()









if __name__ == "__main__":
    main()

   