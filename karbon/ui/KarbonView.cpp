/* This file is part of the KDE project
   Copyright (C) 2001-2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2001-2005,2007 Rob Buis <buis@kde.org>
   Copyright (C) 2002-2003,2005 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   Copyright (C) 2002-2003,2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2002-2006 Stephan Binner <binner@kde.org>
   Copyright (C) 2002,2005 David Faure <faure@kde.org>
   Copyright (C) 2002 Benoit Vautrin <benoit.vautrin@free.fr>
   Copyright (C) 2002,2005-2007 Thomas Zander <zander@kde.org>
   Copyright (C) 2003 Dirk Mueller <mueller@kde.org>
   Copyright (C) 2003,2006 Stephan Kulow <coolo@kde.org>
   Copyright (C) 2004 Brad Hards <bradh@frogmouth.net>
   Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2005 Yann Bodson <yann.bodson@online.fr>
   Copyright (C) 2005-2006 Boudewijn Rempt <boud@valdyas.org>
   Copyright (C) 2005-2008 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2005-2006 Peter Simonsson <psn@linux.se>
   Copyright (C) 2005-2006 Sven Langkamp <sven.langkamp@gmail.com>
   Copyright (C) 2005-2006 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2005-2006 Casper Boemann <cbr@boemann.dk>
   Copyright (C) 2006 Martin Ellis <martin.ellis@kdemail.net>
   Copyright (C) 2006 Adriaan de Groot <groot@kde.org>
   Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>
   Copyright (C) 2006-2007 Thorsten Zachmann <t.zachmann@zagge.de>
   Copyright (C) 2006 Andreas Hartmetz <ahartmetz@gmail.com>
   Copyright (C) 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright (C) 2006-2007 Aaron J. Seigo <aseigo@kde.org>
   Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

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

#include "KarbonView.h"

// Dialogs.
#include "KarbonConfigureDialog.h"

// Dockers.
#include "vdocumentdocker.h"
#include "KarbonLayerDocker.h"

// The rest.
#include "Karbon.h"
#include "KarbonFactory.h"
#include "KarbonPart.h"
#include "KarbonCanvas.h"
#include "KarbonPrintJob.h"
#include "KarbonZoomController.h"
#include "KarbonSmallStylePreview.h"
//#include "karbon_drag.h"

#include <KoMainWindow.h>
#include <KoLineBorder.h>
#include <KoCanvasController.h>
#include <KoCanvasResourceProvider.h>
#include <KoFilterManager.h>
#include <KoUnitDoubleSpinBox.h>
#include <KoPageLayoutDialog.h>
#include <KoRuler.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>
#include <KoToolDocker.h>
#include <KoToolDockerFactory.h>
#include <KoShapeManager.h>
#include <KoShapeController.h>
#include <KoShapeContainer.h>
#include <KoShapeGroup.h>
#include <KoShapeUngroupCommand.h>
#include <KoShapeCreateCommand.h>
#include <KoShapeDeleteCommand.h>
#include <KoShapeReorderCommand.h>
#include <KoShapeBorderCommand.h>
#include <KoShapeBackgroundCommand.h>
#include <KoParameterToPathCommand.h>
#include <KoSelection.h>
#include <KoZoomAction.h>
#include <KoZoomHandler.h>
#include <KoPathShape.h>
#include <KoPathCombineCommand.h>
#include <KoPathSeparateCommand.h>
#include <KoPathReverseCommand.h>
#include <KoToolBoxFactory.h>
#include <KoParameterShape.h>
#include <KoRulerController.h>
#include <KoDockRegistry.h>
#include <KoDockerManager.h>
#include <KoShapeLayer.h>
#include <KoColorBackground.h>

// kde header
#include <kaction.h>
#include <kcolormimedata.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kdeversion.h>
#include <kcomponentdata.h>
#include <kactioncollection.h>
#include <kxmlguifactory.h>
#include <kicon.h>
#include <kstatusbar.h>
#include <kfiledialog.h>
#include <kstandardaction.h>
#include <kparts/partmanager.h>
#include <KToggleAction>

// qt header
#include <QtGui/QIcon>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>
#include <QtGui/QPixmap>
#include <QtGui/QDropEvent>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtCore/QTimer>
#include <QtCore/QEvent>
#include <Qt3Support/q3dragobject.h>
#include <Qt3Support/q3popupmenu.h>
#include <QtGui/QPrinter>


#include <unistd.h>

// Only for debugging.
#include <kdebug.h>

// Uncomment the #define below to print lots of debug information about the view.
// Or use the -DKARBON_DEBUG_VIEW flag when using cmake, so the code stays the same.
#define KARBON_DEBUG_VIEW

#ifdef KARBON_DEBUG_VIEW
#define debugView(text) kDebug(38000) <<"KARBON_DEBUG_VIEW:" << text
#else
#define debugView(text)
#endif

class KarbonView::Private
{
public:
    Private( KarbonPart * p)
        : part(p), canvas( 0 )
    {}

    KarbonPart * part;
    KarbonCanvas * canvas;
    KoCanvasController * canvasController;
    KoRuler * horizRuler;
    KoRuler * vertRuler;

    // actions:
    KAction * groupObjects;
    KAction * ungroupObjects;
    KAction * closePath;
    KAction * combinePath;
    KAction * separatePath;
    KAction * reversePath;
    KAction * intersectPath;
    KAction * subtractPath;
    KAction * unitePath;
    KAction * excludePath;
    KAction * configureAction;
    KAction * deleteSelectionAction;

    KToggleAction * viewAction;
    KToggleAction * showRulerAction;
    KToggleAction * showGridAction;
    KToggleAction * snapGridAction;
    KToggleAction * showPageMargins;
    KToggleAction * showGuidesAction;

    //Status Bar
    QLabel * status;       ///< ordinary status
    QLabel * cursorCoords; ///< cursor coordinates
};

KarbonView::KarbonView( KarbonPart* p, QWidget* parent )
    : KoView( p, parent ), KXMLGUIBuilder( shell() ), d( new Private( p ) )
{
    debugView("KarbonView::KarbonView");

    setComponentData( KarbonFactory::componentData(), true );

    setClientBuilder( this );

    if( !p->isReadWrite() )
        setXMLFile( QString::fromLatin1( "karbon_readonly.rc" ) );
    else
        setXMLFile( QString::fromLatin1( "karbon.rc" ) );

    const int viewMargin = 250;
    d->canvas = new KarbonCanvas( p );
    d->canvas->setParent( this );
    d->canvas->setDocumentViewMargin( viewMargin );
    connect( d->canvas->shapeManager()->selection(), SIGNAL( selectionChanged() ), 
             this, SLOT( selectionChanged() ) );

    d->canvasController = new KoCanvasController(this);
    d->canvasController->setMinimumSize( QSize(viewMargin+50,viewMargin+50) );
    d->canvasController->setCanvas(d->canvas);
    d->canvasController->setCanvasMode( KoCanvasController::Infinite );
    // always show srollbars which fixes some nasty infinite
    // recursion when scrollbars are disabled during resizing
    d->canvasController->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    d->canvasController->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    d->canvasController->show();

    // set up status bar message
    d->status = new QLabel( QString(), statusBar() );
    d->status->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    d->status->setMinimumWidth( 300 );
    addStatusBarItem( d->status, 1 );
    connect( KoToolManager::instance(), SIGNAL(changedStatusText(const QString &)),
             d->status, SLOT(setText(const QString &)) );
    d->cursorCoords = new QLabel( QString(), statusBar() );
    d->cursorCoords->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    d->cursorCoords->setMinimumWidth( 50 );
    addStatusBarItem( d->cursorCoords, 0 );

    // TODO maybe the zoomHandler should be a member of the view and not the canvas.
    // set up the zoom controller
    KarbonZoomController * zoomController = new KarbonZoomController( d->canvasController, actionCollection() );
    zoomController->setPageSize( d->part->document().pageSize() );
    addStatusBarItem( zoomController->zoomAction()->createWidget( statusBar() ), 0 );
    zoomController->setZoomMode( KoZoomMode::ZOOM_PAGE );

    addStatusBarItem( new KarbonSmallStylePreview( statusBar() ), 0 );

    // layout:
    QGridLayout *layout = new QGridLayout();
    layout->setMargin(0);
    layout->addWidget(d->canvasController, 1, 1);

    initActions();

    unsigned int max = part()->maxRecentFiles();
    setNumberOfRecentFiles( max );

    // widgets:
    d->horizRuler = new KoRuler( this, Qt::Horizontal, d->canvas->viewConverter() );
    d->horizRuler->setShowMousePosition(true);
    d->horizRuler->setUnit(p->unit());
    d->horizRuler->setRightToLeft(false);
    new KoRulerController( d->horizRuler, d->canvas->resourceProvider() );

    layout->addWidget( d->horizRuler, 0, 1 );
    connect( p, SIGNAL( unitChanged( KoUnit ) ), this, SLOT( updateUnit( KoUnit ) ) );

    d->vertRuler = new KoRuler( this, Qt::Vertical, d->canvas->viewConverter() );
    d->vertRuler->setShowMousePosition(true);
    d->vertRuler->setUnit(p->unit());
    layout->addWidget( d->vertRuler, 1, 0 );

    connect( d->canvas, SIGNAL(documentOriginChanged( const QPoint &)), this, SLOT(pageOffsetChanged()));
    connect( d->canvasController, SIGNAL(canvasOffsetXChanged(int)), this, SLOT(pageOffsetChanged()));
    connect( d->canvasController, SIGNAL(canvasOffsetYChanged(int)), this, SLOT(pageOffsetChanged()));
    connect( d->canvasController, SIGNAL(canvasMousePositionChanged(const QPoint &)),
            this, SLOT(mousePositionChanged(const QPoint&)));
    connect( d->vertRuler, SIGNAL(guideLineCreated(Qt::Orientation,int)), 
             d->canvasController, SLOT( addGuideLine(Qt::Orientation,int) ) );
    connect( d->horizRuler, SIGNAL(guideLineCreated(Qt::Orientation,int)), 
             d->canvasController, SLOT( addGuideLine(Qt::Orientation,int) ) );

    updateRuler();

    if( shell() )
    {
        KoToolManager::instance()->addController( d->canvasController );
        KoToolManager::instance()->registerTools( actionCollection(), d->canvasController );
        // set the first layer active
        d->canvasController->canvas()->shapeManager()->selection()->setActiveLayer( part()->document().layers().first() );

        //Create Dockers
        createLayersTabDock();

        KoToolBoxFactory toolBoxFactory(d->canvasController, i18n( "Tools" ) );
        createDockWidget( &toolBoxFactory );

        KoDockerManager *dockerManager = new KoDockerManager(this);
        connect( d->canvasController, SIGNAL( toolOptionWidgetsChanged(const QMap<QString, QWidget *> &) ),
             dockerManager, SLOT( newOptionWidgets(const  QMap<QString, QWidget *> &) ) );

        KoToolManager::instance()->requestToolActivation( d->canvasController );

        bool b = d->showRulerAction->isChecked();
        d->horizRuler->setVisible( b );
        d->vertRuler->setVisible( b );
    }

    setLayout(layout);

    reorganizeGUI();

    setFocusPolicy(Qt::NoFocus);
}

KarbonView::~KarbonView()
{
    debugView("KarbonView::~KarbonView()");

    KoToolManager::instance()->removeCanvasController( d->canvasController );

    delete d;
}

KarbonPart * KarbonView::part() const
{
    return d->part;
}

KarbonCanvas * KarbonView::canvasWidget() const
{
    return d->canvas;
}

void KarbonView::resizeEvent( QResizeEvent* /*event*/ )
{
    debugView("KarbonView::resizeEvent()");

    if(!d->showRulerAction)
        return;

    if(!d->canvas)
        return;

    reorganizeGUI();
}

