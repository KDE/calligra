// SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef _STATE_CATEGORY_H_
#define _STATE_CATEGORY_H_

#include <QMap>
#include <QString>

#include "braindumpcore_export.h"

class StateCategory;
class StatesRegistry;
class State;

class BRAINDUMPCORE_EXPORT StateCategory
{
    friend class StatesRegistry;
    StateCategory(const QString &_id, const QString &_name, int _priority);
    ~StateCategory();

public:
    const QString &name() const;
    const QString &id() const;
    QList<QString> stateIds() const;
    const State *state(const QString &) const;
    int priority() const;

private:
    struct Private;
    Private *const d;
};

#endif
