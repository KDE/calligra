/*
   Copyright (c) 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef SHAREDEXPORT_DEBUG_H
#define SHAREDEXPORT_DEBUG_H

// needed because the export plugins for odt2epub, odt2html, odt2mobi
// share code by duplicated listing of source files in the sources
// of the build targets. But at the same time these source files use debug
// statements that want specific debug id for the specific plugins
// TODO: better turn the shared code into a shared lib with own debug id

#ifdef DEBUG_EPUB
#include <EpubExportDebug.h>
#define SHAREDEXPORT_LOG EPUBEXPORT_LOG
#endif

#ifdef DEBUG_HTML
#include <HtmlExportDebug.h>
#define SHAREDEXPORT_LOG HTMLEXPORT_LOG
#endif

#ifdef DEBUG_MOBI
#include <MobiExportDebug.h>
#define SHAREDEXPORT_LOG MOBIEXPORT_LOG
#endif

#define debugSharedExport qCDebug(SHAREDEXPORT_LOG)
#define warnSharedExport qCWarning(SHAREDEXPORT_LOG)
#define errorSharedExport qCCritical(SHAREDEXPORT_LOG)

#endif
