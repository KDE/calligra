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
   Copyright (C) 2005-2007 Jan Hambrecht <jaham@gmx.net>
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

#include "karbon_view.h"

// Dialogs.
#include "KarbonConfigureDialog.h"

// Dockers.
#include "vcolordocker.h"
#include "vdocumentdocker.h"
#include "vstyledocker.h"
#include "KarbonTransformDocker.h"
#include "KarbonLayerDocker.h"
#include "KarbonStylePreviewDocker.h"

// The rest.
#include "karbon_factory.h"
#include "karbon_part.h"
#include "vglobal.h"
#include "KarbonCanvas.h"
//#include "karbon_drag.h"

#include <KoMainWindow.h>
#include <KoLineBorder.h>
#include <KoCanvasController.h>
#include <KoCanvasResourceProvider.h>
#include <KoFilterManager.h>
#include <KoContextHelp.h>
#include <KoUnitDoubleSpinBox.h>
// #include <KoPageLayoutDia.h>
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
#include <KoSelection.h>
#include <KoZoomAction.h>
#include <KoZoomHandler.h>
#include <KoZoomController.h>
#include <KoPathShape.h>
#include <KoPathCombineCommand.h>
#include <KoPathSeparateCommand.h>
#include <KoPathReverseCommand.h>
#include <KoToolBoxFactory.h>
#include <KoParameterShape.h>
#include <KoRulerController.h>
#include <KoDockRegistry.h>
#include <KoShapeLayer.h>

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

// qt header
#include <QtGui/QIcon>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>
#include <QtGui/QPixmap>
#include <QtGui/QDropEvent>
#include <QtGui/QGridLayout>
#include <QtGui/QToolBar>
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

