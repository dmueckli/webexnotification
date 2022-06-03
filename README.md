# Webex notification script for Icinga2

A simple webex notification script for nagios / icinga2 written in c++.

Please note that I am currently taking my first steps in programming through projects like this. 

Advice and contributions are welcome. 

## Usage 

1. Install dependencies: libcurlpp-dev (debian/ubuntu)
2. Compile the main.cpp file: 

        g++ main.cpp -l curlpp -l curl -l pthread -o main

Arguments:

                --token, -t **required**
                --type, -T **required**
                --description, -d **required**
                --summary, -e **required**
                --host, -h **required**
                --service, -s 
                --state, -S **required**
                --roomId, -r **required**

Example:

    ./main --type PROBLEM --token YOURWEBEXBOTTOKEN --description 'apt on Icinga Master is WARNING' --summary 'APT WARNING: 1 packages available for upgrade (0 critical updates).' --host 'Icinga Master' --state WARNING --roomId YOURWEBEXROOMID

Output:

    [PROBLEM]: apt on Icinga Master is WARNING
    APT WARNING: 1 packages available for upgrade (0 critical updates).
