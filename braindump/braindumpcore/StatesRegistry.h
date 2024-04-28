// SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef _STATES_REGISTRY_H_
#define _STATES_REGISTRY_H_

#include <QString>

#include "braindumpcore_export.h"

class State;

class BRAINDUMPCORE_EXPORT StatesRegistry
{
    StatesRegistry();
    virtual ~StatesRegistry();

public:
    static const StatesRegistry *instance();
    QList<QString> categorieIds() const;
    QList<QString> stateIds(const QString &_id) const;
    const State *state(const QString &_category, const QString &_id) const;
    const State *nextState(const State *_state) const;

private:
    struct Private;
    Private *const d;
};

#endif
