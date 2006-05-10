/* This file is part of the KDE project
   Copyright 2001, 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <QMap>
#include <qbuffer.h>
#include <q3picture.h>
#include <qxml.h>
#include <qdom.h>
#include <qdatetime.h>
//Added by qt3to4:
#include <Q3CString>

#include <kdebug.h>
#include <kcodecs.h>
#include <kfilterdev.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>

#include <KoPageLayout.h>
#include <KoStore.h>
#include <KoFilterChain.h>

#include "ImportHelpers.h"
#include "ImportFormatting.h"
#include "ImportStyle.h"
#include "ImportField.h"

#include "abiwordimport.h"

class ABIWORDImportFactory : KGenericFactory<ABIWORDImport>
{
public:
    ABIWORDImportFactory(void) : KGenericFactory<ABIWORDImport> ("kwordabiwordimport")
    {}
protected:
    virtual void setupTranslations( void )
    {
        KGlobal::locale()->insertCatalog( "kofficefilters" );
    }
};

K_EXPORT_COMPONENT_FACTORY( libabiwordimport, ABIWORDImportFactory() )

// *Note for the reader of this code*
// Tags in lower case (e.g. <c>) are AbiWord's ones.
// Tags in upper case (e.g. <TEXT>) are KWord's ones.

// enum StackItemElementType is now in the file ImportFormatting.h

class StructureParser : public QXmlDefaultHandler
{
public:
    StructureParser(KoFilterChain* chain)
        :  m_chain(chain), m_pictureNumber(0), m_pictureFrameNumber(0), m_tableGroupNumber(0),
        m_timepoint(QDateTime::currentDateTime(Qt::UTC)), m_fatalerror(false)
    {
        createDocument();
        structureStack.setAutoDelete(true);
        StackItem *stackItem=new(StackItem);
        stackItem->elementType=ElementTypeBottom;
        stackItem->m_frameset=mainFramesetElement; // The default frame set.
        stackItem->stackElementText=mainFramesetElement; // This is more for DEBUG
        structureStack.push(stackItem); //Security item (not to empty the stack)
    }
    virtual ~StructureParser()
    {
        structureStack.clear();
    }
public:
    virtual bool startDocument(void);
    virtual bool endDocument(void);
    virtual bool startElement( const QString&, const QString&, const QString& name, const QXmlAttributes& attributes);
    virtual bool endElement( const QString&, const QString& , const QString& qName);
    virtual bool characters ( const QString & ch );
    virtual bool warning(const QXmlParseException& exception);
    virtual bool error(const QXmlParseException& exception);
    virtual bool fatalError(const QXmlParseException& exception);
public:
    inline QDomDocument getDocInfo(void) const { return m_info; }
    inline QDomDocument getDocument(void) const { return mainDocument; }
    inline bool wasFatalError(void) const { return m_fatalerror; }
protected:
    bool clearStackUntilParagraph(StackItemStack& auxilaryStack);
    bool complexForcedPageBreak(StackItem* stackItem);
private:
    // The methods that would need too much parameters are private instead of being static outside the class
    bool StartElementC(StackItem* stackItem, StackItem* stackCurrent,
        const QXmlAttributes& attributes);
    bool StartElementA(StackItem* stackItem, StackItem* stackCurrent,
        const QXmlAttributes& attributes);
    bool StartElementImage(StackItem* stackItem, StackItem* stackCurrent,
        const QXmlAttributes& attributes);
    bool EndElementD (StackItem* stackItem);
    bool EndElementM (StackItem* stackItem);
    bool StartElementSection(StackItem* stackItem, StackItem* stackCurrent,
        const QXmlAttributes& attributes);
    bool StartElementFoot(StackItem* stackItem, StackItem* stackCurrent,
        const QXmlAttributes& attributes);
    bool StartElementTable(StackItem* stackItem, StackItem* stackCurrent, const QXmlAttributes& attributes);
    bool StartElementCell(StackItem* stackItem, StackItem* stackCurrent,const QXmlAttributes& attributes);
private:
    void createDocument(void);
    void createDocInfo(void);
    QString indent; //DEBUG
    StackItemStack structureStack;
    QDomDocument mainDocument;
    QDomDocument m_info;
    QDomElement framesetsPluralElement; // <FRAMESETS>
    QDomElement mainFramesetElement;    // The main <FRAMESET> where the body text will be under.
    QDomElement m_picturesElement;      // <PICTURES>
    QDomElement m_paperElement;         // <PAPER>
    QDomElement m_paperBordersElement;  // <PAPERBORDER>
    QDomElement m_ignoreWordsElement;    // <SPELLCHECKIGNORELIST>
    StyleDataMap styleDataMap;
    KoFilterChain* m_chain;
    uint m_pictureNumber;                   // unique: increment *before* use
    uint m_pictureFrameNumber;              // unique: increment *before* use
    uint m_tableGroupNumber;                // unique: increment *before* use
    QMap<QString,QString> m_metadataMap;    // Map for <m> elements
    QDateTime m_timepoint;              // Date/time (for pictures)
    bool m_fatalerror;                  // Did a XML parsing fatal error happened?
};

// Element <c>

bool StructureParser::StartElementC(StackItem* stackItem, StackItem* stackCurrent, const QXmlAttributes& attributes)
{
    // <c> elements can be nested in <p> elements, in <a> elements or in other <c> elements
    // AbiWord does not nest <c> elements in other <c> elements, but explicitly allows external programs to do it!

    // <p> or <c> (not child of <a>)
    if ((stackCurrent->elementType==ElementTypeParagraph)||(stackCurrent->elementType==ElementTypeContent))
    {
        // Contents can have styles, however KWord cannot have character style.
        // Therefore we use the style if it exist, but we do not create it if not.
        QString strStyleProps;
        QString strStyleName=attributes.value("style").trimmed();
        if (!strStyleName.isEmpty())
        {
            StyleDataMap::Iterator it=styleDataMap.find(strStyleName);
            if (it!=styleDataMap.end())
            {
                strStyleProps=it.data().m_props;
            }
        }

        AbiPropsMap abiPropsMap;
        PopulateProperties(stackItem,strStyleProps,attributes,abiPropsMap,true);

        stackItem->elementType=ElementTypeContent;
        stackItem->stackElementParagraph=stackCurrent->stackElementParagraph;   // <PARAGRAPH>
        stackItem->stackElementText=stackCurrent->stackElementText;   // <TEXT>
        stackItem->stackElementFormatsPlural=stackCurrent->stackElementFormatsPlural; // <FORMATS>
        stackItem->pos=stackCurrent->pos; //Propagate the position
    }
    // <a> or <c> when child of <a>
    else if ((stackCurrent->elementType==ElementTypeAnchor)||(stackCurrent->elementType==ElementTypeAnchorContent))
    {
        stackItem->elementType=ElementTypeAnchorContent;
    }
    else
    {//we are not nested correctly, so consider it a parse error!
        kError(30506) << "parse error <c> tag nested neither in <p> nor in <c> nor in <a> but in "
            << stackCurrent->itemName << endl;
        return false;
    }
    return true;
}

bool charactersElementC (StackItem* stackItem, QDomDocument& mainDocument, const QString & ch)
{
    if  (stackItem->elementType==ElementTypeContent)
    {   // Normal <c>
        QDomElement elementText=stackItem->stackElementText;
        QDomElement elementFormatsPlural=stackItem->stackElementFormatsPlural;
        elementText.appendChild(mainDocument.createTextNode(ch));

        QDomElement formatElementOut=mainDocument.createElement("FORMAT");
        formatElementOut.setAttribute("id",1); // Normal text!
        formatElementOut.setAttribute("pos",stackItem->pos); // Start position
        formatElementOut.setAttribute("len",ch.length()); // Start position
        elementFormatsPlural.appendChild(formatElementOut); //Append to <FORMATS>
        stackItem->pos+=ch.length(); // Adapt new starting position

        AddFormat(formatElementOut, stackItem, mainDocument);
    }
    else if (stackItem->elementType==ElementTypeAnchorContent)
    {
        // Add characters to the link name
        stackItem->strTemp2+=ch;
        // TODO: how can we care about the text format?
    }
    else
    {
        kError(30506) << "Internal error (in charactersElementC)" << endl;
    }

	return true;
}

bool EndElementC (StackItem* stackItem, StackItem* stackCurrent)
{
    if (stackItem->elementType==ElementTypeContent)
    {
        stackItem->stackElementText.normalize();
        stackCurrent->pos=stackItem->pos; //Propagate the position back to the parent element
    }
    else if (stackItem->elementType==ElementTypeAnchorContent)
    {
        stackCurrent->strTemp2+=stackItem->strTemp2; //Propagate the link name back to the parent element
    }
    else
    {
        kError(30506) << "Wrong element type!! Aborting! (</c> in StructureParser::endElement)" << endl;
        return false;
    }
    return true;
}

// Element <a>
bool StructureParser::StartElementA(StackItem* stackItem, StackItem* stackCurrent, const QXmlAttributes& attributes)
{
    // <a> elements can be nested in <p> elements
    if (stackCurrent->elementType==ElementTypeParagraph)
    {

        //AbiPropsMap abiPropsMap;
        //PopulateProperties(stackItem,QString::null,attributes,abiPropsMap,true);

        stackItem->elementType=ElementTypeAnchor;
        stackItem->stackElementParagraph=stackCurrent->stackElementParagraph;   // <PARAGRAPH>
        stackItem->stackElementText=stackCurrent->stackElementText;   // <TEXT>
        stackItem->stackElementFormatsPlural=stackCurrent->stackElementFormatsPlural; // <FORMATS>
        stackItem->pos=stackCurrent->pos; //Propagate the position
        stackItem->strTemp1=attributes.value("xlink:href").trimmed(); // link reference
        stackItem->strTemp2=QString::null; // link name

        // We must be careful: AbiWord permits anchors to bookmarks.
        //  However, KWord does not know what a bookmark is.
        if (stackItem->strTemp1[0]=='#')
        {
            kWarning(30506) << "Anchor <a> to bookmark: " << stackItem->strTemp1 << endl
                << " Processing <a> like <c>" << endl;
            // We have a reference to a bookmark. Therefore treat it as a normal content <c>
            return StartElementC(stackItem, stackCurrent, attributes);
        }
    }
    else
    {//we are not nested correctly, so consider it a parse error!
        kError(30506) << "parse error <a> tag not a child of <p> but of "
            << stackCurrent->itemName << endl;
        return false;
    }
    return true;
}

static bool charactersElementA (StackItem* stackItem, const QString & ch)
{
    // Add characters to the link name
    stackItem->strTemp2+=ch;
	return true;
}

static bool EndElementA (StackItem* stackItem, StackItem* stackCurrent, QDomDocument& mainDocument)
{
    if (!stackItem->elementType==ElementTypeAnchor)
    {
        kError(30506) << "Wrong element type!! Aborting! (</a> in StructureParser::endElement)" << endl;
        return false;
    }

    QDomElement elementText=stackItem->stackElementText;
    elementText.appendChild(mainDocument.createTextNode("#"));

    QDomElement formatElement=mainDocument.createElement("FORMAT");
    formatElement.setAttribute("id",4); // Variable
    formatElement.setAttribute("pos",stackItem->pos); // Start position
    formatElement.setAttribute("len",1); // Start position

    QDomElement variableElement=mainDocument.createElement("VARIABLE");
    formatElement.appendChild(variableElement);

    QDomElement typeElement=mainDocument.createElement("TYPE");
    typeElement.setAttribute("key","STRING");
    typeElement.setAttribute("type",9); // link
    typeElement.setAttribute("text",stackItem->strTemp2);
    variableElement.appendChild(typeElement); //Append to <VARIABLE>

    QDomElement linkElement=mainDocument.createElement("LINK");
    linkElement.setAttribute("hrefName",stackItem->strTemp1);
    linkElement.setAttribute("linkName",stackItem->strTemp2);
    variableElement.appendChild(linkElement); //Append to <VARIABLE>

    // Now work on stackCurrent
    stackCurrent->stackElementFormatsPlural.appendChild(formatElement);
    stackCurrent->pos++; //Propagate the position back to the parent element

    return true;
}

// Element <p>

bool StartElementP(StackItem* stackItem, StackItem* stackCurrent,
    QDomDocument& mainDocument,
    StyleDataMap& styleDataMap, const QXmlAttributes& attributes)
{
    // We must prepare the style
    QString strStyle=attributes.value("style");
    if (strStyle.isEmpty())
    {
        strStyle="Normal";
    }
    StyleDataMap::ConstIterator it=styleDataMap.useOrCreateStyle(strStyle);

    QString strLevel=attributes.value("level");
    int level;
    if (strLevel.isEmpty())
    {
        // We have not "level" attribute, so we must use the style's level.
        level=it.data().m_level;
    }
    else
    {
        // We have a "level" attribute, so it overrides the style's level.
        level=strStyle.toInt();
    }

    QDomElement elementText=stackCurrent->stackElementText;
    QDomElement paragraphElementOut=mainDocument.createElement("PARAGRAPH");
    stackCurrent->m_frameset.appendChild(paragraphElementOut);

    QDomElement textElementOut=mainDocument.createElement("TEXT");
    paragraphElementOut.appendChild(textElementOut);
    QDomElement formatsPluralElementOut=mainDocument.createElement("FORMATS");
    paragraphElementOut.appendChild(formatsPluralElementOut);

    AbiPropsMap abiPropsMap;
    PopulateProperties(stackItem,it.data().m_props,attributes,abiPropsMap,false);

    stackItem->elementType=ElementTypeParagraph;
    stackItem->stackElementParagraph=paragraphElementOut; // <PARAGRAPH>
    stackItem->stackElementText=textElementOut; // <TEXT>
    stackItem->stackElementFormatsPlural=formatsPluralElementOut; // <FORMATS>
    stackItem->pos=0; // No text characters yet

    // Now we populate the layout
    QDomElement layoutElement=mainDocument.createElement("LAYOUT");
    paragraphElementOut.appendChild(layoutElement);

    AddLayout(strStyle,layoutElement, stackItem, mainDocument, abiPropsMap, level, false);

    return true;
}

bool charactersElementP (StackItem* stackItem, QDomDocument& mainDocument, const QString & ch)
{
    QDomElement elementText=stackItem->stackElementText;

    elementText.appendChild(mainDocument.createTextNode(ch));

    stackItem->pos+=ch.length(); // Adapt new starting position

    return true;
}

bool EndElementP (StackItem* stackItem)
{
    if (!stackItem->elementType==ElementTypeParagraph)
    {
        kError(30506) << "Wrong element type!! Aborting! (in endElementP)" << endl;
        return false;
    }
    stackItem->stackElementText.normalize();
    return true;
}

static bool StartElementField(StackItem* stackItem, StackItem* stackCurrent,
    QDomDocument& mainDocument, const QXmlAttributes& attributes)
{
    // <field> element elements can be nested in <p>
    if (stackCurrent->elementType==ElementTypeParagraph)
    {
        QString strType=attributes.value("type").trimmed();
        kDebug(30506)<<"<field> type:"<<strType<<endl;

        AbiPropsMap abiPropsMap;
        PopulateProperties(stackItem,QString::null,attributes,abiPropsMap,true);

        stackItem->elementType=ElementTypeEmpty;

        // We create a format element
        QDomElement variableElement=mainDocument.createElement("VARIABLE");

        if (!ProcessField(mainDocument, variableElement, strType, attributes))
        {
            // The field type was not recognised,
            //   therefore write the field type in red as normal text
            kWarning(30506) << "Unknown <field> type: " << strType << endl;
            QDomElement formatElement=mainDocument.createElement("FORMAT");
            formatElement.setAttribute("id",1); // Variable
            formatElement.setAttribute("pos",stackItem->pos); // Start position
            formatElement.setAttribute("len",strType.length());

            formatElement.appendChild(variableElement);

            // Now work on stackCurrent
            stackCurrent->stackElementFormatsPlural.appendChild(formatElement);
            stackCurrent->stackElementText.appendChild(mainDocument.createTextNode(strType));
            stackCurrent->pos+=strType.length(); // Adjust position

            // Add formating (use stackItem)
            stackItem->fgColor.setRgb(255,0,0);
            AddFormat(formatElement, stackItem, mainDocument);
            return true;
        }

        // We create a format element
        QDomElement formatElement=mainDocument.createElement("FORMAT");
        formatElement.setAttribute("id",4); // Variable
        formatElement.setAttribute("pos",stackItem->pos); // Start position
        formatElement.setAttribute("len",1);

        formatElement.appendChild(variableElement);

        // Now work on stackCurrent
        stackCurrent->stackElementFormatsPlural.appendChild(formatElement);
        stackCurrent->stackElementText.appendChild(mainDocument.createTextNode("#"));
        stackCurrent->pos++; // Adjust position

        // Add formating (use stackItem)
        AddFormat(formatElement, stackItem, mainDocument);

    }
    else
    {//we are not nested correctly, so consider it a parse error!
        kError(30506) << "parse error <field> tag not nested in <p> but in "
            << stackCurrent->itemName << endl;
        return false;
    }
    return true;
}

// <s> (style)
static bool StartElementS(StackItem* stackItem, StackItem* /*stackCurrent*/,
    const QXmlAttributes& attributes, StyleDataMap& styleDataMap)
{
    // We do not assume when we are called.
    // We also do not care if a style is defined multiple times.
    stackItem->elementType=ElementTypeEmpty;

    QString strStyleName=attributes.value("name").trimmed();

    if (strStyleName.isEmpty())
    {
        kWarning(30506) << "Style has no name!" << endl;
    }
    else
    {
        QString strLevel=attributes.value("level");
        int level;
        if (strLevel.isEmpty())
            level=-1; //TODO/FIXME: might be wrong if the style is based on another
        else
            level=strLevel.toInt();
        QString strBasedOn=attributes.value("basedon").simplified();
        styleDataMap.defineNewStyleFromOld(strStyleName,strBasedOn,level,attributes.value("props"));
        kDebug(30506) << " Style name: " << strStyleName << endl
            << " Based on: " << strBasedOn  << endl
            << " Level: " << level << endl
            << " Props: " << attributes.value("props") << endl;
    }

    return true;
}

