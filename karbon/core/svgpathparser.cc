/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "svgpathparser.h"
#include <math.h>
#include <qregexp.h>

// parses the coord into number and forwards to the next token
const char *
SVGPathParser::getCoord( const char *ptr, double &number )
{
	int integer, exponent;
	double decimal, frac;
	int sign, expsign;

	exponent = 0;
	integer = 0;
	frac = 1.0;
	decimal = 0;
	sign = 1;
	expsign = 1;

	// read the sign
	if(*ptr == '+')
		ptr++;
	else if(*ptr == '-')
	{
		ptr++;
		sign = -1;
	}

	// read the integer part
	while(*ptr != '\0' && *ptr >= '0' && *ptr <= '9')
		integer = (integer * 10) + *(ptr++) - '0';
	if(*ptr == '.') // read the decimals
    {
		ptr++;
		while(*ptr != '\0' && *ptr >= '0' && *ptr <= '9')
			decimal += (*(ptr++) - '0') * (frac *= 0.1);
    }

	if(*ptr == 'e' || *ptr == 'E') // read the exponent part
	{
		ptr++;

		// read the sign of the exponent
		if(*ptr == '+')
			ptr++;
		else if(*ptr == '-')
		{
			ptr++;
			expsign = -1;
		}

		exponent = 0;
		while(*ptr != '\0' && *ptr >= '0' && *ptr <= '9')
		{
			exponent *= 10;
			exponent += *ptr - '0';
			ptr++;
		}
    }
	number = integer + decimal;
	number *= sign * pow(10, expsign * exponent);

	// skip the following space
	if(*ptr == ' ')
		ptr++;

	return ptr;
}

