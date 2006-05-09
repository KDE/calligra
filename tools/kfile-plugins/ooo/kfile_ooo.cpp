/* This file is part of the KDE project
 * Copyright (C) 2003 Pierre Souchay <pierre@souchay.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

/**
 * CHANGES
 * v1.7 to 1.9
 * Added the editing duration analysis.
 * v1.6 to 1.7
 * Changed the incorrect meta:date to dc:date
 * Now parse date instead of simply print the ISO date
 * Fixes bug with keywords, now properly handled
 * Changed the deprecated upload method to the new one.
 * v1.5 to 1.6
 * Correction for bug 68112: http://bugs.kde.org/show_bug.cgi?id=68112
 * OOo 1.1 does not support garbages in zip files. We now recreate the zip
 * file from scratch. It is slower, but we don't have the choice :(
 * v1.1 to v1.2
 * Added support for  KFileMimeTypeInfo::Hints (Name, Author, Description)
 * Added some Advanced Attributes and Statistics according to DTD of OOo
 * No more duplicated strings of same contents to facilitate changes
 */

#include <config.h>
#include "kfile_ooo.h"

#include <klocale.h>
#include <kgenericfactory.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <kzip.h>
#include <ktempfile.h>
#include <q3ptrstack.h>

#include <qdom.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qvalidator.h>
//Added by qt3to4:
#include <Q3CString>
#include <kdebug.h>
#include <kio/netaccess.h>

typedef KGenericFactory<KOfficePlugin> KOfficeFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_ooo, KOfficeFactory( "kfile_ooo" ))

static const char * const mimetypes[] =
{ "application/vnd.sun.xml.calc",   "application/vnd.sun.xml.calc.template",
  "application/vnd.sun.xml.draw",   "application/vnd.sun.xml.draw.template",
  "application/vnd.sun.xml.impress","application/vnd.sun.xml.impress.template",
  "application/vnd.sun.xml.writer", "application/vnd.sun.xml.writer.global",
  "application/vnd.sun.xml.writer.math",
  "application/vnd.sun.xml.writer.template",
  "application/vnd.oasis.opendocument.chart",
  "application/vnd.oasis.opendocument.formula",
  "application/vnd.oasis.opendocument.graphics",
  "application/vnd.oasis.opendocument.graphics-template",
  "application/vnd.oasis.opendocument.presentation",
  "application/vnd.oasis.opendocument.presentation-template",
  "application/vnd.oasis.opendocument.spreadsheet",
  "application/vnd.oasis.opendocument.spreadsheet-template",
  "application/vnd.oasis.opendocument.text",
  "application/vnd.oasis.opendocument.text-template",
  0};

static const char * const Advanced[] =
 {
  "meta:printed-by"     , I18N_NOOP("Printed By"),
  "meta:print-date"     , I18N_NOOP("Print Date"),
  "dc:date"		, I18N_NOOP("Date"),
  "meta:creation-date"  , I18N_NOOP("Creation Date"),
  "meta:initial-creator", I18N_NOOP("Creator"),
  "meta:generator"	, I18N_NOOP("Generator"),
  "meta:editing-cycles" , I18N_NOOP("Editing Cycles"),
  "meta:editing-duration" , I18N_NOOP("Editing Duration"),
  0};

static const char * dclanguage = "dc:language";

static const char * const Information[] =
 {"dc:title"      , I18N_NOOP("Title")      ,
  "dc:creator"    , I18N_NOOP("Author")     ,
  "dc:description", I18N_NOOP("Description"),
  "dc:subject"    , I18N_NOOP("Subject")    ,
  dclanguage      , I18N_NOOP("Language")   ,
  0};

static const char * const Statistics[] =
 {"meta:draw-count"	 , I18N_NOOP("Draws"),
  "meta:table-count"     , I18N_NOOP("Tables"),
  "meta:image-count"     , I18N_NOOP("Images"),
  "meta:object-count"    , I18N_NOOP("Objects"),
  "meta:ole-object-count", I18N_NOOP("OLE Objects"),
  "meta:page-count"      , I18N_NOOP("Pages"),
  "meta:paragraph-count" , I18N_NOOP("Paragraphs"),
  "meta:word-count"      , I18N_NOOP("Words"),
  "meta:cell-count"      , I18N_NOOP("Cells"),
  "meta:character-count" , I18N_NOOP("Characters"),
  "meta:row-count"       , I18N_NOOP("Rows"),
  0};

