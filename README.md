# Webex notification script for Icinga2

A simple webex notification script for nagios / icinga2 written in c++.

Please note that I am currently taking my first steps in programming through projects like this. 

Advice and contributions are welcome. 

## Usage 

1. Install dependencies: libcurlpp-dev (debian/ubuntu)
2. Compile the main.cpp file: g++ main.cpp -l curlpp -l curl -l pthread -o main

                {"token", required_argument, 0, 't'},
                {"type", required_argument, 0, 'T'},
                {"description", required_argument, 0, 'd'},
                {"summary", required_argument, 0, 'e'},
                {"host", required_argument, 0, 'h'},
                {"service", required_argument, 0, 's'},
                {"state", required_argument, 0, 'S'},
                {"roomId", required_argument, 0, 'r'},

Example:

    ./main --type PROBLEM --token YOURWEBEXBOTTOKEN --description 'apt on Icinga Master is WARNING' --summary 'APT WARNING: 1 packages available for upgrade (0 critical updates).' --host 'Icinga Master' --state WARNING --roomId YOURWEBEXROOMID

Output:

    [PROBLEM]: apt on Icinga Master is WARNING
    APT WARNING: 1 packages available for upgrade (0 critical updates).
