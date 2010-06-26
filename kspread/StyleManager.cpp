/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres, nandres@web.de

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

// own header
#include "StyleManager.h"

#include <QBrush>
#include <QDomDocument>
#include <QDomElement>
#include <QPen>
#include <QStringList>

#include <kdebug.h>
#include <klocale.h>

#include <KoGenStyle.h>
#include <KoOdfStylesReader.h>
#include <KoXmlReader.h>
#include <KoXmlNS.h>

#include "CalculationSettings.h"
#include "Condition.h"
#include "Map.h"
#include "Style.h"

using namespace KSpread;

StyleManager::StyleManager()
        : m_defaultStyle(new CustomStyle())
{
}

StyleManager::~StyleManager()
{
    delete m_defaultStyle;
    qDeleteAll(m_styles);
}

void StyleManager::saveOdf(KoGenStyles &mainStyles)
{
    kDebug(36003) << "StyleManager: Saving default cell style";
    KoGenStyle defStyle = KoGenStyle(KoGenStyle::TableCellStyle, "table-cell");
    defaultStyle()->saveOdf(defStyle, mainStyles, this);

    m_oasisStyles.clear();

    CustomStyles::ConstIterator end = m_styles.constEnd();
    for (CustomStyles::ConstIterator it(m_styles.constBegin()); it != end; ++it) {
        kDebug(36003) << "StyleManager: Saving common cell style" << it.key();
        KoGenStyle customStyle = KoGenStyle(KoGenStyle::TableCellStyle, "table-cell");
        const QString oasisName = (*it)->saveOdf(customStyle, mainStyles, this);
        m_oasisStyles[(*it)->name()] = oasisName;
    }
}

void StyleManager::loadOdfStyleTemplate(KoOdfStylesReader& stylesReader, Map* map)
{
    // reset the map of OpenDocument Styles
    m_oasisStyles.clear();

    // loading default style first
    const KoXmlElement* defStyle = stylesReader.defaultStyle("table-cell");
    if (defStyle) {
        kDebug(36003) << "StyleManager: Loading default cell style";
        Conditions conditions;
      defaultStyle()->loadOdf(stylesReader, *defStyle, "Default", conditions, this, map->parser());
        defaultStyle()->setType(Style::BUILTIN);
        if (map) {
            // Load the default precision to be used, if the (default) cell style
            // is set to arbitrary precision.
            KoXmlNode n = defStyle->firstChild();
            while (!n.isNull()) {
                if (n.isElement() &&
                        n.namespaceURI() == KoXmlNS::style &&
                        n.localName() == "table-cell-properties") {
                    KoXmlElement e = n.toElement();
                    if (n.toElement().hasAttributeNS(KoXmlNS::style, "decimal-places")) {
                        bool ok;
                        const int precision = n.toElement().attributeNS(KoXmlNS::style, "decimal-places").toInt(&ok);
                        if (ok && precision > -1) {
                            kDebug(36003) << "Default decimal precision:" << precision;
                            map->calculationSettings()->setDefaultDecimalPrecision(precision);
                        }
                    }
                }
                n = n.nextSibling();
            }
        }
    } else
        resetDefaultStyle();

    QList<KoXmlElement*> customStyles(stylesReader.customStyles("table-cell").values());
    uint nStyles = customStyles.count();
    for (unsigned int item = 0; item < nStyles; item++) {
        KoXmlElement* styleElem = customStyles[item];
        if (!styleElem) continue;

        // assume the name assigned by the application
        const QString oasisName = styleElem->attributeNS(KoXmlNS::style, "name", QString());

        // then replace by user-visible one (if any)
        const QString name = styleElem->attributeNS(KoXmlNS::style, "display-name", oasisName);
        kDebug(36003) << " StyleManager: Loading common cell style:" << oasisName << " (display name:" << name << ")";

        if (!name.isEmpty()) {
            // The style's parent name will be set in Style::loadOdf(..).
            // After all styles are loaded the pointer to the parent is set.
            CustomStyle * style = new CustomStyle(name);

            Conditions conditions;
            style->loadOdf(stylesReader, *styleElem, name, conditions, this, map->parser());
            // TODO Stefan: conditions
            insertStyle(style);
            // insert it into the the map sorted the OpenDocument name
            m_oasisStyles[oasisName] = name;
            kDebug(36003) << "Style" << name << ":" << style;
        }
    }

    // replace all OpenDocument internal parent names by KSpread's style names
    foreach(CustomStyle* style, m_styles) {
        if (!style->parentName().isNull()) {
            const QString parentOdfName = style->parentName();
            const CustomStyle* parentStyle = this->style(m_oasisStyles.value(parentOdfName));
            if (!parentStyle) {
                kWarning(36003) << parentOdfName << " not found.";
                continue;
            }
            style->setParentName(m_oasisStyles.value(parentOdfName));
            kDebug(36003) << style->name() << " (" << style << ") gets" << style->parentName() << " (" << parentOdfName << ") as parent.";
        } else {
            style->setParentName("Default");
            kDebug(36003) << style->name() << " (" << style << ") has" << style->parentName() << " as parent.";
        }
    }
}

