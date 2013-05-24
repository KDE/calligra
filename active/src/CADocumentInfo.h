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

#ifndef CADOCUMENTINFO_H
#define CADOCUMENTINFO_H

#include <QObject>
#include <QHash>

class QDataStream;

class CADocumentInfo : public QObject
{
    Q_OBJECT
    Q_ENUMS (DocumentType)
    Q_PROPERTY (DocumentType type READ type NOTIFY typeChanged)
    Q_PROPERTY (QString name READ name NOTIFY nameChanged)
    Q_PROPERTY (QString path READ path NOTIFY pathChanged)

public:
    enum DocumentType { Undefined, TextDocument, Spreadsheet, Presentation };

    explicit CADocumentInfo (QObject* parent = 0);
    explicit CADocumentInfo (DocumentType type, QString name, QString path, QObject* parent = 0);
    DocumentType type() const;
    QString name() const;
    QString path() const;
    QStringList toStringList() const;
    static CADocumentInfo* fromStringList (QStringList list);

    bool operator== (const CADocumentInfo& docInfo);

private:
    DocumentType m_type;
    QString m_name;
    QString m_path;

    static QString stringFromDocumentType (DocumentType type);
    static DocumentType documentTypeFromString (QString type);
    static QHash<DocumentType, QString> typeNameHash();

signals:
    void typeChanged();
    void nameChanged();
    void pathChanged();
};

#endif // CADOCUMENTINFO_H
