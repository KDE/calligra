#ifndef __KOPRINTER_H__
#define __KOPRINTER_H__

#include <config.h>

#ifdef HAVE_KDEPRINT
#include <kprinter.h>
#else
#include <qprinter.h>
#define KPrinter QPrinter
#endif

#endif
