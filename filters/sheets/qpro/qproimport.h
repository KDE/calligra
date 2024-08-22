/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef QPROIMPORT_H
#define QPROIMPORT_H

#include <KoFilter.h>
#include <qpro/tablenames.h>

#include <QByteArray>
#include <QLoggingCategory>
#include <QVariantList>

Q_DECLARE_LOGGING_CATEGORY(lcQPro)

namespace Calligra
{
namespace Sheets
{
class SheetBase;
}
}

class QpImport : public KoFilter
{
    Q_OBJECT

public:
    QpImport(QObject *parent, const QVariantList &);
    ~QpImport() override = default;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;
    void InitTableName(int pIdx, QString &pResult);

protected:
    void setText(Calligra::Sheets::SheetBase *sheet, int row, int column, const QString &text, bool asString = false);
};

class QpTableList : public QpTableNames
{
public:
    QpTableList();
    ~QpTableList();

    void table(unsigned pIdx, Calligra::Sheets::SheetBase *pTable);
    Calligra::Sheets::SheetBase *table(unsigned pIdx);

protected:
    Calligra::Sheets::SheetBase *cTable[cNameCnt];
};

#endif // QPROIMPORT_H
