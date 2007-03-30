/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002,2005 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   Copyright (C) 2002-2003 Rob Buis <buis@kde.org>
   Copyright (C) 2004 Laurent Montel <montel@kde.org>
   Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2005-2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>
   Copyright (C) 2006 Sven Langkamp <sven.langkamp@gmail.com>
   Copyright (C) 2006 Boudewijn Rempt <boud@valdyas.org

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

#ifndef __VCOLORDOCKER_H__
#define __VCOLORDOCKER_H__

#include <QMouseEvent>
#include <KoColor.h>
#include <KoDockFactory.h>

class QDockWidget;
class KarbonView;
class KoUniColorChooser;

class VColorDockerFactory : public KoDockFactory
{
public:
    VColorDockerFactory();

    virtual QString dockId() const;
    virtual Qt::DockWidgetArea defaultDockWidgetArea() const;
    virtual QDockWidget* createDockWidget();
};

class VColorDocker : public QDockWidget
{
    Q_OBJECT

public:
    VColorDocker();
    virtual ~VColorDocker();

    virtual bool isStrokeDocker() { return m_isStrokeDocker; }
    KoColor color() { return m_color; }

public slots:
    virtual void setFillDocker();
    virtual void setStrokeDocker();
    virtual void update();

signals:
    void colorChanged( const KoColor &c );
    //void fgColorChanged( const QColor &c );
    //void bgColorChanged( const QColor &c );

private slots:
    void updateColor( const KoColor &c );

private:

    KoUniColorChooser *m_colorChooser;
    bool m_isStrokeDocker; //Are we setting stroke color ( true ) or fill color ( false )
    KoColor m_color;
    KoColor m_oldColor;
};

#endif

