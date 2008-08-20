/* This file is part of the KDE project
   Copyright (C) 2003-2007 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef _KEXI_EXPORT_H_
#define _KEXI_EXPORT_H_

#include <kexidb/kexidb_export.h>

#ifndef KEXICORE_EXPORT
# ifdef MAKE_KEXICORE_LIB
#  define KEXICORE_EXPORT KDE_EXPORT
# else
#  define KEXICORE_EXPORT KDE_IMPORT
# endif
#endif

#ifndef KEXIMAIN_EXPORT
# ifdef MAKE_KEXIMAIN_LIB
#  define KEXIMAIN_EXPORT KDE_EXPORT
# else
#  define KEXIMAIN_EXPORT KDE_IMPORT
# endif
#endif

#ifndef KEXIDATATABLE_EXPORT
# ifdef MAKE_KEXIDATATABLE_LIB
#  define KEXIDATATABLE_EXPORT  KDE_EXPORT
# else
#  define KEXIDATATABLE_EXPORT  KDE_IMPORT
# endif
#endif

#ifndef KEXIEXTWIDGETS_EXPORT
# ifdef MAKE_KEXIEXTENDEDWIDGETS_LIB
#  define KEXIEXTWIDGETS_EXPORT  KDE_EXPORT
# else
#  define KEXIEXTWIDGETS_EXPORT  KDE_IMPORT
# endif
#endif

#ifndef KFORMEDITOR_EXPORT
# ifdef MAKE_KFORMEDITOR_LIB
#  define KFORMEDITOR_EXPORT  KDE_EXPORT
# else
#  define KFORMEDITOR_EXPORT  KDE_IMPORT
# endif
#endif

#ifndef KEXIRELATIONSVIEW_EXPORT
# ifdef MAKE_KEXIRELATIONSVIEW_LIB
#  define KEXIRELATIONSVIEW_EXPORT  KDE_EXPORT
# else
#  define KEXIRELATIONSVIEW_EXPORT  KDE_IMPORT
# endif
#endif

#ifndef KEXIGUIUTILS_EXPORT
# ifdef MAKE_KEXIGUIUTILS_LIB
#  define KEXIGUIUTILS_EXPORT KDE_EXPORT
# else
#  define KEXIGUIUTILS_EXPORT KDE_IMPORT
# endif
#endif

#ifndef KROSS_MAIN_EXPORT
# ifdef MAKE_KROSS_MAIN_LIB
#  define KROSS_MAIN_EXPORT KDE_EXPORT
# else
#  define KROSS_MAIN_EXPORT KDE_IMPORT
# endif
#endif

#ifndef KEXIFORMUTILS_EXPORT
# ifdef MAKE_KEXIFORMUTILS_LIB
#  define KEXIFORMUTILS_EXPORT KDE_EXPORT
# else
#  define KEXIFORMUTILS_EXPORT KDE_IMPORT
# endif
#endif

#ifndef KEXIREPORTUTILS_EXPORT
# ifdef MAKE_KEXIREPORTUTILS_LIB
#  define KEXIREPORTUTILS_EXPORT KDE_EXPORT
# else
#  define KEXIREPORTUTILS_EXPORT KDE_IMPORT
# endif
#endif

#ifndef KOMACRO_EXPORT
# ifdef MAKE_KOMACRO_LIB
#  define KOMACRO_EXPORT KDE_EXPORT
# else
#  define KOMACRO_EXPORT KDE_IMPORT
# endif
#endif

/* additional default options */
#ifndef KEXI_NO_CTXT_HELP
# define KEXI_NO_CTXT_HELP
#endif
# define KDE_CXXFLAGS

/* -- compile-time settings -- */
#if defined(Q_WS_WIN) || defined(KEXI_OPTIONS)
/* defined in a .pro file or 'KEXI_OPTIONS' env. variable */
#else

/* unfinished features visibility */
/* -- undefined because people asked why these doesn't work: --
# define KEXI_SHOW_UNIMPLEMENTED 
# define KEXI_STARTUP_SHOW_TEMPLATES 
# define KEXI_STARTUP_SHOW_RECENT
# define KEXI_REPORTS_SUPPORT
*/
//# define KEXI_NO_FEEDBACK_AGENT
#endif

#endif //KEXI_EXPORT_H
