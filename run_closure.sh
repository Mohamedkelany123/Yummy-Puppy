#!/bin/bash

# Define variables
LMS_COMMAND="./bin/cmd_services/new_lms"
ADDRESS="192.168.1.51"
PORT_NUMBER="5432"
DB_NAME="c_plus_plus"
USERNAME="postgres"
PASSWORD="postgres"
STEP="full_closure"
DATE="2024-04-24"
THREADS_COUNT="1"

# mod 2 offset(0,1) ....
MOD="0"
OFFSET="0"

LOANIDS="105050"



# Build the command
COMMAND="$LMS_COMMAND $ADDRESS $PORT_NUMBER $DB_NAME $USERNAME $PASSWORD $STEP $DATE $THREADS_COUNT $MOD $OFFSET $LOANIDS"

# Run the command
time $COMMAND