
/* This file is part of the KDE project
   Copurogjt (C) 2001 Michael Johnson <mikej@xnet.com>


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

#include <rtfexport.h>
#include <rtfexport.moc>


// global variables
QValueList<FontTable> fontTable; // holds all fonts found in the parsed text
QString fontHeader;  // Font header string (markup)
char ctrla = 1;  // this is a control A character
paraNumberingType type[10];  // type of paragraph numbering for 10 levels
// paragraph numbers for ten levels
char paraNumber[10] = {0,0,0,0,0,0,0,0,0,0};
QString pageMarkup;  // global to store page size markup
QString bookMarkup;  // markup bor document info file

/***************************************************************************/

static void CreateMissingFormatData(QString &paraText, QValueList<FormatData> &paraFormatDataList)
// Creates back the behaviour of KWord 0.8 (the data of <TEXT> is fully discribed by the FormatData array)
// Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>
{
    QValueList<FormatData>::Iterator  paraFormatDataIt;
    int lastPos=0; // last position

    paraFormatDataIt = paraFormatDataList.begin ();
    while (paraFormatDataIt != paraFormatDataList.end ())
    {
        // Some default values!
		int pos;
		int len;

		if (1==(*paraFormatDataIt).id)
		{
			pos=(*paraFormatDataIt).text.pos;
			len=(*paraFormatDataIt).text.len;
		}
		else if (2==(*paraFormatDataIt).id)
		{
			pos=(*paraFormatDataIt).pictureAnchor.pos;
			len=1; // TODO: verify that this value is right
		}
		else if (6==(*paraFormatDataIt).id)
		{
			pos=(*paraFormatDataIt).tableAnchor.pos;
			len=1;
		}
        else
        {   // Give default values if we do not know the value of "id"
            pos=lastPos;
            len=1;
        }
        if (pos>lastPos)
        {
            //We must add a FormatData
			TextFormatting format(lastPos,pos-lastPos,11,50,"times",false,false,0);
            FormatData formatData(format);
            paraFormatDataList.insert(paraFormatDataIt,formatData);
        }
        lastPos=pos+len;
        paraFormatDataIt++; // To the next one, please!
    }
    // Add the last one if needed
    if (paraText.length()>lastPos)
    {
		TextFormatting format(lastPos,paraText.length()-lastPos,11,50,"times",false,false,0);
        FormatData formatData(format);
        paraFormatDataList.append(formatData);
    }
}

// ProcessParagraphData () mangles the pure text through the
// formatting information stored in the FormatData list and prints it
// out to the export file.

