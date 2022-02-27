/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2005-2006 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2000-2002 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 2002 Phillip Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 1999-2000 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "Doc.h"
#include "../DocBase_p.h"

#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>

#include <QApplication>
#include <QFont>
#include <QTimer>
#include <QList>
#include <QPainter>
#include <QGraphicsItem>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <kmessagebox.h>

#include <KoComponentData.h>
#include <KoDocumentInfo.h>
#include <KoMainWindow.h>
#include <KoOasisSettings.h>
#include <KoDocumentResourceManager.h>
#include <KoShapeConfigFactoryBase.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <KoZoomHandler.h>
#include <KoShapeSavingContext.h>
#include <KoUpdater.h>
#include <KoProgressUpdater.h>
#include <KoView.h>
#include <KoUnit.h>

#include "SheetsDebug.h"
#include "BindingManager.h"
#include "CalculationSettings.h"
#include "Canvas.h"
#include "CanvasItem.h"
#include "DependencyManager.h"
#include "Factory.h"
#include "Formula.h"
#include "Function.h"
#include "FunctionModuleRegistry.h"
#include "HeaderFooter.h"
#include "LoadingInfo.h"
#include "Localization.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "PrintSettings.h"
#include "RecalcManager.h"
#include "Sheet.h"
#include "SheetPrint.h"
#include "StyleManager.h"
#include "Util.h"
#include "View.h"
#include "SheetAccessModel.h"
#include "BindingModel.h"
#include "ksp/SheetsKsp.h"

// D-Bus
#ifndef QT_NO_DBUS
#include "interfaces/MapAdaptor.h"
#include "interfaces/SheetAdaptor.h"
#include <QDBusConnection>
#endif

// chart shape
#include "plugins/chartshape/ChartShape.h"
#include "chart/ChartDialog.h"

// ui
#include "ui/Selection.h"
#include "ui/SheetView.h"

using namespace std;
using namespace Calligra::Sheets;

class Q_DECL_HIDDEN Doc::Private
{
public:
    Map *map;
    static QList<Doc*> s_docs;
    static int s_docId;

    // document properties
    bool configLoadFromFile       : 1;
    QStringList spellListIgnoreAll;
    SheetAccessModel *sheetAccessModel;
    KoDocumentResourceManager *resourceManager;
};

/*****************************************************************************
 *
 * Doc
 *
 *****************************************************************************/

QList<Doc*> Doc::Private::s_docs;
int Doc::Private::s_docId = 0;

Doc::Doc(KoPart *part)
        : DocBase(part)
        , dd(new Private)
{
    Q_ASSERT(part);
    connect(d->map, &Map::sheetAdded, this, &Doc::sheetAdded);

#ifndef QT_NO_DBUS
    new MapAdaptor(d->map);
    QDBusConnection::sessionBus().registerObject('/' + objectName() + '/' + d->map->objectName(), d->map);
#endif

    KoShapeRegistry *registry = KoShapeRegistry::instance();
    foreach (const QString &id, registry->keys()) {
        KoShapeFactoryBase *shapeFactory = registry->value(id);
        shapeFactory->newDocumentResourceManager(resourceManager());
    }

    // Init chart shape factory with Calligra Sheets' specific configuration panels.
    KoShapeFactoryBase *chartShape = KoShapeRegistry::instance()->value(ChartShapeId);
    if (chartShape) {
        QList<KoShapeConfigFactoryBase*> panels = ChartDialog::panels(d->map);
        chartShape->setOptionPanels(panels);
    } else {
        warnSheets << "chart shape factory not found";
    }

    connect(d->map, &Map::commandAdded,
            this, &KoDocument::addCommand);

    // Load the function modules.
    FunctionModuleRegistry::instance()->loadFunctionModules();
}

Doc::~Doc()
{
    //don't save config when words is embedded into konqueror
    saveConfig();

    delete dd;
}

void Doc::initEmpty()
{
    KSharedConfigPtr config = Factory::global().config();
    const int page = config->group("Parameters").readEntry("NbPage", 1);

    for (int i = 0; i < page; ++i)
        map()->addNewSheet();

    resetURL();
    initConfig();
    map()->styleManager()->createBuiltinStyles();

    KoDocument::initEmpty();
}

