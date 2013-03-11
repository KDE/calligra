/*
 * Scenario.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */
#ifndef _Scenario_h_
#define _Scenario_h_

#include "kplatotj_export.h"

#include "ScenarioList.h"

class QString;

namespace TJ
{

class Project;

class KPLATOTJ_EXPORT Scenario : public CoreAttributes
{
    friend int ScenarioList::compareItemsLevel(CoreAttributes* c1,
                                               CoreAttributes* c2,
                                               int level);
public:
    Scenario(Project* p, const QString& i, const QString& n, Scenario* s);
    virtual ~Scenario();

    virtual CAType getType() const { return CA_Scenario; }

    Scenario* getParent() const { return static_cast<Scenario*>(parent); }

    ScenarioListIterator getSubListIterator() const;

    void setEnabled(bool e) { enabled = e; }
    bool getEnabled() const { return enabled; }

    void setProjectionMode(bool p) { projectionMode = p; }
    bool getProjectionMode() const  { return projectionMode; }

    void setOptimize(bool o) { optimize = o; }
    bool getOptimize() const { return optimize; }

    void setStrictBookings(bool s) { strictBookings = s; }
    bool getStrictBookings() const { return strictBookings; }

    void setMinSlackRate(double msr) { minSlackRate = msr; }
    double getMinSlackRate() const { return minSlackRate; }

    void setMaxPaths(long l) { maxPaths = l; }
    long getMaxPaths() const { return maxPaths; }

private:
    bool enabled;
    bool projectionMode;
    bool strictBookings;
    bool optimize;
    double minSlackRate;
    long maxPaths;
} ;

} // namespace TJ

#endif
