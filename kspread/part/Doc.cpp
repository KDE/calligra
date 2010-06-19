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

#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>

#include <QApplication>
#include <QFont>
#include <QTimer>
//Added by qt3to4:
#include <QList>
#include <QPainter>

#include <kstandarddirs.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <ktemporaryfile.h>

#include <KoApplication.h>
#include <KoDataCenterBase.h>
#include <KoDocumentInfo.h>
#include <KoMainWindow.h>
#include <KoOasisSettings.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoOdfReadStore.h>
#include <KoOdfWriteStore.h>
#include <KoShapeConfigFactoryBase.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeManager.h>
#include <KoShapeRegistry.h>
#include <KoStoreDevice.h>
#include <KoStyleStack.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <KoZoomHandler.h>
#include <KoShapeSavingContext.h>

#include "BindingManager.h"
#include "CalculationSettings.h"
#include "Canvas.h"
#include "DependencyManager.h"
#include "Factory.h"
#include "Formula.h"
#include "FunctionModuleRegistry.h"
#include "Functions.h"
#include "HeaderFooter.h"
#include "LoadingInfo.h"
#include "Localization.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "PrintSettings.h"
#include "RecalcManager.h"
#include "Selection.h"
#include "Sheet.h"
#include "SheetPrint.h"
#include "StyleManager.h"
#include "Util.h"
#include "View.h"
#include "SheetAccessModel.h"

// chart shape
#include "plugins/chartshape/ChartShape.h"
#include "chart/ChartDialog.h"

// ui
#include "ui/SheetView.h"

using namespace std;
using namespace KSpread;

static const int CURRENT_SYNTAX_VERSION = 1;
// Make sure an appropriate DTD is available in www/koffice/DTD if changing this value
static const char * CURRENT_DTD_VERSION = "1.2";

typedef QMap<QString, QDomElement> SavedDocParts;

class Doc::Private
{
public:
    Map *map;
    static QList<Doc*> s_docs;
    static int s_docId;

    // document properties
    bool configLoadFromFile : 1;
    QStringList spellListIgnoreAll;
    SavedDocParts savedDocParts;
    SheetAccessModel *sheetAccessModel;
};

/*****************************************************************************
 *
 * Doc
 *
 *****************************************************************************/

QList<Doc*> Doc::Private::s_docs;
int Doc::Private::s_docId = 0;

Doc::Doc(QWidget *parentWidget, QObject* parent, bool singleViewMode)
        : KoDocument(parentWidget, parent, singleViewMode)
        , d(new Private)
{
    d->map = new Map(this, CURRENT_SYNTAX_VERSION);
    // Document Url for FILENAME function and page header/footer.
    d->map->calculationSettings()->setFileName(url().prettyUrl());

    d->configLoadFromFile = false;

    documents().append(this);

    setComponentData(Factory::global(), false);
    setTemplateType("kspread_template");

    d->sheetAccessModel = new SheetAccessModel(d->map);

    // Init chart shape factory with KSpread's specific configuration panels.
    KoShapeFactoryBase *chartShape = KoShapeRegistry::instance()->value(ChartShapeId);
    if (chartShape) {
        QList<KoShapeConfigFactoryBase*> panels = ChartDialog::panels(d->map);
        chartShape->setOptionPanels(panels);
    }

    connect(d->map, SIGNAL(commandAdded(QUndoCommand *)),
            this, SLOT(addCommand(QUndoCommand *)));
    // Load the function modules.
    FunctionModuleRegistry::instance()->loadFunctionModules();
}

Doc::~Doc()
{
    //don't save config when kword is embedded into konqueror
    if (isReadWrite())
        saveConfig();

    delete d->map;
    delete d->sheetAccessModel;
    delete d;
}

QList<Doc*> Doc::documents()
{
    return Private::s_docs;
}

void Doc::setReadWrite(bool readwrite)
{
    map()->setReadWrite(readwrite);
    KoDocument::setReadWrite(readwrite);
}

