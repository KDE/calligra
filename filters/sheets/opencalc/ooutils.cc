/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Laurent Montel <lmontel@mandrakesoft.com>
   SPDX-FileCopyrightText: 2003 Lukas Tinkl <lukas@kde.org>
   SPDX-FileCopyrightText: 2003 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ooutils.h"
#include <KoStyleStack.h>
#include <KoStore.h>
#include <KoXmlReader.h>
#include <QDomDocument>
#include <QColor>
#include <QImage>
#include <KoUnit.h>
#include <QDebug>
#include <kzip.h>

const char ooNS::office[] = "http://openoffice.org/2000/office";
const char ooNS::style[] = "http://openoffice.org/2000/style";
const char ooNS::text[] = "http://openoffice.org/2000/text";
const char ooNS::table[] = "http://openoffice.org/2000/table";
const char ooNS::draw[] = "http://openoffice.org/2000/drawing";
const char ooNS::presentation[] = "http://openoffice.org/2000/presentation";
const char ooNS::fo[] = "http://www.w3.org/1999/XSL/Format";
const char ooNS::xlink[] = "http://www.w3.org/1999/xlink";
const char ooNS::number[] = "http://openoffice.org/2000/datastyle";
const char ooNS::svg[] = "http://www.w3.org/2000/svg";
const char ooNS::dc[] = "http://purl.org/dc/elements/1.1/";
const char ooNS::meta[] = "http://openoffice.org/2000/meta";
const char ooNS::config[] = "http://openoffice.org/2001/config";

QString OoUtils::expandWhitespace(const KoXmlElement& tag)
{
    //tags like <text:s text:c="4">

    int howmany = 1;
    if (tag.hasAttributeNS(ooNS::text, "c"))
        howmany = tag.attributeNS(ooNS::text, "c", QString()).toInt();

    QString result;
    return result.fill(QChar(32), howmany);
}

bool OoUtils::parseBorder(const QString &tag, double *width, int *style, QColor *color)
{
    // string like "0.088cm solid #800000"

    if (tag.isEmpty() || tag == "none" || tag == "hidden") {
        // in fact no border
        return false;
    }

    QString _width = tag.section(' ', 0, 0);
    QString _style = tag.section(' ', 1, 1);
    QString _color = tag.section(' ', 2, 2);

    *width = KoUnit::parseValue(_width, 1.0);

    if (_style == "dashed") {
        *style = 1;
    } else if (_style == "dotted") {
        *style = 2;
    } else if (_style == "dot-dash") {
        // not in xsl/fo, but in OASIS (in other places)
        *style = 3;
    } else if (_style == "dot-dot-dash") {
        // not in xsl/fo, but in OASIS (in other places)
        *style = 4;
    } else if (_style == "double") {
        *style = 5;
    } else {
        *style = 0;
    }

    if (_color.isEmpty()) {
        *color = QColor();
    } else {
        *color  = QColor::fromString(_color);
    }

    return true;
}

void OoUtils::importIndents(QDomElement& parentElement, const KoStyleStack& styleStack)
{
    if (styleStack.hasProperty(ooNS::fo, "margin-left") ||    // 3.11.19
            styleStack.hasProperty(ooNS::fo, "margin-right"))
        // *text-indent must always be bound to either margin-left or margin-right
    {
        double marginLeft = KoUnit::parseValue(styleStack.property(ooNS::fo, "margin-left"));
        double marginRight = KoUnit::parseValue(styleStack.property(ooNS::fo, "margin-right"));
        double first = 0;
        if (styleStack.property(ooNS::style, "auto-text-indent") == "true")  // style:auto-text-indent takes precedence
            // ### "indented by a value that is based on the current font size"
            // ### and "requires margin-left and margin-right
            // ### but how much is the indent?
            first = 10;
        else if (styleStack.hasProperty(ooNS::fo, "text-indent"))
            first = KoUnit::parseValue(styleStack.property(ooNS::fo, "text-indent"));

        if (marginLeft != 0 || marginRight != 0 || first != 0) {
            QDomElement indent = parentElement.ownerDocument().createElement("INDENTS");
            if (marginLeft != 0)
                indent.setAttribute("left", QString::number(marginLeft));
            if (marginRight != 0)
                indent.setAttribute("right", QString::number(marginRight));
            if (first != 0)
                indent.setAttribute("first", QString::number(first));
            parentElement.appendChild(indent);
        }
    }
}

