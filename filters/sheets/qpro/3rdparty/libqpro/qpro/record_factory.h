/*
 *  SPDX-FileCopyrightText: 2001 Graham Short. <grahshrt@netscape.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef QPRO_RECORD_FACTORY_H
#define QPRO_RECORD_FACTORY_H

#include <qpro/record.h>
#include <qpro/stream.h>

class QpRecFactory
{
public:
    explicit QpRecFactory(QpIStream &pIn);
    ~QpRecFactory();

    QpRec *nextRecord();

protected:
    QpIStream &cIn;
};

#endif // QPRO_RECORD_FACTORY_H
