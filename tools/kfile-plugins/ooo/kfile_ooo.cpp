/* This file is part of the KDE project
 * Copyright (C) 2003 Pierre Souchay <pierre@souchay.net>
 *
 * This program is free softbmpre; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Softbmpre Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied bmprranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Softbmpre Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include <config.h>
#include "kfile_ooo.h"

#include <klocale.h>
#include <kgenericfactory.h>
#include <koStore.h>
#include <koStoreDevice.h>
#include <kzip.h>

#include <qdom.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qvalidator.h>
#include <kdebug.h>

typedef KGenericFactory<KOfficePlugin> KOfficeFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_ooo, KOfficeFactory( "kfile_ooo" ));

static const char * mimetypes[] =
{ "application/vnd.sun.xml.calc",   "application/vnd.sun.xml.calc.template",
  "application/vnd.sun.xml.draw",   "application/vnd.sun.xml.draw.template",
  "application/vnd.sun.xml.impress","application/vnd.sun.xml.impress.template",
  "application/vnd.sun.xml.writer", "application/vnd.sun.xml.writer.global",
  "application/vnd.sun.xml.writer.math",
  "application/vnd.sun.xml.writer.template",
  0};

static const char * Advanced[] =
 {"meta:print-date"     , I18N_NOOP("Print Date"),
  "meta:printed-by"     , I18N_NOOP("Printed By"),
  "meta:initial-creator", I18N_NOOP("Creator"),
  0};

static const char * Information[] =
 {"dc:title",       I18N_NOOP("Title")      , 
  "dc:subject",     I18N_NOOP("Subject")    , 
  "dc:creator",     I18N_NOOP("Author")     ,
  "dc:description", I18N_NOOP("Description"),
  "dc:language"   , I18N_NOOP("Language"),
  0};

static const char * Statistics[] =
 {"meta:table-count"    , I18N_NOOP("Tables"),
  "meta:image-count"    , I18N_NOOP("Images"),
  "meta:object-count"   , I18N_NOOP("Objects"),
  "meta:page-count"     , I18N_NOOP("Pages"),
  "meta:paragraph-count", I18N_NOOP("Paragraphs"),
  "meta:word-count"     , I18N_NOOP("Words"),
  "meta:cell-count"     , I18N_NOOP("Cells"),
  "meta:character-count", I18N_NOOP("Characters"),
  0};

static const char * metakeywords = "meta:keywords";
static const char * metakeyword  = "meta:keyword" ;
static const char * DocumentInfo = "DocumentInfo" ;
static const char * UserDefined  = "UserDefined"  ;

KOfficePlugin::KOfficePlugin(QObject *parent, const char *name,
                       const QStringList &args)
    
    : KFilePlugin(parent, name, args)
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
    }
      
    //setHint(item, KFileMimeTypeInfo::Author);
    item = addItemInfo(group, metakeyword, i18n("Keywords"),
		       QVariant::String);
    setHint(item, KFileMimeTypeInfo::Description);
    setAttributes(item, KFileMimeTypeInfo::Modifiable);

    group = addGroupInfo(info, "Advanced", i18n("Document Advanced"));
    for (i = 0; Advanced[i]; i+=2){
      item = addItemInfo(group, Advanced[i], i18n(Advanced[i+1]),
		         QVariant::String);
      setHint(item, KFileMimeTypeInfo::Description);
    }
    
    group = addGroupInfo(info, "Statistics", i18n("Document Statistics"));
    for (i = 0; Statistics[i]; i+=2){
      item = addItemInfo(group, Statistics[i], i18n(Statistics[i+1]),
		         QVariant::Int);
      setHint(item, KFileMimeTypeInfo::Length);
    }
}

bool KOfficePlugin::readInfo( KFileMetaInfo& info, uint /*what*/)
{
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
    appendItem(group, metakeyword,
	       stringFromNode(base.namedItem(metakeywords), metakeyword));
    
    KFileMetaInfoGroup group1 = appendGroup(info, "Advanced");
    for (int i = 0; Advanced[i]; i+=2){
	    QString txt = stringFromNode(base, Advanced[i]);
	    if (txt != "")
		    appendItem(group1, Advanced[i], txt);
    }

    QDomNode dstat = base.namedItem("meta:document-statistic");

    KFileMetaInfoGroup group2 = appendGroup(info, "Statistics");
    if (!dstat.isNull() && dstat.isElement()){
	    QDomElement dinfo = dstat.toElement();
    	    for (int i = 0; Statistics[i]; i+=2)
		    addAttributeInfo(dinfo, group2, Statistics[i] );
    }
    

    QDomNodeList userList = base.elementsByTagName( "meta:user-defined" );

    KFileMetaInfoGroup groupuser = appendGroup(info, UserDefined);

    for (uint i = 0; i < userList.length(); i++){
      QDomNode node = userList.item(i);
      if (node.isElement()){
	appendItem(groupuser,
		   node.toElement().attribute("meta:name",
					      QString("User %1").arg(i)),
		   node.toElement().text());
      }
    }
    return true;
}

