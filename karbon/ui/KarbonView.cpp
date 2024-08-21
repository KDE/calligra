/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2001-2002 Lennart Kudling <kudling@kde.org>
 * SPDX-FileCopyrightText: 2001-2005, 2007 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2002-2003, 2005 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
 * SPDX-FileCopyrightText: 2002-2003, 2006 Laurent Montel <montel@kde.org>
 * SPDX-FileCopyrightText: 2002-2006 Stephan Binner <binner@kde.org>
 * SPDX-FileCopyrightText: 2002, 2005 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2002 Benoit Vautrin <benoit.vautrin@free.fr>
 * SPDX-FileCopyrightText: 2002, 2005-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2003 Dirk Mueller <mueller@kde.org>
 * SPDX-FileCopyrightText: 2003, 2006 Stephan Kulow <coolo@kde.org>
 * SPDX-FileCopyrightText: 2004 Brad Hards <bradh@frogmouth.net>
 * SPDX-FileCopyrightText: 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * SPDX-FileCopyrightText: 2005 Yann Bodson <yann.bodson@online.fr>
 * SPDX-FileCopyrightText: 2005-2010 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2005-2009, 2011 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2005-2006 Peter Simonsson <psn@linux.se>
 * SPDX-FileCopyrightText: 2005-2006 Sven Langkamp <sven.langkamp@gmail.com>
 * SPDX-FileCopyrightText: 2005-2006 Inge Wallin <inge@lysator.liu.se>
 * SPDX-FileCopyrightText: 2005-2006 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2006 Martin Ellis <martin.ellis@kdemail.net>
 * SPDX-FileCopyrightText: 2006 Adriaan de Groot <groot@kde.org>
 * SPDX-FileCopyrightText: 2006 Sebastian Sauer <mail@dipe.org>
 * SPDX-FileCopyrightText: 2006-2007 Thorsten Zachmann <t.zachmann@zagge.de>
 * SPDX-FileCopyrightText: 2006 Andreas Hartmetz <ahartmetz@gmail.com>
 * SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 * SPDX-FileCopyrightText: 2006-2007 Aaron J. Seigo <aseigo@kde.org>
 * SPDX-FileCopyrightText: 2007 Matthias Kretz <kretz@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KarbonView.h"

// Dialogs.
#include "KarbonConfigureDialog.h"

// The rest.
#include "Karbon.h"
#include "KarbonDocument.h"
#include "KarbonDocumentMergeCommand.h"
#include "KarbonFactory.h"
#include "KarbonOutlinePaintingStrategy.h"
#include "KarbonPaletteBarWidget.h"
#include "KarbonPart.h"
#include "KarbonSmallStylePreview.h"
#include "KarbonUiDebug.h"

#include <KoCanvasResourceManager.h>
#include <KoPACanvas.h>
#include <KoPAPageBase.h>

#include <KoCanvasControllerWidget.h>
#include <KoCanvasResourceManager.h>
#include <KoColorBackground.h>
#include <KoComponentData.h>
#include <KoCopyController.h>
#include <KoCutController.h>
#include <KoDockRegistry.h>
#include <KoDockerManager.h>
#include <KoDocumentResourceManager.h>
#include <KoFileDialog.h>
#include <KoFilterManager.h>
#include <KoGridData.h>
#include <KoGuidesData.h>
#include <KoIcon.h>
#include <KoImageCollection.h>
#include <KoImageData.h>
#include <KoMainWindow.h>
#include <KoParameterShape.h>
#include <KoParameterToPathCommand.h>
#include <KoPasteController.h>
#include <KoPathCombineCommand.h>
#include <KoPathPoint.h>
#include <KoPathPointData.h>
#include <KoPathPointMoveCommand.h>
#include <KoPathReverseCommand.h>
#include <KoPathShape.h>
#include <KoPluginLoader.h>
#include <KoProperties.h>
#include <KoRuler.h>
#include <KoRulerController.h>
#include <KoSelection.h>
#include <KoShapeBackgroundCommand.h>
#include <KoShapeClipCommand.h>
#include <KoShapeContainer.h>
#include <KoShapeController.h>
#include <KoShapeCreateCommand.h>
#include <KoShapeDeleteCommand.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeGroup.h>
#include <KoShapeGroupCommand.h>
#include <KoShapeLayer.h>
#include <KoShapeManager.h>
#include <KoShapeRegistry.h>
#include <KoShapeReorderCommand.h>
#include <KoShapeStroke.h>
#include <KoShapeStrokeCommand.h>
#include <KoShapeTransformCommand.h>
#include <KoShapeUnclipCommand.h>
#include <KoSnapGuide.h>
#include <KoStandardAction.h>
#include <KoToolBoxFactory.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>
#include <KoUnit.h>
#include <KoZoomAction.h>
#include <KoZoomController.h>
#include <KoZoomHandler.h>

// KF5 header
#include <KActionCollection>
#include <KLocalizedString>
#include <KMessageBox>
#include <KPluginFactory>
#include <KStandardAction>
#include <KXMLGUIFactory>
#include <kcolormimedata.h>
#include <ktoggleaction.h>

// qt header
#include <QAction>
#include <QDebug>
#include <QDropEvent>
#include <QGridLayout>
#include <QImageReader>
#include <QLabel>
#include <QLocale>
#include <QMimeDatabase>
#include <QPluginLoader>
#include <QResizeEvent>
#include <QStatusBar>

#include <KConfigGroup>
#include <unistd.h>

