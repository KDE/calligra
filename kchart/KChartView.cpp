/* This file is part of the KDE project

   Copyright 1999-2007  Kalle Dalheimer <kalle@kde.org>
   Copyright 2005-2009  Inge Wallin <inge@lysator.liu.se>

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


// Own
#include "KChartView.h"

// Qt
#include <QFile>
#include <qpainter.h>
#include <qcursor.h>
#include <QMenu>
#include <QMouseEvent>
#include <QPixmap>
#include <QPaintEvent>
#include <QGridLayout>
#include <QtGui/QPrinter>

// KDE
#include <kicon.h>
#include <kaction.h>
#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <ktemporaryfile.h>
#include <kcomponentdata.h>
#include <kxmlguifactory.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <ktoggleaction.h>
#include <kactioncollection.h>

// KOffice
#include <KoCsvImportDialog.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoShapeCreateCommand.h>
#include <KoTemplateCreateDia.h>
#include <KoToolBoxFactory.h>
#include <KoToolManager.h>
#include <KoViewAdaptor.h>
#include <KoZoomAction.h>
#include <KoZoomHandler.h>
#include <KoDockerManager.h>
#include <KoCanvasController.h>

// KChart
#include "KChartFactory.h"
#include "KChartPart.h"
#include "KChartViewAdaptor.h"
#include "KCPageLayout.h"
#include "KCPrinterDialog.h"
#include "KChartPrintJob.h"
#include "ChartShape.h"
#include "KChartCanvas.h"

#include "prefs.h"

#include "commands/ChartTypeCommand.h"
#include <KoMainWindow.h>


using namespace std;


namespace KChart
{


KChartView::KChartView( KChartPart* part, QWidget* parent )
    : KoView( part, parent )
{
    setComponentData( KChartFactory::global() );
    if ( koDocument()->isReadWrite() )
        setXMLFile( "kchart.rc" );
    else
        setXMLFile( "kchart_readonly.rc" );

    m_dbus = new ViewAdaptor(this);

    m_importData  = new KAction(i18n("Import Data..."), this);
    actionCollection()->addAction("import_data", m_importData );
    connect(m_importData, SIGNAL(triggered(bool)), SLOT( importData() ));
    KAction *actionExtraCreateTemplate  = new KAction(i18n("&Create Template From Document..."), this);
    actionCollection()->addAction("extra_template", actionExtraCreateTemplate );
    connect(actionExtraCreateTemplate, SIGNAL(triggered(bool)), SLOT( extraCreateTemplate() ));

    m_edit  = new KAction(KIcon("document-properties"), i18n("Edit &Data..."), this);
    actionCollection()->addAction("editdata", m_edit );
    connect(m_edit, SIGNAL(triggered(bool) ), SLOT( editData() ));

    // The KChartCanvas class requires an instanciated KoZoomHandler,
    // so we'll do that first.
    m_zoomHandler = new KoZoomHandler;

    m_canvas = new KChartCanvas( this, part );

    connect( m_canvas, SIGNAL( documentViewRectChanged( const QRectF& ) ),
             this,     SLOT( documentViewRectChanged( const QRectF& ) ) );

    m_canvasController = new KoCanvasController( this );
    m_canvasController->setCanvas( m_canvas );
    m_canvasController->setCanvasMode( KoCanvasController::Centered );

    m_zoomController = new KoZoomController( m_canvasController, m_zoomHandler, actionCollection() );

    connect( m_canvasController, SIGNAL( moveDocumentOffset( const QPoint& ) ),
             m_canvas, SLOT( setDocumentOffset( const QPoint& ) ) );

    QGridLayout *layout = new QGridLayout;
    layout->setMargin( 0 );
    layout->addWidget( m_canvasController, 0, 0 );

    setLayout( layout );

    KoZoomAction *zoomAction = m_zoomController->zoomAction();

    // Initially, zoom is at 100%
    m_zoomController->setZoom( KoZoomMode::ZOOM_CONSTANT, 100 );
    m_zoomHandler->setZoom( 100.0 );

    //kDebug() << "----------------------------------------------------------------";
    //kDebug() << "Size is: " << m_zoomHandler->viewToDocument( size() );
    //kDebug() << "size() is: " << size();
    //part->shape()->setSize( m_zoomHandler->viewToDocument( size() ) );

    // This is probably a good default size for a chart.
    part->chart()->setSize( QSizeF( CM_TO_POINT( 12 ), CM_TO_POINT( 8 ) ) );
    //m_zoomController->setZoomMode( KoZoomMode::ZOOM_PAGE );

    m_zoomController->setPageSize( m_canvas->documentViewRect().size() );
    m_zoomController->setDocumentSize( m_canvas->documentViewRect().size() );

    addStatusBarItem( zoomAction->createWidget( ( QWidget* )statusBar() ), 0 );
    connect( m_zoomController, SIGNAL(zoomChanged(KoZoomMode::Mode, qreal)),
             this, SLOT(zoomChanged(KoZoomMode::Mode, qreal)));
    m_zoomController->setZoomMode( KoZoomMode::ZOOM_PAGE );

    // initialize the configuration
    //    loadConfig();

    //KChartPart *part = (KChartPart*)koDocument();

    KoToolManager::instance()->addController( m_canvasController );
    KoToolManager::instance()->registerTools( actionCollection(), m_canvasController );

    if (shell())
    {
        connect( m_canvasController, SIGNAL( toolOptionWidgetsChanged(const QMap<QString, QWidget *> &) ),
             shell()->dockerManager(), SLOT( newOptionWidgets(const  QMap<QString, QWidget *> &) ) );

        KoToolBoxFactory toolBoxFactory( m_canvasController, i18n("Tools") );
        shell()->createDockWidget( &toolBoxFactory );
    }

    // Disable some things if we can't change the data, e.g. because
    // we are inside another application that provides the data for us.
    if ( !part->canChangeValue() ) {
        m_edit->setEnabled( false );
        m_importData->setEnabled( false );
    }

    m_canvas->shapeManager()->addShape( part->chart() );
    connect( m_canvas->shapeManager()->selection(), SIGNAL( selectionChanged() ), this, SLOT( selectionChanged() ) );

    m_canvasController->show();
}


KChartView::~KChartView()
{
    delete m_dbus;

    // FIXME: More deletions here?  There are lots of new's in the constructor.
}


ViewAdaptor* KChartView::dbusObject()
{
    return m_dbus;
}


void KChartView::paintEvent( QPaintEvent* /*ev*/ )
{
    //QPainter painter( this );
    //QRect clipRect =

    // ### TODO: Scaling

    // Let the document do the drawing
    // This calls KChartPart::paintContent, basically.
    //koDocument()->paintEverything( painter, rect(), this );
    //m_canvas->shapeManager()->paint( painter, KoZoomHandler(), true );
}


