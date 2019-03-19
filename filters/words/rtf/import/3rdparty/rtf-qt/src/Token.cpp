/*
    Copyright (C)  2010  Brad Hards <bradh@frogmouth.net>

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2.1 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Token.h"
#include "rtfdebug.h"

namespace RtfReader
{
    void Token::dump() const
    {
        switch ( type ) {
	  case OpenGroup:
            qCDebug(lcRtf) << "token type: OpenGroup";
	    break;
	  case CloseGroup:
            qCDebug(lcRtf) << "token type: CloseGroup";
	    break;
	  case Control:
            qCDebug(lcRtf) << "token type: Control";
	    break;
	  case Plain:
            qCDebug(lcRtf) << "token type: Plain";
	    break;
	  case Binary:
            qCDebug(lcRtf) << "token type: Binary";
	    break;
	  default:
            qCDebug(lcRtf) << "unexpected token type: " << type;
	}
	if ( type > CloseGroup ) {
            qCDebug(lcRtf) << "name: " << name;
	    if ( hasParameter ) {
                qCDebug(lcRtf) << "parameter: " << parameter;
	    }
	}
    }
}
