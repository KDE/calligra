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

#ifndef KPLATO_PART_H
#define KPLATO_PART_H

#include "kpttask.h"
#include "kptconfig.h"
#include "kptwbsdefinition.h"
#include "kptxmlloaderobject.h"

#include <KoDocument.h>
#include <KoDocumentChild.h>

class KoView;
class K3CommandHistory;
class K3Command;

/// The main namespace.
namespace KPlato
{

class DocumentChild;
class View;
class Project;
class MainProjectDialog;
class ResourceGroup;
class Context;
class GanttView;

class Part : public KoDocument
{
    Q_OBJECT

public:
    explicit Part( QWidget *parentWidget = 0,
          QObject* parent = 0,
          bool singleViewMode = false );
    ~Part();

    virtual void paintContent( QPainter& painter, const QRect& rect);

    /**
     * Edit the settings of the project
     */
    void editProject();

    Project &getProject() { return * m_project; }
    const Project &getProject() const { return * m_project; }

    // The load and save functions. Look in the file kplato.dtd for info
    virtual bool loadXML( QIODevice *, const KoXmlDocument &document );
    virtual QDomDocument saveXML();

    bool saveOasis( KoStore*, KoXmlWriter* ) { return false; }
    bool loadOasis( const KoXmlDocument &doc, KoOasisStyles &, const QDomDocument&, KoStore * );

    void addCommand( K3Command * cmd, bool execute = true );

    void setCommandType( int type );

    Config &config() { return m_config; }

    void generateWBS();
    WBSDefinition &wbsDefinition() { return m_wbsDefinition; }

    const XMLLoaderObject &xmlLoader() const { return m_xmlLoader; }
    
    void activate( QWidget *w = 0 );
    DocumentChild *createChild( KoDocument *doc, const QRect &geometry = QRect() );
    
protected:
    virtual KoView* createViewInstance( QWidget* parent );
    virtual bool loadChildren( KoStore* );

protected slots:
    void slotDocumentRestored();
    void slotCommandExecuted( K3Command * );
    void slotCopyContextFromView();
    void slotViewDestroyed();

private:
    void loadObjects( const KoXmlElement &element );
    
private:
    Project *m_project;
    MainProjectDialog *m_projectDialog;
    QWidget* m_parentWidget;
    View *m_view;

    /**
     * Used for drawing the project when embedded into another koffice app.
     * @see paintContent()
     */
    GanttView* m_embeddedGanttView;
    Context* m_embeddedContext;
    bool m_embeddedContextInitialized;

    K3CommandHistory *m_commandHistory;
    bool m_update, m_calculate;

    Config m_config;
    Context *m_context;

    WBSDefinition m_wbsDefinition;

    XMLLoaderObject m_xmlLoader;
};

class DocumentChild : public KoDocumentChild
{
public:
    DocumentChild( KoDocument* parent );
    DocumentChild( KoDocument* parent, KoDocument* doc, const QRect& geometry );
    //void setEmbedded( bool emb ) { m_embedded = emb; }
    void activate( QWidget *w = 0 );
    virtual KoDocument* hitTest( const QPoint& p, KoView* view, const QMatrix& _matrix = QMatrix() );
    virtual QDomElement save( QDomDocument &doc, bool uppercase = false );
    virtual bool load( const KoXmlElement& element, bool uppercase = false );
    
    void setIcon( const QString &icon ) { m_icon = icon; }
    QString icon() const { return m_icon; }
    
    void setTitle( const QString &title ) { m_title = title; }
    QString title() const { return m_title; }
    
private:
    //bool m_embedded;
    QString m_icon;
    QString m_title;
};

}  //KPlato namespace

#endif
