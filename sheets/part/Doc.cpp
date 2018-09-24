/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2007 Thorsten Zachmann <zachmann@kde.org>
   Copyright 2005-2006 Inge Wallin <inge@lysator.liu.se>
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2000-2002 Laurent Montel <montel@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2002 Phillip Mueller <philipp.mueller@gmx.de>
   Copyright 2000 Werner Trobin <trobin@kde.org>
   Copyright 1999-2000 Simon Hausmann <hausmann@kde.org>
   Copyright 1999 David Faure <faure@kde.org>
   Copyright 1998-2000 Torben Weis <weis@kde.org>

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
    SavedDocParts savedDocParts;
    SheetAccessModel *sheetAccessModel;
    KoDocumentResourceManager *resourceManager;
};

// Make sure an appropriate DTD is available in www/calligra/DTD if changing this value
static const char CURRENT_DTD_VERSION[] = "1.2";

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
    connect(d->map, SIGNAL(sheetAdded(Sheet*)), this, SLOT(sheetAdded(Sheet*)));

#ifndef QT_NO_DBUS
    new MapAdaptor(d->map);
    QDBusConnection::sessionBus().registerObject('/' + objectName() + '/' + d->map->objectName(), d->map);
#endif

    // Init chart shape factory with Calligra Sheets' specific configuration panels.
    KoShapeFactoryBase *chartShape = KoShapeRegistry::instance()->value(ChartShapeId);
    if (chartShape) {
        QList<KoShapeConfigFactoryBase*> panels = ChartDialog::panels(d->map);
        chartShape->setOptionPanels(panels);
    } else {
        warnSheets << "chart shape factory not found";
    }

    connect(d->map, SIGNAL(commandAdded(KUndo2Command*)),
            this, SLOT(addCommand(KUndo2Command*)));

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

    QDomDocument doc = KoDocument::createDomDocument("tables", "spreadsheet", CURRENT_DTD_VERSION);
    QDomElement spread = doc.documentElement();
    spread.setAttribute("editor", "Calligra Sheets");
    spread.setAttribute("mime", "application/x-kspread");
    spread.setAttribute("syntaxVersion", QString::number(CURRENT_SYNTAX_VERSION));

    if (!d->spellListIgnoreAll.isEmpty()) {
        QDomElement spellCheckIgnore = doc.createElement("SPELLCHECKIGNORELIST");
        spread.appendChild(spellCheckIgnore);
        for (QStringList::ConstIterator it = d->spellListIgnoreAll.constBegin(); it != d->spellListIgnoreAll.constEnd(); ++it) {
            QDomElement spellElem = doc.createElement("SPELLCHECKIGNOREWORD");
            spellCheckIgnore.appendChild(spellElem);
            spellElem.setAttribute("word", *it);
        }
    }

    SavedDocParts::const_iterator iter = d->savedDocParts.constBegin();
    SavedDocParts::const_iterator end  = d->savedDocParts.constEnd();
    while (iter != end) {
        // save data we loaded in the beginning and which has no owner back to file
        spread.appendChild(iter.value().documentElement());
        ++iter;
    }

    QDomElement e = map()->save(doc);
/*FIXME
        // Save visual info for the first view, such as active sheet and active cell
        // It looks like a hack, but reopening a document creates only one view anyway (David)
        View *const view = static_cast<View*>(views().first());
        Canvas *const canvas = view->canvasWidget();
        e.setAttribute("activeTable",  canvas->activeSheet()->sheetName());
        e.setAttribute("markerColumn", QString::number(view->selection()->marker().x()));
        e.setAttribute("markerRow",    QString::number(view->selection()->marker().y()));
        e.setAttribute("xOffset",      QString::number(canvas->xOffset()));
        e.setAttribute("yOffset",      QString::number(canvas->yOffset()));
*/
    spread.appendChild(e);

    setModified(false);

    return doc;
}

