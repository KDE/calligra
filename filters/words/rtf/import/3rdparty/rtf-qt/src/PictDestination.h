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

#ifndef RTFREADER_PICTDESTINATION_H
#define RTFREADER_PICTDESTINATION_H

#include <QString>
#include <QColor>

#include "Destination.h"
#include "rtfreader_export.h"
namespace RtfReader
{
    class Reader;

    class RTFREADER_EXPORT PictDestination: public Destination
    {
      public:
	PictDestination( Reader *reader, AbstractRtfOutput *output, const QString &name );

	~PictDestination() override;

	void handleControlWord( const QByteArray &controlWord, bool hasValue, const int value ) override;
	void handlePlainText( const QByteArray &plainText ) override;
	void aboutToEndDestination() override;

      private:
	// The hexadecimal version of the data for the image that is currently being built
	QByteArray m_pictData;
	
	// The format information for the current image
	QTextImageFormat m_imageFormat;
	const char *m_format = nullptr;
	qreal m_xScale = 1.;
	qreal m_yScale = 1.;
	qreal m_goalWidth = 0.;
	qreal m_goalHeight = 0.;
      };
}

#endif
