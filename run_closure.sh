#!/bin/bash

# Define variables
LMS_COMMAND="./bin/mains/cmd_services/new_lms"
ADDRESS="192.168.1.51"
# ADDRESS="localhost"
PORT_NUMBER="5432"
DB_NAME="django_ostaz_before_closure"
# DB_NAME="django_ostaz_25102023"
USERNAME="postgres"
PASSWORD="postgres"
#STEP="full_closure"
STEP="due_to_overdue"
DATE="2024-06-11"
THREADS_COUNT="16"

# mod 2 offset(0,1) ....
MOD="0"
OFFSET="0"

LOANIDS=""



# Build the command
COMMAND="$LMS_COMMAND $ADDRESS $PORT_NUMBER $DB_NAME $USERNAME $PASSWORD $STEP $DATE $THREADS_COUNT $MOD $OFFSET $LOANIDS"

echo $COMMAND
# Run the command
#time (($COMMAND)| grep "PROCESSTIME due_to_overdue")
time $COMMAND
