/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002-2003 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   Copyright (C) 2004 Laurent Montel <montel@kde.org>
   Copyright (C) 2005 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2005 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>
   Copyright (C) 2005-2006 Jan Hambrecht <jaham@gmx.net>
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

#ifndef __VTRANSFORMDOCKER_H__
#define __VTRANSFORMDOCKER_H__

class KoUnitDoubleSpinBox;
class KoDockFactory;

class VTransformDockerFactory : public KoDockFactory
{
public:
    VTransformDockerFactory();

    virtual QString dockId() const;
    virtual Qt::DockWidgetArea defaultDockWidgetArea() const;
    virtual QDockWidget* createDockWidget();
};

class VTransformDocker : public QDockWidget
{
	Q_OBJECT

public:
	VTransformDocker();

public slots:
	void update();
	virtual void setUnit( KoUnit unit );

private slots:
	void translate();
	void scale();
	void enableSignals( bool enable );
	void shearX();
	void shearY();
	void rotate();

private:
	KoUnitDoubleSpinBox *m_x;
	KoUnitDoubleSpinBox *m_y;
	KoUnitDoubleSpinBox *m_width;
	KoUnitDoubleSpinBox *m_height;
	KDoubleSpinBox *m_rotate;
	KDoubleSpinBox *m_shearX;
	KDoubleSpinBox *m_shearY;
};

#endif