QDomElement StyleManager::save(QDomDocument & doc)
{
    QDomElement styles = doc.createElement("styles");

    m_defaultStyle->save(doc, styles, this);

    CustomStyles::iterator iter = m_styles.begin();
    CustomStyles::iterator end  = m_styles.end();

    while (iter != end) {
        CustomStyle * styleData = iter.value();

        styleData->save(doc, styles, this);

        ++iter;
    }

    return styles;
}

bool StyleManager::loadXML(KoXmlElement const & styles)
{
    bool ok = true;
    KoXmlElement e = styles.firstChild().toElement();
    while (!e.isNull()) {
        QString name;
        if (e.hasAttribute("name"))
            name = e.attribute("name");
        Style::StyleType type = (Style::StyleType)(e.attribute("type").toInt(&ok));
        if (!ok)
            return false;

        if (name == "Default" && type == Style::BUILTIN) {
            if (!m_defaultStyle->loadXML(e, name))
                return false;
            m_defaultStyle->setType(Style::BUILTIN);
        } else if (!name.isNull()) {
            CustomStyle* style = 0;
            if (e.hasAttribute("parent") && e.attribute("parent") == "Default")
                style = new CustomStyle(name, m_defaultStyle);
            else
                style = new CustomStyle(name);

            if (!style->loadXML(e, name)) {
                delete style;
                return false;
            }

            if (style->type() == Style::AUTO)
                style->setType(Style::CUSTOM);
            insertStyle(style);
            kDebug(36003) << "Style" << name << ":" << style;
        }

        e = e.nextSibling().toElement();
    }

    // reparent all styles
    QStringList names = styleNames();
    QStringList::iterator it;
    for (it = names.begin(); it != names.end(); ++it) {
        if (*it != "Default") {
            CustomStyle * styleData = style(*it);
            if (styleData && !styleData->parentName().isNull() && m_styles.value(styleData->parentName()))
                styleData->setParentName(m_styles.value(styleData->parentName())->name());
        }
    }

    return true;
}

void StyleManager::resetDefaultStyle()
{
    delete m_defaultStyle;
    m_defaultStyle = new CustomStyle;
}

void StyleManager::createBuiltinStyles()
{
    CustomStyle * header1 = new CustomStyle(i18n("Header"), m_defaultStyle);
    QFont f(header1->font());
    f.setItalic(true);
    f.setPointSize(f.pointSize() + 2);
    f.setBold(true);
    header1->setFont(f);
    header1->setType(Style::BUILTIN);
    m_styles[ header1->name()] = header1;

    CustomStyle * header2 = new CustomStyle(i18n("Header1"), header1);
    QColor color("#F0F0FF");
    header2->setBackgroundColor(color);
    QPen pen(Qt::black, 1, Qt::SolidLine);
    header2->setBottomBorderPen(pen);
    header2->setType(Style::BUILTIN);

    m_styles[ header2->name()] = header2;
}

CustomStyle * StyleManager::style(QString const & name) const
{
    if (name.isEmpty())
        return 0;
    // on OpenDocument loading
//     if ( !m_oasisStyles.isEmpty() )
    {
        if (m_oasisStyles.contains(name) && m_styles.contains(m_oasisStyles[name]))
            return m_styles.value(m_oasisStyles[name]);
//         return 0;
    }
    if (m_styles.contains(name))
        return m_styles[name];
    if ((name == "Default") || (name == m_defaultStyle->name()))
        return m_defaultStyle;
    return 0;
}

