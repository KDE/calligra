/* This file is part of the KDE project
   Copyright (C) 2001 Enno Bartels <ebartels@nwn.de>

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

#include <qmessagebox.h>
#include <qstringlist.h> 
#include <qregexp.h>
#include <qlist.h> 
#include <applixspreadimport.h>
#include <applixspreadimport.moc>
#include <kdebug.h>




APPLIXSPREADImport::APPLIXSPREADImport (KoFilter *parent, const char *name) :
                     KoFilter(parent, name) 
{
}

const bool 
APPLIXSPREADImport::filter (
   const QString &fileIn, 
   const QString &fileOut,
   const QString &from, 
   const QString &to,
   const QString &) 
{

    if (to != "application/x-kspread" || from != "application/x-applixspread")
        return false;

    QFile in (fileIn);
    if (!in.open(IO_ReadOnly)) 
    {
        kdError(30502) << "Unable to open input file!" << endl;
        in.close();
        return false;
    }

    QString str;
    QList<t_mycolor> mcol;

    str += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    str += "<!DOCTYPE spreadsheet>\n";
    str += "<spreadsheet mime=\"application/x-kspread\" editor=\"KSpread\" >\n";
    str += " <paper format=\"A4\" orientation=\"Portrait\" >\n";
    str += "  <borders right=\"20\" left=\"20\" bottom=\"20\" top=\"20\" />\n";
    str += "  <head/>\n";
    str += "  <foot/>\n";
    str += " </paper>\n";
    str += " <locale positivePrefixCurrencySymbol=\"True\" negativeMonetarySignPosition=\"1\" negativePrefixCurrencySymbol=\"True\" fracDigits=\"2\" thousandsSeparator=\" \" dateFormat=\"%A, %e. %B %Y\" timeFormat=\"%H:%M:%S\" monetaryDecimalSymbol=\",\" weekStartsMonday=\"True\" currencySymbol=\"DM\" negativeSign=\"-\" positiveSign=\"\" positiveMonetarySignPosition=\"1\" decimalSymbol=\",\" monetaryThousandsSeparator=\" \" dateFormatShort=\"%d.%m.%Y\" />\n";
    str += " <map markerColumn=\"1\" activeTable=\"Table1\" markerRow=\"1\" >\n";
//      str += "  <table columnnumber=\"0\" borders=\"0\" hide=\"0\" hidezero=\"0\" firstletterupper=\"0\" grid=\"1\" formular=\"0\" lcmode=\"0\" name=\"Tabelle1\" >\n";


    // QTextStream
    QTextStream stream (&in);
    int  step  = in.size()/50;
    int  value = 0;
    int  i     = 0;
    int  rueck;
    int  pos;
    int  vers[3] = { 0, 0, 0 };
    char ccol;
    int  irow, icol;
    QString  tabctr ;  // Tab control
    QString  mystr, typestr, cellnostr, tabnostr;
    QStringList typefacetab;

    //    QStringList rclist;
    t_rc my_rc;

    // Read Headline  
    mystr = stream.readLine ();
    rueck = sscanf ((const char *) mystr.latin1() , 
                    "*BEGIN SPREADSHEETS VERSION=%d/%d ENCODING=%dBIT", 
	             &vers[0], &vers[1], &vers[2]); 
    printf ("Versions info: %d %d %d\n", vers[0], vers[1], vers[2]); 

    // Check the headline
    if (rueck <= 0)  
    {
      printf ("Header not correkt - May be it is not an applixspreadsheet file\n");  
      printf ("Headerline: <%s>\n", (const char *) mystr.latin1());

      QMessageBox::critical (0L, "Applix spreadsheet header problem",
              QString ("The Applix Spreadsheet header is not correct. "
                       "May be it is not an applix spreadsheet file! <BR>"
                       "This is the header line I did read:<BR><B>%1</B>").arg(mystr.latin1()),
			"Okay");


      return false;
    }






    while (!stream.atEnd ())
    {
        ++i;
        
        // Read one line
        mystr = stream.readLine ();

        printf ("INPUT <%s>\n", (const char *) mystr.latin1() );
 

        /**********************************************************************
         *  Looking for the colormap                                          *
         **********************************************************************/
        if (mystr.startsWith ("COLORMAP") ) 
	{
          readColormap (stream, mcol);
	}

        /**********************************************************************
         *  Looking for the typeface table                                    *
         **********************************************************************/
        else if (mystr.startsWith ("TYPEFACE TABLE") ) 
	{
          readTypefaceTable (stream, typefacetab);
	}
          
        /**********************************************************************
         *  Looking for some View-Informations                                *
         **********************************************************************/
        else if (mystr.startsWith ("View Start, Name:") ) 
	{
          readView (stream, mystr, my_rc); 
	}


        /**********************************************************************
         *   Detect ( at the first place of the Linie                         *
         **********************************************************************/
        else if (mystr[0] == '(') 
	{
	  int fg=-1, sh=-1, bg=-1; // foregound, sh=shading

          // Delete  '('
          mystr.remove (0, 1);

          int alllenght;
          QString mystrn;

          // Remember lenght of the string
          alllenght = mystr.length ();
          if (alllenght >= 80-1) 
          { 
            printf (" !!!!! Zeile <= 80 Zeichen \n");
            int ok = true;
            do
	    {
              pos = in.at ();
              mystrn = stream.readLine ();  
              if (mystrn[0] == ' ')
	      {
                mystrn.remove (0, 1);  
                mystr += mystrn;
	      }
              else
	      {
                in.at (pos);
                ok = false;
	      }
	    }
            while (ok == true); 
             
          }


          // Search for ')'
          pos = mystr.find (')'); 
          typestr = mystr.left (pos);


          // Delete typeformat infos incl. Space
          mystr.remove (0, pos+1);
          // alllenght = alllenght - pos - 1;

          // Search for ':'
          pos = mystr.find (':'); 

          // Copy cellnumber informations
          cellnostr = mystr.left (pos);

          // Delete cellnumber informations
          mystr.remove (0, pos+1);
          // alllenght = alllenght - pos - 1;


          // Split Table and Cell Number
          pos = cellnostr.find ('!'); 

          // Copy tabnumber informations
          tabnostr = cellnostr.left (pos);

          // Delete cellnumber informations
          cellnostr.remove (0, pos+1);

          sscanf ((const char *) cellnostr.latin1(), "%c%d",&ccol, &irow);
  
          // Transformat ASCII column to int column
          icol = ccol - 64;

          // Remove first whitespace 
          mystr.remove    (0, 1);
          tabnostr.remove (0, 1);


          // Replace Part for this characters: <, >, &
          mystr.replace (QRegExp ("&"), "&amp;");
          mystr.replace (QRegExp ("<"), "&lt;");
          mystr.replace (QRegExp (">"), "&gt;");


          // Replace part for Applix Characters
          int   foundSpecialCharakter;
          QChar newchar;

          do 
	  {
            // initialize
            foundSpecialCharakter = false;

            pos = mystr.find ("^");

            // is there a special character ?
            if (pos > -1 ) 
            { 
              // i have found a special character !
              foundSpecialCharakter = true;

              // translate the applix special character
              newchar = specCharfind (mystr[pos+1], mystr[pos+2]);

              // replace the character
              mystr.replace (pos, 3, newchar);
	    }
            
	  }
          while (foundSpecialCharakter == true);
        

	  // examine the typestring 
          // splitt typestring in 3 parts by an |       
          QString typeFormStr;
          QString typeCharStr;
          QString typeCellStr;

          int pos1 = typestr.find    ("|");
          int pos2 = typestr.findRev ("|");


          typeFormStr = typestr.left  (pos1);

          typeCharStr = typestr.mid   (pos1+1,  pos2 - pos1 - 1);

          typeCellStr = typestr.right (typestr.length() - pos2 - 1);
        
          // Is it a new table
          if (tabctr != tabnostr)
          {
	     // is it not the first table
             if (!(tabctr.isNull()))  str += "  </table>\n";

             str += "  <table columnnumber=\"0\" borders=\"0\" hide=\"0\" hidezero=\"0\" firstletterupper=\"0\" grid=\"1\" formular=\"0\" lcmode=\"0\" name=\"";
             str += tabnostr;
	     str += "\" >\n";

             tabctr = tabnostr;

             // Searching for the rowcol part and adding to the hole string
             pos = my_rc.tabname.findIndex (tabnostr);
             if (pos > -1) str += my_rc.rc[pos];
          }

          printf (" -DATA: Text:<%s>  Tab:<%s> <%s> %c  %d    <%s> <%s> <%s>\n", 
                  (const char *) mystr.latin1(),  
                  (const char *) tabnostr.latin1(), 
                  (const char *) cellnostr.latin1(), ccol, irow,
                  (const char *) typeFormStr.latin1(), 
                  (const char *) typeCharStr.latin1(), 
                  (const char *) typeCellStr.latin1());


          /********************************************************************
           * examine charakter format String, splitt it up in basic parts     *
           ********************************************************************/
          QStringList typeCharList; 
          int bold=0, italic=0, underline=0, nn=0, fontsize=12, fontnr=-1;

          typeCharList = QStringList::split (',', typeCharStr);

          for (QStringList::Iterator it = typeCharList.begin(); 
               it != typeCharList.end(); ++it ) 
	  {
	    // Output
            printf ("   Char (%2d)   >%s< ", 
                    nn, (*it).latin1() );
            nn++;

            if      ((*it) == "B")  
            {
              printf ("   = bold\n");
              bold  = 1;  
	    }
            else if ((*it) == "I")  
            {
              printf ("   = italic\n");
              italic = 1;  
	    }
            else if ((*it) == "U")  
            {
              printf ("   = underline\n");
              underline = 1;  
	    }
            else if ((*it).startsWith("FG") )
	    {
              sscanf ((const char *) (*it).latin1(), "FG%d", &fg);
              printf ("  = Colornr %d\n", fg); 
	    }
            else if ((*it).startsWith("TF") )
	    {
              sscanf ((const char *) (*it).latin1(), "TF%d", &fontnr);
              printf ("  = Font (%d) %s\n", 
                      fontnr, (const char *) typefacetab[fontnr].latin1()); 
	    }
            else if ((*it).startsWith("P") )
	    {
              sscanf ((const char *) (*it).latin1(), "P%d", &fontsize);
              printf ("   = Fontsize %d\n", fontsize); 
	    }
            else
	    {
              printf ("   = ???\n"); 
	    }
	  }
          printf ("\n");



          /********************************************************************
           * examine pos format String, splitt it up in basic parts           *
           ********************************************************************/
          QStringList typeFormList;
          int align = 0, valign=0;

          typeFormList = QStringList::split (',', typeFormStr);
          nn=0;
          for (QStringList::Iterator it = typeFormList.begin(); it != typeFormList.end(); ++it ) 
          {
	    // Output
            printf ("   Type (%2d)   >%s< ", 
                    nn, (*it).latin1() );
            nn++;
            // Grep horizontal alignment
            if      ( (*it) == "1")  
            { 
              printf (" = left align\n");
              align = 1; // left 
	    }
            else if ( (*it) == "2")    
            { 
              printf (" = right align\n");
              align = 3; // right
	    }
            else if ( (*it) == "3")    
            { 
              printf (" = center align\n");
              align = 2; // center
	    }

            // Grep verticale alignment
            else if ( (*it) == "VT")   
            { 
              printf (" = top valign\n");
              valign =  1; // top
	    }
            else if ( (*it) == "VC")   
            { 
              printf (" = center valign\n");
              valign =  0; // center - default (2)
	    }
            else if ( (*it) == "VB")   
            { 
              printf (" = bottom valign\n");
              valign =  3; // bottom
	    }
            else
	    {
              printf ("   = ???\n"); 
	    }
	  }


          /********************************************************************
           * examine cell format String, splitt it up in basic parts          *
           ********************************************************************/
          QStringList typeCellList;
          int topPenWidth=0, bottomPenWidth=0, leftPenWidth = 0, rightPenWidth = 0, sh_bg=-1, fg_bg=-1;
          int topPenStyle=0, bottomPenStyle=0, leftPenStyle = 0, rightPenStyle = 0;
          int brushstyle=0,     brushcolor=1;
          int topbrushstyle=0,  topbrushcolor=1, topfg_bg=1;
          int leftbrushstyle=0, leftbrushcolor=1, leftfg_bg=1;
          int rightbrushstyle=0, rightbrushcolor=1, rightfg_bg=1;
          int bottombrushstyle=0, bottombrushcolor=1, bottomfg_bg=1;
  
          typeCellList = QStringList::split (',', typeCellStr);

          nn=0;
          for ( QStringList::Iterator it = typeCellList.begin(); it != typeCellList.end(); ++it ) 
          {
	    // Output
            printf ("   Cell (%2d)   >%s< ", 
                    nn, (*it).latin1() );
            nn++;

	    if ((*it)[0] == 'T')
	    {
	      printf (" = top    "); 
              transPenFormat ((*it), &topPenWidth, &topPenStyle);

              if ((*it).length() > 2) 
	      {
		(*it).remove (0, 2);
                filterSHFGBG ((*it), &topbrushstyle, &topbrushcolor, &topfg_bg);
    	      }

	    }

	    else if ( (*it)[0] == 'B')
	    {
	      printf (" = bottom "); 
              transPenFormat ((*it), &bottomPenWidth, &bottomPenStyle);

              if ((*it).length() > 2) 
	      {
		(*it).remove (0, 2);
                filterSHFGBG ((*it), &bottombrushstyle, &bottombrushcolor, &bottomfg_bg);
    	      }
	    } 

	    else if ( (*it)[0] == 'L')
	    {
              printf (" = left   "); 
              transPenFormat ((*it), &leftPenWidth, &leftPenStyle);
 
              if ((*it).length() > 2) 
	      {
		(*it).remove (0, 2);
                filterSHFGBG ((*it), &leftbrushstyle, &leftbrushcolor, &leftfg_bg);
    	      }
	    }

	    else if ( (*it)[0] == 'R')
	    {
              printf (" = right  "); 
              transPenFormat ((*it), &rightPenWidth, &rightPenStyle);
   
              if ((*it).length() > 2) 
	      {
		(*it).remove (0, 2);
                filterSHFGBG ((*it), &rightbrushstyle, &rightbrushcolor, &rightfg_bg);
    	      }
              printf ("\n");
	    }

            else if ( ((*it).startsWith ("SH")) || ((*it).startsWith ("FG")) ||
                      ((*it).startsWith ("BG")) )
	    {
              printf (" = ");
              filterSHFGBG ((*it), &brushstyle, &fg_bg, &brushcolor);
	    }

            else 
	    {
              printf ("   = ???\n"); 
	    }
            
          }





          QString col;
 
          // create kspread fileformat output
          str += "   <cell row=\"" + QString::number (irow) + "\"";
          str += " column=\""      + QString::number (icol) + "\">\n"; 
	  if (bold == 1  || italic == 1 || underline == 1 || 
              align != 0 || valign != 0 ||
              topPenStyle  != 0  || bottomPenStyle != 0 || 
              leftPenStyle != 0  || rightPenStyle  != 0 || fg !=-1 || fg_bg != -1 ||
              fontsize != 12 || brushstyle != 0)
	  {
	    str += "    <format";
            if (brushstyle != 0)
            { 
               str += " brushstyle=\""  + QString::number(brushstyle) + "\" ";
               str += " brushcolor=\"";
               str += writeColor (mcol.at(brushcolor));
               str += "\"";
	    }

            if (align   != 0)  str += " align=\""  + QString::number(align) + "\" ";
            if (valign  != 0)  str += " alignY=\"" + QString::number(valign) + "\" ";
            if (fg_bg != -1) 
            {
              str += " bgcolor=\"";
              str += writeColor (mcol.at(fg_bg));
              str += "\" ";
	    }
            str += ">\n"; 

            // Font color
            if (fg != -1) 
            {
              str += "    <pen width=\"0\" style=\"1\" color=\"";  
              str += writeColor (mcol.at(fg));  
	      str += "\" />\n"; 
	    } 

            // Left border
            if (leftPenWidth > 0)
            {
              str += "    <left-border>\n";
              col = writeColor (mcol.at(leftfg_bg));
              writePen (str, leftPenWidth, leftPenStyle, col);
              str += "    </left-border>\n";
	    }

            // Right border
            if (rightPenWidth > 0)
            {
              str += "    <right-border>\n";
              col = writeColor (mcol.at(rightfg_bg));
              writePen (str, rightPenWidth, rightPenStyle, col);
              str += "    </right-border>\n";
	    }

            // Bottom border
            if (bottomPenWidth > 0)
            {
              str += "    <bottom-border>\n";
              col = writeColor (mcol.at(bottomfg_bg));
              writePen (str, bottomPenWidth, bottomPenStyle, col);
              str += "    </bottom-border>\n";
	    }

            // Top border
            if (topPenWidth > 0)
            {
              str += "    <top-border>\n";
              col = writeColor (mcol.at(topfg_bg));
              writePen (str, topPenWidth, topPenStyle, col);
              str += "    </top-border>\n";
	    }


            str += "     <font size=\"";
            str += QString::number (fontsize); 
            str += "\" family=\"helvetica\" weight=\"0\""; 
            if (italic    == 1) str += " italic=\"yes\"";
            if (bold      == 1) str += " bold=\"yes\"";
            if (underline == 1) str += " underline=\"yes\"";
            str +=" />\n";    
	    str += "    </format>\n";
	  }
          str += "    <text>" + mystr + "</text>\n";  
          str += "   </cell>\n"; 
	}


        if (i>step) 
        {
            i=0;
            value+=2;
            emit sigProgress (value);
        }
    }
    emit sigProgress(100);

    str += "  </table>\n";
    str += " </map>\n";
    str += "</spreadsheet>\n";
