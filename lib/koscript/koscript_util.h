/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#ifndef __KSCRIPT_UTIL_H__
#define __KSCRIPT_UTIL_H__

#include "koscript_value.h"
#include <koffice_export.h>

#include <qrect.h>

class KSContext;
class KSStruct;

/**
 * Utility functions for checking arguments and creating exceptions.
 *
 * @author Torben Weis <weis@kde.org>
 */
class KOSCRIPT_EXPORT KSUtil
{
public:
    /**
     * Checks whether @p method has enough arguments (@p count).
     *
     * @return TRUE on success, otherwise FALSE and raises one of @ref #tooFewArgumentsError or
     * @ref #tooManyArgumentsError exceptions
     */
    static bool checkArgumentsCount( KSContext& context, uint count, const QString& method, bool fatal = true );
    /**
     * @return TRUE if the value matches the requested type @p t or if KSValue will seamlessly
     *         convert to that type.
     *
     * @param context is used if @p fatal is TRUE and an exception has to be created.
     * @param v is the value that is to be tested.
     * @param t is the type @p v has to match.
     * @param fatal determines whether an exception is set on error.
     */
    static bool checkType( KSContext& context, KSValue* v, KSValue::Type t, bool fatal = true );
    /**
     * @return TRUE if the value matches the requested type @p t or if KSValue will seamlessly
     *         convert to that type.
     *
     * @param context is used if @p fatal is TRUE and an exception has to be created.
     * @param v is the value that is to be tested.
     * @param t is the type @p v has to match.
     * @param fatal determines whether an exception is set on error.
     */
    static bool checkType( KSContext& context, const KSValue::Ptr& v, KSValue::Type t, bool fatal = true );
    /**
     * Creates an exception for @p context telling that the value @p v did not
     * match the type @p t.
     */
    static void castingError( KSContext& context, KSValue* v, KSValue::Type t );
    /**
     * Creates an exception for @p context telling that the type @p from was
     * requested but @p to appreaed.
     */
    static void castingError( KSContext& context, const QString& from, const QString& to );
    /**
     * Creates an exception for @p context telling that the passed parameters do not
     * match the methods signature.
     */
    static void argumentsMismatchError( KSContext& context, const QString& methodname );
    /**
     * Creates an exception for @p context telling that there were too few arguments
     * in the call to method @p methodname.
     */
    static void tooFewArgumentsError( KSContext& context, const QString& methodname );
    /**
     * Creates an exception for @p context telling that there were too many arguments
     * in the call to method @p methodname.
     */
    static void tooManyArgumentsError( KSContext& context, const QString& methodname );

    /**
     * A convenience function that extracts the arguemnts out of "context.value()". It checks
     * whether this value is really a list.
     */
    static bool checkArgs( KSContext& context, const QCString& signature, const QString& method, bool fatal = TRUE );
    /**
     * Checks whether the argument list passed in @p args matches the @p signature.
     *
     * @param context is used if @p fatal is TRUE and an exception has to be created.
     * @param args is the list of arguments.
     * @param signature is the functions signature (see below)
     * @param method is the name of the method for which we test the argument list.
     * @param fatal determines whether an exception is set on error.
     *
     * @return TRUE if the check was successful.
     *
     * The signature is a sequence of the following characters:
     * <ul>
     * <li> f = float
     * <li> b = bool
     * <li> s = string
     * <li> i = integer
     * <li> [] = a list
     * <li> {} = a map
     * <li> Om:n; = An object of module "m" and name "n".
     * <li> Sm:n; = An struct of module "m" and name "n".
     * </ul>
     * You may put "|" between the characters. This means that the list
     * of arguments may end now. If there are still arguments left then they
     * have to match with the rest of the signature.
     *
     * Example: "iSqt:QRect;f|b" is a signature that takes
     * 1) an integer
     * 2) a struct named "QRect" in module "qt"
     * 3) a float
     * 4) an optional boolean
     */
    static bool checkArgs( KSContext& context, const QValueList<KSValue::Ptr>& args,
			   const QCString& signature, const QString& method, bool fatal = TRUE );
    /**
     * Checks whether the argument passed in @p args matches the @p signature.
     *
     * @param context is used if @p fatal is TRUE and an exception has to be created.
     * @param arg is the argument to check
     * @param signature is the functions signature (see below)
     * @param method is the name of the method for which we test the argument list.
     * @param fatal determines whether an exception is set on error.
     *
     * @return TRUE if the check was successful.
     *
     * The signature is a sequence of the following characters:
     * <ul>
     * <li> f = float
     * <li> b = bool
     * <li> s = string
     * <li> i = integer
     * <li> [] = a list
     * <li> {} = a map
     * <li> Om:n; = An object of module "m" and name "n".
     * <li> Sm:n; = An struct of module "m" and name "n".
     * </ul>
     *
     * Example: "Sqt:QRect;" is a signature that takes
     *          a struct named "QRect" in module "qt"
     */
    static bool checkArg( KSContext& context, const KSValue::Ptr& arg,
			  const QCString& signature, const QString& method = "", bool fatal = FALSE );
};

#endif
