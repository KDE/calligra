/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Used and modified Perview Implementation by                    */
/*                  Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de) */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Filedialog Preview                                     */
/******************************************************************/

#include <qpicture.h>
#include "preview.h"
//#include "qwmf.h"

/**
 * A preview handler for the KFilePreviewDialag that shows
 * a WMF object.
 */
bool wmfPreviewHandler( const KFileInfo* fInfo, const QString fileName, QString&, QPixmap& pixmap )
{
    bool res = false;
//   QString ext = fileName.right( 3 ).lower();

//   if ( fInfo->isFile() && ( ext == "wmf" ) )
//     {
//       QWinMetaFile wmf;

//       if ( wmf.load( fileName.data() ) )
//  {
//    QPicture pic;
//    wmf.paint( &pic );

//    pixmap = QPixmap( 200, 200 );
//    QPainter p;

//    p.begin( &pixmap );
//    p.setBackgroundColor( white );
//    pixmap.fill( white );

//    QRect oldWin = p.window();
//    QRect vPort = p.viewport();
//    p.setViewport( 0, 0, 200, 200 );
//    p.drawPicture( pic );
//    p.setWindow( oldWin );
//    p.setViewport( vPort );
//    p.end();
    
//    res = true;
//  }
//     }
    return res;
}

/**
 * A preview handler for the KFilePreviewDialag that shows
 * a Pixmap object.
 */
bool pixmapPreviewHandler( const KFileInfo* fInfo, const QString fileName, QString&, QPixmap& pixmap )
{
    if ( fInfo->isFile() )
    {
        pixmap.load( fileName.data() );
        return !pixmap.isNull();
    }
    return false;
}
