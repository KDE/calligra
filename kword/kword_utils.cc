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
/* Module: Utils                                                  */
/******************************************************************/

#include "kword_utils.h"

/*================================================================*/
QString makeRomanNumber( int n )
{
    return QString( RNThousands[ ( n / 1000 ) ] +
                    RNHundreds[ ( n / 100 ) % 10 ] +
                    RNTens[ ( n / 10 ) % 10 ] +
                    RNUnits[ ( n ) % 10 ] );
}
