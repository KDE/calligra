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
#include <QFileInfo>
#include <QProcess>
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


/**
 * DocumentChild can handle documents opened for editing in a separate process
 */
class DocumentChild : public KoDocumentChild
{
    Q_OBJECT
public:
    explicit DocumentChild( Part *parent );
    
    Part *part() const;
    const Document *doc() const { return m_doc; }
    /// Set document, return true if ok, false if failure
    bool setDoc( const Document *doc );
    /// Open document for editing, return true if ok, false if failure
    bool editDoc();
    bool isOpen() const { return m_process != 0; }
    bool isModified() const;
    QString fileName() const { return m_fileinfo.fileName(); }
    QString filePath() const { return m_fileinfo.canonicalFilePath(); }
    void setFileInfo( const KUrl &url );
    const QFileInfo &fileInfo() const { return m_fileinfo; }

protected slots:
    void slotEditFinished( int,  QProcess::ExitStatus );
    void slotEditError( QProcess::ProcessError status );
    
protected:
    const Document *m_doc;
    bool m_copy;
    KProcess *m_process;
    QFileInfo m_fileinfo;
};


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
    virtual void paintChildren( QPainter &painter, const QRect &/*rect*/, KoView *view) {}

    // The load and save functions.
    virtual bool loadXML( QIODevice *, const KoXmlDocument &document );
    virtual QDomDocument saveXML();

    /// Check if we have documents open for editing before saving
    virtual bool completeSaving( KoStore* store );

    bool saveOdf( SavingContext &documentContext ) { return false; }
    bool loadOdf( KoOdfReadStore & odfStore );

    /// Extract document file from the store to disk
    KUrl extractFile( const Document *doc );
    /// Copy file @p filename from old store @p from to the new store @p to
    bool copyFile( KoStore *from, KoStore *to, const QString &filename );
    
    //Config &config() { return m_config; }
    
    /// Open document for editing, return true if ok, false if failure
    bool editDocument( const Document *doc );
    
    QList<DocumentChild*> childDocs() { return m_childdocs; }
    void addChild( DocumentChild *child );
    
signals:
    void changed();

protected:
    virtual KoView* createViewInstance( QWidget* parent );
    bool completeLoading( KoStore *store );
    
    Node *node() const;
    
protected slots:
    void slotViewDestroyed();
    void slotDirty( const QString & );
    
private:
    Project *m_project;
    XMLLoaderObject m_xmlLoader;
    //Config m_config;
    QList<DocumentChild*> m_childdocs;
};


}  //KPlatoWork namespace

#endif
