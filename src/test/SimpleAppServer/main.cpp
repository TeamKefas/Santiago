#include "Server.h"

void onSigintHandler(int signum_)
{
    throw std::runtime_error("Received sigint signal..exiting");
}


int main()
{
  try
  {
      if(signal(SIGINT, onSigintHandler) == SIG_ERR)
      {
          throw std::runtime_error("Unable to register for sigint signal...exiting");
      }


      SimpleAppServer::Server server(Santiago::LocalEndpoint<boost::asio::ip::tcp> (7000));

      server.start();
      int i;
      std::cin>>i;
      server.stop();
  }
  catch (std::exception& e)
  {
      std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