QString KOfficePlugin::stringFromNode(QDomNode node, QString name)
{
    QString value = node.namedItem(name).toElement().text();
    return value.isEmpty() ? "" : value;
}

void KOfficePlugin::addAttributeInfo(const QDomElement & elem, KFileMetaInfoGroup & group, const QString attributeName)
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
				  const QString  nodeName,
				  const QString  value) const
{
  if (parentNode.toElement().isNull()){
    kdDebug(7034) << "Parent node is Null or not an Element, cannot write node "
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
    kdDebug(7034) << "Wrong type of node " << nodeName << ", should be Element"
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
  no_errors = no_errors &&
    writeTextNode(doc, metaKeyNode, metakeyword,
		  info[DocumentInfo][metakeyword].value().toString());

  // Now, we store the user-defined data
  QDomNodeList theElements = base.elementsByTagName("meta:user-defined");
  for (uint i = 0; i < theElements.length(); i++)
    {
      QDomElement el = theElements.item(i).toElement();
      if (el.isNull()){
	kdDebug(7034) << "meta:user-defined is not an Element" << endl;
	no_errors = false;
      }

      QString s = info[UserDefined][el.attribute("meta:name")].value().toString();
      if (s != el.text()){
	QDomText txt = doc.createTextNode(s);
	if (!el.firstChild().isNull())
	  el.replaceChild(txt, el.firstChild());
	else
	  el.appendChild(txt);
      }
    }

  if (!no_errors){
    kdDebug(7034) << "Errors were found while building meta.xml for file "
		  << info.path() << endl;
    // It is safer to avoid to manipulate meta.xml if errors, we abort.
    return false;
  }
  writeMetaData(info.path(), doc);
  return true;
}

bool KOfficePlugin::writeMetaData(const QString & path,
				  const QDomDocument &doc) const
{
    KZip m_zip(path);
    // Fixme : open WriteOnly to avoid garbages in Zip file ?
    if (!m_zip.open(IO_ReadWrite))
	    return false;
    QCString text = doc.toCString();
    m_zip.setCompression(KZip::DeflateCompression);
    m_zip.writeFile("meta.xml", QString::null, QString::null,text.length(),
		    text);
    return true;
}


QIODevice* KOfficePlugin::getData(KArchive &m_zip, int fileMode) const
{

    if ( !m_zip.open(fileMode) || !m_zip.directory())
        return 0;
    
    const KArchiveEntry* entry = m_zip.directory()->entry( "meta.xml" );
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
    QIODevice * io = getData(m_zip, IO_ReadOnly);
    if (!io || !io->isReadable())
	    return doc;
    QString errorMsg;
    int errorLine, errorColumn;
    if ( !doc.setContent( io, &errorMsg, &errorLine, &errorColumn ) ){
      kdDebug(7034) << "Error " << errorMsg.latin1()
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
	if (key == "dc:language")
		return new QRegExpValidator(QRegExp("[a-zA-Z-]{1,5}"),
					    parent, name);
	return 0;
}

#include "kfile_ooo.moc"
