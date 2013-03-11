/*
 * Allocation.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004, 2005 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */
#ifndef _Allocation_h_
#define _Allocation_h_

#include "kplatotj_export.h"

#include <QDomDocument>

#include "ShiftSelectionList.h"

namespace TJ
{

class Resource;
class UsageLimits;

class KPLATOTJ_EXPORT Allocation
{
public:
    Allocation();
    Allocation(const Allocation& a);

    ~Allocation();

    void setLimits(UsageLimits* l);
    const UsageLimits* getLimits() const { return limits; }

    void setPersistent(bool p) { persistent = p; }
    bool isPersistent() const { return persistent; }

    void setMandatory(bool m) { mandatory = m; }
    bool isMandatory() const { return mandatory; }

    void setLockedResource(Resource* r) { lockedResource = r; }
    Resource* getLockedResource() const { return lockedResource; }

    void addCandidate(Resource* r) { candidates.append(r); }
    QListIterator<Resource*> getCandidatesIterator() const
    {
        return QListIterator<Resource*> (candidates);
    }
    QList<Resource*> getCandidates() const { return candidates; }

    /// Return the list of required resources for @p resource
    QList<Resource*> getRequiredResources(Resource *r) const { return requiredResources.value(r); }
    /// Add the @p required resource for @p resource
    void addRequiredResource(Resource *resource, Resource *required)
    {
        requiredResources[resource].append(required);
    }
    bool hasRequiredResources(Resource *resource) const { return requiredResources.contains(resource); }

    bool addShift(const Interval& i, Shift* s)
    {
        return shifts.insert(new ShiftSelection(i, s));
    }

    bool isOnShift(const Interval& i)
    {
        return shifts.isOnShift(i);
    }

    bool isWorker() const;

    enum SelectionModeType
    {
        order, minAllocationProbability, minLoaded, maxLoaded, random
    };
    void setSelectionMode(SelectionModeType smt) { selectionMode = smt; }
    bool setSelectionMode(const QString& smt);
    SelectionModeType getSelectionMode() const { return selectionMode; }

    QDomElement xmlElement(QDomDocument& doc);

    void setConflictStart(time_t cs) { conflictStart = cs; }
    time_t getConflictStart() const { return conflictStart; }

    void init()
    {
        lockedResource = 0;
        conflictStart = 0;
    }

private:
    /**
     * Limit values that regulate how many allocations can be made per
     * day/week/month.
     */
    UsageLimits* limits;

    /// The shifts that can limit the allocation to certain intervals.
    ShiftSelectionList shifts;

    /**
     * True if the allocation should be persistent over the whole task.
     * If set the first selection will not be changed even if there is an
     * available alternative. */
    bool persistent;

    /**
     * True if the resource is mandatory. This means, that only if this
     * allocation can be made, all other allocations will be made. If this
     * allocation cannot be made for a time slot, no other allocations will be
     * made for the task.
     */
    bool mandatory;

    /// The persistent resource picked by the scheduler.
    Resource* lockedResource;

    /// Start of a resource conflict. Used during scheduling only.
    time_t conflictStart;

    /// List of potential resources.
    QList<Resource*> candidates;
    /// Map of required resources
    QMap<Resource*, QList<Resource*> > requiredResources;

    /* The selection mode determines how the resource is selected from
     * the candidate list. */
    SelectionModeType selectionMode;
} ;

} // namespace TJ

#endif
