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

#include <qstack.h>
#include <qmap.h>
#include <qxml.h>
#include <qdom.h>

#include <kdebug.h>

#include <koGlobal.h>

#include "processors.h"
#include "kqiodevicegzip.h"
#include <abiwordimport.h>
#include <abiwordimport.moc>

// *Note for the reader of this code*
// Tags in lower case (e.g. <c>) are AbiWord's ones.
// Tags in upper case (e.g. <TEXT>) are KWord's ones.

class AbiProps
{
public:
    AbiProps() {};
    AbiProps(QString newValue) : m_value(newValue) {};
    virtual ~AbiProps() {};
public:
    inline QString getValue(void) const { return m_value; }
private:
    QString m_value;
};

class AbiPropsMap : public QMap<QString,AbiProps>
{
public:
    AbiPropsMap() {};
    virtual ~AbiPropsMap() {};
public:
    bool setProperty(QString newName, QString newValue);
};

bool AbiPropsMap::setProperty(QString newName, QString newValue)
{
    replace(newName,AbiProps(newValue));
}

// Treat the "props" attribute of AbiWord's tags and split it in separates names and values
static void TreatAbiProps(QString strProps, AbiPropsMap &abiPropsMap)
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
        // Now set the property
        abiPropsMap.setProperty(name,value);
    }
}

