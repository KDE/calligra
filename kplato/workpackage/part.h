/* This file is part of the KDE project
  Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
  Copyright (C) 2004 - 2007 Dag Andersen <danders@get2net.dk>
  Copyright (C) 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
  Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>

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

#include "kptxmlloaderobject.h"

#include <KoDocument.h>
#include <KoDocumentChild.h>

class KoView;

namespace KPlato
{
    class Project;
}

using namespace KPlato;

/// The main namespace for KPlato WorkPackage Handler
namespace KPlatoWork
{

class DocumentChild;
class View;

/**
 This part handles a work package.
 A work package file consists of a Project node and one Task node
 along with scheduling information and assigned resources.
*/

class Part : public KoDocument
{
    Q_OBJECT

public:
    explicit Part( QWidget *parentWidget = 0, QObject* parent = 0, bool singleViewMode = false );
    ~Part();

    void setProject( Project *project );
    Project &getProject() { return *m_project; }
    const Project &getProject() const { return *m_project; }

    virtual void paintContent( QPainter& painter, const QRect& rect);

    // The load and save functions.
    virtual bool loadXML( QIODevice *, const KoXmlDocument &document );
    virtual QDomDocument saveXML();

    bool saveOasis( KoStore*, KoXmlWriter* ) { return false; }
    bool loadOdf( KoOdfReadStore & odfStore );

    //Config &config() { return m_config; }
    
signals:
    void changed();

protected:
    virtual KoView* createViewInstance( QWidget* parent );

protected slots:
    void slotViewDestroyed();
    
private:
    Project *m_project;
    XMLLoaderObject m_xmlLoader;
    //Config m_config;
};


}  //KPlatoWork namespace

#endif
