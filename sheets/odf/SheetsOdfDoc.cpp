/* This file is part of the KDE project
   Copyright 1998-2016 The Calligra Team <calligra-devel@kde.org>
   Copyright 2016 Tomas Mecir <mecirt@gmail.com>
   Copyright 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
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

#include "SheetsOdf.h"
#include "SheetsOdfPrivate.h"

#include "DocBase.h"
#include "BindingModel.h"
#include "CalculationSettings.h"
#include "Map.h"
#include "SheetsDebug.h"
#include "SheetAccessModel.h"
#include "calligra_sheets_limits.h"

#include <KoGenStyles.h>
#include <KoOdfReadStore.h>
#include <KoOdfWriteStore.h>
#include <KoProgressUpdater.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoUnit.h>
#include <KoUpdater.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>

#include <KCodecs>
#include <QBuffer>

// This file contains functionality to load/save a DocBase

namespace Calligra {
namespace Sheets {

namespace Odf {
    void loadDocSettings(DocBase *doc, const KoXmlDocument &settingsDoc);
    void loadDocIgnoreList(DocBase *doc, const KoOasisSettings& settings);
    void saveSettings(DocBase *doc, KoXmlWriter &settingsWriter);
};


bool Odf::loadDocument(DocBase *doc, KoOdfReadStore &odfStore)
{
    QPointer<KoUpdater> updater;
    if (doc->progressUpdater()) {
        updater = doc->progressUpdater()->startSubtask(1, "Calligra::Sheets::Odf::loadDocument");
        updater->setProgress(0);
    }

    doc->setSpellListIgnoreAll(QStringList());

    KoXmlElement content = odfStore.contentDoc().documentElement();
    KoXmlElement realBody(KoXml::namedItemNS(content, KoXmlNS::office, "body"));
    if (realBody.isNull()) {
        doc->setErrorMessage(i18n("Invalid OASIS OpenDocument file. No office:body tag found."));
        doc->map()->deleteLoadingInfo();
        return false;
    }
    KoXmlElement body = KoXml::namedItemNS(realBody, KoXmlNS::office, "spreadsheet");

    if (body.isNull()) {
        errorSheetsODF << "No office:spreadsheet found!" << endl;
        KoXmlElement childElem;
        QString localName;
        forEachElement(childElem, realBody) {
            localName = childElem.localName();
        }
        if (localName.isEmpty())
            doc->setErrorMessage(i18n("Invalid OASIS OpenDocument file. No tag found inside office:body."));
        else
            doc->setErrorMessage(i18n("This document is not a spreadsheet, but %1. Please try opening it with the appropriate application." , KoDocument::tagNameToDocumentType(localName)));
        doc->map()->deleteLoadingInfo();
        return false;
    }

    // Document Url for FILENAME function and page header/footer.
    doc->map()->calculationSettings()->setFileName(doc->url().toDisplayString());

    KoOdfLoadingContext context(odfStore.styles(), odfStore.store());

    // TODO check versions and mimetypes etc.

    // all <sheet:sheet> goes to workbook
    if (!loadMap(doc->map(), body, context)) {
        doc->map()->deleteLoadingInfo();
        return false;
    }

    if (!odfStore.settingsDoc().isNull()) {
        loadDocSettings(doc, odfStore.settingsDoc());
    }
    doc->initConfig();

    //update plugins that rely on bindings, as loading order can mess up the data of the plugins
    SheetAccessModel* sheetModel = doc->sheetAccessModel();
    QList< Sheet* > sheets = doc->map()->sheetList();
    Q_FOREACH( Sheet* sheet, sheets ){
        // This region contains the entire sheet
        const QRect region (0, 0, KS_colMax - 1, KS_rowMax - 1);
        QModelIndex index = sheetModel->index( 0, doc->map()->indexOf( sheet ) );
          QVariant bindingModelValue = sheetModel->data( index , Qt::DisplayRole );
          BindingModel *curBindingModel = dynamic_cast< BindingModel* >( qvariant_cast< QPointer< QAbstractItemModel > >( bindingModelValue ).data() );
          if ( curBindingModel ){
              curBindingModel->emitDataChanged( region );
          }
    }

    if (updater) updater->setProgress(100);

    return true;
}

void Odf::loadDocSettings(DocBase *doc, const KoXmlDocument &settingsDoc)
{
    KoOasisSettings settings(settingsDoc);
    KoOasisSettings::Items viewSettings = settings.itemSet("view-settings");
    if (!viewSettings.isNull()) {
        doc->setUnit(KoUnit::fromSymbol(viewSettings.parseConfigItemString("unit")));
    }
    loadMapSettings(doc->map(), settings);
    loadDocIgnoreList(doc, settings);
}

void Odf::loadDocIgnoreList(DocBase *doc, const KoOasisSettings& settings)
{
    KoOasisSettings::Items configurationSettings = settings.itemSet("configuration-settings");
    if (!configurationSettings.isNull()) {
        const QString ignorelist = configurationSettings.parseConfigItemString("SpellCheckerIgnoreList");
        //debugSheets<<" ignorelist :"<<ignorelist;
        doc->setSpellListIgnoreAll (ignorelist.split(',', QString::SkipEmptyParts));
    }
}

bool Odf::saveDocument(DocBase *doc, KoDocument::SavingContext &documentContext)
{
    KoStore * store = documentContext.odfStore.store();
    KoXmlWriter * manifestWriter = documentContext.odfStore.manifestWriter();

    KoStoreDevice dev(store);
    KoGenStyles mainStyles;//for compile

    KoXmlWriter* contentWriter = documentContext.odfStore.contentWriter();
    if (!contentWriter) return false;

    // Document Url for FILENAME function and page header/footer.
    doc->map()->calculationSettings()->setFileName(doc->url().toDisplayString());

    KoXmlWriter* bodyWriter = documentContext.odfStore.bodyWriter();
    KoShapeSavingContext savingContext(*bodyWriter, mainStyles, documentContext.embeddedSaver);

    //todo fixme just add a element for testing saving content.xml
    bodyWriter->startElement("office:body");
    bodyWriter->startElement("office:spreadsheet");

    // Saving the map.
    saveMap(doc->map(), *bodyWriter, savingContext);

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

    doc->saveUnitOdf(settingsWriter);

    saveSettings(doc, *settingsWriter);

    settingsWriter->endElement(); // config:config-item-set

    settingsWriter->startElement("config:config-item-set");
    settingsWriter->addAttribute("config:name", "configuration-settings");
    settingsWriter->addConfigItem("SpellCheckerIgnoreList", doc->spellListIgnoreAll().join(","));
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

    doc->setModified(false);

    return true;
}

void Odf::saveSettings(DocBase *doc, KoXmlWriter &settingsWriter)
{
    settingsWriter.startElement("config:config-item-map-indexed");
    settingsWriter.addAttribute("config:name", "Views");
    settingsWriter.startElement("config:config-item-map-entry");
    settingsWriter.addConfigItem("ViewId", QString::fromLatin1("View1"));
    //<config:config-item-map-named config:name="Tables">
    settingsWriter.startElement("config:config-item-map-named");
    settingsWriter.addAttribute("config:name", "Tables");
    foreach (Sheet *sheet, doc->map()->sheetList()) {
        settingsWriter.startElement("config:config-item-map-entry");
        settingsWriter.addAttribute("config:name", sheet->sheetName());
        saveSheetSettings(sheet, settingsWriter);
        settingsWriter.endElement();
    }
    settingsWriter.endElement();
    settingsWriter.endElement();
    settingsWriter.endElement();
}

void Odf::loadProtection(ProtectableObject *prot, const KoXmlElement& element)
{
    if (!element.hasAttributeNS(KoXmlNS::table, "protection-key")) return;
    QString p = element.attributeNS(KoXmlNS::table, "protection-key", QString());
    if (p.isNull()) return;

    QByteArray str(p.toUtf8());
    debugSheetsODF <<"Decoding password:" << str;
    prot->setProtected(KCodecs::base64Decode(str));
}

bool Odf::paste(QBuffer &buffer, Map *map)
{
    KoStore * store = KoStore::createStore(&buffer, KoStore::Read);

    KoOdfReadStore odfStore(store); // does not delete the store on destruction
    KoXmlDocument doc;
    QString errorMessage;
    bool ok = odfStore.loadAndParse("content.xml", doc, errorMessage);
    if (!ok) {
        errorSheetsODF << "Error parsing content.xml: " << errorMessage << endl;
    delete store;
        return false;
    }

    KoOdfStylesReader stylesReader;
    KoXmlDocument stylesDoc;
    (void)odfStore.loadAndParse("styles.xml", stylesDoc, errorMessage);
    // Load styles from style.xml
    stylesReader.createStyleMap(stylesDoc, true);
    // Also load styles from content.xml
    stylesReader.createStyleMap(doc, false);

    // from KSpreadDoc::loadOdf:
    KoXmlElement content = doc.documentElement();
    KoXmlElement realBody(KoXml::namedItemNS(content, KoXmlNS::office, "body"));
    if (realBody.isNull()) {
        debugSheetsUI << "Invalid OASIS OpenDocument file. No office:body tag found.";
        delete store;
        return false;
    }
    KoXmlElement body = KoXml::namedItemNS(realBody, KoXmlNS::office, "spreadsheet");

    if (body.isNull()) {
        errorSheetsODF << "No office:spreadsheet found!" << endl;
        delete store;
        return false;
    }

    KoOdfLoadingContext context(stylesReader, store);
    Q_ASSERT(!stylesReader.officeStyle().isNull());

    // all <sheet:sheet> goes to workbook
    bool result = loadMap(map, body, context);

    delete store;

    return result;
}

void Odf::loadCalculationSettings(CalculationSettings *settings, const KoXmlElement& body)
{
    KoXmlNode xmlsettings = KoXml::namedItemNS(body, KoXmlNS::table, "calculation-settings");
    debugSheets << "Calculation settings found?" << !xmlsettings.isNull();
    if (!xmlsettings.isNull()) {
        KoXmlElement element = xmlsettings.toElement();
        if (element.hasAttributeNS(KoXmlNS::table,  "case-sensitive")) {
            settings->setCaseSensitiveComparisons(Qt::CaseSensitive);
            QString value = element.attributeNS(KoXmlNS::table, "case-sensitive", "true");
            if (value == "false")
                settings->setCaseSensitiveComparisons(Qt::CaseInsensitive);
        } else if (element.hasAttributeNS(KoXmlNS::table, "precision-as-shown")) {
            settings->setPrecisionAsShown(false);
            QString value = element.attributeNS(KoXmlNS::table, "precision-as-shown", "false");
            if (value == "true")
                settings->setPrecisionAsShown(true);
        } else if (element.hasAttributeNS(KoXmlNS::table, "search-criteria-must-apply-to-whole-cell")) {
            settings->setWholeCellSearchCriteria(true);
            QString value = element.attributeNS(KoXmlNS::table, "search-criteria-must-apply-to-whole-cell", "true");
            if (value == "false")
                settings->setWholeCellSearchCriteria(false);
        } else if (element.hasAttributeNS(KoXmlNS::table, "automatic-find-labels")) {
            settings->setAutomaticFindLabels(true);
            QString value = element.attributeNS(KoXmlNS::table, "automatic-find-labels", "true");
            if (value == "false")
                settings->setAutomaticFindLabels(false);
        } else if (element.hasAttributeNS(KoXmlNS::table, "use-regular-expressions")) {
            settings->setUseRegularExpressions(true);
            QString value = element.attributeNS(KoXmlNS::table, "use-regular-expressions", "true");
            if (value == "false")
                settings->setUseRegularExpressions(false);
        } else if (element.hasAttributeNS(KoXmlNS::table, "use-wildcards")) {
            settings->setUseWildcards(false);
            QString value = element.attributeNS(KoXmlNS::table, "use-wildcards", "false");
            if (value == "true")
                settings->setUseWildcards(true);
        } else if (element.hasAttributeNS(KoXmlNS::table, "null-year")) {
            settings->setReferenceYear(1930);
            QString value = element.attributeNS(KoXmlNS::table, "null-year", "1930");
            if (!value.isEmpty() && value != "1930") {
                bool ok;
                int refYear = value.toInt(&ok);
                if (ok)
                    settings->setReferenceYear(refYear);
            }
        }

        forEachElement(element, xmlsettings) {
            if (element.namespaceURI() != KoXmlNS::table)
                continue;
            else if (element.tagName() ==  "null-date") {
                settings->setReferenceDate(QDate(1899, 12, 30));
                QString valueType = element.attributeNS(KoXmlNS::table, "value-type", "date");
                if (valueType == "date") {
                    QString value = element.attributeNS(KoXmlNS::table, "date-value", "1899-12-30");
                    QDate date = QDate::fromString(value, Qt::ISODate);
                    if (date.isValid())
                        settings->setReferenceDate(date);
                } else {
                    debugSheets << "CalculationSettings: Error on loading null date."
                    << "Value type """ << valueType << """ not handled"
                    << ", falling back to default." << endl;
                    // NOTE Stefan: I don't know why different types are possible here!
                    // sebsauer: because according to ODF-specs a zero null date can
                    // mean QDate::currentDate(). Still unclear what a numeric value !=0
                    // means through :-/
                }
            } else if (element.tagName() ==  "iteration") {
                // TODO
            }
        }
    }
}

bool Odf::saveCalculationSettings(const CalculationSettings *settings, KoXmlWriter &xmlWriter)
{
    xmlWriter.startElement("table:calculation-settings");
    if (!settings->caseSensitiveComparisons())
        xmlWriter.addAttribute("table:case-sensitive", "false");
    if (settings->precisionAsShown())
        xmlWriter.addAttribute("table:precision-as-shown", "true");
    if (!settings->wholeCellSearchCriteria())
        xmlWriter.addAttribute("table:search-criteria-must-apply-to-whole-cell", "false");
    if (!settings->automaticFindLabels())
        xmlWriter.addAttribute("table:automatic-find-labels", "false");
    if (!settings->useRegularExpressions())
        xmlWriter.addAttribute("table:use-regular-expressions", "false");
    if (settings->useWildcards())
        xmlWriter.addAttribute("table:use-wildcards", "true");
    if (settings->referenceYear() != 1930)
        xmlWriter.addAttribute("table:null-year", QString::number(settings->referenceYear()));
    xmlWriter.endElement();
    return true;
}




}  // Sheets
}  // Calligra