enum StackItemElementType{
    ElementTypeUnknown  = 0,
    ElementTypeBottom,      // Bottom of the stack
    ElementTypeIgnore,      // Element is known but ignored
    ElementTypeEmpty,       // Element is empty
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
    QDomElement stackElementText,stackElementFormatsPlural;
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
        stackItem->stackElementText=mainFramesetElement;
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

void PopulateProperties(StackItem* stackItem, 
                        const QXmlAttributes& attributes,
                        AbiPropsMap& abiPropsMap, const bool allowInit)
// TODO: find a better name for this function
{
    if (allowInit)
    {
        // Initialize the QStrings with the previous values of the properties
        if (stackItem->italic)
        {
            abiPropsMap.setProperty("font-style","italic");
        }
        if (stackItem->bold)
        {
            abiPropsMap.setProperty("font-weight","bold");
        }

        if (stackItem->underline)
        {
            abiPropsMap.setProperty("text-decoration","underline");
        }
        else if (stackItem->strikeout)
        {
            abiPropsMap.setProperty("text-decoration","line-through");
        }
    }

    kdDebug(30506)<< "========== props=\"" << attributes.value("props") << "\"" << endl;
    // Treat the props attributes in the two available flavors: lower case and upper case.
    TreatAbiProps(attributes.value("props"),abiPropsMap);
    TreatAbiProps(attributes.value("PROPS"),abiPropsMap);

    stackItem->italic=(abiPropsMap["font-style"].getValue()=="italic");
    stackItem->bold=(abiPropsMap["font-weight"].getValue()=="bold");

    QString strDecoration=abiPropsMap["text-decoration"].getValue();
    stackItem->underline=(strDecoration=="underline");
    stackItem->strikeout=(strDecoration=="line-through");

    QString strTextPosition=abiPropsMap["text-position"].getValue();
    if (strTextPosition=="subscript")
    {
        stackItem->textPosition=1;
    }
    else if (strTextPosition=="superscript")
    {
        stackItem->textPosition=2;
    }
    else if (!strTextPosition.isEmpty())
    {
        // we have any other new value, assume it means normal!
        stackItem->textPosition=0;
    }

    QString strColour=abiPropsMap["color"].getValue();
    if (!strColour.isEmpty())
    {
        // we have a new colour, so decode it!
        long int colour=strColour.toLong(NULL,16);
        stackItem->red  =(colour&0xFF0000)>>16;
        stackItem->green=(colour&0x00FF00)>>8;
        stackItem->blue =(colour&0x0000FF);
    }

    QString strFontSize=abiPropsMap["font-size"].getValue();
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

    QString strFontFamily=abiPropsMap["font-family"].getValue();
    if (!strFontFamily.isEmpty())
    {
        // TODO: transform the font-family in a font we have on the system on which KWord runs.
        stackItem->fontName=strFontFamily;
    }
}

// Element <c>

bool StartElementC(StackItem* stackItem, StackItem* stackCurrent, const QXmlAttributes& attributes)
{
    // <c> elements can be nested in <p> elements or in other <c> elements
    // AbiWord does not use it but explicitely allows external programs to write AbiWord files with nested <c> elements!
    if ((stackCurrent->elementType==ElementTypeParagraph)||(stackCurrent->elementType==ElementTypeContent))
    {

        AbiPropsMap abiPropsMap;
        PopulateProperties(stackItem,attributes,abiPropsMap,true);

        stackItem->elementType=ElementTypeContent;
        stackItem->stackElementText=stackCurrent->stackElementText;   // <TEXT>
        stackItem->stackElementFormatsPlural=stackCurrent->stackElementFormatsPlural; // <FORMATS>
        stackItem->pos=stackCurrent->pos; //Propagate the position
    }
    else
    {//we are not nested correctly, so consider it a parse error!
        kdError(30506) << "Abiword Import: parse error <c> tag not nested in neither a <p>  nor a <c> tag" << endl;
        return false;
    }
    return true;
}

bool charactersElementC (StackItem* stackItem, QDomDocument& mainDocument, const QString & ch)
{
    QDomElement elementText=stackItem->stackElementText;
    QDomElement elementFormatsPlural=stackItem->stackElementFormatsPlural;
    elementText.appendChild(mainDocument.createTextNode(ch));

    QDomElement formatElementOut=mainDocument.createElement("FORMAT");
    formatElementOut.setAttribute("id",1); // Normal text!
    formatElementOut.setAttribute("pos",stackItem->pos); // Start position
    formatElementOut.setAttribute("len",ch.length()); // Start position
    elementFormatsPlural.appendChild(formatElementOut); //Append to <FORMATS>
    stackItem->pos+=ch.length(); // Adapt new starting position

    if (!stackItem->fontName.isEmpty())
    {
        QDomElement fontElementOut=mainDocument.createElement("FONT");
        fontElementOut.setAttribute("name",stackItem->fontName); // Font name
        formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
    }

    if (stackItem->fontSize)
    {
        QDomElement fontElementOut=mainDocument.createElement("SIZE");
        fontElementOut.setAttribute("value",stackItem->fontSize);
        formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
    }

    if (stackItem->italic)
    {
        QDomElement fontElementOut=mainDocument.createElement("ITALIC");
        fontElementOut.setAttribute("value",1);
        formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
    }

    if (stackItem->bold)
    {
        QDomElement fontElementOut=mainDocument.createElement("WEIGHT");
        fontElementOut.setAttribute("value",75);
        formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
    }

    if (stackItem->underline)
    {
        QDomElement elementOut=mainDocument.createElement("UNDERLINE");
        elementOut.setAttribute("value",1);
        formatElementOut.appendChild(elementOut); //Append to <FORMAT>
    }

    if (stackItem->strikeout)
    {
        QDomElement elementOut=mainDocument.createElement("STRIKEOUT");
        elementOut.setAttribute("value",1);
        formatElementOut.appendChild(elementOut); //Append to <FORMAT>
    }

    if (stackItem->textPosition)
    {
        QDomElement fontElementOut=mainDocument.createElement("VERTALIGN");
        fontElementOut.setAttribute("value",stackItem->textPosition);
        formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
    }

    if (stackItem->red || stackItem->green || stackItem->blue)
    {
        QDomElement fontElementOut=mainDocument.createElement("COLOR");
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
    stackItem->stackElementText.normalize();
    stackCurrent->pos=stackItem->pos; //Propagate the position back to the parent element
    return true;
}

// Element <p>

bool StartElementP(StackItem* stackItem, StackItem* stackCurrent, QDomDocument& mainDocument,
 QDomElement& mainFramesetElement, const QXmlAttributes& attributes)
{
    QDomElement elementText=stackCurrent->stackElementText;
    //We use mainFramesetElement here not to be dependant that <section> has happened before
    QDomElement paragraphElementOut=mainDocument.createElement("PARAGRAPH");
    mainFramesetElement.appendChild(paragraphElementOut);
    QDomElement textElementOut=mainDocument.createElement("TEXT");
    paragraphElementOut.appendChild(textElementOut);
    QDomElement formatsPluralElementOut=mainDocument.createElement("FORMATS");
    paragraphElementOut.appendChild(formatsPluralElementOut);

    AbiPropsMap abiPropsMap;
    PopulateProperties(stackItem,attributes,abiPropsMap,false);

    stackItem->elementType=ElementTypeParagraph;
    stackItem->stackElementText=textElementOut; // <TEXT>
    stackItem->stackElementFormatsPlural=formatsPluralElementOut; // <FORMATS>
    stackItem->pos=0; // No text characters yet

    // Now we populate the layout
    QDomElement layoutElement=mainDocument.createElement("LAYOUT");
    paragraphElementOut.appendChild(layoutElement);

    QDomElement element;
    element=mainDocument.createElement("NAME");
    element.setAttribute("value","Standard");
    layoutElement.appendChild(element);

    element=mainDocument.createElement("FOLLOWING");
    element.setAttribute("value","Standard");
    layoutElement.appendChild(element);

    QString strFlow=abiPropsMap["text-align"].getValue();
    element=mainDocument.createElement("FLOW");
    if ((strFlow=="left") || (strFlow=="center") || (strFlow=="right") || (strFlow=="justify"))
    {
        element.setAttribute("align",strFlow);
    }
    else
    {
        element.setAttribute("align","left");
    }
    layoutElement.appendChild(element);

    QDomElement formatElementOut=mainDocument.createElement("FORMAT");
    layoutElement.appendChild(formatElementOut);

    if (!stackItem->fontName.isEmpty())
    {
        QDomElement fontElementOut=mainDocument.createElement("FONT");
        fontElementOut.setAttribute("name",stackItem->fontName); // Font name
        formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
    }

    if (stackItem->fontSize)
    {
        QDomElement fontElementOut=mainDocument.createElement("SIZE");
        fontElementOut.setAttribute("value",stackItem->fontSize);
        formatElementOut.appendChild(fontElementOut); //Append to <FORMAT>
    }

    if (stackItem->italic)
    {
        element=mainDocument.createElement("ITALIC");
        element.setAttribute("value",1);
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->bold)
    {
        element=mainDocument.createElement("WEIGHT");
        element.setAttribute("value",75);
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->underline)
    {
        element=mainDocument.createElement("UNDERLINE");
        element.setAttribute("value",1);
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->strikeout)
    {
        element=mainDocument.createElement("STRIKEOUT");
        element.setAttribute("value",1);
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->textPosition)
    {
        element=mainDocument.createElement("VERTALIGN");
        element.setAttribute("value",stackItem->textPosition);
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->red || stackItem->green || stackItem->blue)
    {
        element=mainDocument.createElement("COLOR");
        element.setAttribute("red",stackItem->red);
        element.setAttribute("green",stackItem->green);
        element.setAttribute("blue",stackItem->blue);
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

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
        kdError(30506) << "Wrong element type!! Aborting! (in endElementP)" << endl;
        return false;
    }
    stackItem->stackElementText.normalize();
    return true;
}

// <br> (forced line break)
static bool StartElementBR(StackItem* stackItem, StackItem* stackCurrent,
                           QDomDocument& mainDocument,
                           QDomElement& mainFramesetElement)
{
    // We are simulating a line break by starting a new paragraph!
    // TODO: when KWord has learnt what line breaks are, change to them.

    if (stackCurrent->elementType==ElementTypeContent)
    {
        // TODO: this can happen in an AbiWord file, so we must support it!
        kdWarning(30506) << "Forced line break in <c> element! Ignoring! (Not supported)" <<endl;
        return true; // It is only a warning, so continue parsing!
    }
    if (stackCurrent->elementType!=ElementTypeParagraph)
    {
        kdError(30506) << "Forced line break found out of turn! Aborting! (in StartElementPBR)" <<endl;
        return false;
    }
    // Now we are sure to be the child of a <p> element

    // The following code is similar to StartElementP
    // We use mainFramesetElement here not to be dependant that <section> has happened before
    QDomElement paragraphElementOut=mainDocument.createElement("PARAGRAPH");
    mainFramesetElement.appendChild(paragraphElementOut);
    QDomElement textElementOut=mainDocument.createElement("TEXT");
    paragraphElementOut.appendChild(textElementOut);
    QDomElement formatsPluralElementOut=mainDocument.createElement("FORMATS");
    paragraphElementOut.appendChild(formatsPluralElementOut);

    // We must now copy/clone the layout of elementText.

    QDomNodeList nodeList=stackCurrent->stackElementText.elementsByTagName("LAYOUT");

    if (!nodeList.count())
    {
        kdError(30506) << "Unable to find <LAYOUT> element! Aborting! (in StartElementBR)" <<endl;
        return false;
    }

    // Now clone it
    QDomNode newNode=nodeList.item(0).cloneNode(true); // We make a deep cloning of the first element/node
    if (newNode.isNull())
    {
        kdError(30506) << "Unable to clone <LAYOUT> element! Aborting! (in StartElementBR)" <<endl;
        return false;
    }
    paragraphElementOut.appendChild(newNode);

    // NOTE: The following code is similar to StartElementP but we are working on stackCurrent!
    // Now that we have done with the old paragraph,
    //  we can write stackCurrent with the data of the new one!
    stackCurrent->elementType=ElementTypeParagraph;
    stackCurrent->stackElementText=textElementOut; // <TEXT>
    stackCurrent->stackElementFormatsPlural=formatsPluralElementOut; // <FORMATS>
    stackCurrent->pos=0; // No text characters yet

    return true;
}

// <pagesize>

static inline double CentimetresToPoints(const double d)
{
    return d * 72.0 / 2.54;
}

static inline double MillimetresToPoints(const double d)
{
    return d * 72.0 / 25.4;
}

static inline double InchesToPoints(const double d)
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

    double kwordHeight=0.0; // Security value!
    double kwordWidth=0.0;  // Security value!
    int kwordFormat;

    QString strPageType=attributes.value("pagetype").stripWhiteSpace();

    // Do we know the page size or do we need to measure
    // For page format that KWord knows, use our own values in case the values in the file would be wrong.

    // At first, the two of the most used formats
    if (strPageType=="A4")
    {
        kwordHeight = CentimetresToPoints(29.7);
        kwordWidth  = CentimetresToPoints(21.0);
        kwordFormat = PG_DIN_A4; // ISO A4
    }
    else if (strPageType=="Letter")
    {
        kwordHeight = InchesToPoints(11.0);
        kwordWidth  = InchesToPoints( 8.5);
        kwordFormat = PG_US_LETTER; // US Letter
    }
    // Other ISO formats that KWord and AbiWord know
    else if (strPageType=="A0")
    {
        kwordHeight = CentimetresToPoints(118.0);
        kwordWidth  = CentimetresToPoints(84.1);
        kwordFormat = PG_DIN_A0; // ISO A0
    }
    else if (strPageType=="A1")
    {
        kwordHeight = CentimetresToPoints(84.1);
        kwordWidth  = CentimetresToPoints(59.4);
        kwordFormat = PG_DIN_A1; // ISO A1
    }
    else if (strPageType=="A2")
    {
        kwordHeight = CentimetresToPoints(59.4);
        kwordWidth  = CentimetresToPoints(42.0);
        kwordFormat = PG_DIN_A2; // ISO A2
    }
    else if (strPageType=="A3")
    {
        kwordHeight = CentimetresToPoints(42.0);
        kwordWidth  = CentimetresToPoints(29.7);
        kwordFormat = PG_DIN_A3; // ISO A3
    }
    else if (strPageType=="A5")
    {
        kwordHeight = CentimetresToPoints(21.0);
        kwordWidth  = CentimetresToPoints(14.8);
        kwordFormat = PG_DIN_A5; // ISO A5
    }
    else if (strPageType=="A6")
    {
        kwordHeight = CentimetresToPoints(14.8);
        kwordWidth  = CentimetresToPoints(10.5);
        kwordFormat = PG_DIN_A6; // ISO A6
    }
    else if (strPageType=="B0")
    {
        kwordHeight = CentimetresToPoints(141.0);
        kwordWidth  = CentimetresToPoints(100.0);
        kwordFormat = PG_DIN_B0; // ISO B0
    }
    else if (strPageType=="B1")
    {
        kwordHeight = CentimetresToPoints(100.0);
        kwordWidth  = CentimetresToPoints(70.7);
        kwordFormat = PG_DIN_B1; // ISO B1
    }
    else if (strPageType=="B2")
    {
        kwordHeight = CentimetresToPoints(70.7);
        kwordWidth  = CentimetresToPoints(50.0);
        kwordFormat = PG_DIN_B2; // ISO B2
    }
    else if (strPageType=="B3")
    {
        kwordHeight = CentimetresToPoints(50.0);
        kwordWidth  = CentimetresToPoints(35.3);
        kwordFormat = PG_DIN_B3; // ISO B3
    }
    else if (strPageType=="B4")
    {
        kwordHeight = CentimetresToPoints(35.3);
        kwordWidth  = CentimetresToPoints(25.8);
        kwordFormat = PG_DIN_B4; // ISO B4
    }
    else if (strPageType=="B5")
    {
        kwordHeight = CentimetresToPoints(25.0);
        kwordWidth  = CentimetresToPoints(17.6);
        kwordFormat = PG_DIN_B5; // ISO B5
    }
    else if (strPageType=="B6")
    {
        kwordHeight = CentimetresToPoints(17.6);
        kwordWidth  = CentimetresToPoints(12.5);
        kwordFormat = PG_DIN_B6; // ISO B6
    }
    // Other American format that KWord and AbiWord know
    else if (strPageType=="Legal")
    {
        kwordHeight = InchesToPoints(14.0);
        kwordWidth  = InchesToPoints( 8.5);
        kwordFormat = PG_US_LEGAL; // US Legal
    }
    else
        // We do not know the format, so we must use the values in the attributes
    {
        kdDebug(30506) << "Other page format found: " << strPageType << endl;

        double height = attributes.value("height").toDouble();
        double width  = attributes.value("width" ).toDouble();

        QString strUnits = attributes.value("units").stripWhiteSpace();

        kdDebug(30506) << "Explicit page size: "
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
            kdWarning(30506) << "Unknown unit type: " << strUnits << endl;
        }

        if ((kwordHeight <= 1.0) || (kwordWidth <=1.0))
            // At least one of the two values is ridiculous
        {
            kdWarning(30506) << "Page width or height is too small: "
             << kwordHeight << "x" << kwordWidth << endl;
            // As we have no correct page size, we assume we have A4
            kwordHeight = CentimetresToPoints(29.7);
            kwordWidth  = CentimetresToPoints(21.0);
        }

        kwordFormat = PG_CUSTOM; // Say that we are a custom format!
    }


