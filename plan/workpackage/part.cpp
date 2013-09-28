/* This file is part of the KDE project
 Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
 Copyright (C) 2004 - 2009 Dag Andersen <danders@get2net.dk>
 Copyright (C) 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
 Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>
  Copyright (C) 2007 - 2009, 2012 Dag Andersen <danders@get2net.dk>

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

#include "part.h"
#include "view.h"
#include "factory.h"
#include "mainwindow.h"
#include "workpackage.h"

#include "KPlatoXmlLoader.h" //NB!

#include "kptglobal.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptdocuments.h"
#include "kptcommand.h"

#include <KoZoomHandler.h>
#include <KoXmlReader.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoOdfReadStore.h>
#include <KoDocumentInfo.h>

#include <QPainter>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <kundo2qstack.h>
#include <QPointer>

#include <kdebug.h>
#include <kcomponentdata.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kparts/partmanager.h>
#include <kurl.h>
#include <kopenwithdialog.h>
#include <kmimetype.h>
#include <kmimetypetrader.h>
//#include <kserviceoffer.h>
#include <krun.h>
#include <kprocess.h>
#include <kdirwatch.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kxmlguifactory.h>

#include <KoGlobal.h>
#include <KoMainWindow.h>

#include "debugarea.h"

using namespace KPlato;

namespace KPlatoWork
{

//-------------------------------
DocumentChild::DocumentChild( WorkPackage *parent)
    : QObject( parent ),
    m_doc( 0 ),
    m_type( Type_Unknown ),
    m_copy( false ),
    m_process( 0 ),
    m_editor( 0 ),
    m_editormodified( false ),
    m_filemodified( false )
{
}

// DocumentChild::DocumentChild( KParts::ReadWritePart *editor, const KUrl &url, const Document *doc, Part *parent)
//     : KoDocumentChild( parent ),
//     m_doc( doc ),
//     m_type( Type_Unknown ),
//     m_copy( true ),
//     m_process( 0 ),
//     m_editor( editor ),
//     m_editormodified( false ),
//     m_filemodified( false )
// {
//     setFileInfo( url );
//     if ( dynamic_cast<KoDocument*>( editor ) ) {
//         kDebug(planworkDbg())<<"Creating Calligra doc";
//         m_type = Type_Calligra;
//         connect( static_cast<KoDocument*>( editor ), SIGNAL(modified(bool)), this, SLOT(setModified(bool)) );
//     } else {
//         kDebug(planworkDbg())<<"Creating KParts doc";
//         m_type = Type_KParts;
//         slotUpdateModified();
//     }
// }

DocumentChild::~DocumentChild()
{
    kDebug(planworkDbg());
    disconnect( KDirWatch::self(), SIGNAL(dirty(QString)), this, SLOT(slotDirty(QString)) );
    KDirWatch::self()->removeFile( filePath() );

    if ( m_type == Type_Calligra || m_type == Type_KParts ) {
        delete m_editor;
    }
}

WorkPackage *DocumentChild::parentPackage() const
{
    return static_cast<WorkPackage*>( parent() );
}

void DocumentChild::setFileInfo( const KUrl &url )
{
    m_fileinfo.setFile( url.path() );
    //kDebug(planworkDbg())<<url;
    bool res = connect( KDirWatch::self(), SIGNAL(dirty(QString)), this, SLOT(slotDirty(QString)) );
    //kDebug(planworkDbg())<<res<<filePath();
#ifndef NDEBUG
    Q_ASSERT( res );
#else
    Q_UNUSED( res );
#endif
    KDirWatch::self()->addFile( filePath() );
}

void DocumentChild::setModified( bool mod )
{
    kDebug(planworkDbg())<<mod<<filePath();
    if ( m_editormodified != mod ) {
        m_editormodified = mod;
        emit modified( mod );
    }
}

void DocumentChild::slotDirty( const QString &file )
{
    //kDebug(planworkDbg())<<filePath()<<file<<m_filemodified;
    if ( file == filePath() && ! m_filemodified ) {
        kDebug(planworkDbg())<<file<<"is modified";
        m_filemodified = true;
        emit fileModified( true );
    }
}

void DocumentChild::slotUpdateModified()
{
    if ( m_type == Type_KParts && m_editor && ( m_editor->isModified() != m_editormodified ) ) {
        setModified( m_editor->isModified() );
    }
    QTimer::singleShot( 500, this, SLOT(slotUpdateModified()) );
}

bool DocumentChild::setDoc( const Document *doc )
{
    Q_ASSERT ( m_doc == 0 );
    if ( isOpen() ) {
        KMessageBox::error( 0, i18n( "Document is already open:<br>%1", doc->url().pathOrUrl() ) );
        return false;
    }
    m_doc = doc;
    KUrl url;
    if ( parentPackage()->newDocuments().contains( doc ) ) {
        url = parentPackage()->newDocuments().value( doc );
        Q_ASSERT( url.isValid() );
        parentPackage()->removeNewDocument( doc );
    } else if ( doc->sendAs() == Document::SendAs_Copy ) {
        url = parentPackage()->extractFile( doc );
        if ( url.url().isEmpty() ) {
            KMessageBox::error( 0, i18n( "Could not extract document from storage:<br>%1", doc->url().pathOrUrl() ) );
            return false;
        }
        m_copy = true;
    } else {
        url = doc->url();
    }
    if ( ! url.isValid() ) {
        KMessageBox::error( 0, i18n( "Invalid URL:<br>%1", url.pathOrUrl() ) );
        return false;
    }
    setFileInfo( url );
    return true;
}

bool DocumentChild::openDoc( const Document *doc, KoStore *store )
{
    Q_ASSERT ( m_doc == 0 );
    if ( isOpen() ) {
        KMessageBox::error( 0, i18n( "Document is already open:<br>%1", doc->url().pathOrUrl() ) );
        return false;
    }
    m_doc = doc;
    KUrl url;
    if ( doc->sendAs() == Document::SendAs_Copy ) {
        url = parentPackage()->extractFile( doc, store );
        if ( url.url().isEmpty() ) {
            KMessageBox::error( 0, i18n( "Could not extract document from storage:<br>%1", doc->url().pathOrUrl() ) );
            return false;
        }
        m_copy = true;
    } else {
        url = doc->url();
    }
    if ( ! url.isValid() ) {
        KMessageBox::error( 0, i18n( "Invalid URL:<br>%1", url.pathOrUrl() ) );
        return false;
    }
    setFileInfo( url );
    return true;
}

bool DocumentChild::editDoc()
{
    Q_ASSERT( m_doc != 0 );
    kDebug(planworkDbg())<<"file:"<<filePath();
    if ( isOpen() ) {
        KMessageBox::error( 0, i18n( "Document is already open:<br> %1", m_doc->url().pathOrUrl() ) );
        return false;
    }
    if ( ! m_fileinfo.exists() ) {
        KMessageBox::error( 0, i18n( "File does not exist:<br>%1", fileName() )  );
        return false;
    }
    KUrl filename( filePath() );
    KMimeType::Ptr mimetype = KMimeType::findByUrl( filename, 0, true );
    KService::Ptr service = KMimeTypeTrader::self()->preferredService( mimetype->name() );
    bool editing = startProcess( service, filename );
    if ( editing ) {
        m_type = Type_Other; // FIXME: try to be more specific
    }
    return editing;
}

bool DocumentChild::startProcess( KService::Ptr service, const KUrl &url )
{
    QStringList args;
    KUrl::List files;
    if ( url.isValid() ) {
        files << url;
    }
    if ( service ) {
        args = KRun::processDesktopExec( *service, files );
    } else {
        KUrl::List list;
        QPointer<KOpenWithDialog> dlg = new KOpenWithDialog( list, i18n("Edit with:"), QString(), 0 );
        if ( dlg->exec() == QDialog::Accepted && dlg ){
            args << dlg->text();
        }
        if ( args.isEmpty() ) {
            kDebug(planworkDbg())<<"No executable selected";
            return false;
        }
        args << url.url();
        delete dlg;
    }
    kDebug(planworkDbg())<<args;
    m_process = new KProcess();
    m_process->setProgram( args );
    connect( m_process, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(slotEditFinished(int,QProcess::ExitStatus)) );
    connect( m_process, SIGNAL(error(QProcess::ProcessError)), SLOT(slotEditError(QProcess::ProcessError)) );
    m_process->start();
    //kDebug(planworkDbg())<<m_process->pid()<<m_process->program();
    return true;
}

bool DocumentChild::isModified() const
{
    return m_editormodified;
}

bool DocumentChild::isFileModified() const
{
    return m_filemodified;
}

void DocumentChild::slotEditFinished( int /*par*/,  QProcess::ExitStatus )
{
    //kDebug(planworkDbg())<<par<<filePath();
    delete m_process;
    m_process = 0;
}

