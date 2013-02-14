/*
    This file is part of the Krita program, made within the KDE community.

    Copyright 2006-2008 Friedrich W. H. Kossebau <kossebau@kde.org>
    Copyright 2013 Boudewijn Rempt <boud@kde.org>
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy 
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KRITAMAINWINDOW_H
#define KRITAMAINWINDOW_H

// Kasten gui
#include <shellwindow.h>

template<class T> class QList;
class QMimeData;


namespace Kasten2
{

class KritaProgram;
class DocumentManager;


class KritaMainWindow : public ShellWindow
{
   Q_OBJECT

  public:
    explicit KritaMainWindow( KritaProgram* program );
    virtual ~KritaMainWindow();

  protected: // KMainWindow API
    virtual bool queryClose();

    virtual void saveProperties( KConfigGroup& configGroup );
    virtual void readProperties( const KConfigGroup& configGroup );

  protected:
    void setupControllers();

  protected Q_SLOTS:
    void onDataOffered( const QMimeData* mimeData, bool& accept );
    void onDataDropped( const QMimeData* mimeData );
    void onCloseRequest( const QList<Kasten2::AbstractView*>& views );

  protected:
    KritaProgram* mProgram;
};

}

#endif
