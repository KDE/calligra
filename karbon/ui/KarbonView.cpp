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
   Copyright (C) 2005-2009 Jan Hambrecht <jaham@gmx.net>
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
#include "KarbonLayerDocker.h"

// The rest.
#include "Karbon.h"
#include "KarbonFactory.h"
#include "KarbonPart.h"
#include "KarbonCanvas.h"
#include "KarbonPrintJob.h"
#include "KarbonZoomController.h"
#include "KarbonSmallStylePreview.h"
#include "KarbonDocumentMergeCommand.h"

#include <KoMainWindow.h>
#include <KoLineBorder.h>
#include <KoCanvasController.h>
#include <KoResourceManager.h>
#include <KoFilterManager.h>
#include <KoUnitDoubleSpinBox.h>
#include <KoPageLayoutDialog.h>
#include <KoRuler.h>
#include <KoToolManager.h>
#include <KoStandardAction.h>
#include <KoToolProxy.h>
#include <KoShapeManager.h>
#include <KoShapeController.h>
#include <KoShapeContainer.h>
#include <KoShapeGroup.h>
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
#include <KoPathPointData.h>
#include <KoPathCombineCommand.h>
#include <KoPathSeparateCommand.h>
#include <KoPathReverseCommand.h>
#include <KoPathPointMoveCommand.h>
#include <KoToolBoxFactory.h>
#include <KoParameterShape.h>
#include <KoRulerController.h>
#include <KoDockRegistry.h>
#include <KoDockerManager.h>
#include <KoShapeLayer.h>
#include <KoColorBackground.h>
#include <KoCutController.h>
#include <KoCopyController.h>
#include <KoPasteController.h>
#include <KoSnapGuide.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>
#include <KoImageCollection.h>
#include <KoImageData.h>

// kde header
#include <kaction.h>
#include <kcolormimedata.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kcomponentdata.h>
#include <kactioncollection.h>
#include <kxmlguifactory.h>
#include <kicon.h>
#include <kstatusbar.h>
#include <kfiledialog.h>
#include <kstandardaction.h>
#include <kparts/partmanager.h>
#include <KToggleAction>
#include <KDebug>

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
#include <QtGui/QPrinter>
#include <QtGui/QImageReader>

#include <unistd.h>

class KarbonView::Private
{
public:
    Private(KarbonPart * p)
            : part(p), canvas(0), canvasController(0), horizRuler(0), vertRuler(0)
            , closePath(0), combinePath(0)
            , separatePath(0), reversePath(0), intersectPath(0), subtractPath(0)
            , unitePath(0), excludePath(0), pathSnapToGrid(0), configureAction(0)
            , deleteSelectionAction(0), viewAction(0), showRulerAction(0)
            , snapGridAction(0), showPageMargins(0), showGuidesAction(0)
            , status(0), cursorCoords(0), smallPreview(0), zoomActionWidget(0)
    {}

    KarbonPart * part;
    KarbonCanvas * canvas;
    KoCanvasController * canvasController;
    KoRuler * horizRuler;
    KoRuler * vertRuler;

    // actions:
    KAction * closePath;
    KAction * combinePath;
    KAction * separatePath;
    KAction * reversePath;
    KAction * intersectPath;
    KAction * subtractPath;
    KAction * unitePath;
    KAction * excludePath;
    KAction * pathSnapToGrid;
    KAction * configureAction;
    KAction * deleteSelectionAction;

    KToggleAction * viewAction;
    KToggleAction * showRulerAction;
    KToggleAction * snapGridAction;
    KToggleAction * showPageMargins;
    KToggleAction * showGuidesAction;

    //Status Bar
    QLabel * status;       ///< ordinary status
    QLabel * cursorCoords; ///< cursor coordinates
    KarbonSmallStylePreview * smallPreview; ///< small style preview
    QWidget * zoomActionWidget; ///< zoom action widget
};

