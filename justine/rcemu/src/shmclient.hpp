#ifndef ROBOCAR_SHMCLIENT_HPP
#define ROBOCAR_SHMCLIENT_HPP

/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file shmclient.hpp
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

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>

#include <smartcity.hpp>
#include <car.hpp>

#include <cstdlib>
#include <iterator>

#include <boost/asio.hpp>

#include <limits>

#include <memory>

namespace justine
{
namespace sampleclient
{

/**
 * @brief A sample class used for testing IPC mechanisms (SHM and sockets) which are used by the city emulator.
 *
 * This sample class shows how client agents can connect and communicate with traffic emulator using shared memory.
 *
 * @author Norbert Bátfai
 * @date Dec. 7, 2014
 */
class ShmClient
{
public:

  /**
   * @brief This constructor initializes the shared memory segment.
   * @param shm_segment the shared memory object name
   *
   * This constructor attaches the shared memory segment identified by the param shm_segment.
   */
  ShmClient ( const char * shm_segment )
  {

    segment = new boost::interprocess::managed_shared_memory (
      boost::interprocess::open_only,
      shm_segment );

    shm_map =
      segment->find<justine::robocar::shm_map_Type> (
        "JustineMap" ).first;
  }

  ~ShmClient()
  {
  }

  /**
   * @brief This function starts the client.
   * @param io_service
   * @param port the TCP port of the traffic server
   *
   * This method does the following: retrieves a value from shared memory,
   * then establishes a connection with the traffic server, finally
   * sends some client commands.
   */
  void start ( boost::asio::io_service& io_service, const char * port );


  /**
   * @brief This function returns a randomly chosen node from the map.
   * @return the randomly chosen node
   *
   * This method may be useful if you want to add a new car to the map.
   */
  osmium::unsigned_object_id_type virtual get_random_node ( void )
  {

    justine::robocar::shm_map_Type::iterator iter=shm_map->begin();
    std::advance ( iter, std::rand() % shm_map->size() );

    return iter->first;
  }

  /**
   * @brief Returns the number of out edges of a given vertex.
   * @param from a given vertex
   * @return the number of edges
   *
   * This method returns the size of the vector of neighboring vertices in the shared adjacency list.
   */
  size_t num_edges ( osmium::unsigned_object_id_type from ) const
  {

    justine::robocar::shm_map_Type::iterator iter=shm_map->find ( from );
    return iter->second.m_alist.size();

  }

  /**
   * @brief Returns the i-th neighbor of the actual vertex.
   * @param to the index i
   * @return the (osmium) reference number of the i-th neighbor
   *
   * This method returns the i-th neighbor of the actual vertex in the shared adjacency list.
   */
  osmium::unsigned_object_id_type alist ( osmium::unsigned_object_id_type from, int to ) const
  {

    justine::robocar::shm_map_Type::iterator iter=shm_map->find ( from );
    return iter->second.m_alist[to];

  }

  int alist_inv ( osmium::unsigned_object_id_type from, osmium::unsigned_object_id_type to ) const
  {

    justine::robocar::shm_map_Type::iterator iter=shm_map->find ( from );

    int ret = -1;

    for ( justine::robocar::uint_vector::iterator noderefi = iter->second.m_alist.begin();
          noderefi!=iter->second.m_alist.end();
          ++noderefi )
      {

        if ( to == *noderefi )
          {
            ret = std::distance ( iter->second.m_alist.begin(), noderefi );
            break;
          }

      }

    return ret;
  }

  osmium::unsigned_object_id_type salist ( osmium::unsigned_object_id_type from, int to ) const
  {

    justine::robocar::shm_map_Type::iterator iter=shm_map->find ( from );
    return iter->second.m_salist[to];

  }
  void set_salist ( osmium::unsigned_object_id_type from, int to , osmium::unsigned_object_id_type value )
  {

    justine::robocar::shm_map_Type::iterator iter=shm_map->find ( from );
    iter->second.m_salist[to] = value;

  }
  osmium::unsigned_object_id_type palist ( osmium::unsigned_object_id_type from, int to ) const
  {

    justine::robocar::shm_map_Type::iterator iter=shm_map->find ( from );
    return iter->second.m_palist[to];

  }

  bool hasNode ( osmium::unsigned_object_id_type node )
  {

    justine::robocar::shm_map_Type::iterator iter=shm_map->find ( node );
    return ! ( iter == shm_map->end() );

  }

  double dst ( osmium::unsigned_object_id_type n1, osmium::unsigned_object_id_type n2 ) const
  {

    justine::robocar::shm_map_Type::iterator iter1=shm_map->find ( n1 );
    justine::robocar::shm_map_Type::iterator iter2=shm_map->find ( n2 );

    osmium::geom::Coordinates c1 {iter1->second.lon/10000000.0, iter1->second.lat/10000000.0};
    osmium::geom::Coordinates c2 {iter2->second.lon/10000000.0, iter2->second.lat/10000000.0};

    return osmium::geom::haversine::distance ( c1, c2 );

  }

  double dst ( double lon1, double lat1, double lon2, double lat2 ) const
  {

    osmium::geom::Coordinates c1 {lon1, lat1};
    osmium::geom::Coordinates c2 {lon2, lat2};

    return osmium::geom::haversine::distance ( c1, c2 );

  }

  void toGPS ( osmium::unsigned_object_id_type from, double *lo, double *la ) const
  {

    justine::robocar::shm_map_Type::iterator iter1=shm_map->find ( from );
    double lon1 {iter1->second.lon/10000000.0}, lat1 {iter1->second.lat/10000000.0};

    *lo = lon1;
    *la = lat1;

  }

  void toGPS ( osmium::unsigned_object_id_type from,
  osmium::unsigned_object_id_type to,
  osmium::unsigned_object_id_type step, double *lo, double *la ) const
  {

    justine::robocar::shm_map_Type::iterator iter1=shm_map->find ( from );
    double lon1 {iter1->second.lon/10000000.0}, lat1 {iter1->second.lat/10000000.0};

    justine::robocar::shm_map_Type::iterator iter2=shm_map->find ( alist ( from, to ) );
    double lon2 {iter2->second.lon/10000000.0}, lat2 {iter2->second.lat/10000000.0};

    osmium::unsigned_object_id_type maxstep = palist ( from, to );

    if ( maxstep == 0 )
    {
      maxstep = 1;
    }

    lat1 += step * ( ( lat2 - lat1 ) / maxstep );
    lon1 += step * ( ( lon2 - lon1 ) / maxstep );

    *lo = lon1;
    *la = lat1;

  }


protected:

  /**
   * \brief The OSM map data stored in a shared memory segment.
   */
  boost::interprocess::offset_ptr<justine::robocar::shm_map_Type> shm_map;

private:

  /**
   * \brief The shared memory object.
   */
  boost::interprocess::managed_shared_memory *segment;

  void foo ( void );
  int init ( boost::asio::ip::tcp::socket & socket );
  void gangsters ( boost::asio::ip::tcp::socket & socket, int id );
  void pos ( boost::asio::ip::tcp::socket & socket, int id );
  void car ( boost::asio::ip::tcp::socket & socket, int id, unsigned *f, unsigned *t, unsigned* s );
  void route1 ( boost::asio::ip::tcp::socket & socket, int id );
  void route2 ( boost::asio::ip::tcp::socket & socket, int id );
};

}
} // justine::sampleclient::

#endif // ROBOCAR_SHMCLIENT_HPP