//  str += "</DOC>\n";

    printf ("Text %s\n", (const char *) str.utf8());

    KoStore out=KoStore(QString(fileOut), KoStore::Write);

    if (!out.open("root")) 
    {
      kdError(38000/*30502*/) << "Unable to open output file!" << endl;
        in.close  ();
        out.close ();
        return false;
    }

    QCString cstring = str.utf8();
    out.write ( (const char*)cstring, cstring.length() );

    out.close ();
    in.close  ();
    return true;
}




/******************************************************************************
 *  function: specCharfind                                                    *
 ******************************************************************************/
QChar 
APPLIXSPREADImport::specCharfind (QChar a, QChar b)
{
  QChar chr;

   if      ( (a == 'n') && (b == 'p') ) chr = 'ß'; 


   else if ( (a == 'n') && (b == 'c') ) chr = 'Ò'; 
   else if ( (a == 'p') && (b == 'c') ) chr = 'ò'; 

   else if ( (a == 'n') && (b == 'd') ) chr = 'Ó'; 
   else if ( (a == 'p') && (b == 'd') ) chr = 'ó'; 

   else if ( (a == 'n') && (b == 'e') ) chr = 'Ô'; 
   else if ( (a == 'p') && (b == 'e') ) chr = 'ô'; 

   else if ( (a == 'n') && (b == 'f') ) chr = 'Õ'; 
   else if ( (a == 'p') && (b == 'f') ) chr = 'õ'; 

   else if ( (a == 'p') && (b == 'g') ) chr = 'ö'; 
   else if ( (a == 'n') && (b == 'g') ) chr = 'Ö'; 



   else if ( (a == 'n') && (b == 'j') ) chr = 'Ù'; 
   else if ( (a == 'p') && (b == 'j') ) chr = 'ù'; 

   else if ( (a == 'n') && (b == 'k') ) chr = 'Ú'; 
   else if ( (a == 'p') && (b == 'k') ) chr = 'ú'; 

   else if ( (a == 'n') && (b == 'l') ) chr = 'Û'; 
   else if ( (a == 'p') && (b == 'l') ) chr = 'û'; 

   else if ( (a == 'p') && (b == 'm') ) chr = 'ü'; 
   else if ( (a == 'n') && (b == 'm') ) chr = 'Ü';



   else if ( (a == 'm') && (b == 'a') ) chr = 'À'; 
   else if ( (a == 'o') && (b == 'a') ) chr = 'à'; 

   else if ( (a == 'm') && (b == 'b') ) chr = 'Á'; 
   else if ( (a == 'o') && (b == 'b') ) chr = 'á'; 

   else if ( (a == 'm') && (b == 'c') ) chr = 'Â'; 
   else if ( (a == 'o') && (b == 'c') ) chr = 'â'; 

   else if ( (a == 'm') && (b == 'd') ) chr = 'Ã'; 
   else if ( (a == 'o') && (b == 'd') ) chr = 'ã'; 

   else if ( (a == 'm') && (b == 'e') ) chr = 'Ä'; 
   else if ( (a == 'o') && (b == 'e') ) chr = 'ä'; 

   else if ( (a == 'm') && (b == 'f') ) chr = 'Å'; 
   else if ( (a == 'o') && (b == 'f') ) chr = 'å'; 

   else if ( (a == 'm') && (b == 'g') ) chr = 'Æ'; 
   else if ( (a == 'o') && (b == 'g') ) chr = 'æ'; 



   else if ( (a == 'm') && (b == 'i') ) chr = 'È'; 
   else if ( (a == 'o') && (b == 'i') ) chr = 'è'; 

   else if ( (a == 'm') && (b == 'j') ) chr = 'É'; 
   else if ( (a == 'o') && (b == 'j') ) chr = 'é'; 

   else if ( (a == 'm') && (b == 'k') ) chr = 'Ê'; 
   else if ( (a == 'o') && (b == 'k') ) chr = 'ê';
 
   else if ( (a == 'm') && (b == 'l') ) chr = 'Ë'; 
   else if ( (a == 'o') && (b == 'l') ) chr = 'ë'; 






   else if ( (a == 'm') && (b == 'm') ) chr = 'Ì'; 
   else if ( (a == 'o') && (b == 'm') ) chr = 'ì'; 

   else if ( (a == 'm') && (b == 'n') ) chr = 'Í'; 
   else if ( (a == 'o') && (b == 'n') ) chr = 'í'; 

   else if ( (a == 'm') && (b == 'o') ) chr = 'Î'; 
   else if ( (a == 'o') && (b == 'o') ) chr = 'î'; 

   else if ( (a == 'm') && (b == 'p') ) chr = 'Ï'; 
   else if ( (a == 'o') && (b == 'p') ) chr = 'ï'; 


   else if ( (a == 'n') && (b == 'b') ) chr = 'Ñ'; 
   else if ( (a == 'p') && (b == 'b') ) chr = 'ñ'; 


   else if ( (a == 'k') && (b == 'c') ) chr = '¢'; 
   else if ( (a == 'k') && (b == 'j') ) chr = '©'; 
   else if ( (a == 'l') && (b == 'f') ) chr = 'µ'; 
   else if ( (a == 'n') && (b == 'i') ) chr = 'Ø'; 
   else if ( (a == 'p') && (b == 'i') ) chr = 'ø'; 

   else if ( (a == 'l') && (b == 'j') ) chr = '¹'; 
   else if ( (a == 'l') && (b == 'c') ) chr = '²'; 
   else if ( (a == 'l') && (b == 'd') ) chr = '³'; 

   else if ( (a == 'l') && (b == 'm') ) chr = '¼'; 
   else if ( (a == 'l') && (b == 'n') ) chr = '½'; 
   else if ( (a == 'l') && (b == 'o') ) chr = '¾'; 

   else if ( (a == 'l') && (b == 'a') ) chr = '°'; 

   else if ( (a == 'k') && (b == 'o') ) chr = '®'; 
   else if ( (a == 'k') && (b == 'h') ) chr = '§'; 
   else if ( (a == 'k') && (b == 'd') ) chr = '£'; 

   else if ( (a == 'p') && (b == 'a') ) chr = 'ð'; 
   else if ( (a == 'n') && (b == 'a') ) chr = 'Ð'; 

   else if ( (a == 'l') && (b == 'l') ) chr = '»'; 
   else if ( (a == 'k') && (b == 'l') ) chr = '«'; 

   else if ( (a == 'l') && (b == 'k') ) chr = 'º'; 

   else if ( (a == 'l') && (b == 'h') ) chr = '·'; 

   else if ( (a == 'k') && (b == 'b') ) chr = '¡'; 

   else if ( (a == 'k') && (b == 'e') ) chr = '¤'; 

   else if ( (a == 'l') && (b == 'b') ) chr = '±'; 

   else if ( (a == 'l') && (b == 'p') ) chr = '¿'; 

   else if ( (a == 'k') && (b == 'f') ) chr = '¥'; 

   else if ( (a == 'p') && (b == 'o') ) chr = 'þ';
   else if ( (a == 'n') && (b == 'o') ) chr = 'Þ';

   else if ( (a == 'n') && (b == 'n') ) chr = 'Ý'; 
   else if ( (a == 'p') && (b == 'n') ) chr = 'ý'; 
   else if ( (a == 'p') && (b == 'p') ) chr = 'ÿ'; 

   else if ( (a == 'k') && (b == 'k') ) chr = 'ª'; 

   else if ( (a == 'k') && (b == 'm') ) chr = '¬'; 
   else if ( (a == 'p') && (b == 'h') ) chr = '÷';
 
   else if ( (a == 'k') && (b == 'g') ) chr = '|'; 

   else if ( (a == 'l') && (b == 'e') ) chr = '\''; 

   else if ( (a == 'k') && (b == 'i') ) chr = '¨';  

   else if ( (a == 'k') && (b == 'n') ) chr = '­';  

   else if ( (a == 'k') && (b == 'p') ) chr = '¯';  

   else if ( (a == 'l') && (b == 'g') ) chr = '¶';  

   else if ( (a == 'l') && (b == 'i') ) chr = '¸';  
 
   else if ( (a == 'm') && (b == 'h') ) chr = 'Ç';  
   else if ( (a == 'o') && (b == 'h') ) chr = 'ç';  

   else if ( (a == 'n') && (b == 'h') ) chr = '×';  

   else if ( (a == 'k') && (b == 'a') ) chr = ' ';  

   else if ( (a == 'a') && (b == 'j') ) chr = '!';  

   else  chr = '#'; 

   return chr;            
}