static const char * metakeywords  = "meta:keywords";
static const char * metakeyword   = "meta:keyword" ;
static const char * DocumentInfo  = "DocumentInfo" ;
static const char * UserDefined   = "UserDefined"  ;
static const char * DocAdvanced   = "Advanced"     ;
static const char * DocStatistics = "Statistics"   ;
static const char * metadocstat   = "meta:document-statistic";
static const char * metaname      = "meta:name"    ;
static const char * metauserdef   = "meta:user-defined";
static const char * metafile      = "meta.xml"     ;
KOfficePlugin::KOfficePlugin(QObject *parent,
                       const QStringList &args)

    : KFilePlugin(parent, args)
{
   int i = 0;
   while (mimetypes[i])
    makeMimeTypeInfo( mimetypes[i++] );
}

void KOfficePlugin::makeMimeTypeInfo(const QString& mimeType)
{
    KFileMimeTypeInfo* info = addMimeTypeInfo( mimeType );

    userdefined = addGroupInfo(info, UserDefined, i18n("User Defined"));
    addVariableInfo(userdefined, QVariant::String,
		    KFileMimeTypeInfo::Addable   |
		    KFileMimeTypeInfo::Removable |
		    KFileMimeTypeInfo::Modifiable);

    KFileMimeTypeInfo::GroupInfo* group = 0L;
    group = addGroupInfo(info, DocumentInfo, i18n("Document Information"));
    KFileMimeTypeInfo::ItemInfo* item;
    int i = 0;
    for (i = 0; Information[i]; i+=2){
      item = addItemInfo(group, Information[i], i18n(Information[i+1]),
		      QVariant::String);
      setAttributes(item, KFileMimeTypeInfo::Modifiable);
      switch (i){
	      case 0:
		setHint(item, KFileMimeTypeInfo::Name);
		break;
	      case 1:
		setHint(item, KFileMimeTypeInfo::Author);
		break;
	      case 2:
		setHint(item, KFileMimeTypeInfo::Description);
	      default:;
     }
    }

    item = addItemInfo(group, metakeyword, i18n("Keywords"),
		       QVariant::String);
    setHint(item, KFileMimeTypeInfo::Description);
    setAttributes(item, KFileMimeTypeInfo::Modifiable);

    group = addGroupInfo(info, DocAdvanced, i18n("Document Advanced"));
    for (i = 0; Advanced[i]; i+=2){
      // I should add the isDate property instead of testing the index, but it works well, who cares ? :-)
    QVariant::Type typ = QVariant::String;
    if (i > 1 && i < 8)
	    typ = QVariant::DateTime;
    if (i == 14)
	    typ = QVariant::String;
      item = addItemInfo(group, Advanced[i], i18n(Advanced[i+1]), typ);
      setHint(item, KFileMimeTypeInfo::Description);
    }

    group = addGroupInfo(info, DocStatistics, i18n("Document Statistics"));
    for (i = 0; Statistics[i]; i+=2){
      item = addItemInfo(group, Statistics[i], i18n(Statistics[i+1]),
		         QVariant::Int);
      setHint(item, KFileMimeTypeInfo::Length);
    }
}

/**
 * Gets a number in the string and update the position next character which
 * is not a number
 * @param str The string to check
 * @param the position to start to, updated to the next character NAN
 * @return the number parsed, 0 if number was not valid
 */
int getNumber(QString &str, int * pos){
	int k = *pos;
	for (int len = str.length() ;
	     str.at(k).isNumber() && k < len ;
	     k++);
	bool result = false;
	int num = str.mid( *pos, k-(*pos)).toInt(&result);
	*pos = k;
	if (!result)
		return 0;
	return num;
}

