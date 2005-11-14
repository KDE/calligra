/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KexiFrame_H
#define KexiFrame_H

#include <qframe.h>

//! Push Button widget for Kexi forms
class KEXIFORMUTILS_EXPORT KexiFrame : public QFrame
{
	Q_OBJECT
	Q_PROPERTY( QColor frameColor READ frameColor WRITE setFrameColor DESIGNABLE true )

	public:
		KexiFrame( QWidget * parent, const char * name = 0 );
		virtual ~KexiFrame();

		virtual const QColor& frameColor() const;

	public slots:
		virtual void setPalette( const QPalette &pal );
		virtual void setFrameColor(const QColor& color);

	protected:
		virtual void drawFrame( QPainter * );

		class Private;
		Private *d;
};

#endif
