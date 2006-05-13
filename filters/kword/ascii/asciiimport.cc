/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2000 Michael Johnson <mikej@xnet.com>
   Copyright (C) 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>

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

#include <QObject>
#include <QString>
// #include <QRegExp> // Currently not needed (due to disabled code)
#include <QTextCodec>
#include <QFile>
#include <QTextStream>
#include <qdom.h>
#include <qfontinfo.h>
//Added by qt3to4:
#include <QByteArray>

#include <kdebug.h>
#include <kglobal.h>
#include <kgenericfactory.h>

#include <KoGlobal.h>
#include <KoPageLayout.h>
#include <KoStore.h>
#include <KoFilterChain.h>
#include <KoFilterManager.h>

#include <asciiimport.h>
#include <asciiimport.moc>
#include "ImportDialog.h"

class ASCIIImportFactory : KGenericFactory<ASCIIImport>
{
public:
    ASCIIImportFactory(void) : KGenericFactory<ASCIIImport> ("kwordasciiimport")
    {}
protected:
    virtual void setupTranslations( void )
    {
        KGlobal::locale()->insertCatalog( "kofficefilters" );
    }
};

K_EXPORT_COMPONENT_FACTORY( libasciiimport, ASCIIImportFactory() )

ASCIIImport::ASCIIImport(QObject *parent, const QStringList &) :
                     KoFilter(parent) {
}

void ASCIIImport::prepareDocument(QDomDocument& mainDocument, QDomElement& mainFramesetElement)
{
    // TODO: other paper formats
    KoFormat paperFormat=PG_DIN_A4; // ISO A4
    KoOrientation paperOrientation=PG_PORTRAIT;

    mainDocument.appendChild(
        mainDocument.createProcessingInstruction(
        "xml","version=\"1.0\" encoding=\"UTF-8\""));

    QDomElement elementDoc;
    elementDoc=mainDocument.createElement("DOC");
    elementDoc.setAttribute("editor","KWord's Plain Text Import Filter");
    elementDoc.setAttribute("mime","application/x-kword");
    // TODO: We claim to be syntax version 2, but we should verify that it is also true.
    elementDoc.setAttribute("syntaxVersion",2);
    mainDocument.appendChild(elementDoc);

    QDomElement element;
    element=mainDocument.createElement("ATTRIBUTES");
    element.setAttribute("processing",0);
    element.setAttribute("standardpage",1);
    element.setAttribute("hasHeader",0);
    element.setAttribute("hasFooter",0);
    //element.setAttribute("unit","mm"); // use KWord default instead
    elementDoc.appendChild(element);

    QDomElement elementPaper=mainDocument.createElement("PAPER");
    elementPaper.setAttribute("format",paperFormat);
    elementPaper.setAttribute("width" ,KoPageFormat::width (paperFormat,paperOrientation) * 72.0 / 25.4);
    elementPaper.setAttribute("height",KoPageFormat::height(paperFormat,paperOrientation) * 72.0 / 25.4);
    elementPaper.setAttribute("orientation",PG_PORTRAIT);
    elementPaper.setAttribute("columns",1);
    elementPaper.setAttribute("columnspacing",2);
    elementPaper.setAttribute("hType",0);
    elementPaper.setAttribute("fType",0);
    elementPaper.setAttribute("spHeadBody",9);
    elementPaper.setAttribute("spFootBody",9);
    elementPaper.setAttribute("zoom",100);
    elementDoc.appendChild(elementPaper);

    element=mainDocument.createElement("PAPERBORDERS");
    element.setAttribute("left",28);
    element.setAttribute("top",42);
    element.setAttribute("right",28);
    element.setAttribute("bottom",42);
    elementPaper.appendChild(element);

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

    QDomElement elementStylesPlural=mainDocument.createElement("STYLES");
    elementDoc.appendChild(elementStylesPlural);

    QDomElement elementStyleStandard=mainDocument.createElement("STYLE");
    elementStylesPlural.appendChild(elementStyleStandard);

    element=mainDocument.createElement("NAME");
    element.setAttribute("value","Standard");
    elementStyleStandard.appendChild(element);

    element=mainDocument.createElement("FOLLOWING");
    element.setAttribute("name","Standard");
    elementStyleStandard.appendChild(element);

    QDomElement elementFormat=mainDocument.createElement("FORMAT");
    elementStyleStandard.appendChild(elementFormat);

    // Use QFontInfo, as it does not give back -1 as point size.
    QFontInfo defaultFontInfo(KoGlobal::defaultFont());

    element=mainDocument.createElement("FONT");
    element.setAttribute("name",defaultFontInfo.family());
    elementFormat.appendChild(element);

    element=mainDocument.createElement("SIZE");
    element.setAttribute("value",defaultFontInfo.pointSize());
    elementFormat.appendChild(element);

    element=mainDocument.createElement("ITALIC");
    element.setAttribute("value",0);
    elementFormat.appendChild(element);

    element=mainDocument.createElement("WEIGHT");
    element.setAttribute("value",50);
    elementFormat.appendChild(element);

    element=mainDocument.createElement("UNDERLINE");
    element.setAttribute("value",0);
    elementFormat.appendChild(element);

    element=mainDocument.createElement("STRIKEOUT");
    element.setAttribute("value",0);
    elementFormat.appendChild(element);

    element=mainDocument.createElement("VERTALIGN");
    element.setAttribute("value",0);
    elementFormat.appendChild(element);

    element=mainDocument.createElement("COLOR");
    element.setAttribute("red",  -1);
    element.setAttribute("green",1);
    element.setAttribute("blue", -1);
    elementFormat.appendChild(element);

    element=mainDocument.createElement("TEXTBACKGROUNDCOLOR");
    element.setAttribute("red",  -1);
    element.setAttribute("green",-1);
    element.setAttribute("blue", -1);
    elementFormat.appendChild(element);

}