void Doc::openTemplate(const KUrl& url)
{
    map()->loadingInfo()->setLoadTemplate(true);
    KoDocument::openTemplate(url);
    map()->deleteLoadingInfo();
    initConfig();
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

Map *Doc::map() const
{
    return d->map;
}

void Doc::saveConfig()
{
    if (isEmbedded() || !isReadWrite())
        return;
    KSharedConfigPtr config = Factory::global().config();
#ifdef KSPREAD_DOC_ZOOM
    config->group("Parameters").writeEntry("Zoom", zoomInPercent());
#endif // KSPREAD_DOC_ZOOM
}

void Doc::initConfig()
{
    KSharedConfigPtr config = Factory::global().config();

    const int page = config->group("KSpread Page Layout").readEntry("Default unit page", 0);
    setUnit(KoUnit((KoUnit::Unit) page));

#if 0 // UNDOREDOLIMIT
    const int undo = config->group("Misc").readEntry("UndoRedo", 30);
    setUndoRedoLimit(undo);
#endif

#ifdef KSPREAD_DOC_ZOOM
    const int zoom = config->group("Parameters").readEntry("Zoom", 100);
    setZoomAndResolution(zoom, KoGlobal::dpiX(), KoGlobal::dpiY());
#endif // KSPREAD_DOC_ZOOM
}

int Doc::syntaxVersion() const
{
    return d->map->syntaxVersion();
}

KoView* Doc::createViewInstance(QWidget* parent)
{
    return new View(parent, this);
}

bool Doc::saveChildren(KoStore* _store)
{
    return map()->saveChildren(_store);
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
    if (!isAutosaving()) {
        foreach(KoView* view, views())
        static_cast<View *>(view)->selection()->emitCloseEditor(true);
    }

    QDomDocument doc = KoDocument::createDomDocument("kspread", "spreadsheet", CURRENT_DTD_VERSION);
    QDomElement spread = doc.documentElement();
    spread.setAttribute("editor", "KSpread");
    spread.setAttribute("mime", "application/x-kspread");
    spread.setAttribute("syntaxVersion", CURRENT_SYNTAX_VERSION);

    if (!d->spellListIgnoreAll.isEmpty()) {
        QDomElement spellCheckIgnore = doc.createElement("SPELLCHECKIGNORELIST");
        spread.appendChild(spellCheckIgnore);
        for (QStringList::Iterator it = d->spellListIgnoreAll.begin(); it != d->spellListIgnoreAll.end(); ++it) {
            QDomElement spellElem = doc.createElement("SPELLCHECKIGNOREWORD");
            spellCheckIgnore.appendChild(spellElem);
            spellElem.setAttribute("word", *it);
        }
    }

    SavedDocParts::const_iterator iter = d->savedDocParts.constBegin();
    SavedDocParts::const_iterator end  = d->savedDocParts.constEnd();
    while (iter != end) {
        // save data we loaded in the beginning and which has no owner back to file
        spread.appendChild(iter.value());
        ++iter;
    }

    QDomElement e = map()->save(doc);
    spread.appendChild(e);

    setModified(false);

    return doc;
}

bool Doc::loadChildren(KoStore* _store)
{
    return map()->loadChildren(_store);
}

bool Doc::saveOdf(SavingContext &documentContext)
{
    ElapsedTime et("OpenDocument Saving", ElapsedTime::PrintOnlyTime);
    return saveOdfHelper(documentContext, SaveAll);
}

bool Doc::saveOdfHelper(SavingContext & documentContext, SaveFlag saveFlag,
                        QString* /*plainText*/)
{
    Q_UNUSED(saveFlag);
    KoStore * store = documentContext.odfStore.store();
    KoXmlWriter * manifestWriter = documentContext.odfStore.manifestWriter();

    /* don't pull focus away from the editor if this is just a background
       autosave */
    if (!isAutosaving()) {
        foreach(KoView* view, views())
        static_cast<View *>(view)->selection()->emitCloseEditor(true);
    }

    KoStoreDevice dev(store);
    KoGenStyles mainStyles;//for compile

    KoXmlWriter* contentWriter = documentContext.odfStore.contentWriter();

    KoXmlWriter* bodyWriter = documentContext.odfStore.bodyWriter();
    KoShapeSavingContext savingContext(*bodyWriter, mainStyles, documentContext.embeddedSaver);

    //todo fixme just add a element for testing saving content.xml
    bodyWriter->startElement("office:body");
    bodyWriter->startElement("office:spreadsheet");

    // Saving the map.
    map()->saveOdf(*contentWriter, savingContext);

    bodyWriter->endElement(); ////office:spreadsheet
    bodyWriter->endElement(); ////office:body

    // Done with writing out the contents to the tempfile, we can now write out the automatic styles
    mainStyles.saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, contentWriter);

    documentContext.odfStore.closeContentWriter();

    //add manifest line for content.xml
    manifestWriter->addManifestEntry("content.xml",  "text/xml");

    mainStyles.saveOdfStylesDotXml(store, manifestWriter);

    if (!store->open("settings.xml"))
        return false;

    KoXmlWriter* settingsWriter = KoOdfWriteStore::createOasisXmlWriter(&dev, "office:document-settings");
    settingsWriter->startElement("office:settings");
    settingsWriter->startElement("config:config-item-set");
    settingsWriter->addAttribute("config:name", "view-settings");

    saveUnitOdf(settingsWriter);

    saveOdfSettings(*settingsWriter);

    settingsWriter->endElement(); // config:config-item-set

    settingsWriter->startElement("config:config-item-set");
    settingsWriter->addAttribute("config:name", "configuration-settings");
    settingsWriter->addConfigItem("SpellCheckerIgnoreList", d->spellListIgnoreAll.join(","));
    settingsWriter->endElement(); // config:config-item-set
    settingsWriter->endElement(); // office:settings
    settingsWriter->endElement(); // Root:element
    settingsWriter->endDocument();
    delete settingsWriter;

    if (!store->close())
        return false;

    if (!savingContext.saveDataCenter(store, manifestWriter)) {
        return false;
    }

    manifestWriter->addManifestEntry("settings.xml", "text/xml");

    setModified(false);

    return true;
}