void ProcessParagraphData ( QString                     &paraText,
                            QValueList<FormatData>      &paraFormatDataList,
                            QValueList<AnchoredInsert>  &anchoredInsertList,
                            QString                     &outputText          )
{
#if !INSERT_TABLE_IN_PARA
    QValueList<AnchoredInsert> tmpAnchoredInsertList;
#endif

    if ( paraText.length () > 0 )
    {
        QValueList<FormatData>::Iterator  paraFormatDataIt;

        for ( paraFormatDataIt = paraFormatDataList.begin ();
              paraFormatDataIt != paraFormatDataList.end ();
              paraFormatDataIt++ )
        {
            switch ( (*paraFormatDataIt).id )
            {
               case 1:
                  {

                  // create rtf markup to handle font sizes
                  outputText += "{\\fs";;  // begin font size markup
                  outputText += QString::number( 2 * (*paraFormatDataIt).text.fontSize);


                  if ( (*paraFormatDataIt).text.fontWeight >= 70 )
                      outputText += "\\b";

                  if ( (*paraFormatDataIt).text.italic )
                      outputText += "\\i";

                  if ( (*paraFormatDataIt).text.underline )
                      outputText += "\\u";

                  // create the font markup for the specified font
                  outputText += fontMarkup( (*paraFormatDataIt).text.fontName,
                                            fontTable, fontHeader);

                  outputText += paraText.mid ( (*paraFormatDataIt).text.pos, (*paraFormatDataIt).text.len )
                                        .replace ( QRegExp ( "&amp;" ), "&" )
                                        .replace ( QRegExp ( "&lt;" ), "<"  )
                                        .replace ( QRegExp ( "&gt;" ), ">"  )
                                        .replace ( QRegExp( QString(QChar(ctrla))), "" );

                  outputText = escapeRTFsymbols( outputText );

                  outputText += " } "; // delimit font markup

                  if ( (*paraFormatDataIt).text.underline )
                      outputText += "\\u0";  // delimit underline

                  if ( (*paraFormatDataIt).text.italic )
                      outputText += "\\i0";  // delimit italics

                  if ( (*paraFormatDataIt).text.fontWeight >= 70 )
                      outputText += "\\b0"; // delimit bold font

                  outputText += " } ";  // delimit font size command

                  }  // end case 1:
                  break;

               case 2:   // pictures
#if 0
                  kdError (KDEBUG_RTFFILTER) << "Debug - picture name: "
                                                 << (*paraFormatDataIt).pictureAnchor.key
                                                 << endl;
#endif

#if INSERT_TABLE_IN_PARA

                  anchoredInsertList.prepend ( AnchoredInsert ( Picture ( (*paraFormatDataIt).pictureAnchor.key ),
                                                                outputText.length ()                               ) );

#else
                  tmpAnchoredInsertList << AnchoredInsert ( Picture ( (*paraFormatDataIt).pictureAnchor.key ), 0 );
#endif
                  break;

               case 6:   // tables
#if INSERT_TABLE_IN_PARA

                  anchoredInsertList.prepend ( AnchoredInsert ( Table ( (*paraFormatDataIt).tableAnchor.name ),
                                                                outputText.length ()                            ) );

#else
                  tmpAnchoredInsertList << AnchoredInsert ( Table ( (*paraFormatDataIt).tableAnchor.name ), 0 );
#endif

                  break;

               default:
                  kdError (KDEBUG_RTFFILTER) << "Unhandled format id "
                                                 << (*paraFormatDataIt).id << "!" << endl;
           }
        }
    }


#if !INSERT_TABLE_IN_PARA
    QValueList<AnchoredInsert>::Iterator anchoredInsert;

    for ( anchoredInsert = tmpAnchoredInsertList.begin ();
          anchoredInsert != tmpAnchoredInsertList.end ();
          anchoredInsert++ )
    {
        (*anchoredInsert).pos = outputText.length ();
        anchoredInsertList.prepend (*anchoredInsert);
    }
#endif
}  // end ProcessParagraphData

/***************************************************************************/

void CloseItemizedList ( DocData  &docData)
{
    if ( docData.bulletList )
    {
        docData.bulletList = false;
    }
}  // end CloseItemizedList

/***************************************************************************/

void CloseEnumeratedList ( DocData  &docData)
{
    if ( docData.enumeratedList )
    {
        docData.enumeratedList = false;
    }
}  // end CloseEnumeratedList

/***************************************************************************/

void CloseAlphabeticalList ( DocData  &docData)
{
    if ( docData.alphabeticalList )
    {
        docData.alphabeticalList = false;
    }
}  // end CloseAlphabeticalList

/***************************************************************************/

void CloseLists ( DocData &docData)
{
    CloseItemizedList ( docData );
    CloseEnumeratedList ( docData );
    CloseAlphabeticalList ( docData );
}  // end CloseLists


/***************************************************************************/


void OpenArticleUnlessHead1 ( DocData  &docData )
{
    if ( !docData.head1 && !docData.article )
    {
        docData.article = true;
    }
}  // end OpenArticleUnlessHead1

//***************************************************************************/

void CloseHead1AndArticle ( DocData  &docData )
{
    CloseHead2 ( docData );

    if ( docData.article )
    {
        docData.article = false;
    }

    if ( docData.head1 )
    {
        docData.head1 = false;
    }
} // end CloseHead1AndArticle

//***************************************************************************/

void CloseHead3 ( DocData  &docData )
{
    CloseLists ( docData );

    if ( docData.head3 )
    {
        docData.head3 = false;
    }
}  // end CloseHead3()

//***************************************************************************/

void CloseHead2 ( DocData  &docData )
{
    CloseHead3 ( docData );

    if ( docData.head2 )
    {
        docData.head2 = false;
    }
}  // end CloseHead2()



//***************************************************************************/

// ProcessPictureData () takes the available picture data, makes a
// copy of the image file into *.sgml.d/pictures/*.* from KoStore
// pictures/*.*, and creates the necessary Latex tags for it.
//***************************************************************************/

/*void ProcessPictureData ( Picture  &picture,
                          int       picturePos,
                          QString  &storeFileName,
                          QString  &exportFileName,
                          QString  &outputText  )  */
