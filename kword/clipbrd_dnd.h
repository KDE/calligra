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
/* Module: Clipboard and DND                                      */
/******************************************************************/

#ifndef clipbrd_dnd_h
#define clipbrd_dnd_h

#include <qdragobject.h>
#include <qstring.h>
#include <qlist.h>

#include "defs.h"

class KWParag;

/******************************************************************/
/* Class: KWordDrag                                               */
/******************************************************************/

static const QString MimeTypes[] = {"text/plain", "text/html", MIME_TYPE, QString::null};

class KWordDrag : public QDragObject
{
    Q_OBJECT

public:
    KWordDrag( QWidget *dragSource = 0L, const char *name = 0L );

    void setPlain( const QString &_plain );
    void setKWord( const QString &_kword );
    void setHTML( const QString &_html );

    const char *format( int i ) const;
    QByteArray encodedData( const char *mime ) const;

    static bool canDecode( QMimeSource* e );
    static bool decode( QMimeSource* e, QString& s );

protected:
    QString kword, plain, html;

};


#endif