void KarbonView::dropEvent( QDropEvent *e )
{
    debugView("KarbonView::dropEvent()");

    //Accepts QColor - from Color Manager's KColorPatch
    QColor color = KColorMimeData::fromMimeData( e->mimeData() );
    if ( color.isValid() )
    {
        KoSelection * selection = d->canvas->shapeManager()->selection();
        if( ! selection )
            return;

        if( ! part() )
            return;

        if( d->canvas->resourceProvider()->intResource( Karbon::ActiveStyle ) == Karbon::Foreground )
        {
            QList<KoShapeBorderModel*> borders;
            QList<KoShape*> selectedShapes = selection->selectedShapes();
            foreach( KoShape * shape, selectedShapes )
            {
                KoLineBorder * border = dynamic_cast<KoLineBorder*>( shape->border() );
                KoLineBorder * newBorder = 0;
                if( border )
                {
                    newBorder = new KoLineBorder( *border );
                    newBorder->setColor( color );
                }
                else
                {
                    newBorder = new KoLineBorder( 1.0, color );
                }
                borders.append( newBorder );
            }
            d->canvas->addCommand( new KoShapeBorderCommand( selectedShapes, borders, 0 ) );
        }
        else
        {
            KoColorBackground * fill = new KoColorBackground( color );
            d->canvas->addCommand( new KoShapeBackgroundCommand( selection->selectedShapes(), fill, 0 ) );
        }
    }
/* TODO port to flake
    else if( KarbonDrag::decode( e->mimeData(), selection, d->part->document() ) )
    {
        VObject *clipart = selection.first();
        QPointF p( e->pos() );
        p = d->canvas->viewConverter()->viewToDocument( p ); // TODO: or documentToView ?
        QMatrix mat( 1, 0, 0, 1, p.x(), p.y() );

        VTransformCmd trafo( 0L, mat );
        trafo.visit( *clipart );
        VClipartCmd* cmd = new VClipartCmd( &d->part->document(), i18n( "Insert Clipart" ), clipart );

        d->part->addCommand( cmd, true );
    }
*/
}