KarbonView::KarbonView(KarbonPart* p, QWidget* parent)
        : KoView(p, parent), d(new Private(p))
{
    setComponentData(KarbonFactory::componentData(), true);

    if (!p->isReadWrite())
        setXMLFile(QString::fromLatin1("karbon_readonly.rc"));
    else
        setXMLFile(QString::fromLatin1("karbon.rc"));

    const int viewMargin = 250;
    d->canvas = new KarbonCanvas(p);
    d->canvas->setParent(this);
    d->canvas->setDocumentViewMargin(viewMargin);
    connect(d->canvas->shapeManager()->selection(), SIGNAL(selectionChanged()),
            this, SLOT(selectionChanged()));

    d->canvasController = new KoCanvasController(this);
    d->canvasController->setMinimumSize(QSize(viewMargin + 50, viewMargin + 50));
    d->canvasController->setCanvas(d->canvas);
    d->canvasController->setCanvasMode(KoCanvasController::Infinite);
    // always show srollbars which fixes some nasty infinite
    // recursion when scrollbars are disabled during resizing
    d->canvasController->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    d->canvasController->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    d->canvasController->show();

    // set up status bar message
    d->status = new QLabel(QString(), this);
    d->status->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    d->status->setMinimumWidth(300);
    addStatusBarItem(d->status, 1);
    connect(KoToolManager::instance(), SIGNAL(changedStatusText(const QString &)),
            d->status, SLOT(setText(const QString &)));
    d->cursorCoords = new QLabel(QString(), this);
    d->cursorCoords->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    d->cursorCoords->setMinimumWidth(50);
    addStatusBarItem(d->cursorCoords, 0);

    // TODO maybe the zoomHandler should be a member of the view and not the canvas.
    // set up the zoom controller
    KarbonZoomController * zoomController = new KarbonZoomController(d->canvasController, actionCollection());
    zoomController->setPageSize(d->part->document().pageSize());
    d->zoomActionWidget = zoomController->zoomAction()->createWidget(statusBar());
    addStatusBarItem(d->zoomActionWidget, 0);
    zoomController->setZoomMode(KoZoomMode::ZOOM_PAGE);
    connect(zoomController, SIGNAL(zoomedToSelection()), this, SLOT(zoomSelection()));
    connect(zoomController, SIGNAL(zoomedToAll()), this, SLOT(zoomDrawing()));

    d->smallPreview = new KarbonSmallStylePreview(this);
    connect(d->smallPreview, SIGNAL(fillApplied()), this, SLOT(applyFillToSelection()));
    connect(d->smallPreview, SIGNAL(strokeApplied()), this, SLOT(applyStrokeToSelection()));
    addStatusBarItem(d->smallPreview, 0);

    // layout:
    QGridLayout *layout = new QGridLayout();
    layout->setMargin(0);
    layout->addWidget(d->canvasController, 1, 1);

    KoToolManager::instance()->addController(d->canvasController);
    KoToolManager::instance()->registerTools(actionCollection(), d->canvasController);

    initActions();

    unsigned int max = part()->maxRecentFiles();
    setNumberOfRecentFiles(max);

    // widgets:
    d->horizRuler = new KoRuler(this, Qt::Horizontal, d->canvas->viewConverter());
    d->horizRuler->setShowMousePosition(true);
    d->horizRuler->setUnit(p->unit());
    d->horizRuler->setRightToLeft(false);
    d->horizRuler->setVisible(false);
    new KoRulerController(d->horizRuler, d->canvas->resourceManager());

    layout->addWidget(d->horizRuler, 0, 1);
    connect(p, SIGNAL(unitChanged(const KoUnit&)), this, SLOT(updateUnit(const KoUnit&)));

    d->vertRuler = new KoRuler(this, Qt::Vertical, d->canvas->viewConverter());
    d->vertRuler->setShowMousePosition(true);
    d->vertRuler->setUnit(p->unit());
    d->vertRuler->setVisible(false);
    layout->addWidget(d->vertRuler, 1, 0);

    connect(d->canvas, SIGNAL(documentOriginChanged(const QPoint &)), this, SLOT(pageOffsetChanged()));
    connect(d->canvasController, SIGNAL(canvasOffsetXChanged(int)), this, SLOT(pageOffsetChanged()));
    connect(d->canvasController, SIGNAL(canvasOffsetYChanged(int)), this, SLOT(pageOffsetChanged()));
    connect(d->canvasController, SIGNAL(canvasMousePositionChanged(const QPoint &)),
            this, SLOT(mousePositionChanged(const QPoint&)));
    d->vertRuler->createGuideToolConnection(d->canvas);
    d->horizRuler->createGuideToolConnection(d->canvas);

    updateRuler();

    if (shell()) {
        // set the first layer active
        d->canvasController->canvas()->shapeManager()->selection()->setActiveLayer(part()->document().layers().first());

        //Create Dockers
        createLayersTabDock();

        KoToolBoxFactory toolBoxFactory(d->canvasController, i18n("Tools"));
        shell()->createDockWidget(&toolBoxFactory);

        connect(d->canvasController, SIGNAL(toolOptionWidgetsChanged(const QMap<QString, QWidget *> &, QWidget*)),
                shell()->dockerManager(), SLOT(newOptionWidgets(const  QMap<QString, QWidget *> &, QWidget*)));

        KoToolManager::instance()->requestToolActivation(d->canvasController);

        bool b = d->showRulerAction->isChecked();
        d->horizRuler->setVisible(b);
        d->vertRuler->setVisible(b);
    }

    setLayout(layout);

    reorganizeGUI();

    setFocusPolicy(Qt::NoFocus);
}

