#include <fstream>
#include <iostream>
#include <sstream>
#include <future>
#include <thread>
#include <getopt.h>
#include <stdlib.h>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>

/* Flag set by ‘--verbose’. */
static int verbose_flag;

void replaceAll(std::string &str, const std::string &from, const std::string &to)
{
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

std::future<std::string> invoke(std::string const &url, std::string const &body, std::string const &token)
{
    std::future<std::string> rData = std::async(
        std::launch::async,
        [&token](std::string const &url, std::string const &body) mutable
        {
            std::list<std::string> header;
            header.push_back("Authorization: Bearer " + token);
            header.push_back("Content-Type: application/json");
            header.push_back("Accept: application/json");

            curlpp::Cleanup clean;
            curlpp::Easy r;
            r.setOpt(new curlpp::options::Url(url));
            r.setOpt(new curlpp::options::HttpHeader(header));
            r.setOpt(new curlpp::options::PostFields(body));
            r.setOpt(new curlpp::options::PostFieldSize(body.length()));

            std::ostringstream response;
            r.setOpt(new curlpp::options::WriteStream(&response));

            r.perform();

            // other way to retreive URL
            std::cout << std::endl
                      << "Effective URL: "
                      << curlpp::infos::EffectiveUrl::get(r)
                      << std::endl;

            std::cout << "Response code: "
                      << curlpp::infos::ResponseCode::get(r)
                      << std::endl;

            const char responseStatusCode = curlpp::infos::ResponseCode::get(r);
            const char *responseBody = response.str().c_str();

            return std::string(response.str());
        },
        url, body);

    return rData;
}

int main(int argc, char **argv)
{
    int c;
    char *token = NULL;
    char *type = NULL;
    char *description = NULL;
    char *summary = NULL;
    char *host = NULL;
    char *service = NULL;
    char *state = NULL;
    char *roomId = NULL;

    std::cout << argc << std::endl;

    for (int i = 0; i < argc; i++)
    {
        std::cout << argv[i] << std::endl;
    }

    if (argc < 15)
    {
        std::cerr << argv[0] << ": Mandatory fields not provided." << std::endl
                  << argv[0] << ": Usage: "
                  << argv[0]
                  << " --token token "
                  << " --type type "
                  << " --description  description "
                  << " --summary summary "
                  << " --host host "
                  << " --service service "
                  << " --state state "
                  << " --roomId roomId "
                  << std::endl;
        return EXIT_FAILURE;
    }

    while (optind < argc)
    {
        static struct option long_options[] =
            {
                /* These options set a flag. */
                {"verbose", no_argument, &verbose_flag, 1},
                {"brief", no_argument, &verbose_flag, 0},
                /* These options don’t set a flag.
                   We distinguish them by their indices. */

                /** Only roomId is needed for sending
                 *  even a private message
                 */
                {"token", required_argument, 0, 't'},
                {"type", required_argument, 0, 'T'},
                {"description", required_argument, 0, 'd'},
                {"summary", required_argument, 0, 'e'},
                {"host", required_argument, 0, 'h'},
                {"service", required_argument, 0, 's'},
                {"state", required_argument, 0, 'S'},
                {"roomId", required_argument, 0, 'r'},
                // {"files", required_argument, 0, 'f'},
                {0, 0, 0, 0}};
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long(argc, argv, "abc:d:f:",
                        long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
        {
            break;
        }

        switch (c)
        {
        case 0:
            /* If this option set a flag, do nothing else now. */
            if (long_options[option_index].flag != 0)
                break;
            printf("option %s", long_options[option_index].name);
            if (optarg)
                printf(" with arg %s", optarg);
            printf("\n");
            break;

        case 't':
            token = optarg;
            break;

        case 'T':
            type = optarg;
            break;

        case 'd':
            description = optarg;
            break;

        case 'h':
            host = optarg;
            break;

        case 's':
            service = optarg;
            break;

        case 'S':
            state = optarg;
            break;

        case 'r':
            roomId = optarg;
            break;

        case 'e':
            summary = optarg;
            break;

        case '?':
            /* getopt_long already printed an error message. */
            break;

        default:
            abort();
        }
    }

    /* Print any remaining command line arguments (not options). */
    if (optind < argc)
    {
        printf("Unknown elements: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        putchar('\n');

        return EXIT_FAILURE;
    }

    if (type == NULL)
    {
        std::cerr << "Type not provided. \n "
                  << ": Usage: "
                  << argv[0]
                  << " --token token "
                  << " --type type "
                  << " --description  description "
                  << " --summary summary "
                  << " --host host "
                  << " --service service "
                  << " --state state "
                  << " --roomId roomId "
                  << std::endl;
        ;
        return EXIT_FAILURE;
    }

    if (host == NULL)
    {
        std::cerr << argv[0]
                  << ": Hostname not specified " << std::endl
                  << argv[0] << ": Usage: "
                  << argv[0]
                  << " --token token "
                  << " --type type "
                  << " --description  description "
                  << " --summary summary "
                  << " --host host "
                  << " --service service "
                  << " --state state "
                  << " --roomId roomId "
                  << std::endl;
        ;

        return EXIT_FAILURE;
    }

    if (state == NULL)
    {
        std::cerr << "State not provided. \n "
                  << ": Usage: "
                  << argv[0]
                  << " --token token "
                  << " --type type "
                  << " --description  description "
                  << " --summary summary "
                  << " --host host "
                  << " --service service "
                  << " --state state "
                  << " --roomId roomId "
                  << std::endl;
        ;
        return EXIT_FAILURE;
    }

    if (token == NULL)
    {
        std::cerr << "Token not provided. \n "
                  << ": Usage: "
                  << argv[0]
                  << " --token token "
                  << " --type type "
                  << " --description  description "
                  << " --summary summary "
                  << " --host host "
                  << " --service service "
                  << " --state state "
                  << " --roomId roomId "
                  << std::endl;
        ;
        return EXIT_FAILURE;
    }

    if (roomId == NULL)
    {
        std::cerr << "Room ID not provided. \n "
                  << ": Usage: "
                  << argv[0]
                  << " --token token "
                  << " --type type "
                  << " --description  description "
                  << " --summary summary "
                  << " --host host "
                  << " --service service "
                  << " --state state "
                  << " --roomId roomId "
                  << std::endl;
        ;
        return EXIT_FAILURE;
    }

    if (description == NULL)
    {
        std::cerr << "Description not provided. \n "
                  << ": Usage: "
                  << argv[0]
                  << " --token token "
                  << " --type type "
                  << " --description  description "
                  << " --summary summary "
                  << " --host host "
                  << " --service service "
                  << " --state state "
                  << " --roomId roomId "
                  << std::endl;
        ;
        return EXIT_FAILURE;
    }

    if (summary == NULL)
    {
        std::cerr << "Summary not provided. \n "
                  << ": Usage: "
                  << argv[0]
                  << " --token token "
                  << " --type type "
                  << " --description  description "
                  << " --summary summary "
                  << " --host host "
                  << " --service service "
                  << " --state state "
                  << " --roomId roomId "
                  << std::endl;
        ;
        return EXIT_FAILURE;
    }

    std::cout << "#################################" << std::endl;

    printf("Type: %s\n", type);
    printf("Hostname: %s\n", host);
    if (service != NULL)
    {
        printf("Service: %s\n", service);
    }
    printf("State: %s\n", state);
    printf("Token: %s\n", token);
    printf("Room identifier: %s\n", roomId);
    printf("Description : %s\n", description);
    printf("Summary: %s\n", summary);

    std::cout << "#################################" << std::endl;

    const char *url = "https://webexapis.com/v1/messages";

    std::string from = "\n";
    std::string to = "\\n";

    std::string str(description);

    replaceAll(str, from, to);

    std::string body = "{\"roomId\": \"" + std::string(roomId) + "\", \"markdown\": \"[" + type + "]: " + summary + "\\n" + str + "\"}";

    try
    {
        std::list<std::string> header;
        header.push_back("Authorization: Bearer " + std::string(token));
        header.push_back("Content-Type: application/json");
        header.push_back("Accept: application/json");

        curlpp::Cleanup clean;
        curlpp::Easy r;
        r.setOpt(new curlpp::options::Url(url));
        r.setOpt(new curlpp::options::HttpHeader(header));
        r.setOpt(new curlpp::options::PostFields(body));
        r.setOpt(new curlpp::options::PostFieldSize(body.length()));

        std::ostringstream response;
        r.setOpt(new curlpp::options::WriteStream(&response));

        r.perform();

        std::string responseUrl = curlpp::infos::EffectiveUrl::get(r);
        long responseStatusCode = curlpp::infos::ResponseCode::get(r);
        std::string responseBody = response.str();

        // other way to retreive URL
        std::cout << std::endl
                  << "Effective URL: "
                  << responseUrl
                  << std::endl;

        std::cout << "Response code: "
                  << responseStatusCode
                  << std::endl;

        std::cout << "Response body: "
                  << responseBody
                  << std::endl;

        return EXIT_SUCCESS;
    }
    catch (curlpp::LogicError &e)
    {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (curlpp::RuntimeError &e)
    {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    /* Instead of reporting ‘--verbose’
       and ‘--brief’ as they are encountered,
       we report the final status resulting from them. */
    if (verbose_flag)
        puts("verbose flag is set");

    return EXIT_SUCCESS;
    // exit(0);
}
