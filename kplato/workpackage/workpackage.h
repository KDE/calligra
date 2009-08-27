/* This file is part of the KDE project
  Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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
  Boston, MA 02110-1301, USA.
*/

#ifndef KPLATOWORK_WORKPACKAGE_H
#define KPLATOWORK_WORKPACKAGE_H

#include "kptxmlloaderobject.h"
#include "kptcommand.h"
#include "kpttask.h"

#include <KoDocument.h>

#include <QFileInfo>
#include <QProcess>

#include <kmimetype.h>
#include <kservice.h>

class KoView;
class KoStore;

class KProcess;

class QDomDocument;

namespace KPlato
{
    class Project;
    class Document;
    class XMLLoaderObject;
}

using namespace KPlato;

/// The main namespace for KPlato WorkPackage Handler
namespace KPlatoWork
{

class View;
class Part;
class WorkPackage;
class DocumentChild;

/**
 A work package consists of a Project node and one Task node
 along with scheduling information and assigned resources.
*/
class WorkPackage : public QObject
{
    Q_OBJECT
public:
    WorkPackage( bool fromProjectStore );
    WorkPackage( Project *project, bool fromProjectStore );
    ~WorkPackage();

    DocumentChild *findChild( const Document *doc ) const;
    /// Called when loading a work package. Saves to Project store.
    /// Asks to save/overwrite if already there.
    /// Does nothing if opened from Projects store.
    void saveToProjects( Part *part );

    bool contains( const DocumentChild* child ) const { 
        return m_childdocs.contains( const_cast<DocumentChild*>( child ) );
    }
    QList<DocumentChild*> childDocs() { return m_childdocs; }
    bool addChild( Part *part, const Document *doc );
    void removeChild( DocumentChild *child );
    
    bool contains( const Document *doc ) const;

    QString nodeId() const;

    bool loadXML( const KoXmlElement &element, XMLLoaderObject &status );

    QDomDocument saveXML();
    bool saveNativeFormat( Part *part, const QString &path );
    bool saveDocumentsToStore( KoStore *store );
    bool completeSaving( KoStore *store );

    Node *node() const;
    Task *task() const;
    Project *project() const { return m_project; }

    /// Set the file path to this package
    void setFilePath( const QString name ) { m_filePath = name; }
    /// Return the file path to this package
    QString filePath() const { return m_filePath; }

    /// Construct file path to projects store 
    QString fileName( const Part *part ) const;
    /// Remove work package file
    void removeFile();

    /// Merge data from work package @p wp
    void merge( Part *part, const WorkPackage *wp );

    bool isModified() const;

    int queryClose( Part *part );

    KUrl extractFile( const Document *doc );

    QString id() const;

    bool isValid() const { return m_project && node(); }

    WorkPackageSettings &settings() { return m_settings; }
    void setSettings( const WorkPackageSettings &settings );

signals:
    void modified( bool );

public slots:
    void setModified( bool on ) { m_modified = on; }

protected slots:
    void projectChanged();
    void slotChildModified( bool mod );

protected:
    /// Copy file @p filename from old store @p from, to the new store @p to
    bool copyFile( KoStore *from, KoStore *to, const QString &filename );

    bool saveToStream( QIODevice * dev );

protected:
    Project *m_project;
    QString m_filePath;
    bool m_fromProjectStore;
    QList<DocumentChild*> m_childdocs;

    bool m_modified;

    WorkPackageSettings m_settings;
};

//-----------------------------
class PackageRemoveCmd : public NamedCommand
{
public:
    PackageRemoveCmd( Part *part, WorkPackage *value, const QString& name = QString() );
    ~PackageRemoveCmd();
    void execute();
    void unexecute();

private:
    Part *m_part;
    WorkPackage *m_value;
    bool m_mine;
};

//-----------------------------
class ModifyPackageSettingsCmd : public NamedCommand
{
public:
    ModifyPackageSettingsCmd( WorkPackage *wp, WorkPackageSettings &value, const QString& name = QString() );

    void execute();
    void unexecute();

private:
    WorkPackage *m_wp;
    WorkPackageSettings m_value, m_oldvalue;
};

}  //KPlatoWork namespace

#endif