/******************************************************************************
 *  function:   writePen                                                      *
 ******************************************************************************/
void 
APPLIXSPREADImport::writePen (QString &str, int penwidth, int penstyle, QString framecolor)
{
  str += "     <pen width=\"";

  // width of the pen
  str += QString::number (penwidth);
  str += "\" style=\"";

  // style of the pen
  str += QString::number (penstyle);
  str += "\" color=\"";

  // color of the pen
  str += framecolor;
  str += "\" />\n";

}



/******************************************************************************
 *  function:   writeColor                                                    *
 ******************************************************************************/
QString
APPLIXSPREADImport::writeColor (t_mycolor *mc)
{
  char rgb[20];

//    printf ("                 WriteColor: <%d>-<%d>-<%d>   <%d>-<%d>-<%d>-<%d>\n", 
//            mc->r, mc->g, mc->b,
//            mc->c, mc->m, mc->y, mc->k);

  sprintf (rgb, "#%02X%02X%02X", mc->r, mc->g, mc->b);
  QString bla = rgb;


  return bla;
}




/******************************************************************************
 *  function:   readTypefaceTable                                             *
 ******************************************************************************/
void
APPLIXSPREADImport::readTypefaceTable  (QTextStream &stream, QStringList &typefacetab)
{         
   int tftabCounter=0, ok;
   QString mystr;
 
   // Read the colormap
   printf ("Reading typeface table: \n");
          
   ok = true;
   do
   {
     mystr = stream.readLine ();
     if (mystr == "END TYPEFACE TABLE" ) ok = false;
     else 
     {
       printf ("  %2d: <%s>\n", tftabCounter, (const char *) mystr.latin1());  
       typefacetab.append(mystr); 
       tftabCounter++;
     }
    } 
    while (ok == true );

    printf ("... done \n\n");
}



