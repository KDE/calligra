/*
 formuladef.h
 Project KOffice/KFormula
 
 Author: Andrea Rizzi <rizzi@kde.org>
 License:GPL
*/

#ifndef _FORMULA_DEF_H_
#define _FORMULA_DEF_H_

/*
 * Fonts enalarge/reduce constants
 */

/*
Defaults
*/
#define FN_ENLARGE    0   // enlarge is default
#define FN_PROP	      0   // red/en of a fraction (proportionally) see below.
/*
Higher Bits:
*/
#define FN_REDUCE     256    // reduce isn't default
#define FN_ELEMENT    512    // red/en current element
#define FN_CHILDREN   1024   // red/en element chidlren
#define FN_NEXT       2048   // red/en next elements
#define FN_INDEXES    4096   // red/en indexes
#define FN_BYNUM      8192   // red/en of a fixed value stored in bits 0/7
/*
Lower Bits:
 if FN_BYNUM
    They contains an 8 bit value, newNumericFont=oldNumericFont+(-)ThisValue
 if FN_PROP
     Bits 0/3 = Num
     Bits 4/7 = Den
     
     newNumericFont=oldNumericFont*Num/Den
*/

#define FN_P2         2       // Enlarge *2
#define FN_P43       52       //Enlarge  *4/3

#define FN_LOCAL FN_ELEMENT | FN_CHILDREN | FN_INDEXES
#define FN_ALL   FN_NEXT | FN_LOCAL  
#define FN_MIN        2   // min value of numericFont
#define FN_MAX      200   // max value of numericFont
   

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
