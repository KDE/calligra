/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Clipart                                                */
/******************************************************************/

#include "kpclipart.h"

/******************************************************************/
/* class KPClipart                                                */
/******************************************************************/

/*======================= constructor ============================*/
KPClipart::KPClipart()
    : filename(), wmf()
{
    pic = new QPicture;
}

/*======================= constructor ============================*/
KPClipart::KPClipart( QString _filename )
    : filename( _filename ), wmf()
{
    pic = new QPicture;
    setClipartName( filename );
}

/*======================= destructor =============================*/
KPClipart::~KPClipart()
{
    delete pic;
}

/*==================== set clipart ===============================*/
void KPClipart::setClipartName( QString _filename )
{
    filename = _filename;
    wmf.load( filename );
    wmf.paint( pic );
}

/*======================= get pic ================================*/
QPicture* KPClipart::getPic()
{
    return pic;
}

