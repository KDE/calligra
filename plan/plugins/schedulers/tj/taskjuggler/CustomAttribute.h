/*
 * CustomAttribute.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */
#ifndef _CustomAttribute_h_
#define _CustomAttribute_h_

#include "taskjuggler.h"

namespace TJ
{


/*
 * @short This is the virtual base class for all user defined attribute types.
 * @author Chris Schlaeger <cs@kde.org>
 */
class CustomAttribute
{
public:
    CustomAttribute() { }
    virtual ~CustomAttribute() { }

    virtual CustomAttributeType getType() const { return CAT_Undefined; }
} ;

} // namespace TJ

#endif

