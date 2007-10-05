/* This file is part of the KDE project
  Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
  Copyright (C) 2004 - 2007 Dag Andersen <danders@get2net.dk>
  Copyright (C) 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>

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

#ifndef KPLATOWORK_PART_H
#define KPLATOWORK_PART_H

#include <KoDocument.h>
#include <KoDocumentChild.h>

class KoView;
class K3CommandHistory;
class K3Command;

/// The main namespace for KPlato WorkPackage Handler
namespace KPlatoWork
{

class DocumentChild;
class View;

class Part : public KoDocument
{
    Q_OBJECT

public:
    explicit Part( QWidget *parentWidget = 0,
          QObject* parent = 0,
          bool singleViewMode = false );
    ~Part();

    virtual void paintContent( QPainter& painter, const QRect& rect);

    // The load and save functions.
    virtual bool loadXML( QIODevice *, const KoXmlDocument &document );
    virtual QDomDocument saveXML();

    bool saveOasis( KoStore*, KoXmlWriter* ) { return false; }
    bool loadOasis( const KoXmlDocument &doc, KoOasisStyles &, const KoXmlDocument&, KoStore * );

    void addCommand( K3Command * cmd, bool execute = true );

    //Config &config() { return m_config; }
    
protected:
    virtual KoView* createViewInstance( QWidget* parent );

protected slots:
    void slotDocumentRestored();
    void slotCommandExecuted( K3Command * );
    void slotViewDestroyed();
    
private:
    K3CommandHistory *m_commandHistory;

    //Config m_config;
};


}  //KPlatoWork namespace

#endif