// <image>
bool StructureParser::StartElementImage(StackItem* stackItem, StackItem* stackCurrent,
    const QXmlAttributes& attributes)
{
    // <image> elements can be nested in <p> or <c> elements
    if ((stackCurrent->elementType!=ElementTypeParagraph) && (stackCurrent->elementType!=ElementTypeContent))
    {//we are not nested correctly, so consider it a parse error!
        kError(30506) << "parse error <image> tag nested neither in <p> nor in <c> but in "
            << stackCurrent->itemName << endl;
        return false;
    }
    stackItem->elementType=ElementTypeEmpty;

    QString strDataId=attributes.value("dataid").trimmed();

    AbiPropsMap abiPropsMap;
    abiPropsMap.splitAndAddAbiProps(attributes.value("props"));

    double height=ValueWithLengthUnit(abiPropsMap["height"].getValue());
    double width =ValueWithLengthUnit(abiPropsMap["width" ].getValue());

    kDebug(30506) << "Image: " << strDataId << " height: " << height << " width: " << width << endl;

    // TODO: image properties

    if (strDataId.isEmpty())
    {
        kWarning(30506) << "Image has no data id!" << endl;
    }
    else
    {
        kDebug(30506) << "Image: " << strDataId << endl;
    }

    QString strPictureFrameName(i18nc("Frameset name","Picture %1",++m_pictureFrameNumber));

    // Create the frame set of the image

    QDomElement framesetElement=mainDocument.createElement("FRAMESET");
    framesetElement.setAttribute("frameType",2);
    framesetElement.setAttribute("frameInfo",0);
    framesetElement.setAttribute("visible",1);
    framesetElement.setAttribute("name",strPictureFrameName);
    framesetsPluralElement.appendChild(framesetElement);

    QDomElement frameElementOut=mainDocument.createElement("FRAME");
    frameElementOut.setAttribute("left",0);
    frameElementOut.setAttribute("top",0);
    frameElementOut.setAttribute("bottom",height);
    frameElementOut.setAttribute("right" ,width );
    frameElementOut.setAttribute("runaround",1);
    // TODO: a few attributes are missing
    framesetElement.appendChild(frameElementOut);

    QDomElement element=mainDocument.createElement("PICTURE");
    element.setAttribute("keepAspectRatio","true");
    framesetElement.setAttribute("frameType",2); // Picture
    framesetElement.appendChild(element);

    QDomElement key=mainDocument.createElement("KEY");
    key.setAttribute("filename",strDataId);
    key.setAttribute("year",m_timepoint.date().year());
    key.setAttribute("month",m_timepoint.date().month());
    key.setAttribute("day",m_timepoint.date().day());
    key.setAttribute("hour",m_timepoint.time().hour());
    key.setAttribute("minute",m_timepoint.time().minute());
    key.setAttribute("second",m_timepoint.time().second());
    key.setAttribute("msec",m_timepoint.time().msec());
    element.appendChild(key);

    // Now use the image's frame set
    QDomElement elementText=stackItem->stackElementText;
    QDomElement elementFormatsPlural=stackItem->stackElementFormatsPlural;
    elementText.appendChild(mainDocument.createTextNode("#"));

    QDomElement formatElementOut=mainDocument.createElement("FORMAT");
    formatElementOut.setAttribute("id",6); // Normal text!
    formatElementOut.setAttribute("pos",stackItem->pos); // Start position
    formatElementOut.setAttribute("len",1); // Start position
    elementFormatsPlural.appendChild(formatElementOut); //Append to <FORMATS>

    // WARNING: we must change the position in stackCurrent!
    stackCurrent->pos++; // Adapt new starting position

    QDomElement anchor=mainDocument.createElement("ANCHOR");
    // No name attribute!
    anchor.setAttribute("type","frameset");
    anchor.setAttribute("instance",strPictureFrameName);
    formatElementOut.appendChild(anchor);

    return true;
}