/******************************************************************************
 *  function:   readColormap                                                  *
 ******************************************************************************/
void 
APPLIXSPREADImport::readColormap (QTextStream &stream,  QList<t_mycolor> &mcol)
{
  int contcount, ok, pos;

  QString colstr, mystr;
  printf ("Reading colormap: \n");

  ok = true;

  do
  {

     mystr = stream.readLine ();
     mystr.stripWhiteSpace ();

     if (mystr == "END COLORMAP") ok = false;
     else
     {
       printf ("  -> <%-32s> ", (const char *) mystr.latin1());

       // Count the number of  whitespaces
       contcount = mystr.contains (' ');
       printf ("contcount:%d ", contcount);
       contcount -= 5; 

       // Begin off interest
       pos = mystr.find (" 0 ");

       // get colorname
       colstr = mystr.left (pos);
       mystr.remove (0, pos+1);
       mystr.stripWhiteSpace ();
       
       t_mycolor *tmc = new t_mycolor;

       // get sub colors
       pos = sscanf ((const char *) mystr.latin1(), "0 %d %d %d %d 0", 
                     &tmc->c, &tmc->m, &tmc->y, &tmc->k); 

       printf ("  - <%-20s> <%-15s> <%3d> <%3d> <%3d> <%3d>  pos: %d\n", 
               (const char *) mystr.latin1(),
               (const char *) colstr.latin1(),
                tmc->c, tmc->m, tmc->y, tmc->k, pos);

       // Color transformation cmyk -> rgb
       tmc->r = 255 - (tmc->c + tmc->k);
       if (tmc->r < 0) tmc->r = 0;

       tmc->g = 255 - (tmc->m + tmc->k);
       if (tmc->g < 0) tmc->g = 0;

       tmc->b = 255 - (tmc->y + tmc->k);
       if (tmc->b < 0) tmc->b = 0;

       mcol.append (tmc);
     }
 
   }
   while (ok == true );

   printf ("... done <%d>\n\n", mcol.count());


   t_mycolor *emp;
   for (emp=mcol.first(); emp != 0; emp=mcol.next() )
   {
     printf (" c:%3d m:%3d y:%3d k:%3d   r:%3d g:%3d b:%3d\n",
            emp->c, emp->m, emp->y, emp->k, emp->r, emp->g, emp->b);
   }
}




