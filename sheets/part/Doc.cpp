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
#include "Factory.h"
#include "chart/ChartDialog.h"

#include <QPainter>

#include <KConfigGroup>
#include <KSharedConfig>

#include <KoChartInterface.h>
#include <KoComponentData.h>
#include <KoShapeRegistry.h>
#include <KoUnit.h>
#include <KoZoomHandler.h>

#include "core/Map.h"
#include "core/PrintSettings.h"
#include "core/Sheet.h"
#include "core/SheetPrint.h"
#include "core/StyleManager.h"
#include "engine/FunctionModuleRegistry.h"
#include "engine/SheetsDebug.h"

// ui
#include "ui/SheetView.h"

// D-Bus
#ifdef WITH_QTDBUS
#include "MapAdaptor.h"
#include "SheetAdaptor.h"
#include <QDBusConnection>
#endif

using namespace std;
using namespace Calligra::Sheets;

class Q_DECL_HIDDEN Doc::Private
{
public:
    Map *map;
    static QList<Doc *> s_docs;
    static int s_docId;

    // document properties
    bool configLoadFromFile : 1;
    SheetAccessModel *sheetAccessModel;
    KoDocumentResourceManager *resourceManager;
};

/*****************************************************************************
 *
 * Doc
 *
 *****************************************************************************/

QList<Doc *> Doc::Private::s_docs;
int Doc::Private::s_docId = 0;

Doc::Doc(KoPart *part)
    : DocBase(part)
    , dd(new Private)
{
    Q_ASSERT(part);
    connect(map(), &Map::sheetAdded, this, &Doc::sheetAdded);
    dd->configLoadFromFile = false;

#ifdef WITH_QTDBUS
    new MapAdaptor(map());
    QDBusConnection::sessionBus().registerObject('/' + objectName() + '/' + map()->objectName(), map());
#endif

    KoShapeRegistry *registry = KoShapeRegistry::instance();
    foreach (const QString &id, registry->keys()) {
        KoShapeFactoryBase *shapeFactory = registry->value(id);
        shapeFactory->newDocumentResourceManager(resourceManager());
    }

    // Init chart shape factory with Calligra Sheets' specific configuration panels.
    KoShapeFactoryBase *chartShape = KoShapeRegistry::instance()->value(ChartShapeId);
    if (chartShape) {
        QList<KoShapeConfigFactoryBase *> panels = ChartDialog::panels(map());
        chartShape->setOptionPanels(panels);
    } else {
        warnSheets << "chart shape factory not found";
    }

    connect(map(), &Map::commandAdded, this, &KoDocument::addCommand);

    // Load the function modules.
    FunctionModuleRegistry::instance()->loadFunctionModules();
}

Doc::~Doc()
{
    // don't save config when words is embedded into konqueror
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

bool Doc::completeSaving(KoStore *_store)
{
    Q_UNUSED(_store);
    return true;
}

QDomDocument Doc::saveXML()
{
    /* don't pull focus away from the editor if this is just a background
       autosave */
    if (!isAutosaving()) { /* FIXME
         foreach(KoView* view, views())
         static_cast<View *>(view)->selection()->emitCloseEditor(true);
         */
        Q_EMIT closeEditor(true);
    }

    return DocBase::saveXML();
}

bool Doc::completeLoading(KoStore *store)
{
    debugSheets << "------------------------ COMPLETING --------------------";

    setModified(false);
    bool ok = map()->completeLoading(store);

    debugSheets << "------------------------ COMPLETION DONE --------------------";
    return ok;
}

void Doc::addIgnoreWordAllList(const QStringList &_lst)
{
    setSpellListIgnoreAll(_lst);
}

void Doc::paintContent(QPainter &painter, const QRect &rect)
{
    paintContent(painter, rect, nullptr);
}

void Doc::paintContent(QPainter &painter, const QRect &rect, Sheet *_sheet)
{
    if (rect.isEmpty()) {
        return;
    }
    Sheet *firstSheet = dynamic_cast<Sheet *>(map()->sheet(0));
    Sheet *const sheet = _sheet ? _sheet : firstSheet;

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
    sheetView.paintCells(pixmapPainter, QRect(0, 0, pageLayout.width, pageLayout.height), QPointF(0, 0));

    // The pixmap gets scaled to fit the rectangle.
    painter.drawPixmap(rect & QRect(0, 0, 100, 100), thumbnail);
}

void Doc::updateAllViews()
{
    Q_EMIT updateView();
}

void Doc::addIgnoreWordAll(const QString &word)
{
    QStringList lst = spellListIgnoreAll();
    if (lst.indexOf(word) >= 0)
        return;

    lst.append(word);
    setSpellListIgnoreAll(lst);
}

void Doc::clearIgnoreWordAll()
{
    QStringList lst;
    setSpellListIgnoreAll(lst);
}

void Doc::loadConfigFromFile()
{
    dd->configLoadFromFile = true;
}

bool Doc::configLoadFromFile() const
{
    return dd->configLoadFromFile;
}

void Doc::sheetAdded(SheetBase *sheet)
{
#ifdef WITH_QTDBUS
    Sheet *fullSheet = dynamic_cast<Sheet *>(sheet);
    if (!fullSheet)
        return;
    new SheetAdaptor(fullSheet);
    QString dbusPath('/' + fullSheet->map()->objectName() + '/' + fullSheet->objectName());
    if (fullSheet->parent() && !fullSheet->parent()->objectName().isEmpty()) {
        dbusPath.prepend('/' + fullSheet->parent()->objectName());
    }
    QDBusConnection::sessionBus().registerObject(dbusPath, fullSheet);
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
        Q_EMIT closeEditor(true);
    }

    return DocBase::saveOdf(documentContext);
}
