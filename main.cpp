#include <fstream>
#include <iostream>
#include <sstream>
#include <future>
#include <thread>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>

/* Flag set by ‘--verbose’. */
static int verbose_flag;

static int monitoring;

void replaceAll(std::string &mDescription, const std::string &from, const std::string &to)
{
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = mDescription.find(from, start_pos)) != std::string::npos)
    {
        mDescription.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'from' with 'to'
    }
}

std::future<std::string> invoke(std::string const &webexUrl, std::string const &body, std::string const &token)
{
    std::future<std::string> rData = std::async(
        std::launch::async,
        [&token](std::string const &webexUrl, std::string const &body) mutable
        {
            std::list<std::string> header;
            header.push_back("Authorization: Bearer " + token);
            header.push_back("Content-Type: application/json");
            header.push_back("Accept: application/json");

            curlpp::Cleanup clean;
            curlpp::Easy r;
            r.setOpt(new curlpp::options::Url(webexUrl));
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
        webexUrl, body);

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
    char *hostname = NULL;
    char *service = NULL;
    char *state = NULL;
    char *roomId = NULL;
    char *icingaUrl = NULL;
    char *author = NULL;
    char *comment = NULL;

    while (optind < argc)
    {
        static struct option long_options[] =
            {
                /* These options set a flag. */
                {"verbose", no_argument, &verbose_flag, 1},
                {"brief", no_argument, &verbose_flag, 0},

                {"monitoring", no_argument, &monitoring, 1},

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
                {"hostname", required_argument, 0, 'N'},
                {"service", required_argument, 0, 's'},
                {"state", required_argument, 0, 'S'},
                {"roomId", required_argument, 0, 'r'},
                {"icingaUrl", required_argument, 0, 'i'},
                {"author", required_argument, 0, 'a'},
                {"comment", required_argument, 0, 'c'},

                {0, 0, 0, 0}};
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long(argc, argv, "ma:c:t:T:d:e:h:N:s:S:r:i:",
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
            std::cout << argc << std::endl;

            for (int i = 0; i < argc; i++)
            {
                std::cout << argv[i] << std::endl;
            }
            printf("\n");
            break;

        case 'a':
            if (optarg > 0)
                author = optarg;
            break;

        case 'c':
            comment = optarg;
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

        case 'N':
            hostname = optarg;
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

        case 'i':
            icingaUrl = optarg;
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

    if (!type || strlen(type) == 0 || !token || strlen(token) == 0 || !host || strlen(host) == 0 || !state || strlen(state) == 0 || !roomId || strlen(roomId) == 0 || !description || strlen(description) == 0)
    {
        std::string message;

        if (!type || strlen(type) == 0)
        {
            message += "type, ";
        }
        if (!token || strlen(token) == 0)
        {
            message += "token, ";
        }
        if (!host || strlen(host) == 0)
        {
            message += "host, ";
        }
        if (!state || strlen(state) == 0)
        {
            message += "state, ";
        }
        if (!roomId || strlen(roomId) == 0)
        {
            message += "roomId, ";
        }
        if (!description || strlen(description) == 0)
        {
            message += "description, ";
        }

        message.pop_back();
        message.pop_back();

        std::string fromComma = ",";
        int lastCommaPosition = message.find_last_of(fromComma);
        if (lastCommaPosition != std::string::npos)
            ;
        {
            std::string toAnd = " and";
            message.replace(lastCommaPosition, fromComma.length(), toAnd);
        }

        message += " are mandatory but not provided! \n";

        std::cerr << message
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

    printf("Monitoring FLAG: %i\n", monitoring);

    printf("Type: %s\n", type);
    printf("Hostname: %s\n", host);
    if (service)
    {
        printf("Service: %s\n", service);
    }
    printf("State: %s\n", state);
    printf("Token: %s\n", token);
    printf("Room identifier: %s\n", roomId);
    printf("Description : %s\n", description);
    if (summary)
    {
        printf("Summary: %s\n", summary);
    }
    if (icingaUrl)
    {
        printf("Icinga Url: %s\n", icingaUrl);
    }
    if (strlen(author) > 0)
    {
        printf("Author: %s\n", author);
    }
    if (strlen(comment) > 0)
    {
        printf("Comment: %s\n", comment);
    }

    std::cout << "#################################" << std::endl;

    const char *webexUrl = "https://webexapis.com/v1/messages";

    std::string mDescription(description);
    std::string mSummary;

    if (summary)
    {
        mSummary = summary;
    }

    std::string from = "\n";
    std::string to = "\\n";

    replaceAll(mDescription, from, to);
    replaceAll(mDescription, "\[OK\]", "&#128994;");
    replaceAll(mDescription, "\[WARNING\]", "&#128992;");
    replaceAll(mDescription, "\[CRITICAL\]", "&#128308;");
    replaceAll(mDescription, "\[UNKNOWN\]", "&#128995;");

    if (strlen(author) > 0 && strlen(comment) > 0)
    {
        mDescription += "\\n\\n";
        mDescription += author;
        mDescription += ": ";
        mDescription += comment;
    }

    std::string body;

    std::string mIcingaUrl(icingaUrl);
    std::string mHost(host);
    std::string mHostName(hostname);
    std::string mService;
    std::string mServiceLinkText;

    replaceAll(mHost, ".", "\.");
    replaceAll(mHost, " ", "%20");
    replaceAll(mHostName, ".", "\.");

    if (service)
    {
        /* code */
        mService = std::string(service);
        mServiceLinkText = mService;
        replaceAll(mServiceLinkText, " ", "%20");
    }

    if (icingaUrl)
    {
        std::string mHostLink;

        std::string mServiceLink;

        if (!monitoring)
        {
            mHostLink = "<a href=\\\"" + mIcingaUrl + "/icingadb/host?name=" + mHost + "\\\" rel=\\\"nofollow\\\">" + mHostName + "</a>";

            if (service)
                mServiceLink = "<a href=\\\"" + mIcingaUrl + "/icingadb/service?name=" + mServiceLinkText + "&host.name=" + mHost + "\\\" rel=\\\"nofollow\\\">" + mService + "</a>";
        }
        else
        {
            mHostLink = "<a href=\\\"" + mIcingaUrl + "/monitoring/host/show?host=" + mHost + "\\\" rel=\\\"nofollow\\\">" + mHostName + "</a>";

            if (service)
                mServiceLink = "<a href=\\\"" + mIcingaUrl + "/monitoring/service/show?host=" + mHost + "&service=" + mServiceLinkText + "\\\" rel=\\\"nofollow\\\">" + mService + "</a>";
        }

        // std::string mHostLink = "[" + mHostName + "](" + mIcingaUrl + "/icingadb/host?name=" + mHost + ")";

        // std::string mServiceLink = "[" + mService + "](" + mIcingaUrl + "/icingadb/service?name=" + mServiceLinkText + "&host.name=" + mHost + ")";

        if (service)
        {
            mSummary = mServiceLink + " on " + mHostLink + " is " + std::string(state);
        }
        else
        {
            mSummary = mHostLink + " is " + std::string(state);
        }

        body = "{\"roomId\": \"" + std::string(roomId) + "\", \"html\": \"[" + type + "]: " + mSummary + "\\n" + mDescription + "\"}";
    }
    else
    {
        if (service)
        {
            mSummary = mService + " on " + mHost + " is " + std::string(state);
        }
        else
        {
            mSummary = mHost + " is " + std::string(state);
        }

        body = "{\"roomId\": \"" + std::string(roomId) + "\", \"text\": \"[" + type + "]: " + mSummary + "\\n" + mDescription + "\"}";
    }

    try
    {
        std::list<std::string> header;
        header.push_back("Authorization: Bearer " + std::string(token));
        header.push_back("Content-Type: application/json");
        header.push_back("Accept: application/json");

        curlpp::Cleanup clean;
        curlpp::Easy r;
        r.setOpt(new curlpp::options::Url(webexUrl));
        r.setOpt(new curlpp::options::HttpHeader(header));
        r.setOpt(new curlpp::options::PostFields(body));
        r.setOpt(new curlpp::options::PostFieldSize(body.length()));

        std::ostringstream response;
        r.setOpt(new curlpp::options::WriteStream(&response));

        r.perform();

        std::string responseUrl = curlpp::infos::EffectiveUrl::get(r);
        long responseStatusCode = curlpp::infos::ResponseCode::get(r);
        std::string responseBody = response.str();

        if (responseStatusCode != 200)
        {
            return EXIT_FAILURE;
        }

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
