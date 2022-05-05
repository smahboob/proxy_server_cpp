#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

using boost::asio::ip::tcp;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono;

void make_request_to_proxy_server(std::string url, std::vector<std::string> filter_keyword_list)
{
    // buf string should contain url + " " + filter_keyword_list
    // add a *v* to the end of the url
    std::string buf = url;
    buf.append("*v*");
    for (int i = 0; i < filter_keyword_list.size(); i++)
    {
        buf.append("|");
        buf.append(filter_keyword_list[i]);
    }
    // std::cout << "buf: " << buf << std::endl;

    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "3000");

    tcp::socket socket(io_context);
    boost::asio::connect(socket, endpoints);
    boost::system::error_code error;

    // write the url and filter keyword list to the proxy server
    boost::asio::write(socket, boost::asio::buffer(buf, buf.size()), error);
    // boost::asio::write(socket, boost::asio::buffer(filter_keyword_list, filter_keyword_list.size()), error);

    boost::asio::streambuf response;
    std::ofstream out("output.html");
    // While the socket has stuff, keep reading
    bool blocked_flag = false;
    while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error))
        // write to the output file
        out << &response;

    out.close();

    // check if the file contains a blocked keyword
    std::ifstream in("output.html");
    std::string line;
    while (std::getline(in, line))
    {
        if (line.find("Blocked") != std::string::npos)
        {
            blocked_flag = true;
            std::cout << line << std::endl;
            break;
        }
    }
    // end function if content was blocked
    if (blocked_flag)
    {
        return;
    }

    std::cout << "\n"
              << std::endl;
    while (true)
    {
        // show the option to the user to open the output file in browser or print the output file to the console
        std::cout << "(O)pen the HTML file in browser or (P)rint the response in the console? (o/p)" << std::endl;
        // show a disclaimer that opening in browser will open the file in the default browser but might not work in some cases
        std::cout << "In some cases orignal HTML might not be available, e.g if page moved, or not available.\n"
                  << std::endl;
        std::string option;
        std::cin >> option;
        if (option == "o" || option == "O" || option == "open")
        {
            std::cout << "Opening the output file in browser" << std::endl;
            std::string cmd = "open output.html";
            system(cmd.c_str());
            break;
        }
        else if (option == "p" || option == "P" || option == "print")
        {
            std::cout << "Printing the output file to the console" << std::endl;
            std::ifstream in("output.html");
            std::string line;
            while (std::getline(in, line))
                std::cout << line << std::endl;
            break;
        }
        else
        {
            std::cout << "\nInvalid option selected!" << std::endl;
        }
    }
}

int main(int argc, char **argv)
{
    // run a while loop to show menu 1) Request URL 2) Input URL to block 3) Add keyword to filter 4) Exit
    std::string choice;
    std::vector<std::string> filter_keyword_list;
    std::string url;
    std::string keyword;
    std::string input;
    bool exit = false;
    int choice_int;

    while (!exit)
    {
        // display menu
        std::cout << "\n----\n1) Request URL" << std::endl;
        std::cout << "2) Add keyword to filter" << std::endl;
        std::cout << "3) Remove keyword from filter list" << std::endl;
        std::cout << "4) View blocked keywords" << std::endl;
        std::cout << "5) Exit" << std::endl;
        std::cout << "Choose a number (1-4): ";
        std::cin >> choice;

        try
        {
            choice_int = std::stoi(choice);
        }
        catch (const std::invalid_argument &e)
        {
            std::cout << "Invalid input" << std::endl;
            continue;
        }

        // switch statement for choice
        switch (choice_int)
        {
        case 1:
            std::cout << "Enter URL (e.g www.example.com): ";
            std::cin >> url;
            std::cout << "Requesting URL..." << std::endl;
            make_request_to_proxy_server(url, filter_keyword_list);
            break;
        case 2:
            std::cout << "Enter keyword: ";
            std::cin >> keyword;
            filter_keyword_list.push_back(keyword);
            std::cout << keyword << " added to filter list" << std::endl;
            break;
        case 3:
            std::cout << "Enter keyword: ";
            std::cin >> keyword;
            if (std::find(filter_keyword_list.begin(), filter_keyword_list.end(), keyword) != filter_keyword_list.end())
            {
                filter_keyword_list.erase(std::remove(filter_keyword_list.begin(), filter_keyword_list.end(), keyword), filter_keyword_list.end());
                std::cout << keyword << " removed from filter list" << std::endl;
            }
            else
            {
                std::cout << "Keyword not found in filter list." << std::endl;
            }
            break;
        case 4:
            std::cout << "Blocked keywords: ";
            if (filter_keyword_list.size() == 0)
            {
                std::cout << "Empty!" << std::endl;
            }
            else
            {
                for (int i = 0; i < filter_keyword_list.size(); i++)
                {
                    std::cout << filter_keyword_list[i] << " ";
                }
                std::cout << std::endl;
            }
            break;
        case 5:
            exit = true;
            std::cout << "Goodbye!" << std::endl;
            break;
        default:
            std::cout << "Invalid choice" << std::endl;
            break;
        }
    }
}