// <d>
static bool StartElementD(StackItem* stackItem, StackItem* /*stackCurrent*/,
    const QXmlAttributes& attributes)
{
    // We do not assume when we are called or if we are or not a child of <data>
    // However, we assume that we are after all <image> elements
    stackItem->elementType=ElementTypeRealData;

    QString strName=attributes.value("name").trimmed();
    kDebug(30506) << "Data: " << strName << endl;

    QString strBase64=attributes.value("base64").trimmed();
    QString strMime=attributes.value("mime").trimmed();

    if (strName.isEmpty())
    {
        kWarning(30506) << "Data has no name!" << endl;
        stackItem->elementType=ElementTypeEmpty;
        return true;
    }

    if (strMime.isEmpty())
    {
        // Old AbiWord files had no mime types for images but the data were base64-coded PNG
        strMime="image/png";
        strBase64="yes";
    }

    stackItem->fontName=strName;        // Store the data name as font name.
    stackItem->bold=(strBase64=="yes"); // Store base64-coded as bold
    stackItem->strTemp1=strMime;        // Mime type
    stackItem->strTemp2=QString::null;  // Image data

    return true;
}

static bool CharactersElementD (StackItem* stackItem, QDomDocument& /*mainDocument*/, const QString & ch)
{
    // As we have no guarantee to have the whole stream in one call, we must store the data.
    stackItem->strTemp2+=ch;
    return true;
}

bool StructureParser::EndElementD (StackItem* stackItem)
{
    if (!stackItem->elementType==ElementTypeRealData)
    {
        kError(30506) << "Wrong element type!! Aborting! (in endElementD)" << endl;
        return false;
    }
    if (!m_chain)
    {
        kError(30506) << "No filter chain! Aborting! (in endElementD)" << endl;
        return false;
    }

    bool isSvg=false;  // SVG ?

    QString extension;

    // stackItem->strTemp1 contains the mime type
    if (stackItem->strTemp1=="image/png")
    {
        extension=".png";
    }
    else if (stackItem->strTemp1=="image/jpeg") // ### FIXME: in fact it does not exist in AbiWord
    {
        extension=".jpeg";
    }
    else if (stackItem->strTemp1=="image/svg-xml") //Yes it is - not +
    {
        extension=".svg";
        isSvg=true;
    }
    else
    {
        kWarning(30506) << "Unknown or unsupported mime type: "
            << stackItem->strTemp1 << endl;
        return true;
    }

    QString strStoreName;
    strStoreName="pictures/picture";
    strStoreName+=QString::number(++m_pictureNumber);
    strStoreName+=extension;

    QString strDataId=stackItem->fontName;  // AbiWord's data id
    QDomElement key=mainDocument.createElement("KEY");
    key.setAttribute("filename",strDataId);
    key.setAttribute("year",m_timepoint.date().year());
    key.setAttribute("month",m_timepoint.date().month());
    key.setAttribute("day",m_timepoint.date().day());
    key.setAttribute("hour",m_timepoint.time().hour());
    key.setAttribute("minute",m_timepoint.time().minute());
    key.setAttribute("second",m_timepoint.time().second());
    key.setAttribute("msec",m_timepoint.time().msec());
    key.setAttribute("name",strStoreName);
    m_picturesElement.appendChild(key);

    KoStoreDevice* out=m_chain->storageFile(strStoreName, KoStore::Write);
    if(!out)
    {
        kError(30506) << "Unable to open output file for: " << stackItem->fontName << " Storage: " << strStoreName << endl;
        return false;
    }

    if (stackItem->bold) // Is base64-coded?
    {
        kDebug(30506) << "Decode and write base64 stream: " << stackItem->fontName << endl;
        // We need to decode the base64 stream
        // However KCodecs has no QString to QByteArray decoder!
        QByteArray base64Stream=stackItem->strTemp2.utf8(); // Use utf8 to avoid corruption of data
        QByteArray binaryStream;
        KCodecs::base64Decode(base64Stream, binaryStream);
        out->write(binaryStream, binaryStream.count());
    }
    else
    {
        // Unknown text format!
        kDebug(30506) << "Write character stream: " << stackItem->fontName << endl;
        // We strip the white space in front to avoid white space before a XML declaration
        Q3CString strOut=stackItem->strTemp2.trimmed().utf8();
        out->write(strOut,strOut.length());
    }

    return true;
}

