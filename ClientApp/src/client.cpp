#include "client.h"


/* Init client wrapper */
ClientWrapper::ClientWrapper(const std::string& ip, const unsigned short& port, const std::string file_path):
m_ip {ip}, m_port {port}, m_file_path{file_path}
{
    m_ep = {boost::asio::ip::address::from_string(m_ip), m_port};   
}


/* Process connect */
void ClientWrapper::process_connect()
{
    /* Create socket */
    std::shared_ptr sock_ptr = std::make_shared<boost::asio::ip::tcp::socket>(m_service);

    /* Wait connect... */
    boost::system::error_code er;

    sock_ptr->connect(m_ep, er);

    if (er)
    {
        log(er.message());

        return;
    }

    int flag_send = -1;

    /* Open file */
    std::ifstream file;

    file.open(m_file_path, std::ios::binary);

    if (file.is_open())
    {
        long file_size = std::filesystem::file_size(m_file_path);
         
        const char* file_name = std::filesystem::path(m_file_path).filename().c_str();

        int file_name_size = sizeof(file_name);

        try
        {
            /* Send info from file */
            sock_ptr->send(boost::asio::buffer(&file_name_size, sizeof(int)));

            sock_ptr->send(boost::asio::buffer(file_name, file_name_size));

            sock_ptr->send(boost::asio::buffer(&file_size, sizeof(long)));


            /* Echo from server */
            sock_ptr->receive(boost::asio::buffer(&flag_send, sizeof(int)));

            if (flag_send!= 1)
            {
                log("Server not recive info from file");

                file.close();

                return;    
            }

            log("Server recived info from file and ready to transfer");

        }
        catch (std::exception& ex)
        {
            log(ex.what());

            file.close();

            return;        
        }
    

        /* Send file */
        char data[m_buf_size];

        long file_size_current = file_size;

        while(!file.eof())
        {                      
            try
            {   
                file.read(data, m_buf_size); 

                file_size_current -= sock_ptr->write_some(boost::asio::buffer(data, file_size_current < m_buf_size ? file_size_current: m_buf_size));
            }
            catch (std::exception& ex) 
            {
                
                log(ex.what());

                break;
            } 
        }

        
        /* Get info upload file */
        flag_send = -1;

        try
        {
            sock_ptr->receive(boost::asio::buffer(&flag_send, sizeof(int)));
        }
        catch(std::exception& ex)
        {
            log(ex.what());
        }

        if (flag_send == 1)
        {
            log("File upload is complete");
        }
        else
        {
            log("File upload is error");
        } 
    }
    else
    {
        log("File not open");        
    }

    file.close();    

}



/* Start process */ 
void ClientWrapper::start()
{
    std::thread t_connect(&ClientWrapper::process_connect, this);

    t_connect.join();

}


/* Logs */
void ClientWrapper::log(const std::string& message)
{
    std::lock_guard loc(m_mut_log);

    time_t t = time(nullptr);

    tm* time = localtime(&t);

    std::cout << "Time: " << std::put_time(time, "%y/%m/%d %X") << ", Message: " << message << "\n";    
}