void OoUtils::importLineSpacing(QDomElement& parentElement, const KoStyleStack& styleStack)
{
    if (styleStack.hasProperty(ooNS::fo, "line-height")) {
        // Fixed line height
        QString value = styleStack.property(ooNS::fo, "line-height");   // 3.11.1
        if (value != "normal") {
            QDomElement lineSpacing = parentElement.ownerDocument().createElement("LINESPACING");
            if (value == "100%")
                lineSpacing.setAttribute("type", "single");
            else if (value == "150%")
                lineSpacing.setAttribute("type", "oneandhalf");
            else if (value == "200%")
                lineSpacing.setAttribute("type", "double");
            else if (value.contains('%')) {
                double percent = value.toDouble();
                lineSpacing.setAttribute("type", "multiple");
                lineSpacing.setAttribute("spacingvalue", QString::number(percent / 100));
            } else { // fixed value (use KoUnit::parseValue to get it in pt)
                qWarning() << "Unhandled value for fo:line-height: " << value;
            }
            parentElement.appendChild(lineSpacing);
        }
    }
    // Line-height-at-least is mutually exclusive with line-height
    else if (styleStack.hasProperty(ooNS::style, "line-height-at-least")) {  // 3.11.2
        QString value = styleStack.property(ooNS::style, "line-height-at-least");
        // kotext has "at least" but that's for the linespacing, not for the entire line height!
        // Strange. kotext also has "at least" for the whole line height....
        // Did we make the wrong choice in kotext?
        //qWarning() << "Unimplemented support for style:line-height-at-least: " << value;
        // Well let's see if this makes a big difference.
        QDomElement lineSpacing = parentElement.ownerDocument().createElement("LINESPACING");
        lineSpacing.setAttribute("type", "atleast");
        lineSpacing.setAttribute("spacingvalue", QString::number(KoUnit::parseValue(value)));
        parentElement.appendChild(lineSpacing);
    }
    // Line-spacing is mutually exclusive with line-height and line-height-at-least
    else if (styleStack.hasProperty(ooNS::style, "line-spacing")) {  // 3.11.3
        double value = KoUnit::parseValue(styleStack.property(ooNS::style, "line-spacing"));
        if (value != 0.0) {
            QDomElement lineSpacing = parentElement.ownerDocument().createElement("LINESPACING");
            lineSpacing.setAttribute("type", "custom");
            lineSpacing.setAttribute("spacingvalue", QString::number(value));
            parentElement.appendChild(lineSpacing);
        }
    }

}

void OoUtils::importTopBottomMargin(QDomElement& parentElement, const KoStyleStack& styleStack)
{
    if (styleStack.hasProperty(ooNS::fo, "margin-top") ||  // 3.11.22
            styleStack.hasProperty(ooNS::fo, "margin-bottom")) {
        double mtop = KoUnit::parseValue(styleStack.property(ooNS::fo, "margin-top"));
        double mbottom = KoUnit::parseValue(styleStack.property(ooNS::fo, "margin-bottom"));
        if (mtop != 0 || mbottom != 0) {
            QDomElement offset = parentElement.ownerDocument().createElement("OFFSETS");
            if (mtop != 0)
                offset.setAttribute("before", QString::number(mtop));
            if (mbottom != 0)
                offset.setAttribute("after", QString::number(mbottom));
            parentElement.appendChild(offset);
        }
    }
}