void ProcessPictureData (  )
{
// temporary until a proper function can be written
return;
/*
#if 0
    kdError (KDEBUG_RTFFILTER) << "Debug - picture " << picture.name
                                   << " is stored as " << picture.koStoreName << endl;
#endif

    KoStore store (storeFileName, KoStore::Read);


    if ( store.open (picture.koStoreName) )
    {
        QByteArray byteArray = store.read ( store.size () );
        store.close ();
                                                                                     ParaLayout layout;  // structure for paragrapl layout data

        QFileInfo fileInfo (exportFileName);
        QDir dir ( fileInfo.dirPath () );
        QString subDirName = fileInfo.fileName () + ".d";

        if ( !dir.exists (subDirName) )
        {
            dir.mkdir (subDirName);
        }

        dir.cd (subDirName);

        if ( !dir.exists ("pictures") )
        {
            dir.mkdir ("pictures");
        }

        QString pictureFileName = dir.filePath (picture.koStoreName);

        QFile pictureFile (pictureFileName);

        if ( pictureFile.open (IO_WriteOnly) )
        {
            pictureFile.writeBlock ( byteArray, byteArray.size () );

            QString pictureText;

#if TABLES_WITH_TITLES
            pictureText += "<FIGURE>\n";

#if 1
            pictureText += "  <TITLE>" + picture.name + "</TITLE>\n";
#else
            pictureText += "  <TITLE></TITLE>\n";
#endif
#else
            pictureText += "<INFORMALFIGURE>\n";
#endif
            pictureText += "  <MEDIAOBJECT>\n";
            pictureText += "    <IMAGEOBJECT>\n";
            pictureText += "      <IMAGEDATA FILEREF=\"" + pictureFileName + "\">\n";
            pictureText += "    </IMAGEOBJECT>\n";
            pictureText += "  </MEDIAOBJECT>\n";
#if TABLES_WITH_TITLES
            pictureText += "</FIGURE>\n";
#else
            pictureText += "</INFORMALFIGURE>\n";
#endif

            outputText.insert (picturePos, pictureText);
        }
        else
        {
            pictureFile.close ();

            kdError (KDEBUG_RTFFILTER) << "Unable to open picture file " << pictureFileName << "!" << endl;
        }
    }
    else
    {
        store.close ();

        kdError (KDEBUG_RTFFILTER) << "Unable to open KoStore file " << picture.koStoreName << "!" << endl;
    }
*/
} // end ProcessPicturnDats


/***************************************************************************/

// ProcessTableData () takes the table data and creates the necessary
// Latex markup..

void ProcessTableData ( Table   &table,
                        int     tablePos,
                        QString &outputText )
{

    // RTF tables are generated a row at a time with row by row markup
    // RTF cell borders (if any) must be a solid line.

    QString tableText; // table markup
    QString cellText;  // portion of table row markup
    QString text;  // temporary text holding string


    int currentRow = -1;

    QValueList<TableCell> ::Iterator cell;

    // Create a new table.from the list of table cells
    for ( cell = table.cellList.begin ();
          cell != table.cellList.end ();
          cell++ )
    {
        if ( (*cell).row != currentRow )  // first or new row
        {
            if ( currentRow >= 0 )  // indicates a new row
            {
                if(currentRow > 0) // if not first row end previous row
                   {
                   tableText += cellText;
                   tableText += "\\pard \\intbl \row\n"; // delimit previous row
                   }
                tableText += "\\trowd \\trgaph";  // start new row
                // insert column gap (1/2 in twips
                tableText += "\\trleft";  // left position of table row
                // insert left position

            }

            currentRow = (*cell).row;

        }
        // The following produces RTF markup for cell borders
        if((*cell).right.style >= 0)  // right border
           {
           tableText += "\\clbrdrr\\brdrs\\brdrw";
           int num = (*cell).left.width * 20;
           tableText += QString::number( num );
           }
        if((*cell).left.style >= 0)  // left border
           {
           tableText += "\\clbrdrl\\brdrs\\brdrw";
           int num = (*cell).left.width * 20;
           tableText += QString::number( num );
           }
        if((*cell).top.style >= 0)  // top border
           {
           tableText += "\\clbrdrt\\brdrs\\brdrw";
           int num = (*cell).left.width * 20;
           tableText += QString::number( num );
           }
        if((*cell).bottom.style >= 0)  // bottom border
           {
           tableText += "\\clbrdrb\\brdrs\\brdrw";
           int num = (*cell).left.width * 20;
           tableText += QString::number( num );
           }

        // cellx  - right position of cell in twips = 1/20 point
        tableText += "cellx";
        tableText += // right position
        cellText += (*cell).text;  // change this to escape and convert unicode
        cellText += "\\cell";  // cell text end mark

    } // end for(cell = ...

    if ( currentRow >= 0 )
    {
        tableText += cellText;
        tableText += "\\pard \\intbl \\row\n";  // delimit last row
    }


    // escape the table text
    text = tableText.replace( QRegExp( "&amp;"),              "&" )
                    .replace( QRegExp( "&lt;") ,              "<" )
                    .replace( QRegExp( "&gt;") ,              ">" )
                    .replace( QRegExp( QString(QChar(ctrla))), "" );
    tableText = escapeRTFsymbols( tableText );

    // insert the table in the location specified by the anchor
    outputText.insert (tablePos, tableText);
} // end ProcessTableData()



