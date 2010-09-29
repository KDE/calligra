/*
 * This file is part of Office 2007 Filters for KOffice
 * Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
 * Copyright (C) 2003 David Faure <faure@kde.org>
 * Copyright (C) 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "XlsxImport.h"
#include "XlsxXmlDocumentReader.h"
#include "XlsxXmlSharedStringsReader.h"
#include "XlsxXmlStylesReader.h"
#include "XlsxXmlCommentsReader.h"
#include "XlsxSharedString.h"

#include <MsooXmlUtils.h>
#include <MsooXmlSchemas.h>
#include <MsooXmlContentTypes.h>
#include <MsooXmlRelationships.h>

#include <memory>

#include <QColor>
#include <QFile>
#include <QFont>
#include <QPen>
#include <QRegExp>
#include <QImage>

#include <kdeversion.h>
#include <KDebug>
#include <KZip>
#include <KGenericFactory>
#include <KMessageBox>

#include <KoOdfWriteStore.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoDocumentInfo.h>
#include <KoDocument.h>
#include <KoFilterChain.h>
#include <KoUnit.h>
#include <KoPageLayout.h>
#include <KoXmlWriter.h>

typedef KGenericFactory<XlsxImport> XlsxImportFactory;
K_EXPORT_COMPONENT_FACTORY(libxlsximport, XlsxImportFactory("kofficefilters"))

enum XlsxDocumentType {
    XlsxDocument,
    XlsxTemplate,
    XlsxMacroDocument
};

class XlsxImport::Private
{
public:
    Private() : type(XlsxDocument), macrosEnabled(false) {
    }

    const char* mainDocumentContentType() const
    {
        if (type == XlsxMacroDocument)
            return MSOOXML::ContentTypes::spreadsheetMacroDocument;
        if (type == XlsxTemplate)
            return MSOOXML::ContentTypes::spreadsheetTemplate;
        return MSOOXML::ContentTypes::spreadsheetDocument;
    }

    XlsxDocumentType type;
    bool macrosEnabled;
};

XlsxImport::XlsxImport(QObject* parent, const QStringList &)
        : MSOOXML::MsooXmlImport(QLatin1String("spreadsheet"), parent), d(new Private)
{
}

XlsxImport::~XlsxImport()
{
    delete d;
}

bool XlsxImport::acceptsSourceMimeType(const QByteArray& mime) const
{
    kDebug() << "Entering XLSX Import filter: from " << mime;
    if (mime == "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet") {
        d->type = XlsxDocument;
        d->macrosEnabled = false;
    }
    else if (mime == "application/vnd.openxmlformats-officedocument.spreadsheetml.template") {
        d->type = XlsxTemplate;
        d->macrosEnabled = false;
    }
    else if (mime == "application/vnd.ms-excel.sheet.macroEnabled") {
        d->type = XlsxMacroDocument;
        d->macrosEnabled = true;
    }
    else if (mime == "application/vnd.ms-excel.sheet.macroEnabled.12") {
        d->type = XlsxDocument;
        d->macrosEnabled = true;
    }
    else if (mime == "application/vnd.ms-excel.template.macroEnabled.12") {
        d->type = XlsxTemplate;
        d->macrosEnabled = true;
    }
    else {
        return false;
    }
    return true;
}

bool XlsxImport::acceptsDestinationMimeType(const QByteArray& mime) const
{
    kDebug() << "Entering XLSX Import filter: to " << mime;
    return mime == "application/vnd.oasis.opendocument.spreadsheet";
}

KoFilter::ConversionStatus XlsxImport::parseParts(KoOdfWriters *writers,
        MSOOXML::MsooXmlRelationships *relationships, QString& errorMessage)
{
    // more here...
    // 0. temporary styles
//! @todo create styles in XlsxXmlDocumentReader (XLSX defines styles in workbook.xml)

    writers->mainStyles->insertRawOdfStyles(
        KoGenStyles::DocumentStyles,
        "    <!-- COPIED -->"
        "\n    <style:default-style style:family=\"table-cell\">"
        "\n      <style:table-cell-properties />"
        "\n      <style:paragraph-properties style:tab-stop-distance=\"1.27cm\"/>"
        "\n      <style:text-properties style:font-name=\"Albany AMT\" fo:language=\"en\" fo:country=\"US\" style:font-name-asian=\"Arial\" style:language-asian=\"zxx\" style:country-asian=\"none\" style:font-name-complex=\"Tahoma\" style:language-complex=\"zxx\" style:country-complex=\"none\"/>"
        "\n    </style:default-style>"
        "\n    <number:number-style style:name=\"N0\">"
        "\n      <number:number number:min-integer-digits=\"1\"/>"
        "\n    </number:number-style>"
        "\n    <number:currency-style style:name=\"N104P0\" style:volatile=\"true\">"
        "\n      <number:currency-symbol number:language=\"en\" number:country=\"US\">$</number:currency-symbol>"
        "\n      <number:number number:decimal-places=\"2\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n    </number:currency-style>"
        "\n    <number:currency-style style:name=\"N104\">"
        "\n      <style:text-properties fo:color=\"#ff0000\"/>"
        "\n      <number:text>-</number:text>"
        "\n      <number:currency-symbol number:language=\"en\" number:country=\"US\">$</number:currency-symbol>"
        "\n      <number:number number:decimal-places=\"2\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <style:map style:condition=\"value()&gt;=0\" style:apply-style-name=\"N104P0\"/>"
        "\n    </number:currency-style>"
        "\n    <number:number-style style:name=\"N5000\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:number number:min-integer-digits=\"1\"/>"
        "\n    </number:number-style>"
        "\n    <number:currency-style style:name=\"N5116P0\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:currency-symbol/>"
        "\n      <number:number number:decimal-places=\"0\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text> </number:text>"
        "\n    </number:currency-style>"
        "\n    <number:currency-style style:name=\"N5116\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text>(</number:text>"
        "\n      <number:currency-symbol/>"
        "\n      <number:number number:decimal-places=\"0\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text>)</number:text>"
        "\n      <style:map style:condition=\"value()&gt;=0\" style:apply-style-name=\"N5116P0\"/>"
        "\n    </number:currency-style>"
        "\n    <number:currency-style style:name=\"N5117P0\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:currency-symbol/>"
        "\n      <number:number number:decimal-places=\"0\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text> </number:text>"
        "\n    </number:currency-style>"
        "\n    <number:currency-style style:name=\"N5117\" number:language=\"en\" number:country=\"US\">"
        "\n      <style:text-properties fo:color=\"#ff0000\"/>"
        "\n      <number:text>(</number:text>"
        "\n      <number:currency-symbol/>"
        "\n      <number:number number:decimal-places=\"0\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text>)</number:text>"
        "\n      <style:map style:condition=\"value()&gt;=0\" style:apply-style-name=\"N5117P0\"/>"
        "\n    </number:currency-style>"
        "\n    <number:currency-style style:name=\"N5118P0\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:currency-symbol/>"
        "\n      <number:number number:decimal-places=\"2\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text> </number:text>"
        "\n    </number:currency-style>"
        "\n    <number:currency-style style:name=\"N5118\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text>(</number:text>"
        "\n      <number:currency-symbol/>"
        "\n      <number:number number:decimal-places=\"2\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text>)</number:text>"
        "\n      <style:map style:condition=\"value()&gt;=0\" style:apply-style-name=\"N5118P0\"/>"
        "\n    </number:currency-style>"
        "\n    <number:currency-style style:name=\"N5119P0\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:currency-symbol/>"
        "\n      <number:number number:decimal-places=\"2\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text> </number:text>"
        "\n    </number:currency-style>"
        "\n    <number:currency-style style:name=\"N5119\" number:language=\"en\" number:country=\"US\">"
        "\n      <style:text-properties fo:color=\"#ff0000\"/>"
        "\n      <number:text>(</number:text>"
        "\n      <number:currency-symbol/>"
        "\n      <number:number number:decimal-places=\"2\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text>)</number:text>"
        "\n      <style:map style:condition=\"value()&gt;=0\" style:apply-style-name=\"N5119P0\"/>"
        "\n    </number:currency-style>"
        "\n    <number:date-style style:name=\"N5120\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:month/>"
        "\n      <number:text>/</number:text>"
        "\n      <number:day/>"
        "\n      <number:text>/</number:text>"
        "\n      <number:year number:style=\"long\"/>"
        "\n    </number:date-style>"
        "\n    <number:date-style style:name=\"N5121\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:day/>"
        "\n      <number:text>-</number:text>"
        "\n      <number:month number:textual=\"true\"/>"
        "\n      <number:text>-</number:text>"
        "\n      <number:year/>"
        "\n    </number:date-style>"
        "\n    <number:date-style style:name=\"N5122\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:day/>"
        "\n      <number:text>-</number:text>"
        "\n      <number:month number:textual=\"true\"/>"
        "\n    </number:date-style>"
        "\n    <number:date-style style:name=\"N5123\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:month number:textual=\"true\"/>"
        "\n      <number:text>-</number:text>"
        "\n      <number:year/>"
        "\n    </number:date-style>"
        "\n    <number:time-style style:name=\"N5124\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:hours/>"
        "\n      <number:text>:</number:text>"
        "\n      <number:minutes number:style=\"long\"/>"
        "\n      <number:text> </number:text>"
        "\n      <number:am-pm/>"
        "\n    </number:time-style>"
        "\n    <number:time-style style:name=\"N5125\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:hours/>"
        "\n      <number:text>:</number:text>"
        "\n      <number:minutes number:style=\"long\"/>"
        "\n      <number:text>:</number:text>"
        "\n      <number:seconds number:style=\"long\"/>"
        "\n      <number:text> </number:text>"
        "\n      <number:am-pm/>"
        "\n    </number:time-style>"
        "\n    <number:time-style style:name=\"N5126\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:hours/>"
        "\n      <number:text>:</number:text>"
        "\n      <number:minutes number:style=\"long\"/>"
        "\n    </number:time-style>"
        "\n    <number:time-style style:name=\"N5127\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:hours/>"
        "\n      <number:text>:</number:text>"
        "\n      <number:minutes number:style=\"long\"/>"
        "\n      <number:text>:</number:text>"
        "\n      <number:seconds number:style=\"long\"/>"
        "\n    </number:time-style>"
        "\n    <number:date-style style:name=\"N5128\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:month/>"
        "\n      <number:text>/</number:text>"
        "\n      <number:day/>"
        "\n      <number:text>/</number:text>"
        "\n      <number:year number:style=\"long\"/>"
        "\n      <number:text> </number:text>"
        "\n      <number:hours/>"
        "\n      <number:text>:</number:text>"
        "\n      <number:minutes number:style=\"long\"/>"
        "\n    </number:date-style>"
        "\n    <number:number-style style:name=\"N5129P0\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:number number:decimal-places=\"0\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text> </number:text>"
        "\n    </number:number-style>"
        "\n    <number:number-style style:name=\"N5129\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text>(</number:text>"
        "\n      <number:number number:decimal-places=\"0\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text>)</number:text>"
        "\n      <style:map style:condition=\"value()&gt;=0\" style:apply-style-name=\"N5129P0\"/>"
        "\n    </number:number-style>"
        "\n    <number:number-style style:name=\"N5130P0\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:number number:decimal-places=\"0\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text> </number:text>"
        "\n    </number:number-style>"
        "\n    <number:number-style style:name=\"N5130\" number:language=\"en\" number:country=\"US\">"
        "\n      <style:text-properties fo:color=\"#ff0000\"/>"
        "\n      <number:text>(</number:text>"
        "\n      <number:number number:decimal-places=\"0\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text>)</number:text>"
        "\n      <style:map style:condition=\"value()&gt;=0\" style:apply-style-name=\"N5130P0\"/>"
        "\n    </number:number-style>"
        "\n    <number:number-style style:name=\"N5131P0\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:number number:decimal-places=\"2\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text> </number:text>"
        "\n    </number:number-style>"
        "\n    <number:number-style style:name=\"N5131\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text>(</number:text>"
        "\n      <number:number number:decimal-places=\"2\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text>)</number:text>"
        "\n      <style:map style:condition=\"value()&gt;=0\" style:apply-style-name=\"N5131P0\"/>"
        "\n    </number:number-style>"
        "\n    <number:number-style style:name=\"N5132P0\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:number number:decimal-places=\"2\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text> </number:text>"
        "\n    </number:number-style>"
        "\n    <number:number-style style:name=\"N5132\" number:language=\"en\" number:country=\"US\">"
        "\n      <style:text-properties fo:color=\"#ff0000\"/>"
        "\n      <number:text>(</number:text>"
        "\n      <number:number number:decimal-places=\"2\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text>)</number:text>"
        "\n      <style:map style:condition=\"value()&gt;=0\" style:apply-style-name=\"N5132P0\"/>"
        "\n    </number:number-style>"
        "\n    <number:number-style style:name=\"N5133P0\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text> </number:text>"
        "\n      <number:number number:decimal-places=\"0\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text> </number:text>"
        "\n    </number:number-style>"
        "\n    <number:number-style style:name=\"N5133P1\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text> (</number:text>"
        "\n      <number:number number:decimal-places=\"0\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text>)</number:text>"
        "\n    </number:number-style>"
        "\n    <number:number-style style:name=\"N5133P2\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text> - </number:text>"
        "\n    </number:number-style>"
        "\n    <number:text-style style:name=\"N5133\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text> </number:text>"
        "\n      <number:text-content/>"
        "\n      <number:text> </number:text>"
        "\n      <style:map style:condition=\"value()&gt;0\" style:apply-style-name=\"N5133P0\"/>"
        "\n      <style:map style:condition=\"value()&lt;0\" style:apply-style-name=\"N5133P1\"/>"
        "\n      <style:map style:condition=\"value()=0\" style:apply-style-name=\"N5133P2\"/>"
        "\n    </number:text-style>"
        "\n    <number:currency-style style:name=\"N5134P0\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text> </number:text>"
        "\n      <number:currency-symbol/>"
        "\n      <number:number number:decimal-places=\"0\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text> </number:text>"
        "\n    </number:currency-style>"
        "\n    <number:currency-style style:name=\"N5134P1\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text> </number:text>"
        "\n      <number:currency-symbol/>"
        "\n      <number:text>(</number:text>"
        "\n      <number:number number:decimal-places=\"0\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text>)</number:text>"
        "\n    </number:currency-style>"
        "\n    <number:currency-style style:name=\"N5134P2\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text> </number:text>"
        "\n      <number:currency-symbol/>"
        "\n      <number:text>- </number:text>"
        "\n    </number:currency-style>"
        "\n    <number:text-style style:name=\"N5134\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text> </number:text>"
        "\n      <number:text-content/>"
        "\n      <number:text> </number:text>"
        "\n      <style:map style:condition=\"value()&gt;0\" style:apply-style-name=\"N5134P0\"/>"
        "\n      <style:map style:condition=\"value()&lt;0\" style:apply-style-name=\"N5134P1\"/>"
        "\n      <style:map style:condition=\"value()=0\" style:apply-style-name=\"N5134P2\"/>"
        "\n    </number:text-style>"
        "\n    <number:number-style style:name=\"N5135P0\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text> </number:text>"
        "\n      <number:number number:decimal-places=\"2\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text> </number:text>"
        "\n    </number:number-style>"
        "\n    <number:number-style style:name=\"N5135P1\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text> (</number:text>"
        "\n      <number:number number:decimal-places=\"2\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text>)</number:text>"
        "\n    </number:number-style>"
        "\n    <number:number-style style:name=\"N5135P2\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text> -</number:text>"
        "\n      <number:number number:decimal-places=\"0\" number:min-integer-digits=\"0\"/>"
        "\n      <number:text> </number:text>"
        "\n    </number:number-style>"
        "\n    <number:text-style style:name=\"N5135\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text> </number:text>"
        "\n      <number:text-content/>"
        "\n      <number:text> </number:text>"
        "\n      <style:map style:condition=\"value()&gt;0\" style:apply-style-name=\"N5135P0\"/>"
        "\n      <style:map style:condition=\"value()&lt;0\" style:apply-style-name=\"N5135P1\"/>"
        "\n      <style:map style:condition=\"value()=0\" style:apply-style-name=\"N5135P2\"/>"
        "\n    </number:text-style>"
        "\n    <number:currency-style style:name=\"N5136P0\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text> </number:text>"
        "\n      <number:currency-symbol/>"
        "\n      <number:number number:decimal-places=\"2\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text> </number:text>"
        "\n    </number:currency-style>"
        "\n    <number:currency-style style:name=\"N5136P1\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text> </number:text>"
        "\n      <number:currency-symbol/>"
        "\n      <number:text>(</number:text>"
        "\n      <number:number number:decimal-places=\"2\" number:min-integer-digits=\"1\" number:grouping=\"true\"/>"
        "\n      <number:text>)</number:text>"
        "\n    </number:currency-style>"
        "\n    <number:currency-style style:name=\"N5136P2\" style:volatile=\"true\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text> </number:text>"
        "\n      <number:currency-symbol/>"
        "\n      <number:text>-</number:text>"
        "\n      <number:number number:decimal-places=\"0\" number:min-integer-digits=\"0\"/>"
        "\n      <number:text> </number:text>"
        "\n    </number:currency-style>"
        "\n    <number:text-style style:name=\"N5136\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:text> </number:text>"
        "\n      <number:text-content/>"
        "\n      <number:text> </number:text>"
        "\n      <style:map style:condition=\"value()&gt;0\" style:apply-style-name=\"N5136P0\"/>"
        "\n      <style:map style:condition=\"value()&lt;0\" style:apply-style-name=\"N5136P1\"/>"
        "\n      <style:map style:condition=\"value()=0\" style:apply-style-name=\"N5136P2\"/>"
        "\n    </number:text-style>"
        "\n    <number:time-style style:name=\"N5137\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:minutes number:style=\"long\"/>"
        "\n      <number:text>:</number:text>"
        "\n      <number:seconds number:style=\"long\"/>"
        "\n    </number:time-style>"
        "\n    <number:time-style style:name=\"N5138\" number:language=\"en\" number:country=\"US\" number:truncate-on-overflow=\"false\">"
        "\n      <number:hours/>"
        "\n      <number:text>:</number:text>"
        "\n      <number:minutes number:style=\"long\"/>"
        "\n      <number:text>:</number:text>"
        "\n      <number:seconds number:style=\"long\"/>"
        "\n    </number:time-style>"
        "\n    <number:time-style style:name=\"N5139\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:minutes number:style=\"long\"/>"
        "\n      <number:text>:</number:text>"
        "\n      <number:seconds number:style=\"long\" number:decimal-places=\"1\"/>"
        "\n    </number:time-style>"
        "\n    <number:number-style style:name=\"N5140\" number:language=\"en\" number:country=\"US\">"
        "\n      <number:scientific-number number:decimal-places=\"1\" number:min-integer-digits=\"3\" number:min-exponent-digits=\"1\"/>"
        "\n    </number:number-style>"
        "\n    <style:style style:name=\"Default\" style:family=\"table-cell\"/>"
        "\n    <style:style style:name=\"Result\" style:family=\"table-cell\" style:parent-style-name=\"Default\">"
        "\n      <style:text-properties fo:font-style=\"italic\" style:text-underline-style=\"solid\" style:text-underline-width=\"auto\" style:text-underline-color=\"font-color\" fo:font-weight=\"bold\"/>"
        "\n    </style:style>"
        "\n    <style:style style:name=\"Result2\" style:family=\"table-cell\" style:parent-style-name=\"Result\" style:data-style-name=\"N104\"/>"
        "\n    <style:style style:name=\"Heading\" style:family=\"table-cell\" style:parent-style-name=\"Default\">"
        "\n      <style:table-cell-properties style:text-align-source=\"fix\" style:repeat-content=\"false\"/>"
        "\n      <style:paragraph-properties fo:text-align=\"center\"/>"
        "\n      <style:text-properties fo:font-size=\"16pt\" fo:font-style=\"italic\" fo:font-weight=\"bold\"/>"
        "\n    </style:style>"
        "\n    <style:style style:name=\"Heading1\" style:family=\"table-cell\" style:parent-style-name=\"Heading\">"
        "\n      <style:table-cell-properties style:rotation-angle=\"90\"/>"
        "\n    </style:style>"
        "\n    <style:style style:name=\"Excel_20_Built-in_20_Normal\" style:display-name=\"Excel Built-in Normal\" style:family=\"table-cell\" style:parent-style-name=\"Default\" style:data-style-name=\"N5000\">"
        "\n      <style:table-cell-properties style:cell-protect=\"protected\" style:print-content=\"true\" style:diagonal-bl-tr=\"none\" style:diagonal-tl-br=\"none\" style:text-align-source=\"value-type\" style:repeat-content=\"false\" fo:background-color=\"transparent\" fo:wrap-option=\"no-wrap\" fo:border=\"none\" style:direction=\"ltr\" style:rotation-angle=\"0\" style:rotation-align=\"none\" style:shrink-to-fit=\"false\" style:vertical-align=\"bottom\"/>"
        "\n      <style:paragraph-properties fo:margin-left=\"0cm\" style:writing-mode=\"page\"/>"
        "\n      <style:text-properties fo:color=\"#000000\" style:text-outline=\"false\" style:text-line-through-style=\"none\" style:font-name=\"Calibri\" fo:font-size=\"11pt\" fo:font-style=\"normal\" fo:text-shadow=\"none\" style:text-underline-style=\"none\" fo:font-weight=\"normal\" style:font-size-asian=\"11pt\" style:font-style-asian=\"normal\" style:font-weight-asian=\"normal\" style:font-size-complex=\"11pt\" style:font-style-complex=\"normal\" style:font-weight-complex=\"normal\"/>"
        "\n    </style:style>"
        "\n    <!-- /COPIED -->"
    );

    writers->mainStyles->insertRawOdfStyles(
        KoGenStyles::MasterStyles,
        "    <!-- COPIED -->"
        "\n    <style:master-page style:name=\"Default\" style:page-layout-name=\"pm1\">"
        "\n      <style:header>"
        "\n        <text:p>"
        "\n          <text:sheet-name>" "???" "</text:sheet-name>"
        "\n        </text:p>"
        "\n      </style:header>"
        "\n      <style:header-left style:display=\"false\"/>"
        "\n      <style:footer>"
        "\n        <text:p>Page <text:page-number>1</text:page-number></text:p>"
        "\n      </style:footer>"
        "\n      <style:footer-left style:display=\"false\"/>"
        "\n    </style:master-page>"
        "\n    <style:master-page style:name=\"Report\" style:page-layout-name=\"pm2\">"
        "\n      <style:header>"
        "\n        <style:region-left>"
        "\n          <text:p><text:sheet-name>" "???" "</text:sheet-name> (<text:title>" "???" "</text:title>)</text:p>"
        "\n        </style:region-left>"
        "\n        <style:region-right>"
        "\n          <text:p><text:date style:data-style-name=\"N2\" text:date-value=\"2009-07-18\">07/18/2009</text:date>, <text:time>21:48:12</text:time></text:p>"
        "\n        </style:region-right>"
        "\n      </style:header>"
        "\n      <style:header-left style:display=\"false\"/>"
        "\n      <style:footer>"
        "\n        <text:p>Page <text:page-number>1</text:page-number> / <text:page-count>99</text:page-count></text:p>"
        "\n      </style:footer>"
        "\n      <style:footer-left style:display=\"false\"/>"
        "\n    </style:master-page>"
        "\n    <style:master-page style:name=\"PageStyle_5f_Test_20_sheet_20__5f_1\" style:display-name=\"PageStyle_Test sheet _1\" style:page-layout-name=\"pm3\">"
        "\n      <style:header style:display=\"false\"/>"
        "\n      <style:header-left style:display=\"false\"/>"
        "\n      <style:footer style:display=\"false\"/>"
        "\n      <style:footer-left style:display=\"false\"/>"
        "\n    </style:master-page>"
        "\n    <style:master-page style:name=\"PageStyle_5f_Test_20_sheet_20__5f_2\" style:display-name=\"PageStyle_Test sheet _2\" style:page-layout-name=\"pm4\">"
        "\n      <style:header style:display=\"false\"/>"
        "\n      <style:header-left style:display=\"false\"/>"
        "\n      <style:footer style:display=\"false\"/>"
        "\n      <style:footer-left style:display=\"false\"/>"
        "\n    </style:master-page>"
        "\n    <!-- COPIED -->"
    );

    writers->mainStyles->insertRawOdfStyles(
        KoGenStyles::StylesXmlAutomaticStyles,
        "    <!-- COPIED -->"
        "\n    <style:page-layout style:name=\"pm1\">"
        "\n      <style:page-layout-properties style:first-page-number=\"continue\" style:writing-mode=\"lr-tb\"/>"
        "\n      <style:header-style>"
        "\n        <style:header-footer-properties fo:min-height=\"0.751cm\" fo:margin-left=\"0cm\" fo:margin-right=\"0cm\" fo:margin-bottom=\"0.25cm\"/>"
        "\n      </style:header-style>"
        "\n      <style:footer-style>"
        "\n        <style:header-footer-properties fo:min-height=\"0.751cm\" fo:margin-left=\"0cm\" fo:margin-right=\"0cm\" fo:margin-top=\"0.25cm\"/>"
        "\n      </style:footer-style>"
        "\n    </style:page-layout>"
        "\n    <style:page-layout style:name=\"pm2\">"
        "\n      <style:page-layout-properties style:writing-mode=\"lr-tb\"/>"
        "\n      <style:header-style>"
        "\n        <style:header-footer-properties fo:min-height=\"0.751cm\" fo:margin-left=\"0cm\" fo:margin-right=\"0cm\" fo:margin-bottom=\"0.25cm\" fo:border=\"0.088cm solid #000000\" fo:padding=\"0.018cm\" fo:background-color=\"#c0c0c0\">"
        "\n          <style:background-image/>"
        "\n        </style:header-footer-properties>"
        "\n      </style:header-style>"
        "\n      <style:footer-style>"
        "\n        <style:header-footer-properties fo:min-height=\"0.751cm\" fo:margin-left=\"0cm\" fo:margin-right=\"0cm\" fo:margin-top=\"0.25cm\" fo:border=\"0.088cm solid #000000\" fo:padding=\"0.018cm\" fo:background-color=\"#c0c0c0\">"
        "\n          <style:background-image/>"
        "\n        </style:header-footer-properties>"
        "\n      </style:footer-style>"
        "\n    </style:page-layout>"
        "\n    <style:page-layout style:name=\"pm3\">"
        "\n      <style:page-layout-properties fo:page-width=\"21.001cm\" fo:page-height=\"29.7cm\" style:num-format=\"1\" style:print-orientation=\"portrait\" fo:margin-top=\"1.905cm\" fo:margin-bottom=\"1.905cm\" fo:margin-left=\"1.778cm\" fo:margin-right=\"1.778cm\" style:print-page-order=\"ttb\" style:first-page-number=\"continue\" style:scale-to=\"100%\" style:writing-mode=\"lr-tb\" style:print=\"charts drawings objects zero-values\"/>"
        "\n      <style:header-style>"
        "\n        <style:header-footer-properties fo:min-height=\"0.75cm\" fo:margin-left=\"0cm\" fo:margin-right=\"0cm\" fo:margin-bottom=\"0.25cm\"/>"
        "\n      </style:header-style>"
        "\n      <style:footer-style>"
        "\n        <style:header-footer-properties fo:min-height=\"0.75cm\" fo:margin-left=\"0cm\" fo:margin-right=\"0cm\" fo:margin-top=\"0.25cm\"/>"
        "\n      </style:footer-style>"
        "\n    </style:page-layout>"
        "\n    <style:page-layout style:name=\"pm4\">"
        "\n      <style:page-layout-properties fo:page-width=\"21.59cm\" fo:page-height=\"27.94cm\" style:num-format=\"1\" style:print-orientation=\"portrait\" fo:margin-top=\"1.905cm\" fo:margin-bottom=\"1.905cm\" fo:margin-left=\"1.778cm\" fo:margin-right=\"1.778cm\" style:print-page-order=\"ttb\" style:first-page-number=\"continue\" style:scale-to=\"100%\" style:writing-mode=\"lr-tb\" style:print=\"charts drawings objects zero-values\"/>"
        "\n      <style:header-style>"
        "\n        <style:header-footer-properties fo:min-height=\"0.75cm\" fo:margin-left=\"0cm\" fo:margin-right=\"0cm\" fo:margin-bottom=\"0.25cm\"/>"
        "\n      </style:header-style>"
        "\n      <style:footer-style>"
        "\n        <style:header-footer-properties fo:min-height=\"0.75cm\" fo:margin-left=\"0cm\" fo:margin-right=\"0cm\" fo:margin-top=\"0.25cm\"/>"
        "\n      </style:footer-style>"
        "\n    </style:page-layout>"
        "\n    <!-- /COPIED -->"
    );

    // 1. parse shared strings
    XlsxSharedStringVector sharedStrings;
    {
        XlsxXmlSharedStringsReader sharedStringsReader(writers);
        XlsxXmlSharedStringsReaderContext context(sharedStrings);
        RETURN_IF_ERROR(loadAndParseDocumentIfExists(
                            MSOOXML::ContentTypes::spreadsheetSharedStrings, &sharedStringsReader, writers, errorMessage, &context))
    }
    // 2. parse styles
    XlsxStyles styles;
    {
        XlsxXmlStylesReader stylesReader(writers);
        XlsxXmlStylesReaderContext context(styles);
        RETURN_IF_ERROR(loadAndParseDocumentIfExists(
                            MSOOXML::ContentTypes::spreadsheetStyles, &stylesReader, writers, errorMessage, &context))
    }
    // 3. parse comments
    XlsxComments comments;
    {
        XlsxXmlCommentsReader commentsReader(writers);
        XlsxXmlCommentsReaderContext context(comments);
        RETURN_IF_ERROR( loadAndParseDocumentFromFileIfExists(
//! @todo only support "xl/comments1.xml" filename for comments?
            "xl/comments1.xml", &commentsReader, writers, errorMessage, &context) )
    }

    // 4. parse themes
    QList<QByteArray> partNames = this->partNames(d->mainDocumentContentType());
    
    // following is a workaround till the patch at https://bugs.freedesktop.org/show_bug.cgi?id=30417 is applied
    // so we are able to proper handle this case already before using the mimetype.
    if (partNames.isEmpty() && d->type != XlsxMacroDocument) {
        QList<QByteArray> macroPartNames = this->partNames(MSOOXML::ContentTypes::spreadsheetMacroDocument);
        if (macroPartNames.count() == 1 && acceptsSourceMimeType("application/vnd.ms-excel.sheet.macroEnabled")) {
            partNames = macroPartNames;
        }
    }

    if (partNames.count() != 1) {
        errorMessage = i18n("Unable to find part for type %1", d->mainDocumentContentType());
        return KoFilter::WrongFormat;
    }
    const QString spreadPathAndFile(partNames.first());
    QString spreadPath, spreadFile;
    MSOOXML::Utils::splitPathAndFile(spreadPathAndFile, &spreadPath, &spreadFile);

    MSOOXML::DrawingMLTheme themes;
    const QString spreadThemePathAndFile(relationships->targetForType(
        spreadPath, spreadFile,
        QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/theme"));
    kDebug() << QLatin1String(MSOOXML::Schemas::officeDocument::relationships) + "/theme";

    QString spreadThemePath, spreadThemeFile;
    MSOOXML::Utils::splitPathAndFile(spreadThemePathAndFile, &spreadThemePath, &spreadThemeFile);

    MSOOXML::MsooXmlThemesReader themesReader(writers);
    MSOOXML::MsooXmlThemesReaderContext themecontext(themes, relationships, (MSOOXML::MsooXmlImport*)this,
        spreadThemePath, spreadThemeFile);

    KoFilter::ConversionStatus status
        = loadAndParseDocument(&themesReader, spreadThemePathAndFile, errorMessage, &themecontext);

    // 5. parse document
    {
        XlsxXmlDocumentReaderContext context(*this, &themes, sharedStrings, comments, styles, *relationships);
        XlsxXmlDocumentReader documentReader(writers);
        RETURN_IF_ERROR(loadAndParseDocument(
            d->mainDocumentContentType(), &documentReader, writers, errorMessage, &context) )
    }
    // more here...
    return KoFilter::OK;
}

#include "XlsxImport.moc"
