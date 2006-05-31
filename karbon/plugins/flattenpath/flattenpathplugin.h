/* This file is part of the KDE project
   Copyright (C) 2002, 2003 The Karbon Developers

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

#ifndef __FLATTENPATHPLUGIN_H__
#define __FLATTENPATHPLUGIN_H__

#include <kparts/plugin.h>
#include <kdialog.h>

class KarbonView;
class VFlattenDlg;

class FlattenPathPlugin : public KParts::Plugin
{
	Q_OBJECT
public:
	FlattenPathPlugin( KarbonView *parent, const QStringList & );
	virtual ~FlattenPathPlugin() {}

private slots:
	void slotFlattenPath();

private:
	VFlattenDlg	*m_flattenPathDlg;
};

class KDoubleNumInput;

class VFlattenDlg : public KDialog
{
	Q_OBJECT

public:
	VFlattenDlg( QWidget* parent = 0L, const char* name = 0L );

	double flatness() const;
	void setFlatness( double value );

private:
	KDoubleNumInput* m_flatness;
};

#endif