/******************************************************************************
 *  function:   readColormap                                                  *
 ******************************************************************************/
void 
APPLIXSPREADImport::readView (QTextStream &stream, QString instr, t_rc &rc)
{
  QString rowcolstr;
  QString mystr, tabname;
  int ok;

   printf ("Reading View\n"); 

   tabname = instr;

   tabname.remove (0, 19);
   tabname.remove (tabname.length()-2, 2);
   printf ("  - Table name: %s\n", (const char *) tabname.latin1());

   ok = true;
   do
   {
     mystr = stream.readLine ();
    
     printf ("  %s\n", (const char *) mystr.latin1());
     if (mystr.startsWith ("View End, Name:")) ok = false;
     else
     {
       // COLUMN Widths
       if  (mystr.startsWith ("View Column Widths"))
       {
          printf ("   - Column Widths\n");
	  mystr.remove (0, 20);
          printf ("      <%s>\n", (const char *) mystr.latin1());
              
          int  colwidth, icolumn;
          char ccolumn;

          // loop
          QStringList ColumnList;
          ColumnList = QStringList::split (' ', mystr);

          for ( QStringList::Iterator it = ColumnList.begin(); it != ColumnList.end(); ++it ) 
          {
            sscanf ((const char *) (*it).latin1(), "%c:%d", &ccolumn, &colwidth);
            printf( "     >%s<- -<%c><%d>  \n", (*it).latin1(), ccolumn, colwidth);

            icolumn = ccolumn - 64;

            rowcolstr += "  <column width=\"";
            rowcolstr += QString::number (colwidth);
            rowcolstr += "\" column=\"";
            rowcolstr += QString::number (icolumn);
            rowcolstr += "\" >\n";
            rowcolstr += "   <format/>\n";
            rowcolstr += "  </column>\n";
          }
       }

       // ROW Heights 
       else if  (mystr.startsWith ("View Row Heights"))
       {
         printf ("   - Row Heights\n");
	 mystr.remove (0, 17);
         printf ("      <%s>\n", (const char *) mystr.latin1());

         int irow, rowheight;

         // loop
         QStringList RowList;
         RowList = QStringList::split (' ', mystr);

         for ( QStringList::Iterator it = RowList.begin(); it != RowList.end(); ++it ) 
         {
            sscanf ((const char *) (*it).latin1(), " %d:%d", 
                    &irow, &rowheight);
            printf ("   row: %2d   height: %2d\n", irow, rowheight);
            rowcolstr += "  <row row=\"";
            rowcolstr += QString::number (irow);
            rowcolstr += "\" height=\"";
            rowcolstr += QString::number (rowheight);
            rowcolstr += "\" >\n";
            rowcolstr += "   <format/>\n";
            rowcolstr += "  </row>\n";
         }


       }
     } // else != END COLORMAP
   }
   while (ok == true );

   // tabname append to my list
   // tabname append to my list
   rc.tabname.append (tabname);
   rc.rc.append (rowcolstr);

   printf ("%s %s\n", (const char *) tabname.latin1(),
                      (const char *) rowcolstr.latin1());

   printf ("...done \n\n");
}