void KChartView::updateReadWrite( bool /*readwrite*/ )
{
#ifdef __GNUC__
    #warning TODO: implement updateReadWrite
#endif
}



void KChartView::slotRepaint()
{
    ((KChartPart*)koDocument())->setModified(true);
    update();
}


void KChartView::saveConfig()
{
    kDebug(35001) <<"Save config...";
    //((KChartPart*)koDocument())->saveConfig( KGlobal::config().data() );
}


void KChartView::loadConfig()
{
    kDebug(35001) <<"Load config...";

    //KGlobal::config()->reparseConfiguration();
    //((KChartPart*)koDocument())->loadConfig( KGlobal::config().data() );

    //refresh chart when you load config
    update();
}


void KChartView::defaultConfig()
{
    //((KChartPart*)koDocument())->defaultConfig(  );
    update();
}


void KChartView::mousePressEvent ( QMouseEvent *e )
{
    if (!koDocument()->isReadWrite() || !factory())
        return;
    if ( e->button() == Qt::RightButton )
        ((QMenu*)factory()->container("action_popup",this))->popup(QCursor::pos());
}


void KChartView::slotConfigPageLayout()
{
    KChartPart    *part = qobject_cast<KChartPart*>( koDocument() );
    KCPageLayout  *dialog = new KCPageLayout(part, this);

    connect( dialog, SIGNAL( dataChanged() ),
             this,   SLOT( slotRepaint() ) );

    dialog->exec();
    delete dialog;
}

KoPrintJob * KChartView::createPrintJob()
{
    return new KChartPrintJob(this);
}

#if 0                           // Disable printing for now.
void KChartView::setupPrinter( QPrinter &printer, QPrintDialog &printDialog )
{
/*  In Qt dialogs get added to QPrintDialog, when prinitn gets ativated again will need to fix
  if ( !printer.previewOnly() )
    printer.addDialogPage( new KCPrinterDialog( 0, "KChart page" ) );
*/
}


void KChartView::print(QPrinter &printer, QPrintDialog &printDialog)
{
    Q_UNUSED( printer );
    printer.setFullPage( false );

    QPainter painter;
    painter.begin(&printer);

    int  height;
    int  width;
    if ( !printer.previewOnly() ) {
        int const scalex = printer.option("kde-kchart-printsizex").toInt();
        int const scaley = printer.option("kde-kchart-printsizey").toInt();

        width  = printer.width()  * scalex / 100;
        height = printer.height() * scaley / 100;
    }
    else {
        // Fill the whole page.
        width  = printer.width();
        height = printer.height();
    }

    QRect  rect(0, 0, width, height);
    KDChart::print(&painter,
                   ((KChartPart*)koDocument())->params(),
                   ((KChartPart*)koDocument())->data(),
                   0, 		// regions
                   &rect);
    painter.end();
}
#endif


