/* This file is part of the KDE project
   Copyright 1998, 1999 Torben Weis <weis@kde.org>
   Copyright 1999- 2006 The KSpread Team
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __kspread_condition_h__
#define __kspread_condition_h__

#include <qdom.h>
#include <qvaluelist.h>

#include <koffice_export.h>

#include "kspread_global.h"

class QColor;
class QFont;
class QString;
class KoGenStyle;

namespace KSpread
{
class Cell;
class Style;

/**
 * Structure to indicate the condition we're testing on a cell and the special
 * font and color that will be applied if it's true
 */
class KSPREAD_EXPORT Conditional
{
public:
  enum Type { None, Equal, Superior, Inferior, SuperiorEqual,
              InferiorEqual, Between, Different, DifferentTo };

  double         val1;
  double         val2;
  QString *      strVal1;
  QString *      strVal2;
  QColor  *      colorcond;
  QFont   *      fontcond;
  QString *      styleName;
  Style *        style;
  Type           cond;

  Conditional();
  ~Conditional();
  Conditional( const Conditional& );
  Conditional& operator=( const Conditional& );
  bool operator==(const Conditional& other) const;
  inline bool operator!=( const Conditional& other ) const { return !operator==( other ); }
};


/**
 * Manages a set of conditions for a cell
 */
class Conditions
{
 public:

  /**
   * Constructor.  There is no default constructor - you must use this one
   * with the owner cell as a parameter
   */
  Conditions( const Cell * ownerCell );
  virtual ~Conditions();

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
  bool currentCondition( Conditional & condition );

  /**
   * Retrieve the current list of conditions we're checking
   */
  QValueList<Conditional> conditionList() const;

  /**
   * Replace the current list of conditions with this new one
   */
  void setConditionList( const QValueList<Conditional> & list );

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
  Style * matchedStyle() const { return m_matchedStyle; }

  void checkMatches();

  bool operator==( const Conditions& other ) const;
  inline bool operator!=( const Conditions& other ) const { return !operator==( other ); }

 private:
  Conditions() {}

    QString saveOasisConditionValue(Conditional &cond);
    void loadOasisConditionValue( const QString &styleCondition, Conditional &newCondition );
    void loadOasisValidationValue( const QStringList &listVal, Conditional &newCondition );
    void loadOasisCondition( QString &valExpression, Conditional &newCondition );


  const Cell * m_cell;
  QValueList<Conditional> m_condList;
  Style * m_matchedStyle;
};

} // namespace KSpread

#endif /*defined __kspread_condition_h__  */