// <m>
static bool StartElementM(StackItem* stackItem, StackItem* /*stackCurrent*/,
    const QXmlAttributes& attributes)
{
    // We do not assume when we are called or if we are or not a child of <metadata>
    stackItem->elementType=ElementTypeRealMetaData;

    QString strKey=attributes.value("key").trimmed();
    kDebug(30506) << "Metadata key: " << strKey << endl;

    if (strKey.isEmpty())
    {
        kWarning(30506) << "Metadata has no key!" << endl;
        stackItem->elementType=ElementTypeIgnore;
        return true;
    }

    stackItem->strTemp1=strKey;        // Key
    stackItem->strTemp2=QString::null;  // Meta data

    return true;
}

static bool CharactersElementM (StackItem* stackItem, const QString & ch)
{
    // As we have no guarantee to have the whole data in one call, we must store the data.
    stackItem->strTemp2+=ch;
    return true;
}

bool StructureParser::EndElementM (StackItem* stackItem)
{
    if (!stackItem->elementType==ElementTypeRealData)
    {
        kError(30506) << "Wrong element type!! Aborting! (in endElementM)" << endl;
        return false;
    }

    if (stackItem->strTemp1.isEmpty())
    {
        // Probably an internal error!
        kError(30506) << "Key name was erased! Aborting! (in endElementM)" << endl;
        return false;
    }

    // Just add it to the metadata map, we do not do something special with the values.
    m_metadataMap[stackItem->strTemp1]=stackItem->strTemp2;

    return true;
}

// <br> (forced line break)
static bool StartElementBR(StackItem* stackItem, StackItem* stackCurrent,
    QDomDocument& mainDocument)
{
    // <br> elements are mostly in <c> but can also be in <p>
    if ((stackCurrent->elementType==ElementTypeParagraph)
        || (stackCurrent->elementType==ElementTypeContent))
    {
        stackItem->elementType=ElementTypeEmpty;

        // Now work on stackCurrent

        if  (stackCurrent->elementType==ElementTypeContent)
        {
            // Child <c>, so we have to add formating of <c>
            QDomElement formatElement=mainDocument.createElement("FORMAT");
            formatElement.setAttribute("id",1); // Normal text!
            formatElement.setAttribute("pos",stackCurrent->pos); // Start position
            formatElement.setAttribute("len",1);
            AddFormat(formatElement, stackCurrent, mainDocument); // Add the format of the parent <c>
            stackCurrent->stackElementFormatsPlural.appendChild(formatElement); //Append to <FORMATS>
        }

        stackCurrent->stackElementText.appendChild(mainDocument.createTextNode(QChar(10))); // Add a LINE FEED
        stackCurrent->pos++; // Adjust position

    }
    else
    {//we are not nested correctly, so consider it a parse error!
        kError(30506) << "parse error <br> tag not nested in <p> or <c> but in "
            << stackCurrent->itemName << endl;
        return false;
    }
    return true;
}

// <cbr> (forced column break, not supported)
// <pbr> (forced page break)
static bool StartElementPBR(StackItem* /*stackItem*/, StackItem* stackCurrent,
    QDomDocument& mainDocument)
{
    // We are sure to be the child of a <p> element

    // The following code is similar to the one in StartElementP
    // We use mainFramesetElement here not to be dependant that <section> has happened before
    QDomElement paragraphElementOut=mainDocument.createElement("PARAGRAPH");
    stackCurrent->m_frameset.appendChild(paragraphElementOut);
    QDomElement textElementOut=mainDocument.createElement("TEXT");
    paragraphElementOut.appendChild(textElementOut);
    QDomElement formatsPluralElementOut=mainDocument.createElement("FORMATS");
    paragraphElementOut.appendChild(formatsPluralElementOut);

    // We must now copy/clone the layout of elementText.

    QDomNodeList nodeList=stackCurrent->stackElementParagraph.elementsByTagName("LAYOUT");

    if (!nodeList.count())
    {
        kError(30506) << "Unable to find <LAYOUT> element! Aborting! (in StartElementPBR)" <<endl;
        return false;
    }

    // Now clone it
    QDomNode newNode=nodeList.item(0).cloneNode(true); // We make a deep cloning of the first element/node
    if (newNode.isNull())
    {
        kError(30506) << "Unable to clone <LAYOUT> element! Aborting! (in StartElementPBR)" <<endl;
        return false;
    }
    paragraphElementOut.appendChild(newNode);

    // We need a page break!
    QDomElement oldLayoutElement=nodeList.item(0).toElement();
    if (oldLayoutElement.isNull())
    {
        kError(30506) << "Cannot find old <LAYOUT> element! Aborting! (in StartElementPBR)" <<endl;
        return false;
    }
    // We have now to add a element <PAGEBREAKING>
    // TODO/FIXME: what if there is already one?
    QDomElement pagebreakingElement=mainDocument.createElement("PAGEBREAKING");
    pagebreakingElement.setAttribute("linesTogether","false");
    pagebreakingElement.setAttribute("hardFrameBreak","false");
    pagebreakingElement.setAttribute("hardFrameBreakAfter","true");
    oldLayoutElement.appendChild(pagebreakingElement);

    // Now that we have done with the old paragraph,
    //  we can write stackCurrent with the data of the new one!
    // NOTE: The following code is similar to the one in StartElementP but we are working on stackCurrent!
    stackCurrent->elementType=ElementTypeParagraph;
    stackCurrent->stackElementParagraph=paragraphElementOut; // <PARAGRAPH>
    stackCurrent->stackElementText=textElementOut; // <TEXT>
    stackCurrent->stackElementFormatsPlural=formatsPluralElementOut; // <FORMATS>
    stackCurrent->pos=0; // No text character yet

    return true;
}

// <pagesize>
static bool StartElementPageSize(QDomElement& paperElement, const QXmlAttributes& attributes)
{
    if (attributes.value("page-scale").toDouble()!=1.0)
    {
        kWarning(30506) << "Ignoring unsupported page scale: " << attributes.value("page-scale") << endl;
    }

    int kwordOrientation;
    QString strOrientation=attributes.value("orientation").trimmed();

    if (strOrientation=="portrait")
    {
        kwordOrientation=0;
    }
    else if (strOrientation=="landscape")
    {
        kwordOrientation=1;
    }
    else
    {
        kWarning(30506) << "Unknown page orientation: " << strOrientation << "! Ignoring! " << endl;
        kwordOrientation=0;
    }

    double kwordHeight;
    double kwordWidth;

    QString strPageType=attributes.value("pagetype").trimmed();

    // Do we know the page size or do we need to measure?
    // For page formats that KWord knows, use our own values in case the values in the file would be wrong.

    KoFormat kwordFormat = KoPageFormat::formatFromString(strPageType);

    if (kwordFormat==PG_CUSTOM)
    {
        kDebug(30506) << "Custom or other page format found: " << strPageType << endl;

        double height = attributes.value("height").toDouble();
        double width  = attributes.value("width" ).toDouble();

        QString strUnits = attributes.value("units").trimmed();

        kDebug(30506) << "Explicit page size: "
         << height << " " << strUnits << " x " << width << " " << strUnits
         << endl;

        if (strUnits=="cm")
        {
            kwordHeight = CentimetresToPoints(height);
            kwordWidth  = CentimetresToPoints(width);
        }
        else if (strUnits=="inch")
        {
            kwordHeight = InchesToPoints(height);
            kwordWidth  = InchesToPoints(width);
        }
        else if (strUnits=="mm")
        {
            kwordHeight = MillimetresToPoints(height);
            kwordWidth  = MillimetresToPoints(width);
        }
        else
        {
            kwordHeight = 0.0;
            kwordWidth  = 0.0;
            kWarning(30506) << "Unknown unit type: " << strUnits << endl;
        }
    }
    else
    {
        // We have a format known by KOffice, so use KOffice's functions
        kwordHeight = MillimetresToPoints(KoPageFormat::height(kwordFormat,PG_PORTRAIT));
        kwordWidth  = MillimetresToPoints(KoPageFormat::width (kwordFormat,PG_PORTRAIT));
    }

    if ((kwordHeight <= 1.0) || (kwordWidth <= 1.0))
        // At least one of the two values is ridiculous
    {
        kWarning(30506) << "Page width or height is too small: "
         << kwordHeight << "x" << kwordWidth << endl;
        // As we have no correct page size, we assume we have A4
        kwordFormat = PG_DIN_A4;
        kwordHeight = CentimetresToPoints(29.7);
        kwordWidth  = CentimetresToPoints(21.0);
    }

    // Now that we have gathered all the page size data, put it in the right element!

    if (paperElement.isNull())
    {
        kError(30506) << "<PAPER> element cannot be accessed! Aborting!" << endl;
        return false;
    }

    paperElement.setAttribute("format",kwordFormat);
    paperElement.setAttribute("width",kwordWidth);
    paperElement.setAttribute("height",kwordHeight);
    paperElement.setAttribute("orientation",kwordOrientation);

    return true;
}


