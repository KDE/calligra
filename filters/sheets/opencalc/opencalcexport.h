/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000-2003 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef OPENCALCEXPORT_H
#define OPENCALCEXPORT_H

#include "opencalcstyleexport.h"

#include <KoFilter.h>
#include <QByteArray>
#include <QVariantList>

class QDomDocument;
class QDomElement;
class KoStore;

namespace Calligra
{
namespace Sheets
{
class DocBase;
class Localization;
class Sheet;
}
}

class OpenCalcExport : public KoFilter
{
    Q_OBJECT

public:
    OpenCalcExport(QObject *parent, const QVariantList &);
    ~OpenCalcExport() override = default;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

private:
    enum files { metaXML = 0x01, contentXML = 0x02, stylesXML = 0x04, settingsXML = 0x08 };
    OpenCalcStyles m_styles;

    bool writeFile(const Calligra::Sheets::DocBase *ksdoc);

    bool exportDocInfo(KoStore *store, const Calligra::Sheets::DocBase *ksdoc);
    bool exportStyles(KoStore *store, const Calligra::Sheets::DocBase *ksdoc);
    bool exportContent(KoStore *store, const Calligra::Sheets::DocBase *ksdoc);
    bool exportSettings(KoStore *store, const Calligra::Sheets::DocBase *ksdoc);

    bool exportBody(QDomDocument &doc, QDomElement &content, const Calligra::Sheets::DocBase *ksdoc);
    void exportSheet(QDomDocument &doc, QDomElement &tabElem, Calligra::Sheets::Sheet *sheet, int maxCols, int maxRows);
    void exportCells(QDomDocument &doc, QDomElement &rowElem, Calligra::Sheets::Sheet *sheet, int row, int maxCols);
    void exportDefaultCellStyle(QDomDocument &doc, QDomElement &officeStyles);
    void exportPageAutoStyles(QDomDocument &doc, QDomElement &autoStyles, const Calligra::Sheets::DocBase *ksdoc);
    void exportMasterStyles(QDomDocument &doc, QDomElement &masterStyles, const Calligra::Sheets::DocBase *ksdoc);

    bool writeMetaFile(KoStore *store, uint filesWritten);

    void convertPart(QString const &part, QDomDocument &doc, QDomElement &parent, const Calligra::Sheets::DocBase *ksdoc);
    void addText(QString const &text, QDomDocument &doc, QDomElement &parent);

    void createDefaultStyles();
    QString convertFormula(QString const &formula) const;

private:
    /// Pointer to the Calligra::Sheets locale
    Calligra::Sheets::Localization *m_locale;
};

#endif
