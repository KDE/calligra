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

#include "ColorTableDestination.h"

#include "rtfreader.h"
#include "rtfdebug.h"

namespace RtfReader
{
    ColorTableDestination::ColorTableDestination( Reader *reader, AbstractRtfOutput *output, const QString &name ) :
      Destination( reader, output, name ), m_currentColor(Qt::black), m_colorSet(false)
    {
    }

    ColorTableDestination::~ColorTableDestination()
    {}

    void ColorTableDestination::handleControlWord( const QByteArray &controlWord, bool hasValue, const int value )
    {
	bool handled = true;
	if ( controlWord == "red" ) {
	    m_currentColor.setRed( value );
	} else if (controlWord == "green" ) {
	    m_currentColor.setGreen( value );
	} else if (controlWord == "blue" ) {
	    m_currentColor.setBlue( value );
	} else {
	    handled = false;
            qCDebug(lcRtf) << "unexpected control word in colortbl:" << controlWord;
	}
	if ( handled ) {
	    m_colorSet = true;
	}
    }

    void ColorTableDestination::handlePlainText( const QByteArray &plainText )
    {
	if ( plainText == ";" ) {
	    m_output->appendToColourTable( m_colorSet ?  m_currentColor : QColor() );
	    resetCurrentColor();
	} else {
            qCDebug(lcRtf) << "unexpected text in ColorTableDestination:" << plainText;
	}
    }

    void ColorTableDestination::resetCurrentColor()
    {
	m_currentColor = Qt::black;
	m_colorSet = false;
    }
}