void Doc::saveConfig()
{
    KSharedConfigPtr config = Factory::global().config();
    Q_UNUSED(config);
}

void Doc::initConfig()
{
    KSharedConfigPtr config = Factory::global().config();

    const int page = config->group("Tables Page Layout").readEntry("Default unit page", 0);
    setUnit(KoUnit::fromListForUi(page, KoUnit::HidePixel));
}

int Doc::supportedSpecialFormats() const
{
    return KoDocument::supportedSpecialFormats();
}

bool Doc::completeSaving(KoStore* _store)
{
    Q_UNUSED(_store);
    return true;
}


QDomDocument Doc::saveXML()
{
    /* don't pull focus away from the editor if this is just a background
       autosave */
    if (!isAutosaving()) {/* FIXME
        foreach(KoView* view, views())
        static_cast<View *>(view)->selection()->emitCloseEditor(true);
        */
        emit closeEditor(true);
    }

    return DocBase::saveXML();
}


bool Doc::completeLoading(KoStore* store)
{
    debugSheets << "------------------------ COMPLETING --------------------";

    setModified(false);
    bool ok = map()->completeLoading(store);

    debugSheets << "------------------------ COMPLETION DONE --------------------";
    return ok;
}


void Doc::addIgnoreWordAllList(const QStringList & _lst)
{
    d->spellListIgnoreAll = _lst;
}

QStringList Doc::spellListIgnoreAll() const
{
    return d->spellListIgnoreAll;
}

void Doc::paintContent(QPainter& painter, const QRect& rect)
{
    paintContent(painter, rect, 0);
}

void Doc::paintContent(QPainter& painter, const QRect& rect, Sheet* _sheet)
{
    if (rect.isEmpty()) {
        return;
    }
    Sheet *const sheet = _sheet ? _sheet : d->map->sheet(0);

    const KoPageLayout pageLayout = sheet->printSettings()->pageLayout();
    QPixmap thumbnail(pageLayout.width, pageLayout.height);
    thumbnail.fill(Qt::white);

    SheetView sheetView(sheet);

    const qreal zoom = sheet->printSettings()->zoom();
    KoZoomHandler zoomHandler;
    zoomHandler.setZoom(zoom);
    sheetView.setViewConverter(&zoomHandler);

    sheetView.setPaintCellRange(sheet->print()->cellRange(1)); // first page

    QPainter pixmapPainter(&thumbnail);
    pixmapPainter.setClipRect(QRect(QPoint(0, 0), thumbnail.size()));
    sheetView.paintCells(pixmapPainter, QRect(0, 0, pageLayout.width, pageLayout.height), QPointF(0,0));

    // The pixmap gets scaled to fit the rectangle.
    painter.drawPixmap(rect & QRect(0, 0, 100, 100), thumbnail);
}

void Doc::updateAllViews()
{
    emit updateView();
}

void Doc::addIgnoreWordAll(const QString & word)
{
    if (d->spellListIgnoreAll.indexOf(word) == -1)
        d->spellListIgnoreAll.append(word);
}

void Doc::clearIgnoreWordAll()
{
    d->spellListIgnoreAll.clear();
}

void Doc::loadConfigFromFile()
{
    d->configLoadFromFile = true;
}

bool Doc::configLoadFromFile() const
{
    return d->configLoadFromFile;
}

void Doc::sheetAdded(Sheet* sheet)
{
#ifndef QT_NO_DBUS
    new SheetAdaptor(sheet);
    QString dbusPath('/' + sheet->map()->objectName() + '/' + sheet->objectName());
    if (sheet->parent() && !sheet->parent()->objectName().isEmpty()) {
        dbusPath.prepend('/' + sheet->parent()->objectName());
    }
    QDBusConnection::sessionBus().registerObject(dbusPath, sheet);
#endif
}

bool Doc::saveOdf(SavingContext &documentContext)
{
    /* don't pull focus away from the editor if this is just a background
       autosave */
    if (!isAutosaving()) {
        /*FIXME
        foreach(KoView* view, views())
            static_cast<View *>(view)->selection()->emitCloseEditor(true);
        */
        emit closeEditor(true);
    }

    return DocBase::saveOdf(documentContext);
}