KarbonView::KarbonView( KarbonPart* p, QWidget* parent )
        : KoView( p, parent ), KXMLGUIBuilder( shell() ), m_part( p ), m_canvas( 0L )
{
    debugView("KarbonView::KarbonView");

    setComponentData( KarbonFactory::componentData(), true );

    setClientBuilder( this );

    if( !p->isReadWrite() )
        setXMLFile( QString::fromLatin1( "karbon_readonly.rc" ) );
    else
        setXMLFile( QString::fromLatin1( "karbon.rc" ) );

    m_canvas = new KarbonCanvas( p );
    m_canvas->setParent( this );
    m_canvas->setDocumentViewMargin( 250 );
    connect( m_canvas->shapeManager()->selection(), SIGNAL( selectionChanged() ), this, SLOT( selectionChanged() ) );

    m_canvasController = new KoCanvasController(this);
    m_canvasController->setCanvas(m_canvas);
    m_canvasController->setCanvasMode( KoCanvasController::Infinite );
    m_canvasController->show();

    // set up status bar message
    m_status = new QLabel( QString(), statusBar() );
    m_status->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    m_status->setMinimumWidth( 300 );
    addStatusBarItem( m_status, 1 );
    m_cursorCoords = new QLabel( QString(), statusBar() );
    m_cursorCoords->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    m_cursorCoords->setMinimumWidth( 50 );
    addStatusBarItem( m_cursorCoords, 0 );

    // TODO maybe the zoomHandler should be a member of the view and not the canvas.
    m_zoomController = new KoZoomController( m_canvasController, dynamic_cast<KoZoomHandler*>(const_cast<KoViewConverter*>(m_canvas->viewConverter())), actionCollection(), false );
    m_zoomController->setPageSize( m_part->document().pageSize() );
    m_zoomController->setDocumentSize( m_canvas->documentViewRect().size() );
    m_zoomController->setFitMargin( 25 );
    KoZoomAction * zoomAction = m_zoomController->zoomAction();
    zoomAction->setZoomModes( KoZoomMode::ZOOM_WIDTH | KoZoomMode::ZOOM_PAGE );
    addStatusBarItem( zoomAction->createWidget( statusBar() ), 0 );
    connect( m_zoomController, SIGNAL(zoomChanged(KoZoomMode::Mode, double)),
             this, SLOT(zoomChanged(KoZoomMode::Mode, double)));
    m_zoomController->setZoomMode( KoZoomMode::ZOOM_PAGE );

    // layout:
    QGridLayout *layout = new QGridLayout();
    layout->setMargin(0);
    layout->addWidget(m_canvasController, 1, 1);

    initActions();

    m_DocumentTab = 0L;
    m_stylePreview = 0L;
    m_ColorManager = 0L;
    m_styleDocker = 0L;
    m_TransformDocker = 0L;
    m_layerDocker = 0L;

    unsigned int max = part()->maxRecentFiles();
    setNumberOfRecentFiles( max );

    // widgets:
    m_horizRuler = new KoRuler( this, Qt::Horizontal, m_canvas->viewConverter() );
    m_horizRuler->setShowMousePosition(true);
    m_horizRuler->setUnit(p->unit());
    m_horizRuler->setRightToLeft(false);
    new KoRulerController( m_horizRuler, m_canvas->resourceProvider() );

    layout->addWidget( m_horizRuler, 0, 1 );
    connect( p, SIGNAL( unitChanged( KoUnit ) ), this, SLOT( updateUnit( KoUnit ) ) );

    m_vertRuler = new KoRuler( this, Qt::Vertical, m_canvas->viewConverter() );
    m_vertRuler->setShowMousePosition(true);
    m_vertRuler->setUnit(p->unit());
    layout->addWidget( m_vertRuler, 1, 0 );

    connect(m_canvas, SIGNAL(documentOriginChanged( const QPoint &)), this, SLOT(pageOffsetChanged()));
    connect(m_canvas, SIGNAL(documentViewRectChanged(const QRectF &)),
            this, SLOT(documentViewRectChanged(const QRectF &)));
    connect( m_canvasController, SIGNAL(canvasOffsetXChanged(int)), this, SLOT(pageOffsetChanged()));
    connect( m_canvasController, SIGNAL(canvasOffsetYChanged(int)), this, SLOT(pageOffsetChanged()));
    connect( m_canvasController, SIGNAL(canvasMousePositionChanged(const QPoint &)),
            this, SLOT(mousePositionChanged(const QPoint&)));

    connect(m_canvasController, SIGNAL(moveDocumentOffset(const QPoint&)),
            m_canvas, SLOT(setDocumentOffset(const QPoint&)));

    updateRuler();

    if( shell() )
    {
        KoToolManager::instance()->addController( m_canvasController );
        // set the first layer active
        m_canvasController->canvas()->shapeManager()->selection()->setActiveLayer( part()->document().layers().first() );

        //Create Dockers
        createColorDock();
        createTransformDock();
        //createDocumentTabDock();
        createLayersTabDock();
        //createResourceDock();

        KoToolBoxFactory toolBoxFactory(m_canvasController, i18n( "Tools" ) );
        createDockWidget( &toolBoxFactory );

        KarbonStylePreviewDockerFactory styleFactory;
        m_stylePreview = dynamic_cast<KarbonStylePreviewDocker*>( createDockWidget( &styleFactory ) );

        KoToolDockerFactory toolDockerFactory;
        KoToolDocker * toolDocker =  dynamic_cast<KoToolDocker*>( createDockWidget( &toolDockerFactory ) );
        connect(m_canvasController, SIGNAL(toolOptionWidgetChanged(QWidget*)), 
                toolDocker, SLOT(newOptionWidget(QWidget*)));

        KoToolManager::instance()->requestToolActivation( m_canvasController );

        bool b = m_showRulerAction->isChecked();
        m_horizRuler->setVisible( b );
        m_vertRuler->setVisible( b );
    }

    setLayout(layout);

    reorganizeGUI();
}

KarbonView::~KarbonView()
{
    debugView("KarbonView::~KarbonView()");

    // widgets:
    delete m_status;
    delete m_cursorCoords;

    delete m_zoomController;

    KoToolManager::instance()->removeCanvasController( m_canvasController );
}

void
KarbonView::resizeEvent( QResizeEvent* /*event*/ )
{
    debugView("KarbonView::resizeEvent()");

    if(!m_showRulerAction)
        return;

    if(!m_canvas)
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
        KoSelection * selection = m_canvas->shapeManager()->selection();
        if( ! selection )
            return;

        if( ! part() )
            return;

        if( m_stylePreview->strokeIsSelected() )
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
            m_canvas->addCommand( new KoShapeBorderCommand( selectedShapes, borders, 0 ) );
        }
        else
            m_canvas->addCommand( new KoShapeBackgroundCommand( selection->selectedShapes(), QBrush( color ), 0 ) );
    }
/* TODO port to flake
    else if( KarbonDrag::decode( e->mimeData(), selection, m_part->document() ) )
    {
        VObject *clipart = selection.first();
        QPointF p( e->pos() );
        p = m_canvas->viewConverter()->viewToDocument( p ); // TODO: or documentToView ?
        QMatrix mat( 1, 0, 0, 1, p.x(), p.y() );

        VTransformCmd trafo( 0L, mat );
        trafo.visit( *clipart );
        VClipartCmd* cmd = new VClipartCmd( &m_part->document(), i18n( "Insert Clipart" ), clipart );

        m_part->addCommand( cmd, true );
    }
*/
}

