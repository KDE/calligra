#ifndef __KOPRINTER_H__
#define __KOPRINTER_H__

#include <config.h>

#ifndef HAVE_KDEPRINT_TESTED
#error "The presence of kdeprint wasn't tested by configure. If you are compiling KOffice itself, rerun configure. If you are compiling another KOffice application, add a test for it (see koffice/lib/configure.in.in)."
#endif

#ifdef HAVE_KDEPRINT
#include <kprinter.h>
#else
#include <qprinter.h>
#define KPrinter QPrinter
#endif

#endif
