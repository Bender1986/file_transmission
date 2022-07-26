#include "server.h"



/* Processing connections */
void ServerWrapper::process_connections()
{
      
        boost::asio::ip::tcp::acceptor acceptor(m_service, m_ep);

        while(!m_stop)
        {                    
            
            /* Create socket */
            std::shared_ptr sock_ptr = std::make_shared<boost::asio::ip::tcp::socket>(m_service); 
            

            /* Wait connect... */
            acceptor.accept(*sock_ptr);
            

            log("accept connection");
            
            /* Create Task */        
            {
                std::lock_guard loc(m_mut_queue_valid_connections);

                std::packaged_task<bool()> task(std::bind(&ServerWrapper::process_connect, this, sock_ptr));

                m_queue_valid_connections.push(std::move(task));

                log("connection push to queue");        
            }   
    
        }
    
    
  

}


/* Processing Queue Valid Connections */
void ServerWrapper::process_queue_valid_connections()
{
    while(!m_stop)
    {
        if (!m_queue_valid_connections.empty())
        {
            if (m_current_threads_process_connect < m_max_threads_process_connect)
            {
                               
                std::lock_guard loc(m_mut_queue_valid_connections);

                if (m_queue_valid_connections.empty() && 
                    m_current_threads_process_connect >= m_max_threads_process_connect)
                    continue;

                
                std::packaged_task<bool()> task_connect = std::move(m_queue_valid_connections.front());

                m_queue_valid_connections.pop();

                m_current_threads_process_connect++;

                std::thread t_task(std::move(task_connect));

                t_task.detach();                                
            }
        }
    }
}


/* Process Connection: Get data from client and write file */
bool ServerWrapper::process_connect(std::shared_ptr<boost::asio::ip::tcp::socket> sock_ptr)
{
    log("begin process download file"); 
    
    int flag_recive = -1; 


    /* Get info from File */
    int size_file_name;
    
    char* file_name;

    long file_size;

    try
    {
        sock_ptr->receive(boost::asio::buffer(&size_file_name, sizeof(int)));

        file_name = new char[size_file_name];   

        sock_ptr->receive(boost::asio::buffer(file_name, size_file_name));

        sock_ptr->receive(boost::asio::buffer(&file_size, sizeof(long)));
           
        log("download file info [name: " + std::string(file_name) + ", size: " + std::to_string(file_size) + "]");
        
        /* Info from file is recived  */
        flag_recive = 1;

        sock_ptr->send(boost::asio::buffer(&flag_recive, sizeof(flag_recive)));        
    }
    catch (std::exception& ex)
    {           
        /* Dispose resource */   
        delete file_name;  
        {
            std::lock_guard loc(m_mut_queue_valid_connections);

            m_current_threads_process_connect--;
        }

        return false;
    }

           
    /* Download file */
    std::string file_path = m_dir + std::string(file_name);

    log("file download to "+ file_path);

    std::ofstream file(file_path.c_str(), std::ios::binary);

    if (file.is_open())
    {
        log("file is open");
    }
    else 
    {
        /* Dispose resource */   
        delete file_name;  
        {
            std::lock_guard loc(m_mut_queue_valid_connections);

            m_current_threads_process_connect--;
        }

        return false;
    }
            
    long file_size_current = file_size;

    char data[256];

    int read_byte = 0;

    while(file_size_current > 0)
    {
        try
        {
            read_byte = sock_ptr->read_some(boost::asio::buffer(data, 256));

            file.write(data, read_byte);

            file_size_current -= read_byte;
        }
        catch (std::exception& ex) 
        {
            log(ex.what());
            
            flag_recive = -1;

            break;
        }
        
        /* Signal stop */
        if (m_stop)
        {
            /* Set signal client */
            flag_recive = -1;
        }                                       
    }

     

    /* Close file */
    file.close();

    log("file upload is complete");


    /* Send info result from client */
    try
    {
        sock_ptr->send(boost::asio::buffer(&flag_recive, sizeof(flag_recive)));
    }
    catch (std::exception& ex)
    {
        log(ex.what());
    }       
        

    /* Dispose resource */   
    delete file_name;  
    {
        std::lock_guard loc(m_mut_queue_valid_connections);

        m_current_threads_process_connect--;
    }

    return true;
}




/* Start work server */
void ServerWrapper::start()
{          
    std::thread t_connections(&ServerWrapper::process_connections, this);

    t_connections.detach();

    std::thread t_queue_valid_connections(&ServerWrapper::process_queue_valid_connections, this);

    t_queue_valid_connections.detach();  

    
}


/* Stop work server */
void ServerWrapper::stop()
{
    m_stop = true;
}


void ServerWrapper::set_log_callback(void(* log_callback)(const std::string&))
{
    m_log_callback = log_callback;
}


/* Logs */
void ServerWrapper::log(const std::string& message)
{
    std::lock_guard loc(m_mut_log);

    time_t t = time(nullptr);

    tm* time = localtime(&t);

    //std::string mess = std::string(std::put_time(time, "%y/%m/%d %X")) + "1";

    m_log_callback(message);

    //std::cout << "Time: " << std::put_time(time, "%y/%m/%d %X") << ", Thread ID: " << id << ", Message: " << message << "\n";    
}


