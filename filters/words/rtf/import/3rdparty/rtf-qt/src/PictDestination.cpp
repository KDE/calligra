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

#include "PictDestination.h"

#include "rtfreader.h"
#include "rtfdebug.h"

namespace RtfReader
{
    PictDestination::PictDestination( Reader *reader, AbstractRtfOutput *output, const QString &name ) :
      Destination( reader, output, name )
    {
    }

    PictDestination::~PictDestination()
    {
    }

    void PictDestination::handleControlWord( const QString &controlWord, bool hasValue, const int value )
    {
	if ( controlWord == "jpegblip" ) {
	    // handle this later
	} else if ( controlWord == "wmetafile" ) {
            qCDebug(lcRtf) << "todo: get WMF data";
	} else if ( controlWord == "picw" ) {
            qCDebug(lcRtf) << "pict width: " << value;
	    m_imageFormat.setWidth( value );
	} else if ( controlWord == "pich" ) {
            qCDebug(lcRtf) << "pict height: " << value;
	    m_imageFormat.setHeight( value );
	} else if ( controlWord == "picscalex" ) {
            qCDebug(lcRtf) << "X scale: " << value;
	} else if ( controlWord == "picscaley" ) {
            qCDebug(lcRtf) << "Y scale: " << value;
	} else if ( controlWord == "piccropl" ) {
            qCDebug(lcRtf) << "Left crop:" << value;
	} else if ( controlWord == "piccropr" ) {
            qCDebug(lcRtf) << "Right crop:" << value;
	} else if ( controlWord == "piccropt" ) {
            qCDebug(lcRtf) << "Top crop:" << value;
	} else if ( controlWord == "piccropb" ) {
            qCDebug(lcRtf) << "Bottom crop:" << value;
	} else if ( controlWord == "pichgoal" ) {
            qCDebug(lcRtf) << "Goal Height:" << value;
	} else if ( controlWord == "picwgoal" ) {
            qCDebug(lcRtf) << "Goal Width:" << value;
	} else {
            qCDebug(lcRtf) << "unexpected control word in pict:" << controlWord;
	}
    }

    void PictDestination::handlePlainText( const QString &plainText )
    {
	m_pictHexData += plainText.toLatin1();
    }

    void PictDestination::aboutToEndDestination()
    {
	QImage image = QImage::fromData( QByteArray::fromHex( m_pictHexData ) );
	m_output->createImage(image, m_imageFormat);
    }
}