void Doc::loadOdfSettings(const KoXmlDocument&settingsDoc)
{
    KoOasisSettings settings(settingsDoc);
    KoOasisSettings::Items viewSettings = settings.itemSet("view-settings");
    if (!viewSettings.isNull()) {
        setUnit(KoUnit::unit(viewSettings.parseConfigItemString("unit")));
    }
    map()->loadOdfSettings(settings);
    loadOdfIgnoreList(settings);
}

void Doc::saveOdfSettings(KoXmlWriter &settingsWriter)
{
    settingsWriter.startElement("config:config-item-map-indexed");
    settingsWriter.addAttribute("config:name", "Views");
    settingsWriter.startElement("config:config-item-map-entry");
    map()->saveOdfSettings(settingsWriter);
    settingsWriter.endElement();
    settingsWriter.endElement();
}


void Doc::loadOdfIgnoreList(const KoOasisSettings& settings)
{
    KoOasisSettings::Items configurationSettings = settings.itemSet("configuration-settings");
    if (!configurationSettings.isNull()) {
        const QString ignorelist = configurationSettings.parseConfigItemString("SpellCheckerIgnoreList");
        //kDebug()<<" ignorelist :"<<ignorelist;
        d->spellListIgnoreAll = ignorelist.split(',', QString::SkipEmptyParts);
    }
}


