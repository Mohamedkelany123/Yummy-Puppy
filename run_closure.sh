#!/bin/bash

# Define variables
LMS_COMMAND="./bin/mains/cmd_services/new_lms"
ADDRESS="192.168.65.216"
# ADDRESS="localhost"
PORT_NUMBER="5432"
DB_NAME="django_ostaz_23042024_aliaclosure"
# DB_NAME="django_ostaz_25102023"
USERNAME="development"
PASSWORD="5k6MLFM9CLN3bD1"
#STEP="full_closure"
STEP="full_closure"
DATE="2024-06-24"
THREADS_COUNT="6"

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
