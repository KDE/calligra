/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kspread_condition.h"
#include "kspread_cell.h"
#include "kspread_table.h"
#include "kspread_layout.h"
#include <qdom.h>
#include <kdebug.h>
#include <float.h>


KSpreadConditions::KSpreadConditions(KSpreadCell *ownerCell)
{
  Q_ASSERT(ownerCell != NULL);

  cell = ownerCell;
}

KSpreadConditions::~KSpreadConditions()
{
    conditionList.clear();
}

bool KSpreadConditions::GetCurrentCondition(KSpreadConditional& condition)
{
  /* for now, the first condition that is true is the one that will be used */

  QValueList<KSpreadConditional>::iterator it;
  double value = cell->valueDouble() * cell->factor(cell->column(),
						    cell->row());

  if (cell->isNumeric() && !cell->table()->getShowFormula())
  {
    for (it = conditionList.begin(); it != conditionList.end(); it++)
    {
      condition = *it;
      switch (condition.m_cond)
      {
      case Equal :
	if (value - condition.val1 < DBL_EPSILON &&
	    value - condition.val1 > (0.0 - DBL_EPSILON))
	{
	  return true;
	}
	break;

      case Superior :
	if( value > condition.val1 )
	{
	  return true;
	}
	break;

      case Inferior :
	if(value < condition.val1 )
	{
	  return true;
	}
	break;

      case SuperiorEqual :
	if( value >= condition.val1 )
	{
	  return true;
	}
	break;

      case InferiorEqual :
	if(value <= condition.val1 )
	{
	  return true;
	}
	break;

      case Between :
	if( ( value > QMIN(condition.val1, condition.val2 ) ) &&
	    ( value < QMAX(condition.val1, condition.val2 ) ) )
	{
	  return true;
	}
	break;

      case Different :
	if( ( value < QMIN(condition.val1, condition.val2 ) ) ||
	    ( value > QMAX(condition.val1, condition.val2) ) )
	{
	  return true;
	}
	break;

      default:
	break;
      }
    }
  }
  return false;
}

QValueList<KSpreadConditional> KSpreadConditions::GetConditionList()
{
  QValueList<KSpreadConditional> list;
  QValueList<KSpreadConditional>::iterator it;

  for (it = conditionList.begin(); it != conditionList.end(); it++)
  {
    list.append(*it);
  }

  return list;
}

void KSpreadConditions::SetConditionList(QValueList<KSpreadConditional> list)
{
  conditionList.clear();
  QValueList<KSpreadConditional>::iterator it;

  for (it = list.begin(); it != list.end(); it++)
  {
    conditionList.append(*it);
  }
}

QDomElement KSpreadConditions::SaveConditions(QDomDocument& doc)
{
  QDomElement conditions = doc.createElement("condition");
  KSpreadConditional condition = conditionList.first();
  QValueList<KSpreadConditional>::iterator it;
  QDomElement child;
  int num = 0;
  QString name;

  for (it = conditionList.begin(); it != conditionList.end(); it++)
  {
    condition = *it;

    /* the name of the element will be "condition<n>"
     * This is unimportant now but in older versions three conditions were
     * hardcoded with names "first" "second" and "third"
     */
    name.setNum(num);
    name.prepend("condition");

    child = doc.createElement(name);
    child.setAttribute("cond", (int)condition.m_cond);
    child.setAttribute("val1", condition.val1);
    child.setAttribute("val2", condition.val2);
    child.setAttribute("color", condition.colorcond.name());
    child.appendChild( cell->createElement( "font", condition.fontcond, doc ));

    conditions.appendChild(child);

    num++;
  }

  if (num == 0)
  {
    /* there weren't any real conditions -- return a null dom element */
    return QDomElement();
  }
  else
  {
    return conditions;
  }
}

void KSpreadConditions::LoadConditions(const QDomElement &element)
{
  QDomNodeList nodeList = element.childNodes();
  KSpreadConditional newCondition;
  bool ok;

  for (int i = 0; i < (int)(nodeList.length()); i++)
  {
    QDomElement conditionElement = nodeList.item(i).toElement();
    ok = true;

    ok = conditionElement.hasAttribute( "cond" ) &&
         conditionElement.hasAttribute( "val1" ) &&
         conditionElement.hasAttribute( "val2" ) &&
         conditionElement.hasAttribute( "color" );

    if (ok) newCondition.m_cond =
	      (Conditional) conditionElement.attribute("cond").toInt( &ok );

    if (ok) newCondition.val1 =
	      conditionElement.attribute("val1").toDouble( &ok );

    if (ok) newCondition.val2 =
	      conditionElement.attribute("val2").toDouble( &ok );

    if (ok) newCondition.colorcond =
	      QColor(conditionElement.attribute("color"));

    QDomElement font = conditionElement.namedItem( "font" ).toElement();
    if ( !font.isNull() )
    {
      newCondition.fontcond = cell->toFont(font);
    }

    if (ok)
    {
      conditionList.append(newCondition);
    }
    else
    {
      kdDebug(36001) << "Error loading condition " << conditionElement.nodeName()<< endl;
    }
  }
}
