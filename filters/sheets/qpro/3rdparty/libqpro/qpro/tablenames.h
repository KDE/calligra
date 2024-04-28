/*
 *  SPDX-FileCopyrightText: 2001 Graham Short. <grahshrt@netscape.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef QPRO_TABLENAMES_H
#define QPRO_TABLENAMES_H

// -----------------------------------------------------------------------

class QpTableNames
{
public:
    enum { cNameCnt = 256 };

    QpTableNames();
    ~QpTableNames();

    void name(unsigned pIdx, const char *pName);
    const char *name(unsigned pIdx);

    int allocated(unsigned pIdx);

protected:
    char *cName[cNameCnt];
};

#endif // QPRO_TABLENAMES_H
