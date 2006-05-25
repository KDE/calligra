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

#ifndef __ROUNDCORNERSPLUGIN_H__
#define __ROUNDCORNERSPLUGIN_H__

#include <kparts/plugin.h>
#include <kdialogbase.h>
#include <commands/vreplacingcmd.h>
#include <koffice_export.h>

class KarbonView;
class VRoundCornersDlg;

class KARBONBASE_EXPORT VRoundCornersPlugin : public KParts::Plugin
{
	Q_OBJECT
public:
	VRoundCornersPlugin( KarbonView *parent, const QStringList & );
	virtual ~VRoundCornersPlugin();

private slots:
	void slotRoundCorners();

private:
	VRoundCornersDlg	*m_roundCornersDlg;
};

class KDoubleNumInput;

class VRoundCornersDlg : public KDialogBase
{
	Q_OBJECT

public:
	VRoundCornersDlg( QWidget* parent = 0L, const char* name = 0L );

	double radius() const;
	void setRadius( double value );

private:
	KDoubleNumInput* m_radius;
};

class VSubpath;

class VRoundCornersCmd : public VReplacingCmd
{
public:
	VRoundCornersCmd( VDocument* doc, double radius );
	virtual ~VRoundCornersCmd() {}

	virtual void visitVSubpath( VSubpath& path );

protected:
	double m_radius;
};

#endif

