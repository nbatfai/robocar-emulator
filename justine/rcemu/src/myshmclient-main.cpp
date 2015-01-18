/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file myshmclient-main.cpp
 * @author  Norbert Bátfai <nbatfai@gmail.com>
 * @version 0.0.10
 *
 * @section LICENSE
 *
 * Copyright (C) 2014 Norbert Bátfai, batfai.norbert@inf.unideb.hu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @section DESCRIPTION
 * Robocar City Emulator and Robocar World Championship
 *
 * desc
 *
 */

#include <myshmclient.hpp>
#include <boost/program_options.hpp>

int main ( int argc, char* argv[] )
{
     boost::program_options::options_description desc ( "Options" );
     desc.add_options()
     ( "version", "produce version message" )
     ( "help", "produce help message" )
     ( "shm", boost::program_options::value< std::string > (), "shared memory segment name" )
     ( "port", boost::program_options::value< std::string > (), "the TCP port that the traffic server is listening on to allow agents to communicate with the traffic simulation, the default value is 10007" )
     ( "team", boost::program_options::value< std::string > (), "team name" )
     ;

     boost::program_options::variables_map vm;
     boost::program_options::store ( boost::program_options::parse_command_line ( argc, argv, desc ), vm );
     boost::program_options::notify ( vm );

     if ( vm.count ( "version" ) ) {
          std::cout << "Robocar City Emulator and Robocar World Championship, Sample (My) SHM Client" << std::endl
                    << "Copyright (C) 2014, 2015 Norbert Bátfai\n" << std::endl
                    << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>" << std::endl
                    << "This is free software: you are free to change and redistribute it." << std::endl
                    << "There is NO WARRANTY, to the extent permitted by law." << std::endl;
          return 0;
     }

     if ( vm.count ( "help" ) ) {
          std::cout << "Robocar City Emulator and Robocar World Championship home page: https://code.google.com/p/robocar-emulator/" << std::endl;
          std::cout << desc << std::endl;
          std::cout << "Please report bugs to: nbatfai@gmail.com" << std::endl;
          return 0;
     }

     std::string shm;
     if ( vm.count ( "shm" ) )
          shm.assign ( vm["shm"].as < std::string > () );
     else
          shm.assign ( "JustineSharedMemory" );

     std::string port;
     if ( vm.count ( "port" ) )
          port.assign ( vm["port"].as < std::string > () );
     else
          port.assign ( "10007" );

     std::string team;
     if ( vm.count ( "team" ) )
       team.assign ( vm["team"].as < std::string > () );
     else
       team.assign ( "Norbi" );
     
     // If you use this sample you should add your copyright information here too:
     /*
     std::cout << "This SHM Client program has been modified by <Your Name>" << std::endl
     << "Copyright (C) 2014, 2015 Norbert Bátfai" << std::endl
     << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>" << std::endl
     */

     // Do not remove this copyright notice!
     std::cout << "Robocar City Emulator and Robocar World Championship, Sample (My) SHM Client" << std::endl
               << "Copyright (C) 2014, 2015 Norbert Bátfai" << std::endl
               << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>" << std::endl
               << "This is free software: you are free to change and redistribute it." << std::endl
               << "There is NO WARRANTY, to the extent permitted by law." << std::endl;

     justine::sampleclient::MyShmClient myShmClient {shm.c_str(), team };

     try {
          boost::asio::io_service io_service;
          myShmClient.start10 ( io_service, port.c_str() );
	  // myShmClient.start ( io_service, port.c_str() ); // 
     } catch ( std::exception& e ) {
          std::cerr << "Exception: " << e.what() << "\n";
     }

}