/******************************************************************************
 *  function:   filterSHFGBG                                                  *
 ******************************************************************************/
void 
APPLIXSPREADImport::filterSHFGBG (QString it, int *style, int *bgcolor, 
                                  int *fgcolor)
{              
  QString tmpstr;
  int     pos;
  int     m2=0, m3=0;

  // filter SH = Brushstyle Background 
  pos = it.find ("SH");
  if (pos > -1)
  {
     tmpstr = it;
     if (pos > 0)   tmpstr.remove(0, pos);
     pos = sscanf ((const char *) tmpstr.latin1(), "SH%d", 
                   style);

     printf ("style: %d(%d)  ", 
             *style, pos);
  }


  // filter FG = FGCOLOR 
  pos = it.find ("FG");
  if (pos > -1)
  {
    tmpstr = it;
    if (pos > 0)   tmpstr.remove(0, pos);
    pos = sscanf ((const char *) tmpstr.latin1(), "FG%d", 
                  fgcolor);
    printf ("fg: %d(%d)  ", 
            *fgcolor, pos);
     m2=1;
  }


  // filter BG = BGCOLOR 
  pos = it.find ("BG");
  if (pos > -1)
  {
    tmpstr = it;
    if (pos > 0)   tmpstr.remove(0, pos);
    pos = sscanf ((const char *) tmpstr.latin1(), "BG%d", 
                  bgcolor);
    printf ("bgcolor: %d(%d)  ", 
            *bgcolor, pos);
    m3=1;
  }


  printf ("\n");
  
  
  // corrent the bgcolor to the fgcolor if the background is plain
  if ((*style == 8) && (m2 == 1) && (m3 == 0))
  {
    *bgcolor = *fgcolor;
  }


  // Translate brushstyle to kspread brushstyle  
  if (*style != 0)
  {
    if       (*style ==  1) *style =  0;
    else if  (*style ==  2) *style =  7;
    else if  (*style ==  3) *style =  0;
    else if  (*style ==  4) *style =  4;  
    else if  (*style ==  5) *style =  3;  
    else if  (*style ==  6) *style =  2;  
    else if  (*style ==  7) *style =  0;
    else if  (*style ==  8) *style =  0;
    else if  (*style ==  9) *style = 10;  
    else if  (*style == 10) *style =  9;  
    else if  (*style == 11) *style = 11;  
    else if  (*style == 12) *style = 12;  
    else if  (*style == 13) *style = 13;  
    else if  (*style == 14) *style = 14;  
    else if  (*style == 15) *style =  0;
    else if  (*style == 16) *style =  0;
    else if  (*style == 17) *style =  0;
    else if  (*style == 18) *style =  0;
    else if  (*style == 19) *style =  0;
  }
}



/******************************************************************************
 *  function:   filterSHFGBG                                                  *
 ******************************************************************************/
void 
APPLIXSPREADImport::transPenFormat (QString it, int *PenWidth, int *PenStyle)
{
   
   if       ( it[1] == '1' ) 
   {
     *PenWidth = 1; 
     *PenStyle = 1;
   }

   else if  ( it[1] == '2' )  
   {
     *PenWidth = 2; 
     *PenStyle = 1;
   }

   else if  ( it[1] == '3' )  
   {
     *PenWidth = 3; 
     *PenStyle = 1;
   }

   else if  ( it[1] == '4' )  
   {
     *PenWidth = 1; 
     *PenStyle = 3;
   }

   else if  ( it[1] == '5' )  
   {
     *PenWidth = 5; 
     *PenStyle = 1;
   }

   printf ("frame (w:%d - s:%d) ", *PenWidth, *PenStyle); 
}