/***************************************************************************/

// ProcessStoreFile () takes a KoStore file and starts parsing it as
// a KWord XML file.
/***************************************************************************/

bool ProcessStoreFile ( QString   storeFileName,
                        QString   storeCompFileName,
                        void     (*processor)(QDomNode, void *, QString &),
                        QString   exportFileName,
                        QString  &outputBuffer )
{
    KoStore store (storeFileName, KoStore::Read);

    if ( !store.open (storeCompFileName) )
    {
        store.close ();

        kdError (KDEBUG_RTFFILTER) << "Unable to open KoStore " << storeFileName
                                       << " component " << storeCompFileName << "!" << endl;
        return false;
    }

    QByteArray byteArrayIn = store.read ( store.size () );
    store.close ();

    QString stringBufIn = QString::fromUtf8 ( (const char *) byteArrayIn, byteArrayIn.size () );

    QDomDocument qDomDocumentIn;
    qDomDocumentIn.setContent (stringBufIn);

#if 0
    kdError (KDEBUG_RTFFILTER) << "DOM document type of KoStore " << storeFileName
                                    << " component " << storeCompFileName
                                    << " is " << qDomDocumentIn.doctype ().name () << "." << endl;
#endif

    QDomNode docNode = qDomDocumentIn.documentElement ();

    FilterData filterData;
    filterData.storeFileName = storeFileName;
    filterData.exportFileName = exportFileName;
        kdError (KDEBUG_RTFFILTER) << " Starting processor() " << endl;
    processor (docNode, &filterData, outputBuffer);

    return true;
}  // end ProcessStoreFile()

/***************************************************************************/
// The virtual function called by the file dialog to export the RTF file
// It is the starting point in the file converson process.

bool RTFExport::filter ( const QString  &filenameIn,
                         const QString  &filenameOut,
                         const QString  &from,
                         const QString  &to,
                         const QString  &             )
{

    if ( to != "text/rtf" || from != "application/x-kword" )
    {
        return false;
    }

    QString stringBufOut;
    QString mainDoc;
    QString docInf;


    ProcessStoreFile ( filenameIn, "documentinfo.xml", &ProcessDocumentInfoTag, filenameOut, docInf );
        kdError (KDEBUG_RTFFILTER) << " Processed docinfo " << endl;
    ProcessStoreFile ( filenameIn, "root",             ProcessDocTag,          filenameOut, mainDoc );
        kdError (KDEBUG_RTFFILTER) << " Processed DOC tag " << endl;

    // Compose the RTF file
    // Begin rtf document
    stringBufOut += "{\\rtf1\\ansi \\deff5\\deflang1033\n";

    stringBufOut += fontHeader; // insert the font table into the header

    // insert the color table
    stringBufOut += "{\\colortbl;\\red0\\green0\\blue0;\\red0\\green0\\blue255;\\red0\\green255\\blue255;\\red0\\green255\\blue0;\\red255\\green0\\blue255;\\red255\\green0\\blue0;\n";
    stringBufOut += "\\red255\\green255\\blue0;\\red255\\green255\\blue255;\\red0\\green0\\blue128;\\red0\\green128\\blue128;\\red0\\green128\\blue0;\\red128\\green0\\blue128;\\red128\\green0\\blue0;\\red128\\green128\\blue0;\\red128\\green128\\blue128;\\red192\\green192\\blue192;}\n";


    // Insert the string from docinfoExport
    stringBufOut += docInf;

    stringBufOut += "\\widowctrl\\ftnbj\\aenddoc\\formshade \\fet0\\sectd\n";
    stringBufOut += "\\linex0\\endnhere {\\*\\pnseclvl1\\pnucrm\\pnstart1\\pnindent720\\pnhang{\\pntxta .}}{\\*\\pnseclvl2\\pnucltr\\pnstart1\\pnindent720\\pnhang{\\pntxta .}}{\\*\\pnseclvl3\\pndec\\pnstart1\\pnindent720\\pnhang{\\pntxta .}}{\\*\\pnseclvl4\\pnlcltr\\pnstart1\\pnindent720\\pnhang\n";
    stringBufOut += "{\\pntxta )}}{\\*\\pnseclvl5\\pndec\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}{\\*\\pnseclvl6\\pnlcltr\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}{\\*\\pnseclvl7\\pnlcrm\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}{\\*\\pnseclvl8\n";
    stringBufOut += "\\pnlcltr\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}{\\*\\pnseclvl9\\pnlcrm\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}\n";

    // Markup from main parse
    stringBufOut += mainDoc;

    stringBufOut += " }";   // file terminator


    QFile fileOut (filenameOut);

    if ( !fileOut.open (IO_WriteOnly) )
    {
        fileOut.close ();

        kdError (KDEBUG_RTFFILTER) << "Unable to open output file!" << endl;
        return false;
    }

    fileOut.writeBlock ( (const char *) stringBufOut.local8Bit (), stringBufOut.length () );
    fileOut.close ();

    return true;
}


