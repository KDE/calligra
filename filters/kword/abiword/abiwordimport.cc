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

#include <qptrstack.h>
#include <qmap.h>
#include <qxml.h>
#include <qdom.h>

#include <kdebug.h>
#include <kfilterdev.h>

#include <koGlobal.h>

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
    return true;
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
    ElementTypeContent,     // <c>
    ElementTypeField        // <field>
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
        textPosition=0;
    }
    ~StackItem()
    {
    }
public:
    StackItemElementType elementType;
    QDomElement stackElementParagraph; // <PARAGRAPH>
    QDomElement stackElementText; // <TEXT>
    QDomElement stackElementFormatsPlural; // <FORMATS>
    QString     fontName;
    int         fontSize;
    int         pos; //Position
    bool        italic;
    bool        bold;
    bool        underline;
    bool        strikeout;
    QColor      fgColor;
    QColor      bgColor;
    int         textPosition; //Normal (0), subscript(1), superscript (2)
};

class StackItemStack : public QPtrStack<StackItem>
{
public:
        StackItemStack(void) { }
        virtual ~StackItemStack(void) { }
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
protected:
    bool clearStackUntilParagraph(StackItemStack& auxilaryStack);
    bool complexForcedBreak(StackItem* stackItem, const bool pageBreak);
private:
    void createMainFramesetElement(void);
    QString indent; //DEBUG
    StackItemStack structureStack;
    QDomDocument mainDocument;
    QDomElement mainFramesetElement;     // The main <FRAMESET> where the body text will be under.
};


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

static inline double PicaToPoints(const double d)
{
    // 1 pica = 12 pt
    return d * 12.0;
}

// Do not put this function inline (it's too long!)
static double ValueWithLengthUnit( const QString& _str )
{
    double d;
    int pos=0;
    if ((pos=_str.find("cm"))>=0)
    {
        d=CentimetresToPoints(_str.left(pos).toDouble());
    }
    else if ((pos=_str.find("in"))>=0)
    {
        d=InchesToPoints(_str.left(pos).toDouble());
    }
    else if ((pos=_str.find("mm"))>=0)
    {
        d=MillimetresToPoints(_str.left(pos).toDouble());
    }
    else if((pos=_str.find("pt"))>=0)
    {
        d=_str.left(pos).toDouble();
    }
    else if((pos=_str.find("pi"))>=0)
    {
        d=PicaToPoints(_str.left(pos).toDouble());
    }
    else
    {
        bool b=false;
        d=_str.toDouble(&b);
        if (!b)
        {
            d=0;
            kdWarning(30506) << "Unknown value: " << _str << " (ValueWithLengthUnit)" << endl;
        }
    }
    return d;
}