void KarbonView::setupPrinter( QPrinter &printer, QPrintDialog &printDialog )
{
    Q_UNUSED( printer );
    Q_UNUSED( printDialog );
}

void KarbonView::print( QPrinter &printer, QPrintDialog &printDialog )
{
    Q_UNUSED( printDialog );
    debugView("KarbonView::print(QPrinter)");

    const int resolution = 600;
    printer.setResolution( resolution );
    printer.setFullPage( true );

    KoZoomHandler zoomHandler;
    zoomHandler.setZoomAndResolution( 100, resolution, resolution );

    QPainter painter;

    painter.begin( &printer );
    m_canvas->shapeManager()->paint( painter, zoomHandler, true );
    painter.end();
}

void
KarbonView::fileImportGraphic()
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
        QString importedFile = man.import( fname, status );
        part()->mergeNativeFormat( importedFile );
        if( !importedFile.isEmpty() )
            unlink( QFile::encodeName( importedFile ) );
    }
    delete dialog;
    part()->repaintAllViews();
}

void
KarbonView::editCut()
{
    debugView("KarbonView::editCut()");

    m_canvas->toolProxy()->cut();
    /*
    addSelectionToClipboard();
    // remove selection
    editDeleteSelection();
    */
}

void
KarbonView::editCopy()
{
    debugView("KarbonView::editCopy()");
    m_canvas->toolProxy()->copy();

    /*
    addSelectionToClipboard();
    */
}

void
KarbonView::addSelectionToClipboard() const
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

void
KarbonView::editPaste()
{
    debugView("KarbonView::editPaste()");

    m_canvas->toolProxy()->paste();
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

    KoSelection* selection = m_canvas->shapeManager()->selection();
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

    KoSelection* selection = m_canvas->shapeManager()->selection();
    if( selection )
        selection->deselectAll();

    selectionChanged();
}

void KarbonView::editDeleteSelection()
{
    debugView("KarbonView::editDeleteSelection()");

    KoSelection* selection = m_canvas->shapeManager()->selection();
    if( ! selection )
        return;

    QList<KoShape*> selectedShapes = selection->selectedShapes();
    if( selectedShapes.count() < 1)
        return;
    selection->deselectAll();

    KoShapeDeleteCommand *cmd = new KoShapeDeleteCommand( part(), selectedShapes );
    m_canvas->addCommand( cmd );
}

void
KarbonView::selectionAlignHorizontalLeft()
{
    debugView("KarbonView::selectionAlignHorizontalLeft()");

    selectionAlign(KoShapeAlignCommand::HorizontalLeftAlignment);
}
void
KarbonView::selectionAlignHorizontalCenter()
{
    debugView("KarbonView::selectionAlignHorizontalCenter()");

    selectionAlign(KoShapeAlignCommand::HorizontalCenterAlignment);
}

void
KarbonView::selectionAlignHorizontalRight()
{
    debugView("KarbonView::selectionAlignHorizontalRight()");

    selectionAlign(KoShapeAlignCommand::HorizontalRightAlignment);
}

void
KarbonView::selectionAlignVerticalTop()
{
    debugView("KarbonView::selectionAlignVerticalTop()");

    selectionAlign(KoShapeAlignCommand::VerticalTopAlignment);
}

void
KarbonView::selectionAlignVerticalCenter()
{
    debugView("KarbonView::selectionAlignVerticalCenter()");

    selectionAlign(KoShapeAlignCommand::VerticalCenterAlignment);
}

void
KarbonView::selectionAlignVerticalBottom()
{
    debugView("KarbonView::selectionAlignVerticalBottom()");

    selectionAlign(KoShapeAlignCommand::VerticalBottomAlignment);
}

void
KarbonView::selectionAlign(KoShapeAlignCommand::Align align)
{
    KoSelection* selection = m_canvas->shapeManager()->selection();
    if( ! selection )
        return;

    QList<KoShape*> selectedShapes = selection->selectedShapes( KoFlake::TopLevelSelection );
    if( selectedShapes.count() < 1)
        return;

    QRectF bRect;
    bRect= (selectedShapes.count() == 1) ? part()->document().boundingRect() : selection->boundingRect();
    KoShapeAlignCommand *cmd = new KoShapeAlignCommand( selectedShapes, align, bRect);

    m_canvas->addCommand( cmd );
}


void
KarbonView::selectionDistributeHorizontalCenter()
{
    debugView("KarbonView::selectionDistributeHorizontalCenter()");

    selectionDistribute(KoShapeDistributeCommand::HorizontalCenterDistribution);
}