void DocumentChild::slotEditError( QProcess::ProcessError status )
{
    kDebug(planworkDbg())<<status;
    if ( status == QProcess::FailedToStart || status == QProcess::Crashed ) {
        m_process->deleteLater();
        m_process = 0;
    } else kDebug(planworkDbg())<<"Error="<<status<<" what to do?";
}

bool DocumentChild::saveToStore( KoStore *store )
{
    kDebug(planworkDbg())<<filePath();
    KDirWatch::self()->removeFile( filePath() );
    bool ok = false;
    bool wasmod = m_filemodified;
    if ( m_type == Type_Calligra || m_type == Type_KParts ) {
        if ( m_editor->isModified() ) {
            ok = m_editor->save(); // hmmmm
        } else {
            ok = true;
        }
    } else if ( m_type == Type_Other ) {
        if ( isOpen() ) {
            kWarning()<<"External editor open";
        }
        ok = true;
    } else {
        kError()<<"Unknown document type";
    }
    if ( ok ) {
        kDebug(planworkDbg())<<"Add to store:"<<fileName();
        store->addLocalFile( filePath(), fileName() );
        m_filemodified = false;
        if ( wasmod != m_filemodified ) {
            emit fileModified( m_filemodified );
        }
    }
    KDirWatch::self()->addFile( filePath() );
    return ok;
}


