#ifndef BACK_H
#define BACK_H

#include "server.h"
#include <unistd.h>
#include <csignal>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <syslog.h>


/* Class Server wrapper from background */
class ServerWrapperBackground
{   

    private:

    ServerWrapper m_server;

    void process_background();

    static void signal_callback(int sig);
    

    public:

    ServerWrapperBackground(const std::string& dir): m_server{dir}{};

    ServerWrapperBackground(ServerWrapperBackground&&) noexcept = delete;
    
    void start();

};






#endif