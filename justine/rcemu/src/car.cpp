/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file car.cpp
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
 * GNU Robocar City Emulator and Robocar World Championship
 *
 * desc
 *
 */

#include <car.hpp>
#include <traffic.hpp>
#include <boost/iterator/iterator_concepts.hpp>

justine::robocar::Car::Car (
  justine::robocar::Traffic & traffic,
  justine::robocar::CarType type ) :  traffic ( traffic ), m_type ( type )
{

}

justine::robocar::SmartCar::SmartCar ( justine::robocar::Traffic & traffic,
                                       justine::robocar::CarType type,
                                       bool guided ) : justine::robocar::Car ( traffic, type ), m_guided ( guided )
{

}

justine::robocar::CopCar::CopCar ( justine::robocar::Traffic & traffic,
                                   bool guided ) : justine::robocar::SmartCar ( traffic, CarType::POLICE, guided )
{

}

void justine::robocar::Car::init()
{

  m_from = traffic.node();
  m_to = 0;
  m_step = 0;
  traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to ) +1 );

}

void justine::robocar::SmartCar::init()
{

  if ( m_guided )
    {

      osmium::unsigned_object_id_type ini {2969934868};

      if ( traffic.hasNode ( ini ) )
        {
          m_from = ini;
        }
      else
        {
          m_from = traffic.node();
        }

      m_to = 0;
      m_step = 0;
      traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to ) +1 );

    }
  else
    Car::init();

}


void justine::robocar::Car::nextSmarterEdge ( void )
{
  osmium::unsigned_object_id_type next_m_from = traffic.alist ( m_from, m_to );
  size_t nes = traffic.nedges ( next_m_from );
  if ( !nes )
    return;
  osmium::unsigned_object_id_type next_m_to = std::rand() % nes;

  if ( traffic.alist ( next_m_from, next_m_to ) == m_from )
    next_m_to = ( next_m_to + 1 ) % nes;

  if ( traffic.palist ( next_m_from, next_m_to ) >
       traffic.salist ( next_m_from, next_m_to ) )
    {

      traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to )-1 );

      m_from = next_m_from;
      m_to = next_m_to;
      m_step = 0;

      traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to ) +1 );
    }
}

void justine::robocar::Car::nextEdge ( void )
{
  osmium::unsigned_object_id_type next_m_from = traffic.alist ( m_from, m_to );
  size_t nes = traffic.nedges ( next_m_from );
  if ( !nes )
    return;
  osmium::unsigned_object_id_type next_m_to = std::rand() % nes;

  if ( traffic.palist ( next_m_from, next_m_to ) >
       traffic.salist ( next_m_from, next_m_to ) )
    {

      traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to )-1 );

      m_from = next_m_from;
      m_to = next_m_to;
      m_step = 0;

      traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to ) +1 );
    }
}

void justine::robocar::Car::step()
{

  if ( traffic.hasNode ( to_node() ) )
    {

      if ( m_step >= traffic.palist ( m_from, m_to ) )
        {

          nextSmarterEdge();

        }
      else
        ++m_step;


    }
  else
    {

      // car stopped

    }
}

void justine::robocar::SmartCar::nextEdge ( void )
{

  if ( traffic.hasNode ( to_node() ) )
    {

      if ( m_step >= traffic.palist ( m_from, m_to ) )
        {

          osmium::unsigned_object_id_type next_m_from
            = traffic.alist ( m_from, m_to );
          osmium::unsigned_object_id_type next_m_to
            = traffic.naive_node_for_nearest_gangster ( m_from, m_to, m_step );

          if ( traffic.palist ( next_m_from, next_m_to ) >
               traffic.salist ( next_m_from, next_m_to ) )
            {

              traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to )-1 );

              m_from = next_m_from;
              m_to = next_m_to;
              m_step = 0;

              traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to ) +1 );
            }

        }
      else
        ++m_step;

    }
  else
    {

      // car stopped

    }

}

void justine::robocar::SmartCar::nextGuidedEdge ( void )
{
  if ( traffic.hasNode ( to_node() ) )
    {

      if ( m_step >= traffic.palist ( m_from, m_to ) )
        {

          std::vector<unsigned int>::iterator i = std::find ( route.begin(), route.end(), to_node() );

          if ( i == route.end() )
            return;

          osmium::unsigned_object_id_type next_m_to;
          osmium::unsigned_object_id_type next_m_from;

          if ( std::distance ( route.begin(), i ) == route.size() )
            {

              next_m_to = 0;
              next_m_from = to_node();

            }
          else
            {

              next_m_to;
              osmium::unsigned_object_id_type inv = traffic.alist_inv ( to_node(), * ( i+1 ) );

              if ( inv != -1 )
                {
                  next_m_to = inv;
                }
              else
                {
                  return;
                }

              next_m_from = to_node();
            }

          if ( traffic.palist ( next_m_from, next_m_to ) >=
               traffic.salist ( next_m_from, next_m_to ) )
            {

              traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to )-1 );

              m_from = next_m_from;
              m_to = next_m_to;
              m_step = 0;

              traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to ) +1 );
            }
          else
            {
              ;

            }

        }
      else
        ++m_step;

    }
  else
    {

      // car stopped

    }

}


void justine::robocar::SmartCar::step()
{

  if ( m_type == CarType::POLICE )
    {

      if ( m_guided )
        nextGuidedEdge();
      else
        nextEdge();

    }
  else if ( m_type == CarType::CAUGHT )
    {

      return;

    }
  else
    Car::step();

}

osmium::unsigned_object_id_type justine::robocar::Car::to_node() const
{
  return traffic.alist ( m_from, m_to );
}

osmium::unsigned_object_id_type justine::robocar::Car::get_max_steps() const
{
  return traffic.palist ( m_from, m_to );
}

void justine::robocar::SmartCar::set_route ( std::vector<unsigned int> & route )
{
  this->route = route;
  m_routed = true;
  
  if ( m_from == route[0] )
    {
      int next_m_to = traffic.alist_inv ( m_from, route[1] );

      if ( next_m_to != m_to )
        {
          m_to = next_m_to;
          m_step = 0;
        }
    }
}

bool justine::robocar::SmartCar::set_fromto ( unsigned int from, unsigned int to )
{
  if ( m_from == from && traffic.hasNode ( to ) )
    {

      traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to ) -1 );

      m_to = traffic.alist_inv ( m_from, to );

      traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to ) +1 );

      return true;
    }

  return false;
}