//------------------------------------
Part::Part( QWidget *parentWidget, QObject *parent, const QVariantList & /*args*/ )
    : KParts::ReadWritePart( parent ),
    m_xmlLoader(),
    m_modified( false ),
    m_loadingFromProjectStore( false ),
    m_undostack( new KUndo2QStack( this ) )
{
    kDebug(planworkDbg());
    setComponentData( Factory::global() );
    // Add library translation files
    KLocale *locale = KGlobal::locale();
    if ( locale ) {
        locale->insertCatalog( "planlibs" );
#ifdef PLAN_KDEPIMLIBS_FOUND
        locale->insertCatalog( "kabc" );
#endif
    }
    if ( isReadWrite() ) {
        setXMLFile( "planwork.rc" );
    } else {
        setXMLFile( "planwork_readonly.rc" );
    }

    View *v = new View( this, parentWidget, actionCollection() );
    setWidget( v );
    connect( v, SIGNAL(viewDocument(Document*)), SLOT(viewWorkpackageDocument(Document*)) );

    loadWorkPackages();

    connect( m_undostack, SIGNAL(cleanChanged(bool)), SLOT(setDocumentClean(bool)) );

}

Part::~Part()
{
    kDebug(planworkDbg());
//    m_config.save();
    qDeleteAll( m_packageMap );
}

void Part::addCommand( KUndo2Command *cmd )
{
    if ( cmd ) {
        m_undostack->push( cmd );
    }
}