void KarbonView::fileImportGraphic()
{
    debugView("KarbonView::fileImportGraphic()");

    QStringList filter;
    filter << "application/x-karbon" << "image/svg+xml" << "image/x-wmf" << "image/x-eps" << "application/postscript";
    KFileDialog *dialog = new KFileDialog(KUrl("foo"), "", 0);
    dialog->setCaption("Choose Graphic to Add");
    dialog->setModal(true);
    dialog->setMimeFilter( filter, "application/x-karbon" );
    if(dialog->exec()!=QDialog::Accepted) {
        delete dialog;
        return;
    }
    QString fname = dialog->selectedFile();
    //kDebug(38000) <<"in :" << fname.latin1();
    //kDebug(38000) <<"part()->document()->nativeFormatMimeType().latin1() :" << part()->nativeFormatMimeType();
    //kDebug(38000) <<"dialog->currentMimeFilter().latin1() :" << dialog->currentMimeFilter().latin1();
    if( part()->nativeFormatMimeType() == dialog->currentMimeFilter().toLatin1() )
        part()->mergeNativeFormat( fname );
    else
    {
        KoFilterManager man( part() );
        KoFilter::ConversionStatus status;
        QString importedFile = man.importDocument( fname, status );
        part()->mergeNativeFormat( importedFile );
        if( !importedFile.isEmpty() )
            unlink( QFile::encodeName( importedFile ) );
    }
    delete dialog;
}

void KarbonView::editCut()
{
    debugView("KarbonView::editCut()");

    d->canvas->toolProxy()->cut();
    /*
    addSelectionToClipboard();
    // remove selection
    editDeleteSelection();
    */
}

void KarbonView::editCopy()
{
    debugView("KarbonView::editCopy()");
    d->canvas->toolProxy()->copy();

    /*
    addSelectionToClipboard();
    */
}

void KarbonView::addSelectionToClipboard() const
{
    debugView("KarbonView::addSelectionToClipboard()");

    /*
    if( part()->document().selection()->objects().count() <= 0 )
        return;

    KarbonDrag* kd = new KarbonDrag();
    kd->setObjectList( part()->document().selection()->objects() );
    QApplication::clipboard()->setMimeData( kd->mimeData() );
    */
}

void KarbonView::editPaste()
{
    debugView("KarbonView::editPaste()");

    d->canvas->toolProxy()->paste();
    /*
    KarbonDrag kd;
    VObjectList objects;

    if( !kd.decode( QApplication::clipboard()->mimeData(), objects, part()->document() ) )
        return;

    // Paste with a small offset.
    double copyOffset = part()->componentData().config()->group("").readEntry( "CopyOffset", 10 );
    part()->addCommand( new VInsertCmd( &part()->document(),
                                        objects.count() == 1
                                            ? i18n( "Paste Object" )
                                            : i18n( "Paste Objects" ),
                                        &objects, copyOffset ),
                        true );

    part()->repaintAllViews();
    selectionChanged();
    */
}

void KarbonView::editSelectAll()
{
    debugView("KarbonView::editSelectAll()");

    KoSelection* selection = d->canvas->shapeManager()->selection();
    if( ! selection )
        return;

    QList<KoShape*> shapes = part()->document().shapes();
    kDebug(38000) <<"shapes.size() =" << shapes.size();

    foreach( KoShape* shape, shapes )
    {
        selection->select( shape );
        shape->update();
    }

    selectionChanged();
}

void KarbonView::editDeselectAll()
{
    debugView("KarbonView::editDeselectAll()");

    KoSelection* selection = d->canvas->shapeManager()->selection();
    if( selection )
        selection->deselectAll();

    selectionChanged();
}

void KarbonView::editDeleteSelection()
{
    debugView("KarbonView::editDeleteSelection()");

    KoSelection* selection = d->canvas->shapeManager()->selection();
    if( ! selection )
        return;

    d->canvas->toolProxy()->deleteSelection();
}

void KarbonView::selectionAlignHorizontalLeft()
{
    debugView("KarbonView::selectionAlignHorizontalLeft()");

    selectionAlign(KoShapeAlignCommand::HorizontalLeftAlignment);
}
void KarbonView::selectionAlignHorizontalCenter()
{
    debugView("KarbonView::selectionAlignHorizontalCenter()");

    selectionAlign(KoShapeAlignCommand::HorizontalCenterAlignment);
}

void KarbonView::selectionAlignHorizontalRight()
{
    debugView("KarbonView::selectionAlignHorizontalRight()");

    selectionAlign(KoShapeAlignCommand::HorizontalRightAlignment);
}

void KarbonView::selectionAlignVerticalTop()
{
    debugView("KarbonView::selectionAlignVerticalTop()");

    selectionAlign(KoShapeAlignCommand::VerticalTopAlignment);
}

void KarbonView::selectionAlignVerticalCenter()
{
    debugView("KarbonView::selectionAlignVerticalCenter()");

    selectionAlign(KoShapeAlignCommand::VerticalCenterAlignment);
}

void KarbonView::selectionAlignVerticalBottom()
{
    debugView("KarbonView::selectionAlignVerticalBottom()");

    selectionAlign(KoShapeAlignCommand::VerticalBottomAlignment);
}

void KarbonView::selectionAlign(KoShapeAlignCommand::Align align)
{
    KoSelection* selection = d->canvas->shapeManager()->selection();
    if( ! selection )
        return;

    QList<KoShape*> selectedShapes = selection->selectedShapes( KoFlake::TopLevelSelection );
    if( selectedShapes.count() < 1)
        return;

    QRectF bRect;
    bRect= (selectedShapes.count() == 1) ? part()->document().boundingRect() : selection->boundingRect();
    KoShapeAlignCommand *cmd = new KoShapeAlignCommand( selectedShapes, align, bRect);

    d->canvas->addCommand( cmd );
}


void KarbonView::selectionDistributeHorizontalCenter()
{
    debugView("KarbonView::selectionDistributeHorizontalCenter()");

    selectionDistribute(KoShapeDistributeCommand::HorizontalCenterDistribution);
}

void KarbonView::selectionDistributeHorizontalGap()
{
    debugView("KarbonView::selectionDistributeHorizontalGap()");

    selectionDistribute(KoShapeDistributeCommand::HorizontalGapsDistribution);
}

void KarbonView::selectionDistributeHorizontalLeft()
{
    debugView("KarbonView::selectionDistributeHorizontalLeft()");

    selectionDistribute(KoShapeDistributeCommand::HorizontalLeftDistribution);
}

void KarbonView::selectionDistributeHorizontalRight()
{
    debugView("KarbonView::selectionDistributeHorizontalRight()");

    selectionDistribute(KoShapeDistributeCommand::HorizontalRightDistribution);
}

