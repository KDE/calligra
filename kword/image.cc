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
/* Module: Image                                                  */
/******************************************************************/

#include "image.h"
#include "kword_doc.h"
#include "defs.h"
#include "kword_utils.h"

#include <koIMR.h>
#include <komlMime.h>
#include <strstream>
#include <fstream>
#include <unistd.h>

/******************************************************************/
/* Class: KWImage                                                 */
/******************************************************************/

/*================================================================*/
void KWImage::decRef()
{
    --ref;
    QString key = doc->getImageCollection()->generateKey( this );

    if ( ref <= 0 && doc )
        doc->getImageCollection()->removeImage( this );
    if ( !doc && ref == 0 ) warning( "RefCount of the image == 0, but I couldn't delete it, "
                                     " because I have not a pointer to the document!" );
}

/*================================================================*/
void KWImage::incRef()
{
    ++ref;
    QString key = doc->getImageCollection()->generateKey( this );
}

/*================================================================*/
void KWImage::save( ostream &out )
{
    out << indent << "<FILENAME value=\"" << correctQString( filename ).latin1() << "\"/>" << endl;
}

/*================================================================*/
void KWImage::load( KOMLParser& parser, vector<KOMLAttrib>& lst, KWordDocument *_doc )
{
    doc = _doc;
    ref = 0;

    string tag;
    string name;

    while ( parser.open( 0L, tag ) )
    {
        KOMLParser::parseTag( tag.c_str(), name, lst );

        // filename
        if ( name == "FILENAME" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                {
                    filename = correctQString( ( *it ).m_strValue.c_str() );
                    QImage::load( filename );
                }
            }
        }

        else
            cerr << "Unknown tag '" << tag << "' in IMAGE" << endl;

        if ( !parser.close( tag ) )
        {
            cerr << "ERR: Closing Child" << endl;
            return;
        }
    }
}
