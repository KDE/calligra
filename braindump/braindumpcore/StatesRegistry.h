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

#ifndef _STATES_REGISTRY_H_
#define _STATES_REGISTRY_H_

#include <QMap>
#include <QString>

#include "braindumpcore_export.h"

class QSvgRenderer;

class StateCategory;
class StatesRegistry;

class BRAINDUMPCORE_EXPORT State {
    friend class StatesRegistry;
    State( const QString& _id, const QString& _name, StateCategory* _category, const QString& _fileName, int _priority);
    ~State();
  public:
    const QString& name() const;
    const QString& id() const;
    const StateCategory* category() const;
    QSvgRenderer* renderer() const;
    int priority() const;
  private:
    struct Private;
    Private* const d;
};

class BRAINDUMPCORE_EXPORT StateCategory {
    friend class StatesRegistry;
    StateCategory( const QString& _id, const QString& _name, int _priority);
    ~StateCategory();
  public:
    const QString& name() const;
    const QString& id() const;
    QList<QString> stateIds() const;
    const State* state(const QString& ) const;
    int priority() const;
  private:
    struct Private;
    Private* const d;
};

class BRAINDUMPCORE_EXPORT StatesRegistry {
    StatesRegistry();
  public:
    static const StatesRegistry* instance();
    QList<QString> categorieIds() const;
    QList<QString> stateIds(const QString& _id) const;
    const State* state(const QString& _category, const QString& _id) const;
    const State* nextState(const State* _state) const;
  private:
    struct Private;
    Private* const d;
};

#endif