bool Doc::loadOdf(KoOdfReadStore & odfStore)
{
    QTime dt;
    dt.start();

    emit sigProgress(0);
    d->spellListIgnoreAll.clear();

    KoXmlElement content = odfStore.contentDoc().documentElement();
    KoXmlElement realBody(KoXml::namedItemNS(content, KoXmlNS::office, "body"));
    if (realBody.isNull()) {
        setErrorMessage(i18n("Invalid OASIS OpenDocument file. No office:body tag found."));
        map()->deleteLoadingInfo();
        return false;
    }
    KoXmlElement body = KoXml::namedItemNS(realBody, KoXmlNS::office, "spreadsheet");

    if (body.isNull()) {
        kError(32001) << "No office:spreadsheet found!" << endl;
        KoXmlElement childElem;
        QString localName;
        forEachElement(childElem, realBody) {
            localName = childElem.localName();
        }
        if (localName.isEmpty())
            setErrorMessage(i18n("Invalid OASIS OpenDocument file. No tag found inside office:body."));
        else
            setErrorMessage(i18n("This document is not a spreadsheet, but %1. Please try opening it with the appropriate application." , KoDocument::tagNameToDocumentType(localName)));
        map()->deleteLoadingInfo();
        return false;
    }

    KoOdfLoadingContext context(odfStore.styles(), odfStore.store());

    // TODO check versions and mimetypes etc.

    // all <sheet:sheet> goes to workbook
    if (!map()->loadOdf(body, context)) {
        map()->deleteLoadingInfo();
        return false;
    }

    if (!odfStore.settingsDoc().isNull()) {
        loadOdfSettings(odfStore.settingsDoc());
    }
    initConfig();
    emit sigProgress(-1);

    //display loading time
    kDebug(36001) << "Loading took" << (float)(dt.elapsed()) / 1000.0 << " seconds";

    return true;
}

bool Doc::loadXML(const KoXmlDocument& doc, KoStore*)
{
    QTime dt;
    dt.start();

    emit sigProgress(0);
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
                      0, i18n("This document was created with a newer version of KSpread (syntax version: %1)\n"
                              "When you open it with this version of KSpread, some information may be lost.",map()->syntaxVersion()),
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

    emit sigProgress(5);

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

    emit sigProgress(40);
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

    emit sigProgress(85);

    KoXmlElement element(spread.firstChild().toElement());
    while (!element.isNull()) {
        QString tagName(element.tagName());

        if (tagName != "locale" && tagName != "map" && tagName != "styles"
                && tagName != "SPELLCHECKIGNORELIST" && tagName != "areaname"
                && tagName != "paper") {
            // belongs to a plugin, load it and save it for later use
            d->savedDocParts[ tagName ] = KoXml::asQDomElement(QDomDocument(), element);
        }

        element = element.nextSibling().toElement();
    }

    emit sigProgress(90);
    initConfig();
    emit sigProgress(-1);

    kDebug(36001) << "Loading took" << (float)(dt.elapsed()) / 1000.0 << " seconds";

    emit sig_refreshView();

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
    hleft   = hleft.replace("<table>", "<sheet>");
    hcenter = hcenter.replace("<table>", "<sheet>");
    hright  = hright.replace("<table>", "<sheet>");
    fleft   = fleft.replace("<table>", "<sheet>");
    fcenter = fcenter.replace("<table>", "<sheet>");
    fright  = fright.replace("<table>", "<sheet>");

    foreach(Sheet* sheet, map()->sheetList()) {
        sheet->print()->headerFooter()->setHeadFootLine(hleft, hcenter, hright,
                                                        fleft, fcenter, fright);
    }
}

bool Doc::completeLoading(KoStore* store)
{
    kDebug(36001) << "------------------------ COMPLETING --------------------";

    setModified(false);
    bool ok = map()->completeLoading(store);

  kDebug(36001) <<"------------------------ COMPLETION DONE --------------------";
    return ok;
}


bool Doc::docData(QString const & xmlTag, QDomElement & data)
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

void Doc::newZoomAndResolution(bool updateViews, bool /*forPrint*/)
{
    /*    layout();
        updateAllFrames();*/
    if (updateViews) {
        emit sig_refreshView();
    }
}

void Doc::paintContent(QPainter& painter, const QRect& rect)
{
    paintContent(painter, rect, map()->sheet(0), false);
}

