/*
    This file is part of the Krita program, made within the KDE community.

    Copyright 2006-2011 Friedrich W. H. Kossebau <kossebau@kde.org>
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

#include "mainwindow.h"

// program
#include "program.h"
// tmp
#include <bytearrayviewprofilemanager.h>
// tools
#include <stringsextract/stringsextracttoolview.h>
#include <stringsextract/stringsextracttool.h>
#include <info/infotoolview.h>
#include <info/infotool.h>
#include <filter/filtertoolview.h>
#include <filter/filtertool.h>
#include <charsetconversion/charsetconversiontoolview.h>
#include <charsetconversion/charsetconversiontool.h>
#include <checksum/checksumtoolview.h>
#include <checksum/checksumtool.h>
#include <info/documentinfotoolview.h>
#include <info/documentinfotool.h>
#include <poddecoder/poddecodertoolview.h>
#include <poddecoder/poddecodertool.h>
#include <bytetable/bytetabletoolview.h>
#include <bytetable/bytetabletool.h>
#include <bookmarks/bookmarkstoolview.h>
#include <bookmarks/bookmarkstool.h>

#ifndef NO_STRUCTURES_TOOL
#include <structures/structtoolview.h>
#include <structures/structtool.h>
#endif

// Kasten tools
#include <document/versionview/versionviewtoolview.h>
#include <document/versionview/versionviewtool.h>
#include <documentsystem/filesystembrowser/filesystembrowsertoolview.h>
#include <documentsystem/filesystembrowser/filesystembrowsertool.h>
#include <documentsystem/documentsbrowser/documentstoolview.h>
#include <documentsystem/documentsbrowser/documentstool.h>
#include <document/terminal/terminaltoolview.h>
#include <document/terminal/terminaltool.h>
// controllers
#include <overwriteonly/overwriteonlycontroller.h>
#include <overwritemode/overwritemodecontroller.h>
#include <gotooffset/gotooffsetcontroller.h>
#include <selectrange/selectrangecontroller.h>
#include <search/searchcontroller.h>
#include <replace/replacecontroller.h>
#include <bookmarks/bookmarkscontroller.h>
#include <print/printcontroller.h>
#include <viewconfig/viewconfigcontroller.h>
#include <viewmode/viewmodecontroller.h>
#include <viewstatus/viewstatuscontroller.h>
#include <viewprofiles/viewprofilecontroller.h>
#include <viewprofiles/viewprofilesmanagecontroller.h>
// Kasten controllers
#include <document/modified/modifiedbarcontroller.h>
#include <document/readonly/readonlycontroller.h>
#include <document/readonly/readonlybarcontroller.h>
#include <documentsystem/creator/creatorcontroller.h>
#include <documentsystem/loader/loadercontroller.h>
#include <documentsystem/close/closecontroller.h>
#include <io/setremote/setremotecontroller.h>
#include <io/synchronize/synchronizecontroller.h>
#include <io/clipboard/clipboardcontroller.h>
#include <io/insert/insertcontroller.h>
#include <io/copyas/copyascontroller.h>
#include <io/export/exportcontroller.h>
#include <view/version/versioncontroller.h>
#include <view/zoom/zoomcontroller.h>
#include <view/zoom/zoombarcontroller.h>
#include <view/select/selectcontroller.h>
#include <shellwindow/switchview/switchviewcontroller.h>
#include <shellwindow/viewlistmenu/viewlistmenucontroller.h>
#include <shellwindow/viewareasplit/viewareasplitcontroller.h>
#include <shellwindow/toollistmenu/toollistmenucontroller.h>
#include <shellwindow/fullscreen/fullscreencontroller.h>
/*#include <viewsystem/close/closecontroller.h>*/
#include <program/quit/quitcontroller.h>
// Kasten gui
#include <multidocumentstrategy.h>
#include <modelcodecviewmanager.h>
#include <viewmanager.h>
#include <multiviewareas.h>
#include <statusbar.h>
// Kasten core
#include <modelcodecmanager.h>
#include <documentcreatemanager.h>
#include <documentsyncmanager.h>
#include <documentmanager.h>
// KDE
#include <KUrl>
#include <KGlobal>
#include <KConfigGroup>