void
SVGPathParser::parseSVG( const QString &s )
{
	QString d = s;
	d = d.replace( QRegExp( "," ), " ");

	if( !d.isEmpty() )
	{
		d = d.simplifyWhiteSpace();

		const char *ptr = d.latin1();
		const char *end = d.latin1() + d.length() + 1;

		double contrlx, contrly, curx, cury, subpathx, subpathy, tox, toy, x1, y1, x2, y2, xc, yc;
		double px1, py1, px2, py2, px3, py3;
		bool relative;
		char command = *(ptr++), lastCommand = ' ';

		subpathx = subpathy = curx = cury = contrlx = contrly = 0.0;
		while( ptr < end )
		{
			if( *ptr == ' ' )
				ptr++;

			relative = false;

			//std::cout << "Command : " << command << std::endl;
			switch( command )
			{
				case 'm':
					relative = true;
				case 'M':
				{
					ptr = getCoord( ptr, tox );
					ptr = getCoord( ptr, toy );

					subpathx = curx = relative ? curx + tox : tox;
					subpathy = cury = relative ? cury + toy : toy;

					//if( lastCommand == 'z' || lastCommand == 'Z' )
					//	path->close();
					svgMoveTo( curx, cury );
					break;
				}
				case 'l':
					relative = true;
				case 'L':
				{
					ptr = getCoord( ptr, tox );
					ptr = getCoord( ptr, toy );

					curx = relative ? curx + tox : tox;
					cury = relative ? cury + toy : toy;

					svgLineTo( curx, cury );
					break;
				}
				case 'h':
				{
					ptr = getCoord( ptr, tox );
					curx = curx + tox;
					svgLineTo( curx, cury );
					break;
				}
				case 'H':
				{
					ptr = getCoord( ptr, tox );
					curx = tox;
					svgLineTo( curx, cury );
					break;
				}
				case 'v':
				{
					ptr = getCoord( ptr, toy );
					cury = cury + toy;
					svgLineTo( curx, cury );
					break;
				}
				case 'V':
				{
					ptr = getCoord( ptr, toy );
					cury = toy;
					svgLineTo( curx, cury );
					break;
				}
				case 'z':
				case 'Z':
				{
					// reset curx, cury for next path
					curx = subpathx;
					cury = subpathy;
					svgClosePath();
					break;
				}
				case 'c':
					relative = true;
				case 'C':
				{
					ptr = getCoord( ptr, x1 );
					ptr = getCoord( ptr, y1 );
					ptr = getCoord( ptr, x2 );
					ptr = getCoord( ptr, y2 );
					ptr = getCoord( ptr, tox );
					ptr = getCoord( ptr, toy );

					px1 = relative ? curx + x1 : x1;
					py1 = relative ? cury + y1 : y1;
					px2 = relative ? curx + x2 : x2;
					py2 = relative ? cury + y2 : y2;
					px3 = relative ? curx + tox : tox;
					py3 = relative ? cury + toy : toy;

					svgCurveTo( px1, py1, px2, py2, px3, py3 );

					contrlx = relative ? curx + x2 : x2;
					contrly = relative ? cury + y2 : y2;
					curx = relative ? curx + tox : tox;
					cury = relative ? cury + toy : toy;

					break;
				}
				case 's':
					relative = true;
				case 'S':
				{
					ptr = getCoord( ptr, x2 );
					ptr = getCoord( ptr, y2 );
					ptr = getCoord( ptr, tox );
					ptr = getCoord( ptr, toy );

					px1 = 2 * curx - contrlx;
					py1 = 2 * cury - contrly;
					px2 = relative ? curx + x2 : x2;
					py2 = relative ? cury + y2 : y2;
					px3 = relative ? curx + tox : tox;
					py3 = relative ? cury + toy : toy;

					svgCurveTo( px1, py1, px2, py2, px3, py3 );

					contrlx = relative ? curx + x2 : x2;
					contrly = relative ? cury + y2 : y2;
					curx = relative ? curx + tox : tox;
					cury = relative ? cury + toy : toy;
					break;
				}
				case 'q':
					relative = true;
				case 'Q':
				{
					ptr = getCoord( ptr, x1 );
					ptr = getCoord( ptr, y1 );
					ptr = getCoord( ptr, tox );
					ptr = getCoord( ptr, toy );

					px1 = relative ? (curx + 2 * (x1 + curx)) * (1.0 / 3.0) : (curx + 2 * x1) * (1.0 / 3.0);
					py1 = relative ? (cury + 2 * (y1 + cury)) * (1.0 / 3.0) : (cury + 2 * y1) * (1.0 / 3.0);
					px2 = relative ? ((curx + tox) + 2 * (x1 + curx)) * (1.0 / 3.0) : (tox + 2 * x1) * (1.0 / 3.0);
					py2 = relative ? ((cury + toy) + 2 * (y1 + cury)) * (1.0 / 3.0) : (toy + 2 * y1) * (1.0 / 3.0);
					px3 = relative ? curx + tox : tox;
					py3 = relative ? cury + toy : toy;

					svgCurveTo( px1, py1, px2, py2, px3, py3 );

					contrlx = relative ? curx + x1 : (tox + 2 * x1) * (1.0 / 3.0);
					contrly = relative ? cury + y1 : (toy + 2 * y1) * (1.0 / 3.0);
					curx = relative ? curx + tox : tox;
					cury = relative ? cury + toy : toy;
					break;
				}
				case 't':
					relative = true;
				case 'T':
				{
					xc = 2 * curx - contrlx;
	                yc = 2 * cury - contrly;

					ptr = getCoord(ptr, tox);
					ptr = getCoord(ptr, toy);

					px1 = relative ? (curx + 2 * xc) * (1.0 / 3.0) : (curx + 2 * xc) * (1.0 / 3.0);
					py1 = relative ? (cury + 2 * yc) * (1.0 / 3.0) : (cury + 2 * yc) * (1.0 / 3.0);
					px2 = relative ? ((curx + tox) + 2 * xc) * (1.0 / 3.0) : (tox + 2 * xc) * (1.0 / 3.0);
					py2 = relative ? ((cury + toy) + 2 * yc) * (1.0 / 3.0) : (toy + 2 * yc) * (1.0 / 3.0);
					px3 = relative ? curx + tox : tox;
					py3 = relative ? cury + toy : toy;

					svgCurveTo( px1, py1, px2, py2, px3, py3 );

					contrlx = xc;
					contrly = yc;
					curx = relative ? curx + tox : tox;
					cury = relative ? cury + toy : toy;
					break;
				}
				// TODO : arc support
				case 'a':
					relative = true;
				case 'A':
				{
					bool largeArc, sweep;
					double angle, rx, ry;
					ptr = getCoord( ptr, rx );
					ptr = getCoord( ptr, ry );
					ptr = getCoord( ptr, angle );
					ptr = getCoord( ptr, tox );

					largeArc = tox == 1;
					ptr = getCoord( ptr, tox );
					sweep = tox == 1;
					ptr = getCoord( ptr, tox );
					ptr = getCoord( ptr, toy );

					/*if(relative)
						pathSegList()->appendItem(createSVGPathSegArcRel(tox, toy, rx, ry, angle, largeArc, sweep));
					else
						pathSegList()->appendItem(createSVGPathSegArcAbs(tox, toy, rx, ry, angle, largeArc, sweep));*/
				}
			}

			lastCommand = command;

			if(*ptr == '+' || *ptr == '-' || (*ptr >= '0' && *ptr <= '9'))
			{
				// there are still coords in this command
				if(command == 'M')
					command = 'L';
				else if(command == 'm')
					command = 'l';
			}
			else
				command = *(ptr++);

			if( lastCommand != 'C' && lastCommand != 'c' &&
				lastCommand != 'S' && lastCommand != 's' &&
				lastCommand != 'Q' && lastCommand != 'q' &&
				lastCommand != 'T' && lastCommand != 't')
			{
				contrlx = curx;
				contrly = cury;
			}
		}
	}
}

