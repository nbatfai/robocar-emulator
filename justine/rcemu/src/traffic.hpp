#ifndef ROBOCAR_TRAFFIC_HPP
#define ROBOCAR_TRAFFIC_HPP

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

#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>
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

/*
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
*/

#include <memory>

#include <carlexer.hpp>

#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

namespace justine
{
namespace robocar
{


enum class TrafficType: unsigned int
{
  NORMAL=0, ANT, ANT_RND, ANT_RERND, ANT_MRERND
};

class Traffic
{
public:

  Traffic ( int size, const char * shm_segment, double catchdist, TrafficType type = TrafficType::NORMAL, int minutes = 10 )
    :m_size ( size ), m_catchdist ( catchdist ), m_type ( type ), m_minutes ( minutes )
  {

#ifdef DEBUG
    std::cout << "Attaching shared memory segment called "
              << shm_segment
              << "... " << std::endl;
#endif

    segment = new boost::interprocess::managed_shared_memory (
      boost::interprocess::open_only,
      shm_segment );

    shm_map =
      segment->find<shm_map_Type> ( "JustineMap" ).first;

#ifdef DEBUG
    std::cout << "Initializing routine cars ... " << std::endl;
#endif

    if ( type != TrafficType::NORMAL )
      for ( shm_map_Type::iterator iter=shm_map->begin();
            iter!=shm_map->end(); ++iter )
        {

          for ( auto noderef : iter->second.m_alist )
            {
              AntCar::alist[iter->first].push_back ( 1 );
              AntCar::alist_evaporate[iter->first].push_back ( 1 );
            }
        }

    for ( int i {0}; i < m_size; ++i )
      {

        //std::unique_ptr<Car> car(std::make_unique<Car>(*this)); //14, 4.9
        //std::unique_ptr<Car> car(new Car {*this});

        if ( type == TrafficType::NORMAL )
          {
            std::shared_ptr<Car> car ( new Car {*this} );

            car->init();
            cars.push_back ( car );
          }
        else
          {
            std::shared_ptr<AntCar> car ( new AntCar {*this} );

            car->init();
            cars.push_back ( car );

          }


      }

#ifdef DEBUG
    std::cout << "All routine cars initialized." <<"\n";
#endif

    boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

    logfile = boost::posix_time::to_simple_string ( now );
    logFile = new std::fstream ( logfile.c_str() , std::ios_base::out );

    m_cv.notify_one();

    std::cout << "The traffic server is ready." << std::endl;

  }

  ~Traffic()
  {

    m_run = false;
    m_thread.join();
    segment->destroy<shm_map_Type> ( "JustineMap" );
    delete segment;
  }

  void processes ( )
  {
    std::unique_lock<std::mutex> lk ( m_mutex );
    m_cv.wait ( lk );

#ifdef DEBUG
    std::cout << "Traffic simul started." << std::endl;
#endif


    for ( ; m_run; )
      {

        if ( ++m_time > ( m_minutes*60*1000 ) /m_delay )
          {
            m_run = false;
            break;
          }
        else
          {
            traffic_run();
            std::this_thread::sleep_for ( std::chrono::milliseconds ( m_delay ) );
          }

      }

    std::cout << "The traffic simulation is over." << std::endl;

    for ( auto c:m_cop_cars )
      *logFile  << *c << std::endl;

    logFile->close ();

    boost::filesystem::rename (
      boost::filesystem::path ( logfile ),
      boost::filesystem::path ( get_title ( logfile ) ) );

  }

  std::string get_title ( std::string name )
  {

    std::map <std::string, int> res;
    for ( auto c:m_cop_cars )
      {
        res[c->get_name()] += c->get_num_captured_gangsters();
      }

    std::ostringstream ss;

    for ( auto r: res )
      ss << r.first << " " << res[r.first] << " ";

    ss << name << ".txt";

    return ss.str();
  }

  osmium::unsigned_object_id_type virtual node()
  {

    shm_map_Type::iterator iter=shm_map->begin();
    std::advance ( iter, std::rand() % shm_map->size() );

    return iter->first;
  }

  virtual void traffic_run()
  {

    // activities that may occur in the traffic flow

    // std::cout << *this;

    pursuit();

    steps();

  }

  void steps()
  {

    std::lock_guard<std::mutex> lock ( cars_mutex );

    *logFile <<
             m_time <<
             " " <<
             m_minutes <<
             " " <<
             cars.size()
             << std::endl;

    for ( auto car:cars )
      {
        car->step();

        *logFile << *car
                 <<  " " << std::endl;

      }
  }

