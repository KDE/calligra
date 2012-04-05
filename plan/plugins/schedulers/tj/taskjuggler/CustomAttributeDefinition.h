/*
 * CustomAttributeDefinition.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */
#ifndef _CustomAttributeDefinition_h_
#define _CustomAttributeDefinition_h_

#include <QString>

#include "taskjuggler.h"

namespace TJ
{

/*
 * @short Class that stores type and name of a custom attribute. 
 * @author Chris Schlaeger <cs@kde.org>
 */
class CustomAttributeDefinition
{
public:
    CustomAttributeDefinition(const QString& n, CustomAttributeType t) :
        name(n),
        type(t),
        inherit(false)
    { }
    ~CustomAttributeDefinition() { }

    const QString& getName() const { return name; }
    CustomAttributeType getType() const { return type; }
        
    void setInherit(bool i) { inherit = i; }
    bool getInherit() const { return inherit; }

private:
    QString name;
    CustomAttributeType type;
    bool inherit;
} ;

} // namespace TJ

#endif


