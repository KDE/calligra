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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __WHIRLPINCHPLUGIN_H__
#define __WHIRLPINCHPLUGIN_H__

#include <kdialog.h>
#include <kparts/plugin.h>
#include <commands/vcommand.h>

#include <QPointF>

class KarbonView;
class VWhirlPinchDlg;

class WhirlPinchPlugin : public KParts::Plugin
{
	Q_OBJECT
public:
	WhirlPinchPlugin( KarbonView *parent, const QStringList & );
	virtual ~WhirlPinchPlugin() {}

private slots:
	void slotWhirlPinch();

private:
	VWhirlPinchDlg	*m_whirlPinchDlg;
};

class KDoubleNumInput;

class VWhirlPinchDlg : public KDialog
{
	Q_OBJECT

public:
	VWhirlPinchDlg( QWidget* parent = 0L, const char* name = 0L );

	double angle() const;
	double pinch() const;
	double radius() const;
	void setAngle( double value );
	void setPinch( double value );
	void setRadius( double value );

private:
	KDoubleNumInput* m_angle;
	KDoubleNumInput* m_pinch;
	KDoubleNumInput* m_radius;
};

class VPath;
class VSubpath;
class VSelection;

class VWhirlPinchCmd : public VCommand
{
public:
	VWhirlPinchCmd( VDocument* doc,
		double angle, double pinch, double radius );
	virtual ~VWhirlPinchCmd();

	virtual void execute();
	virtual void unexecute();

	virtual void visitVPath( VPath& composite );
	virtual void visitVSubpath( VSubpath& path );

protected:
	VSelection* m_selection;

	QPointF m_center;
	double m_angle;
	double m_pinch;
	double m_radius;
};

#endif

