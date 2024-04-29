/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Norbert Andres nandres @web.de

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// own header
#include "StyleManager.h"

#include <QFont>

#include <KLocalizedString>

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

void StyleManager::resetDefaultStyle()
{
    delete m_defaultStyle;
    m_defaultStyle = new CustomStyle;
    Q_EMIT styleListChanged();
}

void StyleManager::createBuiltinStyles()
{
    CustomStyle *header1 = new CustomStyle(i18n("Header"), m_defaultStyle);
    QFont f(header1->font());
    f.setItalic(true);
    f.setPointSize(f.pointSize() + 2);
    f.setBold(true);
    header1->setFont(f);
    header1->setType(Style::BUILTIN);
    m_styles[header1->name()] = header1;

    CustomStyle *header2 = new CustomStyle(i18n("Header1"), header1);
    QColor color(0xF0, 0xF0, 0xFF);
    header2->setBackgroundColor(color);
    QPen pen(Qt::black, 1, Qt::SolidLine);
    header2->setBottomBorderPen(pen);
    header2->setType(Style::BUILTIN);

    m_styles[header2->name()] = header2;

    Q_EMIT styleListChanged();
}

// Mapping between Oasis and our styles. Only used in loading/saving.
void StyleManager::defineOasisStyle(const QString &oasisName, const QString &styleName)
{
    m_oasisStyles[oasisName] = styleName;
}

CustomStyle *StyleManager::style(QString const &name) const
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

void StyleManager::takeStyle(CustomStyle *style)
{
    const QString parentName = style->parentName();

    CustomStyles::ConstIterator iter = m_styles.constBegin();
    CustomStyles::ConstIterator end = m_styles.constEnd();

    while (iter != end) {
        if (iter.value()->parentName() == style->name())
            iter.value()->setParentName(parentName);

        ++iter;
    }

    CustomStyles::iterator i(m_styles.find(style->name()));

    if (i != m_styles.end()) {
        debugSheetsODF << "Erasing style entry for" << style->name();
        m_styles.erase(i);
        Q_EMIT styleListChanged();
    }
}

bool StyleManager::checkCircle(QString const &name, QString const &parent)
{
    CustomStyle *style = this->style(parent);
    if (!style || style->parentName().isNull())
        return true;
    if (style->parentName() == name)
        return false;
    else
        return checkCircle(name, style->parentName());
}

bool StyleManager::validateStyleName(QString const &name)
{
    if (m_defaultStyle->name() == name || name == "Default")
        return false;

    CustomStyles::const_iterator iter = m_styles.constBegin();
    CustomStyles::const_iterator end = m_styles.constEnd();

    while (iter != end) {
        if (iter.key() == name)
            return false;

        ++iter;
    }

    return true;
}

void StyleManager::changeName(QString const &oldName, QString const &newName)
{
    CustomStyles::iterator iter = m_styles.begin();
    CustomStyles::iterator end = m_styles.end();

    while (iter != end) {
        if (iter.value()->parentName() == oldName)
            iter.value()->setParentName(newName);

        ++iter;
    }

    iter = m_styles.find(oldName);
    if (iter != end) {
        CustomStyle *s = iter.value();
        m_styles.erase(iter);
        m_styles[newName] = s;
        Q_EMIT styleListChanged();
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
    Q_EMIT styleListChanged();
}

QStringList StyleManager::styleNames(bool includeDefault) const
{
    QStringList list;

    if (includeDefault)
        list.push_back(i18n("Default"));

    CustomStyles::const_iterator iter = m_styles.begin();
    CustomStyles::const_iterator end = m_styles.end();

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

QString StyleManager::openDocumentName(const QString &name) const
{
    return m_oasisStyles.value(name);
}

void StyleManager::dump() const
{
    debugSheetsStyle << "Custom styles:";
    for (const QString &name : m_styles.keys()) {
        debugSheetsStyle << name;
    }
}