class Q_DECL_HIDDEN KarbonView::Private
{
public:
    Private(KarbonPart *part, KarbonDocument *doc)
        : karbonPart(part)
        , part(doc)
        , colorBar(nullptr)
        , closePath(nullptr)
        , combinePath(nullptr)
        , separatePath(nullptr)
        , reversePath(nullptr)
        , intersectPath(nullptr)
        , subtractPath(nullptr)
        , unitePath(nullptr)
        , excludePath(nullptr)
        , pathSnapToGrid(nullptr)
        , configureAction(nullptr)
        , clipObjects(nullptr)
        , unclipObjects(nullptr)
        , flipVertical(nullptr)
        , flipHorizontal(nullptr)
        , viewAction(nullptr)
        , snapGridAction(nullptr)
        , showPageMargins(nullptr)
        , showGuidesAction(nullptr)
        , showPaletteAction(nullptr)
        , status(nullptr)
        , cursorCoords(nullptr)
        , smallPreview(nullptr)
    {
    }

    KarbonPart *karbonPart;
    KarbonDocument *part;
    KarbonPaletteBarWidget *colorBar;

    // actions:
    QAction *closePath;
    QAction *combinePath;
    QAction *separatePath;
    QAction *reversePath;
    QAction *intersectPath;
    QAction *subtractPath;
    QAction *unitePath;
    QAction *excludePath;
    QAction *pathSnapToGrid;
    QAction *configureAction;
    QAction *clipObjects;
    QAction *unclipObjects;
    QAction *flipVertical;
    QAction *flipHorizontal;

    KToggleAction *viewAction;

    KToggleAction *snapGridAction;
    KToggleAction *showPageMargins;
    KToggleAction *showGuidesAction;

    KToggleAction *showPaletteAction;

    // Status Bar
    QLabel *status; ///< ordinary status
    QLabel *cursorCoords; ///< cursor coordinates
    KarbonSmallStylePreview *smallPreview; ///< small style preview
};

KarbonView::KarbonView(KarbonPart *karbonPart, KarbonDocument *doc, QWidget *parent)
    : KoPAView(karbonPart, doc, KoPAView::NormalMode, parent)
    , d(new Private(karbonPart, doc))
{
    setAcceptDrops(true);

    setXMLFile(QString::fromLatin1("karbon.rc"));

    d->cursorCoords = new QLabel(QString(), this);
    d->cursorCoords->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    d->cursorCoords->setMinimumWidth(50);
    addStatusBarItem(d->cursorCoords, 0);
    connect(canvasController()->proxyObject, &KoCanvasControllerProxyObject::canvasMousePositionChanged, this, &KarbonView::mousePositionChanged);

    d->smallPreview = new KarbonSmallStylePreview(this);
    connect(d->smallPreview, &KarbonSmallStylePreview::fillApplied, this, &KarbonView::applyFillToSelection);
    connect(d->smallPreview, &KarbonSmallStylePreview::strokeApplied, this, &KarbonView::applyStrokeToSelection);
    addStatusBarItem(d->smallPreview, 0);
    // FIXME: This was not necessary before refactoring to pageapp, why now?
    // Also, changing colors of a shape does not update preview
    connect(shapeManager(), &KoShapeManager::selectionChanged, d->smallPreview, &KarbonSmallStylePreview::selectionChanged);

    initActions();

    // Load all plugins
    const QVector<KPluginMetaData> availablePlugins = KPluginMetaData::findPlugins(QStringLiteral("karbon/extensions"));

    for (const KPluginMetaData &metaData : availablePlugins) {
        auto clientPlugin = KPluginFactory::instantiatePlugin<QObject>(metaData, this);
        if (clientPlugin) {
            auto client = dynamic_cast<KXMLGUIClient *>(clientPlugin.plugin);
            if (client) {
                insertChildClient(client);
            } else {
                clientPlugin.plugin->deleteLater();
                qWarning() << "Not a valid plugin";
            }
        } else {
            qWarning() << "Not a valid plugin";
        }
    }

    unsigned int max = static_cast<KarbonDocument *>(kopaDocument())->maxRecentFiles();
    setNumberOfRecentFiles(max);

    d->colorBar = new KarbonPaletteBarWidget(Qt::Horizontal, this);
    connect(d->colorBar, &KarbonPaletteBarWidget::colorSelected, this, &KarbonView::applyPaletteColor);
    connect(shapeManager(), &KoShapeManager::selectionContentChanged, d->colorBar, &KarbonPaletteBarWidget::updateDocumentColors);
    connect(kopaDocument(), &KoPADocument::shapeAdded, d->colorBar, &KarbonPaletteBarWidget::updateDocumentColors);
    connect(kopaDocument(), &KoPADocument::shapeRemoved, d->colorBar, &KarbonPaletteBarWidget::updateDocumentColors);

    if (mainWindow()) {
        KSharedConfigPtr config = KSharedConfig::openConfig();
        if (config->hasGroup("Interface")) {
            KConfigGroup interfaceGroup = config->group("Interface");
            if (!interfaceGroup.readEntry<bool>("ShowPalette", true)) {
                d->colorBar->setVisible(false);
                d->showPaletteAction->setChecked(false);
            }
        }

        mainWindow()->setStatusBar(statusBar());
    }

    reorganizeGUI();

    setFocusPolicy(Qt::NoFocus);
}

KarbonView::~KarbonView()
{
    removeStatusBarItem(d->cursorCoords);
    removeStatusBarItem(d->smallPreview);

    if (factory()) {
        factory()->removeClient(this);
    }
    delete d;
}

KarbonPaletteBarWidget *KarbonView::colorBar() const
{
    return d->colorBar;
}

KarbonDocument *KarbonView::part() const
{
    return static_cast<KarbonDocument *>(kopaDocument());
}

KoCanvasResourceManager *KarbonView::resourceManager() const
{
    return kopaCanvas()->resourceManager();
}

KoPACanvas *KarbonView::canvasWidget() const
{
    return dynamic_cast<KoPACanvas *>(kopaCanvas());
}