static inline double IndentPos( const QString& _str )
// DEPRECIATED: use ValueWithLengthUnit
{
    return ValueWithLengthUnit(_str);
}

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
        // The colour information is *not* lead by a hash (#)
        stackItem->fgColor.setNamedColor("#"+strColour);
   }

    QString strBackgroundTextColor=abiPropsMap["bgcolor"].getValue();
    if(!strBackgroundTextColor.isEmpty())
    {
        // The colour information is *not* lead by a hash (#)
        stackItem->bgColor.setNamedColor("#"+strBackgroundTextColor);
    }

    QString strFontSize=abiPropsMap["font-size"].getValue();
    if (!strFontSize.isEmpty())
    {
        const int size=int(ValueWithLengthUnit(strFontSize));
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

static void AddFormat(QDomElement& formatElementOut, StackItem* stackItem, QDomDocument& mainDocument)
{
    QDomElement element;
    if (!stackItem->fontName.isEmpty())
    {
        element=mainDocument.createElement("FONT");
        element.setAttribute("name",stackItem->fontName); // Font name
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->fontSize)
    {
        element=mainDocument.createElement("SIZE");
        element.setAttribute("value",stackItem->fontSize);
        formatElementOut.appendChild(element); //Append to <FORMAT>
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

    if (stackItem->fgColor.isValid())
    {
        element=mainDocument.createElement("COLOR");
        element.setAttribute("red",  stackItem->fgColor.red());
        element.setAttribute("green",stackItem->fgColor.green());
        element.setAttribute("blue", stackItem->fgColor.blue());
        formatElementOut.appendChild(element); //Append to <FORMAT>
    }

    if (stackItem->bgColor.isValid())
    {
        element=mainDocument.createElement("TEXTBACKGROUNDCOLOR");
        element.setAttribute("red",  stackItem->bgColor.red());
        element.setAttribute("green",stackItem->bgColor.green());
        element.setAttribute("blue", stackItem->bgColor.blue());
        formatElementOut.appendChild(element); //Append to <FORMAT>
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
        stackItem->stackElementParagraph=stackCurrent->stackElementParagraph;   // <PARAGRAPH>
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

    AddFormat(formatElementOut, stackItem, mainDocument);

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
    stackItem->stackElementParagraph=paragraphElementOut; // <PARAGRAPH>
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

    QString strLeftMargin=abiPropsMap["margin-left"].getValue();
    QString strRightMargin=abiPropsMap["margin-right"].getValue();
    QString strTextIndent=abiPropsMap["text-indent"].getValue();

    if ( !strLeftMargin.isEmpty()
        || !strRightMargin.isEmpty()
        || !strTextIndent.isEmpty() )
    {
        element=mainDocument.createElement("INDENTS");
        if (!strLeftMargin.isEmpty())
            element.setAttribute("left",ValueWithLengthUnit(strLeftMargin));
        if (!strRightMargin.isEmpty())
            element.setAttribute("right",ValueWithLengthUnit(strRightMargin));
        if (!strTextIndent.isEmpty())
            element.setAttribute("first",ValueWithLengthUnit(strTextIndent));
        layoutElement.appendChild(element);
    }

    QString strTopMargin=abiPropsMap["margin-top"].getValue();
    QString strBottomMargin=abiPropsMap["margin-bottom"].getValue();
    if (!strTopMargin.isEmpty() || !strBottomMargin.isEmpty() )
    {
        element=mainDocument.createElement("OFFSETS");
        const double margin_top=ValueWithLengthUnit(strTopMargin);
        const double margin_bottom=ValueWithLengthUnit(strTopMargin);
        // Zero is propably a valid value!
        if (!strBottomMargin.isEmpty())
            element.setAttribute("after",margin_bottom);
        if (!strTopMargin.isEmpty())
            element.setAttribute("before",margin_top);
        layoutElement.appendChild(element);
    }

    QString strLineHeight=abiPropsMap["line-height"].getValue();
    if(!strLineHeight.isEmpty())
    {
        element=mainDocument.createElement("LINESPACING");
        double lineHeight;
        // Do we have a unit symbol or not?
        bool flag=false;
        lineHeight=strLineHeight.toDouble(&flag);

        if (flag)
        {
            if (lineHeight==1.5)
            {
                element.setAttribute("value","oneandhalf");
            }
            else if (lineHeight==2.0)
            {
                element.setAttribute("value","double");
            }
            else if (lineHeight!=1.0)
            {
                kdWarning(30506) << "Unsupported line height " << lineHeight << " (Ignoring !)" << endl;
            }
        }
        else
        {
            // Soemthing went wrong, so we assume that an unit is specified
            lineHeight=IndentPos(strLineHeight);
            if (lineHeight>1.0)
            {
                // We have a meaningful value, so use it!
                element.setAttribute("value",lineHeight);
            }
        }
        layoutElement.appendChild(element);
    }

    QString strTab=abiPropsMap["tabstops"].getValue();
    if(!strTab.isEmpty())
    {
        QStringList listTab=QStringList::split(",",strTab);
        for ( QStringList::Iterator it = listTab.begin(); it != listTab.end(); ++it )
        {
            QStringList tab=QStringList::split("/",*it);
            int type;
            if(tab[1]=="L0")
                type=0;
            else if(tab[1]=="C0")
                type=1;
            else if(tab[1]=="R0")
                type=2;
            else if(tab[1]=="D0")
                type=3;
            else
                type=0;
            element=mainDocument.createElement("TABULATOR");
            element.setAttribute("ptpos",ValueWithLengthUnit(tab[0]));
            element.setAttribute("type",type);
            layoutElement.appendChild(element);
        }
    }

    QDomElement formatElementOut=mainDocument.createElement("FORMAT");
    layoutElement.appendChild(formatElementOut);

    AddFormat(formatElementOut, stackItem, mainDocument);

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

bool StartElementField(StackItem* stackItem, StackItem* stackCurrent, const QXmlAttributes& attributes)
{
    // <field> element elements can be nested in <p>
    if (stackCurrent->elementType==ElementTypeParagraph)
    {
        QString strType=attributes.value("type").stripWhiteSpace();
        kdDebug()<<"field type ****************************:"<<strType<<endl;
        AbiPropsMap abiPropsMap;
        PopulateProperties(stackItem,attributes,abiPropsMap,true);

        stackItem->elementType=ElementTypeField;
        stackItem->stackElementParagraph=stackCurrent->stackElementParagraph;   // <PARAGRAPH>
        stackItem->stackElementText=stackCurrent->stackElementText;   // <TEXT>
        stackItem->stackElementFormatsPlural=stackCurrent->stackElementFormatsPlural; // <FORMATS>
        stackItem->pos=stackCurrent->pos; //Propagate the position
    }
    else
    {//we are not nested correctly, so consider it a parse error!
        kdError(30506) << "Abiword Import: parse error <field> tag not nested in neither a <p>" << endl;
        return false;
    }
    return true;
}

bool charactersElementField (StackItem* stackItem, QDomDocument& mainDocument, const QString & ch)
{
//todo
    return true;
}

bool EndElementField (StackItem* stackItem, StackItem* stackCurrent)
{
    if (!stackItem->elementType==ElementTypeField)
    {
        kdError(30506) << "Wrong element type!! Aborting! (</field> in StructureParser::endElement)" << endl;
        return false;
    }
    stackItem->stackElementText.normalize();
    stackCurrent->pos=stackItem->pos; //Propagate the position back to the parent element
    return true;
}

// <br> (forced line break)
// <cbr> (forced column break, not supported)
// <pbr> (forced page break)
static bool StartElementBR(StackItem* stackItem, StackItem* stackCurrent,
                           QDomDocument& mainDocument,
                           QDomElement& mainFramesetElement, const bool pageBreak)
// pageBreak:
//  true, if we should make a forced page break,
//  false, if we should make a forced line break
{
    // We are simulating a line break by starting a new paragraph!
    // TODO: when KWord has learnt what line breaks are, change to them.

    // We are sure to be the child of a <p> element

    // The following code is similar to the one in StartElementP
    // We use mainFramesetElement here not to be dependant that <section> has happened before
    QDomElement paragraphElementOut=mainDocument.createElement("PARAGRAPH");
    mainFramesetElement.appendChild(paragraphElementOut);
    QDomElement textElementOut=mainDocument.createElement("TEXT");
    paragraphElementOut.appendChild(textElementOut);
    QDomElement formatsPluralElementOut=mainDocument.createElement("FORMATS");
    paragraphElementOut.appendChild(formatsPluralElementOut);

    // We must now copy/clone the layout of elementText.

    QDomNodeList nodeList=stackCurrent->stackElementParagraph.elementsByTagName("LAYOUT");

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

    if (pageBreak)
    {
        // We need a page break!
        QDomElement oldLayoutElement=nodeList.item(0).toElement();
        if (oldLayoutElement.isNull())
        {
            kdError(30506) << "Cannot find old <LAYOUT> element! Aborting! (in StartElementBR)" <<endl;
            return false;
        }
        // We have now to add a element <PAGEBREAKING>
        // TODO/FIXME: what if there is already one?
        QDomElement pagebreakingElement=mainDocument.createElement("PAGEBREAKING");
        pagebreakingElement.setAttribute("linesTogether","false");
        pagebreakingElement.setAttribute("hardFrameBreak","false");
        pagebreakingElement.setAttribute("hardFrameBreakAfter","true");
        oldLayoutElement.appendChild(pagebreakingElement);
    }

    // Now that we have done with the old paragraph,
    //  we can write stackCurrent with the data of the new one!
    // NOTE: The following code is similar to the one in StartElementP but we are working on stackCurrent!
    stackCurrent->elementType=ElementTypeParagraph;
    stackCurrent->stackElementParagraph=paragraphElementOut; // <PARAGRAPH>
    stackCurrent->stackElementText=textElementOut; // <TEXT>
    stackCurrent->stackElementFormatsPlural=formatsPluralElementOut; // <FORMATS>
    stackCurrent->pos=0; // No text characters yet

    return true;
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

    double kwordHeight;
    double kwordWidth;

    QString strPageType=attributes.value("pagetype").stripWhiteSpace();

    // Do we know the page size or do we need to measure
    // For page format that KWord knows, use our own values in case the values in the file would be wrong.

	KoFormat kwordFormat = KoPageFormat::formatFromString(strPageType);

    if (kwordFormat==PG_CUSTOM)
	{
        kdDebug(30506) << "Custom or other page format found: " << strPageType << endl;

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
            kwordHeight = 0.0;
            kwordWidth  = 0.0;
            kdWarning(30506) << "Unknown unit type: " << strUnits << endl;
        }
    }
    else
    {
        // We have a format know by KOffice, so use KOffice's functions
        kwordHeight = MillimetresToPoints(KoPageFormat::height(kwordFormat,PG_PORTRAIT));
        kwordWidth  = MillimetresToPoints(KoPageFormat::width (kwordFormat,PG_PORTRAIT));
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


bool StructureParser::complexForcedBreak(StackItem* stackItem, const bool pageBreak)
{
    // We are not a child of a <p> element, so we cannot use StartElementBR directly

    StackItemStack auxilaryStack;

    if (!clearStackUntilParagraph(auxilaryStack))
        return false;

    // Now we are a child of a <p> element!

    bool success=StartElementBR(stackItem,structureStack.current(),mainDocument,mainFramesetElement,pageBreak);

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
        stackItem->stackElementText=structureStack.current()->stackElementText; // TODO: reason?
        success=true;
    }
    else if (name=="br") // NOTE: Not sure if it only exists in lower case!
    {
        // We have a forced line break
        stackItem->elementType=ElementTypeEmpty;
        StackItem* stackCurrent=structureStack.current();
        if (stackCurrent->elementType==ElementTypeContent)
        {
            success=complexForcedBreak(stackItem,false);
        }
        else if (stackCurrent->elementType==ElementTypeParagraph)
        {
            success=StartElementBR(stackItem,stackCurrent,mainDocument,mainFramesetElement,false);
        }
        else
        {
            kdError(30506) << "Forced line break found out of turn! Aborting!" <<endl;
            success=false;
        }
    }
    else if (name=="cbr") // NOTE: Not sure if it only exists in lower case!
    {
        // We have a forced column break (not supported by KWord)
        stackItem->elementType=ElementTypeEmpty;
        StackItem* stackCurrent=structureStack.current();
        if (stackCurrent->elementType==ElementTypeContent)
        {
            kdWarning(30506) << "Forced column break found! Transforming to forced page break" << endl;
            success=complexForcedBreak(stackItem,true);
        }
        else if (stackCurrent->elementType==ElementTypeParagraph)
        {
            kdWarning(30506) << "Forced column break found! Transforming to forced page break" << endl;
            success=StartElementBR(stackItem,stackCurrent,mainDocument,mainFramesetElement,true);
        }
        else
        {
            kdError(30506) << "Forced column break found out of turn! Aborting!" <<endl;
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
            success=complexForcedBreak(stackItem,true);
        }
        else if (stackCurrent->elementType==ElementTypeParagraph)
        {
            success=StartElementBR(stackItem,stackCurrent,mainDocument,mainFramesetElement,true);
        }
        else
        {
            kdError(30506) << "Forced page break found out of turn! Aborting!" <<endl;
            success=false;
        }
    }
    else if (name=="pagesize")
        // Does only exists as lower case tag!
    {
        stackItem->elementType=ElementTypeEmpty;
        stackItem->stackElementText=structureStack.current()->stackElementText; // TODO: reason?
        success=StartElementPageSize(mainDocument,attributes);
    }
    else if (name=="field")
    {
        kdDebug()<<"A Field ------------------------------------\n";
        success=StartElementField(stackItem,structureStack.current(),attributes);
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
    else if (name=="field")
    {
        success=EndElementField(stackItem,structureStack.current());
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
    else if (stackItem->elementType==ElementTypeField)
    {
        success=charactersElementField(stackItem,mainDocument,ch);
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

    mainFramesetElement=mainDocument.createElement("FRAMESET");
    mainFramesetElement.setAttribute("frameType",1);
    mainFramesetElement.setAttribute("frameInfo",0);
    mainFramesetElement.setAttribute("autoCreateNewFrame",1);
    mainFramesetElement.setAttribute("removable",0);
    // TODO: "name" attribute (needs I18N)
    framesetsPluralElementOut.appendChild(mainFramesetElement);

    QDomElement frameElementOut=mainDocument.createElement("FRAME");
    frameElementOut.setAttribute("left",28);
    frameElementOut.setAttribute("top",42);
    frameElementOut.setAttribute("bottom",566);
    frameElementOut.setAttribute("right",798);
    frameElementOut.setAttribute("runaround",1);
    mainFramesetElement.appendChild(frameElementOut);

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
                kdError(30506) << "Cannot clear this element!" << endl;
                return false;
            }
        }
    }
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

    QDomDocument qDomDocumentOut("DOC");
    qDomDocumentOut.appendChild(
        qDomDocumentOut.createProcessingInstruction(
        "xml","version=\"1.0\" encoding=\"UTF-8\""));

    QDomElement elementDoc;
    elementDoc=qDomDocumentOut.createElement("DOC");
    elementDoc.setAttribute("editor","KWord's AbiWord Import Filter");
    elementDoc.setAttribute("mime","application/x-kword");
    elementDoc.setAttribute("syntaxVersion",2);
    qDomDocumentOut.appendChild(elementDoc);

    QDomElement element;
    element=qDomDocumentOut.createElement("ATTRIBUTES");
    element.setAttribute("processing",0);
    element.setAttribute("standardpage",1);
    element.setAttribute("hasHeader",0);
    element.setAttribute("hasFooter",0);
    element.setAttribute("unit","mm");
    elementDoc.appendChild(element);

    QDomElement elementPaper;
    // <PAPER> will be partialy changed by an AbiWord <pagesize> element.
    // default paper format of AbiWord is "Letter"
    elementPaper=qDomDocumentOut.createElement("PAPER");
    elementPaper.setAttribute("format",PG_US_LETTER);
    elementPaper.setAttribute("width",MillimetresToPoints(KoPageFormat::width (PG_US_LETTER,PG_PORTRAIT)));
    elementPaper.setAttribute("height",MillimetresToPoints(KoPageFormat::height(PG_US_LETTER,PG_PORTRAIT)));
    elementPaper.setAttribute("orientation",PG_PORTRAIT);
    elementPaper.setAttribute("columns",1);
    elementPaper.setAttribute("columnspacing",2);
    elementPaper.setAttribute("hType",0);
    elementPaper.setAttribute("fType",0);
    elementPaper.setAttribute("spHeadBody",9);
    elementPaper.setAttribute("spFootBody",9);
    elementPaper.setAttribute("zoom",100);
    elementDoc.appendChild(elementPaper);

    element=qDomDocumentOut.createElement("PAPERBORDERS");
    element.setAttribute("left",28);
    element.setAttribute("top",42);
    element.setAttribute("right",28);
    element.setAttribute("bottom",42);
    elementPaper.appendChild(element);

    kdDebug(30506) << "Header " << endl << qDomDocumentOut.toString() << endl;

    StructureParser handler(qDomDocumentOut);

    //We arbitrarily decide that Qt can handle the encoding in which the file was written!!
    QXmlSimpleReader reader;
    reader.setContentHandler( &handler );

    //Find the last extension
    QString strExt;
    const int result=fileIn.findRev('.');
    if (result>=0)
    {
        strExt=fileIn.mid(result);
    }

    kdDebug(30506) << "File extension: -" << strExt << "-" << endl;

    QString strMime; // Mime type of the compressor (default: unknown)

    if ((strExt==".gz")||(strExt==".GZ")        //in case of .abw.gz (logical extension)
        ||(strExt==".zabw")||(strExt==".ZABW")) //in case of .zabw (extension used prioritary with AbiWord)
    {
        // Compressed with gzip
        strMime="application/x-gzip";
        kdDebug(30506) << "Compression: gzip" << endl;
    }
    else if ((strExt==".bz2")||(strExt==".BZ2") //in case of .abw.bz2 (logical extension)
        ||(strExt==".bzabw")||(strExt==".BZABW")) //in case of .bzabw (extension used prioritary with AbiWord)
    {
        // Compressed with bzip2
        strMime="application/x-bzip2";
        kdDebug(30506) << "Compression: bzip2" << endl;
    }

    QIODevice* in = KFilterDev::deviceForFile(fileIn,strMime);

    if (!in->open(IO_ReadOnly))
    {
        kdError(30506) << "Cannot open file! Aborting!" << endl;
        delete in;
        return false;
    }

    QXmlInputSource source(in); // Read the file

    in->close();

    if (!reader.parse( source ))
    {
        kdError(30506) << "Import (COMPRESSED): Parsing unsuccessful. Aborting!" << endl;
        delete in;
        return false;
    }
    delete in;

    KoStore out=KoStore(fileOut, KoStore::Write);
    if(!out.open("root"))
    {
        kdError(30506) << "AbiWord Import unable to open output file!" << endl;
        out.close();
        return false;
    }

    //Write the document!
    QCString strOut=qDomDocumentOut.toCString();
    out.write(strOut);
    out.close();

#if 0
    kdDebug(30506) << qDomDocumentOut.toString();
#endif

    kdDebug(30506) << "Now importing to KWord!" << endl;

    return true;
}
