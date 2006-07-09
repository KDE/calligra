/* This file is part of the KDE project
   Copyright (C) 2002, 2003, The Karbon Developers

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

#ifndef __SHADOWEFFECTPLUGIN_H__
#define __SHADOWEFFECTPLUGIN_H__

#include <kparts/plugin.h>
#include <kdialogbase.h>
#include <commands/vcommand.h>

class KarbonView;
class VSelection;
class VShadowEffectDlg;

class ShadowEffectPlugin : public KParts::Plugin
{
	Q_OBJECT
public:
	ShadowEffectPlugin( KarbonView *parent, const char* name, const QStringList & );
	virtual ~ShadowEffectPlugin() {}

private slots:
	void slotShadowEffect();

private:
	VShadowEffectDlg	*m_shadowEffectDlg;
};

class KIntNumInput;

class VShadowEffectDlg : public KDialogBase
{
	Q_OBJECT

public:
	VShadowEffectDlg( QWidget* parent = 0L, const char* name = 0L );

	void setAngle( int );
	void setDistance( int );
	void setOpacity( int );

	int angle() const;
	int distance() const;
	int opacity() const;

private:
	KIntNumInput	*m_angle;
	KIntNumInput	*m_distance;
	KIntNumInput	*m_opacity;
};

class VCreateShadowCmd : public VCommand
{
public:
	VCreateShadowCmd( VDocument* doc, int distance, int angle, float opacity );
	virtual ~VCreateShadowCmd();

	virtual void execute();
	virtual void unexecute();
	virtual bool changesSelection() const { return true; }
private:
	VSelection	*m_oldObjects;
	VSelection	*m_newObjects;
	int			m_distance;
	int			m_angle;
	float		m_opacity;
};

#endif

