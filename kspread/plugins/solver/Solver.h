/* This file is part of the KDE project
   Copyright (C) 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_SOLVER
#define KSPREAD_SOLVER

#include <gsl_multimin.h>

#include <kparts/plugin.h>

#include <koffice_export.h>

class QObject;
class QStringList;

namespace KSpread
{
namespace Plugins
{

class KSPREAD_EXPORT Solver : public KParts::Plugin
{
  Q_OBJECT

public:
  struct Parameters
  {
    QList<Cell*> cells;
  };

  Solver( QObject* parent, const QStringList& args );
  ~Solver();
  double evaluate( const gsl_vector* vector, void* parameters );

protected Q_SLOTS:
  void showDialog();
  void optimize();

private:
  class Private;
  Private * const d;
};

} // namespace Plugins
} // namespace KSpread

#endif
