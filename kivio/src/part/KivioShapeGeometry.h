/* This file is part of the KDE project
 *   Copyright (C)  2006 Peter Simonsson <peter.simonsson@gmail.com>
 * 
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 * 
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 * 
 *   You should have received a copy of the GNU Library General Public License
 *   along with this library; see the file COPYING.LIB.  If not, write to
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *   Boston, MA 02110-1301, USA.
 */

#ifndef KIVIOSHAPEGEOMETRY_H
#define KIVIOSHAPEGEOMETRY_H

#include <QDockWidget>

#include <KoDockFactory.h>

#include "ui_KivioShapeGeometry.h"

class KoSelection;
class KivioDocument;

class KivioShapeGeometry : public QDockWidget, public Ui::KivioShapeGeometry
{
    Q_OBJECT
    public:
        KivioShapeGeometry(KivioDocument* doc);

    public slots:
        void setSelection(KoSelection* selection);
        void update();

        void setUnit(KoUnit::Unit unit);

    protected slots:
        void positionChanged();
        void sizeChanged();
        void protectSizeChanged(bool protect);
        void rotationChanged();

    private:
        KivioDocument* m_doc;
        KoSelection* m_selection;
        QWidget* m_mainWidget;

        bool m_lockedForUpdate;
};

class KivioShapeGeometryFactory : public KoDockFactory
{
    public:
        KivioShapeGeometryFactory(KivioDocument* doc);

        virtual QString dockId() const;
        virtual Qt::DockWidgetArea defaultDockWidgetArea() const;
        virtual QDockWidget* createDockWidget();

    private:
        KivioDocument* m_doc;
};

#endif
