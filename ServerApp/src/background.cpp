#include "background.h"



/* Init background */
void ServerWrapperBackground::process_background()
{
    pid_t pid;

    /* Close parent */
    pid = fork();

    if (pid < 0)
        exit(EXIT_FAILURE);

    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* Create new session */
    if (setsid() < 0)
        exit(EXIT_FAILURE);


    /* Ignore handle signals */
    signal(SIGHUP, SIG_IGN);


    /* Close first child */
    pid = fork();

    if (pid < 0)
        exit(EXIT_FAILURE);

    if (pid > 0)
        exit(EXIT_SUCCESS);
        

    umask(0);
   
    /* Change the working directory to the root directory */
    chdir("/");

    /* Close descriptors */
    for (int i = 0; i < sysconf(_SC_OPEN_MAX); i++)
    {
        close (i);
    }

    /* Open the log file */
    openlog ("ServerApp", LOG_PID, LOG_DAEMON);
}


/* Signal callback  */
void ServerWrapperBackground::signal_callback(int sig)
{
    syslog(LOG_NOTICE, "Server: stoped"); 

    exit(0);
}


/* Start server mode background */
void ServerWrapperBackground::start()
{
    /* Start daemon */
    process_background();


    /* Start server */
    m_server.set_log_callback([](const std::string& mes)-> void {
         syslog(LOG_NOTICE, "Server: %s", mes.c_str());
    });

    m_server.start();


    /* Log */
    syslog(LOG_NOTICE, "Server: started");


    /* Registry signal close */
    signal(SIGTERM, ServerWrapperBackground::signal_callback);


    while(1)
    {
        //std::this_thread::sleep_for(std::chrono::seconds(10));        
    }

}

