/******************************************************************/ 
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Utils (Header)                                         */
/******************************************************************/

#ifndef kword_utils_h
#define kword_utils_h

#include <qstring.h>

const QString RNUnits[] = {"", "i", "ii", "iii", "iv", "v", "vi", "vii", "viii", "ix"}; 
const QString RNTens[] = {"", "x", "xx", "xxx", "xl", "l", "lx", "lxx", "lxxx", "xc"}; 
const QString RNHundreds[] = {"", "c", "cc", "ccc", "cd", "d", "dc", "dcc", "dccc", "cm"}; 
const QString RNThousands[] = {"", "m", "mm", "mmm"}; 

QString makeRomanNumber( int n ); 

#endif
