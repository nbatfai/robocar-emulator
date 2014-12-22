#ifndef ROBOCAR_CARLEXER_HPP
#define ROBOCAR_CARLEXER_HPP

/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file carlexer.hpp
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

#ifndef __FLEX_LEXER_H
#include <FlexLexer.h>
#endif

#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <vector>

namespace justine
{
namespace robocar
{

class CarLexer : public yyFlexLexer
{
public:

  virtual int yylex ();

  friend std::ostream & operator<< ( std::ostream & os, CarLexer & cl )
  {
    os << cl.name
       << " "
       << cl.role
       << std::endl;

    return os;
  }

  char* get_name()
  {
    return name;
  }
  char get_role() const
  {
    return role;
  }
  int get_num() const
  {
    return num;
  }
  int get_errnumber() const
  {
    return m_errnumber;
  }
  bool get_guided() const
  {
    return m_guided;
  }
  int get_cmd() const
  {
    return m_cmd;
  }
  int get_id() const
  {
    return m_id;
  }
  std::vector<unsigned int> & get_route ( void )
  {
    return route;
  }
  unsigned int get_from() const
  {
    return from;
  }
  unsigned int get_to() const
  {
    return to;
  }

private:
  int m_cmd {0};
  char name[128];
  int num {0};
  char role;
  int m_errnumber {0};
  bool m_guided {false};
  std::vector<unsigned int> route;
  int m_id {0};
  unsigned int from {0u};
  unsigned int to {0u};

};

}
} // justine::robocar::

#endif
