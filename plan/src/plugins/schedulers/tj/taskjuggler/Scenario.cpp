/*
 * Scenario.cpp - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#include "Scenario.h"
#include "Project.h"

namespace TJ
{

Scenario::Scenario(Project* p, const QString& i, const QString& n,
                   Scenario* pr) :
    CoreAttributes(p, i, n, pr),
    enabled(true),
    projectionMode(false),
    strictBookings(false),
    optimize(false),
    minSlackRate(0.05),
    maxPaths(10000000)
{
    p->addScenario(this);
    if (pr)
    {
        // Inherit settings from parent scenario.
        enabled = pr->enabled;
        projectionMode = pr->projectionMode;
        optimize = pr->optimize;
        strictBookings = pr->strictBookings;
        minSlackRate = pr->minSlackRate;
        maxPaths = pr->maxPaths;
    }
}

Scenario::~Scenario()
{
    project->deleteScenario(this);
}

ScenarioListIterator
Scenario::getSubListIterator() const
{
    return ScenarioListIterator(*sub);
}

} // namespace TJ