bool StructureParser::complexForcedPageBreak(StackItem* stackItem)
{
    // We are not a child of a <p> element, so we cannot use StartElementPBR directly

    StackItemStack auxilaryStack;

    if (!clearStackUntilParagraph(auxilaryStack))
    {
        kError(30506) << "Could not clear stack until a paragraph!" << endl;
        return false;
    }

    // Now we are a child of a <p> element!

    bool success=StartElementPBR(stackItem,structureStack.current(),mainDocument);

    // Now restore the stack

    StackItem* stackCurrent=structureStack.current();
    StackItem* item;
    while (auxilaryStack.count()>0)
    {
        item=auxilaryStack.pop();
        // We cannot put back the item on the stack like that.
        // We must set a few values for each item.
        item->pos=0; // Start at position 0
        item->stackElementParagraph=stackCurrent->stackElementParagraph; // new <PARAGRAPH>
        item->stackElementText=stackCurrent->stackElementText; // new <TEXT>
        item->stackElementFormatsPlural=stackCurrent->stackElementFormatsPlural; // new <FORMATS>
        structureStack.push(item);
    }

    return success;
}


// <section>
bool StructureParser::StartElementSection(StackItem* stackItem, StackItem* /*stackCurrent*/,
    const QXmlAttributes& attributes)
{
    //TODO: non main text sections (e.g. footers)
    stackItem->elementType=ElementTypeSection;

    AbiPropsMap abiPropsMap;
    // Treat the props attributes in the two available flavors: lower case and upper case.
    kDebug(30506)<< "========== props=\"" << attributes.value("props") << "\"" << endl;
    abiPropsMap.splitAndAddAbiProps(attributes.value("props"));
    abiPropsMap.splitAndAddAbiProps(attributes.value("PROPS")); // PROPS is deprecated

    // TODO: only the first main text section should change the page margins
    // TODO;   (as KWord does not allow different page sizes/margins for the same document)
    if (true && (!m_paperBordersElement.isNull()))
    {
        QString str;
        str=abiPropsMap["page-margin-top"].getValue();
        if (!str.isEmpty())
        {
            m_paperBordersElement.setAttribute("top",ValueWithLengthUnit(str));
        }
        str=abiPropsMap["page-margin-left"].getValue();
        if (!str.isEmpty())
        {
            m_paperBordersElement.setAttribute("left",ValueWithLengthUnit(str));
        }
        str=abiPropsMap["page-margin-bottom"].getValue();
        if (!str.isEmpty())
        {
            m_paperBordersElement.setAttribute("bottom",ValueWithLengthUnit(str));
        }
        str=abiPropsMap["page-margin-right"].getValue();
        if (!str.isEmpty())
        {
            m_paperBordersElement.setAttribute("right",ValueWithLengthUnit(str));
        }
    }
    return true;
}

// <iw>

static bool EndElementIW(StackItem* stackItem, StackItem* /*stackCurrent*/,
    QDomDocument& mainDocument, QDomElement& m_ignoreWordsElement)
{
    if (!stackItem->elementType==ElementTypeIgnoreWord)
    {
        kError(30506) << "Wrong element type!! Aborting! (in endElementIW)" << endl;
        return false;
    }
    QDomElement wordElement=mainDocument.createElement("SPELLCHECKIGNOREWORD");
    wordElement.setAttribute("word",stackItem->strTemp2.trimmed());
    m_ignoreWordsElement.appendChild(wordElement);
    return true;
}

// <foot>
bool StructureParser::StartElementFoot(StackItem* stackItem, StackItem* /*stackCurrent*/,
    const QXmlAttributes& /*attributes*/)
{
#if 0
    stackItem->elementType=ElementTypeFoot;

    const QString id(attributes.value("endnote-id").trimmed());
    kDebug(30506) << "Foot note id: " << id << endl;

    if (id.isEmpty())
    {
        kWarning(30506) << "Footnote has no id!" << endl;
        stackItem->elementType=ElementTypeIgnore;
        return true;
    }

    // We need to create a frameset for the foot note.
    QDomElement framesetElement(mainDocument.createElement("FRAMESET"));
    framesetElement.setAttribute("frameType",1);
    framesetElement.setAttribute("frameInfo",7);
    framesetElement.setAttribute("visible",1);
    framesetElement.setAttribute("name",getFootnoteFramesetName(id));
    framesetsPluralElement.appendChild(framesetElement);

    QDomElement frameElementOut(mainDocument.createElement("FRAME"));
    //frameElementOut.setAttribute("left",28);
    //frameElementOut.setAttribute("top",42);
    //frameElementOut.setAttribute("bottom",566);
    //frameElementOut.setAttribute("right",798);
    frameElementOut.setAttribute("runaround",1);
    // ### TODO: a few attributes are missing
    framesetElement.appendChild(frameElementOut);

    stackItem->m_frameset=framesetElement;
#else
    stackItem->elementType=ElementTypeIgnore;
#endif
    return true;
}

// Element <table>
bool StructureParser::StartElementTable(StackItem* stackItem, StackItem* stackCurrent,
    const QXmlAttributes& attributes)
{
#if 1
    // In KWord, inline tables are inside a paragraph.
    // In AbiWord, tables are outside any paragraph.

    QStringList widthList;
    widthList.split('/', attributes.value("table-column-props"), false);
    const uint columns = widthList.size();
    stackItem->m_doubleArray.detach(); // Be sure not to modify parents
    stackItem->m_doubleArray.resize(columns+1); // All left positions but the last right one
    stackItem->m_doubleArray[0] = 0.0;
    QStringList::ConstIterator it;
    uint i;
    for ( i=0, it=widthList.begin(); i<columns; ++i, ++it )
    {
        kDebug(30506) << "Column width: " << (*it) << " cooked " << ValueWithLengthUnit(*it) << endl;
        stackItem->m_doubleArray[i+1] = ValueWithLengthUnit(*it) + stackItem->m_doubleArray[i];
    }
    // ### TODO: in case of automatic column widths, we have not any width given by AbiWord

    const uint tableNumber(++m_tableGroupNumber);
    const QString tableName(i18n("Table %1",tableNumber));

    QDomElement elementText=stackCurrent->stackElementText;
    QDomElement paragraphElementOut=mainDocument.createElement("PARAGRAPH");
    stackCurrent->m_frameset.appendChild(paragraphElementOut);

    QDomElement textElementOut(mainDocument.createElement("TEXT"));
    textElementOut.appendChild(mainDocument.createTextNode("#"));
    paragraphElementOut.appendChild(textElementOut);

    QDomElement formatsPluralElementOut=mainDocument.createElement("FORMATS");
    paragraphElementOut.appendChild(formatsPluralElementOut);

    QDomElement elementFormat(mainDocument.createElement("FORMAT"));
    elementFormat.setAttribute("id",6);
    elementFormat.setAttribute("pos",0);
    elementFormat.setAttribute("len",1);
    formatsPluralElementOut.appendChild(elementFormat);

    QDomElement elementAnchor(mainDocument.createElement("ANCHOR"));
    elementAnchor.setAttribute("type","frameset");
    elementAnchor.setAttribute("instance",tableName);
    elementFormat.appendChild(elementAnchor);

    stackItem->elementType=ElementTypeTable;
    stackItem->stackElementParagraph=paragraphElementOut; // <PARAGRAPH>
    stackItem->stackElementText=textElementOut; // <TEXT>
    stackItem->stackElementFormatsPlural=formatsPluralElementOut; // <FORMATS>
    stackItem->strTemp1=tableName;
    stackItem->strTemp2=QString::number(tableNumber); // needed as I18N does not allow adding phrases
    stackItem->pos=1; // Just #

    // Now we populate the layout
    QDomElement layoutElement=mainDocument.createElement("LAYOUT");
    paragraphElementOut.appendChild(layoutElement);

    AbiPropsMap abiPropsMap;
    styleDataMap.useOrCreateStyle("Normal"); // We might have to create the "Normal" style.
    AddLayout("Normal", layoutElement, stackItem, mainDocument, abiPropsMap, 0, false);
#else
    stackItem->elementType=ElementTypeIgnore;
#endif
    return true;
}