namespace Kasten2
{

static const char LoadedUrlsKey[] = "LoadedUrls";

KritaMainWindow::KritaMainWindow( KritaProgram* program )
  : ShellWindow( program->viewManager() ),
    mProgram( program )
{
    setObjectName( QLatin1String("Shell") );

    // there is only one mainwindow, so have this show the document if requested
    connect( mProgram->documentManager(), SIGNAL(focusRequested(Kasten2::AbstractDocument*)),
             SLOT(showDocument(Kasten2::AbstractDocument*)) );
    connect( viewArea(), SIGNAL(dataOffered(const QMimeData*,bool&)),
             SLOT(onDataOffered(const QMimeData*,bool&)) );
    connect( viewArea(), SIGNAL(dataDropped(const QMimeData*)),
             SLOT(onDataDropped(const QMimeData*)) );
    connect( viewArea(), SIGNAL(closeRequest(QList<Kasten2::AbstractView*>)),
             SLOT(onCloseRequest(QList<Kasten2::AbstractView*>)) );

    // XXX: Workaround for Qt 4.4's lacking of proper handling of the initial layout of dock widgets
    //      This state is taken from an kritarc where the docker constellation was configured by hand.
    //      Setting this state if none is present seems to work, but there's
    //      still the versioning problem to be accounted for.
    //      Hack borrowed from trunk/koffice/krita/ui/kis_view2.cpp:
    const QString mainWindowState = QLatin1String(
"AAAA/wAAAAD9AAAAAwAAAAAAAADPAAACg/wCAAAAAvsAAAAiAEYAaQBsAGUAUwB5AHMAdABlAG0AQgByAG8AdwBzAGUAcgAAAABJAAACgwAAAB4BAAAF+wAAABIARABvAGMAdQBtAGUAbgB0AHMAAAAASQAAAmMAAABeAQAABQAAAAEAAAGcAAACXPwCAAAACPsAAAAUAFAATwBEAEQAZQBjAG8AZABlAHIAAAAAQgAAARMAAAB9AQAABfsAAAAUAFMAdAByAHUAYwB0AFQAbwBvAGwAAAAAQgAAAlwAAAB9AQAABfsAAAAQAFYAZQByAHMAaQBvAG4AcwAAAABNAAAAVgAAAF4BAAAF+wAAABgAQgBpAG4AYQByAHkARgBpAGwAdABlAHIAAAABegAAAM0AAAC8AQAABfsAAAAQAEMAaABlAGMAawBzAHUAbQAAAAF8AAAAywAAAL0BAAAF/AAAAREAAADlAAAAAAD////6AAAAAAEAAAAE+wAAABAAQwBoAGUAYwBrAFMAdQBtAQAAAAD/////AAAAAAAAAAD7AAAAEgBCAG8AbwBrAG0AYQByAGsAcwIAAALBAAAAPQAAAT8AAAFk+wAAAA4AUwB0AHIAaQBuAGcAcwAAAAAA/////wAAAQ8BAAAF+wAAAAgASQBuAGYAbwAAAAGRAAABTAAAAIUBAAAF+wAAABIAQgB5AHQAZQBUAGEAYgBsAGUAAAAAUwAAAjkAAAB9AQAABfsAAAAYAEQAbwBjAHUAbQBlAG4AdABJAG4AZgBvAAAAAEkAAAJjAAAA+wEAAAUAAAADAAAAAAAAAAD8AQAAAAH7AAAAEABUAGUAcgBtAGkAbgBhAGwAAAAAAP////8AAABPAQAABQAABBUAAAGLAAAABAAAAAQAAAAIAAAACPwAAAABAAAAAgAAAAEAAAAWAG0AYQBpAG4AVABvAG8AbABCAGEAcgEAAAAAAAAEBgAAAAAAAAAA");
    const char mainWindowStateKey[] = "State";
    KConfigGroup group( KGlobal::config(), QLatin1String("MainWindow") );
    if( !group.hasKey(mainWindowStateKey) )
        group.writeEntry( mainWindowStateKey, mainWindowState );

    setStatusBar( new Kasten2::StatusBar(this) );

    setupControllers();
    setupGUI();

    // all controllers which use plugActionList have to do so after(!) setupGUI() or their entries will be removed
    // TODO: why is this so?
    // tmp
    addXmlGuiController( new ToolListMenuController(this,this) );
    addXmlGuiController( new ViewListMenuController(viewManager(),viewArea(),this) );
}

void KritaMainWindow::setupControllers()
{
    MultiDocumentStrategy* const documentStrategy = mProgram->documentStrategy();
    ViewManager* const viewManager = this->viewManager();
    MultiViewAreas* const viewArea = this->viewArea();
    ModelCodecViewManager* const codecViewManager = viewManager->codecViewManager();
    DocumentManager* const documentManager = mProgram->documentManager();
    ModelCodecManager* const codecManager = documentManager->codecManager();
    DocumentSyncManager* const syncManager = documentManager->syncManager();
    // tmp
    ByteArrayViewProfileManager* const byteArrayViewProfileManager = mProgram->byteArrayViewProfileManager();

    // general, part of Kasten
    addXmlGuiController( new CreatorController(codecManager,
                                               documentStrategy,this) );
    addXmlGuiController( new LoaderController(documentStrategy,this) );
    addXmlGuiController( new SetRemoteController(syncManager,this) );
    addXmlGuiController( new SynchronizeController(syncManager,this) );
    addXmlGuiController( new ExportController(codecViewManager,
                                              codecManager,this) );
    addXmlGuiController( new CloseController(documentStrategy,this) );
    addXmlGuiController( new VersionController(this) );
    addXmlGuiController( new ReadOnlyController(this) );
    addXmlGuiController( new SwitchViewController(viewArea,this) );
    addXmlGuiController( new ViewAreaSplitController(viewManager,viewArea,this) );
    addXmlGuiController( new FullScreenController(this) );
    addXmlGuiController( new QuitController(0,this) );

    addXmlGuiController( new ZoomController(this) );
    addXmlGuiController( new SelectController(this) );
    addXmlGuiController( new ClipboardController(this) );
    addXmlGuiController( new InsertController(codecViewManager,
                                              codecManager,this) );
    addXmlGuiController( new CopyAsController(codecViewManager,
                                              codecManager,this) );

    addTool( new FileSystemBrowserToolView(new FileSystemBrowserTool( syncManager )) );
    addTool( new DocumentsToolView(new DocumentsTool( documentManager )) );
    addTool( new TerminalToolView(new TerminalTool( syncManager )) );
#ifndef NDEBUG
    addTool( new VersionViewToolView(new VersionViewTool()) );
#endif

    // Krita specific
//     addXmlGuiController( new OverwriteOnlyController(this) );
    addXmlGuiController( new OverwriteModeController(this) );
    addXmlGuiController( new SearchController(this,this) );
    addXmlGuiController( new ReplaceController(this,this) );
    addXmlGuiController( new GotoOffsetController(viewArea,this) );
    addXmlGuiController( new SelectRangeController(viewArea,this) );
    addXmlGuiController( new BookmarksController(this) );
    addXmlGuiController( new PrintController(this) );
    addXmlGuiController( new ViewConfigController(this) );
    addXmlGuiController( new ViewModeController(this) );
    addXmlGuiController( new ViewProfileController(byteArrayViewProfileManager,this,this) );
    addXmlGuiController( new ViewProfilesManageController(this,byteArrayViewProfileManager,this) );

    Kasten2::StatusBar* const bottomBar = static_cast<Kasten2::StatusBar*>( statusBar() );
    addXmlGuiController( new ViewStatusController(bottomBar) );
    addXmlGuiController( new ModifiedBarController(bottomBar) );
    addXmlGuiController( new ReadOnlyBarController(bottomBar) );
    addXmlGuiController( new ZoomBarController(bottomBar) );

    addTool( new DocumentInfoToolView(new DocumentInfoTool(syncManager)) );
    addTool( new ChecksumToolView(new ChecksumTool()) );
    addTool( new FilterToolView(new FilterTool()) );
    addTool( new CharsetConversionToolView(new CharsetConversionTool()) );
    addTool( new StringsExtractToolView(new StringsExtractTool()) );
    addTool( new ByteTableToolView(new ByteTableTool()) );
    addTool( new InfoToolView(new InfoTool()) );
    addTool( new PODDecoderToolView(new PODDecoderTool()) );
#ifndef NO_STRUCTURES_TOOL
    addTool( new StructToolView(new StructTool()) );
#endif
    addTool( new BookmarksToolView(new BookmarksTool()) );
}

bool KritaMainWindow::queryClose()
{
    // TODO: query the document manager or query the view manager?
    return mProgram->documentManager()->canCloseAll();
}

void KritaMainWindow::saveProperties( KConfigGroup& configGroup )
{
    DocumentManager* const documentManager = mProgram->documentManager();
    DocumentSyncManager* const syncManager = documentManager->syncManager();
    const QList<AbstractDocument*> documents = documentManager->documents();

    QStringList urls;
    foreach( AbstractDocument* document, documents )
        urls.append( syncManager->urlOf(document).url() );

    configGroup.writePathEntry( LoadedUrlsKey, urls );
}

void KritaMainWindow::readProperties( const KConfigGroup& configGroup )
{
    const QStringList urls = configGroup.readPathEntry( LoadedUrlsKey, QStringList() );

    DocumentManager* const documentManager = mProgram->documentManager();
    DocumentSyncManager* const syncManager = documentManager->syncManager();
    DocumentCreateManager* const createManager = documentManager->createManager();
    foreach( const KUrl& url, urls )
    {
        if( url.isEmpty() )
            createManager->createNew();
        else
            syncManager->load( url );
        // TODO: set view to offset
        // if( offset != -1 )
    }
}

void KritaMainWindow::onDataOffered( const QMimeData* mimeData, bool& accept )
{
    accept = KUrl::List::canDecode( mimeData )
             || mProgram->documentManager()->createManager()->canCreateNewFromData( mimeData );
}

void KritaMainWindow::onDataDropped( const QMimeData* mimeData )
{
    const KUrl::List urls = KUrl::List::fromMimeData( mimeData );

    DocumentManager* const documentManager = mProgram->documentManager();
    if( ! urls.isEmpty() )
    {
        DocumentSyncManager* const syncManager = documentManager->syncManager();

        foreach( const KUrl& url, urls )
            syncManager->load( url );
    }
    else
        documentManager->createManager()->createNewFromData( mimeData, true );
}

void KritaMainWindow::onCloseRequest( const QList<Kasten2::AbstractView*>& views )
{
    // group views per document
    QHash<AbstractDocument*,QList<AbstractView*> > viewsToClosePerDocument;
    foreach( AbstractView* view, views )
    {
        AbstractDocument* document = view->findBaseModel<AbstractDocument*>();
        viewsToClosePerDocument[document].append( view );
    }

    // find documents which lose all views
    const QList<AbstractView*> allViews = viewManager()->views();
    foreach( AbstractView* view, allViews )
    {
        AbstractDocument* document = view->findBaseModel<AbstractDocument*>();
        QHash<AbstractDocument*,QList<AbstractView*> >::Iterator it =
            viewsToClosePerDocument.find( document );

        if( it != viewsToClosePerDocument.end() )
        {
            const QList<AbstractView*>& viewsOfDocument = it.value();
            const bool isAnotherView = ! viewsOfDocument.contains( view );
            if( isAnotherView )
                viewsToClosePerDocument.erase( it );
        }
    }

    const QList<AbstractDocument*> documentsWithoutViews = viewsToClosePerDocument.keys();

    DocumentManager* const documentManager = mProgram->documentManager();
    if( documentManager->canClose(documentsWithoutViews) )
    {
        viewManager()->removeViews( views );
        documentManager->closeDocuments( documentsWithoutViews );
    }
}


KritaMainWindow::~KritaMainWindow() {}

}