bool Doc::loadChildren(KoStore* _store)
{
    return map()->loadChildren(_store);
}


bool Doc::loadXML(const KoXmlDocument& doc, KoStore*)
{
    QPointer<KoUpdater> updater;
    if (progressUpdater()) {
        updater = progressUpdater()->startSubtask(1, "KSpread::Doc::loadXML");
        updater->setProgress(0);
    }

    d->spellListIgnoreAll.clear();
    // <spreadsheet>
    KoXmlElement spread = doc.documentElement();

    if (spread.attribute("mime") != "application/x-kspread" && spread.attribute("mime") != "application/vnd.kde.kspread") {
        setErrorMessage(i18n("Invalid document. Expected mimetype application/x-kspread or application/vnd.kde.kspread, got %1" , spread.attribute("mime")));
        return false;
    }

    bool ok = false;
    int version = spread.attribute("syntaxVersion").toInt(&ok);
    map()->setSyntaxVersion(ok ? version : 0);
    if (map()->syntaxVersion() > CURRENT_SYNTAX_VERSION) {
        int ret = KMessageBox::warningContinueCancel(
                      0, i18n("This document was created with a newer version of Calligra Sheets (syntax version: %1)\n"
                              "When you open it with this version of Calligra Sheets, some information may be lost.", map()->syntaxVersion()),
                      i18n("File Format Mismatch"), KStandardGuiItem::cont());
        if (ret == KMessageBox::Cancel) {
            setErrorMessage("USER_CANCELED");
            return false;
        }
    }

    // <locale>
    KoXmlElement loc = spread.namedItem("locale").toElement();
    if (!loc.isNull())
        static_cast<Localization*>(map()->calculationSettings()->locale())->load(loc);

    if (updater) updater->setProgress(5);

    KoXmlElement defaults = spread.namedItem("defaults").toElement();
    if (!defaults.isNull()) {
        double dim = defaults.attribute("row-height").toDouble(&ok);
        if (!ok)
            return false;
        map()->setDefaultRowHeight(dim);

        dim = defaults.attribute("col-width").toDouble(&ok);

        if (!ok)
            return false;

        map()->setDefaultColumnWidth(dim);
    }

    KoXmlElement ignoreAll = spread.namedItem("SPELLCHECKIGNORELIST").toElement();
    if (!ignoreAll.isNull()) {
        KoXmlElement spellWord = spread.namedItem("SPELLCHECKIGNORELIST").toElement();

        spellWord = spellWord.firstChild().toElement();
        while (!spellWord.isNull()) {
            if (spellWord.tagName() == "SPELLCHECKIGNOREWORD") {
                d->spellListIgnoreAll.append(spellWord.attribute("word"));
            }
            spellWord = spellWord.nextSibling().toElement();
        }
    }

    if (updater) updater->setProgress(40);
    // In case of reload (e.g. from konqueror)
    qDeleteAll(map()->sheetList());
    map()->sheetList().clear();

    KoXmlElement styles = spread.namedItem("styles").toElement();
    if (!styles.isNull()) {
        if (!map()->styleManager()->loadXML(styles)) {
            setErrorMessage(i18n("Styles cannot be loaded."));
            return false;
        }
    }

    // <map>
    KoXmlElement mymap = spread.namedItem("map").toElement();
    if (mymap.isNull()) {
        setErrorMessage(i18n("Invalid document. No map tag."));
        return false;
    }
    if (!map()->loadXML(mymap)) {
        return false;
    }

    // named areas
    const KoXmlElement areaname = spread.namedItem("areaname").toElement();
    if (!areaname.isNull())
        map()->namedAreaManager()->loadXML(areaname);

    //Backwards compatibility with older versions for paper layout
    if (map()->syntaxVersion() < 1) {
        KoXmlElement paper = spread.namedItem("paper").toElement();
        if (!paper.isNull()) {
            loadPaper(paper);
        }
    }

    if (updater) updater->setProgress(85);

    KoXmlElement element(spread.firstChild().toElement());
    while (!element.isNull()) {
        QString tagName(element.tagName());

        if (tagName != "locale" && tagName != "map" && tagName != "styles"
                && tagName != "SPELLCHECKIGNORELIST" && tagName != "areaname"
                && tagName != "paper") {
            // belongs to a plugin, load it and save it for later use
            QDomDocument doc;
            KoXml::asQDomElement(doc, element);
            d->savedDocParts[ tagName ] = doc;
        }

        element = element.nextSibling().toElement();
    }

    if (updater) updater->setProgress(90);
    initConfig();
    if (updater) updater->setProgress(100);

    return true;
}

