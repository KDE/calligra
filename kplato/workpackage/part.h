/* This file is part of the KDE project
  Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
  Copyright (C) 2004 - 2009 Dag Andersen <danders@get2net.dk>
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

#include <QFileInfo>
#include <QProcess>

#include <kmimetype.h>
#include <kservice.h>

class KoView;
class KoStore;

class KProcess;

namespace KPlato
{
    class Project;
    class Document;
}

using namespace KPlato;

/// The main namespace for KPlato WorkPackage Handler
namespace KPlatoWork
{

class View;
class Part;
class WorkPackage;

/**
 * DocumentChild stores info about documents opened for editing.
 * Editors can be KParts, KOffice or Other.
 */
class DocumentChild : public QObject
{
    Q_OBJECT
public:
    // The type of document this child handles
    enum DocType { Type_Unknown = 0, Type_KOffice, Type_KParts, Type_Other };

    explicit DocumentChild( WorkPackage *parent );
//    DocumentChild( KParts::ReadWritePart *editor, const KUrl &url, const Document *doc, Part *parent);
    
    ~DocumentChild();
    
    WorkPackage *parentPackage() const;
    const Document *doc() const { return m_doc; }
    /// Set document, return true if ok, false if failure
    bool setDoc( const Document *doc );
    /// Open document for editing, return true if ok, false if failure
    bool editDoc();
    bool isOpen() const { return m_process != 0; }
    bool isModified() const;
    bool isFileModified() const;
    
    QString fileName() const { return m_fileinfo.fileName(); }
    QString filePath() const { return m_fileinfo.canonicalFilePath(); }
    void setFileInfo( const KUrl &url );
    const QFileInfo &fileInfo() const { return m_fileinfo; }

    KUrl url() const { return KUrl( filePath() ); }
    KParts::ReadWritePart *editor() const { return m_editor; }
    bool startProcess( KService::Ptr service, const KUrl &url = KUrl() );
    int type() const { return m_type; }
    void setType( int type ) { m_type = type; }
    
    bool saveToStore( KoStore *store );
    
signals:
    void modified( bool );
    void fileModified( bool );
    
public slots:
    void setModified( bool mod );
    
protected slots:
    void slotEditFinished( int,  QProcess::ExitStatus );
    void slotEditError( QProcess::ProcessError status );
    
    void slotDirty( const QString &file );

    void slotUpdateModified();
    
protected:
    const Document *m_doc;
    int m_type;
    bool m_copy;
    KProcess *m_process; // Used if m_type == Type_Other;
    KParts::ReadWritePart *m_editor; // 0 if m_type == Type_Other
    QFileInfo m_fileinfo;
    bool m_editormodified;
    bool m_filemodified;
};

/**
 This part handles work packages.
 A work package file consists of a Project node and one Task node
 along with scheduling information and assigned resources.
*/

class Part : public KoDocument
{
    Q_OBJECT

public:
    explicit Part( QWidget *parentWidget = 0, QObject* parent = 0, bool singleViewMode = false );
    ~Part();

    int docType( const Document *doc ) const;
    
    void setProject( Project *project );
    Project &getProject() { return *m_project; }
    const Project &getProject() const { return *m_project; }

    bool setWorkPackage( Project *project );

    virtual void paintContent( QPainter& painter, const QRect& rect);
    virtual void paintChildren( QPainter &painter, const QRect &/*rect*/, KoView *view) {}

    bool loadWorkPackages();
    // The load and save functions.
    virtual bool loadXML( const KoXmlDocument &document, KoStore *store );
    virtual QDomDocument saveXML();
    
    bool saveAs( const KUrl &url );
    /// Check if we have documents open for editing before saving
    virtual bool completeSaving( KoStore* store );

    bool saveOdf( SavingContext &documentContext ) { return false; }
    bool loadOdf( KoOdfReadStore & odfStore );

    /// Extract document file from the store to disk
    KUrl extractFile( const Document *doc );
    
    //Config &config() { return m_config; }
    
    /// Open KOffice document for editing
//     DocumentChild *openKOfficeDocument( KMimeType::Ptr mimetype, const Document *doc );
    /// Open KParts document for editing
//     DocumentChild *openKPartsDocument( KService::Ptr service, const Document *doc );
    /// Open document for editing, return true if ok, false if failure
    bool editWorkpackageDocument( const Document *doc );
    /// Open document for editing, return true if ok, false if failure
    bool editOtherDocument( const Document *doc );
    /// Remove the child document
//    void removeChildDocument( DocumentChild *child );
    /// Find the child that handles document @p doc
    DocumentChild *findChild( const Document *doc ) const;
    /// Add @p child document to work package @p wp
//    void addChild( WorkPackage *wp, DocumentChild *child );

    /// Number of workpackages
    int workPackageCount() const { return m_packageMap.count(); }
    /// Work package at index
    WorkPackage *workPackage( int index ) const { return m_packageMap.values().value( index ); }
    /// Work package containing node
    WorkPackage *workPackage( Node *node ) const { 
        return m_packageMap.value( node->projectNode()->id() + node->id() );
    }
    int indexOf( WorkPackage *package ) const { return m_packageMap.values().indexOf( package ); }

    /// Find the work package that handles document @p doc
    WorkPackage *findWorkPackage( const Document *doc ) const;
    /// Find the work package that handles document child @p child
    WorkPackage *findWorkPackage( const DocumentChild *child ) const;
    /// Find the work package that handles @p node
    WorkPackage *findWorkPackage( const Node *node ) const;

    /// Save all work packages
    bool saveWorkPackages( bool silent );

    Node *node() const;
    
    bool queryClose();

    using KoDocument::setModified;

public slots:
    /**
     * Called by the undo stack when the document is saved or all changes has been undone
     * @param clean if the document's undo stack is clean or not
     */
    virtual void setDocumentClean(bool clean);

    virtual void setModified( bool mod );

signals:
    void changed();
    void workPackageAdded( WorkPackage *package, int index );

protected:
    virtual KoView* createViewInstance( QWidget* parent );
    bool completeLoading( KoStore *store );
    
private:
    Project *m_project;
    XMLLoaderObject m_xmlLoader;
    //Config m_config;
    
    QMap<QString, WorkPackage*> m_packageMap;
    WorkPackage *m_currentWorkPackage;

    bool m_modified;
    bool m_loadingFromProjectStore;
};


}  //KPlatoWork namespace

#endif
