/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer 1997-1998                   */
/* based on Torben Weis' KWord                                    */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Format Collection                                      */
/******************************************************************/

#include "formatcollection.h"
#include "kword_doc.h"

/******************************************************************/
/* Class: KWFormatCollection                                      */
/******************************************************************/

/*================================================================*/
KWFormatCollection::KWFormatCollection(KWordDocument_impl *_doc)
  : formats(1999,true,true)
{
  formats.setAutoDelete(true);
  doc = _doc;
}

/*================================================================*/
KWFormatCollection::~KWFormatCollection()
{
  formats.clear();
}

/*================================================================*/
KWFormat *KWFormatCollection::getFormat(const KWFormat &_format)
{
  QString key = generateKey(_format);

  KWFormat *format = findFormat(key);
  if (format)
    {
      format->incRef();
      return format;
    }
  else
    return insertFormat(key,_format);
}

/*================================================================*/
void KWFormatCollection::removeFormat(KWFormat *_format)
{
  QString key = generateKey(*_format);

  if (formats.remove(key))
    debug("remove: %s",key.data());
}

/*================================================================*/
QString KWFormatCollection::generateKey(const KWFormat &_format)
{
  QString key;

  // Key: BIU-Fontname-Fontsize-red-gree-blue
  // e.g. B**-Times-12-256-40-32
  key.sprintf("%c%c%c-%s-%d-%d-%d-%d",
	      (_format.getWeight() == QFont::Bold ? 'B' : '*'),
	      (_format.getItalic() == 1 ? 'I' : '*'),
	      (_format.getUnderline() == 1 ? 'U' : '*'),
	      _format.getUserFont()->getFontName(),
	      _format.getPTFontSize(),_format.getColor().red(),
	      _format.getColor().green(),_format.getColor().blue());

  return key;
}

/*================================================================*/
KWFormat *KWFormatCollection::findFormat(QString _key)
{
  return formats.find(_key.data());
}

/*================================================================*/
KWFormat *KWFormatCollection::insertFormat(QString _key,const KWFormat &_format)
{
  KWFormat *format = new KWFormat(doc,_format);
  
  formats.insert(_key.data(),format);
  debug("insert: %s",_key.data());
  format->incRef();

  return format;
}


