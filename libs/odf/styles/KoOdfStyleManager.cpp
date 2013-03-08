/* This file is part of the KDE project
 *
 * Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

// Own
#include "KoOdfStyleManager.h"

// Qt
#include <QHash>
#include <QString>

// KDE
#include <kdebug.h>

// odf lib
#include "KoStore.h"
#include <KoOdfReadStore.h>
#include <KoXmlReader.h>
#include <KoXmlReader.h>
#include <KoXmlNS.h>

#include "KoOdfStyle.h"


// ================================================================
//                 class KoOdfStyleManager


class KoOdfStyleManager::Private
{
public:
    Private() {};

    QHash<QString, KoOdfStyle*> styles;         // name, style
    QHash<QString, KoOdfStyle*> defaultStyles;  // family, style
};


// ----------------------------------------------------------------


KoOdfStyleManager::KoOdfStyleManager()
    : d(new KoOdfStyleManager::Private())
{
}

KoOdfStyleManager::~KoOdfStyleManager()
{
    delete d;
}


KoOdfStyle *KoOdfStyleManager::style(QString &name) const
{
    return d->styles.value(name, 0);
}

void KoOdfStyleManager::setStyle(QString &name, KoOdfStyle *style)
{
    d->styles.insert(name, style);
}


KoOdfStyle *KoOdfStyleManager::defaultStyle(QString &family) const
{
    return d->defaultStyles.value(family, 0);
}

void KoOdfStyleManager::setDefaultStyle(QString &name, KoOdfStyle *family)
{
    d->styles.insert(name, family);
}


void KoOdfStyleManager::clear()
{
    qDeleteAll(d->styles);
    qDeleteAll(d->defaultStyles);
}


// ----------------------------------------------------------------


bool KoOdfStyleManager::loadStyles(KoStore *odfStore)
{
    KoXmlDocument doc;
    QString errorMsg;
    int errorLine;
    int errorColumn;



    // ----------------------------------------------------------------
    // Get style info from content.xml.

    // Try to open content.xml. Return if it failed.
    //kDebug(30503) << "parse content.xml styles";
    if (!odfStore->open("content.xml")) {
        kError(30503) << "Unable to open input file content.xml" << endl;
        return false;
    }

    if (!doc.setContent(odfStore->device(), true, &errorMsg, &errorLine, &errorColumn)) {
        kDebug() << "Error occurred while parsing content.xml "
                 << errorMsg << " in Line: " << errorLine
                 << " Column: " << errorColumn;
        odfStore->close();
        return false;
    }

    // Get the xml node that contains the styles.
    KoXmlNode stylesNode = doc.documentElement();
    stylesNode = KoXml::namedItemNS(stylesNode, KoXmlNS::office, "automatic-styles");

    // Collect info about the styles.
    collectStyleSet(stylesNode);

    odfStore->close(); // end of parsing styles in content.xml

    // ----------------------------------------------------------------
    // Get style info from styles.xml.

    // Try to open and set styles.xml as a KoXmlDocument. Return if it failed.
    if (!odfStore->open("styles.xml")) {
        kError(30503) << "Unable to open input file styles.xml" << endl;
        return false;
    }
    if (!doc.setContent(odfStore->device(), true, &errorMsg, &errorLine, &errorColumn)) {
        kDebug() << "Error occurred while parsing styles.xml "
                 << errorMsg << " in Line: " << errorLine
                 << " Column: " << errorColumn;
        odfStore->close();
        return false;
    }

    // Parse properties of the named styles referred by the automatic styles.
    stylesNode = doc.documentElement();
    stylesNode = KoXml::namedItemNS(stylesNode, KoXmlNS::office, "styles");

    // Collect info about the styles.
    collectStyleSet(stylesNode);

    odfStore->close();
    return true;
}

void KoOdfStyleManager::collectStyleSet(KoXmlNode &stylesNode)
{
    KoXmlElement styleElement;
    forEachElement (styleElement, stylesNode) {

        // FIXME: Handle text:outline-style also.
        QString tagName = styleElement.tagName();
        if (tagName != "style" && tagName != "default-style")
            continue;

        KoOdfStyle *style = new KoOdfStyle;

        // Get the style name and other general information.
        // FIXME: display-name
        QString styleName = styleElement.attribute("name", QString());
        style->setName(styleName);
        QString dummy = styleElement.attribute("family");
        style->setFamily(dummy);
        dummy = styleElement.attribute("parent-style-name", QString());
        style->setParent(dummy);

        // Go through all property lists (like paragraph--properties,
        // text-properties, etc) and collect the attributes from them.
        KoXmlElement propertiesElement;
        forEachElement (propertiesElement, styleElement) {
            kDebug() << propertiesElement.tagName() << propertiesElement.attributeNames();
            kDebug() << propertiesElement.tagName() << propertiesElement.attributeFullNames();
            //collectStyleAttributes(propertiesElement, style);
        }

#if 0 // debug
        kDebug(30503) << "==" << styleName << ":\t"
                      << styleInfo->parent
                      << style->family
                      << style->isDefaultStyle;
#endif
        setStyle(styleName, style);
    }
}

#if 0
void OdtHtmlConverter::collectStyleAttributes(KoXmlElement &propertiesElement, KoOdfInfoe *style)
{
    // font properties
    QString attribute = propertiesElement.attribute("font-family");
    if (!attribute.isEmpty()) {
        attribute = '"' + attribute + '"';
        styleInfo->attributes.insert("font-family", attribute);
    }

    QStringList attributes;
    attributes
        // font
        << "font-style" << "font-variant" << "font-weight" << "font-size"
        // text
        << "text-indent" << "text-align" << "text-decoration" << "white-space"
        // color
        << "color" << "background-color"
        // visual formatting
        << "width" << "min-width" << "max-width"
        << "height" << "min-height" << "max-height" << "line-height" << "vertical-align"
        // border
        << "border-top-width" << "border-bottom-width"
        << "border-left-width" << "border-right-width" << "border-width"
        // border
        << "border-top-color" << "border-bottom-color"
        << "border-left-color" << "border-right-color" << "border-color"
        // border
        << "border-top-style" << "border-bottom-style"
        << "border-left-style" << "border-right-style" << "border-style"
        << "border-top" << "border-bottom" << "border-left" << "border-right" << "border"
        // padding
        << "padding-top" << "padding-bottom" << "padding-left" << "padding-right" << "padding"
        << "margin-top" << "margin-bottom" << "margin-left" << "margin-right" //<< "margin"
        << "auto";

    // Handle all general text formatting attributes
    foreach(const QString &attrName, attributes) {
        QString attrVal = propertiesElement.attribute(attrName);

        if (!attrVal.isEmpty()) {
            styleInfo->attributes.insert(attrName, attrVal);
        }
    }

    // Text Decorations
    attribute = propertiesElement.attribute("text-underline-style");
    if (!attribute.isEmpty() && attribute != "none") {
        styleInfo->attributes.insert("text-decoration", "underline");
    }
    attribute = propertiesElement.attribute("text-overline-style");
    if (!attribute.isEmpty() && attribute != "none") {
        styleInfo->attributes.insert("text-decoration", "overline");
    }
    attribute = propertiesElement.attribute("text-line-through-style");
    if (!attribute.isEmpty() && attribute != "none") {
        styleInfo->attributes.insert("text-decoration", "line-through");
    }

    // Visual Display Model
    attribute = propertiesElement.attribute("writing-mode");
    if (!attribute.isEmpty()) {
        if (attribute == "rl")
            attribute = "rtl";
        else if (attribute == "lr")
            attribute = "ltr";
        else
            attribute = "inherited";
        styleInfo->attributes.insert("direction", attribute);
    }

    // Image align
    attribute = propertiesElement.attribute("horizontal-pos");
    if (!attribute.isEmpty()) {
        //kDebug(30503) << "horisontal pos attribute" << attribute;
        if (attribute == "right" || attribute == "from-left") {
            styleInfo->attributes.insert("float", "right");
            styleInfo->attributes.insert("margin", "5px 0 5px 15px");
        }
        // Center doesn't show very well.
//        else if (attribute == "center") {
//            styleInfo->attributes.insert("display", "block");
//            styleInfo->attributes.insert("margin", "10px auto");
//        }
        else if (attribute == "left") {
            styleInfo->attributes.insert("display", "inline");
            styleInfo->attributes.insert("float", "left");
            styleInfo->attributes.insert("margin","5px 15px 5px 0");
        }
    }

    // Lists and numbering
    if (propertiesElement.hasAttribute("num-format")) {
        attribute = propertiesElement.attribute("num-format");
        if (!attribute.isEmpty()) {
            if (attribute == "1")
                attribute = "decimal";
            else if (attribute == "i")
                attribute = "lower-roman";
            else if (attribute == "I")
                attribute = "upper-roman";
            else if (attribute == "a")
                attribute = "lower-alpha";
            else if (attribute == "A")
                attribute = "upper-alpha";
            else
                attribute = "decimal";
        }
        styleInfo->attributes.insert("list-style-type:", attribute);
        styleInfo->attributes.insert("list-style-position:", "outside");
    }
    else if (propertiesElement.hasAttribute("bullet-char")) {
        attribute = propertiesElement.attribute("bullet-char");
        if (!attribute.isEmpty()) {
            switch (attribute[0].unicode()) {
            case 0x2022:
                attribute = "disc";
                break;
            case 0x25CF:
                attribute = "disc";
                break;
            case 0x25CB:
                attribute = "circle";
                break;
            case 0x25A0:
                attribute = "square";
                break;
            default:
                attribute = "disc";
                break;
            }
        }
        styleInfo->attributes.insert("list-style-type:", attribute);
        styleInfo->attributes.insert("list-style-position:", "outside");
    }
}
#endif