void
KarbonView::selectionDistributeHorizontalGap()
{
    debugView("KarbonView::selectionDistributeHorizontalGap()");

    selectionDistribute(KoShapeDistributeCommand::HorizontalGapsDistribution);
}

void
KarbonView::selectionDistributeHorizontalLeft()
{
    debugView("KarbonView::selectionDistributeHorizontalLeft()");

    selectionDistribute(KoShapeDistributeCommand::HorizontalLeftDistribution);
}

void
KarbonView::selectionDistributeHorizontalRight()
{
    debugView("KarbonView::selectionDistributeHorizontalRight()");

    selectionDistribute(KoShapeDistributeCommand::HorizontalRightDistribution);
}

void
KarbonView::selectionDistributeVerticalCenter()
{
    debugView("KarbonView::selectionDistributeVerticalCenter()");

    selectionDistribute(KoShapeDistributeCommand::VerticalCenterDistribution);
}

void
KarbonView::selectionDistributeVerticalGap()
{
    debugView("KarbonView::selectionDistributeVerticalGap()");

    selectionDistribute(KoShapeDistributeCommand::VerticalGapsDistribution);
}

void
KarbonView::selectionDistributeVerticalBottom()
{
    debugView("KarbonView::selectionDistributeVerticalBottom()");

    selectionDistribute(KoShapeDistributeCommand::VerticalBottomDistribution);
}

void
KarbonView::selectionDistributeVerticalTop()
{
    debugView("KarbonView::selectionDistributeVerticalTop()");

    selectionDistribute(KoShapeDistributeCommand::VerticalTopDistribution);
}

void
KarbonView::selectionDistribute(KoShapeDistributeCommand::Distribute distribute)
{
    KoSelection* selection = m_canvas->shapeManager()->selection();
    if( ! selection )
        return;

    QList<KoShape*> selectedShapes = selection->selectedShapes( KoFlake::TopLevelSelection );
    if( selectedShapes.count() < 2) return;

    KoShapeDistributeCommand *cmd = new KoShapeDistributeCommand( selectedShapes, distribute, selection->boundingRect());

    m_canvas->addCommand( cmd );
}

void
KarbonView::selectionDuplicate()
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

void
KarbonView::selectionBringToFront()
{
    debugView("KarbonView::selectionBringToFront()");

    QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes( KoFlake::TopLevelSelection );
    m_canvas->addCommand( KoShapeReorderCommand::createCommand( selectedShapes, m_canvas->shapeManager(), KoShapeReorderCommand::BringToFront ) );
}

void
KarbonView::selectionMoveUp()
{
    debugView("KarbonView::selectionMoveUp()");

    QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes( KoFlake::TopLevelSelection );
    m_canvas->addCommand( KoShapeReorderCommand::createCommand( selectedShapes, m_canvas->shapeManager(), KoShapeReorderCommand::RaiseShape ) );
}

void
KarbonView::selectionMoveDown()
{
    debugView("KarbonView::selectionMoveDown()");

    QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes( KoFlake::TopLevelSelection );
    m_canvas->addCommand( KoShapeReorderCommand::createCommand( selectedShapes, m_canvas->shapeManager(), KoShapeReorderCommand::LowerShape ) );
}

void
KarbonView::selectionSendToBack()
{
    debugView("KarbonView::selectionSendToBack()");

    QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes( KoFlake::TopLevelSelection );
    m_canvas->addCommand( KoShapeReorderCommand::createCommand( selectedShapes, m_canvas->shapeManager(), KoShapeReorderCommand::SendToBack ) );
}

void KarbonView::groupSelection()
{
    debugView("KarbonView::groupSelection()");

    KoSelection* selection = m_canvas->shapeManager()->selection();
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
    new KoShapeCreateCommand( m_part, group, cmd );
    new KoShapeGroupCommand( group, groupedShapes, cmd );
    m_canvas->addCommand( cmd );
}

void
KarbonView::ungroupSelection()
{
    debugView("KarbonView::ungroupSelection()");

    KoSelection* selection = m_canvas->shapeManager()->selection();
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
            new KoShapeDeleteCommand( m_part, container, cmd );
        }
    }
    m_canvas->addCommand( cmd );
}

void
KarbonView::closePath()
{
    debugView("KarbonView::closePath()");

    // TODO add the new close path command here
}

void
KarbonView::combinePath()
{
    debugView("KarbonView::combinePath()");

    KoSelection* selection = m_canvas->shapeManager()->selection();
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
        m_canvas->addCommand( new KoPathCombineCommand( part(), paths ) );
}

