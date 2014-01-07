/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2014  Adam Pigg <email>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef QUICKFORMDATAVIEW_H
#define QUICKFORMDATAVIEW_H

#include <KexiView.h>

class QGraphicsObject;
class QDeclarativeEngine;
class QDeclarativeEngine;
class QGraphicsView;
class QGraphicsScene;

class QuickFormDataView : public KexiView
{
    Q_OBJECT
public:
    explicit QuickFormDataView(QWidget* parent);
    virtual ~QuickFormDataView();
    
    void setDefinition(const QString &def);

private:
    //QDeclarativeView *m_view;
    QGraphicsView *m_view;
    QGraphicsScene *m_scene;
    QDeclarativeEngine *m_engine;
    QGraphicsObject *m_object;
};

#endif // QUICKFORMDATAVIEW_H