// <cell>
bool StructureParser::StartElementCell(StackItem* stackItem, StackItem* stackCurrent,
    const QXmlAttributes& attributes)
{
#if 1
    if (stackCurrent->elementType!=ElementTypeTable)
    {
        kError(30506) << "Wrong element type!! Aborting! (in StructureParser::endElementCell)" << endl;
        return false;
    }

    stackItem->elementType=ElementTypeCell;

    const QString tableName(stackCurrent->strTemp1);
    kDebug(30506) << "Table name: " << tableName << endl;

    if (tableName.isEmpty())
    {
        kError(30506) << "Table name is empty! Aborting!" << endl;
        return false;
    }

    AbiPropsMap abiPropsMap;
    abiPropsMap.splitAndAddAbiProps(attributes.value("props")); // Do not check PROPS

    // We abuse the attach number to know the row and col numbers.
    const uint row=abiPropsMap["top-attach"].getValue().toUInt();
    const uint col=abiPropsMap["left-attach"].getValue().toUInt();

    if ( col >= stackItem->m_doubleArray.size() )
    {
        // We do not know the right position of this column, so improvise. (### TODO)
        // We play on the fact that QByteArray uses shallow copies by default.
        //  (We do want that the change is known at <table> level)
        stackItem->m_doubleArray.resize( stackItem->m_doubleArray.size() + 1, Q3GArray::SpeedOptim );
        stackItem->m_doubleArray[col+1] = stackItem->m_doubleArray[col] + 72; // Try 1 inch
    }

    const QString frameName(i18nc("Frameset name","Table %3, row %1, column %2",row,col,stackCurrent->strTemp2)); // As the stack could be wrong, be careful and use the string as last!

    // We need to create a frameset for the cell
    QDomElement framesetElement(mainDocument.createElement("FRAMESET"));
    framesetElement.setAttribute("frameType",1);
    framesetElement.setAttribute("frameInfo",0);
    framesetElement.setAttribute("visible",1);
    framesetElement.setAttribute("name",frameName);
    framesetElement.setAttribute("row",row);
    framesetElement.setAttribute("col",col);
    framesetElement.setAttribute("rows",1); // ### TODO: rowspan
    framesetElement.setAttribute("cols",1); // ### TODO: colspan
    framesetElement.setAttribute("grpMgr",tableName);
    framesetsPluralElement.appendChild(framesetElement);

    QDomElement frameElementOut(mainDocument.createElement("FRAME"));
    frameElementOut.setAttribute( "left", stackItem->m_doubleArray[col] );
    frameElementOut.setAttribute( "right", stackItem->m_doubleArray[col+1] );
    frameElementOut.setAttribute("top",0);
    frameElementOut.setAttribute("bottom",0);
    frameElementOut.setAttribute("runaround",1);
    frameElementOut.setAttribute("autoCreateNewFrame",0); // Very important for cell growing!
    // ### TODO: a few attributes are missing
    framesetElement.appendChild(frameElementOut);

    stackItem->m_frameset=framesetElement;
    QDomElement nullDummy;
    stackItem->stackElementParagraph=nullDummy; // <PARAGRAPH>
    stackItem->stackElementText=nullDummy; // <TEXT>
    stackItem->stackElementFormatsPlural=nullDummy; // <FORMATS>

#else
    stackItem->elementType=ElementTypeIgnore;
#endif
    return true;
}

// Parser for SAX2

bool StructureParser :: startElement( const QString&, const QString&, const QString& name, const QXmlAttributes& attributes)
{
    //Warning: be careful that some element names can be lower case or upper case (not very XML)
    kDebug(30506) << indent << " <" << name << ">" << endl; //DEBUG
    indent += "*"; //DEBUG

    if (structureStack.isEmpty())
    {
        kError(30506) << "Stack is empty!! Aborting! (in StructureParser::startElement)" << endl;
        return false;
    }

    // Create a new stack element copying the top of the stack.
    StackItem *stackItem=new StackItem(*structureStack.current());

    if (!stackItem)
    {
        kError(30506) << "Could not create Stack Item! Aborting! (in StructureParser::startElement)" << endl;
        return false;
    }

    stackItem->itemName=name;

    bool success=false;

    if ((name=="c")||(name=="C"))
    {
        success=StartElementC(stackItem,structureStack.current(),attributes);
    }
    else if ((name=="p")||(name=="P"))
    {
        success=StartElementP(stackItem,structureStack.current(),mainDocument,
            styleDataMap,attributes);
    }
    else if ((name=="section")||(name=="SECTION"))
    {
        success=StartElementSection(stackItem,structureStack.current(),attributes);
    }
    else if (name=="a")
    {
        success=StartElementA(stackItem,structureStack.current(),attributes);
    }
    else if (name=="br") // NOTE: Not sure if it only exists in lower case!
    {
        // We have a forced line break
        StackItem* stackCurrent=structureStack.current();
        success=StartElementBR(stackItem,stackCurrent,mainDocument);
    }
    else if (name=="cbr") // NOTE: Not sure if it only exists in lower case!
    {
        // We have a forced column break (not supported by KWord)
        stackItem->elementType=ElementTypeEmpty;
        StackItem* stackCurrent=structureStack.current();
        if (stackCurrent->elementType==ElementTypeContent)
        {
            kWarning(30506) << "Forced column break found! Transforming to forced page break" << endl;
            success=complexForcedPageBreak(stackItem);
        }
        else if (stackCurrent->elementType==ElementTypeParagraph)
        {
            kWarning(30506) << "Forced column break found! Transforming to forced page break" << endl;
            success=StartElementPBR(stackItem,stackCurrent,mainDocument);
        }
        else
        {
            kError(30506) << "Forced column break found out of turn! Aborting! Parent: "
                << stackCurrent->itemName <<endl;
            success=false;
        }
    }
    else if (name=="pbr") // NOTE: Not sure if it only exists in lower case!
    {
        // We have a forced page break
        stackItem->elementType=ElementTypeEmpty;
        StackItem* stackCurrent=structureStack.current();
        if (stackCurrent->elementType==ElementTypeContent)
        {
            success=complexForcedPageBreak(stackItem);
        }
        else if (stackCurrent->elementType==ElementTypeParagraph)
        {
            success=StartElementPBR(stackItem,stackCurrent,mainDocument);
        }
        else
        {
            kError(30506) << "Forced page break found out of turn! Aborting! Parent: "
                << stackCurrent->itemName <<endl;
            success=false;
        }
    }
    else if (name=="pagesize")
        // Does only exist as lower case tag!
    {
        stackItem->elementType=ElementTypeEmpty;
        stackItem->stackElementText=structureStack.current()->stackElementText; // TODO: reason?
        success=StartElementPageSize(m_paperElement,attributes);
    }
    else if ((name=="field") //TODO: upper-case?
        || (name=="f")) // old deprecated name for <field>
    {
        success=StartElementField(stackItem,structureStack.current(),mainDocument,attributes);
    }
    else if (name=="s") // Seems only to exist as lower case
    {
        success=StartElementS(stackItem,structureStack.current(),attributes,styleDataMap);
    }
    else if ((name=="image") //TODO: upper-case?
        || (name=="i")) // old deprecated name for <image>
    {
        success=StartElementImage(stackItem,structureStack.current(),attributes);
    }
    else if (name=="d") // TODO: upper-case?
    {
        success=StartElementD(stackItem,structureStack.current(),attributes);
    }
    else if (name=="iw") // No upper-case
    {
        stackItem->elementType=ElementTypeIgnoreWord;
        success=true;
    }
    else if (name=="m") // No upper-case
    {
        success=StartElementM(stackItem,structureStack.current(),attributes);
    }
    else if (name=="foot") // No upper-case
    {
        success=StartElementFoot(stackItem,structureStack.current(),attributes);
    }
    else if (name=="table") // No upper-case
    {
        success=StartElementTable(stackItem,structureStack.current(), attributes);
    }
    else if (name=="cell") // No upper-case
    {
        success=StartElementCell(stackItem,structureStack.current(),attributes);
    }
    else
    {
        stackItem->elementType=ElementTypeUnknown;
        stackItem->stackElementText=structureStack.current()->stackElementText; // TODO: reason?
        success=true;
    }
    if (success)
    {
        structureStack.push(stackItem);
    }
    else
    {   // We have a problem so destroy our resources.
        delete stackItem;
    }
    return success;
}

