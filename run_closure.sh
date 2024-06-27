#!/bin/bash

#DATABASE_URL=postgres://development:5k6MLFM9CLN3bD1@192.168.65.216:5432/django_ostaz_22062024_omneya


# Define variables
LMS_COMMAND="./bin/mains/cmd_services/new_lms"
ADDRESS="192.168.1.51"
PORT_NUMBER="5432"
DB_NAME="django_closure"
USERNAME="postgres"
PASSWORD="postgres"
STEP="full_closure"
DATE="2024-07-02"
THREADS_COUNT="8"

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