void StyleManager::takeStyle(CustomStyle * style)
{
    const QString parentName = style->parentName();

    CustomStyles::iterator iter = m_styles.begin();
    CustomStyles::iterator end  = m_styles.end();

    while (iter != end) {
        if (iter.value()->parentName() == style->name())
            iter.value()->setParentName(parentName);

        ++iter;
    }

    CustomStyles::iterator i(m_styles.find(style->name()));

    if (i != m_styles.end()) {
        kDebug(36003) << "Erasing style entry for" << style->name();
        m_styles.erase(i);
    }
}

bool StyleManager::checkCircle(QString const & name, QString const & parent)
{
    CustomStyle* style = this->style(parent);
    if (!style || style->parentName().isNull())
        return true;
    if (style->parentName() == name)
        return false;
    else
        return checkCircle(name, style->parentName());
}

bool StyleManager::validateStyleName(QString const & name, CustomStyle * style)
{
    if (m_defaultStyle->name() == name || name == "Default")
        return false;

    CustomStyles::const_iterator iter = m_styles.constBegin();
    CustomStyles::const_iterator end  = m_styles.constEnd();

    while (iter != end) {
        if (iter.key() == name && iter.value() != style)
            return false;

        ++iter;
    }

    return true;
}

void StyleManager::changeName(QString const & oldName, QString const & newName)
{
    CustomStyles::iterator iter = m_styles.begin();
    CustomStyles::iterator end  = m_styles.end();

    while (iter != end) {
        if (iter.value()->parentName() == oldName)
            iter.value()->setParentName(newName);

        ++iter;
    }

    iter = m_styles.find(oldName);
    if (iter != end) {
        CustomStyle * s = iter.value();
        m_styles.erase(iter);
        m_styles[newName] = s;
    }
}

void StyleManager::insertStyle(CustomStyle *style)
{
    const QString base = style->name();
    // do not add the default style
    if (base == "Default" && style->type() == Style::BUILTIN)
        return;
    int num = 1;
    QString name = base;
    while (name == "Default" || (m_styles.contains(name) && (m_styles[name] != style))) {
        name = base;
        name += QString::number(num++);
    }
    if (base != name)
        style->setName(name);
    m_styles[name] = style;
}

QStringList StyleManager::styleNames() const
{
    QStringList list;

    list.push_back(i18n("Default"));

    CustomStyles::const_iterator iter = m_styles.begin();
    CustomStyles::const_iterator end  = m_styles.end();

    while (iter != end) {
        list.push_back(iter.key());

        ++iter;
    }

    return list;
}

Styles StyleManager::loadOdfAutoStyles(KoOdfStylesReader& stylesReader,
                                       QHash<QString, Conditions>& conditionalStyles,
                                       const ValueParser *parser)
{
    Styles autoStyles;
    foreach(KoXmlElement* element, stylesReader.autoStyles("table-cell")) {
        if (element->hasAttributeNS(KoXmlNS::style , "name")) {
            QString name = element->attributeNS(KoXmlNS::style , "name" , QString());
            kDebug(36003) << "StyleManager: Preloading automatic cell style:" << name;
            autoStyles.remove(name);
            Conditions conditions;
            autoStyles[name].loadOdfStyle(stylesReader, *(element), conditions, this, parser);
            if (!conditions.isEmpty()) {
                kDebug() << "\t\tCONDITIONS";
                conditionalStyles[name] = conditions;
            }

            if (element->hasAttributeNS(KoXmlNS::style, "parent-style-name")) {
                const QString parentOdfName = element->attributeNS(KoXmlNS::style, "parent-style-name", QString());
                if (parentOdfName == "Default")
                    continue;
                const CustomStyle* parentStyle = style(m_oasisStyles.value(parentOdfName));
                if (!parentStyle) {
                    kWarning(36003) << parentOdfName << " not found.";
                    continue;
                }
                autoStyles[name].setParentName(parentStyle->name());
                kDebug(36003) << "\t parent-style-name:" << autoStyles[name].parentName();
            }
        }
    }
    return autoStyles;
}

void StyleManager::releaseUnusedAutoStyles(Styles autoStyles)
{
    // Just clear the list. The styles are released, if not used.
    autoStyles.clear();

    // Now, we can clear the map of styles sorted by OpenDocument name.
    m_oasisStyles.clear();
}

QString StyleManager::openDocumentName(const QString& name) const
{
    return m_oasisStyles.value(name);
}

void StyleManager::dump() const
{
    kDebug(36006) << "Custom styles:";
    foreach(QString name, m_styles.keys()) {
        kDebug(36006) << name;
    }
}
