from dotenv import load_dotenv
from due_to_overdue import DueToOverDue
from database_utils import SQLUtilsService
from full_closure import FullClosure
from ledger_closure import LedgerClosure
from colors import *
import os
from dotenv import load_dotenv
from datetime import datetime

load_dotenv()

def main():

    DB_c_url = os.environ.get('DB_c_url')
    DB_python = os.environ.get('DB_python')

    connection_c = SQLUtilsService.connect_to_database(DB_c_url)
    connection_python = SQLUtilsService.connect_to_database(DB_python)

    print_colored("Database 1: ", color_options.CYAN, bold=True)
    print_colored(DB_c_url, color_options.PURPLE, bold=True)
    print_colored("Database 2: ", color_options.CYAN, bold=True)
    print_colored(DB_python, color_options.PURPLE, bold=True)
    print_colored("------------------------------------------------------ ", color_options.PURPLE, bold=True)
    
    #--------------------------------------------------Ledger Closure Tests------------------------------------------
    
    print_colored("--------------------------------->LEDGER CLOSURE TESTS ", color_options.PURPLE, bold=True)

    # ledgerClosure = LedgerClosure(connection_c, connection_python)
    # ledgerClosure.test()

    #--------------------------------------------------Full Closure Tests------------------------------------------
    
    print_colored("--------------------------------->FULL CLOSURE TESTS ", color_options.PURPLE, bold=True)
    full_closure = FullClosure(connection_c, connection_python)
    full_closure.test()



    connection_c.close()
    connection_python.close()


if __name__ == "__main__":
    main()

   