/***************************************************************************/

QString fontMarkup(QString fontName, QValueList< FontTable > &fontTable,
                   QString &fontHeader)

   {
   int counter;  // counts position in font table
   QString font;
   QValueList < FontTable > ::Iterator fontTableIt;

      counter = 1;  // initialize table entry counter

      // search font table for this font
      for( fontTableIt =  fontTable.begin();
           fontTableIt != fontTable.end();
           fontTableIt++ )
         {

         if((*fontTableIt).fontName == fontName)  // check for match
            {
            font = "{\\f";  //markup for font selection
            font += QString::number(counter);
            fontHeader += font;  // insert font number into table
            return font;
            }  // end if((*fontTableIt).fontName == fontName)
         counter++;  // increment counter
         }  // end for()

      // font not in table - add it to the table
      if(fontName == "courier"  ||
         fontName == "fixed" ||
         fontName == "lucidia typewriter" ||
         fontName == "terminal" )
         {

         font=fontTableMarkup(fontName, fontTable, fontHeader,modern, counter);
         return font;

         } // end if(fontName == "courier"||

      if(fontName == "chancery I")
         {

         font = fontTableMarkup(fontName, fontTable, fontHeader, script,counter);
         return font;

         } // end if(fontName == "chancery I"||

      if(fontName == "dingbats"  ||
         fontName == "standard symbols I" ||
         fontName == "symbol" )
         {

         font = fontTableMarkup(fontName, fontTable, fontHeader, tech,counter);
         return font;

         } // end if(fontName == "dingbats"||

      if(fontName == "clean"  ||
         fontName == "gothic" ||
         fontName == "lucidia" ||
         fontName == "gothic I" ||
         fontName == "lucidiabright" ||
         fontName == "mincho" ||
         fontName == "nil" ||
         fontName == "nimbo sans I" ||
         fontName == "sung ti" )
         {

         font = fontTableMarkup(fontName, fontTable, fontHeader, swiss,counter);
         return font;

         } // end if(fontName == "clean"||

      // default is roman
      font = fontTableMarkup(fontName, fontTable, fontHeader, roman,counter);
      return font;

   }  // end fontMarkup()

QString fontTableMarkup(QString fontName, QValueList< FontTable > &fontTable,
                   QString &fontHeader, FontType fontType, int counter)
   {

   FontTable entry; // for new entries
   QString font;  // Holds RTF markup for the font

   entry.fontName = fontName;  // enter font into font table
   entry.fontType = fontType;
   fontTable << entry;

   font = "{\\f";  //markup for font definition or selection
   font += QString::number(counter);
   fontHeader += font;  // insert font number into the font table

   // define the font in the font table
   switch (fontType)
      {
      case roman:
         {
         fontHeader += "\\froman";
         }
      case swiss:
         {
         fontHeader += "\\fswiss";
         }
      case modern:
         {
         fontHeader += "\\fmodern";
         }
      case script:
         {
         fontHeader += "\\fscript";
         }
      case decor:
         {
         fontHeader += "\\fdecor";
         }
      case tech:
         {
         fontHeader += "\\ftech";
         }
      }  // end switch
   fontHeader += "\\fcharset0\\fprq2 "; // font definition
   fontHeader += fontName;
   fontHeader += " ;}"; // end font table entry
   return font;  // this is the font selection markup

   }  // end fontTableMarkup()

QString escapeRTFsymbols( QString text)
   {
   QString text1;

   text1 = text.replace( QRegExp( "\\"), "\\\\");
   text1 = text1.replace( QRegExp( "{"), "\\{");
   text1 = text1.replace( QRegExp( "}"), "\\}");
   return text1;
   }  // end escapeRTFsymbols()


