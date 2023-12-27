from datetime import datetime


class color_options:
    PURPLE = '\033[95m'
    CYAN = '\033[96m'
    DARKCYAN = '\033[36m'
    BLUE = '\033[94m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    END = '\033[0m'


current_date = datetime.now().strftime("%Y%m%d")
LOG_FILE = f'/home/kmsobh/closure_logs/test_logs_{current_date}.txt'

def print_colored(msg: str, color: str = color_options.GREEN, bold: bool = False):
    if bold:
        colored_msg = color_options.BOLD + color + msg + color_options.END
    else:
        colored_msg = color + msg + color_options.END

    # Print to console
    print(colored_msg)

    # Write to log file
    with open(LOG_FILE, 'a') as log_file:
        log_file.write(msg + '\n')

