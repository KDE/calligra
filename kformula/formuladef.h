/*
 formuladef.h
 Project KOffice/KFormula
 
 Author: Andrea Rizzi <rizzi@kde.org>
 License:GPL
*/

/*
 * Fonts enalarge/reduce constants
 */
#ifndef _FORMULA_DEF_H_
#define _FORMULA_DEF_H_

#define FN_P34        0   // red/en 3/4 of prev (or 4/3)
#define FN_ENLARGE    0   // enlarge is default
#define FN_REDUCE     1   // reduce isn't default
#define FN_ELEMENT    2   // red/en current element
#define FN_CHILDREN   4   // red/en element chidlren
#define FN_NEXT       8   // red/en next elements
#define FN_INDEXES   16   // red/en indexes
#define FN_ONE       32   // red/en +(-) 1 
#define FN_P12       64   // red/en  1/2 of previous (or *2)
#define FN_LOCAL FN_ELEMENT | FN_CHILDREN | FN_INDEXES
#define FN_ALL   FN_NEXT | FN_LOCAL  
#define FN_MIN        2   // min value of numericFont
#define FN_MAX      120   // max value of numericFont
   

/*
 * Elements Type
 */
#define EL_BASIC       0
#define EL_TEXT        1
#define EL_ROOT        2
#define EL_INTEGRAL    3
#define EL_FRACTION    4
#define EL_DECORATION  5
#define EL_SYMBOL      6
#define EL_MATRIX      7
#define EL_BRACKET     8
/*
 * Formula Commands
 */

#define FCOM_DELETEME  -1
#define FCOM_TEXTCLONE -2
#define FCOM_ROOTCLONE -3

/*
 * Index Position
 */

#define IN_TOPLEFT       0
#define IN_BOTTOMLEFT    1
#define IN_TOPRIGHT      2
#define IN_BOTTOMRIGHT   3

/*
 * keyboard action: special keys like delete,insert,backspace,arrows...
 */
 //May be I don't need them
#endif		     