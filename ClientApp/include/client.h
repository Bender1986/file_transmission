#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <thread>
#include <memory>
#include <fstream>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <boost/asio.hpp>

class ClientWrapper
{

    private:

    const int m_buf_size = 256;

    const std::string m_ip = "127.0.0.1";
    const unsigned short m_port = 2001;
    const std::string m_file_path ;  


    boost::asio::io_service m_service;
    boost::asio::ip::tcp::endpoint m_ep;

    
    std::mutex m_mut_log;

    void process_connect();

    void log(const std::string& message);

    
    public:

    ClientWrapper(const std::string& ip, const unsigned short& port, const std::string file_path);
   
    ClientWrapper(ClientWrapper&&) = delete;


    void start();

    void stop();

};



#endif