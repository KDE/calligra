/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Eva Brucherseifer <eva@kde.org>
   SPDX-FileCopyrightText: 2005 Bram Schoenmakers <bramschoenmakers@kde.nl>
   based on kspread csv export filter by David Faure

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef HTMLIMPORT_H
#define HTMLIMPORT_H

#include <KoFilter.h>
#include <QByteArray>
#include <QDir>
#include <QObject>
#include <QStack>
#include <QUrl>
#include <QVariantList>

// namespace Calligra::Sheets
// {
// class Sheet;
// }

class KoOdfWriteStore;
class KoXmlWriter;
class KoGenStyles;
class QDomNode;
class QDomElement;

class HTMLImport : public KoFilter
{
    Q_OBJECT
public:
    HTMLImport(QObject *parent, const QVariantList &);
    virtual ~HTMLImport();

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

Q_SIGNALS:
    void completed();

private:
    KoFilter::ConversionStatus loadUrl(const QUrl &url);
    void parseNode(QDomNode n);
    bool parseTag(QDomElement e);
    bool createStyle();
    bool createMeta();

private:
    enum State { InNone, InFrameset, InBody, InTable, InRow, InCell };
    QStack<State> m_states;

    QDir m_inputDir;
    KoOdfWriteStore *m_store;
    KoXmlWriter *m_manifestWriter;
    KoGenStyles *m_mainStyles;
};

#endif
