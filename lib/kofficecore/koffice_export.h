/*
    This file is part of kofficecore
    Copyright (c) 2005 Koffice Team

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



#ifndef _KOFFICE_EXPORT_H
#define _KOFFICE_EXPORT_H

#include <kdeversion.h>
 
#if KDE_IS_VERSION(3,3,90)
 
#include <kdemacros.h>
 
#define KOFFICECORE_EXPORT
#define KOFFICEUI_EXPORT
#define KOTEXT_EXPORT
#define KOFORMULA_EXPORT
#define KOSTORE_EXPORT
#define KOWMF_EXPORT
#define KOSCRIPT_EXPORT
#define KOPAINTER_EXPORT
#define KSPREAD_EXPORT
#define KFORMULA_EXPORT

/*
#define KOFFICECORE_EXPORT KDE_EXPORT
#define KOFFICEUI_EXPORT KDE_EXPORT
#define KOTEXT_EXPORT KDE_EXPORT
#define KOFORMULA_EXPORT KDE_EXPORT
#define KOSTORE_EXPORT KDE_EXPORT
#define KOWMF_EXPORT KDE_EXPORT
#define KOSCRIPT_EXPORT KDE_EXPORT 
#define KOPAINTER_EXPORT KDE_EXPORT 
#define KSPREAD_EXPORT KDE_EXPORT
#define KFORMULA_EXPORT KDE_EXPORT
*/
#else
#define KOFFICECORE_EXPORT
#define KOFFICEUI_EXPORT
#define KOTEXT_EXPORT
#define KOFORMULA_EXPORT
#define KOSTORE_EXPORT
#define KOWMF_EXPORT
#define KOSCRIPT_EXPORT
#define KOPAINTER_EXPORT
#define KSPREAD_EXPORT
#define KFORMULA_EXPORT
#endif
#endif