void KOfficePlugin::getEditingTime(KFileMetaInfoGroup group1,
		                   const char * labelid, QString & txt){
	QString t;
	int days = 0;
	int hours = 0;
	int minutes = 0;
	int seconds = 0;
	if (txt.at(0) != 'P'){
		kDebug(7034) << labelid << "=" << txt <<
			" does not seems to be a valid duration" << endl;
		return;
	}
	int pos = 1;
	if (txt.at(pos).isNumber()){
		days = getNumber(txt, &pos);
		if (txt.at(pos++)!='D'){
			days=0;
			kDebug(7034) << labelid <<
				" First arg was not a day in " << txt << endl;
		}
	}
	if (txt.at(pos)!= 'T'){
		kDebug(7034) << labelid << "=" << txt <<
			" does not seems to contain time information" << endl;
		return;
	}
	pos++;
	int len = txt.length();
	while (pos < len){
		int res = getNumber(txt, &pos);
		if (pos >= len)
			return;
		switch (txt.at(pos).toLatin1()){
			case 'H':
				hours = res;
				break;
			case 'M':
				minutes = res;
				break;
			case 'S':
				seconds = res;
				break;
			default:
				kDebug(7034) << "Unknown unit at pos " << pos << " while parsing " <<
					labelid << "="<< txt << endl;
		}
		pos++;
	}
	hours += days * 24;
	appendItem(group1, labelid,
		   i18n("%1:%2.%3").arg(hours).arg(minutes, 2).arg(seconds,2 ));
}

void KOfficePlugin::getDateTime(KFileMetaInfoGroup group1,
		                const char * labelid, QString & txt)
{
	QDateTime dt = QDateTime::fromString( txt, Qt::ISODate);
	appendItem( group1, labelid, dt);
}

bool KOfficePlugin::readInfo( KFileMetaInfo& info, uint /*what*/)
{
    if ( info.path().isEmpty() ) // remote file
        return false;

    KFileMetaInfoGroup group = appendGroup(info, DocumentInfo);
    QDomDocument doc = getMetaDocument(info.path());
    if (doc.isNull())
	    return false;
    QDomElement base = getBaseNode(doc).toElement();
    if (base.isNull())
	    return false;
    for (int i = 0; Information[i]; i+=2)
	    appendItem(group, Information[i],
		       stringFromNode(base, Information[i]));
    // Special case for keyword
    QDomNodeList keywordList = base.elementsByTagName( metakeyword );
    QString allKeywords;
    for (uint i = 0; i < keywordList.length(); i++){
      QDomNode node = keywordList.item(i);
      if (node.isElement()){
	if (i>0)
		allKeywords += ", ";
	allKeywords += node.toElement().text();
      }
    }
    appendItem(group, metakeyword,
	       allKeywords);

    KFileMetaInfoGroup group1 = appendGroup(info, DocAdvanced);
    for (int i = 0; Advanced[i]; i+=2){
	    QString txt = stringFromNode(base, Advanced[i]);
	    if (!txt.isEmpty()){
		    // A silly way to do it...
		    switch (i){
			    case 2:
			    case 4:
			    case 6:
			    	getDateTime(group1, Advanced[i], txt);
				break;
			    case 14:
				getEditingTime(group1, Advanced[i], txt);
				break;
			    default:
				appendItem(group1, Advanced[i], txt);}
		   }
	    }

    QDomNode dstat = base.namedItem(metadocstat);

    KFileMetaInfoGroup group2 = appendGroup(info, DocStatistics);
    if (!dstat.isNull() && dstat.isElement()){
	    QDomElement dinfo = dstat.toElement();
    	    for (int i = 0; Statistics[i]; i+=2)
		    addAttributeInfo(dinfo, group2, Statistics[i] );
    }


    QDomNodeList userList = base.elementsByTagName( metauserdef );

    KFileMetaInfoGroup groupuser = appendGroup(info, UserDefined);

    for (uint i = 0; i < userList.length(); i++){
      QDomNode node = userList.item(i);
      if (node.isElement()){
	appendItem(groupuser,
		   node.toElement().attribute(metaname,
					      QString("User %1").arg(i)),
		   node.toElement().text());
      }
    }
    return true;
}

