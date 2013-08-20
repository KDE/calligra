/* This file is part of the KDE libraries
   Copyright (C) 2010 Teo Mrnjavac <teo@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KABOUT_APPLICATION_PERSON_LIST_VIEW_H
#define KABOUT_APPLICATION_PERSON_LIST_VIEW_H

#include <QListView>

namespace KDEPrivate
{

class KAboutApplicationPersonListView : public QListView
{
public:
    KAboutApplicationPersonListView( QWidget *parent = 0 );

protected:
    void wheelEvent( QWheelEvent *e );

};

} //namespace KDEPrivate

#endif // KABOUT_APPLICATION_PERSON_LIST_VIEW_H
