/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002-2005 The Karbon Developers
   Copyright (C) 2006-2007 Jan Hambrecht <jaham@gmx.net>

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

#ifndef __VSTROKEDOCKER_H__
#define __VSTROKEDOCKER_H__

#include <KoLineBorder.h>

class QDockWidget;
class QButtonGroup;
class KoUnitDoubleSpinBox;
class KoDockFactory;
class KoShapeBorderModel;
class KarbonLineStyleSelector;

class VStrokeDockerFactory : public KoDockFactory
{
public:
    VStrokeDockerFactory();

    virtual QString dockId() const;
    virtual Qt::DockWidgetArea defaultDockWidgetArea() const;
    virtual QDockWidget* createDockWidget();
};

class VStrokeDocker : public QDockWidget
{
	Q_OBJECT

public:
	 VStrokeDocker();

public slots:
	virtual void setStroke( const KoShapeBorderModel * );
	virtual void setUnit( KoUnit unit );

private:
	QButtonGroup *m_capGroup;
	QButtonGroup *m_joinGroup;
	KoUnitDoubleSpinBox *m_setLineWidth;
	KoUnitDoubleSpinBox *m_miterLimit;
    KarbonLineStyleSelector * m_lineStyle;
private slots:
	void slotCapChanged( int ID );
	void slotJoinChanged( int ID );
	void updateCanvas();
	void updateDocker();
	void widthChanged();
	void miterLimitChanged();
    void styleChanged();

protected:
	KoLineBorder m_border;
};

#endif