void Doc::loadPaper(KoXmlElement const & paper)
{
    KoPageLayout pageLayout;
    pageLayout.format = KoPageFormat::formatFromString(paper.attribute("format"));
    pageLayout.orientation = (paper.attribute("orientation")  == "Portrait")
                             ? KoPageFormat::Portrait : KoPageFormat::Landscape;

    // <borders>
    KoXmlElement borders = paper.namedItem("borders").toElement();
    if (!borders.isNull()) {
        pageLayout.leftMargin   = MM_TO_POINT(borders.attribute("left").toFloat());
        pageLayout.rightMargin  = MM_TO_POINT(borders.attribute("right").toFloat());
        pageLayout.topMargin    = MM_TO_POINT(borders.attribute("top").toFloat());
        pageLayout.bottomMargin = MM_TO_POINT(borders.attribute("bottom").toFloat());
    }

    //apply to all sheet
    foreach(Sheet* sheet, map()->sheetList()) {
        sheet->printSettings()->setPageLayout(pageLayout);
    }

    QString hleft, hright, hcenter;
    QString fleft, fright, fcenter;
    // <head>
    KoXmlElement head = paper.namedItem("head").toElement();
    if (!head.isNull()) {
        KoXmlElement left = head.namedItem("left").toElement();
        if (!left.isNull())
            hleft = left.text();
        KoXmlElement center = head.namedItem("center").toElement();
        if (!center.isNull())
            hcenter = center.text();
        KoXmlElement right = head.namedItem("right").toElement();
        if (!right.isNull())
            hright = right.text();
    }
    // <foot>
    KoXmlElement foot = paper.namedItem("foot").toElement();
    if (!foot.isNull()) {
        KoXmlElement left = foot.namedItem("left").toElement();
        if (!left.isNull())
            fleft = left.text();
        KoXmlElement center = foot.namedItem("center").toElement();
        if (!center.isNull())
            fcenter = center.text();
        KoXmlElement right = foot.namedItem("right").toElement();
        if (!right.isNull())
            fright = right.text();
    }
    //The macro "<sheet>" formerly was typed as "<table>"
    hleft.replace("<table>", "<sheet>");
    hcenter.replace("<table>", "<sheet>");
    hright.replace("<table>", "<sheet>");
    fleft.replace("<table>", "<sheet>");
    fcenter.replace("<table>", "<sheet>");
    fright.replace("<table>", "<sheet>");

    foreach(Sheet* sheet, map()->sheetList()) {
        sheet->print()->headerFooter()->setHeadFootLine(hleft, hcenter, hright,
                fleft, fcenter, fright);
    }
}

bool Doc::completeLoading(KoStore* store)
{
    debugSheets << "------------------------ COMPLETING --------------------";

    setModified(false);
    bool ok = map()->completeLoading(store);

    debugSheets << "------------------------ COMPLETION DONE --------------------";
    return ok;
}


bool Doc::docData(QString const & xmlTag, QDomDocument & data)
{
    SavedDocParts::iterator iter = d->savedDocParts.find(xmlTag);
    if (iter == d->savedDocParts.end())
        return false;
    data = iter.value();
    d->savedDocParts.erase(iter);
    return true;
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