    // Now that we have gathered all the page size data, put it in the right element!

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

    if (!stackItem)
    {
        kdError(30506) << "Could not create Stack Item! Aborting! (in StructureParser::startElement)" << endl;
        return false;
    }

    bool success=false;

    if ((name=="c")||(name=="C"))
    {
        success=StartElementC(stackItem,structureStack.current(),attributes);
    }
    else if ((name=="p")||(name=="P"))
    {
        success=StartElementP(stackItem,structureStack.current(),mainDocument,mainFramesetElement,attributes);
    }
    else if ((name=="section")||(name=="SECTION"))
    {//Not really needed, as it is the default behaviour for now!
        //TODO: non main text sections (e.g. footers)
        stackItem->elementType=ElementTypeSection;
        stackItem->stackElementText=structureStack.current()->stackElementText;
        success=true;
    }
    else if (name=="br") // NOTE: Not sure if it only exists in lower case!
    {
        // We have a forced line break
        // NOTE: this is an empty element!
        stackItem->elementType=ElementTypeEmpty;
        stackItem->stackElementText=structureStack.current()->stackElementText;
        success=StartElementBR(stackItem,structureStack.current(),mainDocument,mainFramesetElement);
    }
    else if (name=="pagesize")
        // Does only exists as lower case tag!
        // NOTE: this is an empty element!
    {
        stackItem->elementType=ElementTypeEmpty;
        stackItem->stackElementText=structureStack.current()->stackElementText;
        success=StartElementPageSize(mainDocument,attributes);
    }
    else
    {
        stackItem->elementType=ElementTypeUnknown;
        stackItem->stackElementText=structureStack.current()->stackElementText;
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
        success=charactersElementC(stackItem,mainDocument,ch);
    }
    else if (stackItem->elementType==ElementTypeParagraph)
    { // <p>
        success=charactersElementC(stackItem,mainDocument,ch);
    }
    else if (stackItem->elementType==ElementTypeEmpty)
    {
        success=ch.stripWhiteSpace().isEmpty();
        if (!success)
        {
            // We have a parsing error, so abort!
            kdError(30506) << "Empty element is not empty! Aborting! (in StructureParser::characters)" << endl;
        }
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
    strHeader+="<DOC editor=\"KWord's AbiWord Import Filter\"";
    strHeader+=" mime=\"application/x-kword\" syntaxVersion=\"2\" >\n";
    strHeader+="<ATTRIBUTES processing=\"0\" standardpage=\"1\" hasHeader=\"0\" hasFooter=\"0\" unit=\"mm\" />\n";

    // <PAPER> will be partialy changed by an AbiWord <pagesize> element.
    // FIXME: default paper of AbiWord is "Letter" not "ISO A4"
    strHeader+="<PAPER format=\"1\" width=\"595\" height=\"841\"";
    strHeader+=" orientation=\"0\" columns=\"1\" columnspacing=\"2\"";
    strHeader+="hType=\"0\" fType=\"0\" spHeadBody=\"9\" spFootBody=\"9\" zoom=\"100\">\n";

    strHeader+="<PAPERBORDERS left=\"28\" top=\"42\" right=\"28\" bottom=\"42\" />\n";
    strHeader+="</PAPER>\n</DOC>\n";

    QDomDocument qDomDocumentOut(fileOut);
    qDomDocumentOut.setContent(strHeader);

    StructureParser handler(qDomDocumentOut);

    //We arbitrarily decide that Qt can handle the encoding in which the file was written!!
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

#if 1
    kdDebug(30506) << qDomDocumentOut.toString();
#endif

    kdDebug(30506) << "Now importing to KWord!" << endl;

    return true;
}
