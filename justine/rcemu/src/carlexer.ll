/*
* @brief Justine - this is a rapid prototype for development of Robocar City Emulator
*
* @file carlexer.ll
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

%option c++
%option noyywrap

%{
#define YY_DECL int justine::robocar::CarLexer::yylex()
#include "carlexer.hpp"
#include <cstdio>
#include <limits>
%}

INIT	"<init"
INITG	"<init guided"
WS	[ \t]*
WORD	[^-:\n \t()]{2,}
INT	[0123456789]+
FLOAT	[-.0123456789]+
ROUTE	"<route"
CAR	"<car"
POS	"<pos"
GANGSTERS	"<gangsters"
STAT	"<stat"
DISP	"<disp>"
%% 
{DISP}					{
					  m_cmd = 0;
					}
{POS}{WS}{INT}{WS}{INT}{WS}{INT}	{
					  std::sscanf(yytext, "<pos %d %u %u", &m_id, &from, &to);
					  m_cmd = 10001;
					}
{CAR}{WS}{INT}				{
					  std::sscanf(yytext, "<car %d", &m_id);
					  m_cmd = 1001;
					}
{STAT}{WS}{INT}				{
					  std::sscanf(yytext, "<stat %d", &m_id);
					  m_cmd = 1003;
					}
{GANGSTERS}{WS}{INT}			{
					  std::sscanf(yytext, "<gangsters %d", &m_id);
					  m_cmd = 1002;
					}
{ROUTE}{WS}{INT}{WS}{INT}({WS}{INT})*	{
				  int size{0};
				  int ss{0};
				  int sn{0};				  
				  
				  std::sscanf(yytext, "<route %d %d%n", &size, &m_id, &sn);
				  ss += sn;
				  for(int i{0}; i<size; ++i)
				  {
				    unsigned int u{0u};
				    std::sscanf(yytext+ss, "%u%n", &u, &sn);
				    route.push_back(u);
				    ss += sn; 				    
				  }
				  m_cmd = 101;
				}
{INIT}{WS}{WORD}{WS}("c"|"g")	{
				  std::sscanf(yytext, "<init %s %c>", name, &role);
				  num = 1;
				  m_cmd = 0;
				}
{INIT}{WS}{WORD}{WS}{INT}{WS}("c"|"g")	{
				  std::sscanf(yytext, "<init %s %d %c>", name, &num, &role);
				  if(num >200)
				  {
				    m_errnumber = 1;
				    num = 200;
				  }
				  m_cmd = 1;
				}				
{INITG}{WS}{WORD}{WS}("c"|"g")	{
				  std::sscanf(yytext, "<init guided %s %c>", name, &role);
				  num = 1;
				  m_guided = true;
				  m_cmd = 3;
				}
{INITG}{WS}{WORD}{WS}{INT}{WS}("c"|"g")	{				  
				  std::sscanf(yytext, "<init guided %s %d %c>", name, &num, &role);
				  if(num >200)
				  {
				    m_errnumber = 1;
				    num = 200;
				  }
				  m_guided = true;
				  m_cmd = 2;
				}								
.				{;}
%%

int yyFlexLexer::yylex(){return -1;}            