QString KOfficePlugin::stringFromNode(const QDomNode &node, const QString &name)
{
    QString value = node.namedItem(name).toElement().text();
    return value.isEmpty() ? QString::null : value;
}

void KOfficePlugin::addAttributeInfo(const QDomElement & elem, KFileMetaInfoGroup & group, const QString &attributeName)
{
  if (!elem.hasAttribute(attributeName)){
  	return;
  }
  QString m_attribute = elem.attribute(attributeName, "0");
  if (m_attribute == "0")
	  return;
  appendItem(group, attributeName, m_attribute);
}

bool KOfficePlugin::writeTextNode(QDomDocument & doc,
				  QDomNode & parentNode,
				  const QString  &nodeName,
				  const QString  &value) const
{
  if (parentNode.toElement().isNull()){
    kDebug(7034) << "Parent node is Null or not an Element, cannot write node "
		  << nodeName << endl;
    return false;
  }

  // If the node does not exist, we create it...
  if (parentNode.namedItem(nodeName).isNull())
    QDomNode ex = parentNode.appendChild(doc.createElement(nodeName));

  // Now, we are sure we have a node
  QDomElement nodeA = parentNode.namedItem(nodeName).toElement();

  // Ooops... existing node were not of the good type...
  if (nodeA.isNull()){
    kDebug(7034) << "Wrong type of node " << nodeName << ", should be Element"
		  << endl;
    return false;
  }

  QDomText txtNode = doc.createTextNode(value);

  // If the node has already Text Child, we replace it.
  if (nodeA.firstChild().isNull())
    nodeA.appendChild(txtNode);
  else
    nodeA.replaceChild( txtNode, nodeA.firstChild());
  return true;
}

bool KOfficePlugin::writeInfo( const KFileMetaInfo& info) const
{
  bool no_errors = true;
  QDomDocument doc = getMetaDocument(info.path());
  QDomElement base = getBaseNode(doc).toElement();
  if (base.isNull())
    return false;
  for (int i = 0; Information[i]; i+=2)
    no_errors = no_errors &&
      writeTextNode(doc, base, Information[i],
		    info[DocumentInfo][Information[i]].value().toString());
  // If we need a meta:keywords container, we create it.
  if (base.namedItem(metakeywords).isNull())
    base.appendChild(doc.createElement(metakeywords));
  QDomNode metaKeyNode = base.namedItem(metakeywords);

  QDomNodeList childs = doc.elementsByTagName(metakeyword);
  for (int i = childs.length(); i >= 0; --i){
	  metaKeyNode.removeChild( childs.item(i) );
  }
  QStringList keywordList = QStringList::split(",", info[DocumentInfo][metakeyword].value().toString().trimmed(), false);
  for ( QStringList::Iterator it = keywordList.begin(); it != keywordList.end(); ++it ) {
	QDomElement elem = doc.createElement(metakeyword);
	metaKeyNode.appendChild(elem);
	elem.appendChild(doc.createTextNode((*it).trimmed()));
    }

  // Now, we store the user-defined data
  QDomNodeList theElements = base.elementsByTagName(metauserdef);
  for (uint i = 0; i < theElements.length(); i++)
    {
      QDomElement el = theElements.item(i).toElement();
      if (el.isNull()){
	kDebug(7034) << metauserdef << " is not an Element" << endl;
	no_errors = false;
      }

      QString s = info[UserDefined][el.attribute(metaname)].value().toString();
      if (s != el.text()){
	QDomText txt = doc.createTextNode(s);
	if (!el.firstChild().isNull())
	  el.replaceChild(txt, el.firstChild());
	else
	  el.appendChild(txt);
      }
    }

  if (!no_errors){
    kDebug(7034) << "Errors were found while building " << metafile
	     	  << " for file " << info.path() << endl;
    // It is safer to avoid to manipulate meta.xml if errors, we abort.
    return false;
  }
  writeMetaData(info.path(), doc);
  return true;
}

/**
 * This function recreate a zip in dest with all files from src
 * except meta.xml
 */
