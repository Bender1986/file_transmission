#include <iostream>
#include <boost/program_options.hpp>
#include "background.h"

namespace opt = boost::program_options;


int main(int argc, char** argv) {

  std::string dir_path;
    
    
  try
  {
    opt::options_description desc("All options");

    desc.add_options()
      ("dir,d", opt::value<std::string>(&dir_path)->required(), "Directory from download")
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


  ServerWrapperBackground swb(dir_path);

  swb.start();  

}
