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
#include <QPair>

// odf lib
#include "KoStore.h"
#include <KoXmlStreamReader.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

#include "KoOdfStyle.h"
#include "KoOdfListStyle.h"
#include "Odf2Debug.h"


// ================================================================
//                 class KoOdfStyleManager


class Q_DECL_HIDDEN KoOdfStyleManager::Private
{
public:
    Private() {};

    QHash<QPair<QString, QString>, KoOdfStyle*> styles;         // <name, family>, style
    QHash<QString, KoOdfStyle*> defaultStyles;  // family, style
    QHash<QString, KoOdfListStyle*> listStyles; // list-style styles
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

KoOdfStyle *KoOdfStyleManager::style(const QString &name, const QString &family) const
{
    debugOdf2 << d->styles.value(qMakePair(name, family), 0);
    return d->styles.value(qMakePair(name, family), 0);
}

void KoOdfStyleManager::setStyle(const QString &name, KoOdfStyle *style)
{
    d->styles.insert(qMakePair(name, style->family()), style);
}

KoOdfListStyle *KoOdfStyleManager::listStyle(const QString &name) const
{
    return d->listStyles.value(name, 0);
}

void KoOdfStyleManager::setListStyle(const QString &name, KoOdfListStyle *style)
{
    d->listStyles.insert(name, style);
}


KoOdfStyle *KoOdfStyleManager::defaultStyle(const QString &family) const
{
    return d->defaultStyles.value(family, 0);
}

void KoOdfStyleManager::setDefaultStyle(const QString &family, KoOdfStyle *style)
{
    d->defaultStyles.insert(family, style);
}

QList<KoOdfStyle*> KoOdfStyleManager::styles() const
{
    return d->styles.values();
}

QList<KoOdfStyle*> KoOdfStyleManager::defaultStyles() const
{
    return d->defaultStyles.values();
}

void KoOdfStyleManager::clear()
{
    // The style manager owns the styles so we should delete them, not
    // just empty the lists.
    qDeleteAll(d->styles);
    d->styles.clear();

    qDeleteAll(d->defaultStyles);
    d->defaultStyles.clear();
}


// ----------------------------------------------------------------


bool KoOdfStyleManager::loadStyles(KoStore *odfStore)
{
    QString errorMsg;
    //int errorLine;
    //int errorColumn;

    KoXmlStreamReader reader;
    prepareForOdf(reader);

    // ----------------------------------------------------------------
    // Get styles from styles.xml.

    debugOdf2 << "================================================================\n"
             << "Loading styles from styles.xml";

    // Try to open and set styles.xml as a KoXmlDocument. Return if it failed.
    if (!odfStore->open("styles.xml")) {
        errorOdf2 << "Unable to open input file styles.xml" << endl;
        return false;
    }

    reader.setDevice(odfStore->device());
    // FIXME: Error handling

    // Collect the styles.
    collectStyleSet(reader, true /*fromStylesXml*/);
    odfStore->close();

    // ----------------------------------------------------------------
    // Get styles from content.xml.

    // Try to open content.xml. Return if it failed.
    //debugOdf2 << "parse content.xml styles";
    if (!odfStore->open("content.xml")) {
        errorOdf2 << "Unable to open input file content.xml" << endl;
        return false;
    }

    debugOdf2 << "================================================================\n"
             << "Loading styles from content.xml";

    reader.setDevice(odfStore->device());
    // FIXME: Error handling

    // Collect the styles.
    collectStyleSet(reader, false /*!fromStylesXml*/);

    odfStore->close(); // end of parsing styles in content.xml

    return true;
}

void KoOdfStyleManager::collectStyleSet(KoXmlStreamReader &reader, bool fromStylesXml)
{
    debugOdf2 << "incoming element:" << reader.qualifiedName().toString();

    while (!reader.atEnd() && !reader.isEndDocument ()) {
        reader.readNext();
        if (!reader.isStartElement()) {
            continue;
        }
        debugOdf2 << "---------------- style element:" << reader.qualifiedName().toString();
        QString tagName = reader.qualifiedName().toString();

            if (tagName == "office:styles"
                || tagName == "office:automatic-styles"
                || tagName == "office:document-content"
                || tagName == "office:document-styles")
            {
                continue;
            }

            // For now: handle style:style and style:default-style and text:list-style
            // and only the text, paragraph and graphic families.
            if (tagName != "style:style" && tagName != "style:default-style" && tagName != "text:list-style") {
                reader.skipCurrentElement();
                continue;
            }

            KoXmlStreamAttributes  attrs = reader.attributes();
#if 0  // debug
            debugOdf2 << "Attributes:";
            for (int i = 0; i < attrs.size(); ++i) {
                debugOdf2 << "  " << attrs[i].qualifiedName().toString()
                         << attrs[i].value().toString();
            }
#endif

            if ( tagName == "style:style" || tagName == "style:default-style") {
                QString family = attrs.value("style:family").toString();
                if (family == "text" || family == "paragraph" || family == "graphic") {
                    // FIXME: In the future, create style per type (family).
                    KoOdfStyle *style = new KoOdfStyle;

                    style->setIsFromStylesXml(fromStylesXml);
                    //debugOdf2 << "This style should be loaded:" << tagName << "Family:" <<family;

                    style->setIsDefaultStyle(tagName == "style:default-style");
                    style->readOdf(reader);
#if 0 // debug
                    debugOdf2 << "==" << styleName << ":\t"
                                  << style->family()
                                  << style->parent()
                                  << style->isDefaultStyle;
#endif
                    if (style->isDefaultStyle()) {
                        QString family = style->family();
                        setDefaultStyle(family, style);
                    }
                    else {
                        QString styleName = style->name();
                        setStyle(styleName, style);
                    }
                }
            }
            else if (tagName == "text:list-style"){
                KoOdfListStyle *listStyle = new KoOdfListStyle;
                listStyle->readOdf(reader);
                QString styleName = listStyle->name();
                setListStyle(styleName, listStyle);
            }
            else {
                reader.skipCurrentElement();
                continue;
            }
    }
}


bool KoOdfStyleManager::saveNamedStyles(KoXmlWriter *writer)
{
    foreach(KoOdfStyle *style, d->defaultStyles) {
        style->saveOdf(writer);
    }
    foreach(KoOdfStyle *style, d->styles) {
        // FIXME: When we support named styles of types that may use
        //        automatic styles, e.g. page layouts (with
        //        headers/footers), then this logic needs to be more
        //        advanced.
        if (!style->displayName().isEmpty()) {
            style->saveOdf(writer);
        }
    }
    return true;
}
