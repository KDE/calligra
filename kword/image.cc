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
  QString key = doc->getImageCollection()->generateKey(this);
  
  if (ref <= 0 && doc) 
    doc->getImageCollection()->removeImage(this); 
  if (!doc && ref == 0) warning("RefCount of the image == 0, but I couldn't delete it,"
				" because I have not a pointer to the document!");
}

/*================================================================*/
void KWImage::incRef() 
{ 
  ++ref;
  QString key = doc->getImageCollection()->generateKey(this);
}

/*================================================================*/
void KWImage::save(ostream &out)
{
  out << indent << "<FILENAME value=\"" << filename << "\"/>" << endl;
}