KarbonView::~KarbonView()
{
    KoToolManager::instance()->removeCanvasController(d->canvasController);

    removeStatusBarItem(d->status);
    removeStatusBarItem(d->cursorCoords);
    removeStatusBarItem(d->smallPreview);
    removeStatusBarItem(d->zoomActionWidget);

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

KoZoomController * KarbonView::zoomController() const
{
    return 0;
}

void KarbonView::resizeEvent(QResizeEvent* /*event*/)
{
    if (!d->showRulerAction)
        return;

    if (!d->canvas)
        return;

    reorganizeGUI();
}

void KarbonView::dropEvent(QDropEvent *e)
{
    //Accepts QColor - from Color Manager's KColorPatch
    QColor color = KColorMimeData::fromMimeData(e->mimeData());
    if (color.isValid()) {
        KoSelection * selection = d->canvas->shapeManager()->selection();
        if (! selection)
            return;

        if (! part())
            return;

        if (d->canvas->resourceManager()->intResource(KoCanvasResource::ActiveStyleType) == KoFlake::Foreground) {
            QList<KoShapeBorderModel*> borders;
            QList<KoShape*> selectedShapes = selection->selectedShapes();
            foreach(KoShape * shape, selectedShapes) {
                KoLineBorder * border = dynamic_cast<KoLineBorder*>(shape->border());
                KoLineBorder * newBorder = 0;
                if (border) {
                    newBorder = new KoLineBorder(*border);
                    newBorder->setColor(color);
                } else {
                    newBorder = new KoLineBorder(1.0, color);
                }
                borders.append(newBorder);
            }
            d->canvas->addCommand(new KoShapeBorderCommand(selectedShapes, borders, 0));
        } else {
            KoColorBackground * fill = new KoColorBackground(color);
            d->canvas->addCommand(new KoShapeBackgroundCommand(selection->selectedShapes(), fill, 0));
        }
    }
}

void KarbonView::fileImportGraphic()
{
    QByteArray nativeMimeType = part()->nativeFormatMimeType();
    QStringList filter = KoFilterManager::mimeFilter(nativeMimeType, KoFilterManager::Import);

    QStringList imageFilter;
    // add filters for all formats supported by QImage
    foreach(const QByteArray &format, QImageReader::supportedImageFormats()) {
        imageFilter << "image/" + format;
    }
    filter.append(imageFilter);

    QPointer<KFileDialog> dialog = new KFileDialog(KUrl(), "", 0);
    dialog->setCaption(i18n("Choose Graphic to Add"));
    dialog->setModal(true);
    dialog->setMimeFilter(filter);
    if (dialog->exec() != QDialog::Accepted) {
        delete dialog;
        return;
    }
    QString fname = dialog ? dialog->selectedFile() : QString();
    QString currentMimeFilter = dialog ? dialog->currentMimeFilter() : QString();
    delete dialog;

    QMap<QString, KoDataCenterBase*> dataCenters = part()->document().dataCenterMap();

    KarbonPart importPart;
    // use data centers of this document for importing
    importPart.document().useExternalDataCenterMap(dataCenters);

    bool success = true;

    // check if we have an empty mime type (probably because the "All supported files"
    // filter was active)
    if (currentMimeFilter.isEmpty()) {
        // get mime type from file
        KMimeType::Ptr mimeType = KMimeType::findByFileContent(fname);
        if (mimeType) {
            if (mimeType->is(nativeMimeType)) {
                currentMimeFilter = nativeMimeType;
            } else {
                foreach(const QString &filter, imageFilter) {
                    if (mimeType->is(filter)) {
                        currentMimeFilter = filter;
                        break;
                    }
                }
            }
        }
    }

    // check if we are loading an image format
    if (imageFilter.contains(currentMimeFilter)) {
        QImage image;
        if (!image.load(fname)) {
            KMessageBox::error(0, i18n("Could not load image."), i18n("Import graphic"), 0);
            return;
        }
        KoShapeFactoryBase * factory = KoShapeRegistry::instance()->get("PictureShape");
        if (!factory) {
            KMessageBox::error(0, i18n("Could not create image shape."), i18n("Import graphic"), 0);
            return;
        }

        KoShape *picture = factory->createDefaultShape(part()->document().resourceManager());
        KoImageCollection *imageCollection = part()->document().resourceManager()->imageCollection();
        if (!picture || !imageCollection) {
            KMessageBox::error(0, i18n("Could not create image shape."), i18n("Import graphic"), 0);
            return;
        }

        // calculate shape size in point from image resolution
        qreal pxWidth = static_cast<qreal>(image.width());
        qreal pxHeight = static_cast<qreal>(image.height());
        qreal width = DM_TO_POINT(pxWidth / static_cast<qreal>(image.dotsPerMeterX()) * 10.0);
        qreal height = DM_TO_POINT(pxHeight / static_cast<qreal>(image.dotsPerMeterY()) * 10.0);

        // set shape data
        picture->setUserData(imageCollection->createImageData(image));
        picture->setSize(QSizeF(width, height));
        picture->setPosition(QPointF());
        picture->setKeepAspectRatio(true);

        QUndoCommand * cmd = d->canvas->shapeController()->addShapeDirect(picture);
        cmd->setText(i18n("Insert graphics"));
        d->canvas->addCommand(cmd);
        d->canvas->shapeManager()->selection()->select(picture);
        return;
    }

    // check if we are loading our native format
    if (nativeMimeType == currentMimeFilter) {
        // directly load the native format
        success = importPart.loadNativeFormat(fname);
        if (!success) {
            importPart.showLoadingErrorDialog();
        }
    } else {
        // use import filters to load the file
        KoFilterManager man(&importPart);
        KoFilter::ConversionStatus status = KoFilter::OK;
        QString importedFile = man.importDocument(fname, status);
        if (status != KoFilter::OK) {
            importPart.showLoadingErrorDialog();
            success = false;
        } else if (!importedFile.isEmpty()) {
            success = importPart.loadNativeFormat(importedFile);
            if (!success) {
                importPart.showLoadingErrorDialog();
            }
            // remove the temporary file created during format conversion
            unlink(QFile::encodeName(importedFile));
        }
    }

    if (success) {
        QList<KoShape*> importedShapes = importPart.document().shapes();

        KarbonDocumentMergeCommand * cmd = new KarbonDocumentMergeCommand(part(), &importPart);
        d->canvas->addCommand(cmd);

        foreach(KoShape * shape, importedShapes) {
            d->canvas->shapeManager()->selection()->select(shape, false);
        }
    }
}

void KarbonView::selectionDuplicate()
{
    d->canvas->toolProxy()->copy();
    d->canvas->toolProxy()->paste();
}

void KarbonView::editSelectAll()
{
    KoSelection* selection = d->canvas->shapeManager()->selection();
    if (! selection)
        return;

    QList<KoShape*> shapes = part()->document().shapes();
    kDebug(38000) << "shapes.size() =" << shapes.size();

    foreach(KoShape* shape, shapes) {
        selection->select(shape);
        shape->update();
    }

    selectionChanged();
}

void KarbonView::editDeselectAll()
{
    KoSelection* selection = d->canvas->shapeManager()->selection();
    if (selection)
        selection->deselectAll();

    selectionChanged();
}

void KarbonView::editDeleteSelection()
{
    d->canvas->toolProxy()->deleteSelection();
}

void KarbonView::selectionDistributeHorizontalCenter()
{
    selectionDistribute(KoShapeDistributeCommand::HorizontalCenterDistribution);
}

void KarbonView::selectionDistributeHorizontalGap()
{
    selectionDistribute(KoShapeDistributeCommand::HorizontalGapsDistribution);
}

void KarbonView::selectionDistributeHorizontalLeft()
{
    selectionDistribute(KoShapeDistributeCommand::HorizontalLeftDistribution);
}

void KarbonView::selectionDistributeHorizontalRight()
{
    selectionDistribute(KoShapeDistributeCommand::HorizontalRightDistribution);
}

void KarbonView::selectionDistributeVerticalCenter()
{
    selectionDistribute(KoShapeDistributeCommand::VerticalCenterDistribution);
}

void KarbonView::selectionDistributeVerticalGap()
{
    selectionDistribute(KoShapeDistributeCommand::VerticalGapsDistribution);
}

void KarbonView::selectionDistributeVerticalBottom()
{
    selectionDistribute(KoShapeDistributeCommand::VerticalBottomDistribution);
}

void KarbonView::selectionDistributeVerticalTop()
{
    selectionDistribute(KoShapeDistributeCommand::VerticalTopDistribution);
}

void KarbonView::selectionDistribute(KoShapeDistributeCommand::Distribute distribute)
{
    KoSelection* selection = d->canvas->shapeManager()->selection();
    if (! selection)
        return;

    QList<KoShape*> selectedShapes = selection->selectedShapes(KoFlake::TopLevelSelection);
    if (selectedShapes.count() < 2) return;

    KoShapeDistributeCommand *cmd = new KoShapeDistributeCommand(selectedShapes, distribute, selection->boundingRect());

    d->canvas->addCommand(cmd);
}

void KarbonView::closePath()
{
    // TODO add the new close path command here
}

void KarbonView::combinePath()
{
    KoSelection* selection = d->canvas->shapeManager()->selection();
    if (! selection)
        return;

    QList<KoShape*> selectedShapes = selection->selectedShapes();
    QList<KoPathShape*> paths;

    foreach(KoShape* shape, selectedShapes) {
        KoPathShape *path = dynamic_cast<KoPathShape*>(shape);
        if (path) {
            KoParameterShape * paramShape = dynamic_cast<KoParameterShape*>(path);
            if (paramShape && paramShape->isParametricShape())
                continue;
            paths << path;
            selection->deselect(shape);
        }
    }

    if (paths.size())
        d->canvas->addCommand(new KoPathCombineCommand(part(), paths));
}

void KarbonView::separatePath()
{
    KoSelection* selection = d->canvas->shapeManager()->selection();
    if (! selection)
        return;

    QList<KoShape*> selectedShapes = selection->selectedShapes();
    QList<KoPathShape*> paths;

    foreach(KoShape* shape, selectedShapes) {
        KoPathShape *path = dynamic_cast<KoPathShape*>(shape);
        if (path) {
            paths << path;
            selection->deselect(shape);
        }
    }

    if (paths.size())
        d->canvas->addCommand(new KoPathSeparateCommand(part(), paths));
}

void KarbonView::reversePath()
{
    QList<KoPathShape*> paths = selectedPathShapes();
    if (paths.size())
        d->canvas->addCommand(new KoPathReverseCommand(paths));
}

void KarbonView::intersectPaths()
{
    booleanOperation(KarbonBooleanCommand::Intersection);
}

void KarbonView::subtractPaths()
{
    booleanOperation(KarbonBooleanCommand::Subtraction);
}

void KarbonView::unitePaths()
{
    booleanOperation(KarbonBooleanCommand::Union);
}

void KarbonView::excludePaths()
{
    booleanOperation(KarbonBooleanCommand::Exclusion);
}

void KarbonView::booleanOperation(KarbonBooleanCommand::BooleanOperation operation)
{
    KoSelection* selection = d->canvas->shapeManager()->selection();
    if (! selection)
        return;

    QList<KoShape*> selectedShapes = selection->selectedShapes();
    QList<KoPathShape*> paths;

    foreach(KoShape* shape, selectedShapes) {
        KoPathShape *path = dynamic_cast<KoPathShape*>(shape);
        if (path) {
            paths << path;
            selection->deselect(shape);
        }
    }

    if (paths.size() == 2) {
        QUndoCommand * macro = new QUndoCommand(i18n("Boolean Operation"));
        KoParameterShape * paramShape = dynamic_cast<KoParameterShape*>(paths[0]);
        if (paramShape && paramShape->isParametricShape())
            new KoParameterToPathCommand(paramShape, macro);
        paramShape = dynamic_cast<KoParameterShape*>(paths[1]);
        if (paramShape && paramShape->isParametricShape())
            new KoParameterToPathCommand(paramShape, macro);
        new KarbonBooleanCommand(part(), paths[0], paths[1], operation, macro);
        new KoShapeDeleteCommand(part(), paths[0], macro);
        new KoShapeDeleteCommand(part(), paths[1], macro);
        d->canvas->addCommand(macro);
    }
}

void KarbonView::pathSnapToGrid()
{
    KoSelection* selection = d->canvas->shapeManager()->selection();
    if (! selection)
        return;

    QList<KoShape*> selectedShapes = selection->selectedShapes();
    QList<KoPathPointData> points;
    QList<QPointF> offsets;

    // store current grid snap state
    bool oldSnapToGrid = part()->gridData().snapToGrid();
    // enable grid snapping
    part()->gridData().setSnapToGrid(true);

    KoSnapGuide snapGuide(d->canvas);
    snapGuide.enableSnapStrategies(KoSnapGuide::GridSnapping);
    snapGuide.setSnapDistance(INT_MAX);

    foreach(KoShape* shape, selectedShapes) {
        KoParameterShape * paramShape = dynamic_cast<KoParameterShape*>(shape);
        if (paramShape && paramShape->isParametricShape())
            continue;

        KoPathShape *path = dynamic_cast<KoPathShape*>(shape);
        if (! path)
            continue;

        uint subpathCount = path->subpathCount();
        for (uint i = 0; i < subpathCount; ++i) {
            uint pointCount = path->subpathPointCount(i);
            for (uint j = 0; j < pointCount; ++j) {
                KoPathPointIndex index(i, j);
                KoPathPoint * p = path->pointByIndex(index);
                if (!p)
                    continue;

                QPointF docPoint = path->shapeToDocument(p->point());
                QPointF offset = snapGuide.snap(docPoint, 0) - docPoint;
                points.append(KoPathPointData(path, index));
                offsets.append(offset);
            }
        }
    }

    // reset grid snapping state to old value
    part()->gridData().setSnapToGrid(oldSnapToGrid);

    d->canvas->addCommand(new KoPathPointMoveCommand(points, offsets));
}

void KarbonView::viewModeChanged(bool outlineMode)
{
    d->canvas->enableOutlineMode(outlineMode);
    d->canvas->updateCanvas(d->canvas->canvasWidget()->rect());
}

void KarbonView::zoomSelection()
{
    KoSelection* selection = d->canvas->shapeManager()->selection();
    if (! selection)
        return;

    if (! selection->count())
        return;

    const KoZoomHandler * zoomHandler = dynamic_cast<const KoZoomHandler*>(d->canvas->viewConverter());
    if (! zoomHandler)
        return;

    QRectF bbox = selection->boundingRect();
    QRect viewRect = zoomHandler->documentToView(bbox).toRect();

    d->canvasController->zoomTo(viewRect.translated(d->canvas->documentOrigin()));
    QPointF newCenter = d->canvas->documentOrigin() + zoomHandler->documentToView(bbox.center());
    d->canvasController->setPreferredCenter(newCenter.toPoint());
}

void KarbonView::zoomDrawing()
{
    const KoZoomHandler * zoomHandler = dynamic_cast<const KoZoomHandler*>(d->canvas->viewConverter());
    if (! zoomHandler)
        return;

    QRectF bbox = d->part->document().contentRect();
    if (bbox.isNull())
        return;

    QRect viewRect = zoomHandler->documentToView(bbox).toRect();
    d->canvasController->zoomTo(viewRect.translated(d->canvas->documentOrigin()));
    QPointF newCenter = d->canvas->documentOrigin() + zoomHandler->documentToView(bbox.center());
    d->canvasController->setPreferredCenter(newCenter.toPoint());
}

void KarbonView::initActions()
{
    // view ----->
    d->viewAction  = new KToggleAction(i18n("Outline &Mode"), this);
    actionCollection()->addAction("view_mode", d->viewAction);
    connect(d->viewAction, SIGNAL(toggled(bool)), this, SLOT(viewModeChanged(bool)));

    d->showPageMargins  = new KToggleAction(KIcon("view_margins"), i18n("Show Page Margins"), this);
    actionCollection()->addAction("view_show_margins", d->showPageMargins);
    connect(d->showPageMargins, SIGNAL(toggled(bool)), SLOT(togglePageMargins(bool)));
    d->showPageMargins->setCheckedState(KGuiItem(i18n("Hide Page Margins")));

    // No need for the other actions in read-only (embedded) mode
    if (!shell())
        return;

    // edit ----->
    QAction *action = actionCollection()->addAction(KStandardAction::Cut,  "edit_cut", 0, 0);
    new KoCutController(d->canvas, action);
    action = actionCollection()->addAction(KStandardAction::Copy,  "edit_copy", 0, 0);
    new KoCopyController(d->canvas, action);
    action = actionCollection()->addAction(KStandardAction::Paste,  "edit_paste", 0, 0);
    new KoPasteController(d->canvas, action);
    actionCollection()->addAction(KStandardAction::SelectAll,  "edit_select_all", this, SLOT(editSelectAll()));
    actionCollection()->addAction(KStandardAction::Deselect,  "edit_deselect_all", this, SLOT(editDeselectAll()));

    KAction *actionImportGraphic  = new KAction(i18n("&Import Graphic..."), this);
    actionCollection()->addAction("file_import", actionImportGraphic);
    connect(actionImportGraphic, SIGNAL(triggered()), this, SLOT(fileImportGraphic()));

    d->deleteSelectionAction  = new KAction(KIcon("edit-delete"), i18n("D&elete"), this);
    actionCollection()->addAction("edit_delete", d->deleteSelectionAction);
    d->deleteSelectionAction->setShortcut(QKeySequence("Del"));
    connect(d->deleteSelectionAction, SIGNAL(triggered()), this, SLOT(editDeleteSelection()));
    connect(d->canvas->toolProxy(), SIGNAL(selectionChanged(bool)), d->deleteSelectionAction, SLOT(setEnabled(bool)));
    // edit <-----

    // object ----->
    KAction *actionDuplicate  = new KAction(KIcon("duplicate"), i18nc("Duplicate selection", "&Duplicate"), this);
    actionCollection()->addAction("object_duplicate", actionDuplicate);
    actionDuplicate->setShortcut(QKeySequence("Ctrl+D"));
    connect(actionDuplicate, SIGNAL(triggered()), this, SLOT(selectionDuplicate()));

    KAction *actionDistributeHorizontalCenter  = new KAction(i18n("Distribute Center (Horizontal)"), this);
    actionCollection()->addAction("object_distribute_horizontal_center", actionDistributeHorizontalCenter);
    connect(actionDistributeHorizontalCenter, SIGNAL(triggered()), this, SLOT(selectionDistributeHorizontalCenter()));

    KAction *actionDistributeHorizontalGap  = new KAction(i18n("Distribute Gaps (Horizontal)"), this);
    actionCollection()->addAction("object_distribute_horizontal_gap", actionDistributeHorizontalGap);
    connect(actionDistributeHorizontalGap, SIGNAL(triggered()), this, SLOT(selectionDistributeHorizontalGap()));

    KAction *actionDistributeLeft  = new KAction(i18n("Distribute Left Borders"), this);
    actionCollection()->addAction("object_distribute_horizontal_left", actionDistributeLeft);
    connect(actionDistributeLeft, SIGNAL(triggered()), this, SLOT(selectionDistributeHorizontalLeft()));

    KAction *actionDistributeRight  = new KAction(i18n("Distribute Right Borders"), this);
    actionCollection()->addAction("object_distribute_horizontal_right", actionDistributeRight);
    connect(actionDistributeRight, SIGNAL(triggered()), this, SLOT(selectionDistributeHorizontalRight()));

    KAction *actionDistributeVerticalCenter  = new KAction(i18n("Distribute Center (Vertical)"), this);
    actionCollection()->addAction("object_distribute_vertical_center", actionDistributeVerticalCenter);
    connect(actionDistributeVerticalCenter, SIGNAL(triggered()), this, SLOT(selectionDistributeVerticalCenter()));

    KAction *actionDistributeVerticalGap  = new KAction(i18n("Distribute Gaps (Vertical)"), this);
    actionCollection()->addAction("object_distribute_vertical_gap", actionDistributeVerticalGap);
    connect(actionDistributeVerticalGap, SIGNAL(triggered()), this, SLOT(selectionDistributeVerticalGap()));

    KAction *actionDistributeBottom  = new KAction(i18n("Distribute Bottom Borders"), this);
    actionCollection()->addAction("object_distribute_vertical_bottom", actionDistributeBottom);
    connect(actionDistributeBottom, SIGNAL(triggered()), this, SLOT(selectionDistributeVerticalBottom()));

    KAction *actionDistributeTop  = new KAction(i18n("Distribute Top Borders"), this);
    actionCollection()->addAction("object_distribute_vertical_top", actionDistributeTop);
    connect(actionDistributeTop, SIGNAL(triggered()), this, SLOT(selectionDistributeVerticalTop()));

    d->showRulerAction  = new KToggleAction(i18n("Show Rulers"), this);
    actionCollection()->addAction("view_show_ruler", d->showRulerAction);
    d->showRulerAction->setCheckedState(KGuiItem(i18n("Hide Rulers")));
    d->showRulerAction->setToolTip(i18n("Shows or hides rulers"));
    d->showRulerAction->setChecked(false);
    connect(d->showRulerAction, SIGNAL(triggered()), this, SLOT(showRuler()));

    KToggleAction *gridAction = d->part->gridData().gridToggleAction(d->canvas);
    actionCollection()->addAction("view_grid", gridAction);

    d->showGuidesAction = KoStandardAction::showGuides(this, SLOT(showGuides()), this);
    actionCollection()->addAction(KoStandardAction::name(KoStandardAction::ShowGuides), d->showGuidesAction);
    d->showGuidesAction->setChecked(d->part->guidesData().showGuideLines());

    d->snapGridAction  = new KToggleAction(i18n("Snap to Grid"), this);
    actionCollection()->addAction("view_snap_to_grid", d->snapGridAction);
    d->snapGridAction->setToolTip(i18n("Snaps to grid"));
    connect(d->snapGridAction, SIGNAL(triggered()), this, SLOT(snapToGrid()));

    action = actionCollection()->action("object_group");
    if (action) {
        action->setShortcut(QKeySequence("Ctrl+G"));
    }
    action = actionCollection()->action("object_ungroup");
    if (action) {
        action->setShortcut(QKeySequence("Ctrl+Shift+G"));
    }

    // object <-----

    // path ------->
    d->closePath  = new KAction(i18n("&Close Path"), this);
    actionCollection()->addAction("close_path", d->closePath);
    d->closePath->setShortcut(QKeySequence("Ctrl+U"));
    d->closePath->setEnabled(false);
    connect(d->closePath, SIGNAL(triggered()), this, SLOT(closePath()));

    d->combinePath  = new KAction(i18n("Com&bine Path"), this);
    actionCollection()->addAction("combine_path", d->combinePath);
    d->combinePath->setShortcut(QKeySequence("Ctrl+K"));
    d->combinePath->setEnabled(false);
    connect(d->combinePath, SIGNAL(triggered()), this, SLOT(combinePath()));

    d->separatePath  = new KAction(i18n("Se&parate Path"), this);
    actionCollection()->addAction("separate_path", d->separatePath);
    d->separatePath->setShortcut(QKeySequence("Shift+Ctrl+K"));
    d->separatePath->setEnabled(false);
    connect(d->separatePath, SIGNAL(triggered()), this, SLOT(separatePath()));

    d->reversePath  = new KAction(i18n("Re&verse Path"), this);
    actionCollection()->addAction("reverse_path", d->reversePath);
    d->reversePath->setShortcut(QKeySequence("Ctrl+R"));
    d->reversePath->setEnabled(false);
    connect(d->reversePath, SIGNAL(triggered()), this, SLOT(reversePath()));

    d->intersectPath = new KAction(i18n("Intersect Paths"), this);
    actionCollection()->addAction("intersect_path", d->intersectPath);
    //d->intersectPath->setShortcut(QKeySequence("Shift+Ctrl+K"));
    d->intersectPath->setEnabled(false);
    connect(d->intersectPath, SIGNAL(triggered()), this, SLOT(intersectPaths()));

    d->subtractPath = new KAction(i18n("Subtract Paths"), this);
    actionCollection()->addAction("subtract_path", d->subtractPath);
    //d->subtractPath->setShortcut(QKeySequence("Shift+Ctrl+K"));
    d->subtractPath->setEnabled(false);
    connect(d->subtractPath, SIGNAL(triggered()), this, SLOT(subtractPaths()));

    d->unitePath = new KAction(i18n("Unite Paths"), this);
    actionCollection()->addAction("unite_path", d->unitePath);
    //d->unitePath->setShortcut(QKeySequence("Shift+Ctrl+K"));
    d->unitePath->setEnabled(false);
    connect(d->unitePath, SIGNAL(triggered()), this, SLOT(unitePaths()));

    d->excludePath = new KAction(i18n("Exclude Paths"), this);
    actionCollection()->addAction("exclude_path", d->excludePath);
    //d->excludePath->setShortcut(QKeySequence("Shift+Ctrl+K"));
    d->excludePath->setEnabled(false);
    connect(d->excludePath, SIGNAL(triggered()), this, SLOT(excludePaths()));

    d->pathSnapToGrid = new KAction(i18n("Snap Path to Grid"), this);
    actionCollection()->addAction("path_snap_to_grid", d->pathSnapToGrid);
    d->pathSnapToGrid->setEnabled(false);
    connect(d->pathSnapToGrid, SIGNAL(triggered()), this, SLOT(pathSnapToGrid()));

    // path <-----

    d->configureAction  = new KAction(KIcon("configure"), i18n("Configure Karbon..."), this);
    actionCollection()->addAction("configure", d->configureAction);
    connect(d->configureAction, SIGNAL(triggered()), this, SLOT(configure()));

    KAction *actionPageLayout  = new KAction(i18n("Page &Layout..."), this);
    actionCollection()->addAction("page_layout", actionPageLayout);
    connect(actionPageLayout, SIGNAL(triggered()), this, SLOT(pageLayout()));

    // view ---->
    KAction * zoomSelection = new KAction(KIcon("zoom_selection"), i18n("Zoom to Selection"), this);
    actionCollection()->addAction("view_zoom_selection", zoomSelection);
    connect(zoomSelection, SIGNAL(triggered()), this, SLOT(zoomSelection()));

    KAction * zoomDrawing = new KAction(KIcon("zoom_drawing"), i18n("Zoom to Drawing"), this);
    actionCollection()->addAction("view_zoom_drawing", zoomDrawing);
    connect(zoomDrawing, SIGNAL(triggered()), this, SLOT(zoomDrawing()));
    // view <-----
}

void KarbonView::mousePositionChanged(const QPoint &position)
{
    QPoint canvasOffset(d->canvasController->canvasOffsetX(), d->canvasController->canvasOffsetY());
    QPoint viewPos = position - d->canvas->documentOrigin() - canvasOffset;
    if (d->horizRuler->isVisible())
        d->horizRuler->updateMouseCoordinate(viewPos.x());
    if (d->vertRuler->isVisible())
        d->vertRuler->updateMouseCoordinate(viewPos.y());

    QPointF documentPos = d->canvas->viewConverter()->viewToDocument(viewPos);
    qreal x = part()->unit().toUserValue(documentPos.x());
    qreal y = part()->unit().toUserValue(documentPos.y());

    if (statusBar() && statusBar()->isVisible())
        d->cursorCoords->setText(QString("%1, %2").arg(KGlobal::locale()->formatNumber(x, 2)).arg(KGlobal::locale()->formatNumber(y, 2)));
}

void KarbonView::reorganizeGUI()
{
    if (d->snapGridAction)
        d->snapGridAction->setChecked(part()->gridData().snapToGrid());
    if (statusBar())
        statusBar()->setVisible(part()->showStatusBar());
}

void KarbonView::setNumberOfRecentFiles(unsigned int number)
{
    if (shell())     // 0L when embedded into konq !
        shell()->setMaxRecentItems(number);
}

void KarbonView::showRuler()
{
    if (shell() && d->showRulerAction->isChecked()) {
        d->horizRuler->show();
        d->vertRuler->show();
        updateRuler();
    } else {
        d->horizRuler->hide();
        d->vertRuler->hide();
    }
}

void KarbonView::togglePageMargins(bool b)
{
    ((KToggleAction*)actionCollection()->action("view_show_margins"))->setChecked(b);
    d->canvas->setShowPageMargins(b);
    d->canvas->update();
}

void KarbonView::pageOffsetChanged()
{
    d->horizRuler->setOffset(d->canvasController->canvasOffsetX() + d->canvas->documentOrigin().x());
    d->vertRuler->setOffset(d->canvasController->canvasOffsetY() + d->canvas->documentOrigin().y());
}

void KarbonView::updateRuler()
{
    d->horizRuler->setRulerLength(part()->document().pageSize().width());
    d->vertRuler->setRulerLength(part()->document().pageSize().height());
}

void KarbonView::showGuides()
{
    d->part->guidesData().setShowGuideLines(d->showGuidesAction->isChecked());
    d->canvas->update();
}

void KarbonView::snapToGrid()
{
    d->part->gridData().setSnapToGrid(d->snapGridAction->isChecked());
    d->canvas->update();
}

void KarbonView::configure()
{
    QPointer<KarbonConfigureDialog> dialog = new KarbonConfigureDialog(this);
    dialog->exec();
    delete dialog;
    d->part->reorganizeGUI();
    d->canvas->update();
}

void KarbonView::pageLayout()
{
    QPointer<KoPageLayoutDialog> dlg = new KoPageLayoutDialog(this, part()->pageLayout());
    dlg->showPageSpread(false);
    dlg->showTextDirection(false);
    dlg->setPageSpread(false);

    if (dlg->exec() == QDialog::Accepted) {
        if (dlg) {
            part()->setPageLayout(dlg->pageLayout());
        }
    }
    delete dlg;
}

void KarbonView::selectionChanged()
{
    if (!shell())
        return;
    KoSelection *selection = d->canvas->shapeManager()->selection();
    int count = selection->selectedShapes(KoFlake::FullSelection).count();

    d->closePath->setEnabled(false);
    d->combinePath->setEnabled(false);
    d->excludePath->setEnabled(false);
    d->intersectPath->setEnabled(false);
    d->subtractPath->setEnabled(false);
    d->unitePath->setEnabled(false);
    d->pathSnapToGrid->setEnabled(false);

    kDebug(38000) << count << " shapes selected";

    if (count > 0) {
        uint selectedPaths = 0;
        uint selectedParametrics = 0;
        // check for different shape types for enabling specific actions
        foreach(KoShape* shape, selection->selectedShapes(KoFlake::FullSelection)) {
            if (dynamic_cast<KoPathShape*>(shape)) {
                KoParameterShape * ps = dynamic_cast<KoParameterShape*>(shape);
                if (ps && ps->isParametricShape())
                    selectedParametrics++;
                else
                    selectedPaths++;
            }
        }
        kDebug(38000) << selectedPaths << " path shapes selected";
        kDebug(38000) << selectedParametrics << " parameter shapes selected";
        //TODO enable action when the ClosePath command is ported
        //d->closePath->setEnabled( selectedPaths > 0 );
        d->combinePath->setEnabled(selectedPaths > 1);
        d->separatePath->setEnabled(selectedPaths > 0);
        d->reversePath->setEnabled(selectedPaths > 0);
        d->excludePath->setEnabled(selectedPaths + selectedParametrics == 2);
        d->intersectPath->setEnabled(selectedPaths + selectedParametrics == 2);
        d->subtractPath->setEnabled(selectedPaths + selectedParametrics == 2);
        d->unitePath->setEnabled(selectedPaths + selectedParametrics == 2);
        d->pathSnapToGrid->setEnabled(selectedPaths > 0);
        // if only one shape selected, set its parent layer as the active layer
        if (count == 1) {
            KoShapeContainer * parent = selection->selectedShapes().first()->parent();
            while (parent) {
                if (parent->parent())
                    parent = parent->parent();
                else
                    break;
            }
            KoShapeLayer * layer = dynamic_cast<KoShapeLayer*>(parent);
            if (layer)
                selection->setActiveLayer(layer);
        }
    }
}

void KarbonView::setCursor(const QCursor &c)
{
    d->canvas->setCursor(c);
}

void KarbonView::createLayersTabDock()
{
    if (shell())
    {
        KarbonLayerDockerFactory layerFactory;
        KarbonLayerDocker * layerDocker = qobject_cast<KarbonLayerDocker*>(shell()->createDockWidget(&layerFactory));
        layerDocker->setPart(d->part);
        connect(d->canvas->shapeManager(), SIGNAL(selectionChanged()),
                layerDocker, SLOT(updateView()));
        connect(d->canvas->shapeManager(), SIGNAL(selectionContentChanged()),
                layerDocker, SLOT(updateView()));
        connect(shell()->partManager(), SIGNAL(activePartChanged(KParts::Part *)),
                layerDocker, SLOT(setPart(KParts::Part *)));
    }
}

void KarbonView::updateReadWrite(bool readwrite)
{
    canvasWidget()->setReadWrite(readwrite);
    KoToolManager::instance()->updateReadWrite(d->canvasController, readwrite);
}

void KarbonView::updateUnit(const KoUnit &unit)
{
    d->horizRuler->setUnit(unit);
    d->vertRuler->setUnit(unit);
    d->canvas->resourceManager()->setResource(KoCanvasResource::Unit, unit);
}

QList<KoPathShape*> KarbonView::selectedPathShapes()
{
    KoSelection* selection = d->canvas->shapeManager()->selection();
    if (! selection)
        return QList<KoPathShape*>();

    QList<KoShape*> selectedShapes = selection->selectedShapes();
    QList<KoPathShape*> paths;

    foreach(KoShape* shape, selectedShapes) {
        KoPathShape *path = dynamic_cast<KoPathShape*>(shape);
        if (path) {
            paths << path;
            selection->deselect(shape);
        }
    }

    return paths;
}

KoPrintJob * KarbonView::createPrintJob()
{
    return new KarbonPrintJob(this);
}

void KarbonView::applyFillToSelection()
{
    KoSelection *selection = d->canvas->shapeManager()->selection();
    if (! selection->count())
        return;

    KoShape * shape = selection->firstSelectedShape();
    d->canvas->addCommand(new KoShapeBackgroundCommand(selection->selectedShapes(), shape->background()));
}

void KarbonView::applyStrokeToSelection()
{
    KoSelection *selection = d->canvas->shapeManager()->selection();
    if (! selection->count())
        return;

    KoShape * shape = selection->firstSelectedShape();
    d->canvas->addCommand(new KoShapeBorderCommand(selection->selectedShapes(), shape->border()));
}

#include "KarbonView.moc"

