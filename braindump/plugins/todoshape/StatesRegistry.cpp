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

#include "StatesRegistry.h"

#include <QSvgRenderer>

State::State( const QString& _name, const QString& _category, const QString& _fileName) : m_name(_name), m_category(_category), m_render(new QSvgRenderer(_fileName))
{
}

State::~State() {
  delete m_render;
}

const QString& State::name() const {
  return m_name;
}

const QString& State::category() const {
  return m_category;
}

QSvgRenderer* State::renderer() const {
  return m_render;
}