  inline void pursuit ( void )
  {

    for ( auto car1:m_cop_cars )
      {

        double lon1 {0.0}, lat1 {0.0};
        toGPS ( car1->from(), car1->to() , car1->get_step(), &lon1, &lat1 );

        double lon2 {0.0}, lat2 {0.0};
        for ( auto car:m_smart_cars )
          {

            if ( car->get_type() == CarType::GANGSTER )
              {

                toGPS ( car->from(), car->to() , car->get_step(), &lon2, &lat2 );
                double d = dst ( lon1, lat1, lon2, lat2 );

                if ( d < m_catchdist )
                  {

                    car1->captured_gangster();
                    car->set_type ( CarType::CAUGHT );

                  }
              }
          }
      }
  }

  size_t nedges ( osmium::unsigned_object_id_type from ) const
  {
    shm_map_Type::iterator iter=shm_map->find ( from );
    return iter->second.m_alist.size();
  }

  osmium::unsigned_object_id_type alist ( osmium::unsigned_object_id_type from, int to ) const
  {
    shm_map_Type::iterator iter=shm_map->find ( from );
    return iter->second.m_alist[to];
  }

  int alist_inv ( osmium::unsigned_object_id_type from, osmium::unsigned_object_id_type to ) const
  {
    shm_map_Type::iterator iter=shm_map->find ( from );

    int ret = -1;

    for ( uint_vector::iterator noderefi = iter->second.m_alist.begin();
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
    shm_map_Type::iterator iter=shm_map->find ( from );
    return iter->second.m_salist[to];

  }
  void set_salist ( osmium::unsigned_object_id_type from, int to , osmium::unsigned_object_id_type value )
  {
    shm_map_Type::iterator iter=shm_map->find ( from );
    iter->second.m_salist[to] = value;

  }
  osmium::unsigned_object_id_type palist ( osmium::unsigned_object_id_type from, int to ) const
  {
    shm_map_Type::iterator iter=shm_map->find ( from );
    return iter->second.m_palist[to];
  }

  bool hasNode ( osmium::unsigned_object_id_type node )
  {
    shm_map_Type::iterator iter=shm_map->find ( node );
    return ! ( iter == shm_map->end() );
  }

  void start_server ( boost::asio::io_service& io_service, unsigned short port );

  void cmd_session ( boost::asio::ip::tcp::socket sock );

  friend std::ostream & operator<< ( std::ostream & os, Traffic & t )
  {

    os << t.m_time <<
       " " <<
       t.shm_map->size()
       << std::endl;

    for ( shm_map_Type::iterator iter=t.shm_map->begin();
          iter!=t.shm_map->end(); ++iter )
      {

        os  << iter->first
            << " "
            << iter->second.lon
            << " "
            << iter->second.lat
            << " "
            << iter->second.m_alist.size()
            << " ";

        for ( auto noderef : iter->second.m_alist )
          os  << noderef
              << " ";

        for ( auto noderef : iter->second.m_salist )
          os  << noderef
              << " ";

        for ( auto noderef : iter->second.m_palist )
          os  << noderef
              << " ";

        os << std::endl;

      }

    return os;

  }

  osmium::unsigned_object_id_type naive_node_for_nearest_gangster ( osmium::unsigned_object_id_type from,
      osmium::unsigned_object_id_type to,
      osmium::unsigned_object_id_type step );
  double dst ( osmium::unsigned_object_id_type n1, osmium::unsigned_object_id_type n2 ) const;
  double dst ( double lon1, double lat1, double lon2, double lat2 ) const;
  void toGPS ( osmium::unsigned_object_id_type from,
               osmium::unsigned_object_id_type to,
               osmium::unsigned_object_id_type step, double *lo, double *la ) const;
  osmium::unsigned_object_id_type naive_nearest_gangster ( osmium::unsigned_object_id_type from,
      osmium::unsigned_object_id_type to,
      osmium::unsigned_object_id_type step );

  TrafficType get_type() const
  {
    return m_type;
  }

  int get_time() const
  {
    return m_time;
  }
  
protected:

  boost::interprocess::managed_shared_memory *segment;
  boost::interprocess::offset_ptr<shm_map_Type> shm_map;

  int m_delay {200};
  bool m_run {true};
  double m_catchdist {15.5};

private:

  int addCop ( CarLexer& cl );
  int addGangster ( CarLexer& cl );

  int m_size {10000};
  int m_time {0};
  int m_minutes {10};
  std::mutex m_mutex;
  std::condition_variable m_cv;
  std::thread m_thread {&Traffic::processes, this};

  std::vector<std::shared_ptr<Car>> cars;
  std::vector<std::shared_ptr<SmartCar>> m_smart_cars;
  std::vector<std::shared_ptr<CopCar>> m_cop_cars;
  std::map<int, std::shared_ptr<SmartCar>> m_smart_cars_map;

  std::mutex cars_mutex;

  TrafficType m_type {TrafficType::NORMAL};

  std::fstream* logFile;
  std::string logfile;
};

}
} // justine::robocar::

#endif // ROBOCAR_TRAFFIC_HPP

