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

#include "clipbrd_dnd.h"
#include "clipbrd_dnd.moc"
#include "parag.h"
#include "defs.h"

/******************************************************************/
/* Class: KWordDrag                                               */
/******************************************************************/

/*================================================================*/
KWordDrag::KWordDrag( QWidget *dragSource = 0L, const char *name = 0L )
	: QDragObject( dragSource, name ), kword(), plain(), html()
{
}

/*================================================================*/
void KWordDrag::setPlain( const QString &_plain )
{
	plain = _plain; 
}

/*================================================================*/
void KWordDrag::setKWord( const QString &_kword )
{
	kword = _kword; 
}

/*================================================================*/
void KWordDrag::setHTML( const QString &_html )
{
	html = _html; 
}

/*================================================================*/
const char *KWordDrag::format( int i ) const
{
	for ( int j = 0; MimeTypes[ j ] != QString::null; j++ )
    {
		if ( i == j )
			return MimeTypes[ j ].ascii(); 
    }

	return 0L; 
}

/*================================================================*/
QByteArray KWordDrag::encodedData( const char *mime ) const
{
	QCString str; 

	if ( QString( mime ) == MimeTypes[ 0 ] )
		str = plain.ascii(); 
	else if ( QString( mime ) == MimeTypes[ 1 ] )
		str = html.ascii(); 
	else if ( QString( mime ) == MimeTypes[ 2 ] )
		str = kword.ascii(); 

	return str; 
}

/*================================================================*/
bool KWordDrag::canDecode( QMimeSource* e )
{
	for ( unsigned int i = 0; MimeTypes[ i ] != QString::null; i++ )
    {
		if ( e->provides( MimeTypes[ i ] ) )
			return true; 
    }
	return false; 
}

/*================================================================*/
bool KWordDrag::decode( QMimeSource* e, QString& s )
{
	for ( unsigned int i = 0; MimeTypes[ i ] != QString::null; i++ )
    {
		QByteArray ba = e->encodedData( MimeTypes[ i ] ); 
		if ( ba.size() )
		{
			s = QString( ba ); 
			return true; 
		}
    }

	return false; 
}

