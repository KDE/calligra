/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef _TRISTATE_TYPE_H_
#define _TRISTATE_TYPE_H_


static const char cancelled = 2; //!< cancelled state
static const char dontKnow = cancelled; //!< the same as cancelled

//! \brief 3-state logical type with convenient operators
/*! You can use objects of this class with similar convenience as bool type:
 - use as return value when 'cancelled'
    tristate doSomething();
 - convert from bool (1) or to bool (2)
    tristate t = true; //(1)
    setVisible( t );   // (2)
 - clear comparisons
    tristate t = doSomething();
    if (t) doSomethingIfTrue();
    if (!t) doSomethingIfFalse();
    if (~t) doSomethingIfCancelled();

 You can use "! ~" as "not cancelled".

 With tristate class you can also forget 
 about it's additional meaning and treat it just as a bool.

 @author Jaroslaw Staniek
*/
class tristate
{
	public:

	/*! States used used internally. */
	enum Value { False = 0, True = 1, cancelled = 2 };

	/*! Default constructor, object have 'cancelled' value. */
	tristate()
	 : value(cancelled)
	{
	}

	/*! Constructor that takes boolean value. */
	tristate(bool boolValue)
	 : value(boolValue ? True : False)
	{
	}

	/*! Constructor that takes char value. It is converted in the following way:
	 - 2 -> cancelled
	 - 1 -> true
	 - other -> false */
	tristate(char c)
	 : value(c==::cancelled ? tristate::cancelled : (c==1 ? True : False))
	{
	}

	/*! Constructor that takes integer value. It is converted in the following way:
	 - 2 -> cancelled
	 - 1 -> true
	 - other -> false */
	tristate(int intValue)
	 : value(intValue==(int)::cancelled ? tristate::cancelled : (intValue==1 ? True : False))
	{
	}

	operator bool() const { return value==True; }

	/*! \return true is the value is equal to 'False' */
	bool operator!() const { return value==False; }

	/*! \return true is the value is equal to 'cancelled' */
	bool operator~() const { return value==cancelled; }

	friend inline bool operator!=(bool boolValue, tristate tsValue);

	Value value;
};

inline bool operator!=(bool boolValue, tristate tsValue) 
{ return !( (tsValue.value==tristate::True && boolValue) || (tsValue.value==tristate::False && !boolValue) ); }

inline bool operator!=(tristate tsValue, bool boolValue) 
{ return !( (tsValue.value==tristate::True && boolValue) || (tsValue.value==tristate::False && !boolValue) ); }

#endif