void KarbonView::selectionDistributeVerticalCenter()
{
    debugView("KarbonView::selectionDistributeVerticalCenter()");

    selectionDistribute(KoShapeDistributeCommand::VerticalCenterDistribution);
}

void KarbonView::selectionDistributeVerticalGap()
{
    debugView("KarbonView::selectionDistributeVerticalGap()");

    selectionDistribute(KoShapeDistributeCommand::VerticalGapsDistribution);
}

void KarbonView::selectionDistributeVerticalBottom()
{
    debugView("KarbonView::selectionDistributeVerticalBottom()");

    selectionDistribute(KoShapeDistributeCommand::VerticalBottomDistribution);
}

void KarbonView::selectionDistributeVerticalTop()
{
    debugView("KarbonView::selectionDistributeVerticalTop()");

    selectionDistribute(KoShapeDistributeCommand::VerticalTopDistribution);
}

void KarbonView::selectionDistribute(KoShapeDistributeCommand::Distribute distribute)
{
    KoSelection* selection = d->canvas->shapeManager()->selection();
    if( ! selection )
        return;

    QList<KoShape*> selectedShapes = selection->selectedShapes( KoFlake::TopLevelSelection );
    if( selectedShapes.count() < 2) return;

    KoShapeDistributeCommand *cmd = new KoShapeDistributeCommand( selectedShapes, distribute, selection->boundingRect());

    d->canvas->addCommand( cmd );
}

void KarbonView::selectionDuplicate()
{
    debugView("KarbonView::selectionDuplicate()");
    /*
    if ( !part()->document().selection()->objects().count() )
        return;

    VObjectList  objects;

    // Create copies of all the objects and not just the list.
    VObjectListIterator itr( part()->document().selection()->objects() );
    for ( ; itr.current() ; ++itr )    {
        objects.append( itr.current()->clone() );
    }

    // Paste with a small offset.
    double copyOffset = part()->componentData().config()->group("").readEntry( "CopyOffset", 10 );
    part()->addCommand( new VInsertCmd( &part()->document(),
                                        objects.count() == 1
                                            ? i18n( "Duplicate Object" )
                                            : i18n( "Duplicate Objects" ),
                                        &objects, copyOffset ),
                        true );

    part()->repaintAllViews();
    selectionChanged();
    */
}

void KarbonView::selectionBringToFront()
{
    debugView("KarbonView::selectionBringToFront()");

    QList<KoShape*> selectedShapes = d->canvas->shapeManager()->selection()->selectedShapes( KoFlake::TopLevelSelection );
    d->canvas->addCommand( KoShapeReorderCommand::createCommand( selectedShapes, d->canvas->shapeManager(), KoShapeReorderCommand::BringToFront ) );
}

void KarbonView::selectionMoveUp()
{
    debugView("KarbonView::selectionMoveUp()");

    QList<KoShape*> selectedShapes = d->canvas->shapeManager()->selection()->selectedShapes( KoFlake::TopLevelSelection );
    d->canvas->addCommand( KoShapeReorderCommand::createCommand( selectedShapes, d->canvas->shapeManager(), KoShapeReorderCommand::RaiseShape ) );
}

void KarbonView::selectionMoveDown()
{
    debugView("KarbonView::selectionMoveDown()");

    QList<KoShape*> selectedShapes = d->canvas->shapeManager()->selection()->selectedShapes( KoFlake::TopLevelSelection );
    d->canvas->addCommand( KoShapeReorderCommand::createCommand( selectedShapes, d->canvas->shapeManager(), KoShapeReorderCommand::LowerShape ) );
}

void KarbonView::selectionSendToBack()
{
    debugView("KarbonView::selectionSendToBack()");

    QList<KoShape*> selectedShapes = d->canvas->shapeManager()->selection()->selectedShapes( KoFlake::TopLevelSelection );
    d->canvas->addCommand( KoShapeReorderCommand::createCommand( selectedShapes, d->canvas->shapeManager(), KoShapeReorderCommand::SendToBack ) );
}

void KarbonView::groupSelection()
{
    debugView("KarbonView::groupSelection()");

    KoSelection* selection = d->canvas->shapeManager()->selection();
    if( ! selection )
        return;

    QList<KoShape*> selectedShapes = selection->selectedShapes( KoFlake::TopLevelSelection );
    QList<KoShape*> groupedShapes;

    // only group shapes with an unselected parent
    foreach( KoShape* shape, selectedShapes )
    {
        if( selectedShapes.contains( shape->parent() ) )
            continue;
        groupedShapes << shape;
    }
    KoShapeGroup *group = new KoShapeGroup();
    if( selection->activeLayer() )
        selection->activeLayer()->addChild( group );
    QUndoCommand *cmd = new QUndoCommand( i18n("Group shapes") );
    new KoShapeCreateCommand( d->part, group, cmd );
    new KoShapeGroupCommand( group, groupedShapes, cmd );
    d->canvas->addCommand( cmd );
}

void KarbonView::ungroupSelection()
{
    debugView("KarbonView::ungroupSelection()");

    KoSelection* selection = d->canvas->shapeManager()->selection();
    if( ! selection )
        return;

    QList<KoShape*> selectedShapes = selection->selectedShapes( KoFlake::TopLevelSelection );
    QList<KoShape*> containerSet;

    // only ungroup shape containers with an unselected parent
    foreach( KoShape* shape, selectedShapes )
    {
        if( selectedShapes.contains( shape->parent() ) )
            continue;
        containerSet << shape;
    }

    QUndoCommand *cmd = new QUndoCommand( i18n("Ungroup shapes") );

    // add a ungroup command for each found shape container to the macro command
    foreach( KoShape* shape, containerSet )
    {
        KoShapeContainer *container = dynamic_cast<KoShapeContainer*>( shape );
        if( container )
        {
            new KoShapeUngroupCommand( container, container->iterator(), cmd );
            new KoShapeDeleteCommand( d->part, container, cmd );
        }
    }
    d->canvas->addCommand( cmd );
}

void KarbonView::closePath()
{
    debugView("KarbonView::closePath()");

    // TODO add the new close path command here
}

void KarbonView::combinePath()
{
    debugView("KarbonView::combinePath()");

    KoSelection* selection = d->canvas->shapeManager()->selection();
    if( ! selection )
        return;

    QList<KoShape*> selectedShapes = selection->selectedShapes();
    QList<KoPathShape*> paths;

    foreach( KoShape* shape, selectedShapes )
    {
        KoPathShape *path = dynamic_cast<KoPathShape*>( shape );
        if( path )
        {
            paths << path;
            selection->deselect( shape );
        }
    }

    if( paths.size() )
        d->canvas->addCommand( new KoPathCombineCommand( part(), paths ) );
}