bool Part::setWorkPackage( WorkPackage *wp, KoStore *store )
{
    //kDebug(planworkDbg());
    QString id = wp->id();
    if ( m_packageMap.contains( id ) ) {
        if ( KMessageBox::warningYesNo( 0, i18n("<p>The work package already exists in the projects store.</p>"
                "<p>Project: %1<br>Task: %2</p>"
                "<p>Do you want to update the existing package with data from the new?</p>",
                wp->project()->name(), wp->node()->name()) ) == KMessageBox::No ) {
            delete wp;
            return false;
        }
        m_packageMap[ id ]->merge( this, wp, store );
        delete wp;
        return true;
    }
    wp->setFilePath( m_loadingFromProjectStore ? wp->fileName( this ) : localFilePath() );
    m_packageMap[ id ] = wp;
    if ( ! m_loadingFromProjectStore ) {
        wp->saveToProjects( this );
    }
    connect( wp->project(), SIGNAL(projectChanged()), wp, SLOT(projectChanged()) );
    connect ( wp, SIGNAL(modified(bool)), this, SLOT(setModified(bool)) );
    emit workPackageAdded( wp, indexOf( wp ) );
    connect(wp, SIGNAL(saveWorkPackage(WorkPackage*)), SLOT(saveWorkPackage(WorkPackage*)));
    return true;
}

void Part::removeWorkPackage( Node *node, MacroCommand *m )
{
    //kDebug(planworkDbg())<<node->name();
    WorkPackage *wp = findWorkPackage( node );
    if ( wp == 0 ) {
        KMessageBox::error( 0, i18n("Remove failed. Cannot find work package") );
        return;
    }
    PackageRemoveCmd *cmd = new PackageRemoveCmd( this, wp, i18nc( "(qtundo-format)", "Remove work package" ) );
    if ( m ) {
        m->addCommand( cmd );
    } else {
        addCommand( cmd );
    }
}

void Part::removeWorkPackages( const QList<Node*> &nodes )
{
    //kDebug(planworkDbg())<<node->name();
    MacroCommand *m = new MacroCommand( i18ncp( "(qtundo-format)", "Remove work package", "Remove work packages", nodes.count() ) );
    foreach ( Node *n, nodes ) {
        removeWorkPackage( n, m );
    }
    if ( m->isEmpty() ) {
        delete m;
    } else {
        addCommand( m );
    }
}

void Part::removeWorkPackage( WorkPackage *wp )
{
    //kDebug(planworkDbg());
    int row = indexOf( wp );
    if ( row >= 0 ) {
        m_packageMap.remove( m_packageMap.keys().at( row ) );
        emit workPackageRemoved( wp, row );
    }
}

void Part::addWorkPackage( WorkPackage *wp )
{
    //kDebug(planworkDbg());
    QString id = wp->id();
    Q_ASSERT( ! m_packageMap.contains( id ) );
    m_packageMap[ id ] = wp;
    emit workPackageAdded( wp, indexOf( wp ) );
}

bool Part::loadWorkPackages()
{
    m_loadingFromProjectStore = true;
    KStandardDirs *sd = componentData().dirs();
    QStringList lst = sd->findAllResources( "projects", "*.planwork", KStandardDirs::Recursive | KStandardDirs::NoDuplicates );
    //kDebug(planworkDbg())<<lst;
    foreach ( const QString &file, lst ) {
        if ( ! loadNativeFormatFromStore( file ) ) {
            KMessageBox::information( 0, i18n( "Failed to load file:<br>%1" , file ) );
        }
    }
    m_loadingFromProjectStore = false;
    return true;

}

bool Part::loadKPlatoWorkPackages()
{
    m_loadingFromProjectStore = true;
    KStandardDirs *sd = componentData().dirs();
    QStringList lst = sd->findAllResources( "planprojects", "*.planwork", KStandardDirs::Recursive | KStandardDirs::NoDuplicates );
    //kDebug(planworkDbg())<<lst;
    foreach ( const QString &file, lst ) {
        if ( ! loadNativeFormatFromStore( file ) ) {
            KMessageBox::information( 0, i18n( "Failed to load file:<br>%1" , file ) );
        }
    }
    m_loadingFromProjectStore = false;
    return true;
}

