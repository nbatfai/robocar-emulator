/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file traffic-main.cpp
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

#include <traffic.hpp>
#include <boost/program_options.hpp>

int main ( int argc, char* argv[] )
{

  int t = std::time ( nullptr );
#ifdef DEBUG
  std::cout << "srand init =  " << t << std::endl;
#endif

  std::srand ( t );
  // std::srand ( 10007 );

  boost::program_options::options_description desc ( "Options" );
  desc.add_options()
  ( "version", "produce version message" )
  ( "help", "produce help message" )
  ( "shm", boost::program_options::value< std::string > (), "shared memory segment name" )
  ( "port", boost::program_options::value< std::string > (), "the TCP port that the traffic server is listening on to allow agents to communicate with the traffic simulation, the default value is 10007" )
  ( "nrcars", boost::program_options::value<int>(), "number of the random cars" )
  ( "minutes", boost::program_options::value<int>(), "how long does the traffic simulation run for?" )
  ( "catchdist", boost::program_options::value<double>(), "the catch distance of cop cars" )
  ( "traffict", boost::program_options::value< std::string > (), "traffic type = NORMAL|ANTS|ANTS_RND|ANTS_RERND|ANTS_MRERND" )
  ;
  
  boost::program_options::variables_map vm;
  boost::program_options::store ( boost::program_options::parse_command_line ( argc, argv, desc ), vm );
  boost::program_options::notify ( vm );

  if ( vm.count ( "version" ) )
    {
      std::cout << "Robocar City Emulator and Robocar World Championship, Traffic Server" << std::endl
                << "Copyright (C) 2014, 2015 Norbert Bátfai\n" << std::endl
                << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>" << std::endl
                << "This is free software: you are free to change and redistribute it." << std::endl
                << "There is NO WARRANTY, to the extent permitted by law." << std::endl;
      return 0;
    }

  if ( vm.count ( "help" ) )
    {
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

  int nrcars {100};
  if ( vm.count ( "nrcars" ) )
    nrcars = vm["nrcars"].as < int > ();

  int minutes {10};
  if ( vm.count ( "minutes" ) )
    minutes = vm["minutes"].as < int > ();
  
  int catchdist {15.5};
  if ( vm.count ( "catchdist" ) )
    catchdist = vm["catchdist"].as < int > ();

  std::string traffict;
  if ( vm.count ( "traffict" ) )
    traffict.assign ( vm["traffict"].as < std::string > () );
  else
    traffict.assign ( "NORMAL" );
  
  justine::robocar::TrafficType type;
  if(traffict == "ANTS_RND")
    type = justine::robocar::TrafficType::ANT_RND;
  else if(traffict == "ANTS_RERND")
    type = justine::robocar::TrafficType::ANT_RERND;
  else if(traffict == "ANTS_MRERND")
    type = justine::robocar::TrafficType::ANT_MRERND;
  else if(traffict == "ANTS")
    type = justine::robocar::TrafficType::ANT;
  else
    type = justine::robocar::TrafficType::NORMAL;
  
  justine::robocar::Traffic traffic {nrcars, shm.c_str(), catchdist, type, minutes };

  try
    {
      boost::asio::io_service io_service;
      traffic.start_server ( io_service, std::atoi ( port.c_str() ) );
    }
  catch ( std::exception& e )
    {
      std::cerr << "Exception: " << e.what() << "\n";
    }

}
