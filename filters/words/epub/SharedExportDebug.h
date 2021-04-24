/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
