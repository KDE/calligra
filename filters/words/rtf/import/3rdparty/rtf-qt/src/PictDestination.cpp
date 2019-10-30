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
#include <QFile>
#include <QImageReader>
#include <QBuffer>
#include <QUuid>

namespace RtfReader
{
    PictDestination::PictDestination( Reader *reader, AbstractRtfOutput *output, const QString &name ) :
      Destination( reader, output, name )
    {
    }

    PictDestination::~PictDestination()
    {
    }

    void PictDestination::handleControlWord( const QByteArray &controlWord, bool hasValue, const int value )
    {
	if ( controlWord == "jpegblip" ) {
	    qCDebug(lcRtf) << "JPEG";
	    m_format = "jpg";
	} else if ( controlWord == "pngblip" ) {
	    qCDebug(lcRtf) << "PNG";
	    m_format = "png";
	} else if ( controlWord == "dibitmap" ) {
	    qCDebug(lcRtf) << "BMP";
	    m_format = "bmp";
	} else if ( controlWord == "wmetafile" ) {
            qCDebug(lcRtf) << "todo: get WMF data";
	} else if ( controlWord == "picw" ) {
            qCDebug(lcRtf) << "pict width: " << value;
	} else if ( controlWord == "pich" ) {
            qCDebug(lcRtf) << "pict height: " << value;
	} else if ( controlWord == "picscalex" ) {
            qCDebug(lcRtf) << "X scale: " << value;
	    m_xScale = value / 100.;
	} else if ( controlWord == "picscaley" ) {
            qCDebug(lcRtf) << "Y scale: " << value;
	    m_yScale = value / 100.;
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
	    m_goalHeight = value * 96 / 1440.;
	} else if ( controlWord == "picwgoal" ) {
            qCDebug(lcRtf) << "Goal Width:" << value;
	    m_goalWidth = value * 96 / 1440.;
	} else {
            qCDebug(lcRtf) << "unexpected control word in pict:" << controlWord;
	}
    }

    void PictDestination::handlePlainText( const QByteArray &plainText )
    {
	m_pictData += QByteArray::fromHex( plainText );
    }

    void PictDestination::aboutToEndDestination()
    {
        if ( m_format ) {
            static int counter = 0;

            if ( m_goalWidth == 0 || m_goalHeight == 0 ) {
                QBuffer buffer(&m_pictData);
                buffer.open(QIODevice::ReadOnly);
                QImageReader reader(&buffer);
                QSize size = reader.size();
                if ( m_goalWidth == 0 ) {
                    m_goalWidth = size.width();
                }
                if ( m_goalHeight == 0 ) {
                    m_goalHeight = size.height();
                }
            }

            m_imageFormat.setName( QStringLiteral("rtfparser://%1.%2").arg( ++counter ).arg( QString::fromUtf8( m_format) ) );
            m_imageFormat.setWidth( m_goalWidth * m_xScale );
            m_imageFormat.setHeight( m_goalHeight * m_yScale );

            m_output->createImage(m_pictData, m_imageFormat);
        } else {
            qCWarning(lcRtf) << "Embedded picture in unknown format";
        }
    }
}
