# Webex notification script for Icinga2

A simple webex notification script for nagios / icinga2 written in c++.

Please note that I am currently taking my first steps in programming through projects like this. 

Advice and contributions are welcome. 

## Usage 

1. Install dependencies:
 
        libcurlpp-dev (debian/ubuntu) or https://github.com/jpbarrette/curlpp
        
2. Compile the main.cpp file: 

        g++ main.cpp -l curlpp -l curl -l pthread -o main
        
3. Create notification command directly via the director or by manually adding a new command template to icinga.

Arguments:

                --token, -t required            // Webex Bot token (See https://developer.webex.com/docs/bots for more information)
                --type, -T required             // Notification Type, e.g. PROBLEM or RECOVERY
                --description, -d required      // e.g. the plugin output: CRITICAL: Interface is 'down' (interface: 2 status: 2) !!! You need to set this in escaped double quotes when sending to the script \"$service.output$\" !!!
                --summary, -e                   // e.g. SERVICE on HOST is CRITICAL, or HOST is DOWN
                --host, -h required             // host
                --hostname, -h required         // host display name
                --service, -s                   // service name
                --state, -S required            // service state
                --roomId, -r required           // Webex RoomId to send the message to
                --icingaUrl, -h required        // Url for linking back to icinga, currently only supporting icingadb

Example:

    ./main --type PROBLEM --token WEBEXTOKEN --description "\"Icinga Director: there are problems

    Director configuration: 3 tests OK
    [OK] Database resource 'Director DB' has been specified
    [OK] Make sure the DB schema exists
    [OK] There are no pending schema migrations

    Director Deployments: 3 tests OK
    [OK] Deployment endpoint is 'icinga.example.com'
    [OK] There are 0 un-deployed changes
    [CRITICAL] The last Deployment failed 0m 32s ago

    Import Sources: 1 tests OK
    [OK] No Import Sources have been defined

    Sync Rules: 1 tests OK
    [OK] No Sync Rules have been defined

    Director Jobs: 1 tests OK
    [OK] No Jobs have been defined\"" --state CRITICAL --roomId WEBEXROOMID --icingaUrl 'https://icinga.example.com' --hostname 'Icinga Master' --service 'director' --host 'muecklich.com'

Output:

    [PROBLEM]: director on Icinga Master is CRITICAL 
    Icinga Director: there are problems    
    
    Director configuration: 3 tests OK 
    🟢 Database resource 'Director DB' has been specified 
    🟢 Make sure the DB schema exists 
    🟢 There are no pending schema migrations 
    
    Director Deployments: 3 tests OK 
    🟢 Deployment endpoint is 'icinga.example.com' 
    🟢 There are 0 un-deployed changes 
    🔴 The last Deployment failed 0m 32s ago
    
    Import Sources: 1 tests OK 
    🟢 No Import Sources have been defined 
    
    Sync Rules: 1 tests OK 
    🟢 No Sync Rules have been defined 
    
    Director Jobs: 1 tests OK 
    🟢 No Jobs have been defined