void KarbonView::separatePath()
{
    debugView("KarbonView::separatePath()");
    KoSelection* selection = d->canvas->shapeManager()->selection();
    if( ! selection )
        return;

    QList<KoShape*> selectedShapes = selection->selectedShapes();
    QList<KoPathShape*> paths;

    foreach( KoShape* shape, selectedShapes )
    {
        KoPathShape *path = dynamic_cast<KoPathShape*>( shape );
        if( path )
        {
            paths << path;
            selection->deselect( shape );
        }
    }

    if( paths.size() )
        d->canvas->addCommand( new KoPathSeparateCommand( part(), paths ) );
}

void KarbonView::reversePath()
{
    debugView("KarbonView::reversePath()");

    QList<KoPathShape*> paths = selectedPathShapes();
    if( paths.size() )
        d->canvas->addCommand( new KoPathReverseCommand( paths ) );
}

void KarbonView::intersectPaths()
{
    booleanOperation( KarbonBooleanCommand::Intersection );
}

void KarbonView::subtractPaths()
{
    booleanOperation( KarbonBooleanCommand::Subtraction );
}

void KarbonView::unitePaths()
{
    booleanOperation( KarbonBooleanCommand::Union );
}

void KarbonView::excludePaths()
{
    booleanOperation( KarbonBooleanCommand::Exclusion );
}

void KarbonView::booleanOperation( KarbonBooleanCommand::BooleanOperation operation )
{
    KoSelection* selection = d->canvas->shapeManager()->selection();
    if( ! selection )
        return;

    QList<KoShape*> selectedShapes = selection->selectedShapes();
    QList<KoPathShape*> paths;

    foreach( KoShape* shape, selectedShapes )
    {
        KoPathShape *path = dynamic_cast<KoPathShape*>( shape );
        if( path )
        {
            paths << path;
            selection->deselect( shape );
        }
    }

    if( paths.size() == 2 )
    {
        QUndoCommand * macro = new QUndoCommand( i18n("Boolean Operation") );
        KoParameterShape * paramShape = dynamic_cast<KoParameterShape*>( paths[0] );
        if( paramShape && paramShape->isParametricShape() )
            new KoParameterToPathCommand( paramShape, macro );
        paramShape = dynamic_cast<KoParameterShape*>( paths[1] );
        if( paramShape && paramShape->isParametricShape() )
            new KoParameterToPathCommand( paramShape, macro );
        new KarbonBooleanCommand( part(), paths[0], paths[1], operation, macro );
        new KoShapeDeleteCommand( part(), paths[0], macro );
        new KoShapeDeleteCommand( part(), paths[1], macro );
        d->canvas->addCommand( macro );
    }
}

void KarbonView::viewModeChanged(bool outlineMode)
{
    debugView("KarbonView::viewModeChanged()");

    d->canvas->enableOutlineMode( outlineMode );
    d->canvas->updateCanvas(d->canvas->canvasWidget()->rect());
}

void KarbonView::zoomSelection()
{
    KoSelection* selection = d->canvas->shapeManager()->selection();
    if( ! selection )
        return;

    if( ! selection->count() )
        return;

    const KoZoomHandler * zoomHandler = dynamic_cast<const KoZoomHandler*>( d->canvas->viewConverter() );
    if( ! zoomHandler )
        return;

    QRectF bbox = selection->boundingRect();
    QRect viewRect = zoomHandler->documentToView( bbox ).toRect();

    d->canvasController->zoomTo( viewRect.translated( d->canvas->documentOrigin() ) );
    QPointF newCenter = d->canvas->documentOrigin() + zoomHandler->documentToView( bbox.center() );
    d->canvasController->setPreferredCenter( newCenter.toPoint() );
}

void KarbonView::zoomDrawing()
{
    const KoZoomHandler * zoomHandler = dynamic_cast<const KoZoomHandler*>( d->canvas->viewConverter() );
    if( ! zoomHandler )
        return;

    QRectF bbox = d->part->document().contentRect();
    if( bbox.isNull() )
        return;

    QRect viewRect = zoomHandler->documentToView( bbox ).toRect();
    d->canvasController->zoomTo( viewRect.translated( d->canvas->documentOrigin() ) );
    QPointF newCenter = d->canvas->documentOrigin() + zoomHandler->documentToView( bbox.center() );
    d->canvasController->setPreferredCenter( newCenter.toPoint() );
}