bool Part::loadNativeFormatFromStore(const QString& file)
{
    kDebug(planworkDbg())<<file;
    KoStore * store = KoStore::createStore(file, KoStore::Read, "", KoStore::Auto);

    if (store->bad()) {
        KMessageBox::error( 0, i18n("Not a valid work package file:<br>%1", file) );
        delete store;
        QApplication::restoreOverrideCursor();
        return false;
    }

    const bool success = loadNativeFormatFromStoreInternal(store);

    delete store;

    return success;
}

bool Part::loadNativeFormatFromStoreInternal(KoStore * store)
{
    if (store->hasFile("root")) {
        KoXmlDocument doc;
        bool ok = loadAndParse(store, "root", doc);
        if (ok) {
            ok = loadXML(doc, store);
        }
        if (!ok) {
            QApplication::restoreOverrideCursor();
            return false;
        }

    } else {
        kError(30003) << "ERROR: No maindoc.xml" << endl;
        KMessageBox::error( 0, i18n("Invalid document. The document does not contain 'maindoc.xml'.") );
        QApplication::restoreOverrideCursor();
        return false;
    }
//     if (store->hasFile("documentinfo.xml")) {
//         KoXmlDocument doc;
//         if (oldLoadAndParse(store, "documentinfo.xml", doc)) {
//             d->m_docInfo->load(doc);
//         }
//     } else {
//         //kDebug(planworkDbg()) <<"cannot open document info";
//         delete d->m_docInfo;
//         d->m_docInfo = new KoDocumentInfo(this);
//     }

    bool res = completeLoading(store);
    QApplication::restoreOverrideCursor();
    return res;
}

bool Part::loadAndParse(KoStore* store, const QString& filename, KoXmlDocument& doc)
{
    //kDebug(planworkDbg()) <<"Trying to open" << filename;

    if (!store->open(filename)) {
        kWarning() << "Entry " << filename << " not found!";
        KMessageBox::error( 0, i18n("Failed to open file: %1", filename) );
        return false;
    }
    // Error variables for QDomDocument::setContent
    QString errorMsg;
    int errorLine, errorColumn;
    bool ok = doc.setContent(store->device(), &errorMsg, &errorLine, &errorColumn);
    store->close();
    if (!ok) {
        kError() << "Parsing error in " << filename << "! Aborting!" << endl
        << " In line: " << errorLine << ", column: " << errorColumn << endl
        << " Error message: " << errorMsg;
        KMessageBox::error( 0, i18n("Parsing error in file '%1' at line %2, column %3<br>Error message: %4", filename  , errorLine, errorColumn ,
                                   QCoreApplication::translate("QXml", errorMsg.toUtf8(), 0,
                                                               QCoreApplication::UnicodeUTF8)) );
        return false;
    }
    return true;
}

bool Part::loadXML( const KoXmlDocument &document, KoStore* store )
{
    kDebug(planworkDbg());
    QString value;
    KoXmlElement plan = document.documentElement();

    // Check if this is the right app
    value = plan.attribute( "mime", QString() );
    if ( value.isEmpty() ) {
        kError() << "No mime type specified!" << endl;
        KMessageBox::error( 0, i18n( "Invalid document. No mimetype specified." ) );
        return false;
    } else if ( value == "application/x-vnd.kde.kplato.work" ) {
        return loadKPlatoXML( document, store );
    } else if ( value != "application/x-vnd.kde.plan.work" ) {
        kError() << "Unknown mime type " << value;
        KMessageBox::error( 0, i18n( "Invalid document. Expected mimetype application/x-vnd.kde.plan.work, got %1", value ) );
        return false;
    }
    QString syntaxVersion = plan.attribute( "version", PLANWORK_FILE_SYNTAX_VERSION );
    m_xmlLoader.setWorkVersion( syntaxVersion );
    if ( syntaxVersion > PLANWORK_FILE_SYNTAX_VERSION ) {
        int ret = KMessageBox::warningContinueCancel(
                      0, i18n( "This document is a newer version than supported by PlanWork (syntax version: %1)<br>"
                               "Opening it in this version of PlanWork will lose some information.", syntaxVersion ),
                      i18n( "File-Format Mismatch" ), KGuiItem( i18n( "Continue" ) ) );
        if ( ret == KMessageBox::Cancel ) {
            return false;
        }
    }
    m_xmlLoader.setVersion( plan.attribute( "plan-version", PLAN_FILE_SYNTAX_VERSION ) );
    m_xmlLoader.startLoad();
    WorkPackage *wp = new WorkPackage( m_loadingFromProjectStore );
    wp->loadXML( plan, m_xmlLoader );
    m_xmlLoader.stopLoad();
    if ( ! setWorkPackage( wp, store ) ) {
        // rejected, so nothing changed...
        return true;
    }
    emit changed();
    return true;
}