void OoUtils::importTabulators(QDomElement& parentElement, const KoStyleStack& styleStack)
{
    if (!styleStack.hasChildNode(ooNS::style, "tab-stops"))     // 3.11.10
        return;
    KoXmlElement tabStops = styleStack.childNode(ooNS::style, "tab-stops");
    //qDebug() << tabStops.childNodes().count() <<" tab stops in layout.";
    for (KoXmlNode it = tabStops.firstChild(); !it.isNull(); it = it.nextSibling()) {
        KoXmlElement tabStop = it.toElement();
        Q_ASSERT(tabStop.prefix() == QLatin1StringView("style"));
        Q_ASSERT(tabStop.tagName() == "tab-stop");
        QString type = tabStop.attributeNS(ooNS::style, "type", QString());   // left, right, center or char

        QDomElement elem = parentElement.ownerDocument().createElement("TABULATOR");
        int calligraType = 0;
        if (type == "left")
            calligraType = 0;
        else if (type == "center")
            calligraType = 1;
        else if (type == "right")
            calligraType = 2;
        else if (type == "char") {
            QString delimiterChar = tabStop.attributeNS(ooNS::style, "char", QString());   // single character
            elem.setAttribute("alignchar", delimiterChar);
            calligraType = 3; // "alignment on decimal point"
        }

        elem.setAttribute("type", calligraType);

        double pos = KoUnit::parseValue(tabStop.attributeNS(ooNS::style, "position", QString()));
        elem.setAttribute("ptpos", QString::number(pos));

        // TODO Convert leaderChar's unicode value to the Calligra enum
        // (blank/dots/line/dash/dash-dot/dash-dot-dot, 0 to 5)
        QString leaderChar = tabStop.attributeNS(ooNS::style, "leader-char", QString());   // single character
        if (!leaderChar.isEmpty()) {
            int filling = 0;
            QChar ch = leaderChar[0];
            switch (ch.toLatin1()) {
            case '.':
                filling = 1; break;
            case '-':
            case '_':  // TODO in Words: differentiate --- and ___
                filling = 2; break;
            default:
                // Words doesn't have support for "any char" as filling.
                // Instead it has dash-dot and dash-dot-dot - but who uses that in a tabstop?
                break;
            }
            elem.setAttribute("filling", filling);
        }
        parentElement.appendChild(elem);
    }

}

void OoUtils::importBorders(QDomElement& parentElement, const KoStyleStack& styleStack)
{
    if (styleStack.hasProperty(ooNS::fo, "border", "left")) {
        double width;
        int style;
        QColor color;
        if (OoUtils::parseBorder(styleStack.property(ooNS::fo, "border", "left"), &width, &style, &color)) {
            QDomElement lbElem = parentElement.ownerDocument().createElement("LEFTBORDER");
            lbElem.setAttribute("width", QString::number(width));
            lbElem.setAttribute("style", QString::number(style));
            if (color.isValid()) {
                lbElem.setAttribute("red", color.red());
                lbElem.setAttribute("green", color.green());
                lbElem.setAttribute("blue", color.blue());
            }
            parentElement.appendChild(lbElem);
        }
    }

    if (styleStack.hasProperty(ooNS::fo, "border", "right")) {
        double width;
        int style;
        QColor color;
        if (OoUtils::parseBorder(styleStack.property(ooNS::fo, "border", "right"), &width, &style, &color)) {
            QDomElement lbElem = parentElement.ownerDocument().createElement("RIGHTBORDER");
            lbElem.setAttribute("width", QString::number(width));
            lbElem.setAttribute("style", QString::number(style));
            if (color.isValid()) {
                lbElem.setAttribute("red", color.red());
                lbElem.setAttribute("green", color.green());
                lbElem.setAttribute("blue", color.blue());
            }
            parentElement.appendChild(lbElem);
        }
    }

    if (styleStack.hasProperty(ooNS::fo, "border", "top")) {
        double width;
        int style;
        QColor color;
        if (OoUtils::parseBorder(styleStack.property(ooNS::fo, "border", "top"), &width, &style, &color)) {
            QDomElement lbElem = parentElement.ownerDocument().createElement("TOPBORDER");
            lbElem.setAttribute("width", QString::number(width));
            lbElem.setAttribute("style", QString::number(style));
            if (color.isValid()) {
                lbElem.setAttribute("red", color.red());
                lbElem.setAttribute("green", color.green());
                lbElem.setAttribute("blue", color.blue());
            }
            parentElement.appendChild(lbElem);
        }
    }

    if (styleStack.hasProperty(ooNS::fo, "border", "bottom")) {
        double width;
        int style;
        QColor color;
        if (OoUtils::parseBorder(styleStack.property(ooNS::fo, "border", "bottom"), &width, &style, &color)) {
            QDomElement lbElem = parentElement.ownerDocument().createElement("BOTTOMBORDER");
            lbElem.setAttribute("width", QString::number(width));
            lbElem.setAttribute("style", QString::number(style));
            if (color.isValid()) {
                lbElem.setAttribute("red", color.red());
                lbElem.setAttribute("green", color.green());
                lbElem.setAttribute("blue", color.blue());
            }
            parentElement.appendChild(lbElem);
        }
    }
}