void KarbonView::initActions()
{
    debugView("KarbonView::initActions()");

    // view ----->
    d->viewAction  = new KToggleAction(i18n("Outline &Mode"), this);
    actionCollection()->addAction("view_mode", d->viewAction );
    connect(d->viewAction, SIGNAL(toggled(bool)), this, SLOT(viewModeChanged(bool)));

    d->showPageMargins  = new KToggleAction(KIcon("view_margins"), i18n("Show Page Margins"), this);
    actionCollection()->addAction("view_show_margins", d->showPageMargins );
    connect( d->showPageMargins, SIGNAL(toggled(bool)), SLOT(togglePageMargins(bool)));
    d->showPageMargins->setCheckedState(KGuiItem(i18n("Hide Page Margins")));

    // No need for the other actions in read-only (embedded) mode
    if( !shell() )
        return;

    // edit ----->
    actionCollection()->addAction(KStandardAction::Cut,  "edit_cut", this, SLOT(editCut()));
    actionCollection()->addAction(KStandardAction::Copy,  "edit_copy", this, SLOT(editCopy()));
    actionCollection()->addAction(KStandardAction::Paste,  "edit_paste", this, SLOT(editPaste()));
    actionCollection()->addAction(KStandardAction::SelectAll,  "edit_select_all", this, SLOT(editSelectAll()));
    actionCollection()->addAction(KStandardAction::Deselect,  "edit_deselect_all", this, SLOT(editDeselectAll()));

    KAction *actionImportGraphic  = new KAction(i18n("&Import Graphic..."), this);
    actionCollection()->addAction("file_import", actionImportGraphic );
    connect(actionImportGraphic, SIGNAL(triggered()), this, SLOT(fileImportGraphic()));

    d->deleteSelectionAction  = new KAction(KIcon("edit-delete"), i18n("D&elete"), this);
    actionCollection()->addAction("edit_delete", d->deleteSelectionAction );
    d->deleteSelectionAction->setShortcut(QKeySequence("Del"));
    connect(d->deleteSelectionAction, SIGNAL(triggered()), this, SLOT(editDeleteSelection()));
    // edit <-----

    // object ----->
    KAction *actionDuplicate  = new KAction(KIcon("duplicate"), i18nc("Duplicate selection", "&Duplicate"), this);
    actionCollection()->addAction("object_duplicate", actionDuplicate );
    actionDuplicate->setShortcut(QKeySequence("Ctrl+D"));
    connect(actionDuplicate, SIGNAL(triggered()), this, SLOT(selectionDuplicate()));

    KAction *actionBringToFront  = new KAction(KIcon("bring_forward"), i18n("Bring to &Front"), this);
    actionCollection()->addAction("object_move_totop", actionBringToFront );
    actionBringToFront->setShortcut(QKeySequence("Ctrl+Shift+]"));
    connect(actionBringToFront, SIGNAL(triggered()), this, SLOT(selectionBringToFront()));

    KAction *actionRaise  = new KAction(KIcon("raise"), i18n("&Raise"), this);
    actionCollection()->addAction("object_move_up", actionRaise );
    actionRaise->setShortcut(QKeySequence("Ctrl+]"));
    connect(actionRaise, SIGNAL(triggered()), this, SLOT(selectionMoveUp()));

    KAction *actionLower  = new KAction(KIcon("lower"), i18nc("Lower selected shapes", "&Lower"), this);
    actionCollection()->addAction("object_move_down", actionLower );
    actionLower->setShortcut(QKeySequence("Ctrl+["));
    connect(actionLower, SIGNAL(triggered()), this, SLOT(selectionMoveDown()));

    KAction *actionSendToBack  = new KAction(KIcon("send_backward"), i18n("Send to &Back"), this);
    actionCollection()->addAction("object_move_tobottom", actionSendToBack );
    actionSendToBack->setShortcut(QKeySequence("Ctrl+Shift+["));
    connect(actionSendToBack, SIGNAL(triggered()), this, SLOT(selectionSendToBack()));

    KAction *actionAlignLeft  = new KAction(KIcon("aoleft"), i18n("Align Left"), this);
    actionCollection()->addAction("object_align_horizontal_left", actionAlignLeft );
    connect(actionAlignLeft, SIGNAL(triggered()), this, SLOT(selectionAlignHorizontalLeft()));

    KAction *actionAlignCenter  = new KAction(KIcon("aocenterh"), i18n("Align Center (Horizontal)"), this);
    actionCollection()->addAction("object_align_horizontal_center", actionAlignCenter );
    connect(actionAlignCenter, SIGNAL(triggered()), this, SLOT(selectionAlignHorizontalCenter()));

    KAction *actionAlignRight  = new KAction(KIcon("aoright"), i18n("Align Right"), this);
    actionCollection()->addAction("object_align_horizontal_right", actionAlignRight );
    connect(actionAlignRight, SIGNAL(triggered()), this, SLOT(selectionAlignHorizontalRight()));

    KAction *actionAlignTop  = new KAction(KIcon("aotop"), i18n("Align Top"), this);
    actionCollection()->addAction("object_align_vertical_top", actionAlignTop );
    connect(actionAlignTop, SIGNAL(triggered()), this, SLOT(selectionAlignVerticalTop()));

    KAction *actionAlignMiddle  = new KAction(KIcon("aocenterv"), i18n("Align Middle (Vertical)"), this);
    actionCollection()->addAction("object_align_vertical_center", actionAlignMiddle );
    connect(actionAlignMiddle, SIGNAL(triggered()), this, SLOT(selectionAlignVerticalCenter()));

    KAction *actionAlignBottom  = new KAction(KIcon("aobottom"), i18n("Align Bottom"), this);
    actionCollection()->addAction("object_align_vertical_bottom", actionAlignBottom );
    connect(actionAlignBottom, SIGNAL(triggered()), this, SLOT(selectionAlignVerticalBottom()));

    KAction *actionDistributeHorizontalCenter  = new KAction(i18n("Distribute Center (Horizontal)"), this);
    actionCollection()->addAction("object_distribute_horizontal_center", actionDistributeHorizontalCenter );
    connect(actionDistributeHorizontalCenter, SIGNAL(triggered()), this, SLOT(selectionDistributeHorizontalCenter()));

    KAction *actionDistributeHorizontalGap  = new KAction(i18n("Distribute Gaps (Horizontal)"), this);
    actionCollection()->addAction("object_distribute_horizontal_gap", actionDistributeHorizontalGap );
    connect(actionDistributeHorizontalGap, SIGNAL(triggered()), this, SLOT(selectionDistributeHorizontalGap()));

    KAction *actionDistributeLeft  = new KAction(i18n("Distribute Left Borders"), this);
    actionCollection()->addAction("object_distribute_horizontal_left", actionDistributeLeft );
    connect(actionDistributeLeft, SIGNAL(triggered()), this, SLOT(selectionDistributeHorizontalLeft()));

    KAction *actionDistributeRight  = new KAction(i18n("Distribute Right Borders"), this);
    actionCollection()->addAction("object_distribute_horizontal_right", actionDistributeRight );
    connect(actionDistributeRight, SIGNAL(triggered()), this, SLOT(selectionDistributeHorizontalRight()));

    KAction *actionDistributeVerticalCenter  = new KAction(i18n("Distribute Center (Vertical)"), this);
    actionCollection()->addAction("object_distribute_vertical_center", actionDistributeVerticalCenter );
    connect(actionDistributeVerticalCenter, SIGNAL(triggered()), this, SLOT(selectionDistributeVerticalCenter()));

    KAction *actionDistributeVerticalGap  = new KAction(i18n("Distribute Gaps (Vertical)"), this);
    actionCollection()->addAction("object_distribute_vertical_gap", actionDistributeVerticalGap );
    connect(actionDistributeVerticalGap, SIGNAL(triggered()), this, SLOT(selectionDistributeVerticalGap()));

    KAction *actionDistributeBottom  = new KAction(i18n("Distribute Bottom Borders"), this);
    actionCollection()->addAction("object_distribute_vertical_bottom", actionDistributeBottom );
    connect(actionDistributeBottom, SIGNAL(triggered()), this, SLOT(selectionDistributeVerticalBottom()));

    KAction *actionDistributeTop  = new KAction(i18n("Distribute Top Borders"), this);
    actionCollection()->addAction("object_distribute_vertical_top", actionDistributeTop );
    connect(actionDistributeTop, SIGNAL(triggered()), this, SLOT(selectionDistributeVerticalTop()));

    d->showRulerAction  = new KToggleAction(i18n("Show Rulers"), this);
    actionCollection()->addAction("view_show_ruler", d->showRulerAction );
    d->showRulerAction->setCheckedState(KGuiItem(i18n("Hide Rulers")));
    d->showRulerAction->setToolTip(i18n("Shows or hides rulers"));
    d->showRulerAction->setChecked(false);
    connect( d->showRulerAction, SIGNAL(triggered()), this, SLOT(showRuler()));

    d->showGridAction  = new KToggleAction(KIcon("grid"), i18n("Show Grid"), this);
    actionCollection()->addAction("view_show_grid", d->showGridAction );
    d->showGridAction->setCheckedState(KGuiItem(i18n("Hide Grid")));
    d->showGridAction->setToolTip(i18n("Shows or hides grid"));
    d->showGridAction->setChecked( d->part->gridData().showGrid() );
    connect(d->showGridAction, SIGNAL(triggered()), this, SLOT(showGrid()));

    d->showGuidesAction  = new KToggleAction(KIcon("guides"), i18n("Show Guides"), this);
    actionCollection()->addAction("view_show_guides", d->showGuidesAction );
    d->showGuidesAction->setCheckedState(KGuiItem(i18n("Hide Guides")));
    d->showGuidesAction->setToolTip(i18n("Shows or hides guides"));
    d->showGuidesAction->setChecked( d->part->guidesData().showGuideLines() );
    connect(d->showGuidesAction, SIGNAL(triggered()), this, SLOT(showGuides()));

    d->snapGridAction  = new KToggleAction(i18n("Snap to Grid"), this);
    actionCollection()->addAction("view_snap_to_grid", d->snapGridAction );
    d->snapGridAction->setToolTip(i18n( "Snaps to grid"));
    connect(d->snapGridAction, SIGNAL(triggered()), this, SLOT(snapToGrid()));

    d->groupObjects  = new KAction(KIcon("group"), i18n("&Group Objects"), this);
    actionCollection()->addAction("selection_group", d->groupObjects );
    d->groupObjects->setShortcut(QKeySequence("Ctrl+G"));
    connect(d->groupObjects, SIGNAL(triggered()), this, SLOT(groupSelection()));

    d->ungroupObjects  = new KAction(KIcon("ungroup"), i18n("&Ungroup Objects"), this);
    actionCollection()->addAction("selection_ungroup", d->ungroupObjects );
    d->ungroupObjects->setShortcut(QKeySequence("Ctrl+Shift+G"));
    connect(d->ungroupObjects, SIGNAL(triggered()), this, SLOT(ungroupSelection()));
    // object <-----

    // path ------->
    d->closePath  = new KAction(i18n("&Close Path"), this);
    actionCollection()->addAction("close_path", d->closePath );
    d->closePath->setShortcut(QKeySequence("Ctrl+U"));
    d->closePath->setEnabled( false );
    connect(d->closePath, SIGNAL(triggered()), this, SLOT(closePath()));

    d->combinePath  = new KAction(i18n("Com&bine Path"), this);
    actionCollection()->addAction("combine_path", d->combinePath );
    d->combinePath->setShortcut(QKeySequence("Ctrl+K"));
    d->combinePath->setEnabled( false );
    connect(d->combinePath, SIGNAL(triggered()), this, SLOT(combinePath()));

    d->separatePath  = new KAction(i18n("Se&parate Path"), this);
    actionCollection()->addAction("separate_path", d->separatePath );
    d->separatePath->setShortcut(QKeySequence("Shift+Ctrl+K"));
    d->separatePath->setEnabled( false );
    connect(d->separatePath, SIGNAL(triggered()), this, SLOT(separatePath()));

    d->reversePath  = new KAction(i18n("Re&verse Path"), this);
    actionCollection()->addAction("reverse_path", d->reversePath );
    d->reversePath->setShortcut(QKeySequence("Ctrl+R"));
    d->reversePath->setEnabled( false );
    connect(d->reversePath, SIGNAL(triggered()), this, SLOT(reversePath()));

    d->intersectPath = new KAction(i18n("Intersect Paths"), this);
    actionCollection()->addAction("intersect_path", d->intersectPath );
    //d->intersectPath->setShortcut(QKeySequence("Shift+Ctrl+K"));
    d->intersectPath->setEnabled( false );
    connect(d->intersectPath, SIGNAL(triggered()), this, SLOT(intersectPaths()));

    d->subtractPath = new KAction(i18n("Subtract Paths"), this);
    actionCollection()->addAction("subtract_path", d->subtractPath );
    //d->subtractPath->setShortcut(QKeySequence("Shift+Ctrl+K"));
    d->subtractPath->setEnabled( false );
    connect(d->subtractPath, SIGNAL(triggered()), this, SLOT(subtractPaths()));

    d->unitePath = new KAction(i18n("Unite Paths"), this);
    actionCollection()->addAction("unite_path", d->unitePath );
    //d->unitePath->setShortcut(QKeySequence("Shift+Ctrl+K"));
    d->unitePath->setEnabled( false );
    connect(d->unitePath, SIGNAL(triggered()), this, SLOT(unitePaths()));

    d->excludePath = new KAction(i18n("Exclude Paths"), this);
    actionCollection()->addAction("exclude_path", d->excludePath );
    //d->excludePath->setShortcut(QKeySequence("Shift+Ctrl+K"));
    d->excludePath->setEnabled( false );
    connect(d->excludePath, SIGNAL(triggered()), this, SLOT(excludePaths()));
    // path <-----

    d->configureAction  = new KAction(KIcon("configure"), i18n("Configure Karbon..."), this);
    actionCollection()->addAction("configure", d->configureAction );
    connect(d->configureAction, SIGNAL(triggered()), this, SLOT(configure()));

    KAction *actionPageLayout  = new KAction(i18n("Page &Layout..."), this);
    actionCollection()->addAction("page_layout", actionPageLayout );
    connect(actionPageLayout, SIGNAL(triggered()), this, SLOT(pageLayout()));

    // view ---->
    KAction * zoomSelection = new KAction(KIcon("zoom_selection"), i18n("Zoom to Selection"), this );
    actionCollection()->addAction( "view_zoom_selection", zoomSelection );
    connect(zoomSelection, SIGNAL(triggered()), this, SLOT(zoomSelection()));

    KAction * zoomDrawing = new KAction(KIcon("zoom_drawing"), i18n("Zoom to Drawing"), this );
    actionCollection()->addAction( "view_zoom_drawing", zoomDrawing );
    connect(zoomDrawing, SIGNAL(triggered()), this, SLOT(zoomDrawing()));
    // view <-----
}

