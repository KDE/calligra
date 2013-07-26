/*
   kostep -- handles changetracking using operational transformation for calligra
   Copyright (C) 2013  Luke Wolf <Lukewolf101010@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef STEPADDTEXTSTEP_H
#define STEPADDTEXTSTEP_H
#include <../../home/luke/kde4/src/calligra/libs/kostep/StepStepBase.h>
#include "kostep_export.h"
class KOSTEP_EXPORT StepAddTextStep: public StepStepBase
{
public:
  explicit StepAddTextStep (QObject * parent = 0);
  StepAddTextStep (QString text, QObject * parent = 0);
  virtual QString toXML ();

private:
  class StepAddTextStepPrivate * const d;
};

#endif // STEPADDTEXTSTEP_H