bool StructureParser :: endElement( const QString&, const QString& , const QString& name)
{
    indent.remove( 0, 1 ); // DEBUG
    kDebug(30506) << indent << " </" << name << ">" << endl;

    if (structureStack.isEmpty())
    {
        kError(30506) << "Stack is empty!! Aborting! (in StructureParser::endElement)" << endl;
        return false;
    }

    bool success=false;

    StackItem *stackItem=structureStack.pop();
    if ((name=="c")||(name=="C"))
    {
        success=EndElementC(stackItem,structureStack.current());
    }
    else if ((name=="p")||(name=="P"))
    {
        success=EndElementP(stackItem);
    }
    else if (name=="a")
    {
        if (stackItem->elementType==ElementTypeContent)
        {
            // Anchor to a bookmark (not supported by KWord))
            success=EndElementC(stackItem,structureStack.current());
        }
        else
        {
            // Normal anchor
            success=EndElementA(stackItem,structureStack.current(), mainDocument);
        }
    }
    else if (name=="d")
    {
        success=EndElementD(stackItem);
    }
    else if (name=="iw") // No upper-case
    {
        success=EndElementIW(stackItem,structureStack.current(), mainDocument, m_ignoreWordsElement);
    }
    else if (name=="m") // No upper-case
    {
        success=EndElementM(stackItem);
    }
    else
    {
        success=true; // No problem, so authorisation to continue parsing
    }
    if (!success)
    {
        // If we have no success, then it was surely a tag mismatch. Help debugging!
        kError(30506) << "Found tag name: " << name
            << " expected: " << stackItem->itemName << endl;
    }
    delete stackItem;
    return success;
}

bool StructureParser :: characters ( const QString & ch )
{
    // DEBUG start
    if (ch=="\n")
    {
        kDebug(30506) << indent << " (LINEFEED)" << endl;
    }
    else if (ch.length()> 40)
    {   // 40 characters are enough (especially for image data)
        kDebug(30506) << indent << " :" << ch.left(40) << "..." << endl;
    }
    else
    {
        kDebug(30506) << indent << " :" << ch << ":" << endl;
    }
    // DEBUG end
    if (structureStack.isEmpty())
    {
        kError(30506) << "Stack is empty!! Aborting! (in StructureParser::characters)" << endl;
        return false;
    }

    bool success=false;

    StackItem *stackItem=structureStack.current();

    if ((stackItem->elementType==ElementTypeContent)
        || (stackItem->elementType==ElementTypeAnchorContent))
    { // <c>
        success=charactersElementC(stackItem,mainDocument,ch);
    }
    else if (stackItem->elementType==ElementTypeParagraph)
    { // <p>
        success=charactersElementP(stackItem,mainDocument,ch);
    }
    else if (stackItem->elementType==ElementTypeAnchor)
    { // <a>
        success=charactersElementA(stackItem,ch);
    }
    else if (stackItem->elementType==ElementTypeEmpty)
    {
        success=ch.trimmed().isEmpty();
        if (!success)
        {
            // We have a parsing error, so abort!
            kError(30506) << "Empty element "<< stackItem->itemName
                <<" is not empty! Aborting! (in StructureParser::characters)" << endl;
        }
    }
    else if (stackItem->elementType==ElementTypeRealData)
    {
        success=CharactersElementD(stackItem,mainDocument,ch);
    }
    else if (stackItem->elementType==ElementTypeIgnoreWord)
    {
        stackItem->strTemp2+=ch; // Just collect the data
        success=true;
    }
    else if (stackItem->elementType==ElementTypeRealMetaData)
    {
        success=CharactersElementM(stackItem,ch);
    }
    else
    {
        success=true;
    }

    return success;
}

bool StructureParser::startDocument(void)
{
    indent = QString::null;  //DEBUG
    styleDataMap.defineDefaultStyles();
    return true;
}

void StructureParser::createDocInfo(void)
{
    QDomImplementation implementation;
    QDomDocument doc(implementation.createDocumentType("document-info",
        "-//KDE//DTD document-info 1.2//EN", "http://www.koffice.org/DTD/document-info-1.2.dtd"));

    m_info=doc;

    m_info.appendChild(
        mainDocument.createProcessingInstruction(
        "xml","version=\"1.0\" encoding=\"UTF-8\""));

    QDomElement elementDoc(mainDocument.createElement("document-info"));
    elementDoc.setAttribute("xmlns","http://www.koffice.org/DTD/document-info");
    m_info.appendChild(elementDoc);

    QDomElement about(mainDocument.createElement("about"));
    elementDoc.appendChild(about);

    QDomElement abstract(mainDocument.createElement("abstract"));
    about.appendChild(abstract);
    abstract.appendChild(mainDocument.createTextNode(m_metadataMap["dc.description"]));

    QDomElement title(mainDocument.createElement("title"));
    about.appendChild(title);
    title.appendChild(mainDocument.createTextNode(m_metadataMap["dc.title"]));

    QDomElement keyword(mainDocument.createElement("keyword"));
    about.appendChild(keyword);
    keyword.appendChild(mainDocument.createTextNode(m_metadataMap["abiword.keywords"]));

    QDomElement subject(mainDocument.createElement("subject"));
    about.appendChild(subject);
    subject.appendChild(mainDocument.createTextNode(m_metadataMap["dc.subject"]));
}

bool StructureParser::endDocument(void)
{
    QDomElement stylesPluralElement=mainDocument.createElement("STYLES");
    // insert before <PICTURES>, as <PICTURES> must remain last.
    mainDocument.documentElement().insertBefore(stylesPluralElement,m_picturesElement);

    kDebug(30506) << "###### Start Style List ######" << endl;
    StyleDataMap::ConstIterator it;

    // At first, we put the Normal style
    it=styleDataMap.find("Normal");
    if (it!=styleDataMap.end())
    {
        kDebug(30506) << "\"" << it.key() << "\" => " << it.data().m_props << endl;
        QDomElement styleElement=mainDocument.createElement("STYLE");
        stylesPluralElement.appendChild(styleElement);
        AddStyle(styleElement, it.key(),it.data(),mainDocument);
    }
    else
        kWarning(30506) << "No 'Normal' style" << endl;

    for (it=styleDataMap.begin();it!=styleDataMap.end();++it)
    {
        if (it.key()=="Normal")
            continue;

        kDebug(30506) << "\"" << it.key() << "\" => " << it.data().m_props << endl;

        QDomElement styleElement=mainDocument.createElement("STYLE");
        stylesPluralElement.appendChild(styleElement);

        AddStyle(styleElement, it.key(),it.data(),mainDocument);
    }
    kDebug(30506) << "######  End Style List  ######" << endl;

    createDocInfo();

    return true;
}

bool StructureParser::warning(const QXmlParseException& exception)
{
    kWarning(30506) << "XML parsing warning: line " << exception.lineNumber()
        << " col " << exception.columnNumber() << " message: " << exception.message() << endl;
    return true;
}

bool StructureParser::error(const QXmlParseException& exception)
{
    // A XML error is recoverable, so it is only a KDE warning
    kWarning(30506) << "XML parsing error: line " << exception.lineNumber()
        << " col " << exception.columnNumber() << " message: " << exception.message() << endl;
    return true;
}

bool StructureParser::fatalError (const QXmlParseException& exception)
{
    kError(30506) << "XML parsing fatal error: line " << exception.lineNumber()
        << " col " << exception.columnNumber() << " message: " << exception.message() << endl;
    m_fatalerror=true;
    KMessageBox::error(NULL, i18n("An error has occurred while parsing the AbiWord file.\nAt line: %1, column %2\nError message: %3",exception.lineNumber(),exception.columnNumber(), i18n( "QXml", exception.message() ) ),
        i18n("AbiWord Import Filter"),0);
    return false; // Stop parsing now, we do not need further errors.
}

