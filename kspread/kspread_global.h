#ifndef KSPREAD_GLOBAL_H
#define KSPREAD_GLOBAL_H

#include <kiconloader.h>

#include "kspread_factory.h"

#define KSBarIcon( x ) BarIcon( x, KSpreadFactory::global() )
#define colWidth 60.0
#define heightOfRow 20.0

/* Definition of maximal supported rows - please check kspread_util (columnName) and kspread_cluster also */
#define KS_rowMax 0x7FFF
/* Definition of maximal supported columns - please check kspread_util (columnName) and kspread_cluster also */
#define KS_colMax 0x7FFF

/* Definition of maximal supported columns/rows, which can be merged */
#define KS_spanMax 0xFFF

enum PasteMode { Normal, Text, Format, NoBorder, Comment, Result, NormalAndTranspose, TextAndTranspoase,
		 FormatAndTranspose, NoBorderAndTranspose };
enum Operation { OverWrite, Add, Mul, Sub, Div };
enum Conditional { None, Equal,Superior,Inferior,SuperiorEqual,InferiorEqual,Between,Different};
enum Action { Stop, Warning, Information};
enum Allow { Allow_All, Allow_Number, Allow_Text,Allow_Time,Allow_Date,Allow_Integer,Allow_TextLength};
enum Series { Column,Row,Linear,Geometric };
namespace KSpread {  // necessary due to QDock* enums (Werner)
enum MoveTo { Bottom,Left,Top,Right };
}
enum MethodOfCalc { SumOfNumber, Min, Max, Average, Count, NoneCalc};
#endif
