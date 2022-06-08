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

                --token, -t required            // String: Webex Bot token (See https://developer.webex.com/docs/bots for more information)
                --type, -T required             // String: Notification Type, e.g. PROBLEM or RECOVERY
                --description, -d required      // String: e.g. the plugin output: CRITICAL: Interface is 'down' (interface: 2 status: 2) 
                --summary, -e                   // String: e.g. SERVICE on HOST is CRITICAL, or HOST is DOWN
                --host, -h required             // String: host name
                --hostname, -h required         // String: host display name
                --service, -s                   // String: service name, if existent
                --state, -S required            // String: service state
                --roomId, -r required           // String: Webex RoomId to send the message to
                --icingaUrl, -h required        // String: Url for linking back to icinga, set '--monitoring' for monitoring backend
                --monitoring                    // Flag: Set this when using icingaweb monitoring backend
                --author, -a                    // String: notification author, if existent
                --comment, -c                   // String: notification comment, if existent

Example:

    ./main --type "PROBLEM" --token WEBEXTOKEN --description "Icinga Director: there are problems

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
    [OK] No Jobs have been defined" --state "CRITICAL" --roomId WEBEXROOMID --icingaUrl "https://icinga.example.com" --hostname "Icinga Master" --service "director" --host "icinga.example.com"

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
