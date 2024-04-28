/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KODUMMYCELLVALUE_H
#define KODUMMYCELLVALUE_H

#include "KoCellValue.h"

class KoDummyCellValue : public KoCellValue
{
public:
    KoDummyCellValue();
    ~KoDummyCellValue() override;

protected:
    QString type() const override;
    QList<QPair<QString, QString>> attributes() const override;
};

#endif
