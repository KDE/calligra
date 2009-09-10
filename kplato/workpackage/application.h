/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2000-2005 David Faure <faure@kde.org>
   Copyright (C) 2005 Sven L�ppken <sven@kde.org>
   Copyright (C) 2008 - 2009 Dag Andersen <kplato@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#ifndef KPLATOWORK_APPLICATION_H
#define KPLATOWORK_APPLICATION_H

#include "kplatowork_export.h"

#include <KoMainWindow.h>

#include <KoApplication.h>

#include <ktabwidget.h>
#include <kuniqueapplication.h>

#include <q3ptrlist.h>
#include <QMap>
#include <qtoolbutton.h>
#include <q3valuelist.h>
//Added by qt3to4:
#include <QLabel>
#include <kvbox.h>

//#include "iconsidepane.h"

class KPlatoWork_MainWindow;

namespace std { }
using namespace std;

namespace KPlatoWork {
    class Part;
}
namespace KPlato {
    class Document;
}

class KComponentData;
class KVBox;
class Q3IconView;
class Q3IconViewItem;
class QSplitter;
class KoDocumentEntry;
class KoView;

class KPLATOWORKAPP_EXPORT KPlatoWork_Application : public KUniqueApplication
{
    Q_OBJECT
public:
    KPlatoWork_Application();
    ~KPlatoWork_Application();

    virtual int newInstance();

private:
    KPlatoWork_MainWindow *m_mainwindow;
};


#endif // KPLATOWORK_APPLICATION_H

