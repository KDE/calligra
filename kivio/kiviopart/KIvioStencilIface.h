/* This file is part of the KDE project
   Copyright (C) 2001, Laurent MONTEL <lmontel@mandrakesoft.com>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KIVIO_STENCIL_IFACE_H
#define KIVIO_STENCIL_IFACE_H

#include <dcopref.h>
#include <dcopobject.h>
#include <QString>
#include <QFont>
#include <QColor>
#include <qrect.h>

class KivioStencil;

class KivioStencilIface : virtual public DCOPObject
{
    K_DCOP
public:
    KivioStencilIface(  KivioStencil *s );

k_dcop:
	virtual DCOPRef ref();
	//virtual DCOPRef duplicate();

	virtual bool connected();
	virtual void setConnected(bool c);

	virtual double x();
	virtual void setX( double f );

	virtual double y();
	virtual void setY( double f );

	virtual double w();
	virtual void setW( double f );

	virtual double h();
	virtual void setH( double f );

	virtual QRect rect();

	virtual void setPosition( double f1, double f2 );
	virtual void setDimensions( double f1, double f2 );

	virtual QColor fgColor();
	virtual void setFGColor( QColor );

	virtual void setBGColor( QColor );
	virtual QColor bgColor();

	virtual void setLineWidth( double );
	virtual double lineWidth();

	// FOnt stuff
	virtual QColor textColor();
	virtual void setTextColor( QColor );

	virtual QFont textFont();
	virtual void setTextFont( const QFont & );

	virtual int hTextAlign();
	virtual int vTextAlign();

	virtual void setHTextAlign(int);
	virtual void setVTextAlign(int);

	virtual void setText( const QString & );
	virtual QString text();

	virtual bool isSelected();
	virtual void select();
	virtual void unselect();
	virtual void subSelect( const double &, const double & );

	//virtual KivioConnectorTarget *connectToTarget( KivioConnectorPoint *, int );

private:
    KivioStencil *stencil;

};

#endif