void
KarbonView::separatePath()
{
    debugView("KarbonView::separatePath()");
    KoSelection* selection = m_canvas->shapeManager()->selection();
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
        m_canvas->addCommand( new KoPathSeparateCommand( part(), paths ) );
}

void KarbonView::reversePath()
{
    debugView("KarbonView::reversePath()");

    QList<KoPathShape*> paths = selectedPathShapes();
    if( paths.size() )
        m_canvas->addCommand( new KoPathReverseCommand( paths ) );
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

void KarbonView::booleanOperation( KarbonBooleanCommand::BooleanOperation operation )
{
    KoSelection* selection = m_canvas->shapeManager()->selection();
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
        KarbonBooleanCommand * cmd = new KarbonBooleanCommand( part(), paths[0], paths[1], operation );
        new KoShapeDeleteCommand( part(), paths[0], cmd );
        new KoShapeDeleteCommand( part(), paths[1], cmd );
        m_canvas->addCommand( cmd );
    }
}

void
KarbonView::viewModeChanged()
{
    debugView("KarbonView::viewModeChanged()");

    // TODO: port: canvasWidget()->pixmap()->fill();

    /* TODO: port: if( m_viewAction->currentItem() == 1 )
        m_painterFactory->setWireframePainter( canvasWidget()->pixmap(), width(), height() );
    else
        m_painterFactory->setPainter( canvasWidget()->pixmap(), width(), height() );*/

    m_canvas->updateCanvas(m_canvas->canvasWidget()->rect());
}

void KarbonView::zoomChanged( KoZoomMode::Mode mode, double zoom )
{
    static KoZoomMode::Mode lastMode = KoZoomMode::ZOOM_CONSTANT;

    debugView(QString("KarbonView::zoomChanged( mode = %1, zoom = %2) )").arg(mode).arg(zoom));

    QRectF documentViewRect = m_canvas->documentViewRect();
    m_zoomController->setDocumentSize( documentViewRect.size() );
    m_canvas->adjustOrigin();
    if( mode != KoZoomMode::ZOOM_CONSTANT )
    {
        // center the page rect when change the zoom mode to ZOOM_PAGE or ZOOM_WIDTH
        QRectF pageRect( -documentViewRect.topLeft(), m_part->document().pageSize() );
        QPointF center = m_canvas->viewConverter()->documentToView( pageRect.center() );
        m_canvasController->setPreferredCenter( center.toPoint() );
    }
    m_canvas->update();

    lastMode = mode;
}

void KarbonView::documentViewRectChanged( const QRectF &viewRect )
{
    debugView("KarbonView::documentViewRectChanged()");
    m_zoomController->setDocumentSize( viewRect.size() );
    m_canvas->update();
    m_canvasController->ensureVisible( m_canvas->shapeManager()->selection()->boundingRect() );
}

