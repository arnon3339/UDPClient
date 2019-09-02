#include <iostream>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <string>
#include <thread>
#include "config.h"

using boost::asio::ip::udp;
using namespace boost;


class Client
{
    public:
      Client(asio::io_service& ios, const std::string& host, const std::string& port)
      : ios_(ios), socket_(ios, udp::endpoint (udp::v4(), 0))
      {
        std::strcpy( send_buf, "Request" ); 

        udp::resolver resolver(ios_);
        udp::resolver::query query(udp::v4(), "127.0.0.1", "13");
        udp::resolver::iterator itr = resolver.resolve(query);
        sender_endpoint_ = *itr;

        ios_.post(bind(&Client::start_send, this));
        
      }

      ~Client()
      {
        socket_.close();
      }
    
    private:
      void start_send()
      {
        socket_.async_send_to(boost::asio::buffer(send_buf,sizeof(send_buf)), sender_endpoint_, 
              boost::bind(&Client::handle_send, this, asio::placeholders::error,
              asio::placeholders::bytes_transferred));
      }

      void handle_send(const boost::system::error_code& error, const size_t bytes_transferred)
      {
        if (!error || error == asio::error::message_size)
        {
          socket_.async_receive_from(boost::asio::buffer(recv_buff,BUFFER_SIZE), sender_endpoint_, 
              boost::bind(&Client::handle_recv, this, asio::placeholders::error,
              asio::placeholders::bytes_transferred));
        }
        else
        {
          std::cout << "Error occerred! Error code = " << error.message();
          return;
        }
         
      }

      void handle_recv2(const boost::system::error_code& error, const size_t bytes_transferred)
      {
        if (!error || error == asio::error::message_size)
        {
          if (count != total_pack - 1)
          {
            std::cout << count << std::endl;
            if (bytes_transferred != PACK_SIZE)
            {
              std::cout << "expecting length of packs:" << bytes_transferred << std::endl;
              socket_.async_receive_from(boost::asio::buffer(recv_buff, BUFFER_SIZE), sender_endpoint_, 
                boost::bind(&Client::handle_recv2, this, asio::placeholders::error,
                asio::placeholders::bytes_transferred));

              count ++;
            }
            else
            {
              memcpy( & longbuf[count * PACK_SIZE], recv_buff, PACK_SIZE);
              socket_.async_receive_from(boost::asio::buffer(recv_buff, BUFFER_SIZE), sender_endpoint_, 
                  boost::bind(&Client::handle_recv2, this, asio::placeholders::error,
                  asio::placeholders::bytes_transferred));

              count ++;
            } 
          }
          else
          {
            memcpy( & longbuf[(total_pack - 1) * PACK_SIZE], recv_buff, PACK_SIZE);
            //std::cout << longbuf << std::endl;
            myfile.open("data.txt", std::ios::trunc);
            if (myfile.is_open())
              myfile << longbuf;
            myfile.close();
          }
        }
        else
        {
          std::cout << "Error occerred! Error code = " << error.message();
          return;
        }
      }

      void handle_recv(const boost::system::error_code& error, const size_t bytes_transferred)
      {
        if (!error || error == asio::error::message_size)
        {
          if(bytes_transferred != sizeof(int))
          {
            socket_.async_receive_from(boost::asio::buffer(recv_buff, BUFFER_SIZE), sender_endpoint_, 
              boost::bind(&Client::handle_recv, this, asio::placeholders::error,
              asio::placeholders::bytes_transferred));
          }
          else
          {
            total_pack = ((int * ) recv_buff)[0];
            longbuf = new char[total_pack*PACK_SIZE];
            socket_.async_receive_from(boost::asio::buffer(recv_buff, BUFFER_SIZE), sender_endpoint_, 
              boost::bind(&Client::handle_recv2, this, asio::placeholders::error,
              asio::placeholders::bytes_transferred));
          }
        }
        else
        {
          std::cout << "Error occerred! Error code = " << error.message();
          return;
        }
      }

    private:
        asio::io_service& ios_;
        udp::socket socket_;
        udp::endpoint sender_endpoint_;
        char send_buf[8];
        char recv_buff[BUFFER_SIZE];
        int total_pack;
        char * longbuf;
        int recvMsgSize;
        std::ofstream myfile;
        int count;
};

int main()
{
    boost::asio::io_service ios;
    Client client(ios, "127.0.0.1", "13");
    ios.run();
    return 0;
}