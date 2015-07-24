/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file smartcity.hpp
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

#include <smartcity.hpp>
#include <boost/program_options.hpp>
#include <exception>

/**
* @brief This function gives a list of all the bus services operating in a given city.
*
*
*/
double justine::robocar::SmartCity::busWayLength ( bool verbose )
{

  double sum_bus_length {0.0};
  for ( auto busit = begin ( m_busWayNodesMap );
        busit != end ( m_busWayNodesMap ); ++busit )
    {

      if ( verbose )
        std::cout << busit->first << ": ";

      double bus_length {sum_bus_length};
      for ( auto ref : busit->second )
        {

          int i {1};

          osmium::Location prev_loc;
          for ( auto node_ref : m_way2nodes[ref] )
            {

              try
                {

                  //osmium::Location loc = m_waynode_locations.get ( node_ref );
		  osmium::Location loc = m_waynode_locations[node_ref];
		  
                  if ( verbose )
                    std::cout << loc << std::endl;

                  if ( i++>1 )
                    {
                      osmium::geom::Coordinates coords {loc};
                      osmium::geom::Coordinates prev_coords {prev_loc};
                      sum_bus_length += osmium::geom::haversine::distance ( coords, prev_coords );
                    }
                  prev_loc = loc;

                }
              catch ( std::exception& e )
                {
                  std::cerr << " No such node on the map. "<< e.what() << std::endl;
                }

            }

        }
      if ( verbose )
        std::cout << ( sum_bus_length-bus_length ) /1000.0 << " km"<< std::endl;

    }

  return sum_bus_length/1000.0;
}

int main ( int argc, char* argv[] )
{
  boost::program_options::options_description desc ( "Options" );
  desc.add_options()
  ( "version", "produce version message" )
  ( "help", "produce help message" )
  ( "osm", boost::program_options::value< std::string > (), "OSM file name" )
  ( "city", boost::program_options::value< std::string > (), "the name of the city" )
  ( "shm", boost::program_options::value< std::string > (), "shared memory segment name" )
  ( "node2gps", boost::program_options::value< std::string > (), "node2gps file name" )
  ( "node2way", boost::program_options::value< std::string > (), "node2way file name" )
 ;

  boost::program_options::variables_map vm;
  boost::program_options::store ( boost::program_options::parse_command_line ( argc, argv, desc ), vm );
  boost::program_options::notify ( vm );

  if ( vm.count ( "version" ) )
    {
      std::cout << "Robocar City Emulator and Robocar World Championship, City Server" << std::endl
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

  std::string osm_input;
  if ( vm.count ( "osm" ) )
    osm_input.assign ( vm["osm"].as < std::string > () );
  else
    osm_input.assign ( "../debrecen.osm" );

  std::string node2gps_output;
  if ( vm.count ( "node2gps" ) )
    node2gps_output.assign ( vm["node2gps"].as < std::string > () );
  else
    node2gps_output.assign ( "../lmap.txt" );

  std::string node2way_output;
  if ( vm.count ( "node2way" ) )
    node2way_output.assign ( vm["node2way"].as < std::string > () );
  else
    node2way_output.assign ( "../lmap.txt" );  
  
  std::string city;
  if ( vm.count ( "city" ) )
    city.assign ( vm["city"].as < std::string > () );
  else
    city.assign ( "Debrecen" );

  std::string shm;
  if ( vm.count ( "shm" ) )
    shm.assign ( vm["shm"].as < std::string > () );
  else
    shm.assign ( "JustineSharedMemory" );

  // Do not remove this copyright notice!
  std::cout << "Robocar City Emulator and Robocar World Championship, City Server" << std::endl
            << "Copyright (C) 2014, 2015 Norbert Bátfai" << std::endl
            << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>" << std::endl
            << "This is free software: you are free to change and redistribute it." << std::endl
            << "There is NO WARRANTY, to the extent permitted by law." << std::endl;

  std::cout << city << " is... " << std::flush;
  try
    {

      if ( vm.count ( "node2gps" ) )
        justine::robocar::SmartCity smartCity ( osm_input.c_str(), shm.c_str(), node2gps_output.c_str(), 0 );
      else if ( vm.count ( "node2way" ) )
        justine::robocar::SmartCity smartCity ( osm_input.c_str(), shm.c_str(), node2way_output.c_str(), 1 );
      else
        justine::robocar::SmartCity smartCity ( osm_input.c_str(), shm.c_str() );

      std::cout << "ready."<<  std::endl;
      for ( ;; );

    }
  catch ( std::exception &err )
    {

      std::cout << "SmartCity cannot be built for "+city << std::endl;
      std::cout << err.what() <<std::endl;

    }

}
