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

#include <KLocalizedString>

#include "SheetsDebug.h"
#include "CalculationSettings.h"
#include "Condition.h"
#include "Map.h"
#include "Style.h"

using namespace Calligra::Sheets;

StyleManager::StyleManager()
        : m_defaultStyle(new CustomStyle())
{
}

StyleManager::~StyleManager()
{
    delete m_defaultStyle;
    qDeleteAll(m_styles);
}

QDomElement StyleManager::save(QDomDocument & doc)
{
    QDomElement styles = doc.createElement("styles");

    m_defaultStyle->save(doc, styles, this);

    CustomStyles::ConstIterator iter = m_styles.constBegin();
    CustomStyles::ConstIterator end  = m_styles.constEnd();

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
            debugSheetsODF << "Style" << name << ":" << style;
        }

        e = e.nextSibling().toElement();
    }

    // reparent all styles
    const QStringList names = styleNames();
    QStringList::ConstIterator it;
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

// Mapping between Oasis and our styles. Only used in loading/saving.
void StyleManager::defineOasisStyle(const QString &oasisName, const QString &styleName)
{
    m_oasisStyles[oasisName] = styleName;
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

    CustomStyles::ConstIterator iter = m_styles.constBegin();
    CustomStyles::ConstIterator end  = m_styles.constEnd();

    while (iter != end) {
        if (iter.value()->parentName() == style->name())
            iter.value()->setParentName(parentName);

        ++iter;
    }

    CustomStyles::iterator i(m_styles.find(style->name()));

    if (i != m_styles.end()) {
        debugSheetsODF << "Erasing style entry for" << style->name();
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

QStringList StyleManager::styleNames(bool includeDefault) const
{
    QStringList list;

    if (includeDefault) list.push_back(i18n("Default"));

    CustomStyles::const_iterator iter = m_styles.begin();
    CustomStyles::const_iterator end  = m_styles.end();

    while (iter != end) {
        list.push_back(iter.key());

        ++iter;
    }

    return list;
}

void StyleManager::clearOasisStyles()
{
    // Now, we can clear the map of styles sorted by OpenDocument name.
    m_oasisStyles.clear();
}

QString StyleManager::openDocumentName(const QString& name) const
{
    return m_oasisStyles.value(name);
}

void StyleManager::dump() const
{
    debugSheetsStyle << "Custom styles:";
    foreach(const QString &name, m_styles.keys()) {
        debugSheetsStyle << name;
    }
}