void KarbonView::mousePositionChanged( const QPoint &position )
{
    QPoint canvasOffset( d->canvasController->canvasOffsetX(), d->canvasController->canvasOffsetY() );
    QPoint viewPos = position - d->canvas->documentOrigin() - canvasOffset;
    d->horizRuler->updateMouseCoordinate( viewPos.x() );
    d->vertRuler->updateMouseCoordinate( viewPos.y() );

    QPointF documentPos = d->canvas->viewConverter()->viewToDocument( viewPos );
    double x = part()->unit().toUserValue(documentPos.x());
    double y = part()->unit().toUserValue(documentPos.y());

    d->cursorCoords->setText( QString( "%1, %2" ).arg(KGlobal::locale()->formatNumber(x, 2)).arg(KGlobal::locale()->formatNumber(y, 2)) );
}

void KarbonView::reorganizeGUI()
{
    debugView("KarbonView::reorganizeGUI()");

    if( statusBar() )
        statusBar()->setVisible( part()->showStatusBar() );
}

void KarbonView::setNumberOfRecentFiles( unsigned int number )
{
    debugView(QString("KarbonView::setNumberOfRecentFiles(%1)").arg(number));

    if( shell() )    // 0L when embedded into konq !
        shell()->setMaxRecentItems( number );
}

void KarbonView::showRuler()
{
    debugView("KarbonView::showRuler()");

    if( shell() && d->showRulerAction->isChecked() )
    {
        d->horizRuler->show();
        d->vertRuler->show();
        updateRuler();
    }
    else
    {
        d->horizRuler->hide();
        d->vertRuler->hide();
    }
}

