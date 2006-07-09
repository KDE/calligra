/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
   Copyright (C) 2004, 2005 Dag Andersen <danders@get2net.dk>
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
#include <KoTextZoomHandler.h>

class KoView;
class KoCommandHistory;
class KCommand;

namespace KPlato
{

class View;
class Project;
class ProjectDialog;
class ResourceGroup;
class Context;
class GanttView;

class Part : public KoDocument, public KoTextZoomHandler {
    Q_OBJECT

public:
    Part(QWidget *parentWidget = 0, const char *widgetName = 0,
	    QObject* parent = 0, const char* name = 0,
	    bool singleViewMode = false);
    ~Part();

    virtual void paintContent(QPainter& painter, const QRect& rect,
			      bool transparent = FALSE,
			      double zoomX = 1.0, double zoomY = 1.0);

    virtual bool initDoc(InitDocFlags flags, QWidget* parentWidget=0);

    /**
     * Edit the settings of the project
     */
    void editProject();

    Project &getProject() { return *m_project; }
    const Project &getProject() const { return *m_project; }

    // The load and save functions. Look in the file kplato.dtd for info
    virtual bool loadXML(QIODevice *, const QDomDocument &document);
    virtual QDomDocument saveXML();

    bool saveOasis(KoStore*, KoXmlWriter*) { return false; }
    bool loadOasis(const QDomDocument &, KoOasisStyles &, const QDomDocument&, KoStore *) { return false; }

    void addCommand(KCommand * cmd, bool execute=true);
    
    void setCommandType(int type);
    
    Config &config() { return m_config; }
    
    void generateWBS();
    WBSDefinition &wbsDefinition() { return m_wbsDefinition; }
    
    const XMLLoaderObject &xmlLoader() const { return m_xmlLoader; }
protected:
    virtual KoView* createViewInstance(QWidget* parent, const char* name);

protected slots:
    void slotDocumentRestored();
    void slotCommandExecuted();
    void slotCopyContextFromView();
    void slotViewDestroyed();

private:
    Project *m_project;
    ProjectDialog *m_projectDialog;
    QWidget* m_parentWidget;
    View *m_view;
    
    /**
     * Used for drawing the project when embedded into another koffice app.
     * @see paintContent()
     */
    GanttView* m_embeddedGanttView;
    Context* m_embeddedContext;
    bool m_embeddedContextInitialized;
    
    KoCommandHistory *m_commandHistory;
    bool m_update, m_calculate, m_baseline;
    
    Config m_config;
    Context *m_context;
    
    WBSDefinition m_wbsDefinition;
    
    XMLLoaderObject m_xmlLoader;
};

}  //KPlato namespace

#endif