/***************************************************************************/

//  The following function generates the common RTF markup at beginning of all
//  list items.
QString listStart( QString font, int fontSize, QString listMarker)
   {
   QString markup;
   markup += "\n\\par {\\pntext\\pard\\plain";
   markup += font;
   markup += "\\fs";
   markup += QString::number((2 * fontSize));
   markup += " ";
   markup += listMarker;
   markup += " \\tab}";
   return markup;

   } // end listStart()

// This function completes the markup for a bullet item or the first item
// of a numbered list. All types of lists are done - numeric, alphabetical,
// roman numerals in upper or lower case.
QString listMarkup( int firstIndent,int listType, int startNumber,
                    QString preceedingText, QString followingText)
   {

   QString markup;

   markup += "\\pard \\fi-";
   markup += firstIndent;
   markup += "\\li";
   markup += firstIndent;
   markup += "{\\*\\pn ";
   switch (listType)
      {
      case 1:  // Numeric
         {
         markup += "\\pndec";
         break;
         }
      case 2: // lower case alphabetical
         {
         markup += "\\pnlctr";
         break;
         }
      case 3:  // upper case alphabetical
         {
         markup += "\\pnuctr";
         break;
         }
      case 4: // lower case roman
         {
         markup += "\\pnlcrm";
         break;
         }
      case 5:  // upper case roman
         {
         markup += "\\pnucrm";
         break;
         }
      case 6:  // bullet list
         {
         markup += "\\pnlvlblt\\pnf1";
         break;
         }
      default:
         markup += "\\pnlvlblt\\pnf1";

      }  // end switch
   markup += "\\pnstart";
   markup += startNumber;
   markup += "\\pnindent";
   markup += firstIndent;
   markup += "\\pnhang";

   if( preceedingText != "" )
      {
      markup += "\\textb ";
      markup += preceedingText;
      }
   if( followingText != "" )
      {
      markup += "\\texta ";
      markup += followingText;
      }

   markup += "}}";
   return markup;

   }  // end ListMarkup()


/***************************************************************************/
// The following function generates paragraph number markup required
// for older rtf readers. It supplements the markup for automatic numbering.
// The markup iw in the form of A., a., 1., 1.1. and so on.
// Globals used are paraNumber[] indicating the current paragraph number
// and type[] which is the type of list (numeric, alpha, etc.)

QString paragraphNumber( bool listStarted, int depth, int start )
   {

   QString markup; // string for paragraph number
   int i;  // index

   markup = "";

   if( !listStarted )
      {
      switch (type[depth])
         {
         case numeric:  // decimal numbering
            {
            paraNumber[depth] = start;
            break;
            }
         case alpha:  // lower case alpha
            {
            paraNumber[depth] = 'a' + start;
            break;
            }
         case ALPHA:  // upper case alpha
            {
            paraNumber[depth] = 'A' + start;
            break;
            }
         case rom: // lower case roman - treat as decimal
            {
            paraNumber[depth] = start;
            break;
            }
         case ROMAN: // upper case roman - treat as decimal
            {
            paraNumber[depth] = start;
            break;
            }
         }  // end switch

      }  // end if( !listStarted )

      else paraNumber[depth]++;

      for( i = 0; i <= depth; i++ )
         {
         if( type[i] == alpha || type[i] == ALPHA)
            {
            markup += paraNumber[i];
            }
         else
            {
            markup += QString::number((int) paraNumber[i]);
            }

         if( i != depth ) markup += ".";  // period between number levels

         }  // end for

      return markup;

   }  // end paragraphNumber()

/***************************************************************************/
// The following is a function called by the document info processor
// once per documentinfo.xml file. It marks up the info into RTF format.

QString ProcessDocumentData ( BookInfo bookInfo )
   {

   bookMarkup = "{\\info ";  // string of document information markup


   if( bookInfo.title != "" ) bookMarkup += ("{\\title " + bookInfo.title + "}");
   if( bookInfo.fullName != "" )
      {
      bookMarkup += ("{\\author "   + bookInfo.fullName + "}");
      bookMarkup += ("{\\operator " + bookInfo.fullName  + "}");
      }
   if( bookInfo.abstract != "" ) bookMarkup +=("{\\subject " +
                                             bookInfo.abstract + "}");
   if( bookInfo.company != "" ) bookMarkup += ("\\company " +
                                             bookInfo.company + "}");
   bookMarkup += "}";

   } // end ProcessDocumnetData


