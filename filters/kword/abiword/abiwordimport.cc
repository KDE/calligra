// $Header$

/* This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <abiwordimport.h>
#include <abiwordimport.moc>
#include <kdebug.h>
#include <qxml.h>
#include <qdom.h>
#include <qstack.h>
#include <ktempfile.h>

#include "processors.h"
#include "kqiodevicegzip.h"

// *Note for the reader of this code*
// Tags in lower case (e.g. <c>) are AbiWord's ones.
// Tags in upper case (e.g. <TEXT>) are KWord's ones.

class AbiProps
{
public:
    AbiProps() {};
    AbiProps(QString newName, void* newValue) : name(newName), value(newValue) {};
    virtual ~AbiProps() {};
    QString name;
    void* value;
};

// Treat the "props" attribute of AbiWord's tags and split it in separates names and values
static void TreatAbiProps(QString strProps,QValueList<AbiProps> &abiPropsList)
{
    if (strProps.isEmpty())
        return;
    
    QString name,value;
    bool notFinished=true;
    int position=0;
    int result;
    while (notFinished)
    {
        //Find next name and its value
        result=strProps.find(':',position);
        if (result==-1)
        {
            name=strProps.mid(position).stripWhiteSpace();
            value="";
            notFinished=false;
        }
        else
        {
            name=strProps.mid(position,result-position).stripWhiteSpace();
            position=result+1;
            result=strProps.find(';',position);
            if (result==-1)
            {
                value=strProps.mid(position).stripWhiteSpace();
                notFinished=false;
            }
            else
            {
                value=strProps.mid(position,result-position).stripWhiteSpace();
                position=result+1;
            }
        }
        kdDebug(30506) << "========== (Property :" << name << "=" << value <<":)"<<endl;
        //Now treat the name and the value that we have just found
        QValueList<AbiProps>::Iterator iterator;
        for (iterator=abiPropsList.begin();iterator!=abiPropsList.end();iterator++)
        {
            if (name==(*iterator).name)
            {
                if((*(iterator)).value)
                {
                    void *pointer=(*(iterator)).value;
                    *((QString*) pointer)=value;
                }
                break;
            }
        }
    }
}

enum StackItemElementType{
    ElementTypeUnknown  = 0,
    ElementTypeBottom,      // Bottom of the stack
    ElementTypeIgnore,      // Element is known but ignored (e.g. empty elements)
    ElementTypeAbiWord,     // <abiword>
    ElementTypeSection,     // <section>
    ElementTypeParagraph,   // <p>
    ElementTypeContent      // <c>
};

class StackItem
{
public:
    StackItem()
    {
        fontName="times"; //Default font
        fontSize=0; //No explicit font size
        italic=false;
        bold=false;
        underline=false;
        strikeout=false;
        red=0;
        green=0;
        blue=0;
        textPosition=0;
    }
    ~StackItem()
    {
    }
public:
    StackItemElementType elementType;
    QDomNode    stackNode,stackNode2;
    QString     fontName;
    int         fontSize;
    int         pos; //Position
    bool        italic;
    bool        bold;
    bool        underline;
    bool        strikeout;
    int         red;
    int         green;
    int         blue;
    int         textPosition; //Normal (0), subscript(1), superscript (2)
};


class StructureParser : public QXmlDefaultHandler
{
public:
    StructureParser(QDomDocument doc) : mainDocument(doc)
    {
        createMainFramesetElement();
        structureStack.setAutoDelete(true);
        StackItem *stackItem=new(StackItem); //TODO: memory failure recovery
        stackItem->elementType=ElementTypeBottom;
        stackItem->stackNode=mainFramesetElement;
        structureStack.push(stackItem); //Security item (not to empty the stack)
    }
    virtual ~StructureParser()
    {
        structureStack.clear();
    }
    bool startDocument()
    {
        indent = "";  //DEBUG
        return true;
    }
    virtual bool startElement( const QString&, const QString&, const QString& name, const QXmlAttributes& attributes);
    virtual bool endElement( const QString&, const QString& , const QString& qName);
    virtual bool characters ( const QString & ch );
private:
    void createMainFramesetElement(void);
    QString indent; //DEBUG
    QStack<StackItem> structureStack;
    QDomDocument mainDocument;
    QDomElement mainFramesetElement;     // The main <FRAMESET> where the body text will be under.
};

// Element <c>

bool StartElementC(StackItem* stackItem, StackItem* stackCurrent, const QXmlAttributes& attributes)
{
    // <c> elements can be nested in <p> elements or in other <c> elements
    // AbiWord does not use it but explicitely allows external programs to write AbiWord files with nested <c> elements!
    if ((stackCurrent->elementType==ElementTypeParagraph)||(stackCurrent->elementType==ElementTypeContent))
    {
        QDomNode nodeOut=stackCurrent->stackNode;
        QDomNode nodeOut2=stackCurrent->stackNode2;
        QValueList<AbiProps> abiPropsList;
        // Initialize the QStrings with the previous values of the properties they represent!
        QString strFontStyle(stackItem->italic?"italic":"");
        QString strWeight(stackItem->bold?"bold":"");
        QString strDecoration(stackItem->underline?"underline":"");
        QString strTextPosition("old");
        QString strColour("old");
        QString strFontSize("old");
        QString strFontFamily(stackItem->fontName);

        abiPropsList.append( AbiProps("font-style",&strFontStyle));
        abiPropsList.append( AbiProps("font-weight",&strWeight));
        abiPropsList.append( AbiProps("text-decoration",&strDecoration));
        abiPropsList.append( AbiProps("text-position",&strTextPosition));
        abiPropsList.append( AbiProps("color",&strColour));
        abiPropsList.append( AbiProps("font-size",&strFontSize));
        abiPropsList.append( AbiProps("font-family",&strFontFamily));
        kdDebug(30506)<< "========== props=\"" << attributes.value("props") << "\"" << endl;
        // Treat the props attributes in the two available flavors: lower case and upper case.
        TreatAbiProps(attributes.value("props"),abiPropsList);
        TreatAbiProps(attributes.value("PROPS"),abiPropsList);
        stackItem->elementType=ElementTypeContent;
        stackItem->stackNode=nodeOut;   // <TEXT>
        stackItem->stackNode2=nodeOut2; // <FORMATS>
        stackItem->pos=stackCurrent->pos; //Propagate the position

        stackItem->italic=(strFontStyle=="italic");
        stackItem->bold=(strWeight=="bold");
        stackItem->underline=(strDecoration=="underline");
        stackItem->strikeout=(strDecoration=="line-through");
        if (strTextPosition=="subscript")
        {
            stackItem->textPosition=1;
        }
        else if (strTextPosition=="superscript")
        {
            stackItem->textPosition=2;
        }
        else if (strTextPosition!="old")
        {
            // we have any other new value, assume it means normal!
            stackItem->textPosition=0;
        }
        if (!strColour.isEmpty() && (strColour!="old"))
        {
            // we have a new colour, so decode it!
            long int colour=strColour.toLong(NULL,16);
            stackItem->red  =(colour&0xFF0000)>>16;
            stackItem->green=(colour&0x00FF00)>>8;
            stackItem->blue =(colour&0x0000FF);
        }
        if (!strFontSize.isEmpty() && (strFontSize!="old"))
        {
            int size=0;
            int ch; // digit value of the character
            for (int pos=0;;pos++)
            {
                ch=strFontSize.at(pos).digitValue();
                if (ch==-1)
                {
                    // Not a digit
                    break;
                }
                else
                {
                    size*=10;
                    size+=ch;
                }
            }
            // TODO: verify that the unit of the font size is really "pt"
            if (size>0)
            {
                stackItem->fontSize=size;
            }
        }
        if (!strFontFamily.isEmpty() && (strFontFamily!="old"))
        {
            // TODO: transform the font-family in a font we have on the system on which KWord runs.
            stackItem->fontName=strFontFamily;
        }
    }
    else
    {//we are not nested correctly, so consider it a parse error!
        kdError(30506) << "Abiword Import: parse error <c> tag not nested in neither a <p>  nor a <c> tag" << endl;
        return false;
    }
	return true;
}

bool charactersElementC (StackItem* stackItem, const QString & ch)
{
    QDomNode nodeOut=stackItem->stackNode;
    QDomNode nodeOut2=stackItem->stackNode2;
    nodeOut.appendChild(nodeOut.ownerDocument().createTextNode(ch));

    QDomElement formatElementOut=nodeOut.ownerDocument().createElement("FORMAT");
    formatElementOut.setAttribute("id",1); // Normal text!
    formatElementOut.setAttribute("pos",stackItem->pos); // Start position
    formatElementOut.setAttribute("len",ch.length()); // Start position
    nodeOut2.appendChild(formatElementOut); //Append to <FORMATS>
    stackItem->pos+=ch.length(); // Adapt new starting position

    //Note: the <FONT> tag is mandatory for KWord (FIXME: not anymore in KWord 0.9)
    QDomElement fontElementOut=nodeOut.ownerDocument().createElement("FONT");
    fontElementOut.setAttribute("name",stackItem->fontName); // Font name
    formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>

    if (stackItem->fontSize)
    {
        QDomElement fontElementOut=nodeOut.ownerDocument().createElement("SIZE");
        fontElementOut.setAttribute("value",stackItem->fontSize);
        formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
    }

    if (stackItem->italic)
    {
        QDomElement fontElementOut=nodeOut.ownerDocument().createElement("ITALIC");
        fontElementOut.setAttribute("value",1);
        formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
    }

    if (stackItem->bold)
    {
        QDomElement fontElementOut=nodeOut.ownerDocument().createElement("WEIGHT");
        fontElementOut.setAttribute("value",75);
        formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
    }

    if (stackItem->underline)
    {
        QDomElement elementOut=nodeOut.ownerDocument().createElement("UNDERLINE");
        elementOut.setAttribute("value",1);
        formatElementOut.appendChild(elementOut); //Append to <FORMAT>
    }

    if (stackItem->strikeout)
    {
        QDomElement elementOut=nodeOut.ownerDocument().createElement("STRIKEOUT");
        elementOut.setAttribute("value",1);
        formatElementOut.appendChild(elementOut); //Append to <FORMAT>
    }

    if (stackItem->textPosition)
    {
        QDomElement fontElementOut=nodeOut.ownerDocument().createElement("VERTALIGN");
        fontElementOut.setAttribute("value",stackItem->textPosition);
        formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
    }

    if (stackItem->red || stackItem->green || stackItem->blue)
    {
        QDomElement fontElementOut=nodeOut.ownerDocument().createElement("COLOR");
        fontElementOut.setAttribute("red",stackItem->red);
        fontElementOut.setAttribute("green",stackItem->green);
        fontElementOut.setAttribute("blue",stackItem->blue);
        formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
    }
	return true;
}

bool EndElementC (StackItem* stackItem, StackItem* stackCurrent)
{
    if (!stackItem->elementType==ElementTypeContent)
    {
        kdError(30506) << "Wrong element type!! Aborting! (</c> in StructureParser::endElement)" << endl;
        return false;
    }
    stackItem->stackNode.toElement().normalize();
    stackCurrent->pos=stackItem->pos; //Propagate the position back to the parent element
    return true;
}

// Element <p>

bool StartElementP(StackItem* stackItem, StackItem* stackCurrent, QDomElement& mainFramesetElement, const QXmlAttributes& attributes)
{
    QDomNode nodeOut=stackCurrent->stackNode;
    //We use mainFramesetElement here not to be dependant that <section> has happened before
    QDomElement paragraphElementOut=mainFramesetElement.ownerDocument().createElement("PARAGRAPH");
    mainFramesetElement.appendChild(paragraphElementOut);
    QDomElement textElementOut=mainFramesetElement.ownerDocument().createElement("TEXT");
    paragraphElementOut.appendChild(textElementOut);
    QDomElement formatsPluralElementOut=mainFramesetElement.ownerDocument().createElement("FORMATS");
    paragraphElementOut.appendChild(formatsPluralElementOut);

    QValueList<AbiProps> abiPropsList;

    QString strFontStyle;
    QString strWeight;
    QString strDecoration;
    QString strTextPosition;
    QString strColour;
    QString strFontSize;
    QString strFontFamily;
    QString strFlow;

    abiPropsList.append( AbiProps("font-style",&strFontStyle));
    abiPropsList.append( AbiProps("font-weight",&strWeight));
    abiPropsList.append( AbiProps("text-decoration",&strDecoration));
    abiPropsList.append( AbiProps("text-position",&strTextPosition));
    abiPropsList.append( AbiProps("color",&strColour));
    abiPropsList.append( AbiProps("font-size",&strFontSize));
    abiPropsList.append( AbiProps("font-family",&strFontFamily));
    abiPropsList.append( AbiProps("text-align",&strFlow));
	
    kdDebug(30506)<< "========== props=\"" << attributes.value("props") << "\"" << endl;
    // Treat the props attributes in the two available flavors: lower case and upper case.
    TreatAbiProps(attributes.value("props"),abiPropsList);
    TreatAbiProps(attributes.value("PROPS"),abiPropsList);


    stackItem->elementType=ElementTypeParagraph;
    stackItem->stackNode=textElementOut; // <TEXT>
    stackItem->stackNode2=formatsPluralElementOut; // <FORMATS>
    stackItem->pos=0; // No text characters yet
    stackItem->italic=(strFontStyle=="italic");
    stackItem->bold=(strWeight=="bold");
    stackItem->underline=(strDecoration=="underline");
	stackItem->strikeout=(strDecoration=="line-through");
    if (strTextPosition=="subscript")
    {
        stackItem->textPosition=1;
    }
    else if (strTextPosition=="superscript")
    {
        stackItem->textPosition=2;
    }
    else
    {
        // we have any other new value, assume it means normal!
        stackItem->textPosition=0;
    }
    if (!strColour.isEmpty())
    {
        // we have a new colour, so decode it!
        long int colour=strColour.toLong(NULL,16);
        stackItem->red  =(colour&0xFF0000)>>16;
        stackItem->green=(colour&0x00FF00)>>8;
        stackItem->blue =(colour&0x0000FF);
    }
    if (!strFontSize.isEmpty())
    {
        int size=0;
        int ch; // digit value of the character
        for (int pos=0;;pos++)
        {
            ch=strFontSize.at(pos).digitValue();
            if (ch==-1)
            {
                // Not a digit
                break;
            }
            else
            {
                size*=10;
                size+=ch;
            }
        }
        // TODO: verify that the unit of the font size is really "pt"
        if (size>0)
        {
            stackItem->fontSize=size;
        }
    }
    if (!strFontFamily.isEmpty())
    {
        // TODO: transform the font-family in a font we have on the system on which KWord runs.
        stackItem->fontName="times";
    }

    // Now we populate the layout
    QDomElement layoutElement=nodeOut.ownerDocument().createElement("LAYOUT");
    paragraphElementOut.appendChild(layoutElement);

    QDomElement element;
    element=layoutElement.ownerDocument().createElement("NAME");
    element.setAttribute("value","Standard");
    layoutElement.appendChild(element);

    element=layoutElement.ownerDocument().createElement("FOLLOWING");
    element.setAttribute("value","Standard");
    layoutElement.appendChild(element);

    element=layoutElement.ownerDocument().createElement("FLOW");
    if ((strFlow=="left") || (strFlow=="center") || (strFlow=="right") || (strFlow=="justify"))
    {
        element.setAttribute("align",strFlow);
    }
    else
    {
        element.setAttribute("align","left");
    }
    layoutElement.appendChild(element);

    element=layoutElement.ownerDocument().createElement("TABULATOR");
    element.setAttribute("mmpos","64.2055");
    element.setAttribute("ptpos","182");
    element.setAttribute("inchpos","2.52778");
    layoutElement.appendChild(element);

    element=layoutElement.ownerDocument().createElement("TABULATOR");
    element.setAttribute("mmpos","128.764");
    element.setAttribute("ptpos","365");
    element.setAttribute("inchpos","5.06944");
    layoutElement.appendChild(element);

    QDomElement formatElementOut=layoutElement.ownerDocument().createElement("FORMAT");
    layoutElement.appendChild(formatElementOut);

    //Note: the <FONT> tag is mandatory for KWord (FIXME: not anymore in KWord 0.9)
    QDomElement fontElementOut=formatElementOut.ownerDocument().createElement("FONT");
    fontElementOut.setAttribute("name",stackItem->fontName); // Font name
    formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>

    if (stackItem->fontSize)
    {
        QDomElement fontElementOut=formatElementOut.ownerDocument().createElement("SIZE");
        fontElementOut.setAttribute("value",stackItem->fontSize);
        formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
    }

    if (stackItem->italic)
    {
        element=formatElementOut.ownerDocument().createElement("ITALIC");
        element.setAttribute("value",1);
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->bold)
    {
        element=formatElementOut.ownerDocument().createElement("WEIGHT");
        element.setAttribute("value",75);
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->underline)
    {
        element=formatElementOut.ownerDocument().createElement("UNDERLINE");
        element.setAttribute("value",1);
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->strikeout)
    {
        element=formatElementOut.ownerDocument().createElement("STRIKEOUT");
        element.setAttribute("value",1);
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->textPosition)
    {
        element=formatElementOut.ownerDocument().createElement("VERTALIGN");
        element.setAttribute("value",stackItem->textPosition);
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->red || stackItem->green || stackItem->blue)
    {
        element=formatElementOut.ownerDocument().createElement("COLOR");
        element.setAttribute("red",stackItem->red);
        element.setAttribute("green",stackItem->green);
        element.setAttribute("blue",stackItem->blue);
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    return true;
}

bool charactersElementP (StackItem* stackItem, const QString & ch)
{
    QDomNode nodeOut=stackItem->stackNode;
    QDomNode nodeOut2=stackItem->stackNode2;
    nodeOut.appendChild(nodeOut.ownerDocument().createTextNode(ch));

    stackItem->pos+=ch.length(); // Adapt new starting position

    return true;
}

bool EndElementP (StackItem* stackItem)
{
    if (!stackItem->elementType==ElementTypeParagraph)
    {
        kdError(30506) << "Wrong element type!! Aborting! (</p> in StructureParser::endElement)" << endl;
        return false;
    }
    stackItem->stackNode.toElement().normalize();
    return true;
}

// <pagesize>

inline double MillimetersToPoints(const double d)
{
    return d * 72.0 / 2.54;
}

inline double InchesToPoints(const double d)
{
    return d * 72.0;
}

static bool StartElementPageSize(QDomDocument& mainDocument, const QXmlAttributes& attributes)
{
    if (attributes.value("page-scale").toDouble()!=1.0)
    {
        kdWarning(30506) << "Ignoring unsupported page scale: " << attributes.value("page-scale") << endl;
    }

    int kwordOrientation;
    QString strOrientation=attributes.value("orientation").stripWhiteSpace();

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
        kdWarning(30506) << "Unknown page orientation: " << strOrientation << "! Ignoring! " << endl;
        kwordOrientation=0;
    }

    double kwordHeight=0.0;
    double kwordWidth=0.0;
    int kwordFormat;

    QString strPageType=attributes.value("pagetype").stripWhiteSpace();

    // Do we know the page size or do we need to measure
    // For page format we know, use our own values in case the values in the file would be wrong.

    if (strPageType=="Custom")
    {
        // We have a Custom page.
        // For now (CVS 2001-04-24), AbiWord cannot handle custom pages
        //  and the height and width value are null, therefore useless.
        // We prefer to set a A4 page size!
        kwordHeight = MillimetersToPoints(29.7);
        kwordWidth  = MillimetersToPoints(21.0);
        kwordFormat = 6; // At least, say that we are a custom format!
        kdWarning(30506) << "Custom page format found! Ignored!" << endl;
    }
    // The two most used formats first: A4 and Letter
    else if (strPageType=="A4")
    {
        kwordHeight = MillimetersToPoints(29.7);
        kwordWidth  = MillimetersToPoints(21.0);
        kwordFormat = 1; // A4
    }
    else if (strPageType=="Letter")
    {
        kwordHeight = InchesToPoints(11.0);
        kwordWidth  = InchesToPoints( 8.5);
        kwordFormat = 3; // US Letter
    }
    // TODO: more formats that KWord knows
    else
        // We do not know the format, use the values in the attributes
    {
        kdWarning(30506) << "Unknown page format found: " << strPageType << endl;
        // TODO: do a real implementation, for now it is only A4
        kwordHeight = MillimetersToPoints(29.7);
        kwordWidth  = MillimetersToPoints(21.0);
        kwordFormat = 6; // At least, say that we are a custom format!
    }


    // Now that we have gathered all the data, put it in the right element!

    QDomNodeList nodeList=mainDocument.elementsByTagName("PAPER");

    if (!nodeList.count())
    {
        kdError(30506) << "Panic: no <PAPER> element was found! Aborting!" << endl;
        return false;
    }

    QDomElement paperElement=nodeList.item(0).toElement();

    if (paperElement.isNull())
    {
        kdError(30506) << "Panic: <PAPER> element cannot be accessed! Aborting!" << endl;
        return false;
    }

    paperElement.setAttribute("format",kwordFormat);
    paperElement.setAttribute("width",kwordWidth);
    paperElement.setAttribute("height",kwordHeight);
    paperElement.setAttribute("orientation",kwordOrientation);

    return true;
}

// Parser for SAX2

bool StructureParser :: startElement( const QString&, const QString&, const QString& name, const QXmlAttributes& attributes)
{
    //Warning: be careful that the element names can be lower case or upper case (not very XML)
    kdDebug(30506) << indent << " <" << name << ">" << endl; //DEBUG
    indent += "*"; //DEBUG

    if (structureStack.isEmpty())
    {
        kdError(30506) << "Stack is empty!! Aborting! (in StructureParser::startElement)" << endl;
        return false;
    }

    // Create a new stack element copying the top of the stack.
    StackItem *stackItem=new StackItem(*structureStack.current());

    //TODO: memory failure recovery

    bool success=false;

    if ((name=="c")||(name=="C"))
    {
        success=StartElementC(stackItem,structureStack.current(),attributes);
    }
    else if ((name=="p")||(name=="P"))
    {
        success=StartElementP(stackItem,structureStack.current(),mainFramesetElement,attributes);
    }
    else if ((name=="section")||(name=="SECTION"))
    {//Not really needed, as it is the default behaviour for now!
        //TODO: non main text sections (e.g. footers)
        stackItem->elementType=ElementTypeSection;
        stackItem->stackNode=structureStack.current()->stackNode;
        success=true;
    }
    else if (name=="pagesize")
        // Does only exists as lower case tag!
    {
        stackItem->elementType=ElementTypeIgnore; // It is in fact an empty element!
        stackItem->stackNode=structureStack.current()->stackNode;
        success=StartElementPageSize(mainDocument,attributes);
    }
    else
    {
        stackItem->elementType=ElementTypeUnknown;
        stackItem->stackNode=structureStack.current()->stackNode;
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
    kdDebug(30506) << indent << " </" << name << ">" << endl;

    if (structureStack.isEmpty())
    {
        kdError(30506) << "Stack is empty!! Aborting! (in StructureParser::endElement)" << endl;
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
    else
    {
        success=true; // No problem, so authorisation to continue parsing
    }
    // Do nothing yet
    delete stackItem;
    return success;
}

bool StructureParser :: characters ( const QString & ch )
{
    // DEBUG start
    if (ch=="\n")
    {
        kdDebug(30506) << indent << " (LINEFEED)" << endl;
    }
    else
    {
        kdDebug(30506) << indent << " :" << ch << ":" << endl;
    }
    // DEBUG end
    if (structureStack.isEmpty())
    {
        kdError(30506) << "Stack is empty!! Aborting! (in StructureParser::characters)" << endl;
        return false;
    }

    bool success=false;

    StackItem *stackItem=structureStack.current();

    if (stackItem->elementType==ElementTypeContent)
    { // <c>
        success=charactersElementC(stackItem,ch);
    }
    else if (stackItem->elementType==ElementTypeParagraph)
    { // <p>
        success=charactersElementC(stackItem,ch);
    }
    else
    {
        success=true;
    }

    return success;
}

void StructureParser :: createMainFramesetElement(void)
{
    QDomElement framesetsPluralElementOut=mainDocument.createElement("FRAMESETS");
    mainDocument.documentElement().appendChild(framesetsPluralElementOut);

    //As we have a new AbiWord <section>, we think we have a KWord <FRAMESET>
    mainFramesetElement=mainDocument.createElement("FRAMESET");
    mainFramesetElement.setAttribute("frameType",1);
    mainFramesetElement.setAttribute("frameInfo",0);
    mainFramesetElement.setAttribute("autoCreateNewFrame",1);
    mainFramesetElement.setAttribute("removable",0);
    //Todo?  attribute "name"
    framesetsPluralElementOut.appendChild(mainFramesetElement);

    QDomElement frameElementOut=mainDocument.createElement("FRAME");
    frameElementOut.setAttribute("left",28);
    frameElementOut.setAttribute("top",42);
    frameElementOut.setAttribute("bottom",566);
    frameElementOut.setAttribute("right",798);
    frameElementOut.setAttribute("runaround",1);
    mainFramesetElement.appendChild(frameElementOut);

}

ABIWORDImport::ABIWORDImport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) {
}

bool ABIWORDImport::filter(const QString &fileIn, const QString &fileOut,
                               const QString& from, const QString& to,
                               const QString &)
{
    if ((to != "application/x-kword") || (from != "application/x-abiword"))
        return false;

    kdDebug(30506)<<"AbiWord to KWord Import filter"<<endl;

    //At first, find the last extension
    QString strExt;
    const int result=fileIn.findRev('.');
    if (result>=0)
    {
        strExt=fileIn.mid(result);
    }

    kdDebug(30506) << "File extension: -" << strExt << "-" << endl;

    //Initiate QDomDocument (TODO: is there are better way?)
    QString strHeader("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    strHeader+="<DOC editor=\"KWord\" mime=\"application/x-kword\" syntaxVersion=\"1\" >\n";
    strHeader+="<ATTRIBUTES processing=\"0\" standardpage=\"1\" hasHeader=\"0\" hasFooter=\"0\" unit=\"mm\" />\n";

    // <PAPER> will be partialy changed by an AbiWord <pagesize> element.
    strHeader+="<PAPER format=\"1\" width=\"595\" height=\"841\" orientation=\"0\" columns=\"1\" columnspacing=\"2\"";
    strHeader+="hType=\"0\" fType=\"0\" spHeadBody=\"9\" spFootBody=\"9\" zoom=\"100\">\n";

    strHeader+="<PAPERBORDERS left=\"28\" top=\"42\" right=\"28\" bottom=\"42\" />\n";
    strHeader+="</PAPER>\n";
    strHeader+="</DOC>\n";

    QDomDocument qDomDocumentOut(fileOut);
    qDomDocumentOut.setContent(strHeader);

    StructureParser handler(qDomDocumentOut);

    //For now, we arbitrarily decide that Qt can handle the encoding in which the file was written!!
    QXmlSimpleReader reader;
    reader.setContentHandler( &handler );

    if ((strExt==".gz")||(strExt==".GZ")        //in case of .abw.gz (logical extension)
        ||(strExt==".zabw")||(strExt==".ZABW")) //in case of .zabw (extension used prioritary with AbiWord)
    {   //The input file is compressed
        KQIODeviceGZip in(fileIn);
        if (!in.open(IO_ReadOnly))
        {
            kdError(30506) << "Cannot open KQIODeviceGzip. Aborting!" << endl;
            return false;
        }
        QTextStream inStream(&in);
        QXmlInputSource source(inStream);
        in.close();
        kdDebug(30506) << source.data() << endl;
        if (!reader.parse( source ))
        {
            kdError(30506) << "Import (GZIP): Parsing unsuccessful. Aborting!" << endl;
            return false;
        }
    }
    else
    {
        // The input file is now uncompressed, so we may handle it directly
        QFile in(fileIn);
        QXmlInputSource source(in);
        if (!reader.parse( source ))
        {
            kdError(30506) << "Import (UNCOMPRESSED): Parsing unsuccessful. Aborting!" << endl;
            return false;
        }
    }

    KoStore out=KoStore(fileOut, KoStore::Write);
    if(!out.open("root"))
    {
        kdError(30506) << "AbiWord Import unable to open output file!" << endl;
        out.close();
        return false;
    }

    //Write the document!
    QCString strOut=qDomDocumentOut.toCString();
    out.write((const char*)strOut, strOut.length());
    out.close();

#if 0
    kdDebug(30506) << qDomDocumentOut.toString();
#endif

    kdDebug(30506) << "Now importing to KWord!" << endl;

    return true;
}
