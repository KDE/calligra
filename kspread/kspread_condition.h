/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 - 2003 The KSpread Team
                             www.koffice.org/kspread

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

#ifndef __kspread_condition_h__
#define __kspread_condition_h__

#include "kspread_global.h"

#include <qvaluelist.h>
#include <qdom.h>
#include <koffice_export.h>
class KSpreadCell;
class KSpreadStyle;
class KoGenStyle;

class QColor;
class QFont;
class QString;

/**
 * Structure to indicate the condition we're testing on a cell and the special
 * font and color that will be applied if it's true
 */
class KSPREAD_EXPORT KSpreadConditional
{
public:
  double         val1;
  double         val2;
  QString *      strVal1;
  QString *      strVal2;
  QColor  *      colorcond;
  QFont   *      fontcond;
  QString *      styleName;
  KSpreadStyle * style;
  Conditional    cond;

  KSpreadConditional();
  ~KSpreadConditional();
  KSpreadConditional( const KSpreadConditional& );
  KSpreadConditional& operator=( const KSpreadConditional& );
};


/**
 * Manages a set of conditions for a cell
 */
class KSpreadConditions
{
 public:

  /**
   * Constructor.  There is no default constructor - you must use this one
   * with the owner cell as a parameter
   */
  KSpreadConditions( const KSpreadCell * ownerCell );
  virtual ~KSpreadConditions();

  /**
   * Use this function to see what conditions actually apply currently
   *
   * @param condition a reference to a condition that will be set to the
   *                  matching condition.  If none of the conditions are true
   *                  then this parameter is undefined on exit (check the
   *                  return value).
   *
   * @return true if one of the conditions is true, false if not.
   */
  bool currentCondition( KSpreadConditional & condition );

  /**
   * Retrieve the current list of conditions we're checking
   */
  QValueList<KSpreadConditional> conditionList() const;

  /**
   * Replace the current list of conditions with this new one
   */
  void setConditionList( const QValueList<KSpreadConditional> & list );

  /**
   * Saves the conditions to a DOM tree structure.
   * @return the DOM element for the conditions.
   */
  QDomElement saveConditions( QDomDocument & doc ) const;

  /**
   * Takes a parsed DOM element and recreates the conditions structure out of
   * it
   */
  void loadConditions( const QDomElement & element );

  void loadOasisConditions( const QDomElement & element );

    void saveOasisConditions( KoGenStyle &currentCellStyle );


  /**
   * returns the style that matches first (or NULL if no condition matches)
   */
  KSpreadStyle * matchedStyle() const { return m_matchedStyle; }

  void checkMatches();

 private:
  KSpreadConditions() {}

    QString saveOasisConditionValue(KSpreadConditional &cond);
    void loadOasisConditionValue( const QString &styleCondition, KSpreadConditional &newCondition );
    void loadOasisValidationValue( const QStringList &listVal, KSpreadConditional &newCondition );
    void loadOasisCondition( QString &valExpression, KSpreadConditional &newCondition );


  const KSpreadCell * m_cell;
  QValueList<KSpreadConditional> m_condList;
  KSpreadStyle * m_matchedStyle;
};


#endif /*defined __kspread_condition_h__  */
