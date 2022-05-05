#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using boost::asio::ip::tcp;
using namespace std;

void handle_request(tcp::acceptor &acceptor)
{
  cout << "\n----\nWaiting for Client Request..." << endl;
  acceptor.async_accept([&](std::error_code ec, tcp::socket &&socket)
                        {
    cout << "Client Request Received" << endl;
    std::string buf;
    buf.resize(256);


    std::vector<std::string> filter_keyword_list;
    filter_keyword_list.reserve(10);

    boost::system::error_code error;

    //receive the buffer array and read it
    size_t len = socket.read_some(boost::asio::buffer(buf), error);

    // buf string should contain url + " " + filter_keyword_list
    // add a *v* to the end of the url
    std::string data_received = buf.data();
    std::string str_url = data_received.substr(0, data_received.find("*v*"));
    std::string str_filter_keyword_list = data_received.substr(data_received.find("*v*") + 3);
    std::cout << "URL: " << str_url << std::endl;

    // std::cout << "data_received: " << data_received << std::endl;
    // std::cout << "str_url: " << str_url << std::endl;
    // std::cout << "str_filter_keyword_list: " << str_filter_keyword_list << std::endl;
    //each filter word is sepearted by a |
    std::stringstream ss(str_filter_keyword_list);
    std::string token;
    while (std::getline(ss, token, '|'))
    {
      //check if token is not empty string
      if (token != "")
      {
        filter_keyword_list.push_back(token);
        std::cout << "Filter Keyword: " << token << std::endl;
      }
    }
    std::cout << "Filter Keyword List Size: " << filter_keyword_list.size() << std::endl;

    boost::asio::io_context ioc;
    //Used to look up the IP address using the domain name
    tcp::resolver resolver(ioc);
    //Socket to connect to the server
    boost::beast::tcp_stream stream(ioc);

    // Look up the domain name, always on port 80
    auto const results = resolver.resolve(str_url, "80");

    //Connect to the server
    stream.connect(results);

   // Set up an HTTP GET request message
    boost::beast::http::request<boost::beast::http::string_body> req{boost::beast::http::verb::get, "/", 10};
    req.set(boost::beast::http::field::host, str_url);
    req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

   // Send the HTTP request
    boost::beast::http::write(stream, req);

   // This buffer is used for reading and must be persisted
    boost::beast::flat_buffer buffer;

   // Best response object to hold th result
    boost::beast::http::response<boost::beast::http::dynamic_body> res;

   // Receive the HTTP response
    boost::beast::http::read(stream, buffer, res);

   // Write the message to a string
    std::string s =  boost::beast::buffers_to_string(res.body().data());
    cout << "HTTP Response received" << endl;
    boost::beast::error_code error_code;
    stream.socket().shutdown(tcp::socket::shutdown_both, error_code);


    //check if the response string s contains any of the filter keywords
    bool contains_filter_keyword = false;
    std::string match;
    for (auto &filter_keyword : filter_keyword_list)
    {
      if (s.find(filter_keyword) != std::string::npos)
      {
        contains_filter_keyword = true;
        match = filter_keyword;
        break;
      }
    }

    //if the response string s contains any of the filter keywords, send the modified s with not html data to the client
    if(contains_filter_keyword)
    {
      s = "Request Blocked! Content contains filter keyword: " + match;
    }


    //Write the string to the original socket
    cout << "Writing to original socket" << endl;
    boost::asio::write(socket, boost::asio::buffer(s), error);
    handle_request(acceptor); });
}

int main()
{
  boost::asio::io_context io_context;
  tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 3000));
  handle_request(acceptor);
  io_context.run();
  return 0;
}
