/*
 * ResourceList.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#ifndef _FlagList_h_
#define _FlagList_h_

#include "QStringList"

namespace TJ
{

class FlagList : public QStringList
{
public:
    FlagList() { }
    virtual ~FlagList() { }

    void addFlag(QString flag)
    {
        if (!hasFlag(flag))
            append(flag);
    }
    void clearFlag(const QString& flag)
    {
        if (contains(flag)) {
            removeAt(indexOf(flag));
        }
    }
    bool hasFlag(const QString& flag) const
    {
        return contains(flag) > 0;
    }
} ;

} // namespace TJ

#endif