void
KarbonView::initActions()
{
    debugView("KarbonView::initActions()");

    // view ----->
    /*
    m_viewAction  = new KSelectAction(i18n("View &Mode"), this);
    actionCollection()->addAction("view_mode", m_viewAction );
    connect(m_viewAction, SIGNAL(triggered()), this, SLOT(viewModeChanged()));
    */

    m_showPageMargins  = new KToggleAction(KIcon("view_margins"), i18n("Show Page Margins"), this);
    actionCollection()->addAction("view_show_margins", m_showPageMargins );
    connect( m_showPageMargins, SIGNAL(toggled(bool)), SLOT(togglePageMargins(bool)));
    m_showPageMargins->setCheckedState(KGuiItem(i18n("Hide Page Margins")));

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

    m_deleteSelectionAction  = new KAction(KIcon("edit-delete"), i18n("D&elete"), this);
    actionCollection()->addAction("edit_delete", m_deleteSelectionAction );
    m_deleteSelectionAction->setShortcut(QKeySequence("Del"));
    connect(m_deleteSelectionAction, SIGNAL(triggered()), this, SLOT(editDeleteSelection()));
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

    m_showRulerAction  = new KToggleAction(i18n("Show Rulers"), this);
    actionCollection()->addAction("view_show_ruler", m_showRulerAction );
    m_showRulerAction->setCheckedState(KGuiItem(i18n("Hide Rulers")));
    m_showRulerAction->setToolTip(i18n("Shows or hides rulers"));
    m_showRulerAction->setChecked(false);
    connect( m_showRulerAction, SIGNAL(triggered()), this, SLOT(showRuler()));

    m_showGridAction  = new KToggleAction(KIcon("grid"), i18n("Show Grid"), this);
    actionCollection()->addAction("view_show_grid", m_showGridAction );
    m_showGridAction->setCheckedState(KGuiItem(i18n("Hide Grid")));
    m_showGridAction->setToolTip(i18n("Shows or hides grid"));
    //m_showGridAction->setChecked(true);
    connect(m_showGridAction, SIGNAL(triggered()), this, SLOT(showGrid()));

    m_snapGridAction  = new KToggleAction(i18n("Snap to Grid"), this);
    actionCollection()->addAction("view_snap_to_grid", m_snapGridAction );
    m_snapGridAction->setToolTip(i18n( "Snaps to grid"));
    //m_snapGridAction->setChecked(true);
    connect(m_snapGridAction, SIGNAL(triggered()), this, SLOT(snapToGrid()));

    m_groupObjects  = new KAction(KIcon("group"), i18n("&Group Objects"), this);
    actionCollection()->addAction("selection_group", m_groupObjects );
    m_groupObjects->setShortcut(QKeySequence("Ctrl+G"));
    connect(m_groupObjects, SIGNAL(triggered()), this, SLOT(groupSelection()));

    m_ungroupObjects  = new KAction(KIcon("ungroup"), i18n("&Ungroup Objects"), this);
    actionCollection()->addAction("selection_ungroup", m_ungroupObjects );
    m_ungroupObjects->setShortcut(QKeySequence("Ctrl+Shift+G"));
    connect(m_ungroupObjects, SIGNAL(triggered()), this, SLOT(ungroupSelection()));
    // object <-----

    // path ------->
    m_closePath  = new KAction(i18n("&Close Path"), this);
    actionCollection()->addAction("close_path", m_closePath );
    m_closePath->setShortcut(QKeySequence("Ctrl+U"));
    m_closePath->setEnabled( false );
    connect(m_closePath, SIGNAL(triggered()), this, SLOT(closePath()));

    m_combinePath  = new KAction(i18n("Com&bine Path"), this);
    actionCollection()->addAction("combine_path", m_combinePath );
    m_combinePath->setShortcut(QKeySequence("Ctrl+K"));
    m_combinePath->setEnabled( false );
    connect(m_combinePath, SIGNAL(triggered()), this, SLOT(combinePath()));

    m_separatePath  = new KAction(i18n("Se&parate Path"), this);
    actionCollection()->addAction("separate_path", m_separatePath );
    m_separatePath->setShortcut(QKeySequence("Shift+Ctrl+K"));
    m_separatePath->setEnabled( false );
    connect(m_separatePath, SIGNAL(triggered()), this, SLOT(separatePath()));

    m_reversePath  = new KAction(i18n("Re&verse Path"), this);
    actionCollection()->addAction("reverse_path", m_reversePath );
    m_reversePath->setShortcut(QKeySequence("Ctrl+R"));
    m_reversePath->setEnabled( false );
    connect(m_reversePath, SIGNAL(triggered()), this, SLOT(reversePath()));

    m_intersectPath = new KAction(i18n("Intersect Paths"), this);
    actionCollection()->addAction("intersect_path", m_intersectPath );
    //m_intersectPath->setShortcut(QKeySequence("Shift+Ctrl+K"));
    m_intersectPath->setEnabled( false );
    connect(m_intersectPath, SIGNAL(triggered()), this, SLOT(intersectPaths()));

    m_subtractPath = new KAction(i18n("Subtract Paths"), this);
    actionCollection()->addAction("subtract_path", m_subtractPath );
    //m_subtractPath->setShortcut(QKeySequence("Shift+Ctrl+K"));
    m_subtractPath->setEnabled( false );
    connect(m_subtractPath, SIGNAL(triggered()), this, SLOT(subtractPaths()));

    m_unitePath = new KAction(i18n("Unite Paths"), this);
    actionCollection()->addAction("unite_path", m_unitePath );
    //m_unitePath->setShortcut(QKeySequence("Shift+Ctrl+K"));
    m_unitePath->setEnabled( false );
    connect(m_unitePath, SIGNAL(triggered()), this, SLOT(unitePaths()));
    // path <-----

    m_configureAction  = new KAction(KIcon("configure"), i18n("Configure Karbon..."), this);
    actionCollection()->addAction("configure", m_configureAction );
    connect(m_configureAction, SIGNAL(triggered()), this, SLOT(configure()));

    KAction *actionPageLayout  = new KAction(i18n("Page &Layout..."), this);
    actionCollection()->addAction("page_layout", actionPageLayout );
    connect(actionPageLayout, SIGNAL(triggered()), this, SLOT(pageLayout()));

    m_contextHelpAction = new KoContextHelpAction( actionCollection(), this );
}