bool Part::loadKPlatoXML( const KoXmlDocument &document, KoStore* )
{
    kDebug(planworkDbg());
    QString value;
    KoXmlElement plan = document.documentElement();

    // Check if this is the right app
    value = plan.attribute( "mime", QString() );
    if ( value.isEmpty() ) {
        kError() << "No mime type specified!" << endl;
        KMessageBox::error( 0, i18n( "Invalid document. No mimetype specified." ) );
        return false;
    } else if ( value != "application/x-vnd.kde.kplato.work" ) {
        kError() << "Unknown mime type " << value;
        KMessageBox::error( 0, i18n( "Invalid document. Expected mimetype application/x-vnd.kde.kplato.work, got %1", value ) );
        return false;
    }
    QString syntaxVersion = plan.attribute( "version", KPLATOWORK_MAX_FILE_SYNTAX_VERSION );
    m_xmlLoader.setWorkVersion( syntaxVersion );
    if ( syntaxVersion > KPLATOWORK_MAX_FILE_SYNTAX_VERSION ) {
        int ret = KMessageBox::warningContinueCancel(
                      0, i18n( "This document is a newer version than supported by PlanWork (syntax version: %1)<br>"
                               "Opening it in this version of PlanWork will lose some information.", syntaxVersion ),
                      i18n( "File-Format Mismatch" ), KGuiItem( i18n( "Continue" ) ) );
        if ( ret == KMessageBox::Cancel ) {
            return false;
        }
    }
    m_xmlLoader.setMimetype( value );
    m_xmlLoader.setVersion( plan.attribute( "kplato-version", KPLATO_MAX_FILE_SYNTAX_VERSION ) );
    m_xmlLoader.startLoad();
    WorkPackage *wp = new WorkPackage( m_loadingFromProjectStore );
    wp->loadKPlatoXML( plan, m_xmlLoader );
    m_xmlLoader.stopLoad();
    if ( ! setWorkPackage( wp ) ) {
        // rejected, so nothing changed...
        return true;
    }
    emit changed();
    return true;
}

bool Part::completeLoading( KoStore * )
{
    return true;
}

KUrl Part::extractFile( const Document *doc )
{
    WorkPackage *wp = findWorkPackage( doc );
    return wp == 0 ? KUrl() : wp->extractFile( doc );
}

int Part::docType( const Document *doc ) const
{
    DocumentChild *ch = findChild( doc );
    if ( ch == 0 ) {
        return DocumentChild::Type_Unknown;
    }
    return ch->type();
}

DocumentChild *Part::findChild( const Document *doc ) const
{
    foreach ( const WorkPackage *wp, m_packageMap ) {
        DocumentChild *c = wp->findChild( doc );
        if ( c ) {
            return c;
        }
    }
    return 0;
}

WorkPackage *Part::findWorkPackage( const Document *doc ) const
{
    foreach ( const WorkPackage *wp, m_packageMap ) {
        if ( wp->contains( doc ) ) {
            return const_cast<WorkPackage*>( wp );
        }
    }
    return 0;
}

WorkPackage *Part::findWorkPackage( const DocumentChild *child ) const
{
    foreach ( const WorkPackage *wp, m_packageMap ) {
        if ( wp->contains( child ) ) {
            return const_cast<WorkPackage*>( wp );
        }
    }
    return 0;
}

