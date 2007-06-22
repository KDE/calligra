/* This file is part of the KDE project
   Copyright (c) 2003-2005 Kexi Team

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

#ifndef _KEXI_GLOBAL_
#define _KEXI_GLOBAL_

/*! Only defined for standalone releases, 
 should be commented out for releases bundled with KOffice. *
 Affects translation file names: "kexi.po" becomes "standalone_kexi.po" (used in main.cpp)
 and "kformdesigner.po" becomes "standalone_kformdesigner.po" (used in FormManager)
 */
//#define KEXI_STANDALONE 1

#include <kexi_export.h>
#include <config-kexi.h>

#define kexidbg  kDebug(44010)   //! General debug area for Kexi
#define kexicoredbg  kDebug(44020)   //! Debug area for Kexi Core
#define kexipluginsdbg kDebug(44021) //! Debug area for Kexi Plugins
#define kexiwarn  kWarning(44010)
#define kexicorewarn kWarning(44020)
#define kexipluginswarn kWarning(44021)

/*! nonconst version of foreach iterator */
#define foreach_nonconst(_class, _variable, _list) \
	for (_class _variable = (_list).begin(); _variable!=(_list).end(); ++_variable)

/*! a shortcut for iterating over QPtrList and QPtrDict */
#define foreach_list(_class, _variable, _list) \
	for (_class _variable(_list); _variable.current(); ++_variable)

#define foreach_dict(_class, _variable, _list) foreach_list(_class, _variable, _list)

#ifndef futureI18n
# ifdef USE_FUTURE_I18N
#  define futureI18n(a) QObject::tr(a)
#  define futureI18n2(a,b) QObject::tr(b)
# else
#  define futureI18n(a) QString(a)
#  define futureI18n2(a,b) QString(b)
# endif
#endif

#ifndef FUTURE_I18N_NOOP
# define FUTURE_I18N_NOOP(x) (x)
#endif

#ifdef _MSC_VER
/* WARNING preprocessor directive
 Reserved: preprocessor needs two indirections to replace __LINE__ with actual
 string
*/
#define _MSG0(msg)     #msg
/* Preprocessor needs two indirections to replace __LINE__ or __FILE__
 with actual string. */
#define _MSG1(msg)    _MSG0(msg)

/*! Creates message prolog with the name of the source file and the line
   number where a preprocessor message has been inserted.

  Example:
     #pragma KMESSAGE(Your message)
  Output:
     C:\MyCode.cpp(111) : Your message
*/
# define _MSGLINENO __FILE__ "(" _MSG1(__LINE__) ") : warning: "
# define WARNING(msg) message(_MSGLINENO #msg)
#endif /*_MSC_VER*/

#endif /* _KEXI_GLOBAL_ */