void OoUtils::importUnderline(const QString& in, QString& underline, QString& styleline)
{
    underline = "single";
    if (in == "none")
        underline = "0";
    else if (in == "single")
        styleline = "solid";
    else if (in == "double") {
        underline = in;
        styleline = "solid";
    } else if (in == "dotted" || in == "bold-dotted")  // bold-dotted not in libkotext
        styleline = "dot";
    else if (in == "dash"
             // those are not in libkotext:
             || in == "long-dash"
             || in == "bold-dash"
             || in == "bold-long-dash"
            )
        styleline = "dash";
    else if (in == "dot-dash"
             || in == "bold-dot-dash") // not in libkotext
        styleline = "dashdot"; // tricky ;)
    else if (in == "dot-dot-dash"
             || in == "bold-dot-dot-dash") // not in libkotext
        styleline = "dashdotdot"; // this is getting fun...
    else if (in == "wave"
             || in == "bold-wave" // not in libkotext
             || in == "double-wave" // not in libkotext
             || in == "small-wave") { // not in libkotext
        underline = in;
        styleline = "solid";
    } else if (in == "bold") {
        underline = "single-bold";
        styleline = "solid";
    } else
        qWarning() << " unsupported text-underline value: " << in;
}

void OoUtils::importTextPosition(const QString& text_position, QString& value, QString& relativetextsize)
{
    //OO: <vertical position (% or sub or super)> [<size as %>]
    //Examples: "super" or "super 58%" or "82% 58%" (where 82% is the vertical position)
    // TODO in words: vertical positions other than sub/super
    QStringList lst = text_position.split(' ');
    if (!lst.isEmpty()) {
        QString textPos = lst.front().trimmed();
        QString textSize;
        lst.pop_front();
        if (!lst.isEmpty())
            textSize = lst.front().trimmed();
        if (!lst.isEmpty())
            qWarning() << "Strange text position: " << text_position;
        bool super = textPos == "super";
        bool sub = textPos == "sub";
        if (textPos.endsWith(QLatin1Char('%'))) {
            textPos.chop(1);
            // This is where we interpret the text position into kotext's simpler
            // "super" or "sub".
            double val = textPos.toDouble();
            if (val > 0)
                super = true;
            else if (val < 0)
                sub = true;
        }
        if (super)
            value = "2";
        else if (sub)
            value = "1";
        else
            value = "0";
        if (!textSize.isEmpty() && textSize.endsWith(QLatin1Char('%'))) {
            textSize.chop(1);
            double textSizeValue = textSize.toDouble() / 100; // e.g. 0.58
            relativetextsize = QString::number(textSizeValue);
        }
    } else
        value = "0";
}

void OoUtils::createDocumentInfo(KoXmlDocument &_meta, QDomDocument & docinfo)
{
    KoXmlNode meta   = KoXml::namedItemNS(_meta, ooNS::office, "document-meta");
    KoXmlNode office = KoXml::namedItemNS(meta, ooNS::office, "meta");

    if (office.isNull())
        return;
    QDomElement elementDocInfo  = docinfo.documentElement();

    KoXmlElement e = KoXml::namedItemNS(office, ooNS::dc, "creator");
    if (!e.isNull() && !e.text().isEmpty()) {
        QDomElement author = docinfo.createElement("author");
        QDomElement t = docinfo.createElement("full-name");
        author.appendChild(t);
        t.appendChild(docinfo.createTextNode(e.text()));
        elementDocInfo.appendChild(author);
    }

    e = KoXml::namedItemNS(office, ooNS::dc, "title");
    if (!e.isNull() && !e.text().isEmpty()) {
        QDomElement about = docinfo.createElement("about");
        QDomElement title = docinfo.createElement("title");
        about.appendChild(title);
        title.appendChild(docinfo.createTextNode(e.text()));
        elementDocInfo.appendChild(about);
    }

    e = KoXml::namedItemNS(office, ooNS::dc, "description");
    if (!e.isNull() && !e.text().isEmpty()) {
        QDomElement about = elementDocInfo.namedItem("about").toElement();
        if (about.isNull()) {
            about = docinfo.createElement("about");
            elementDocInfo.appendChild(about);
        }
        QDomElement title = docinfo.createElement("abstract");
        about.appendChild(title);
        title.appendChild(docinfo.createTextNode(e.text()));
    }
    e = KoXml::namedItemNS(office, ooNS::dc, "subject");
    if (!e.isNull() && !e.text().isEmpty()) {
        QDomElement about = elementDocInfo.namedItem("about").toElement();
        if (about.isNull()) {
            about = docinfo.createElement("about");
            elementDocInfo.appendChild(about);
        }
        QDomElement subject = docinfo.createElement("subject");
        about.appendChild(subject);
        subject.appendChild(docinfo.createTextNode(e.text()));
    }
    e = KoXml::namedItemNS(office, ooNS::meta, "keywords");
    if (!e.isNull()) {
        QDomElement about = elementDocInfo.namedItem("about").toElement();
        if (about.isNull()) {
            about = docinfo.createElement("about");
            elementDocInfo.appendChild(about);
        }
        KoXmlElement tmp = KoXml::namedItemNS(e, ooNS::meta, "keyword");
        if (!tmp.isNull() && !tmp.text().isEmpty()) {
            QDomElement keyword = docinfo.createElement("keyword");
            about.appendChild(keyword);
            keyword.appendChild(docinfo.createTextNode(tmp.text()));
        }
    }
}

