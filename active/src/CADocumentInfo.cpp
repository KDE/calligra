/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "CADocumentInfo.h"

#include <QStringList>
#include <QVariant>

CADocumentInfo::CADocumentInfo (QObject* parent) : QObject (parent)
{

}

CADocumentInfo::CADocumentInfo (CADocumentInfo::DocumentType type, QString name, QString path, QObject* parent)
    : QObject (parent), m_type (type), m_name (name), m_path (path)
{

}

QString CADocumentInfo::name() const
{
    return m_name;
}

QString CADocumentInfo::path() const
{
    return m_path;
}

CADocumentInfo::DocumentType CADocumentInfo::type() const
{
    return m_type;
}

CADocumentInfo::DocumentType CADocumentInfo::documentTypeFromString (QString type)
{
    return typeNameHash().key (type);
}

QString CADocumentInfo::stringFromDocumentType (CADocumentInfo::DocumentType type)
{
    return typeNameHash() [type];
}

CADocumentInfo* CADocumentInfo::fromStringList (QStringList list)
{
    return new CADocumentInfo (documentTypeFromString (list.at (0)), list.at (1), list.at (2));
}

QStringList CADocumentInfo::toStringList() const
{
    QStringList list;
    list << stringFromDocumentType (type()) << name() << path();
    return list;
}

QHash< CADocumentInfo::DocumentType, QString > CADocumentInfo::typeNameHash()
{
    QHash<DocumentType, QString> hash;
    hash[Undefined] = "Undefined";
    hash[TextDocument] = "TextDocument";
    hash[Spreadsheet] = "Spreadsheet";
    hash[Presentation] = "Presentation";
    return hash;
}

bool CADocumentInfo::operator== (const CADocumentInfo& docInfo)
{
    return (docInfo.path() == m_path);
}

#include "CADocumentInfo.moc"
