#include <iostream>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <string>
#include "config.h"

#define RECV_CLIENT_SIZE 10

using boost::asio::ip::udp;
using namespace boost;

class Server
{
    public:
        Server(asio::io_service& ios, const short port)
        : ios_(ios), socket_(ios, udp::endpoint(udp::v4(), port)),
        path("/home/g0/Desktop/data")
        {
           
            socket_.async_receive_from(boost::asio::buffer(recv_buff, RECV_CLIENT_SIZE), sender_endpoint_, 
                boost::bind(&Server::handle_recv, this, asio::placeholders::error,
                asio::placeholders::bytes_transferred));
        }
    
    private:
        void handle_recv(const boost::system::error_code& error, std::size_t bytes_transferred)
        {
            if (!error || error == asio::error::message_size)
            {
                if(strlen(recv_buff.data()) == 0)
                {
                    socket_.async_receive_from(boost::asio::buffer(recv_buff, RECV_CLIENT_SIZE), sender_endpoint_, 
                    boost::bind(&Server::handle_recv, this, asio::placeholders::error,
                    asio::placeholders::bytes_transferred));
                }
                else
                {
                    fp = fopen("/home/g0/Desktop/data/Part0_GW0.csv", "r");
                    len = readFile(fp, &send_buf[0], BUFFER_SIZE);
                    total_pack = 1 + (len - 1) / PACK_SIZE;
                    buff[0] = total_pack;
                    count = 0;
                    socket_.async_send_to(boost::asio::buffer(buff, sizeof(int)), sender_endpoint_,
                        boost::bind(&Server::handle_send, this, asio::placeholders::error,
                        asio::placeholders::bytes_transferred));
                }
                
            }
            else
            {
                std::cout << "Error occerred! Error code = " << error.message();
                return ;
            }
            
        }

        void handle_send(boost::system::error_code error, const size_t bytes_transferred)
        {
            if(!error || error == asio::error::message_size)
            {
                if (count != total_pack)
                {
                   socket_.send_to(boost::asio::buffer(&send_buf[count*PACK_SIZE], PACK_SIZE), sender_endpoint_);
                    count ++;
                    handle_send(error,bytes_transferred);
                }
                socket_.async_receive_from(boost::asio::buffer(recv_buff, RECV_CLIENT_SIZE), sender_endpoint_, 
                    boost::bind(&Server::handle_recv, this, asio::placeholders::error,
                    asio::placeholders::bytes_transferred));
            }
            else
            {
                std::cout << "Error occerred! Error code = " << error.message();
                return ;
            }
        }

        void clear_buf(char* b)
        {
            int i; 
            for (i = 0; i < PACK_SIZE; i++) 
                b[i] = '\0'; 
        }

        int readFile(FILE* fp, char* buf, int s)
        {
            char ch; 
            int i;
            for (i = 0; i < s; i++) 
            { 
                ch = fgetc(fp);
                if (ch == EOF) 
                    break; 
                buf[i] = ch; 
                
            }
            
            fclose(fp); 
            return i;
        }

        void print()
        {
        }

    private:
        asio::io_service& ios_;
        udp::socket socket_;
        udp::endpoint sender_endpoint_;
        boost::array<char,BUFFER_SIZE> send_buf;
        boost::array<char,RECV_CLIENT_SIZE> recv_buff;
        std::ifstream file;
        boost::filesystem::path path;
        FILE* fp; 
        int len;
        int total_pack;
        int buff[1];
        int count;
};

int main()
{
    boost::asio::io_service ios;
    Server serv(ios, 13);
    ios.run();
    return 0;
}