KoFilter::ConversionStatus OoUtils::loadAndParse(const QString& fileName, KoXmlDocument& doc, KoStore* store)
{
    qDebug() << "loadAndParse: Trying to open" << fileName;

    if (!store->open(fileName)) {
        qWarning() << "Entry " << fileName << " not found!";
        return KoFilter::FileNotFound;
    }
    KoFilter::ConversionStatus convertStatus = loadAndParse(store->device(), doc, fileName);
    store->close();
    return convertStatus;

}

KoFilter::ConversionStatus OoUtils::loadAndParse(QIODevice* io, KoXmlDocument& doc, const QString & fileName)
{
    // Error variables for QDomDocument::setContent
    QString errorMsg;
    int errorLine, errorColumn;
    if (!doc.setContent(io, &errorMsg, &errorLine, &errorColumn)) {
        qWarning() << "Parsing error in " << fileName << "! Aborting!" << Qt::endl
        << " In line: " << errorLine << ", column: " << errorColumn << Qt::endl
        << " Error message: " << errorMsg << Qt::endl;
        return KoFilter::ParsingError;
    }

    qDebug() << "File" << fileName << " loaded and parsed!";

    return KoFilter::OK;
}

KoFilter::ConversionStatus OoUtils::loadAndParse(const QString& filename, KoXmlDocument& doc, KZip* zip)
{
    qDebug() << "Trying to open" << filename;

    if (!zip) {
        qWarning() << "No ZIP file!" << Qt::endl;
        return KoFilter::CreationError; // Should not happen
    }

    const KArchiveEntry* entry = zip->directory()->entry(filename);
    if (!entry) {
        qWarning() << "Entry " << filename << " not found!";
        return KoFilter::FileNotFound;
    }
    if (entry->isDirectory()) {
        qWarning() << "Entry " << filename << " is a directory!";
        return KoFilter::WrongFormat;
    }
    const KZipFileEntry* f = static_cast<const KZipFileEntry *>(entry);
    qDebug() << "Entry" << filename << " has size" << f->size();
    QIODevice* io = f->createDevice();
    KoFilter::ConversionStatus convertStatus = loadAndParse(io, doc, filename);
    delete io;
    return convertStatus;
}

KoFilter::ConversionStatus OoUtils::loadThumbnail(QImage& thumbnail, KZip* zip)
{
    const QString filename("Thumbnails/thumbnail.png");
    qDebug() << "Trying to open thumbnail" << filename;

    if (!zip) {
        qWarning() << "No ZIP file!" << Qt::endl;
        return KoFilter::CreationError; // Should not happen
    }

    const KArchiveEntry* entry = zip->directory()->entry(filename);
    if (!entry) {
        qWarning() << "Entry " << filename << " not found!";
        return KoFilter::FileNotFound;
    }
    if (entry->isDirectory()) {
        qWarning() << "Entry " << filename << " is a directory!";
        return KoFilter::WrongFormat;
    }
    const KZipFileEntry* f = static_cast<const KZipFileEntry *>(entry);
    QIODevice* io = f->createDevice();
    qDebug() << "Entry" << filename << " has size" << f->size();

    if (! io->open(QIODevice::ReadOnly)) {
        qWarning() << "Thumbnail could not be opened!";
        delete io;
        return KoFilter::StupidError;
    }

    if (! thumbnail.load(io, "PNG")) {
        qWarning() << "Thumbnail could not be read!";
        delete io;
        return KoFilter::StupidError;
    }

    io->close();

    if (thumbnail.isNull()) {
        qWarning() << "Read thumbnail is null!";
        delete io;
        return KoFilter::StupidError;
    }

    delete io;

    qDebug() << "File" << filename << " loaded!";

    return KoFilter::OK;
}
