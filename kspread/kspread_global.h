#ifndef KSPREAD_GLOBAL_H
#define KSPREAD_GLOBAL_H

#include <kiconloader.h>

#include "kspread_factory.h"

#define KSBarIcon( x ) BarIcon( x, KSpreadFactory::global() )

enum PasteMode { Normal, Text, Format, NoBorder, NormalAndTranspose, TextAndTranspoase,
		 FormatAndTranspose, NoBorderAndTranspose };
enum Operation { OverWrite, Add, Mul, Sub, Div };
enum Conditional { None, Equal,Superior,Inferior,SuperiorEqual,InferiorEqual,Between,Different};
enum Series {Column,Row,Linear,Geometric};
struct Reference
    {
    QString table_name;
    QString ref_name;
    QRect rect;
    };
#endif
