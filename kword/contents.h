/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998	  */
/* Version: 0.0.1						  */
/* Author: Reginald Stadlbauer, Torben Weis			  */
/* E-Mail: reggie@kde.org, weis@kde.org				  */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs			  */
/* needs c++ library Qt (http://www.troll.no)			  */
/* written for KDE (http://www.kde.org)				  */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)	  */
/* needs OpenParts and Kom (weis@kde.org)			  */
/* License: GNU GPL						  */
/******************************************************************/
/* Module: Contents (header)					  */
/******************************************************************/

#ifndef contents_h

#include <qstringlist.h>

class KWordDocument;
class KWParag;

class KWContents
{
public:
    KWContents( KWordDocument *doc_ );

    void createContents();
    void addParagName( const QString &n ) {
	parags.append( n );
    }
    void setEnd( KWParag *e ) {
	end = e;
    }
    bool hasContents() {
	return end && !parags.isEmpty();
    }
    bool numParags() {
	return parags.count();
    }
    QStringList::Iterator begin() {
	return parags.begin();
    }
    QStringList::Iterator ending() {
	return parags.end();
    }
    
protected:
    KWordDocument *doc;
    KWParag *end;
    QStringList parags;

};

#endif