bool copyZipToZip( const KZip * src, KZip * dest)
{
  Q3PtrStack<KArchiveDirectory> src_dirStack ;
  QStringList curDirName;

  src_dirStack.push ( src->directory()  );

  do {
    KArchiveDirectory * src_dir = src_dirStack.pop();
    curDirName.append(src_dir->name());
    const QStringList dirEntries = src_dir->entries();

    /* We now iterate over all entries and create entries in dest */
    QStringList::const_iterator it;
    for ( it = dirEntries.begin(); it != dirEntries.end(); ++it ) {
      if ( *it == metafile )
	continue;

      const KArchiveEntry* curEntry = src_dir->entry( *it );

      if (curEntry->isFile()) {
        const KArchiveFile * input_file = dynamic_cast<const KArchiveFile*>( curEntry );
	QByteArray b = input_file->data();
        QString filename;
	if (curDirName.isEmpty() || src_dir->name()=="/"){
		filename = *it;
	} else {
		filename = curDirName.join("/") + "/" + *it;
	}
	dest->writeFile(filename , QString::null, QString::null,b.data(), b.count() );
      } else
        if (curEntry->isDirectory()) {
          src_dirStack.push( dynamic_cast<const KArchiveDirectory*>( curEntry ) );
        }
        else {
		kDebug(7034) << *it << " is a unknown type. Aborting." << endl;
		return false;
	}
      }
    curDirName.pop_back();
  } while ( ! src_dirStack.isEmpty() );
  return true;
}

bool KOfficePlugin::writeMetaData(const QString & path,
				  const QDomDocument &doc) const
{
    KTempFile tmp_file;
    tmp_file.setAutoDelete(true);
    KZip * m_zip = new KZip(tmp_file.name());
    KZip * current= new KZip(path);
    /* To correct problem with OOo 1.1, we have to recreate the file from scratch */
    if (!m_zip->open(QIODevice::WriteOnly) || !current->open(QIODevice::ReadOnly) )
	    return false;
    QByteArray text = doc.toByteArray();
    m_zip->setCompression(KZip::DeflateCompression);
    if (!copyZipToZip(current, m_zip))
	    return false;
    m_zip->writeFile(QString(metafile), QString::null, QString::null,text,text.length());
    delete current;
    delete m_zip;
    // NULL as third parameter is not good, but I don't know the Window ID
    // That is only to avoid the deprecated warning at compile time
    if (!KIO::NetAccess::upload( tmp_file.name(), KUrl(path), NULL)){
	    kDebug(7034) << "Error while saving " << tmp_file.name() << " as " << path << endl;
	    return false;
    }
    return true;
}


QIODevice* KOfficePlugin::getData(KArchive &m_zip) const
{

    if ( !m_zip.open(QIODevice::ReadOnly) || !m_zip.directory())
        return 0;

    const KArchiveEntry* entry = m_zip.directory()->entry( metafile );
    if (!entry || entry->isDirectory())
        return 0;

    const KZipFileEntry* f = static_cast<const KZipFileEntry *>(entry);
    if (!f)
	    return 0;
    return f->device();
}

QDomDocument KOfficePlugin::getMetaDocument(const QString &path) const
{
    QDomDocument doc;
    KZip m_zip(path);
    QIODevice * io = getData(m_zip);
    if (!io || !io->isReadable())
	    return doc;
    QString errorMsg;
    int errorLine, errorColumn;
    if ( !doc.setContent( io, &errorMsg, &errorLine, &errorColumn ) ){
      kDebug(7034) << "Error " << errorMsg
		    << "while getting XML content at line "
		    << errorLine << ", column "<< errorColumn << endl;
    	delete io;
	return doc;
    }
    delete io;
    return doc;
}

QDomNode KOfficePlugin::getBaseNode(const QDomDocument &doc) const
{
    return
	doc.namedItem("office:document-meta").namedItem("office:meta");
}

QValidator * KOfficePlugin::createValidator(const QString &,      /* mimetype */
					    const QString & ,     /* group */
					    const QString &key,
					    QObject * parent,
					    const char * name ) const
{
	if (key == dclanguage)
		return new QRegExpValidator(QRegExp("[a-zA-Z-]{1,5}"),
					    parent);
	return 0;
}

#include "kfile_ooo.moc"
