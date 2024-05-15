#!/bin/bash

# Define variables
LMS_COMMAND="./bin/cmd_services/ledger_closure"
ADDRESS="192.168.65.216"
PORT_NUMBER="5432"
DB_NAME="django_ostaz_30042024_omneya"
USERNAME="postgres"
PASSWORD="8ZozYD6DhNJgW7a"
STEP="full_closure"
DATE="2024-05-15"
THREADS_COUNT="1"

# mod 2 offset(0,1) ....
MOD="0"
OFFSET="0"

LOANIDS=""



# Build the command
COMMAND="$LMS_COMMAND $ADDRESS $PORT_NUMBER $DB_NAME $USERNAME $PASSWORD $STEP $DATE $THREADS_COUNT $MOD $OFFSET $LOANIDS"

# Run the command
echo $COMMAND
time $COMMAND

