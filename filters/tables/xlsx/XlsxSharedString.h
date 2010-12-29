/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef XLSXSHAREDSTRING_H
#define XLSXSHAREDSTRING_H

#include <QString>
#include <QByteArray>
#include <QVector>

class KoXmlWriter;

//! Represents a single shared string. Can be plain string or XML string.
/*! Use saveXml() to save the data to a Xml Writer. This wil lad a text span
    or complete xml element, dependent on the type of shared string. */
class XlsxSharedString
{
public:
    //! Constructs empty plain string.
    XlsxSharedString();

    bool isPlainText() const { return m_isPlainText; }
    QString data() const { return m_data; }

    //QString plainText() const { return m_isPlainText ? m_data : QString(); }
    void setPlainText(const QString& plainText) { m_data = plainText; m_isPlainText = true; }

    //QString xml() const { return m_isPlainText ? QString() : m_data; }
    void setXml(const QByteArray& xml) { m_data = xml; m_isPlainText = false; }

private:
    QString m_data;
    bool m_isPlainText;
};

typedef QVector<XlsxSharedString> XlsxSharedStringVector;

#endif //XLSXSHAREDSTRING_H