void
KarbonView::mousePositionChanged( const QPoint &position )
{
    QPoint canvasOffset( m_canvasController->canvasOffsetX(), m_canvasController->canvasOffsetY() );
    QPoint viewPos = position - m_canvas->documentOrigin() - canvasOffset;
    m_horizRuler->updateMouseCoordinate( viewPos.x() );
    m_vertRuler->updateMouseCoordinate( viewPos.y() );

    QPointF documentPos = m_canvas->viewConverter()->viewToDocument( viewPos );
    double x = part()->unit().toUserValue(documentPos.x());
    double y = part()->unit().toUserValue(documentPos.y());

    m_cursorCoords->setText( QString( "%1, %2" ).arg(KGlobal::locale()->formatNumber(x, 2)).arg(KGlobal::locale()->formatNumber(y, 2)) );
}

void
KarbonView::reorganizeGUI()
{
    debugView("KarbonView::reorganizeGUI()");

    if( statusBar() )
        statusBar()->setVisible( part()->showStatusBar() );
}

void
KarbonView::setNumberOfRecentFiles( unsigned int number )
{
    debugView(QString("KarbonView::setNumberOfRecentFiles(%1)").arg(number));

    if( shell() )    // 0L when embedded into konq !
        shell()->setMaxRecentItems( number );
}

void
KarbonView::showRuler()
{
    debugView("KarbonView::showRuler()");

    if( shell() && m_showRulerAction->isChecked() )
    {
        m_horizRuler->show();
        m_vertRuler->show();
        updateRuler();
    }
    else
    {
        m_horizRuler->hide();
        m_vertRuler->hide();
    }
}

void
KarbonView::togglePageMargins(bool b)
{
    debugView(QString("KarbonView::togglePageMargins(%1)").arg(b));

    ((KToggleAction*)actionCollection()->action("view_show_margins"))->setChecked(b);
    m_canvas->setShowPageMargins( b );
    m_canvas->update();
}

void
KarbonView::pageOffsetChanged()
{
    m_horizRuler->setOffset( m_canvasController->canvasOffsetX() + m_canvas->documentOrigin().x() );
    m_vertRuler->setOffset( m_canvasController->canvasOffsetY() + m_canvas->documentOrigin().y() );
}

void
KarbonView::updateRuler()
{
    m_horizRuler->setRulerLength( part()->document().pageSize().width() );
    m_vertRuler->setRulerLength( part()->document().pageSize().height() );
}

void
KarbonView::showGrid()
{
    debugView("KarbonView::showGrid()");

    m_part->gridData().setShowGrid( m_showGridAction->isChecked() );
    m_canvas->update();
}

void
KarbonView::snapToGrid()
{
    debugView("KarbonView::snapToGrid()");

    m_part->gridData().setSnapToGrid( m_snapGridAction->isChecked() );
    m_canvas->update();
}