/***************************************************************************/

// The following function creates markup for the paper size and margins
// This function uses the global variable pageMarkup
void paperSize( PaperAttributes &paper, PaperBorders &paperBorders  )
   {
   int num;  // for number conversion
   double dnum;  // dor number conversion

   if( paper.width > 0 )
       {
       pageMarkup += "\\paperw";
       dnum = 56.6929 * (double) paper.width;
       num = (int) dnum;
       pageMarkup += QString::number( num );
       }
   if( paper.height > 0 )
       {
       pageMarkup += "\\paperh";
       dnum = 56.6929 * (double) paper.height;
       num = (int) dnum;
       pageMarkup += QString::number( num );
       }
   if( paper.orientation == 1 )
       {
       pageMarkup += "\\landscape";
       }
   if( paperBorders.mmLeft > 0.0 )
       {
       pageMarkup += "\\margl";
       dnum = 56.6929 * paperBorders.mmLeft;
       num = (int) dnum;
       pageMarkup += QString::number( num );
       }
   if( paperBorders.mmRight > 0.0 )
       {
       pageMarkup += "\\margr";
       dnum = 56.6929 * paperBorders.mmRight;
       num = (int) dnum;
       pageMarkup += QString::number( num );
       }
   if( paperBorders.mmTop > 0.0 )
       {
       pageMarkup += "\\margt";
       dnum = 56.6929 * paperBorders.mmTop;
       num = (int) dnum;
       pageMarkup += QString::number( num );
       }
   if( paperBorders.mmBottom > 0.0 )
       {
       pageMarkup += "\\margb";
       dnum = 56.6929 *  paperBorders.mmBottom;
       num = (int) dnum;
       pageMarkup += QString::number( num );
       }

   }  // end paperSize()

/***************************************************************************/
// This is a virtual function called once for each paragraph
// by the PARAGRAPH tag processor in the kwExport class

