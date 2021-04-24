/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KOTABLETEMPLATE_H
#define KOTABLETEMPLATE_H

#include "KoStyle.h"
#include "KoCellStyle.h"

#include <QMap>

class KoTableTemplate : public KoStyle
{
public:
    enum Type {
        BandingColumns,
        BandingRows,
        FirstColumn,
        FirstRow,
        LastColumn,
        LastRow
    };
    Q_DECLARE_FLAGS(Types, Type);

private:
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TableTemplate::Type)

#endif
