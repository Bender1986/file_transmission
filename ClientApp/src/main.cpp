#include <iostream>
#include <boost/program_options.hpp>

#include "client.h"

namespace opt = boost::program_options;


int main(int argc, char** argv) 
{
    
    std::string server_ip = "127.0.0.1";
    unsigned short server_port = 2001;
    std::string file_path;
    
    
    try
    {
        opt::options_description desc("All options");

        desc.add_options()
        ("server_ip,i",  opt::value<std::string>(&server_ip)->default_value("127.0.0.1"), "Set ip address server")
        ("server_port,p", opt::value<unsigned short>(&server_port)->default_value(2001), "Set port server")
        ("file,f", opt::value<std::string>(&file_path), "File path upload to server")
        ("help,h", "produce help message");

        
        opt::variables_map vm;

        opt::store(opt::parse_command_line(argc, argv, desc), vm);

        opt::notify(vm);

        if (vm.count("help")) 
        {
            std::cout << desc << "\n";
            
            return 1;
        }
    }
    catch(std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }
       
 

    std::cout<< "Server ip: "   << server_ip << std::endl;
    std::cout<< "Server port: " << server_port << std::endl;
    std::cout<< "File path: "   << file_path << std::endl;

    
    ClientWrapper cwr(server_ip, server_port, file_path);

    cwr.start();

}