// Import data from a Comma Separated Values file.
//

void KChartView::importData()
{
    // Get the name of the file to open.
    QString filename = KFileDialog::getOpenFileName(KUrl(QString()),// startDir
                                                    QString(),// filter
                                                    0,
                                                    i18n("Import Data"));
    kDebug(35001) <<"Filename = <" << filename <<">";
    if (filename.isEmpty())
      return;

    // Check to see if we can read the file.
    QFile  inFile(filename);
    if (!inFile.open(QIODevice::ReadOnly)) {
        KMessageBox::sorry( 0, i18n("The file %1 could not be read.",
                                    filename) );
        inFile.close();
        return;
    }

    // Let the CSV dialog structure the data in the file.
    QByteArray  inData( inFile.readAll() );
    inFile.close();
    KoCsvImportDialog *dialog = new KoCsvImportDialog(0L);
    dialog->setData(inData);

    if ( !dialog->exec() ) {
        // kDebug(35001) <<"Cancel was pressed";
        return;
    }

    //kDebug(35001) <<"OK was pressed";

#if 0
    uint  rows = dialog->rows();
    uint  cols = dialog->cols();

    //kDebug(35001) <<"Rows:" << rows <<"  Cols:" << cols;

    bool  hasRowHeaders = ( rows > 1 && dialog->firstRowContainHeaders() );
    bool  hasColHeaders = ( cols > 1 && dialog->firstColContainHeaders() );

    KDChartTableData  data( rows, cols );
    data.setUsedRows( rows );
    data.setUsedCols( cols );
    for (uint row = 0; row < rows; row++) {
        for (uint col = 0; col < cols; col++) {
            bool     ok;
            QString  tmp;
            qreal   val;

            // Get the text and convert to qreal unless in the headers.
            tmp = dialog->text( row, col );
            if ( ( row == 0 && hasRowHeaders )
                 || ( col == 0 && hasColHeaders ) ) {
                kDebug(35001) <<"Setting header (" << row <<"," << col
                               << ") to value " << tmp << endl;
                data.setCell( row, col, tmp );
            }
            else {
                val = tmp.toDouble(&ok);
                if (!ok)
                    val = 0.0;

                kDebug(35001) <<"Setting (" << row <<"," << col
                               << ") to value " << val << endl;

                // and do the actual setting.
                data.setCell( row, col, val );
            }
        }
    }

    ((KChartPart*)koDocument())->doSetData( data,
                                            hasRowHeaders, hasColHeaders );
#else
//js 2008-03-19
//?? warning C4930: 'QStandardItemModel data(void)': prototyped function not called (was a variable definition intended?)
//??    QStandardItemModel  data();
#endif
}


void KChartView::extraCreateTemplate()
{
    int width = 60;
    int height = 60;
    QPixmap pix = koDocument()->generatePreview(QSize(width, height));

    KTemporaryFile tempFile;
    tempFile.setSuffix(".chrt");
    tempFile.open();

    koDocument()->saveNativeFormat( tempFile.fileName() );

    KoTemplateCreateDia::createTemplate( "kchart_template", KChartFactory::global(),
                                         tempFile.fileName(), pix, this );

    KChartFactory::global().dirs()->addResourceType( "kchart_template", "data", "kchart/templates/" );
}

void KChartView::selectionChanged()
{
    KoSelection *selection = m_canvas->shapeManager()->selection();
    Q_UNUSED(selection);
    emit selectionChange();
}

void KChartView::documentViewRectChanged( const QRectF &viewRect )
{
    QSizeF size = viewRect.size();
    m_zoomController->setDocumentSize( size );
    m_zoomController->setPageSize( size );
    m_canvas->update();
    m_canvasController->ensureVisible( m_canvas->shapeManager()->selection()->boundingRect() );
}

void KChartView::zoomChanged( KoZoomMode::Mode mode, qreal zoom )
{
    Q_UNUSED(mode);
    Q_UNUSED(zoom);

    QRectF documentViewRect = m_canvas->documentViewRect();
    m_zoomController->setDocumentSize( documentViewRect.size() );
    m_canvasController->setPreferredCenter( m_zoomHandler->documentToView( documentViewRect ).center().toPoint() );
}

}  //KChart namespace

#include "KChartView.moc"