KoFilter::ConversionStatus ASCIIImport::convert( const QByteArray& from, const QByteArray& to )
{
    if (to!="application/x-kword" || from!="text/plain")
        return KoFilter::NotImplemented;

    AsciiImportDialog* dialog = 0;
    if (!m_chain->manager()->getBatchMode())
    {
    	dialog = new AsciiImportDialog();
	if (!dialog)
	{
	  kError(30502) << "Dialog has not been created! Aborting!" << endl;
	  return KoFilter::StupidError;
	}
	if (!dialog->exec())
	{
	  kDebug(30502) << "Dialog was aborted! Aborting filter!" << endl; // this isn't an error!
	  return KoFilter::UserCancelled;
	}
    }

    QTextCodec* codec;
    int paragraphStrategy;
    if (dialog)
    {
      	codec = dialog->getCodec();
	paragraphStrategy=dialog->getParagraphStrategy();
	delete dialog;
    }
    else
    {
      codec = QTextCodec::codecForName("UTF-8");
      paragraphStrategy=0;
    }

    QFile in(m_chain->inputFile());
    if(!in.open(QIODevice::ReadOnly)) {
        kError(30502) << "Unable to open input file!" << endl;
        in.close();
        return KoFilter::FileNotFound;
    }

    QString tbl;  // string for table XML

    QDomDocument mainDocument;
    QDomElement mainFramesetElement;

    prepareDocument(mainDocument,mainFramesetElement);

    QTextStream stream(&in);

    if (!codec)
    {
        kError(30502) << "Could not create QTextCodec! Aborting" << endl;
        in.close();
        return KoFilter::StupidError;
    }

    kDebug(30502) << "Charset used: " << codec->name() << endl;

    stream.setCodec(codec);

    if (1==paragraphStrategy)
        sentenceConvert(stream, mainDocument, mainFramesetElement);
    else if (999==paragraphStrategy)
        oldWayConvert(stream, mainDocument, mainFramesetElement);
    else
        asIsConvert(stream, mainDocument, mainFramesetElement);


#if 0
    kDebug(30502) << mainDocument.toString() << endl;
#endif

    KoStoreDevice* out=m_chain->storageFile( "root", KoStore::Write );
    if(!out) {
        kError(30502) << "Unable to open output file!" << endl;
        in.close();
        return KoFilter::StorageCreationError;
    }
    QByteArray cstr=mainDocument.toByteArray();
    // WARNING: we cannot use KoStore::write(const QByteArray&) because it gives an extra NULL character at the end.
    out->write(cstr,cstr.length());
    in.close();
    return KoFilter::OK;
}