void ProcessParagraph ( QString &paraText,
                        QValueList<FormatData> &paraFormatDataList,
                        QString &outputText,
                        ParaLayout &layout,
                        DocData *docData )
{

    QString listMarker;  // used to create list markups
    QString font;  // used for para numbering font markup
    QValueList<FormatData>::Iterator it;
    int fontSize;  // font size for paragraph numbers
    QString fontName;  // font name for paragraph numbers
    QString paraLayout;
    int firstIndent;  // indentation of the first line
    int leftIndent;  // indentation of the feft side of the paragraph

    CreateMissingFormatData(paraText,paraFormatDataList);

    outputText += "\n\\pard ";  // set paragraph settings to defaults

    // indent the paragraph
    firstIndent = (int)(layout.mmFirst * 56.6929); // calculate indent in twips
    leftIndent = (int)(layout.mmLeft * 56.6929); // calculate indent in twips

    // get the font name and size for list numbering
    it = paraFormatDataList.begin();
    fontName = (*it).text.fontName;
    fontSize = (*it).text.fontSize;

    paraLayout = layout.layout;   // copy layout string

    if ( paraLayout == "Head 1" )
    {
        CloseHead1AndArticle (*docData );

        // generate the paragragraph number string
        listMarker = paragraphNumber( (*docData).head1, layout.depth,
                                      layout.start );
        // create the font markup for a head1 heading
        font = fontMarkup( "helvetica", fontTable, fontHeader );
        // Generate the beginning paragraph numbering info
        outputText += listStart( font, 24, listMarker);

        if( !(*docData).head1 )
           {
           // Generate the additional markup for automatic numbering - once only
           outputText += listMarkup( firstIndent,
                                  1, layout.start,
                                  layout.lefttext, layout.righttext );
           (*docData).head1 = true;
           }
        // extract heading text
        ProcessParagraphData ( paraText, paraFormatDataList, docData->anchoredInsertList, outputText );
    }
    else if ( paraLayout == "Head 2" )
    {
        CloseHead2 (*docData);

        // generate the paragragraph number string
        listMarker = paragraphNumber( (*docData).head2, layout.depth,
                                      layout.start );
        // create the font markup for a head1 heading
        font = fontMarkup( "helvetica", fontTable, fontHeader );
        // Generate the beginning paragraph numbering info
        outputText += listStart( font, 16, listMarker);

        if( !(*docData).head2 )
           {
           // Generate the additional markup for automatic numbering - once only
           outputText += listMarkup( firstIndent,
                                  1, layout.start,
                                  layout.lefttext, layout.righttext );
           (*docData).head2 = true;
           }


        // extract heading text
        ProcessParagraphData ( paraText, paraFormatDataList, docData->anchoredInsertList, outputText );
    }
    else if ( paraLayout == "Head 3" )
    {
        CloseHead3 (*docData);

        // generate the paragragraph number string
        listMarker = paragraphNumber( (*docData).head3, layout.depth,
                                      layout.start );
        // create the font markup for a head1 heading
        font = fontMarkup( "helvetica", fontTable, fontHeader );
        // Generate the beginning paragraph numbering info
        outputText += listStart( font, 12, listMarker);

        if( !(*docData).head3 )
           {
           // Generate the additional markup for automatic numbering - once only
           outputText += listMarkup( firstIndent,
                                 1, layout.start,
                                 layout.lefttext, layout.righttext );
           (*docData).head3 = true;
           }


        // extract heading text
        ProcessParagraphData ( paraText, paraFormatDataList, docData->anchoredInsertList, outputText );
    }
    else if ( paraLayout == "Bullet List" )
    {
        CloseEnumeratedList (*docData);
        CloseAlphabeticalList (*docData);

        OpenArticleUnlessHead1 (*docData);

        // generate the paragragraph number string
        listMarker = "\\'b7";
        // create the font markup for a bullet item
        font = fontMarkup( fontName, fontTable, fontHeader );
        // Generate the beginning paragraph numbering info
        outputText += listStart( font, fontSize, listMarker);
        // Generate the additional markup for automatic numbering - once only
        outputText += listMarkup( firstIndent ,
                                 6, layout.start,
                                 listMarker, layout.righttext );

        (*docData).bulletList = true;
        // output list item text
        ProcessParagraphData ( paraText, paraFormatDataList, docData->anchoredInsertList, outputText );
    }
    else if ( paraLayout == "Enumerated List" )
    {
        CloseItemizedList (*docData);
        CloseAlphabeticalList (*docData);

        OpenArticleUnlessHead1 (*docData);
        // generate the paragragraph number string
        listMarker = paragraphNumber( (*docData).enumeratedList, layout.depth,
                                      layout.start );
        // create the font markup for an enumerated list
        font = fontMarkup( fontName, fontTable, fontHeader );
        // Generate the beginning paragraph numbering info
        outputText += listStart( font, fontSize, listMarker);

        if ( !(*docData).enumeratedList )
        {
        // Generate the additional markup for automatic numbering - once only
        outputText += listMarkup( firstIndent ,
                                 layout.type, layout.start,
                                 layout.lefttext, layout.righttext );
            (*docData).enumeratedList = true;
        }


        // output text
        ProcessParagraphData ( paraText, paraFormatDataList, docData->anchoredInsertList, outputText );
    }
    else if ( paraLayout == "Alphabetical List" )
    {
        CloseItemizedList (*docData);
        CloseEnumeratedList (*docData);

        OpenArticleUnlessHead1 (*docData);

        // generate the paragragraph number string
        listMarker = paragraphNumber( (*docData).alphabeticalList, layout.depth,
                                      layout.start );
        // create the font markup for an enumerated list
        font = fontMarkup( fontName, fontTable, fontHeader );
        // Generate the beginning paragraph numbering info
        outputText += listStart( font, fontSize, listMarker);

        if ( !(*docData).alphabeticalList )
          {
           // Generate the additional markup for automatic numbering - once only
           outputText += listMarkup( firstIndent ,
                                 layout.type, layout.start,
                                 layout.lefttext, layout.righttext );

            (*docData).alphabeticalList = true;
           }

        ProcessParagraphData ( paraText, paraFormatDataList, docData->anchoredInsertList, outputText );
    }

    else
    {
        CloseLists (*docData);

        OpenArticleUnlessHead1 (*docData);

        if ( paraLayout != "Standard" )
        {
            kdError (KDEBUG_RTFFILTER) << "Unknown layout " + paraLayout + "!" << endl;
        }

       outputText += ("\\fi" + QString::number(firstIndent) + " ");
       outputText += ("\\li" + QString::number(leftIndent) + " ");


       // Examine the flow alignment and align the text accordingly
       if( layout.flow == "center")
          {
          outputText += "\\qc\n";
          }

       if( layout.flow == "right")
          {
          outputText += "\\qr\n";
          }
       if( layout.flow == "justify")
          {
          outputText += "\\qj\n";
          }

        ProcessParagraphData ( paraText, paraFormatDataList, docData->anchoredInsertList, outputText );
        outputText += "\n\\par"; // delimit paragraph
    }

}  // end ProcessParagraph()

