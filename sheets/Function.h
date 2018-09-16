/* This file is part of the KDE project
   Copyright (C) 2003,2004 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#ifndef CALLIGRA_SHEETS_FUNCTIONS
#define CALLIGRA_SHEETS_FUNCTIONS

#include <QList>
#include <QStringList>
#include <QVector>

#include "Region.h"
#include "Value.h"

#include "sheets_odf_export.h"


namespace Calligra
{
namespace Sheets
{
class Sheet;
class ValueCalc;
class Function;

typedef QVector<Value> valVector;

struct rangeInfo {
    int col1, col2, row1, row2;
    int columns() {
        return col2 - col1 + 1;
    }
    int rows() {
        return row2 - row1 + 1;
    }
};
struct FuncExtra {
    // here we'll add all the extras a function may need
    Function* function;
    QVector<rangeInfo> ranges;
    QVector<Region> regions;
    Sheet *sheet;
    int myrow, mycol;
};

typedef Value(*FunctionPtr)(valVector, ValueCalc *, FuncExtra *);

/**
 * \ingroup Value
 * A function pointer and context.
 */
class CALLIGRA_SHEETS_ODF_EXPORT Function
{
public:
    Function(const QString& name, FunctionPtr ptr);
    virtual ~Function();
    /**
    setParamCount sets allowed parameter count for a function.
    if max=0, it means max=min. If max=-1, there is no upper limit.
    */
    void setParamCount(int min, int max = 0);
    /** is it okay for the function to receive this many parameters ? */
    bool paramCountOkay(int count);
    /** when set to true, the function can receive arrays. When set to
    false, the auto-array mechanism will be used for arrays (so the
    function will receive simple values, not arrays). */
    void setAcceptArray(bool accept = true);
    bool needsExtra();
    void setNeedsExtra(bool extra);
    QString name() const;
    QString localizedName() const;
    QString helpText() const;
    void setHelpText(const QString& text);
    Value exec(valVector args, ValueCalc *calc, FuncExtra *extra = 0);

    QString alternateName() const;
    void setAlternateName(const QString &name);

private:
    Q_DISABLE_COPY(Function)

    class Private;
    Private * const d;
};

/**
 * \ingroup Value
 * A helper-class to call a function.
 */
class CALLIGRA_SHEETS_ODF_EXPORT FunctionCaller {
public:
    FunctionPtr m_ptr;
    valVector m_args;
    ValueCalc *m_calc;
    FuncExtra *m_extra;

    FunctionCaller(FunctionPtr ptr, const valVector &args, ValueCalc *calc, FuncExtra *extra = 0);
    Value exec();
    Value exec(const valVector &args);
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_FUNCTIONS
