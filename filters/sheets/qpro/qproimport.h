/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef QPROIMPORT_H
#define QPROIMPORT_H

#include <KoFilter.h>
#include <qpro/tablenames.h>

#include <QByteArray>
#include <QVariantList>

namespace Calligra
{
namespace Sheets
{
class Sheet;
}
}

class QpImport : public KoFilter
{

    Q_OBJECT

public:
    QpImport(QObject* parent, const QVariantList&);
    ~QpImport() override {}

    KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to) override;
    void InitTableName(int pIdx, QString& pResult);

protected:
    void setText(Calligra::Sheets::Sheet* sheet, int row, int column, const QString& text, bool asString = false);
};

class QpTableList : public QpTableNames
{
public:
    QpTableList();
    ~QpTableList();

    void          table(unsigned pIdx, Calligra::Sheets::Sheet* pTable);
    Calligra::Sheets::Sheet* table(unsigned pIdx);
protected:
    Calligra::Sheets::Sheet* cTable[cNameCnt];
};

#endif // QPROIMPORT_H
