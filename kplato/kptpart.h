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

#ifndef KPLATO_PART_H
#define KPLATO_PART_H

#include "kplato_export.h"

#include "kpttask.h"
#include "kptconfig.h"
#include "kptwbsdefinition.h"
#include "kptxmlloaderobject.h"
#include "about/aboutpage.h"

#include "KoDocument.h"

#include <qfileinfo.h>

class KoView;

/// The main namespace.
namespace KPlato
{

class DocumentChild;
class Project;
class Context;
class GanttView;
class SchedulerPlugin;

class Package;

class KPLATO_EXPORT Part : public KoDocument
{
    Q_OBJECT

public:
    explicit Part( QWidget *parentWidget = 0,
          QObject* parent = 0,
          bool singleViewMode = false );
    ~Part();

    void configChanged();
    
    virtual void paintContent( QPainter& painter, const QRect& rect);

    void setProject( Project *project );
    Project &getProject() { return *m_project; }
    const Project &getProject() const { return * m_project; }

    /**
     * Return the set of SupportedSpecialFormats that the kplato wants to
     * offer in the "Save" file dialog.
     * Note: SaveEncrypted is not supported.
     */
    virtual int supportedSpecialFormats() const { return SaveAsDirectoryStore; }

    // The load and save functions. Look in the file kplato.dtd for info
    virtual bool loadXML( const KoXmlDocument &document, KoStore *store );
    virtual QDomDocument saveXML();
    /// Save a workpackage file containing @p node with schedule identity @p id, owned by @p resource
    QDomDocument saveWorkPackageXML( const Node *node, long id, Resource *resource = 0 );
    
    bool saveOdf( SavingContext &documentContext ) { return false; }
    bool loadOdf( KoOdfReadStore & odfStore );

    Config &config() { return m_config; }
    Context *context() const { return m_context; }
    
    WBSDefinition &wbsDefinition() { return m_project->wbsDefinition(); }

    const XMLLoaderObject &xmlLoader() const { return m_xmlLoader; }
    
    void activate( QWidget *w = 0 );
    DocumentChild *createChild( KoDocument *doc, const QRect &geometry = QRect() );
    
    bool saveWorkPackageToStream( QIODevice * dev, const Node *node, long id, Resource *resource = 0 );
    bool saveWorkPackageFormat( const QString &file, const Node *node, long id, Resource *resource = 0 );
    bool saveWorkPackageUrl( const KUrl & _url, const Node *node, long id, Resource *resource = 0  );
    void mergeWorkPackages();
    void mergeWorkPackage( const Package *package );

    /// Load the workpackage from @p url into @p project. Return true if successful, else false.
    bool loadWorkPackage( Project &project, const KUrl &url );
    Project *loadWorkPackageXML( Project &project, QIODevice *, const KoXmlDocument &document, const KUrl &url );
    QMap<Package*, KUrl> workPackages() const { return m_workpackages; }

    void insertFile( const QString &filename, Node *parent, Node *after = 0 );
    bool insertProject( Project &project, Node *parent, Node *after );

    KPlatoAboutPage &aboutPage() { return m_aboutPage; }

signals:
    void changed();
    void workPackageLoaded();

protected:
    virtual KoView* createViewInstance( QWidget* parent );
     
    /// Load kplato specific files
    virtual bool completeLoading( KoStore* store );
    /// Save kplato specific files
    virtual bool completeSaving( KoStore* store );

    void mergeWorkPackage( Task *to, const Task *from, const Package *package );

protected slots:
    void slotViewDestroyed();
    virtual void openTemplate( const KUrl& url );
    void addSchedulerPlugin( const QString&, SchedulerPlugin *plugin );

    void checkForWorkPackages();
    void checkForWorkPackage();

private:
    bool loadAndParse(KoStore* store, const QString& filename, KoXmlDocument& doc);

    void loadSchedulerPlugins();    

private:
    Project *m_project;
    QWidget* m_parentWidget;

    Config m_config;
    Context *m_context;

    XMLLoaderObject m_xmlLoader;
    bool m_loadingTemplate;

    QMap<QString, SchedulerPlugin*> m_schedulerPlugins;
    QMap<Package*, KUrl> m_workpackages;
    QFileInfoList m_infoList;
    QMap<QString, Project*> m_mergedPackages;

    KPlatoAboutPage m_aboutPage;
};


}  //KPlato namespace

#endif
