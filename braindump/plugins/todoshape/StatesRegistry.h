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

#include <QString>

class QSvgRenderer;

class State {
  public:
    State( const QString& _name, const QString& _category, const QString& _fileName);
    ~State();
  public:
    const QString& name() const;
    const QString& category() const;
    QSvgRenderer* renderer() const;
  private:
    QString m_name, m_category;
    QSvgRenderer* m_render;
};

class StatesRegistry {
  private:
    
};

#endif
