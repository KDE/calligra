#ifndef KSPREAD_GLOBAL_H
#define KSPREAD_GLOBAL_H

#include <kiconloader.h>

#include "kspread_factory.h"

#define KSBarIcon( x ) BarIcon( x, KSpreadFactory::global() )
#define colWidth 60.0
#define heightOfRow 20.0

enum PasteMode { Normal, Text, Format, NoBorder, Comment, NormalAndTranspose, TextAndTranspoase,
		 FormatAndTranspose, NoBorderAndTranspose };
enum Operation { OverWrite, Add, Mul, Sub, Div };
enum Conditional { None, Equal,Superior,Inferior,SuperiorEqual,InferiorEqual,Between,Different};
enum Action { Stop, Warning, Information};
enum Allow { Allow_All, Allow_Number, Allow_Text};
enum Series { Column,Row,Linear,Geometric };

#endif
