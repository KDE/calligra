/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef CELLCHILD_H
#define CELLCHILD_H

class KoCell;
class KoXmlWriter;
class KoGenStyles;

/**
 * \class KoCellChild
 * \brief represents all the elements that can be contained
 * inside a Cell.
 *
 * \see ODF1.2 table:table-cell §9.1.4
 */

class KoCellChild
{
    friend class KoCell;

public:
    KoCellChild();
    virtual ~KoCellChild();

protected:
    virtual void saveOdf(KoXmlWriter &writer, KoGenStyles &styles) const = 0;
};

#endif
