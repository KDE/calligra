/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KOCELLVALUE_H
#define KOCELLVALUE_H

class KoXmlWriter;

#include <QList>
#include <QPair>
#include <QString>

class KoCellValue
{
public:
    KoCellValue();
    virtual ~KoCellValue();

    void saveOdf(KoXmlWriter &writer) const;

protected:
    virtual QString type() const = 0;
    virtual QList<QPair<QString, QString>> attributes() const = 0;
};

#endif
