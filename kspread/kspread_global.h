#ifndef KSPREAD_GLOBAL_H
#define KSPREAD_GLOBAL_H

#include <kiconloader.h>

#include "kspread_factory.h"

#define KSBarIcon( x ) BarIcon( x, KSpreadFactory::global() )

enum PasteMode { Normal, Text, Format, NoBorder, NormalAndTranspose, TextAndTranspoase,
		 FormatAndTranspose, NoBorderAndTranspose };
enum Operation { OverWrite, Add, Mul, Sub, Div };

#endif
