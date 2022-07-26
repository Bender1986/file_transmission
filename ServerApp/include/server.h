#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <thread>
#include <memory>
#include <future>
#include <queue>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <boost/asio.hpp>
#include <atomic>
#include <syslog.h>


class ServerWrapper
{

    private:

    const std::string m_dir;

    boost::asio::io_service m_service;

    const boost::asio::ip::tcp::endpoint m_ep = {boost::asio::ip::tcp::v4(),2001};

    const int m_max_threads_process_connect = 5;

    int m_current_threads_process_connect = 0;

    std::queue<std::packaged_task<bool()>> m_queue_valid_connections;

    std::mutex m_mut_queue_valid_connections;

    std::mutex m_mut_log;

    std::atomic<bool> m_stop = false;

    void (* m_log_callback)(const std::string&) = nullptr;


    void process_connections();

    void process_queue_valid_connections();

    bool process_connect(std::shared_ptr<boost::asio::ip::tcp::socket> sock_ptr);

    void log(const std::string& message);
    

    public:

    ServerWrapper(const std::string& dir): m_dir{dir}{};

    ServerWrapper(ServerWrapper&&) = delete;

    void set_log_callback(void (* log_callback)(const std::string&));

    void start();

    void stop();       

};






#endif