void StructureParser :: createDocument(void)
{
    QDomImplementation implementation;
    QDomDocument doc(implementation.createDocumentType("DOC",
        "-//KDE//DTD kword 1.2//EN", "http://www.koffice.org/DTD/kword-1.2.dtd"));

    mainDocument=doc;

    mainDocument.appendChild(
        mainDocument.createProcessingInstruction(
        "xml","version=\"1.0\" encoding=\"UTF-8\""));

    QDomElement elementDoc;
    elementDoc=mainDocument.createElement("DOC");
    elementDoc.setAttribute("xmlns","http://www.koffice.org/DTD/kword");
    elementDoc.setAttribute("editor","AbiWord Import Filter");
    elementDoc.setAttribute("mime","application/x-kword");
    elementDoc.setAttribute( "syntaxVersion", 3 );
    mainDocument.appendChild(elementDoc);

    QDomElement element;
    element=mainDocument.createElement("ATTRIBUTES");
    element.setAttribute("processing",0);
    element.setAttribute("standardpage",1);
    element.setAttribute("hasHeader",0);
    element.setAttribute("hasFooter",0);
    //element.setAttribute("unit","mm"); // use KWord default instead
    element.setAttribute("tabStopValue",36); // AbiWord has a default of 0.5 inch tab stops
    elementDoc.appendChild(element);

    // <PAPER> will be partialy changed by an AbiWord <pagesize> element.
    // Default paper format of AbiWord is "Letter"
    m_paperElement=mainDocument.createElement("PAPER");
    m_paperElement.setAttribute("format",PG_US_LETTER);
    m_paperElement.setAttribute("width",MillimetresToPoints(KoPageFormat::width (PG_US_LETTER,PG_PORTRAIT)));
    m_paperElement.setAttribute("height",MillimetresToPoints(KoPageFormat::height(PG_US_LETTER,PG_PORTRAIT)));
    m_paperElement.setAttribute("orientation",PG_PORTRAIT);
    m_paperElement.setAttribute("columns",1);
    m_paperElement.setAttribute("columnspacing",2);
    m_paperElement.setAttribute("hType",0);
    m_paperElement.setAttribute("fType",0);
    m_paperElement.setAttribute("spHeadBody",9);
    m_paperElement.setAttribute("spFootBody",9);
    m_paperElement.setAttribute("zoom",100);
    elementDoc.appendChild(m_paperElement);

    m_paperBordersElement=mainDocument.createElement("PAPERBORDERS");
    m_paperBordersElement.setAttribute("left",28);
    m_paperBordersElement.setAttribute("top",42);
    m_paperBordersElement.setAttribute("right",28);
    m_paperBordersElement.setAttribute("bottom",42);
    m_paperElement.appendChild(m_paperBordersElement);

    framesetsPluralElement=mainDocument.createElement("FRAMESETS");
    mainDocument.documentElement().appendChild(framesetsPluralElement);

    mainFramesetElement=mainDocument.createElement("FRAMESET");
    mainFramesetElement.setAttribute("frameType",1);
    mainFramesetElement.setAttribute("frameInfo",0);
    mainFramesetElement.setAttribute("visible",1);
    mainFramesetElement.setAttribute("name",i18nc("Frameset name","Main Text Frameset"));
    framesetsPluralElement.appendChild(mainFramesetElement);

    QDomElement frameElementOut=mainDocument.createElement("FRAME");
    frameElementOut.setAttribute("left",28);
    frameElementOut.setAttribute("top",42);
    frameElementOut.setAttribute("bottom",566);
    frameElementOut.setAttribute("right",798);
    frameElementOut.setAttribute("runaround",1);
    // TODO: a few attributes are missing
    mainFramesetElement.appendChild(frameElementOut);

    // As we are manipulating the document, create a few particular elements
    m_ignoreWordsElement=mainDocument.createElement("SPELLCHECKIGNORELIST");
    mainDocument.documentElement().appendChild(m_ignoreWordsElement);
    m_picturesElement=mainDocument.createElement("PICTURES");
    mainDocument.documentElement().appendChild(m_picturesElement);
}

bool StructureParser::clearStackUntilParagraph(StackItemStack& auxilaryStack)
{
    for (;;)
    {
        StackItem* item=structureStack.pop();
        switch (item->elementType)
        {
        case ElementTypeContent:
            {
                // Push the item on the auxilary stack
                auxilaryStack.push(item);
                break;
            }
        case ElementTypeParagraph:
            {
                // Push back the item on this stack and then stop loop
                structureStack.push(item);
                return true;
            }
        default:
            {
                // Something has gone wrong!
                kError(30506) << "Cannot clear this element: "
                    << item->itemName << endl;
                return false;
            }
        }
    }
}

ABIWORDImport::ABIWORDImport(QObject* parent, const QStringList &) :
                     KoFilter(parent) {
}

KoFilter::ConversionStatus ABIWORDImport::convert( const QByteArray& from, const QByteArray& to )
{
    if ((to != "application/x-kword") || (from != "application/x-abiword"))
        return KoFilter::NotImplemented;

    kDebug(30506)<<"AbiWord to KWord Import filter"<<endl;

    StructureParser handler(m_chain);

    //We arbitrarily decide that Qt can handle the encoding in which the file was written!!
    QXmlSimpleReader reader;
    reader.setContentHandler( &handler );
    reader.setErrorHandler( &handler );

    //Find the last extension
    QString strExt;
    QString fileIn = m_chain->inputFile();
    const int result=fileIn.findRev('.');
    if (result>=0)
    {
        strExt=fileIn.mid(result);
    }

    kDebug(30506) << "File extension: -" << strExt << "-" << endl;

    QString strMime; // Mime type of the compressor (default: unknown)

    if ((strExt==".gz")||(strExt==".GZ")        //in case of .abw.gz (logical extension)
        ||(strExt==".zabw")||(strExt==".ZABW")) //in case of .zabw (extension used prioritary with AbiWord)
    {
        // Compressed with gzip
        strMime="application/x-gzip";
        kDebug(30506) << "Compression: gzip" << endl;
    }
    else if ((strExt==".bz2")||(strExt==".BZ2") //in case of .abw.bz2 (logical extension)
        ||(strExt==".bzabw")||(strExt==".BZABW")) //in case of .bzabw (extension used prioritary with AbiWord)
    {
        // Compressed with bzip2
        strMime="application/x-bzip2";
        kDebug(30506) << "Compression: bzip2" << endl;
    }

    QIODevice* in = KFilterDev::deviceForFile(fileIn,strMime);

    if ( !in )
    {
        kError(30506) << "Cannot create device for uncompressing! Aborting!" << endl;
        return KoFilter::FileNotFound; // ### TODO: better error?
    }

    if (!in->open(QIODevice::ReadOnly))
    {
        kError(30506) << "Cannot open file for uncompressing! Aborting!" << endl;
        delete in;
        return KoFilter::FileNotFound;
    }

    QXmlInputSource source(in); // Read the file

    in->close();

    if (!reader.parse( source ))
    {
        kError(30506) << "Import: Parsing unsuccessful. Aborting!" << endl;
        delete in;
        if (!handler.wasFatalError())
        {
            // As the parsing was stopped for something else than a fatal error, we have not yet get an error message. (Can it really happen?)
            KMessageBox::error(NULL, i18n("An error occurred during the load of the AbiWord file: %1",QString(from)),
                i18n("AbiWord Import Filter"),0);
        }
        return KoFilter::ParsingError;
    }
    delete in;

    QByteArray strOut;
    KoStoreDevice* out;

    kDebug(30506) << "Creating documentinfo.xml" << endl;
    out=m_chain->storageFile( "documentinfo.xml", KoStore::Write );
    if(!out)
    {
        kError(30506) << "AbiWord Import unable to open output file! (Documentinfo)" << endl;
        KMessageBox::error(NULL, i18n("Unable to save document information."),i18n("AbiWord Import Filter"),0);
        return KoFilter::StorageCreationError;
    }

    //Write the document information!
    strOut=handler.getDocInfo().toByteArray(); // UTF-8
    // WARNING: we cannot use KoStore::write(const QByteArray&) because it writes an extra NULL character at the end.
    out->write(strOut,strOut.length());

    kDebug(30506) << "Creating maindoc.xml" << endl;
    out=m_chain->storageFile( "root", KoStore::Write );
    if(!out)
    {
        kError(30506) << "AbiWord Import unable to open output file! (Root)" << endl;
        KMessageBox::error(NULL, i18n("Unable to save main document."),i18n("AbiWord Import Filter"),0);
        return KoFilter::StorageCreationError;
    }

    //Write the document!
    strOut=handler.getDocument().toByteArray(); // UTF-8
    // WARNING: we cannot use KoStore::write(const QByteArray&) because it writes an extra NULL character at the end.
    out->write(strOut,strOut.length());

#if 0
    kDebug(30506) << documentOut.toString();
#endif

    kDebug(30506) << "Now importing to KWord!" << endl;

    return KoFilter::OK;
}

#include "abiwordimport.moc"
