/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* KPresenter is under GNU GPL                                    */
/******************************************************************/
/* Module: pixmap collection (header)                             */
/******************************************************************/

#ifndef kppixmapcollection_h
#define kppixmapcollection_h

#include <qlist.h>

#include "kppixmap.h"

class QPixmap;

/******************************************************************/
/* Class: KPPixmapCollection                                      */
/******************************************************************/

class KPPixmapCollection
{
public:
	KPPixmapCollection()
    { pixmapList.setAutoDelete( true ); }

	virtual ~KPPixmapCollection()
    { pixmapList.clear(); }

	virtual QPixmap* getPixmap( QString _filename, KSize _size, QString &_data, bool orig = false, bool addref = true );
	virtual QPixmap* getPixmap( QString _filename, QString _data, KSize _size, bool orig = false, bool addref = true );
	virtual QPixmap* getPixmap( QString _filename, QString _data, QPixmap *_pixmap, KSize _size, bool orig = false, bool addref = true );

	virtual void removeRef( QString _filename, KSize _size );
	virtual void removeRef( QString _filename, QString _data, KSize _size );

protected:
	virtual int inPixmapList( QString _filename, KSize _size );
	virtual int inPixmapList( QString _filename, QString _data, KSize _size );

	QList<KPPixmap> pixmapList;

};

#endif