WorkPackage *Part::findWorkPackage( const Node *node ) const
{
    return m_packageMap.value( node->projectNode()->id() + node->id() );
}

bool Part::editWorkpackageDocument( const Document *doc )
{
    //kDebug(planworkDbg())<<doc<<doc->url();
    // start in any suitable application
    return editOtherDocument( doc );
}

bool Part::editOtherDocument( const Document *doc )
{
    Q_ASSERT( doc != 0 );
    //kDebug(planworkDbg())<<doc->url();
    WorkPackage *wp = findWorkPackage( doc );
    if ( wp == 0 ) {
        KMessageBox::error( 0, i18n( "Edit failed. Cannot find a work package." ) );
        return false;
    }
    return wp->addChild( this, doc );
}

void Part::viewWorkpackageDocument( Document *doc )
{
    kDebug(planworkDbg())<<doc;
    if ( doc == 0 ) {
        return;
    }
    KUrl filename;
    if ( doc->sendAs() == Document::SendAs_Copy ) {
        filename = extractFile( doc );
    } else {
        filename = doc->url();
    }
    // open for view
    viewDocument( filename );
}

bool Part::removeDocument( Document *doc )
{
    if ( doc == 0 ) {
        return false;
    }
    WorkPackage *wp = findWorkPackage( doc );
    if ( wp == 0 ) {
        return false;
    }
    return wp->removeDocument( this, doc );
}

bool Part::viewDocument( const KUrl &filename )
{
    kDebug(planworkDbg())<<"url:"<<filename;
    if ( ! filename.isValid() ) {
        //KMessageBox::error( 0, i18n( "Cannot open document. Invalid url: %1", filename.pathOrUrl() ) );
        return false;
    }
    KRun *run = new KRun( filename, 0 );
    Q_UNUSED(run); // XXX: shouldn't run be deleted?
    return true;
}

void Part::setDocumentClean( bool clean )
{
    kDebug(planworkDbg())<<clean;
    setModified( ! clean );
    if ( ! clean ) {
        saveModifiedWorkPackages();
        return;
    }
}

void Part::setModified( bool mod )
{
    KParts::ReadWritePart::setModified( mod );
    emit captionChanged( QString(), mod );
}

bool Part::saveAs( const KUrl &/*url*/ )
{
    return false;
}

void Part::saveModifiedWorkPackages()
{
    foreach ( WorkPackage *wp, m_packageMap ) {
        if ( wp->isModified() ) {
            saveWorkPackage( wp );
        }
    }
    m_undostack->setClean();
}

void Part::saveWorkPackage( WorkPackage *wp )
{
    wp->saveToProjects( this );
}

bool Part::saveWorkPackages( bool silent )
{
    kDebug(planworkDbg())<<silent;
    foreach ( WorkPackage *wp, m_packageMap ) {
        wp->saveToProjects( this );
    }
    m_undostack->setClean();
    return true;
}

bool Part::completeSaving( KoStore */*store*/ )
{
    return true;
}

QDomDocument Part::saveXML()
{
    kDebug(planworkDbg());
    return QDomDocument();
}

bool Part::queryClose()
{
    kDebug(planworkDbg());
    QList<WorkPackage*> modifiedList;
    foreach ( WorkPackage *wp, m_packageMap ) {
        switch ( wp->queryClose( this ) ) {
            case KMessageBox::No:
                modifiedList << wp;
                break;
            case KMessageBox::Cancel:
                kDebug(planworkDbg())<<"Cancel";
                return false;
        }
    }
    // closeEvent calls queryClose so modified must be reset or else wps are queried all over again
    foreach ( WorkPackage *wp, modifiedList ) {
        wp->setModified( false );
    }
    setModified( false );
    return true;
}

bool Part::openFile()
{
    kDebug(planworkDbg())<<localFilePath();
    return loadNativeFormatFromStore( localFilePath() );
}

bool Part::saveFile()
{
    return false;
}

}  //KPlatoWork namespace

#include "part.moc"