void KarbonView::resizeEvent(QResizeEvent * /*event*/)
{
    if (!kopaCanvas())
        return;

    reorganizeGUI();
}

void KarbonView::dragEnterEvent(QDragEnterEvent *event)
{
    QColor color = KColorMimeData::fromMimeData(event->mimeData());
    if (color.isValid()) {
        event->accept();
    }
    KoView::dragEnterEvent(event);
}

void KarbonView::dropEvent(QDropEvent *e)
{
    // Accepts QColor - from Color Manager's KColorPatch
    QColor color = KColorMimeData::fromMimeData(e->mimeData());
    if (color.isValid()) {
        KoSelection *selection = shapeManager()->selection();
        if (!selection)
            return;

        if (!kopaDocument())
            return;

        if (resourceManager()->intResource(KoCanvasResourceManager::ActiveStyleType) == KoFlake::Foreground) {
            QList<KoShapeStrokeModel *> strokes;
            QList<KoShape *> selectedShapes = selection->selectedShapes();
            foreach (KoShape *shape, selectedShapes) {
                KoShapeStroke *stroke = dynamic_cast<KoShapeStroke *>(shape->stroke());
                KoShapeStroke *newStroke = nullptr;
                if (stroke) {
                    newStroke = new KoShapeStroke(*stroke);
                    newStroke->setColor(color);
                } else {
                    newStroke = new KoShapeStroke(1.0, color);
                }
                strokes.append(newStroke);
            }
            kopaCanvas()->addCommand(new KoShapeStrokeCommand(selectedShapes, strokes, nullptr));
        } else {
            QSharedPointer<KoShapeBackground> fill(new KoColorBackground(color));
            kopaCanvas()->addCommand(new KoShapeBackgroundCommand(selection->selectedShapes(), fill, nullptr));
        }
    }

    KoPAView::dropEvent(e);
}