void KarbonView::togglePageMargins(bool b)
{
    debugView(QString("KarbonView::togglePageMargins(%1)").arg(b));

    ((KToggleAction*)actionCollection()->action("view_show_margins"))->setChecked(b);
    d->canvas->setShowPageMargins( b );
    d->canvas->update();
}

void KarbonView::pageOffsetChanged()
{
    d->horizRuler->setOffset( d->canvasController->canvasOffsetX() + d->canvas->documentOrigin().x() );
    d->vertRuler->setOffset( d->canvasController->canvasOffsetY() + d->canvas->documentOrigin().y() );
}

void KarbonView::updateRuler()
{
    d->horizRuler->setRulerLength( part()->document().pageSize().width() );
    d->vertRuler->setRulerLength( part()->document().pageSize().height() );
}

void KarbonView::showGrid()
{
    debugView("KarbonView::showGrid()");

    d->part->gridData().setShowGrid( d->showGridAction->isChecked() );
    d->canvas->update();
}

void KarbonView::showGuides()
{
    debugView("KarbonView::showGuides()");

    d->part->guidesData().setShowGuideLines( d->showGuidesAction->isChecked() );
    d->canvas->update();
}

void KarbonView::snapToGrid()
{
    debugView("KarbonView::snapToGrid()");

    d->part->gridData().setSnapToGrid( d->snapGridAction->isChecked() );
    d->canvas->update();
}

void KarbonView::showSelectionPopupMenu( const QPoint &pos )
{
    debugView(QString("KarbonView::showSelectionPopupMenu(QPoint(%1, %2))").arg(pos.x()).arg(pos.y()));

    QList<QAction*> actionList;
    if( d->groupObjects->isEnabled() )
        actionList.append( d->groupObjects );
    else if( d->ungroupObjects->isEnabled() )
        actionList.append( d->ungroupObjects );
    if( d->closePath->isEnabled() )
        actionList.append( d->closePath );
    if( d->combinePath->isEnabled() )
        actionList.append( d->combinePath );
    plugActionList( "selection_type_action", actionList );
    ((Q3PopupMenu *)factory()->container( "selection_popup", this ) )->exec( pos );
    unplugActionList( "selection_type_action" );
}

void KarbonView::configure()
{
    debugView("KarbonView::configure()");

    KarbonConfigureDialog dialog( this );
    dialog.exec();
}

void KarbonView::pageLayout()
{
    debugView("KarbonView::pageLayout()");

    KoPageLayoutDialog dlg( this, part()->pageLayout() );
    dlg.showPageSpread( false );
    dlg.showTextDirection( false );
    dlg.setPageSpread( false );

    if( dlg.exec() == QDialog::Accepted )
    {
        part()->setPageLayout( dlg.pageLayout(), part()->unit() );
        emit pageLayoutChanged();
    }
}

void KarbonView::selectionChanged()
{
    debugView("KarbonView::selectionChanged()");

    KoSelection *selection = d->canvas->shapeManager()->selection();
    int count = selection->selectedShapes( KoFlake::TopLevelSelection ).count();

    d->groupObjects->setEnabled( count > 1 );
    d->ungroupObjects->setEnabled( false );
    d->closePath->setEnabled( false );
    d->combinePath->setEnabled( false );
    d->excludePath->setEnabled( false );
    d->intersectPath->setEnabled( false );
    d->subtractPath->setEnabled( false );
    d->unitePath->setEnabled( false );
    d->deleteSelectionAction->setEnabled( count > 0 );

    kDebug(38000) << count <<" shapes selected";

    if( count > 0 )
    {
        uint selectedPaths = 0;
        uint selectedGroups = 0;
        uint selectedParametrics = 0;
        // check for different shape types for enabling specific actions
        foreach( KoShape* shape, selection->selectedShapes() )
        {
            if( dynamic_cast<KoShapeGroup*>( shape->parent() ) )
                selectedGroups++;
            else if( dynamic_cast<KoPathShape*>( shape ) )
            {
                KoParameterShape * ps = dynamic_cast<KoParameterShape*>( shape );
                if( ps && ps->isParametricShape() )
                    selectedParametrics++;
                else
                    selectedPaths++;
            }
        }
        d->ungroupObjects->setEnabled( selectedGroups > 0 );
        //TODO enable action when the ClosePath command is ported
        //d->closePath->setEnabled( selectedPaths > 0 );
        d->combinePath->setEnabled( selectedPaths > 1 );
        d->separatePath->setEnabled( selectedPaths > 0 );
        d->reversePath->setEnabled( selectedPaths > 0 );
        d->excludePath->setEnabled( selectedPaths + selectedParametrics == 2 );
        d->intersectPath->setEnabled( selectedPaths + selectedParametrics == 2 );
        d->subtractPath->setEnabled( selectedPaths + selectedParametrics == 2 );
        d->unitePath->setEnabled( selectedPaths + selectedParametrics == 2 );
    }

    emit selectionChange();
}

void KarbonView::setCursor( const QCursor &c )
{
    debugView("KarbonView::setCursor(QCursor)");

    d->canvas->setCursor( c );
}

void KarbonView::createLayersTabDock()
{
    debugView("KarbonView::createLayersTabDock()");

    KarbonLayerDockerFactory layerFactory;
    KarbonLayerDocker * layerDocker = qobject_cast<KarbonLayerDocker*>(createDockWidget(&layerFactory));
    layerDocker->setPart( d->part );
    connect( this, SIGNAL( selectionChange() ), layerDocker, SLOT( updateView() ) );
    connect( shell()->partManager(), SIGNAL( activePartChanged( KParts::Part * )),
             layerDocker, SLOT( setPart( KParts::Part * ) ) );
}

void KarbonView::updateReadWrite( bool readwrite )
{
    debugView("KarbonView::updateReadWrite( bool )");
    kDebug(38000) <<"writable state =" << readwrite;
}

void KarbonView::updateUnit( KoUnit unit )
{
    d->horizRuler->setUnit( unit );
    d->vertRuler->setUnit( unit );
    d->canvas->resourceProvider()->setUnitChanged();
}

QList<KoPathShape*> KarbonView::selectedPathShapes()
{
    KoSelection* selection = d->canvas->shapeManager()->selection();
    if( ! selection )
        return QList<KoPathShape*>();

    QList<KoShape*> selectedShapes = selection->selectedShapes();
    QList<KoPathShape*> paths;

    foreach( KoShape* shape, selectedShapes )
    {
        KoPathShape *path = dynamic_cast<KoPathShape*>( shape );
        if( path )
        {
            paths << path;
            selection->deselect( shape );
        }
    }

    return paths;
}

KoPrintJob * KarbonView::createPrintJob()
{
    return new KarbonPrintJob(this);
}

#include "KarbonView.moc"

