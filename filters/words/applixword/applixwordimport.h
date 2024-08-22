/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 Enno Bartels <ebartels@nwn.de>
   SPDX-FileCopyrightText: 2011 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef APPLIXWORDIMPORT_H
#define APPLIXWORDIMPORT_H

#include <KoFilter.h>
#include <KoStore.h>
#include <QColor>
#include <QFile>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <QVariantList>

class KoOdfWriteStore;
class QByteArray;
class KoGenStyle;

class APPLIXWORDImport : public KoFilter
{
    Q_OBJECT

public:
    APPLIXWORDImport(QObject *parent, const QVariantList &);
    ~APPLIXWORDImport() override = default;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

private:
    QChar specCharfind(QChar, QChar);
    QString readTagLine(QTextStream &);
    void replaceSpecial(QString &);
    QString nextLine(QTextStream &);
    int readHeader(QTextStream &stream);
    bool createMeta(KoOdfWriteStore &store);
    bool parseFontProperty(const QString &type, KoGenStyle &style) const;

private:
    int m_stepsize;
    int m_instep;
    int m_progress;
    QString m_nextPendingLine;
    QMap<QString, QColor> m_colorMap;
};
#endif // APPLIXWORDIMPORT_H