void Doc::paintContent(QPainter& painter, const QRect& rect, Sheet* sheet, bool drawCursor)
{
    Q_UNUSED(drawCursor);

    if (rect.isEmpty()) {
        return;
    }
    const KoPageLayout pageLayout = sheet->printSettings()->pageLayout();
    QPixmap thumbnail(pageLayout.width, pageLayout.height);
    thumbnail.fill(Qt::white);

    SheetView sheetView(sheet);
    sheetView.setPaintDevice(&thumbnail);

    const qreal zoom = sheet->printSettings()->zoom();
    KoZoomHandler zoomHandler;
    zoomHandler.setZoom(zoom);
    sheetView.setViewConverter(&zoomHandler);

    sheetView.setPaintCellRange(sheet->print()->cellRange(1)); // first page

    QPainter pixmapPainter(&thumbnail);
    sheetView.paintCells(&thumbnail, pixmapPainter, QRect(0, 0, pageLayout.width, pageLayout.height), QPointF(0,0));

    // The pixmap gets scaled to fit the rectangle.
    painter.drawPixmap(rect & QRect(0, 0, 100, 100), thumbnail);
}

void Doc::paintUpdates()
{
    foreach(KoView* view, views()) {
        static_cast<View *>(view)->paintUpdates();
    }
}

void Doc::paintRegion(QPainter &painter, const QRectF &viewRegion,
                      View* view, const QRect &cellRegion, const Sheet* sheet)
{
    // cellRegion has cell coordinates (col,row) while viewRegion has
    // world coordinates.  cellRegion is the cells to update and
    // viewRegion is the area actually onscreen.

    if (cellRegion.left() <= 0 || cellRegion.top() <= 0)
        return;

    const QRectF viewRegionF(viewRegion.left(), viewRegion.right(), viewRegion.width(), viewRegion.height());

    // Get the world coordinates of the upper left corner of the
    // cellRegion The view is 0, when cellRegion is called from
    // paintContent, which itself is only called, when we should paint
    // the output for INACTIVE embedded view.  If inactive embedded,
    // then there is no view and we alwas start at top/left, so the
    // offset is 0.
    //
    QPointF topLeft;
    if (view == 0)   //Most propably we are embedded and inactive, so no offset
        topLeft = QPointF(sheet->columnPosition(cellRegion.left()),
                          sheet->rowPosition(cellRegion.top()));
    else
        topLeft = QPointF(sheet->columnPosition(cellRegion.left()) - view->canvasWidget()->xOffset(),
                          sheet->rowPosition(cellRegion.top()) - view->canvasWidget()->yOffset());

    SheetView sheetView(sheet);   // FIXME Stefan: make member, otherwise cache lost
    if (view) {
        sheetView.setPaintDevice(view->canvasWidget());
        sheetView.setViewConverter(view->zoomHandler());
    }
    sheetView.setPaintCellRange(cellRegion);
    sheetView.paintCells(view ? view->canvasWidget() : 0, painter, viewRegionF, topLeft);
}

void Doc::refreshInterface()
{
    emit sig_refreshView();
}

void Doc::updateBorderButton()
{
    foreach(KoView* view, views())
    static_cast<View*>(view)->updateBorderButton();
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

void Doc::addView(KoView *_view)
{
    KoDocument::addView(_view);
    foreach(KoView* view, views())
    static_cast<View*>(view)->selection()->emitCloseEditor(true);
}

void Doc::loadConfigFromFile()
{
    d->configLoadFromFile = true;
}

bool Doc::configLoadFromFile() const
{
    return d->configLoadFromFile;
}

void Doc::repaint(const QRectF& rect)
{
    QRectF r;
    foreach(KoView* koview, views()) {
        const View* view = static_cast<View*>(koview);
        Canvas* canvas = view->canvasWidget();

        r = view->zoomHandler()->documentToView(rect);
        r.translate(-canvas->xOffset() * view->zoomHandler()->zoomedResolutionX(),
                    -canvas->yOffset() * view->zoomHandler()->zoomedResolutionY());
        canvas->update(r.toRect());
    }
}

SheetAccessModel *Doc::sheetAccessModel() const
{
    return d->sheetAccessModel;
}

#if 0 // UNDOREDOLIMIT
int Doc::undoRedoLimit() const
{
    return d->commandHistory->undoLimit();
}

void Doc::setUndoRedoLimit(int val)
{
    d->commandHistory->setUndoLimit(val);
    d->commandHistory->setRedoLimit(val);
}
#endif

#include "Doc.moc"

