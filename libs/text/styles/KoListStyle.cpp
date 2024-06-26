/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007-2010 Sebastian Sauer <mail@dipe.org>
 * SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
 * SPDX-FileCopyrightText: 2011-2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoListStyle.h"

#include "KoList.h"
#include "KoListLevelProperties.h"

#include "TextDebug.h"
#include <KoGenStyle.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <QBuffer>

class Q_DECL_HIDDEN KoListStyle::Private
{
public:
    Private()
        : styleId(0)
    {
    }

    QString name;
    int styleId;
    QMap<int, KoListLevelProperties> levels;
};

KoListStyle::KoListStyle(QObject *parent)
    : QObject(parent)
    , d(new Private())
{
}

KoListStyle::~KoListStyle()
{
    delete d;
}

bool KoListStyle::operator==(const KoListStyle &other) const
{
    foreach (int level, d->levels.keys()) {
        if (!other.hasLevelProperties(level))
            return false;
        if (!(other.levelProperties(level) == d->levels[level]))
            return false;
    }
    foreach (int level, other.d->levels.keys()) {
        if (!hasLevelProperties(level))
            return false;
    }
    return true;
}

bool KoListStyle::operator!=(const KoListStyle &other) const
{
    return !KoListStyle::operator==(other);
}

void KoListStyle::copyProperties(KoListStyle *other)
{
    d->styleId = other->d->styleId;
    d->levels = other->d->levels;
    setName(other->name());
}

KoListStyle *KoListStyle::clone(QObject *parent)
{
    KoListStyle *newStyle = new KoListStyle(parent);
    newStyle->copyProperties(this);
    return newStyle;
}

QString KoListStyle::name() const
{
    return d->name;
}

void KoListStyle::setName(const QString &name)
{
    if (d->name == name)
        return;
    d->name = name;
    Q_EMIT nameChanged(d->name);
}

int KoListStyle::styleId() const
{
    return d->styleId;
}

void KoListStyle::setStyleId(int id)
{
    d->styleId = id;
    foreach (int level, d->levels.keys()) {
        d->levels[level].setStyleId(id);
    }
}

KoListLevelProperties KoListStyle::levelProperties(int level) const
{
    if (d->levels.contains(level))
        return d->levels.value(level);

    level = qMax(1, level);
    if (d->levels.count()) {
        KoListLevelProperties llp = d->levels.begin().value();
        llp.setLevel(level);
        return llp;
    }
    KoListLevelProperties llp;
    llp.setLevel(level);
    if (d->styleId)
        llp.setStyleId(d->styleId);
    return llp;
}

QTextListFormat KoListStyle::listFormat(int level) const
{
    KoListLevelProperties llp = levelProperties(level);
    QTextListFormat format;
    llp.applyStyle(format);
    return format;
}

void KoListStyle::setLevelProperties(const KoListLevelProperties &properties)
{
    int level = qMax(1, properties.level());
    refreshLevelProperties(properties);
    Q_EMIT styleChanged(level);
}

void KoListStyle::refreshLevelProperties(const KoListLevelProperties &properties)
{
    int level = qMax(1, properties.level());
    KoListLevelProperties llp = properties;
    if (isOulineStyle()) {
        llp.setOutlineList(true);
    }
    llp.setLevel(level);
    d->levels.insert(level, llp);
}

bool KoListStyle::hasLevelProperties(int level) const
{
    return d->levels.contains(level);
}

void KoListStyle::removeLevelProperties(int level)
{
    d->levels.remove(level);
}

void KoListStyle::applyStyle(const QTextBlock &block, int level)
{
    KoList::applyStyle(block, this, level);
}

void KoListStyle::loadOdf(KoShapeLoadingContext &scontext, const KoXmlElement &style)
{
    d->name = style.attributeNS(KoXmlNS::style, "display-name", QString());
    // if no style:display-name is given us the style:name
    if (d->name.isEmpty()) {
        d->name = style.attributeNS(KoXmlNS::style, "name", QString());
    }
    d->name = style.attributeNS(KoXmlNS::style, "name", QString());

    KoXmlElement styleElem;
    forEachElement(styleElem, style)
    {
        KoListLevelProperties properties;
        properties.loadOdf(scontext, styleElem);
        if (d->styleId)
            properties.setStyleId(d->styleId);
        setLevelProperties(properties);
    }

    if (d->levels.isEmpty()) {
        KoListLevelProperties llp;
        llp.setLevel(1);
        llp.setStartValue(1);
        llp.setLabelType(KoListStyle::NumberLabelType);
        llp.setNumberFormat(KoOdfNumberDefinition::Numeric);
        llp.setListItemSuffix(".");
        setLevelProperties(llp);
    }
}

void KoListStyle::saveOdf(KoGenStyle &style, KoShapeSavingContext &context) const
{
    // style:display-name can be used in list styles but not in outline styles
    if (!d->name.isEmpty() && !style.isDefaultStyle() && !isOulineStyle()) {
        style.addAttribute("style:display-name", d->name);
    }
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter elementWriter(&buffer); // TODO pass indentation level
    QMapIterator<int, KoListLevelProperties> it(d->levels);
    while (it.hasNext()) {
        it.next();
        it.value().saveOdf(&elementWriter, context);
    }
    QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
    style.addChildElement("text-list-level-style-content", elementContents);
}

bool KoListStyle::isNumberingStyle() const
{
    QMap<int, KoListLevelProperties>::const_iterator it(d->levels.constBegin());
    for (; it != d->levels.constEnd(); ++it) {
        if (isNumberingStyle(it.value().labelType())) {
            return true;
        }
    }
    return false;
}

bool KoListStyle::isNumberingStyle(int style)
{
    switch (style) {
    case KoListStyle::None:
    case KoListStyle::BulletCharLabelType:
    case KoListStyle::ImageLabelType:
        return false;
    default:
        return true;
    }
}

bool KoListStyle::isOulineStyle() const
{
    QMap<int, KoListLevelProperties>::const_iterator it(d->levels.constBegin());
    for (; it != d->levels.constEnd(); ++it) {
        if (it.value().isOutlineList()) {
            return true;
        }
    }
    return false;
}

QList<int> KoListStyle::listLevels() const
{
    return d->levels.keys();
}
