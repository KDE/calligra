// SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef _STATE_H_
#define _STATE_H_

#include <QString>
#include <QSvgRenderer>

#include "braindumpcore_export.h"

class StateCategory;

class BRAINDUMPCORE_EXPORT State
{
    friend class StatesRegistry;
    State(const QString &_id, const QString &_name, StateCategory *_category, const QString &_fileName, int _priority);
    ~State();

public:
    const QString &name() const;
    const QString &id() const;
    const StateCategory *category() const;
    QSvgRenderer *renderer() const;
    int priority() const;

private:
    struct Private;
    Private *const d;
};

#endif
