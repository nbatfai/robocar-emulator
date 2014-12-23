#ifndef ROBOCAR_SMARTCITY_HPP
#define ROBOCAR_SMARTCITY_HPP

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

#include <osmreader.hpp>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>

#include <exception>
#include <stdexcept>

#include <iomanip>

namespace justine
{
namespace robocar
{

typedef boost::interprocess::managed_shared_memory::segment_manager segment_manager_Type;
typedef boost::interprocess::allocator<void, segment_manager_Type> void_allocator;
typedef boost::interprocess::allocator<unsigned int, segment_manager_Type> uint_allocator;
typedef boost::interprocess::vector<unsigned int, uint_allocator> uint_vector;
typedef boost::interprocess::allocator<uint_vector, segment_manager_Type> uint_vector_allocator;

class SharedData
{

public:
  uint_vector m_alist;
  uint_vector m_salist;
  uint_vector m_palist;

  int lon;
  int lat;

  SharedData ( const void_allocator &void_alloc )
    :  m_alist ( void_alloc ), m_salist ( void_alloc ), m_palist ( void_alloc )
  {}
};

typedef std::pair<const unsigned int, SharedData> map_pair_Type;
typedef boost::interprocess::allocator<map_pair_Type, segment_manager_Type> map_pair_Type_allocator;
typedef boost::interprocess::map< unsigned int, SharedData, std::less<unsigned int>,
        map_pair_Type_allocator> shm_map_Type;

class SmartCity
{
public:

  SmartCity ( const char * osm_file, const char * shm_segment, const char * map_file ) : SmartCity ( osm_file, shm_segment )
  {

    std::fstream gpsFile ( map_file, std::ios_base::out );

    for ( auto loc: m_waynode_locations )
      gpsFile << loc.first
              << " " << loc.second.lat()
              << " " << loc.second.lon() << std::endl;

    gpsFile.close ();

  }

  SmartCity ( const char * osm_file, const char * shm_segment )
  {

    AdjacencyList alist, palist;

    std::size_t estimated_size;

    try
      {
#ifdef DEBUG
        auto start = std::chrono::high_resolution_clock::now();
#endif

        OSMReader osm_reader ( osm_file, alist, palist,
                               m_waynode_locations,
                               m_busWayNodesMap,
                               m_way2nodes );
        estimated_size = 20*3*osm_reader.get_estimated_memory();

#ifdef DEBUG
        std::cout << " Processing OSM: "
                  << std::chrono::duration_cast<std::chrono::milliseconds> (
                    std::chrono::high_resolution_clock::now() - start ).count()
                  << " ms " << std::endl;
#endif
      }
    catch ( std::exception &err )
      {

        m_cv.notify_one();

        m_run = false;
        m_thread.join();

        throw;

      }

    google::protobuf::ShutdownProtobufLibrary();

    m_remover = new shm_remove ( shm_segment );

    segment = new boost::interprocess::managed_shared_memory (
      boost::interprocess::create_only,
      shm_segment,
      estimated_size );

    void_allocator  alloc_obj ( segment->get_segment_manager() );

    shm_map_Type* shm_map_n =
      segment->construct<shm_map_Type>
      ( "JustineMap" ) ( std::less<unsigned int>(), alloc_obj );

    try
      {

        for ( AdjacencyList::iterator iter=alist.begin();
              iter!=alist.end(); ++iter )
          {

            SharedData v ( alloc_obj );

            /*
                       v.lon = m_waynode_locations.get ( iter->first ).x();
                       v.lat = m_waynode_locations.get ( iter->first ).y();
            */
            v.lon = m_waynode_locations[ iter->first ].x();
            v.lat = m_waynode_locations[ iter->first ].y();

            for ( WayNodesVect::iterator noderefi = iter->second.begin();
                  noderefi!= iter->second.end(); ++noderefi )
              {

                v.m_alist.push_back ( *noderefi );
                v.m_salist.push_back ( 0u );
                v.m_palist.push_back ( palist[iter->first][std::distance ( iter->second.begin(), noderefi )]+1 );
              }

            map_pair_Type p ( iter->first, v );
            shm_map_n->insert ( p );
          }

#ifdef DEBUG
        std::cout << " alist.size = " << alist.size() << " (deg- >= 1)"<< std::endl;
        std::cout << " SHM/alist.size = " << shm_map_n->size() << std::endl;
#endif


      }
    catch ( boost::interprocess::bad_alloc e )
      {

        std::cerr << " Out of shared memory..." << std::cerr;
        std::cout << e.what() <<std::endl;

        std::cerr
            << " Shared memory usage: "
            << segment->get_free_memory() /1024.0/1024.0 << " Mbytes "
            << std::setprecision ( 2 )
            << 100.0- ( 100.0*segment->get_free_memory() ) /segment->get_size()
            << "% is free"
            << std::endl;

        m_cv.notify_one();

        m_run = false;
        m_thread.join();

        throw e;
      }

#ifdef DEBUG
    std::streamsize p = std::cout.precision();
    std::cout
        << " Shared memory usage: "
        << segment->get_free_memory() /1024.0/1024.0 << " Mbytes "
        << std::setprecision ( 2 )
        << 100.0- ( 100.0*segment->get_free_memory() ) /segment->get_size()
        << "% is free"
        << std::setprecision ( p )
        << std::endl;
#endif

    shm_map = segment->find<shm_map_Type> ( "JustineMap" ).first;

    m_cv.notify_one();
  }

  ~SmartCity()
  {

    m_run = false;
    m_thread.join();
    delete segment;
    delete m_remover;
  }

  void processes ( )
  {
    std::unique_lock<std::mutex> lk ( m_mutex );
    m_cv.wait ( lk );

    for ( ; m_run; )
      {
        std::this_thread::sleep_for ( std::chrono::milliseconds ( m_delay ) );
        city_run();
      }

  }

  friend std::ostream & operator<< ( std::ostream & os, SmartCity & t )
  {

    for ( shm_map_Type::iterator iter=t.shm_map->begin();
          iter!=t.shm_map->end(); ++iter )
      {

        std::cout
            << iter->first
            << " "
            << iter->second.lon
            << " "
            << iter->second.lat
            << " "
            << iter->second.m_alist.size()
            << " ";

        for ( auto noderef : iter->second.m_alist )
          std::cout
              << noderef
              << " ";

        for ( auto noderef : iter->second.m_salist )
          std::cout
              << noderef
              << " ";

        for ( auto noderef : iter->second.m_palist )
          std::cout
              << noderef
              << " ";

        std::cout << std::endl;

      }

    return os;

  }

  virtual void city_run()
  {

    // activities that may occur in the city

    // std::cout << *this;

  }

  double busWayLength ( bool verbose );

protected:

  boost::interprocess::managed_shared_memory *segment;
  boost::interprocess::offset_ptr<shm_map_Type> shm_map;

  int m_delay {5000};
  bool m_run {true};

private:

  std::mutex m_mutex;
  std::condition_variable m_cv;
  std::thread m_thread {&SmartCity::processes, this};

  WaynodeLocations m_waynode_locations;
  WayNodesMap m_busWayNodesMap;
  Way2Nodes m_way2nodes;

  struct shm_remove
  {
    const char * name;
    shm_remove ( const char * name )
    {
      boost::interprocess::shared_memory_object::remove ( name );
    }
    ~shm_remove()
    {
      boost::interprocess::shared_memory_object::remove ( name );
    }
  } * m_remover;
};

}
} // justine::robocar::


#endif // ROBOCAR_SMARTCITY_HPP



