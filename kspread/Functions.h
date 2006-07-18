/* This file is part of the KDE project
   Copyright (C) 2003,2004 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#ifndef KSPREAD_FUNCTIONS
#define KSPREAD_FUNCTIONS

#include <QList>
#include <QStringList>
#include <QVector>

class QDomElement;

namespace KSpread
{
class Sheet;
class Value;
class ValueCalc;

typedef QVector<Value> valVector;

struct rangeInfo {
  int col1, col2, row1, row2;
};
struct FuncExtra {
  // here we'll add all the extras a function may need
  QVector<rangeInfo> ranges;
  Sheet *sheet;
  int myrow, mycol;
};

typedef Value (*FunctionPtr)(valVector, ValueCalc *, FuncExtra *);

class Function
{
public:
  Function (const QString& name, FunctionPtr ptr);
  ~Function();
  /**
  setParamCount sets allowed parameter count for a function.
  if max=0, it means max=min. If max=-1, there is no upper limit.
  */
  void setParamCount (int min, int max = 0);
  /** is it okay for the function to receive this many parameters ? */
  bool paramCountOkay (int count);
  /** when set to true, the function can receive arrays. When set to
  false, the auto-array mechamism will be used for arrays (so the
  function will receive simple values, not arrays). */
  void setAcceptArray (bool accept = true);
  bool needsExtra ();
  void setNeedsExtra (bool extra);
  QString name() const;
  QString localizedName() const;
  QString helpText() const;
  void setHelpText( const QString& text );
  Value exec (valVector args, ValueCalc *calc, FuncExtra *extra = 0);

private:
  class Private;
  Private* d;
};

enum ParameterType { KSpread_Int, KSpread_Float, KSpread_String, KSpread_Boolean, KSpread_Any };

class FunctionParameter
{
public:
  FunctionParameter();
  FunctionParameter( const FunctionParameter& param );
  FunctionParameter( const QDomElement& element );

  QString helpText() const { return m_help; }
  ParameterType type() const { return m_type; }
  bool hasRange() const { return m_range; }

private:
  QString m_help;
  ParameterType m_type;
  bool m_range;
};

class FunctionDescription
{
public:
  FunctionDescription();
  FunctionDescription (const QDomElement& element);
  FunctionDescription (const FunctionDescription& desc);

  const QStringList& examples() { return m_examples; }
  const QStringList& syntax() { return m_syntax; }
  const QStringList& related() { return m_related; }
  const QStringList& helpText() const { return m_help; }
  QString name() const { return m_name; }
  ParameterType type() const { return m_type; }

  int params() const { return m_params.count(); }
  FunctionParameter& param( int i ) { return m_params[ i ]; }

  void setGroup( const QString& g ) { m_group = g; }
  QString group() const { return m_group; }

  QString toQML() const;

private:
  QString m_group;
  QStringList m_examples;
  QStringList m_syntax;
  QStringList m_related;
  QStringList m_help;
  QString m_name;
  ParameterType m_type;
  QList<FunctionParameter> m_params;
};

class FunctionRepository
{
public:
  FunctionRepository();
  ~FunctionRepository();

  static FunctionRepository *self();

  void add (Function *function);

  Function *function (const QString& name);

  FunctionDescription *functionInfo (const QString& name);

  /** return functions within a group, or all if no group given */
  QStringList functionNames (const QString& group = QString::null);

  const QStringList &groups () const { return m_groups; }

private:

  class Private;
  Private* d;

  static FunctionRepository* s_self;

  /** loads function descriptions from a XML file */
  void loadFile (const QString& filename);

  QStringList m_groups;

  // no copy or assign
  FunctionRepository( const FunctionRepository& );
  FunctionRepository& operator=( const FunctionRepository& );
};


} // namespace KSpread


#endif // KSPREAD_FUNCTIONS
