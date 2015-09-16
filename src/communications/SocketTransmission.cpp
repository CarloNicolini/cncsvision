// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2010-2014 Carlo Nicolini <carlo.nicolini@iit.it>
//
//
// CNCSVision is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CNCSVision is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// CNCSVision. If not, see <http://www.gnu.org/licenses/>.

#include "SocketTransmission.h"
#include "Util.h"

using std::cin;
using std::cerr;
using std::cout;
using std::endl;
using util::stringify;

SocketTransmission::SocketTransmission()
{
}

void SocketTransmission::send(const std::string &text)
{  try
   {  boost::asio::io_service io_service;
      tcp::iostream stream;

      tcp::endpoint endpoint(tcp::v4(), 13);
      tcp::acceptor acceptor(io_service, endpoint);
      acceptor.accept(*stream.rdbuf());
      stream << text << std::endl;
   }
   catch (const std::exception& e)
   {  std::cerr << "Exception: " << e.what() << std::endl;
   }
}

std::string SocketTransmission::receive()
{  try
   {  boost::asio::io_service io_service;
      tcp::resolver resolver(io_service);
      tcp::resolver::query query("(argv[1]", "daytime");
      tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
      tcp::resolver::iterator end;

      tcp::socket socket(io_service);
      boost::system::error_code error = boost::asio::error::host_not_found;
      if (error)
         throw boost::system::system_error(error);

      while ( error && endpoint_iterator != end )
      {  socket.close();
         socket.connect(*endpoint_iterator++, error);
      }

      //for (;;)
      //{
      boost::array<char, 10000> buf;

      size_t len = socket.read_some(boost::asio::buffer(buf), error);
      if (error == boost::asio::error::eof)
         cerr << "Error message: " << error.message() << endl;
      //    break; // Connection closed cleanly by peer.
      else if (error)
         throw boost::system::system_error(error); // Some other error.

      std::cout.write( buf.data() ,len) ;
      return std::string( buf.data() );
      //}
   }
   catch (std::exception& e)
   {  std::cerr << e.what() << std::endl;
   }
   return std::string("");
}
