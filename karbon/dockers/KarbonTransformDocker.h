/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002-2003 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   Copyright (C) 2004 Laurent Montel <montel@kde.org>
   Copyright (C) 2005 Tim Beaulen <tbscope@gmail.com>
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

#ifndef KARBONTRANSFORMDOCKER_H
#define KARBONTRANSFORMDOCKER_H

#include <KoUnit.h>
#include <KoDockFactory.h>
#include <QtGui/QDockWidget>

class QDoubleSpinBox;
class KoUnitDoubleSpinBox;
class KoDockFactory;

class KarbonTransformDockerFactory : public KoDockFactory
{
public:
    KarbonTransformDockerFactory();

    virtual QString id() const;
    virtual QDockWidget* createDockWidget();
    virtual KoDockFactory::DockPosition defaultDockPosition() const;
};

class KarbonTransformDocker : public QDockWidget
{
    Q_OBJECT

public:
    KarbonTransformDocker();
    ~KarbonTransformDocker();

public slots:
    void update();
    void setUnit( KoUnit unit );

private slots:
    void translate();
    void scale();
    void enableSignals( bool enable );
    void shear();
    void rotate();
    void dockLocationChanged( Qt::DockWidgetArea area );

private:
    QRectF selectionRect();
    void layoutVertical();
    void layoutHorizontal();
    void clearLayout();
    void enableWidgets( bool enable );
    class Private;
    Private * const d;
};

#endif // KARBONTRANSFORMDOCKER_H