void ASCIIImport::oldWayConvert(QTextStream& stream, QDomDocument& mainDocument,
    QDomElement& mainFramesetElement)
{
    kDebug(30502) << "Entering: ASCIIImport::oldWayConvert" << endl;
    QStringList paragraph;  // lines of the paragraph
    int linecount = 0;  // line counter used to position tables
    //int table_no = 0;  // used for table identifiers
    int numLines; // Number of lines of the paragraph

    bool lastCharWasCr=false; // Was the previous character a Carriage Return?
    QString strLine;
    while(!stream.atEnd())
    {
        paragraph.clear();
        // Read in paragraph
        for(int line_no = numLines = 0; line_no < MAXLINES; line_no++, numLines++)
        {
            // We need to read a line
            // NOTE: we cannot use QStreamText::readLine,
            //   as it does not know anything about Carriage Returns
            strLine=readLine(stream,lastCharWasCr);
            if (strLine.isEmpty())
            {
                paragraph.append(QString::null);
                break;
            }

            const int length = strLine.length();
            if (strLine.at(length-1) == '-')
                // replace the hard hyphen - at line end by a soft hyphen
                strLine[length-1]=QChar(173);
            else
                strLine += ' '; // add space to end of line

            paragraph.append(strLine);
        } // for(line_no = 0;

         //   process tables
#if 0
        if ( Table( &Line[0], &linecount, numLines, table_no, tbl, str))
            table_no++;
        else
#else
        if (true)
#endif
        {
        // Process bullet and dash lists
#if 0
        if(ListItem( &Line[0], numLines, str))
            linecount += (numLines + 1);
        else
           {
#else
            if (true)
            {
#endif
                processParagraph(mainDocument,mainFramesetElement,paragraph);
                linecount += ( numLines + 1);  // increment the line count
            }  // else
        }  // else
     }  // while(!eof)

#if 0
    // Add table info
    if( table_no > 0) str += tbl;
#endif
}

void ASCIIImport::asIsConvert(QTextStream& stream, QDomDocument& mainDocument,
    QDomElement& mainFramesetElement)
// Paragraph strategy: one line, one paragraph
{
    kDebug(30502) << "Entering: ASCIIImport::asIsConvert" << endl;
    bool lastCharWasCr=false; // Was the previous character a Carriage Return?
    QString strLine;
    while(!stream.atEnd())
    {
        // Read one line and consider it being a paragraph
        strLine=readLine(stream,lastCharWasCr);
        writeOutParagraph(mainDocument,mainFramesetElement, "Standard", strLine,
            0, 0);
    }
}

void ASCIIImport::sentenceConvert(QTextStream& stream, QDomDocument& mainDocument,
    QDomElement& mainFramesetElement)
// Strategy:
// - end a paragraph when a line ends with a point or similar punctuation.
// - search the punctuation at the end of the line, even if the sentence is quoted or in parentheses.
// - an empty line also ends the paragraph
// TODO/FIXME: we have a little problem with empty lines. Perhaps we should not allow empty paragraphs!
{
    kDebug(30502) << "Entering: ASCIIImport::sentenceConvert" << endl;
    QStringList paragraph;  // lines of the paragraph
    bool lastCharWasCr=false; // Was the previous character a Carriage Return?
    QString strLine;
    QString stoppingPunctuation(".!?");
    QString skippingQuotes("\"')");
    while (!stream.atEnd())
    {
        paragraph.clear();
        for(;;)
        {
            // We need to read a line
            // NOTE: we cannot use QStreamText::readLine,
            //   as it does not know anything about Carriage Returns
            strLine=readLine(stream,lastCharWasCr);
            if (strLine.isEmpty())
            {
                break;
            }

            paragraph.append(strLine);

            int lastPos = strLine.length() - 1;
            QChar lastChar;
            // Skip a maximum of 10 quotes (or similar) at the end of the line
            for (int i=0;i<10;i++)
            {
                lastChar=strLine[lastPos];
                if (lastChar.isNull())
                    break;
                else if (skippingQuotes.find(lastChar)==-1)
                    break;
                else
                    lastPos--;
            }

            lastChar=strLine[lastPos];
            if (lastChar.isNull())
                continue;
            else if (stoppingPunctuation.find(lastChar)!=-1)
                break;
        }
#if 1
        writeOutParagraph(mainDocument,mainFramesetElement, "Standard",
            paragraph.join(" ").simplified(), 0, 0);
#else
        // FIXME/TODO: why is this not working?
        //processParagraph(mainDocument,mainFramesetElement,paragraph);
#endif
     }  // while(!eof)
}

void ASCIIImport::processParagraph(QDomDocument& mainDocument,
    QDomElement& mainFramesetElement, const QStringList& paragraph)
{
    // Paragraph with no tables or lists
    QString text;
    QStringList::ConstIterator it=paragraph.begin(); // Current line (at start, the first one)
    QStringList::ConstIterator previousLine=it; // The previous one (at start, also the first one)
    int firstindent=Indent(*it);

    // We work with one line in advance (therefore the two it++)
    for( it++; it!=paragraph.end(); it++)
    {
        text += *previousLine; // add previous line to paragraph
        // check for a short line - if short make it a paragraph
        if( (*previousLine).length() <= shortline)
        {
            if((*it).length() > shortline)
            // skip if short last line of normal paragraph
            {
                const int secondindent = Indent(*previousLine);
                writeOutParagraph(mainDocument,mainFramesetElement,
                    "Standard", text.simplified(), firstindent, secondindent);

                firstindent = Indent(*it);
                text = QString::null;  // reinitialize paragraph text
            }
        }
        previousLine=it;
    }
    // write out paragraph begin to end
    const int secondindent = Indent(*previousLine);
    writeOutParagraph(mainDocument,mainFramesetElement,
        "Standard", text.simplified(), firstindent, secondindent);
}

void ASCIIImport::writeOutParagraph(QDomDocument& mainDocument,
    QDomElement& mainFramesetElement, const QString& name,
    const QString& text, const int firstindent, const int secondindent)
{
    QDomElement paragraphElementOut=mainDocument.createElement("PARAGRAPH");
    mainFramesetElement.appendChild(paragraphElementOut);
    QDomElement textElement=mainDocument.createElement("TEXT");
    paragraphElementOut.appendChild(textElement);
    //QDomElement formatsPluralElementOut=mainDocument.createElement("FORMATS");
    //paragraphElementOut.appendChild(formatsPluralElementOut);
    QDomElement layoutElement=mainDocument.createElement("LAYOUT");
    paragraphElementOut.appendChild(layoutElement);

    QDomElement element;
    element=mainDocument.createElement("NAME");
    element.setAttribute("value",name);
    layoutElement.appendChild(element);

    double size;
    element=mainDocument.createElement("INDENTS");
    size = firstindent-secondindent;
    size *= ptsperchar;  // convert indent spaces to points
    element.setAttribute("first",QString::number(size));
    size = secondindent;
    size *= ptsperchar;  // convert indent spaces to points
    element.setAttribute("left",QString::number(size));
    element.setAttribute("right",0);

    textElement.appendChild(mainDocument.createTextNode(text));
    textElement.normalize(); // Put text together (not sure if needed)

#if 0
    // If the paragraph is indented, write out indentation elements.
    // TODO: why not always write identation?
    if (firstindent > 0 || secondindent > 0)         \
             WriteOutIndents( firstindent, secondindent, str);

    // If this is a bullet or a dash list, write out a COUNTER element
    if(type.isEmpty())
       {
       str += "<COUNTER type=\"";
       str += type;  // "6" = bullet "7" = dash
       str += "\" depth=\"0\" start=\"1\" numberingtype=\"0\" lefttext=\"\" righttext=\".\" />\n";
       }

    str += "<FORMAT>\n";
    // for now we try with style (TODO: verify that KWord 1.2 works correctly)
    //str += "<FONT name=\"times\"/>\n";
    str += "</FORMAT>\n";
    str += "</LAYOUT>\n";
    str += "</PARAGRAPH>\n";
#endif
}  // WriteOutParagraph

   /* The Indent method determines the equivalent number of spaces
      at the beginning of a line   */

int ASCIIImport::Indent(const QString& line) const
{

    QChar c;  // for reading string a character at a time
    int count=0;  // amount of white space at the begining of the line

    for( int i = 0; i < line.length(); i++ )
    {
         c = line.at(i);
         if( c == QChar(' '))
            count++;
         else if( c == QChar('\t'))
            count += spacespertab;
         else
            break;
    }

   return count;

}  // Indent

    /* The WriteOutTableCell method writes out a single table cell
        in Kword XML to the output string str. The table is sized according
       to the spacing in the ascii document.
       Arguments
         int table_no - used to keep the group number count
         int row - row number
         int col - column number
         Position *pos - pointer to the table position
         QString str - the output string.

       Returns - void
        */

#if 0
   void ASCIIImport::WriteOutTableCell( int table_no, int row,
                         int col, Position *pos, QString &str)
   {

   str += "<FRAMESET frameType=\"1\" frameInfo=\"0\" grpMgr=\"grpmgr_";

   QString buf = QString::number(table_no); // convert int to string
   str += buf;

   str += "\" row=\"";
   buf = QString::number(row); // convert row to string
   str += buf;

   str += "\" col=\"";
   buf = QString::number(col); // convert col to string
   str += buf;

   str += "\" rows=\"1\" cols=\"1\" removable=\"0\" visible=\"1\" name=\"Cell ";
   buf = QString::number(row); // convert row to string
   str += buf;
   str += ",";
   buf = QString::number(col); // convert col to string
   str += buf;

   str += "\">\n";

   str += "<FRAME left=\"";
   buf = QString::number(pos->left); // convert left coordinate to string
   str += buf;

   str += "\" top=\"";
   buf = QString::number(pos->top); // convert top coordinate to string
   str += buf;

   str += "\" right=\"";
   buf = QString::number(pos->right); // convert right coordinate to string
   str += buf;

   str += "\" bottom=\"";
   buf = QString::number(pos->bottom); // convert bottom coordinate to string
   str += buf;

   str += "\" runaround=\"0\" autoCreateNewFrame=\"0\" newFrameBehavior=\"1\" />\n";

   return;

   }  // WriteOutTableCell




bool ASCIIImport::Table( QString *Line, int *linecount, int no_lines,
                         int table_no, QString &tbl, QString &str )
{
    return false; // this method is disabled

   /* This method examines a paragraph for embedded tables.
      If none are found, it returns. If tables are found then it
      writes out the paragraph including the tables. It does not
      check for embedded bullet or dash lists which is a seperate
      method.
      Arguments
         QString *Line - pointer to the array of text lines in the paragraph.
         int *linecount - pointer to the line counter used to position tables
         int no_lines - the number of lines in the paragraph
         int *table_no - pointer to the table number counter
         QString str  - the output string
      Returns - enum value indicating wheterer a table was processed. */

      enum LiType{paragraph, table} linetype[MAXLINES];
      struct Tabs tabs[MAXLINES];  // array of structures for tables
      struct Position pos;  // struct to pass cell position
      int width[MAXCOLUMNS]; // array of column widths
      QString *currentline;  // pointer to the current line
      QString *beginline;  // pointer to the beginning line
      QString text;  // text buffer
      QString text1;  // another text buffer
      QString No_indents;  // Line with leading white space removed
      int index, index1, index2, index3; // used to define column positions in a line

      int no_cols;  // column counter
      int firstindent;
      int secondindent;
      int i;  // counter
      int j;  // counter
      int k;  // counter
      QString *firstline;
      firstline = Line;
      int begin = 0;  // beginning line number
      int tabcount = 0;   // counts tab characters

      // check all lines for tabs
      for(i = 0; i < no_lines; i++)
         {
         no_cols = 0;  // inilialize the number of columns
         index = 0;  // initialize the line index

         // check the line for leading tabs and indentation
         if( (tabs[i].indent = Indent( *Line)) > 0)
             {
             // save and remove tabs at beginning of the line
             text = *Line;  // copy
             Line->remove(QRegExp("^[\\ \t]*"));
             }

         // find column positions and record text fields
         while((index2 = Line->find( QRegExp("\t"),index)) > index
         || (index3 = MultSpaces( *Line, index)) > index )
           {
           index1 = qMax(index2, index3);
           if( index2 > index3)
           index1 = Line->find( QRegExp("[^\t]"), index1);
           tabcount++;
           tabs[i].field[no_cols] = Line->mid(index, (index1 - index -1));
           tabs[i].width[no_cols] = index1 - index + spacespertab - 1;

           no_cols++;
           index = index1;
           }  // while( (index1 = ...

         // record last column
         if( no_cols > 0)
            {
            index1 = Line->length() - (index + 1);  // +1 for space at line end
            tabs[i].field[no_cols] = Line->mid(index,index1);
            tabs[i].width[no_cols] = index1;
            }

         if(tabs[i].indent > 0) *Line = text;  // restore original line
         Line++;  // increment the line pointer
         tabs[i].columns = no_cols;
         }  // for(i = 0; ...
         // All lines of the paragraph have been checked for tabs

     // check for a tab in at least one line
     if( tabcount == 0) return false;
     tabcount = 0;

     Line = firstline;  // reset the line pointer

     for(j = 0; j < MAXCOLUMNS; j++) width[j] = 0;
     for( i = 0; i <= no_lines; i++)
        {
        if(tabs[i].columns == 0 )  // indicates no tabs in line
           {
           text += *Line;  // add line of text to the paragraph
           linetype[i] = paragraph;
           }

        else  // this means a tabulated line
           {
           linetype[i] = table;

           // calculate the column widths
           for(j = 0; j <= tabs[i].columns; j++)
              width[j] = qMax(tabs[i].width[j], width[j] );
           }  // else

        if(i > 0)
           {
           if( linetype[i] != linetype[(i - 1)] || i == no_lines) // change of type
              {
              if( linetype[(begin)] == paragraph )  // write out a paragraph
                 {
                 text = "";  // initialize a null string for paragraph text
                 currentline = firstline;
                 currentline += begin;  //starting line
                 beginline = currentline;
                 firstindent = Indent(*currentline);
                 currentline += 1;  // second line
                 if( (i - begin) > 1)
                        secondindent = Indent( *currentline );
                 else secondindent = 0;

                 for( j= begin; j < i; j++)
                    {
                    text += *beginline;  // form text entry
                    beginline++;   // increment pointer
                    }
                 // process the white space to eliminate unwanted spaces
                 QString text1 = text.simplified();
                 WriteOutParagraph( "Standard", "", text1 , firstindent, secondindent, str);
                 *linecount += (i - begin);

                 }  // if( linetype[(i - 1)]

              else       // this implies a table
                 {
                 // Write out a blank line to align text and table
                 WriteOutParagraph( "Standard", "", "" , 0, 0, str);
                 for( j = begin; j < i; j++ )
                    {
                    int row = j - begin;
                    pos.top = (double)((*linecount) + 1) * ptsperline;
                    pos.bottom = pos.top + ptsperline;
                    // initialize position of first col
                    pos.left = leftmargin + (double)tabs[i].indent * ptsperchar;

                    // write out a cell for each column
                    for( k = 0; k <= tabs[j].columns; k++)
                       {

                       text = tabs[j].field[k];
                       // process the white space to eliminate unwanted spaces
                       text1 = text.simplified();

                       // calculate position of table cell
                       pos.right = pos.left + (double)width[k] * ptsperchar;

                       WriteOutTableCell( table_no, row, k, &pos, tbl);
                       WriteOutParagraph( "Standard", "", text1 , 0, 0, tbl);
                       tbl += "</FRAMESET>\n";
                       pos.left += ptsperchar * (double)width[k];
                       }  // for( k = 0; ...
                    (*linecount)  += 1;
                    }   // for( j = begin; ...


                 }  // else...
              begin = i;
              }  // if( linetype[i]...

           }  // if(i > 0)
        Line++;
        }  // for( i = 0; ...

   (*linecount) += 1;

   return true;
   } // end of Table()
#endif

   // the following method finds the location of multiple spaces in a string
int ASCIIImport::MultSpaces(const QString& text, const int index) const
{

    QChar c;
    QChar lastchar = 'c'; // previous character - initialize non blank
    bool found = false;
    // kDebug(30502) << "length = "  << text.length() << endl;
    for (int i = index; i < text.length(); i++)
    {
        c = text.at(i);
    // kDebug(30502) << "i = " << i << " found = " << found << " c = " << c << " lastchar = " << lastchar << endl;
        if ( (c != ' ') && found)
            return i;
        else if (c == ' ' && lastchar == ' ')
            found = true;
        lastchar = c;
    }
    return -1;
} // MultSpaces

#if 0
   bool ASCIIImport::ListItem( QString *Line, int no_lines,
             QString &str )
      {

   /* This method examines a paragraph for embedded lists.
      If none are found, it returns. If lists are found then it
      writes out the paragraph including the lists.
      Arguments
         QString *Line - pointer to the array of text lines in the paragraph.
         int *linecount - pointer to the line counter used to position tables
         int no_lines - the number of lines in the paragraph
         QString &str  - the output string
      Returns - bool value indicating whether a list item was processed. */


      QString *currentline;  // pointer to the current line
      enum Listtype{none, bullet, dash} listtype[MAXLINES];
      QString type;  // type of list - bullet or dash
      QString *firstline;
      int i;  // counter
      int firstindent;
      int secondindent;
      int begin = 0;  // beginning line number
      int listcount = 0;   // counts lines with list items
      firstline = Line;

      // check all lines for list item indicators - or *
      for(i = 0; i < no_lines; i++)
         {
         listtype[i] = none;  // initialize

         // check for a bullet list item
         if( IsListItem( *Line, QChar('*')))
            {
            listtype[i] = bullet;
            listcount++;
            }
         // check for a dash list item
         if( IsListItem( *Line, QChar('-')))
            {
            listtype[i] = dash;
            listcount++;
            }

         Line++;  // increment the line pointer
         }  // for(i = 0; ...
         // All lines of the paragraph have been checked for list items

         // check for at least one list
         if( listcount == 0) return false;

         QString text = "";  // initialize a null string for paragraph text
         Line = firstline;  // reset the line pointer

         for( i = 0; i < no_lines; i++)
            {

            if(i > 0)
               {
               if( listtype[i] != none || Line->isEmpty()) // new list item
                  {
                  if( listtype[begin] == bullet)  // write out a bullet list
                     {
                     type = "6";
                     // delete * at beginning of line
                     text.remove( QRegExp("^[ \t]*\\* ") );
                     }
                  else if( listtype[begin] == dash) // write out a dash list
                     {
                     type = "7";
                     // delete - at beginning of line
                     text.remove( QRegExp("^[ \t]*\\- ") );
                     }
                  else if( listtype[begin] == none) // write out a paragraph
                     type = "";

                     currentline = firstline;
                     currentline += begin;  //starting line
                     firstindent = Indent(*currentline);
                     if( (i - begin) > 1)
                        {
                        currentline += 1;  // second line
                        secondindent = Indent( *currentline );
                        }
                     else secondindent = 0;

                     // process the white space to eliminate unwanted spaces
                     QString text1 = text.simplified();
                     WriteOutParagraph( "Standard", type, text1 , firstindent, secondindent, str);

                     begin = i;
                     text = "";  // reset text


                  }  // if( listtype[i] != none


               }  // if(i > 0)

            text += *Line;  // add text to paragraph
            Line++;

            }  // for( i = 0; ...

         // write out the last paragraph
         if( listtype[begin] == bullet)  // write out a bullet list
            {
            type = "6";
            // delete * at beginning of line
            text.replace( QRegExp("^[ \t]*\\* "), "");
            }
         else if( listtype[begin] == dash) // write out a dash list
            {
            type = "7";
            // delete - at beginning of line
            text.replace( QRegExp("^[ \t]*\\- "), "");
            }
         else if( listtype[begin] == none) // write out a paragraph
            type = "";

         currentline = firstline;
         currentline += begin;  //starting line
         firstindent = Indent(*currentline);
         if( (i - begin) > 1)
            {
            currentline += 1;  // second line
            secondindent = Indent( *currentline );
            }
         else secondindent = 0;

         // process the white space to eliminate unwanted spaces
         QString text1 = text.simplified();
         WriteOutParagraph( "Standard", type, text1 , firstindent, secondindent, str);



   return true;
   } // end of ListItem()
#endif

/* The IsListItem method checks a paragraph's first line and determines if
   the text appears to be a list item (bullet or dash).

   Arguments
     QString FirstLine  the first line of text in a paragraph
     QChar mark         the mark - or * that indicates a list item

   Return
     true if the line appears as a list item, false if not. */

bool ASCIIImport::IsListItem( QString FirstLine, QChar mark )
  {

     int i;  // counter
     QChar c; // to stream into one character at a time



     int k = FirstLine.find(mark);

     if( k < 0) return false;  // list item mark not on line

     // find first non white character on the line
     for( i=0; IsWhiteSpace( c = FirstLine.at(i) ); i++)
         ;

     if(i != k ) return false;  // mark is in wrong position

     c = FirstLine.at(++i);

     return IsWhiteSpace(c);  // character after the mark must be a white space

  }  // IsListItem


bool ASCIIImport::IsWhiteSpace(const QChar& c) const
{
     if ( c == QChar(' ') || c == QChar('\t') )
        return true;
     else
        return false;
} // IsWhiteSpace

QString ASCIIImport::readLine(QTextStream& textstream, bool& lastCharWasCr)
{
    // We need to read a line, character by character
    // NOTE: we cannot use QStreamText::readLine,
    //   as it does not know anything about Carriage Returns
    QString strLine;
    QChar ch;
    while (!textstream.atEnd())
    {
        textstream >> ch; // Read one character
        if (ch=='\n')
        {
            if (lastCharWasCr)
            {
                // We have a line feed following a Carriage Return
                // As the Carriage Return has already ended the previous line,
                // discard this Line Feed.
                lastCharWasCr=false;
            }
            else
            {
                // We have a normal Line Feed, therefore we end the line
                break;
            }
        }
        else if (ch=='\r')
        {
            // We have a Carriage Return, therefore we end the line
            lastCharWasCr=true;
            break;
        }
        else if (ch==char(12)) // Form Feed
        {
            // Ignore the form feed
            continue;
        }
        else
        {
            strLine+=ch;
            lastCharWasCr=false;
        }
    } // while
    return strLine;
}