void KarbonView::fileImportGraphic()
{
    QByteArray nativeMimeType = kopaDocument()->nativeFormatMimeType();
    QStringList filter = KoFilterManager::mimeFilter(nativeMimeType, KoFilterManager::Import);

    QStringList imageFilter;
    // add filters for all formats supported by QImage
    foreach (const QByteArray &mimeType, QImageReader::supportedMimeTypes()) {
        imageFilter << QLatin1String(mimeType);
    }
    filter.append(imageFilter);

    KoFileDialog dialog(nullptr, KoFileDialog::OpenFile, "OpenDocument");
    dialog.setCaption(i18n("Choose Graphic to Add"));
    dialog.setMimeTypeFilters(imageFilter);
    QString fname = dialog.filename();

    if (fname.isEmpty())
        return;

    KarbonPart importPart(nullptr);
    KarbonDocument importDocument(&importPart);
    importPart.setDocument(&importDocument);

    bool success = true;

    // check if we have an empty mime type (probably because the "All supported files"
    // filter was active)
    QString currentMimeFilter;
    // get mime type from file
    QMimeType mimeType = QMimeDatabase().mimeTypeForFile(fname);
    if (mimeType.isValid()) {
        const QString mime = mimeType.name();
        if (mime == nativeMimeType) {
            currentMimeFilter = nativeMimeType;
        } else {
            foreach (const QString &filter, imageFilter) {
                if (mime == filter) {
                    currentMimeFilter = filter;
                    break;
                }
            }
        }
    }

    // check if we are loading an image format
    if (imageFilter.contains(currentMimeFilter)) {
        QImage image;
        if (!image.load(fname)) {
            KMessageBox::error(nullptr, i18n("Could not load image."), i18n("Import graphic"), KMessageBox::Options());
            return;
        }
        KoShapeFactoryBase *factory = KoShapeRegistry::instance()->get("PictureShape");
        if (!factory) {
            KMessageBox::error(nullptr, i18n("Could not create image shape."), i18n("Import graphic"), KMessageBox::Options());
            return;
        }

        KoShape *picture = factory->createDefaultShape(kopaDocument()->resourceManager());
        KoImageCollection *imageCollection = kopaDocument()->resourceManager()->imageCollection();
        if (!picture || !imageCollection) {
            KMessageBox::error(nullptr, i18n("Could not create image shape."), i18n("Import graphic"), KMessageBox::Options());
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

        KUndo2Command *cmd = kopaCanvas()->shapeController()->addShapeDirect(picture);
        cmd->setText(kundo2_i18n("Insert graphics"));
        kopaCanvas()->addCommand(cmd);
        shapeManager()->selection()->select(picture);
        return;
    }
    // TODO: It is not obvious how this is best implemented when importing multipage docs
    // Append pages?
    // Append layers to existing pages?
    // Add shapes to active page?
    // etc?
    // check if we are loading our native format
    if (nativeMimeType == currentMimeFilter) {
        // directly load the native format
        success = importDocument.loadNativeFormat(fname);
        if (!success) {
            importDocument.showLoadingErrorDialog();
        }
    } else {
        // use import filters to load the file
        KoFilterManager man(&importDocument);
        KoFilter::ConversionStatus status = KoFilter::OK;
        QString importedFile = man.importDocument(fname, QString(), status);
        if (status != KoFilter::OK) {
            importDocument.showLoadingErrorDialog();
            success = false;
        } else if (!importedFile.isEmpty()) {
            success = importDocument.loadNativeFormat(importedFile);
            if (!success) {
                importDocument.showLoadingErrorDialog();
            }
            // remove the temporary file created during format conversion
            unlink(QFile::encodeName(importedFile));
        }
    }

    if (success) {
        KarbonDocumentMergeCommand *cmd = new KarbonDocumentMergeCommand(dynamic_cast<KarbonDocument *>(kopaDocument()), importDocument);
        kopaCanvas()->addCommand(cmd);
        /*
                foreach(KoShape * shape, importedShapes) {
                    d->canvas->shapeManager()->selection()->select(shape, false);
                }*/
    }
}

void KarbonView::selectionDuplicate()
{
    kopaCanvas()->toolProxy()->copy();
    kopaCanvas()->toolProxy()->paste();
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
    KoSelection *selection = shapeManager()->selection();
    if (!selection)
        return;

    QList<KoShape *> selectedShapes = selection->selectedShapes(KoFlake::TopLevelSelection);
    if (selectedShapes.count() < 2)
        return;

    KoShapeDistributeCommand *cmd = new KoShapeDistributeCommand(selectedShapes, distribute, selection->boundingRect());

    kopaCanvas()->addCommand(cmd);
}

void KarbonView::clipObjects()
{
    KoSelection *selection = shapeManager()->selection();
    if (!selection)
        return;

    QList<KoShape *> selectedShapes = selection->selectedShapes(KoFlake::TopLevelSelection);
    if (!selectedShapes.count())
        return;

    KoShape *shapeToClip = selectedShapes.first();
    selectedShapes.removeOne(shapeToClip);

    QList<KoPathShape *> clipPaths;
    foreach (KoShape *shape, selectedShapes) {
        KoPathShape *path = dynamic_cast<KoPathShape *>(shape);
        if (path)
            clipPaths.append(path);
    }

    if (!clipPaths.count())
        return;

    KUndo2Command *cmd = new KoShapeClipCommand(kopaDocument(), shapeToClip, clipPaths);
    kopaCanvas()->addCommand(cmd);
}

void KarbonView::unclipObjects()
{
    KoSelection *selection = shapeManager()->selection();
    if (!selection)
        return;

    QList<KoShape *> selectedShapes = selection->selectedShapes(KoFlake::TopLevelSelection);
    if (!selectedShapes.count())
        return;

    QList<KoShape *> shapesToUnclip;
    foreach (KoShape *shape, selectedShapes) {
        if (shape->clipPath())
            shapesToUnclip.append(shape);
    }
    if (!shapesToUnclip.count())
        return;

    kopaCanvas()->addCommand(new KoShapeUnclipCommand(kopaDocument(), shapesToUnclip));
}

void KarbonView::flipVertical()
{
    selectionFlip(false, true);
}

void KarbonView::flipHorizontal()
{
    selectionFlip(true, false);
}

void KarbonView::selectionFlip(bool horizontally, bool vertically)
{
    if (!horizontally && !vertically)
        return;

    KoSelection *selection = shapeManager()->selection();
    if (!selection)
        return;

    QList<KoShape *> selectedShapes = selection->selectedShapes(KoFlake::StrippedSelection);
    const int selectedShapesCount = selectedShapes.count();
    if (selectedShapesCount < 1)
        return;

    // mirror about center point
    QPointF mirrorCenter = selection->absolutePosition(KoFlake::CenteredPosition);

    QTransform mirrorMatrix;
    mirrorMatrix.translate(mirrorCenter.x(), mirrorCenter.y());
    mirrorMatrix.scale(horizontally ? -1.0 : 1.0, vertically ? -1.0 : 1.0);
    mirrorMatrix.translate(-mirrorCenter.x(), -mirrorCenter.y());

    QVector<QTransform> oldState;
    QVector<QTransform> newState;
    oldState.reserve(selectedShapesCount);
    newState.reserve(selectedShapesCount);

    foreach (KoShape *shape, selectedShapes) {
        shape->update();
        oldState << shape->transformation();
        // apply the mirror transformation
        shape->applyAbsoluteTransformation(mirrorMatrix);
        newState << shape->transformation();
    }
    selection->applyAbsoluteTransformation(mirrorMatrix);

    KUndo2Command *cmd = new KoShapeTransformCommand(selectedShapes, oldState, newState);
    if (horizontally && !vertically)
        cmd->setText(kundo2_i18n("Mirror Horizontally"));
    else if (!horizontally && vertically)
        cmd->setText(kundo2_i18n("Mirror Vertically"));
    else
        cmd->setText(kundo2_i18n("Mirror Horizontally and Vertically"));
    kopaCanvas()->addCommand(cmd);
}

void KarbonView::closePath()
{
    // TODO add the new close path command here
}

void KarbonView::combinePath()
{
    KoSelection *selection = shapeManager()->selection();
    if (!selection)
        return;

    QList<KoShape *> selectedShapes = selection->selectedShapes();
    QList<KoPathShape *> paths;

    foreach (KoShape *shape, selectedShapes) {
        KoPathShape *path = dynamic_cast<KoPathShape *>(shape);
        if (path) {
            KoParameterShape *paramShape = dynamic_cast<KoParameterShape *>(path);
            if (paramShape && paramShape->isParametricShape())
                continue;
            paths << path;
            selection->deselect(shape);
        }
    }

    if (paths.size())
        kopaCanvas()->addCommand(new KoPathCombineCommand(kopaDocument(), paths));
}

void KarbonView::separatePath()
{
    KoSelection *selection = shapeManager()->selection();
    if (!selection)
        return;

    QList<KoShape *> selectedShapes = selection->selectedShapes();
    QList<KoPathShape *> paths;

    foreach (KoShape *shape, selectedShapes) {
        KoPathShape *path = dynamic_cast<KoPathShape *>(shape);
        if (path) {
            paths << path;
            selection->deselect(shape);
        }
    }

    if (!paths.size()) {
        return;
    }

    KUndo2Command *cmd = new KUndo2Command;
    cmd->setText(kundo2_i18n("Separate paths"));

    foreach (KoPathShape *p, paths) {
        QList<KoPathShape *> separatedPaths;
        QList<KoShape *> newShapes;
        if (p->separate(separatedPaths)) {
            foreach (KoPathShape *subPath, separatedPaths) {
                new KoShapeCreateCommand(kopaDocument(), subPath, cmd);
                newShapes << subPath;
            }
            // make sure we put the new subpaths into the parent
            // of the original path
            KoShapeGroup *parentGroup = dynamic_cast<KoShapeGroup *>(p->parent());
            if (parentGroup) {
                new KoShapeGroupCommand(parentGroup, newShapes, cmd);
            }
            new KoShapeDeleteCommand(kopaDocument(), p, cmd);
        }
    }
    kopaCanvas()->addCommand(cmd);
}

void KarbonView::reversePath()
{
    QList<KoPathShape *> paths = selectedPathShapes();
    if (paths.size())
        kopaCanvas()->addCommand(new KoPathReverseCommand(paths));
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
    KoSelection *selection = shapeManager()->selection();
    if (!selection)
        return;

    QList<KoShape *> selectedShapes = selection->selectedShapes();
    QList<KoPathShape *> paths;

    foreach (KoShape *shape, selectedShapes) {
        KoPathShape *path = dynamic_cast<KoPathShape *>(shape);
        if (path) {
            paths << path;
            selection->deselect(shape);
        }
    }

    if (paths.size() == 2) {
        KUndo2Command *macro = new KUndo2Command(kundo2_i18n("Boolean Operation"));
        KoParameterShape *paramShape = dynamic_cast<KoParameterShape *>(paths[0]);
        if (paramShape && paramShape->isParametricShape())
            new KoParameterToPathCommand(paramShape, macro);
        paramShape = dynamic_cast<KoParameterShape *>(paths[1]);
        if (paramShape && paramShape->isParametricShape())
            new KoParameterToPathCommand(paramShape, macro);
        new KarbonBooleanCommand(kopaDocument(), paths[0], paths[1], operation, macro);
        new KoShapeDeleteCommand(kopaDocument(), paths[0], macro);
        new KoShapeDeleteCommand(kopaDocument(), paths[1], macro);
        kopaCanvas()->addCommand(macro);
    }
}

void KarbonView::pathSnapToGrid()
{
    KoSelection *selection = shapeManager()->selection();
    if (!selection)
        return;

    QList<KoShape *> selectedShapes = selection->selectedShapes();
    QList<KoPathPointData> points;
    QVector<QPointF> offsets;

    // store current grid snap state
    bool oldSnapToGrid = kopaDocument()->gridData().snapToGrid();
    // enable grid snapping
    kopaDocument()->gridData().setSnapToGrid(true);

    KoSnapGuide snapGuide(kopaCanvas());
    snapGuide.enableSnapStrategies(KoSnapGuide::GridSnapping);
    snapGuide.setSnapDistance(INT_MAX);

    foreach (KoShape *shape, selectedShapes) {
        KoParameterShape *paramShape = dynamic_cast<KoParameterShape *>(shape);
        if (paramShape && paramShape->isParametricShape())
            continue;

        KoPathShape *path = dynamic_cast<KoPathShape *>(shape);
        if (!path)
            continue;

        uint subpathCount = path->subpathCount();
        for (uint i = 0; i < subpathCount; ++i) {
            uint pointCount = path->subpathPointCount(i);
            for (uint j = 0; j < pointCount; ++j) {
                KoPathPointIndex index(i, j);
                KoPathPoint *p = path->pointByIndex(index);
                if (!p)
                    continue;

                QPointF docPoint = path->shapeToDocument(p->point());
                QPointF offset = snapGuide.snap(docPoint, {}) - docPoint;
                points.append(KoPathPointData(path, index));
                offsets.append(offset);
            }
        }
    }

    // reset grid snapping state to old value
    kopaDocument()->gridData().setSnapToGrid(oldSnapToGrid);

    kopaCanvas()->addCommand(new KoPathPointMoveCommand(points, offsets));
}

void KarbonView::viewModeChanged(bool outlineMode)
{
    if (outlineMode) {
        new KarbonOutlinePaintingStrategy(shapeManager());
    } else {
        shapeManager()->setPaintingStrategy(new KoShapeManagerPaintingStrategy(shapeManager()));
    }
}

void KarbonView::zoomSelection()
{
    KoSelection *selection = shapeManager()->selection();
    if (!selection)
        return;

    if (!selection->count())
        return;

    const KoZoomHandler *zoomHandler = dynamic_cast<const KoZoomHandler *>(viewConverter());
    if (!zoomHandler)
        return;

    QRectF bbox = selection->boundingRect();
    QRect viewRect = zoomHandler->documentToView(bbox).toRect();

    kopaCanvas()->canvasController()->zoomTo(viewRect.translated(kopaCanvas()->documentOrigin()));
    //     QPointF newCenter = kopaCanvas()->documentOrigin() + zoomHandler->documentToView(bbox.center());
    //     kopaCanvas()->setPreferredCenter(newCenter.toPoint());
}

void KarbonView::zoomDrawing()
{
    const KoZoomHandler *zoomHandler = dynamic_cast<const KoZoomHandler *>(kopaCanvas()->viewConverter());
    if (!zoomHandler)
        return;

    QRectF bbox = activePage()->contentRect();
    if (bbox.isNull())
        return;

    QRect viewRect = zoomHandler->documentToView(bbox).toRect();
    kopaCanvas()->canvasController()->zoomTo(viewRect.translated(kopaCanvas()->documentOrigin()));
    //     QPointF newCenter = kopaCanvas()->documentOrigin() + zoomHandler->documentToView(bbox.center());
    //     kopaCanvas()->setPreferredCenter(newCenter.toPoint());
}

void KarbonView::initActions()
{
    // view ----->
    d->viewAction = new KToggleAction(i18n("Outline &Mode"), this);
    actionCollection()->addAction("view_mode", d->viewAction);
    connect(d->viewAction, &QAction::toggled, this, &KarbonView::viewModeChanged);

    // No need for the other actions in read-only (embedded) mode
    if (!mainWindow())
        return;

    QAction *actionImportGraphic = new QAction(i18n("&Import Graphic..."), this);
    actionCollection()->addAction("file_import", actionImportGraphic);
    connect(actionImportGraphic, &QAction::triggered, this, &KarbonView::fileImportGraphic);

    QAction *actionEditGuides = new QAction(koIcon("edit-guides"), i18n("Edit Guides"), this);
    actionCollection()->addAction("edit_guides", actionEditGuides);
    connect(actionEditGuides, &QAction::triggered, this, &KarbonView::editGuides);
    // edit <-----

    // object ----->
    QAction *actionDuplicate = new QAction(i18nc("Duplicate selection", "&Duplicate"), this);
    actionCollection()->addAction("object_duplicate", actionDuplicate);
    actionDuplicate->setShortcut(QKeySequence("Ctrl+D"));
    connect(actionDuplicate, &QAction::triggered, this, &KarbonView::selectionDuplicate);

    QAction *actionDistributeHorizontalCenter = new QAction(koIcon("distribute-horizontal-center"), i18n("Distribute Center (Horizontal)"), this);
    actionCollection()->addAction("object_distribute_horizontal_center", actionDistributeHorizontalCenter);
    connect(actionDistributeHorizontalCenter, &QAction::triggered, this, &KarbonView::selectionDistributeHorizontalCenter);

    QAction *actionDistributeHorizontalGap = new QAction(koIcon("distribute-horizontal-equal"), i18n("Distribute Gaps (Horizontal)"), this);
    actionCollection()->addAction("object_distribute_horizontal_gap", actionDistributeHorizontalGap);
    connect(actionDistributeHorizontalGap, &QAction::triggered, this, &KarbonView::selectionDistributeHorizontalGap);

    QAction *actionDistributeLeft = new QAction(koIcon("distribute-horizontal-left"), i18n("Distribute Left Borders"), this);
    actionCollection()->addAction("object_distribute_horizontal_left", actionDistributeLeft);
    connect(actionDistributeLeft, &QAction::triggered, this, &KarbonView::selectionDistributeHorizontalLeft);

    QAction *actionDistributeRight = new QAction(koIcon("distribute-horizontal-right"), i18n("Distribute Right Borders"), this);
    actionCollection()->addAction("object_distribute_horizontal_right", actionDistributeRight);
    connect(actionDistributeRight, &QAction::triggered, this, &KarbonView::selectionDistributeHorizontalRight);

    QAction *actionDistributeVerticalCenter = new QAction(koIcon("distribute-vertical-center"), i18n("Distribute Center (Vertical)"), this);
    actionCollection()->addAction("object_distribute_vertical_center", actionDistributeVerticalCenter);
    connect(actionDistributeVerticalCenter, &QAction::triggered, this, &KarbonView::selectionDistributeVerticalCenter);

    QAction *actionDistributeVerticalGap = new QAction(koIcon("distribute-vertical-equal"), i18n("Distribute Gaps (Vertical)"), this);
    actionCollection()->addAction("object_distribute_vertical_gap", actionDistributeVerticalGap);
    connect(actionDistributeVerticalGap, &QAction::triggered, this, &KarbonView::selectionDistributeVerticalGap);

    QAction *actionDistributeBottom = new QAction(koIcon("distribute-vertical-bottom"), i18n("Distribute Bottom Borders"), this);
    actionCollection()->addAction("object_distribute_vertical_bottom", actionDistributeBottom);
    connect(actionDistributeBottom, &QAction::triggered, this, &KarbonView::selectionDistributeVerticalBottom);

    QAction *actionDistributeTop = new QAction(koIcon("distribute-vertical-top"), i18n("Distribute Top Borders"), this);
    actionCollection()->addAction("object_distribute_vertical_top", actionDistributeTop);
    connect(actionDistributeTop, &QAction::triggered, this, &KarbonView::selectionDistributeVerticalTop);

    d->showPaletteAction = new KToggleAction(i18n("Show Color Palette"), this);
    actionCollection()->addAction("view_show_palette", d->showPaletteAction);
    d->showPaletteAction->setToolTip(i18n("Show or hide color palette"));
    d->showPaletteAction->setChecked(true);
    connect(d->showPaletteAction, &QAction::triggered, this, &KarbonView::showPalette);

    d->clipObjects = new QAction(i18n("&Clip Object"), this);
    actionCollection()->addAction("object_clip", d->clipObjects);
    connect(d->clipObjects, &QAction::triggered, this, &KarbonView::clipObjects);

    d->unclipObjects = new QAction(i18n("&Unclip Objects"), this);
    actionCollection()->addAction("object_unclip", d->unclipObjects);
    connect(d->unclipObjects, &QAction::triggered, this, &KarbonView::unclipObjects);

    d->flipVertical = new QAction(koIcon("object-flip-vertical"), i18n("Mirror Vertically"), this);
    actionCollection()->addAction("object_flip_vertical", d->flipVertical);
    connect(d->flipVertical, &QAction::triggered, this, &KarbonView::flipVertical);

    d->flipHorizontal = new QAction(koIcon("object-flip-horizontal"), i18n("Mirror Horizontally"), this);
    actionCollection()->addAction("object_flip_horizontal", d->flipHorizontal);
    connect(d->flipHorizontal, &QAction::triggered, this, &KarbonView::flipHorizontal);

    // object <-----

    // path ------->
    d->closePath = new QAction(i18n("&Close Path"), this);
    actionCollection()->addAction("close_path", d->closePath);
    d->closePath->setShortcut(QKeySequence("Ctrl+U"));
    d->closePath->setEnabled(false);
    connect(d->closePath, &QAction::triggered, this, &KarbonView::closePath);

    d->combinePath = new QAction(i18n("Com&bine Path"), this);
    actionCollection()->addAction("combine_path", d->combinePath);
    d->combinePath->setShortcut(QKeySequence("Ctrl+K"));
    d->combinePath->setEnabled(false);
    connect(d->combinePath, &QAction::triggered, this, &KarbonView::combinePath);

    d->separatePath = new QAction(i18n("Se&parate Path"), this);
    actionCollection()->addAction("separate_path", d->separatePath);
    d->separatePath->setShortcut(QKeySequence("Shift+Ctrl+K"));
    d->separatePath->setEnabled(false);
    connect(d->separatePath, &QAction::triggered, this, &KarbonView::separatePath);

    d->reversePath = new QAction(i18n("Re&verse Path"), this);
    actionCollection()->addAction("reverse_path", d->reversePath);
    d->reversePath->setShortcut(QKeySequence("Ctrl+R"));
    d->reversePath->setEnabled(false);
    connect(d->reversePath, &QAction::triggered, this, &KarbonView::reversePath);

    d->intersectPath = new QAction(i18n("Intersect Paths"), this);
    actionCollection()->addAction("intersect_path", d->intersectPath);
    // d->intersectPath->setShortcut(QKeySequence("Shift+Ctrl+K"));
    d->intersectPath->setEnabled(false);
    connect(d->intersectPath, &QAction::triggered, this, &KarbonView::intersectPaths);

    d->subtractPath = new QAction(i18n("Subtract Paths"), this);
    actionCollection()->addAction("subtract_path", d->subtractPath);
    // d->subtractPath->setShortcut(QKeySequence("Shift+Ctrl+K"));
    d->subtractPath->setEnabled(false);
    connect(d->subtractPath, &QAction::triggered, this, &KarbonView::subtractPaths);

    d->unitePath = new QAction(i18n("Unite Paths"), this);
    actionCollection()->addAction("unite_path", d->unitePath);
    // d->unitePath->setShortcut(QKeySequence("Shift+Ctrl+K"));
    d->unitePath->setEnabled(false);
    connect(d->unitePath, &QAction::triggered, this, &KarbonView::unitePaths);

    d->excludePath = new QAction(i18n("Exclude Paths"), this);
    actionCollection()->addAction("exclude_path", d->excludePath);
    // d->excludePath->setShortcut(QKeySequence("Shift+Ctrl+K"));
    d->excludePath->setEnabled(false);
    connect(d->excludePath, &QAction::triggered, this, &KarbonView::excludePaths);

    d->pathSnapToGrid = new QAction(i18n("Snap Path to Grid"), this);
    actionCollection()->addAction("path_snap_to_grid", d->pathSnapToGrid);
    d->pathSnapToGrid->setEnabled(false);
    connect(d->pathSnapToGrid, &QAction::triggered, this, &KarbonView::pathSnapToGrid);

    // path <-----

    // view ---->
    QAction *zoomSelection = new QAction(koIcon("zoom-select"), i18n("Zoom to Selection"), this);
    actionCollection()->addAction("view_zoom_selection", zoomSelection);
    connect(zoomSelection, &QAction::triggered, this, &KarbonView::zoomSelection);

    QAction *zoomDrawing = new QAction(koIcon("zoom-draw"), i18n("Zoom to Drawing"), this);
    actionCollection()->addAction("view_zoom_drawing", zoomDrawing);
    connect(zoomDrawing, &QAction::triggered, this, &KarbonView::zoomDrawing);
    // view <-----
}

void KarbonView::mousePositionChanged(const QPoint &position)
{
    const QPoint canvasOffset(canvasController()->canvasOffsetX(), canvasController()->canvasOffsetY());
    const QPoint viewPos = position - kopaCanvas()->documentOrigin() - canvasOffset;

    QPointF documentPos = kopaCanvas()->viewConverter()->viewToDocument(viewPos);
    qreal x = kopaDocument()->unit().toUserValue(documentPos.x());
    qreal y = kopaDocument()->unit().toUserValue(documentPos.y());

    if (statusBar() && statusBar()->isVisible()) {
        QLocale locale;
        d->cursorCoords->setText(QString::fromLatin1("%1, %2").arg(locale.toString(x, 'f', 2), locale.toString(y, 'f', 2)));
    }
}

void KarbonView::reorganizeGUI()
{
    // TODO: Find a better solution, maybe move to KoPAView?
    if (statusBar()) {
        bool show = true;
        if (mainWindow()) {
            KSharedConfigPtr config = KSharedConfig::openConfig();
            if (config->hasGroup("Interface")) {
                KConfigGroup interfaceGroup = config->group("Interface");
                if (!interfaceGroup.readEntry<bool>("ShowStatusBar", true)) {
                    show = false;
                }
            }
        }
        statusBar()->setVisible(show);
    }
}

void KarbonView::setNumberOfRecentFiles(unsigned int number)
{
    if (mainWindow()) { // 0L when embedded into konq !
        mainWindow()->setMaxRecentItems(number);
    }
}

void KarbonView::editGuides()
{
    KoToolManager::instance()->switchToolRequested("GuidesTool_ID");
}

void KarbonView::showPalette()
{
    if (!mainWindow())
        return;

    const bool showPalette = d->showPaletteAction->isChecked();
    d->colorBar->setVisible(showPalette);

    // this will make the last setting of the ruler visibility persistent
    KConfigGroup interfaceGroup = KarbonFactory::global().config()->group("Interface");
    if (showPalette && !interfaceGroup.hasDefault("ShowPalette"))
        interfaceGroup.revertToDefault("ShowPalette");
    else
        interfaceGroup.writeEntry("ShowPalette", showPalette);
}

void KarbonView::openConfiguration()
{
    QPointer<KarbonConfigureDialog> dialog = new KarbonConfigureDialog(this);
    dialog->exec();
    delete dialog;
    reorganizeGUI();
}

void KarbonView::selectionChanged()
{
    if (!mainWindow())
        return;
    KoSelection *selection = kopaCanvas()->shapeManager()->selection();
    QList<KoShape *> selectedShapes = selection->selectedShapes(KoFlake::FullSelection);
    const int count = selectedShapes.count();

    d->closePath->setEnabled(false);
    d->combinePath->setEnabled(false);
    d->excludePath->setEnabled(false);
    d->intersectPath->setEnabled(false);
    d->subtractPath->setEnabled(false);
    d->unitePath->setEnabled(false);
    d->pathSnapToGrid->setEnabled(false);
    d->clipObjects->setEnabled(false);
    d->unclipObjects->setEnabled(false);
    d->flipHorizontal->setEnabled(count > 0);
    d->flipVertical->setEnabled(count > 0);

    debugKarbonUi << count << " shapes selected";

    if (count > 0) {
        uint selectedPaths = 0;
        uint selectedParametrics = 0;
        // check for different shape types for enabling specific actions
        foreach (KoShape *shape, selectedShapes) {
            if (dynamic_cast<KoPathShape *>(shape)) {
                KoParameterShape *ps = dynamic_cast<KoParameterShape *>(shape);
                if (ps && ps->isParametricShape())
                    selectedParametrics++;
                else
                    selectedPaths++;
            }
        }
        debugKarbonUi << selectedPaths << " path shapes selected";
        debugKarbonUi << selectedParametrics << " parameter shapes selected";
        // TODO enable action when the ClosePath command is ported
        // d->closePath->setEnabled( selectedPaths > 0 );
        d->combinePath->setEnabled(selectedPaths > 1);
        d->separatePath->setEnabled(selectedPaths > 0);
        d->reversePath->setEnabled(selectedPaths > 0);
        d->excludePath->setEnabled(selectedPaths + selectedParametrics == 2);
        d->intersectPath->setEnabled(selectedPaths + selectedParametrics == 2);
        d->subtractPath->setEnabled(selectedPaths + selectedParametrics == 2);
        d->unitePath->setEnabled(selectedPaths + selectedParametrics == 2);
        d->pathSnapToGrid->setEnabled(selectedPaths > 0);
        d->clipObjects->setEnabled(selectedPaths > 0 && count > 1);
        d->unclipObjects->setEnabled(selectedShapes.first()->clipPath() != nullptr);
        // if only one shape selected, set its parent layer as the active layer
        if (count == 1) {
            KoShapeContainer *parent = selection->selectedShapes().first()->parent();
            while (parent) {
                if (parent->parent())
                    parent = parent->parent();
                else
                    break;
            }
            KoShapeLayer *layer = dynamic_cast<KoShapeLayer *>(parent);
            if (layer)
                selection->setActiveLayer(layer);
        }
    }
}

void KarbonView::setCursor(const QCursor &c)
{
    kopaCanvas()->setCursor(c);
}

void KarbonView::updateReadWrite(bool readwrite)
{
    Q_UNUSED(readwrite);
}

QList<KoPathShape *> KarbonView::selectedPathShapes()
{
    KoSelection *selection = shapeManager()->selection();
    if (!selection)
        return QList<KoPathShape *>();

    QList<KoShape *> selectedShapes = selection->selectedShapes();
    QList<KoPathShape *> paths;

    foreach (KoShape *shape, selectedShapes) {
        KoPathShape *path = dynamic_cast<KoPathShape *>(shape);
        if (path) {
            paths << path;
            selection->deselect(shape);
        }
    }

    return paths;
}

void KarbonView::applyFillToSelection()
{
    KoSelection *selection = shapeManager()->selection();
    if (!selection->count())
        return;

    KoShape *shape = selection->firstSelectedShape();
    kopaCanvas()->addCommand(new KoShapeBackgroundCommand(selection->selectedShapes(), shape->background()));
}

void KarbonView::applyStrokeToSelection()
{
    KoSelection *selection = shapeManager()->selection();
    if (!selection->count())
        return;

    KoShape *shape = selection->firstSelectedShape();
    kopaCanvas()->addCommand(new KoShapeStrokeCommand(selection->selectedShapes(), shape->stroke()));
}

void KarbonView::applyPaletteColor(const KoColor &color)
{
    KoSelection *selection = shapeManager()->selection();
    if (!selection->count())
        return;

    int style = resourceManager()->intResource(KoCanvasResourceManager::ActiveStyleType);
    if (style == KoFlake::Foreground) {
        QList<KoShapeStrokeModel *> newStrokes;
        foreach (KoShape *shape, selection->selectedShapes()) {
            KoShapeStroke *stroke = dynamic_cast<KoShapeStroke *>(shape->stroke());
            if (stroke) {
                // preserve stroke properties
                KoShapeStroke *newStroke = new KoShapeStroke(*stroke);
                newStroke->setColor(color.toQColor());
                newStrokes << newStroke;
            } else {
                newStrokes << new KoShapeStroke(1.0, color.toQColor());
            }
        }
        kopaCanvas()->addCommand(new KoShapeStrokeCommand(selection->selectedShapes(), newStrokes));
        resourceManager()->setForegroundColor(color);
    } else {
        QSharedPointer<KoShapeBackground> fill(new KoColorBackground(color.toQColor()));
        kopaCanvas()->addCommand(new KoShapeBackgroundCommand(selection->selectedShapes(), fill));
        resourceManager()->setBackgroundColor(color);
    }
}

void KarbonView::replaceActivePage(KoPAPageBase *page, KoPAPageBase *newActivePage)
{
    if (page == activePage()) {
        viewMode()->updateActivePage(newActivePage);
    }
}
