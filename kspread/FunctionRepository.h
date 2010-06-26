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


#ifndef KSPREAD_FUNCTION_REPOSITORY
#define KSPREAD_FUNCTION_REPOSITORY

#include <QSharedPointer>
#include <QStringList>

#include "kspread_export.h"

namespace KSpread
{
class Function;
class FunctionDescription;

/**
 * \ingroup Value
 * The function repository.
 */
class KSPREAD_EXPORT FunctionRepository
{
public:
    FunctionRepository();
    ~FunctionRepository();

    static FunctionRepository *self();

    /**
     * Adds \p function to the repository.
     */
    void add(const QSharedPointer<Function>& function);
    void add(FunctionDescription *desc);

    /**
     * Removes \p function from the repository.
     * The Function object and the appropriate description will be destroyed.
     */
    void remove(const QSharedPointer<Function>& function);

    QSharedPointer<Function> function(const QString& name);

    FunctionDescription *functionInfo(const QString& name);

    /** return functions within a group, or all if no group given */
    QStringList functionNames(const QString& group = QString());

    const QStringList &groups() const;
    void addGroup(const QString& groupname);

    /**
     * Loads function descriptions from an XML file.
     */
    void loadFunctionDescriptions(const QString& filename);

private:

    class Private;
    Private * const d;

    // no copy or assign
    FunctionRepository(const FunctionRepository&);
    FunctionRepository& operator=(const FunctionRepository&);
};

} // namespace KSpread

#endif // KSPREAD_FUNCTION_REPOSITORY
