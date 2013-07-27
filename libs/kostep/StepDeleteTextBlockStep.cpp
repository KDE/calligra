/*
 *     kostep -- handles changetracking using operational transformation for calligra
 *     Copyright (C) 2013  Luke Wolf <Lukewolf101010@gmail.com>
 *
 *     This library is free software; you can redistribute it and/or
 *     modify it under the terms of the GNU Lesser General Public
 *     License as published by the Free Software Foundation; either
 *     version 2.1 of the License, or (at your option) any later version.
 *
 *     This library is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     Lesser General Public License for more details.
 *
 *     You should have received a copy of the GNU Lesser General Public
 *     License along with this library; if not, write to the Free Software
 *     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "StepDeleteTextBlockStep.h"
#include "StepDeleteTextBlockStep_p.h"
#include "StepStepLocation.h"

StepDeleteTextBlockStepPrivate::StepDeleteTextBlockStepPrivate (StepDeleteTextBlockStep * q):q (q)
{
}

StepDeleteTextBlockStepPrivate::~StepDeleteTextBlockStepPrivate ()
{
}

StepDeleteTextBlockStep::StepDeleteTextBlockStep (QObject * parent):d (new
   StepDeleteTextBlockStepPrivate(this)), StepStepBase("Delete TextBlock",parent)
{

}

StepDeleteTextBlockStep::StepDeleteTextBlockStep (QTextBlock * block, QObject* parent):StepStepBase
  ("Delete TextBlock",parent), d(new StepDeleteTextBlockStepPrivate(this))
{

}


StepDeleteTextBlockStep::StepDeleteTextBlockStep (const StepDeleteTextBlockStep & other):
d (new StepDeleteTextBlockStepPrivate (this))
{

}

StepDeleteTextBlockStep::~StepDeleteTextBlockStep ()
{
  delete d;

}

QString StepDeleteTextBlockStep::toXML ()
{
  return "<del type=\"paragraph\" " + location().toString() + " />";

}
