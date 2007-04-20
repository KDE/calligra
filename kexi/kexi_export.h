/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifdef MAKE_KEXICORE_LIB
# define KEXICORE_EXPORT KDE_EXPORT
#elif defined(KDE_MAKE_LIB)
# define KEXICORE_EXPORT KDE_IMPORT
#else
# define KEXICORE_EXPORT 
#endif

#ifdef MAKE_KEXIMAIN_LIB
# define KEXIMAIN_EXPORT KDE_EXPORT
#elif defined(KDE_MAKE_LIB)
# define KEXIMAIN_EXPORT KDE_IMPORT
#else
# define KEXIMAIN_EXPORT 
#endif

#ifdef MAKE_KEXITABLEFILTERS_LIB
# define KEXITABLEFILTERS_EXPORT  KDE_EXPORT
#elif defined(KDE_MAKE_LIB)
# define KEXITABLEFILTERS_EXPORT  KDE_IMPORT
#else
# define KEXITABLEFILTERS_EXPORT //for apps
#endif

#ifdef MAKE_KEXIDATATABLE_LIB
# define KEXIDATATABLE_EXPORT  KDE_EXPORT
#elif defined(KDE_MAKE_LIB)
# define KEXIDATATABLE_EXPORT  KDE_IMPORT
#else
# define KEXIDATATABLE_EXPORT //for apps
#endif

#ifndef KEXIEXTWIDGETS_EXPORT //tmp

#ifdef MAKE_KEXIEXTWIDGETS_LIB
# define KEXIEXTWIDGETS_EXPORT  KDE_EXPORT
#elif defined(KDE_MAKE_LIB)
# define KEXIEXTWIDGETS_EXPORT  KDE_IMPORT
#else
# define KEXIEXTWIDGETS_EXPORT //for apps
#endif

#endif

#ifdef MAKE_KFORMEDITOR_LIB
# define KFORMEDITOR_EXPORT  KDE_EXPORT
#elif defined(KDE_MAKE_LIB)
# define KFORMEDITOR_EXPORT  KDE_IMPORT
#else
# define KFORMEDITOR_EXPORT //for apps
#endif

#ifdef MAKE_KEXIPRJWIZARD_LIB
# define KEXIPRJWIZARD_EXPORT  KDE_EXPORT
#elif defined(KDE_MAKE_LIB)
# define KEXIPRJWIZARD_EXPORT  KDE_IMPORT
#else
# define KEXIPRJWIZARD_EXPORT //for apps
#endif

#ifdef MAKE_KEXIFILTER_LIB
# define KEXIFILTER_EXPORT  KDE_EXPORT
#elif defined(KDE_MAKE_LIB)
# define KEXIFILTER_EXPORT  KDE_IMPORT
#else
# define KEXIFILTER_EXPORT //for apps
#endif

#ifdef MAKE_KEXIWIDGETS_LIB
# define KEXIWIDGETS_EXPORT  KDE_EXPORT
#elif defined(KDE_MAKE_LIB)
# define KEXIWIDGETS_EXPORT  KDE_IMPORT
#else
# define KEXIWIDGETS_EXPORT //for apps
#endif

#ifdef MAKE_KEXIUUID_LIB
# define KEXIUUID_EXPORT  KDE_EXPORT
#else
# define KEXIUUID_EXPORT //for apps
#endif

#ifdef MAKE_KEXIRELATIONSVIEW_LIB
# define KEXIRELATIONSVIEW_EXPORT  KDE_EXPORT
#elif defined(KDE_MAKE_LIB)
# define KEXIRELATIONSVIEW_EXPORT  KDE_IMPORT
#else
# define KEXIRELATIONSVIEW_EXPORT //for apps
#endif

#ifdef MAKE_KEXIGUIUTILS_LIB
# define KEXIGUIUTILS_EXPORT KDE_EXPORT
#elif defined(KDE_MAKE_LIB)
# define KEXIGUIUTILS_EXPORT KDE_IMPORT
#else
# define KEXIGUIUTILS_EXPORT //for apps
#endif

#ifdef MAKE_KROSS_MAIN_LIB
# define KROSS_MAIN_EXPORT KDE_EXPORT
#elif defined(KDE_MAKE_LIB)
# define KROSS_MAIN_EXPORT KDE_IMPORT
#else
# define KROSS_MAIN_EXPORT //for apps
#endif

#ifdef MAKE_KEXIFORMUTILS_LIB
# define KEXIFORMUTILS_EXPORT KDE_EXPORT
#elif defined(KDE_MAKE_LIB)
# define KEXIFORMUTILS_EXPORT KDE_IMPORT
#else
# define KEXIFORMUTILS_EXPORT //for apps
#endif

#ifdef MAKE_KEXIREPORTUTILS_LIB
# define KEXIREPORTUTILS_EXPORT KDE_EXPORT
#elif defined(KDE_MAKE_LIB)
# define KEXIREPORTUTILS_EXPORT KDE_IMPORT
#else
# define KEXIREPORTUTILS_EXPORT //for apps
#endif

/* temporary */
/*#ifndef KOPROPERTY_EXPORT
# ifdef MAKE_KOPROPERTY_LIB
#  define KOPROPERTY_EXPORT KDE_EXPORT
# elif defined(KDE_MAKE_LIB)
#  define KOPROPERTY_EXPORT KDE_IMPORT
# else
#  define KOPROPERTY_EXPORT
# endif
#endif*/

/* temporary */
#ifndef KOMACRO_EXPORT
# ifdef MAKE_KOMACRO_LIB
#  define KOMACRO_EXPORT KDE_EXPORT
# elif defined(KDE_MAKE_LIB)
#  define KOMACRO_EXPORT KDE_IMPORT
# else
#  define KOMACRO_EXPORT
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
# define KEXI_NO_UNFINISHED
# define KEXI_FORMS_SUPPORT
//# define KEXI_NO_FEEDBACK_AGENT
#endif

#endif //KEXI_EXPORT_H
