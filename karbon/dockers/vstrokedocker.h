/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   Copyright (C) 2002 Rob Buis <buis@kde.org>
   Copyright (C) 2004 Laurent Montel <montel@kde.org>
   Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2005 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>
   Copyright (C) 2005-2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2006 Casper Boemann <cbr@boemann.dk>

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

