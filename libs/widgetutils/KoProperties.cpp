/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2006-2008 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoProperties.h"

#include <QDataStream>
#include <QDomDocument>
#include <QIODevice>

class Q_DECL_HIDDEN KoProperties::Private
{
public:
    QMap<QString, QVariant> properties;
};

KoProperties::KoProperties()
    : d(new Private())
{
}

KoProperties::KoProperties(const KoProperties &rhs)
    : d(new Private())
{
    d->properties = rhs.d->properties;
}

KoProperties::~KoProperties()
{
    delete d;
}

QMapIterator<QString, QVariant> KoProperties::propertyIterator() const
{
    return QMapIterator<QString, QVariant>(d->properties);
}

bool KoProperties::isEmpty() const
{
    return d->properties.isEmpty();
}

void KoProperties::load(const QDomElement &root)
{
    d->properties.clear();

    QDomElement e = root;
    QDomNode n = e.firstChild();

    while (!n.isNull()) {
        // We don't nest elements.
        QDomElement e = n.toElement();
        if (!e.isNull()) {
            if (e.tagName() == QLatin1String("property")) {
                const QString name = e.attribute(QStringLiteral("name"));
                const QString value = e.text();
                QDataStream in(QByteArray::fromBase64(value.toLatin1()));
                QVariant v;
                in >> v;
                d->properties[name] = v;
            }
        }
        n = n.nextSibling();
    }
}

bool KoProperties::load(const QString &s)
{
    QDomDocument doc;

    if (!doc.setContent(s))
        return false;
    load(doc.documentElement());

    return true;
}

void KoProperties::save(QDomElement &root) const
{
    QDomDocument doc = root.ownerDocument();
    QMap<QString, QVariant>::ConstIterator it;
    for (it = d->properties.constBegin(); it != d->properties.constEnd(); ++it) {
        QDomElement e = doc.createElement(QStringLiteral("property"));
        e.setAttribute(QStringLiteral("name"), it.key());
        QVariant v = it.value();
        e.setAttribute(QStringLiteral("type"), QString::fromLatin1(v.typeName()));

        QByteArray bytes;
        QDataStream out(&bytes, QIODevice::WriteOnly);
        out << v;
        QDomText text = doc.createCDATASection(QString::fromLatin1(bytes.toBase64()));
        e.appendChild(text);
        root.appendChild(e);
    }
}

QString KoProperties::store(const QString &s) const
{
    QDomDocument doc = QDomDocument(s);
    QDomElement root = doc.createElement(s);
    doc.appendChild(root);

    save(root);
    return doc.toString();
}

void KoProperties::setProperty(const QString &name, const QVariant &value)
{
    // If there's an existing value for this name already, replace it.
    d->properties.insert(name, value);
}

bool KoProperties::property(const QString &name, QVariant &value) const
{
    QMap<QString, QVariant>::const_iterator it = d->properties.constFind(name);
    if (it == d->properties.constEnd()) {
        return false;
    } else {
        value = *it;
        return true;
    }
}

QVariant KoProperties::property(const QString &name) const
{
    return d->properties.value(name, QVariant());
}

int KoProperties::intProperty(const QString &name, int def) const
{
    const QVariant v = property(name);
    if (v.isValid())
        return v.toInt();
    else
        return def;
}

qreal KoProperties::doubleProperty(const QString &name, qreal def) const
{
    const QVariant v = property(name);
    if (v.isValid())
        return v.toDouble();
    else
        return def;
}

bool KoProperties::boolProperty(const QString &name, bool def) const
{
    const QVariant v = property(name);
    if (v.isValid())
        return v.toBool();
    else
        return def;
}

QString KoProperties::stringProperty(const QString &name, const QString &def) const
{
    const QVariant v = property(name);
    if (v.isValid())
        return v.toString();
    else
        return def;
}

bool KoProperties::contains(const QString &key) const
{
    return d->properties.contains(key);
}

QVariant KoProperties::value(const QString &key) const
{
    return d->properties.value(key);
}

bool KoProperties::operator==(const KoProperties &other) const
{
    if (d->properties.count() != other.d->properties.count())
        return false;
    QMapIterator<QString, QVariant> i(d->properties);
    while (i.hasNext()) {
        i.next();
        if (other.d->properties.value(i.key()) != i.value())
            return false;
    }
    return true;
}

KoProperties KoProperties::operator=(const KoProperties &other) const
{
    d->properties = other.d->properties;
    return *this;
}
