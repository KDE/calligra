/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

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
    StateCategory(const QString& _id, const QString& _name, int _priority);
    ~StateCategory();
public:
    const QString& name() const;
    const QString& id() const;
    QList<QString> stateIds() const;
    const State* state(const QString&) const;
    int priority() const;
private:
    struct Private;
    Private* const d;
};

#endif
