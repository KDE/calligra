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
#ifndef KPLATOWORK_MAINWINDOW_H
#define KPLATOWORK_MAINWINDOW_H

#include "kplatowork_export.h"

#include <kxmlguiwindow.h>

#include <KoApplication.h>

#include <ktabwidget.h>

#include <QMap>
#include <qtoolbutton.h>
#include <QLabel>

#include <kmimetype.h>
#include <kvbox.h>
#include <kservice.h>
#include <KoDocumentEntry.h>
#include <kparts/mainwindow.h>

namespace std { }
using namespace std;

namespace KParts {
    class PartManager;
}
namespace KPlatoWork {
    class DocumentChild;
    class Part;
}
namespace KPlato {
    class Document;
}

class KComponentData;
class KVBox;
class QSplitter;
class KoDocumentEntry;
class KoView;

class KPlatoWork_MainGUIClient;

/////// class KPlatoWork_MainWindow ////////

class KPLATOWORK_EXPORT KPlatoWork_MainWindow : public KParts::MainWindow
{
  Q_OBJECT

public:
    explicit KPlatoWork_MainWindow();
    virtual ~KPlatoWork_MainWindow();

    KPlatoWork::Part *rootDocument() const { return m_part; }
    bool openDocument(const KUrl & url);

    virtual QString configFile() const;

     void editDocument( KPlatoWork::Part *part, const KPlato::Document *doc );

//     bool isEditing() const { return m_editing; }
//     bool isModified() const;

signals:
    void undo();
    void redo();

public slots:
    virtual void slotFileClose();

protected slots:
    virtual bool queryClose();

    virtual void slotFileOpen();
    /**
     *  Saves all workpackages
     */
    virtual void slotFileSave();

protected:
     virtual bool saveDocument( bool saveas = false, bool silent = false );

private:
    KPlatoWork::Part *m_part;
};


#endif // KPLATOWORK_MAINWINDOW_H