void
KarbonView::showSelectionPopupMenu( const QPoint &pos )
{
    debugView(QString("KarbonView::showSelectionPopupMenu(QPoint(%1, %2))").arg(pos.x()).arg(pos.y()));

    QList<QAction*> actionList;
    if( m_groupObjects->isEnabled() )
        actionList.append( m_groupObjects );
    else if( m_ungroupObjects->isEnabled() )
        actionList.append( m_ungroupObjects );
    if( m_closePath->isEnabled() )
        actionList.append( m_closePath );
    if( m_combinePath->isEnabled() )
        actionList.append( m_combinePath );
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

void
KarbonView::pageLayout()
{
    debugView("KarbonView::pageLayout()");

// TODO show a simple page layout dialog without all the whistels and bells of the kword one.
#if 0
    KoHeadFoot hf;
    KoPageLayout layout = part()->pageLayout();
    KoUnit unit = part()->unit();
    if( KoPageLayoutDia::pageLayout( layout, hf, FORMAT_AND_BORDERS | DISABLE_UNIT, unit ) )
    {
        part()->setPageLayout( layout, unit );
        m_horizRuler->setUnit( unit );
        m_vertRuler->setUnit( unit );
        m_canvas->canvasWidget()->resize( int( ( part()->pageLayout().width + 300 ) * zoom() ),
                                  int( ( part()->pageLayout().height + 460 ) * zoom() ) );
        m_canvas->adjustOrigin();
        part()->repaintAllViews();

        emit pageLayoutChanged();
    }
#endif
}

void
KarbonView::selectionChanged()
{
    debugView("KarbonView::selectionChanged()");

    KoSelection *selection = m_canvas->shapeManager()->selection();
    int count = selection->selectedShapes( KoFlake::TopLevelSelection ).count();

    m_groupObjects->setEnabled( count > 1 );
    m_ungroupObjects->setEnabled( false );
    m_closePath->setEnabled( false );
    m_combinePath->setEnabled( false );
    m_intersectPath->setEnabled( false );
    m_subtractPath->setEnabled( false );
    m_unitePath->setEnabled( false );
    m_deleteSelectionAction->setEnabled( count > 0 );

    kDebug(38000) << count <<" shapes selected";

    if( count > 0 )
    {
        KoShape *shape = *selection->selectedShapes().begin();
        if( shape )
        {
            if ( shell() ) {
                //if ( this == shell()->rootView() || koDocument()->isEmbedded() ) {
                    m_stylePreview->updateStyle( shape->border(), shape->background() );
                //}
            }
        }
        else
        {
            if ( shell() ) {
                //if ( this == shell()->rootView() || koDocument()->isEmbedded() ) {
                    m_stylePreview->updateStyle( 0, QBrush( Qt::NoBrush ) );
                //}
            }
        }

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
        m_ungroupObjects->setEnabled( selectedGroups > 0 );
        //TODO enable action when the ClosePath command is ported
        //m_closePath->setEnabled( selectedPaths > 0 );
        m_combinePath->setEnabled( selectedPaths > 1 );
        m_separatePath->setEnabled( selectedPaths > 0 );
        m_reversePath->setEnabled( selectedPaths > 0 );
        m_intersectPath->setEnabled( selectedPaths == 2 );
        m_subtractPath->setEnabled( selectedPaths == 2 );
        m_unitePath->setEnabled( selectedPaths == 2 );
    }
    else
    {
        if ( shell() )
            //if ( this == shell()->rootView() || koDocument()->isEmbedded() && m_stylePreview )
            m_stylePreview->updateStyle( 0, QBrush( Qt::NoBrush ) );
    }
    emit selectionChange();
}
void
KarbonView::setCursor( const QCursor &c )
{
    debugView("KarbonView::setCursor(QCursor)");

    m_canvas->setCursor( c );
}

void KarbonView::createDocumentTabDock()
{
    debugView("KarbonView::createDocumentTabDock()");

    /*
    m_DocumentTab = new VDocumentTab(this, this);
    m_DocumentTab->setWindowTitle(i18n("Document"));
    createDock(i18n("Document"), m_DocumentTab);
    connect( m_part, SIGNAL( unitChanged( KoUnit ) ), m_DocumentTab, SLOT( updateDocumentInfo() ) );
    */
}

void KarbonView::createLayersTabDock()
{
    debugView("KarbonView::createLayersTabDock()");

    KarbonLayerDockerFactory layerFactory( m_part, &part()->document() );
    m_layerDocker = qobject_cast<KarbonLayerDocker*>(createDockWidget(&layerFactory));
    connect( this, SIGNAL( selectionChange() ), m_layerDocker, SLOT( updateView() ) );
}

void KarbonView::createColorDock()
{
    debugView("KarbonView::createColorDock()");

    VColorDockerFactory colorFactory;
    m_ColorManager = qobject_cast<VColorDocker*>(createDockWidget(&colorFactory));

    connect( this, SIGNAL( selectionChange() ), m_ColorManager, SLOT( update() ) );
}

void KarbonView::createTransformDock()
{
    debugView("KarbonView::createTransformDock()");

    KarbonTransformDockerFactory transformFactory;
    m_TransformDocker = qobject_cast<KarbonTransformDocker*>(createDockWidget(&transformFactory));
    connect( this, SIGNAL( selectionChange() ), m_TransformDocker, SLOT( update() ) );
}

void KarbonView::createResourceDock()
{
    debugView("KarbonView::createResourceDock()");

    /*
    m_styleDocker = new VStyleDocker( part(), this );
    m_styleDocker->setWindowTitle(i18n("Resources"));
    createDock(i18n("Resources"), m_styleDocker);
    */
}

void KarbonView::updateReadWrite( bool readwrite )
{
    debugView("KarbonView::updateReadWrite( bool )");
    kDebug(38000) <<"writable state =" << readwrite;
}

void KarbonView::updateUnit( KoUnit unit )
{
    m_horizRuler->setUnit( unit );
    m_vertRuler->setUnit( unit );
    m_TransformDocker->setUnit( unit );
    m_canvas->resourceProvider()->setUnitChanged();
}

QList<KoPathShape*> KarbonView::selectedPathShapes()
{
    KoSelection* selection = m_canvas->shapeManager()->selection();
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

#include "karbon_view.moc"

