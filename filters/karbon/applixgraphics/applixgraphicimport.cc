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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#define FAKTOR 39.4  // 1000 dots/inch / 2.54 cm/inch    =   394 dots/cm  = 39.4 dots/mm

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <kdebug.h>
#include <QFile>
#include <qtextstream.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3CString>
#include <KoFilterChain.h>
#include <kgenericfactory.h>
#include <math.h>

#include <applixgraphicimport.h>

typedef KGenericFactory<APPLIXGRAPHICImport> APPLIXGRAPHICImportFactory;
K_EXPORT_COMPONENT_FACTORY( libapplixgraphicimport, APPLIXGRAPHICImportFactory( "kofficefilters" ) )

int s_area = 30517;

/******************************************************************************
 *  class: applixGraphicsLine          function:  constructor                 *
 ******************************************************************************
 *                                                                            *
 *  Short description : - Initialize all variables                            *
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
applixGraphicsLine::applixGraphicsLine()
{
  offX      = 0;
  offY      = 0;
  reColor   = 0;
  thickNess = 1; // ??

  for (int i=0; i<5; i++)
  {
    ptX[0] = 0;
    ptY[0] = 0;
  }
}





/******************************************************************************
 *  class: applixGraphicsRect          function:  constructor                 *
 ******************************************************************************
 *                                                                            *
 *  Short description : - Initialize all variables                            *
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
applixGraphicsRect::applixGraphicsRect() : applixGraphicsLine()
{
  for (int i=0; i<7;i++)
  {
    bf[i] = 0;
    lf[i] = 0;
  }

  for (int i=0; i<5;i++)
  {
    sh[i] = 0;
  }

  for (int i=0; i<8;i++)
  {
    pa[i] = 0;
  }

  for (int i=0; i<3;i++)
  {
    vs[i] = 0;
  }

  for (int i=0; i<2;i++)
  {
    xr[i] = 0;
  }

}





/******************************************************************************
 *  class: APPLIXGRAPHICImport          function:  constructor                *
 ******************************************************************************
 *                                                                            *
 *  Short description : constructor                                           *
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
APPLIXGRAPHICImport::APPLIXGRAPHICImport (KoFilter *, const char *, const QStringList&) :
                     KoFilter ()
{

}



/******************************************************************************
 *  class: APPLIXGRAPHICImport          function:  filter                     *
 ******************************************************************************
 *                                                                            *
 *  Short description : - Read inputfile,                                     *
 *                      - convert it to kontour fileformat and                *
 *                      - save it                                             *
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
KoFilter::ConversionStatus APPLIXGRAPHICImport::convert( const QByteArray& from, const QByteArray& to )
{

    // Check MIME Types
    if (to!="application/x-karbon" || from!="application/x-applixgraphic")
        return KoFilter::NotImplemented;

    // Open Inputfile
    QFile in (m_chain->inputFile());
    if (!in.open (QIODevice::ReadOnly) )
    {
        kError(30502) << "Unable to open input file!" << endl;
        in.close ();
        return KoFilter::FileNotFound;
    }

    QString str;

    // Create karbon header
    str += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    str += "<!DOCTYPE karbon>\n";
    str += " <karbon mime=\"application/x-karbon\" version=\"2\" editor=\"APPLIXGRAPHICS import filter\">\n";
//      str += "  <head>\n";
//      str += "   <layout width=\"210\" lmargin=\"0\" format=\"a4\" bmargin=\"0\" height=\"297\" rmargin=\"0\" tmargin=\"0\" orientation=\"portrait\"/>\n";
//      str += "   <grid dx=\"20\" dy=\"20\" align=\"0\">\n";
//      str += "    <helplines align=\"0\"/>\n";
//      str += "   </grid>\n";
//      str += "  </head>\n";
    str += "  <layer name=\"ag-layer\" visible=\"1\">\n";

    QTextStream stream(&in);
    int step  = in.size()/50;
    int value = 0;
    int i     = 0;
    int picture_rememberer = 0;
    int pos   = 0;
    int vers[3] = { 0, 0, 0 };
    int rueck;  // Check scanf inputs
    QString mystr;

    // Read Headline
    mystr = stream.readLine ();
    rueck = sscanf ((const char *) mystr.latin1() ,
                    "*BEGIN GRAPHICS VERSION=%d/%d ENCODING=%dBIT",
	             &vers[0], &vers[1], &vers[2]);
    kDebug (s_area) << "Versions info: " << vers[0] << vers[1] << vers[2] << endl;

    // Check the headline
    if (rueck <= 0)
    {
      kDebug (s_area) << "Header not correkt - May be it is not an applixgraphics file" << endl;
      kDebug (s_area) << "Headerline: " << mystr << endl;

      QMessageBox::critical (0L, "Applixgraphics header problem",
                                  QString ("The Applixgraphics header is not correct. "
                                           "May be it is not an applixgraphics file! <BR>"
                                           "This is the header line I did read:<BR><B>%1</B>").arg(mystr.latin1()),
				    "Comma");

      // i18n( "What is the separator used in this file ? First line is \n%1" ).arg(firstLine),
      return KoFilter::StupidError;
    }

    while (!stream.atEnd ())
    {
        ++i;

        // Read one line
        mystr = stream.readLine ();
        kDebug (s_area) << "<<" << mystr << ">>" << endl;

        if      (mystr == "PICTURE")     picture_rememberer = 1;
        else if (mystr == "END PICTURE") picture_rememberer = 0;


        // Detect a point at the first place of the ascii_linie
        else if ((mystr[0] == '.') && (picture_rememberer == 1))
	{
          // Delete point at the first place of the ascii_linie
          mystr.remove (0, 1);
          kDebug (s_area) << "StartPoint recognized <" << mystr << ">" << endl;


          /********************************************************************
           * Element "LINE"                                                   *
           ********************************************************************/
          if (mystr.startsWith ("LINE AT") )
	  {
            // Define
            applixGraphicsLine agLine;

            //str += agLine.read (int, mystr);

            kDebug (s_area) << " Linie recognized: " << endl;
            mystr.remove (0, 8);
            //mystr.remove (0, 7);


            //remove_pos = mystr.find ('(');
            //mystr.remove (0, remove_pos);
            //agLine.offX= mystr.toInt();

            //remove_pos = mystr.find (',');
            //mystr.remove (0, remove_pos);
            //agLine.offY= mystr.toInt();
            rueck = sscanf ((const char *) mystr.latin1(), "(%d,%d)",
                            &agLine.offX, &agLine.offY);
	    kDebug (s_area) << "  Offset ->   x:" << agLine.offX << "  y:" << agLine.offY << endl;
            if (rueck <= 0)
	    {
              kDebug (s_area) << "LINE tag not correkt" << endl;
              kDebug (s_area) << "LINE: <" << mystr << ">" << endl;
              return KoFilter::StupidError;
	    }

            do
	    {
              // Get acutal position
              pos = in.at ();
              // Read one line
              mystr = stream.readLine ();

	      if      (mystr.startsWith (" RECOLOR ") )
	      {
                kDebug (s_area) << "  Recolor  ";
                mystr.remove (0, 9);
                if      (mystr == "ON")
                {
                  kDebug (s_area) << "On" << endl;
                  agLine.reColor = true;
                }
                else if (mystr == "OFF")
                {
                  kDebug (s_area) << "Off" << endl;
                  agLine.reColor = false;
                }
                else
                {
                   kDebug (s_area) << "!!!!!    Unknown RECOLOR item <" << mystr << ">" << endl;
                }
	      }
	      else if (mystr.startsWith (" THICKNESS ") )
	      {
                kDebug (s_area) << "  Thickness: ";
                mystr.remove (0, 11);
                sscanf ((const char *) mystr.latin1(), "%d", &agLine.thickNess);
                //
                kDebug (s_area) <<  agLine.thickNess << endl;
	      }
	      else if (mystr.startsWith (" PNTS ") )
	      {
                kDebug (s_area) << "  Pnts    : ";
                mystr.remove (0, 6);
                sscanf ((const char *) mystr.latin1(), "(%d,%d)(%d,%d)",
                        &agLine.ptX[0], &agLine.ptY[0], &agLine.ptX[1], &agLine.ptY[1]);
                kDebug (s_area) << agLine.ptX[0] << " " << agLine.ptY[0] << " "  << agLine.ptX[1] << " "  << agLine.ptY[1] << endl;
	      }

	    }
	    while ((mystr[0] != '.') && (mystr[0] != 'E'));

            // An die Position zurueckspringen
            in.at (pos);

            // Werte in die Struktur einlagern
            str += "   <polyline arrow1=\"0\" arrow2=\"0\" >\n";
//          str += "    <point x=\"" + agLine.ptX[0]  + "\" y=\"" + agLine.ptY[0] + "\" />";
            str += "    <point x=\"" ;
            str += QString::number ((agLine.ptX[0] + agLine.offX)/FAKTOR);
            str += "\" y=\"";
            str += QString::number ((agLine.ptY[0] + agLine.offY)/FAKTOR);
            str += "\" />\n";

//          str += "    <point x=\"" + agLine.ptX[1]  + "\" y=\"" + agLine.ptY[1] + "\" />";
            str += "    <point x=\"";
            str += QString::number ((agLine.ptX[1] + agLine.offX)/FAKTOR);
            str += "\" y=\"";
            str += QString::number ((agLine.ptY[1] + agLine.offY)/FAKTOR);
            str += "\" />\n";
            str += "    <gobject fillstyle=\"0\" linewidth=\"1\" strokecolor=\"#000000\" strokestyle=\"1\" >\n";
            str += "     <matrix dx=\"0\" dy=\"0\" m21=\"0\" m22=\"1\" m11=\"1\" m12=\"0\" />\n";
            str += "    </gobject>\n";



            str += "   </polyline>\n";


	  }
          /********************************************************************
           * Element RPOL Vieleck                                             *
           ********************************************************************/
          if (mystr.startsWith ("RPOL AT") )
	  {
            // Define
            applixGraphicsLine agLine;
            int nsides;

            //str += agLine.read (int, mystr);

            kDebug (s_area) << " RPOL recognized: " << endl;
            mystr.remove (0, 8);

            rueck = sscanf ((const char *) mystr.latin1(), "(%d,%d)",
                            &agLine.offX, &agLine.offY);
	    kDebug (s_area) << "  Offset ->   x:" << agLine.offX << "  y:" << agLine.offY << endl;
            if (rueck <= 0)
	    {
              kDebug (s_area) << "RPOL tag not correkt " << endl;
              kDebug (s_area) << "RPOL: <" << mystr << ">" << endl;
              return KoFilter::StupidError;
	    }

            do
	    {
              // Akutelle Position bestimmen
              pos = in.at ();
              // Zeile einlesen
              mystr = stream.readLine ();
//checkSubElement (mystr, subelements)
	      if      (mystr.startsWith (" RECOLOR ") )
	      {
                kDebug (s_area) << "  Recolor  " ;
                mystr.remove (0, 9);
                if      (mystr == "ON")
                {
                  kDebug (s_area) << "On" << endl;
                  agLine.reColor = true;
                }
                else if (mystr == "OFF")
                {
                  kDebug (s_area) << "Off" << endl;
                  agLine.reColor = false;
                }
                else
                {
                   kDebug (s_area) << "!!!!!    Whats that <" << mystr << ">" << endl;
                }
	      }
	      else if (mystr.startsWith (" NSIDES") )
	      {
                kDebug (s_area) << "  NSIDES: " ;
                mystr.remove (0, 8);
                pos = sscanf ((const char *) mystr.latin1(), "%d", &nsides);
                kDebug (s_area) << nsides << "(" << pos << ")" << endl;
	      }
	      else if (mystr.startsWith (" PNTS ") )
	      {
                kDebug (s_area) << "  Pnts    : ";
                mystr.remove (0, 6);
                sscanf ((const char *) mystr.latin1(), "(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)",
                        &agLine.ptX[0], &agLine.ptY[0], &agLine.ptX[1], &agLine.ptY[1],
                        &agLine.ptX[2], &agLine.ptY[2], &agLine.ptX[3], &agLine.ptY[3],
                        &agLine.ptX[4], &agLine.ptY[4]);
                kDebug (s_area) << agLine.ptX[0] << " " << agLine.ptY[0] << "   " <<
                                    agLine.ptX[1] << " " << agLine.ptY[1] << "   " <<
                                    agLine.ptX[2] << " " << agLine.ptY[2] << "   " <<
                                    agLine.ptX[3] << " " << agLine.ptY[3] << "   " <<
		                    agLine.ptX[4] << " " << agLine.ptY[4] << endl;
	      }
//checkSubElement() ende

	    }
	    while ((mystr[0] != '.') && (mystr[0] != 'E'));



            // An die Position zurueckspringen
            in.at (pos);

            int a, b;
            float wink=0.0;
            int *x = new int[nsides];
            int *y = new int[nsides];
            a = agLine.ptX[2] / 2;
            b = agLine.ptY[2] / 2;
            for (int i=0; i<nsides; i++)
	    {
              x[i] = qRound (a * cos (wink*M_PI/180));
              y[i] = qRound (b * sin (wink*M_PI/180));
	      wink += (360/nsides);
	    }

            // Werte in die Struktur einlagern
            str += "   <polyline arrow1=\"0\" arrow2=\"0\" >\n";
            for (int i=0; i<nsides; i++)
	    {
              str += "    <point x=\"" ;
              str += QString::number ((x[i] + agLine.offX)/FAKTOR);
              str += "\" y=\"";
              str += QString::number ((y[i] + agLine.offY)/FAKTOR);
              str += "\" />\n";
	    }

            str += "    <gobject fillstyle=\"0\" linewidth=\"1\" strokecolor=\"#000000\" strokestyle=\"1\" >\n";
            str += "     <matrix dx=\"0\" dy=\"0\" m21=\"0\" m22=\"1\" m11=\"1\" m12=\"0\" />\n";
            str += "    </gobject>\n";



            str += "   </polyline>\n";


	  }
          /********************************************************************
           * Element TXT - Text                                               *
           ********************************************************************/
          else if (mystr.startsWith ("TXT AT") )
	  {
            // Define
            //applixGraphicsText agText;
            applixGraphicsLine agText;

            kDebug (s_area) << " Habe Text erkannt (keine Werte uebernommen)" << endl;
            mystr.remove (0, 7);
            sscanf ((const char *) mystr.latin1(), "(%d,%d)",
                    &agText.offX, &agText.offY);
	    kDebug (s_area) << "  Offset ->   x:" << agText.offX << "  y:" << agText.offY << endl;


            do
	    {
              // Aktuelle Position bestimmen
              pos = in.at ();
              // Zeile einlesen
              mystr = stream.readLine ();
//checkSubElement()
	      if      (mystr.startsWith (" RECOLOR ") )
	      {
                kDebug (s_area) << "  Recolor  : ";
                mystr.remove (0, 9);
                if      (mystr == "ON")
                {
                  kDebug (s_area) << "On" << endl;
                  agText.reColor = true;
                }
                else if (mystr == "OFF")
                {
                  kDebug (s_area) << "Off" << endl;
                  agText.reColor = false;
                }
                else
                {
                   kDebug (s_area) << "!!!!!     Whats that <" << mystr << ">" << endl;
                }
	      }
	      else if (mystr.startsWith (" .STR") )
	      {
                kDebug (s_area) << " Textstring: ";

                // Zeile einlesen
                agText.str = stream.readLine ();
                agText.str.remove (0, 3);  // delete ront part
                kDebug (s_area) << agText.str;
	      }
	      else if (mystr.startsWith (" THICKNESS ") )
	      {
                kDebug (s_area) << "  Thickness: ";
                mystr.remove (0, 11);
                sscanf ((const char *) mystr.latin1(), "%d", &agText.thickNess);
                kDebug (s_area) <<  agText.thickNess << endl;
	      }
	      else if (mystr.startsWith (" PNTS ") )
	      {
                kDebug (s_area) << "  Pnts    : ";
                mystr.remove (0, 6);
                sscanf ((const char *) mystr.latin1(), "(%d,%d)(%d,%d)",
                        &agText.ptX[0], &agText.ptY[0], &agText.ptX[1], &agText.ptY[1]);
                kDebug (s_area) << agText.ptX[0] << " " << agText.ptY[0] << " " << agText.ptX[1] << " " << agText.ptY[1];
	      }
//checkSubElement() ende

	    }
	    while ((mystr[0] != '.') && (mystr[0] != 'E'));

            // An die Position zurueckspringen
            in.at (pos);

            // Werte in die Struktur einlagern
            str += "   <text ";
            str += "x=\"" ;
            str += QString::number (agText.offX);
            str += "\" y=\"" ;
            str += QString::number (agText.offY);
            str += "\" >\n";
            str += "    <![CDATA[" + agText.str;
            str += "]]>\n";
            str += "    <gobject fillstyle=\"0\" linewidth=\"1\" strokecolor=\"#000000\" strokestyle=\"1\" >\n";
            str += "     <matrix dx=\"119\" dy=\"310\" m21=\"0\" m22=\"1\" m11=\"1\" m12=\"0\" />\n";
            str += "    </gobject>\n";
            str += "   </text>\n";

	  }

          /********************************************************************
           * Element rectangle                                                *
           ********************************************************************/
          else if (mystr.startsWith ("RECT AT"))
	  {
            applixGraphicsRect agRect;

            kDebug (s_area) << " Habe Rectangle erkannt " << endl;
            mystr.remove (0, 8);
            rueck = sscanf ((const char *)mystr.latin1(), "(%d,%d)",    &agRect.offX, &agRect.offY);
	    if (rueck < 1) kDebug (s_area) <<"Fehler im String <" << mystr.latin1() << ">" << endl;
	    kDebug (s_area) << "  Offset ->   x:" << agRect.offX << "  y:" << agRect.offY << endl;


            do
	    {
              // Akutelle Position bestimmen
              pos = in.at ();
              // read one line
              mystr = stream.readLine ();

//checkSubElement()
              // option RECOLOR
	      if      (mystr.startsWith (" RECOLOR ") )
	      {
                kDebug (s_area) <<("  Recolor : ");
                mystr.remove (0, 9);
                if      (mystr == "ON")
                {  kDebug (s_area) << "On" << endl;  agRect.reColor = true;}
                else if (mystr == "OFF")
                { kDebug (s_area) << "Off" << endl;  agRect.reColor = false;}
                else
                {
                   kDebug (s_area) << "!!!!!    Whats that <" << mystr << ">" << endl;
                }
	      }
              // option BACKFILL
	      else if (mystr.startsWith (" BACKFILL ") )
	      {
                kDebug (s_area) <<("  Backfill: ");
                mystr.remove (0, 12);
                sscanf ((const char *)mystr.latin1(), "<%d %d %d %d %d %d %d>",
                        &agRect.bf[0], &agRect.bf[1], &agRect.bf[2],
                        &agRect.bf[3], &agRect.bf[4], &agRect.bf[5],
                        &agRect.bf[6]);
                kDebug (s_area) << agRect.bf[0] << " " << agRect.bf[1] << " "  <<
                                    agRect.bf[2] << " " << agRect.bf[3] << " "  <<
                                    agRect.bf[4] << " " << agRect.bf[5] << " "  <<
                                    agRect.bf[6];

	      }
              // option LINEFILL
	      else if (mystr.startsWith (" LINEFILL ") )
	      {
                kDebug (s_area) << "  Linefill: ";
                mystr.remove (0, 12);
                sscanf ((const char *)mystr.latin1(), "<%d %d %d %d %d %d %d>",
                        &agRect.lf[0], &agRect.lf[1], &agRect.lf[2],
                        &agRect.lf[3], &agRect.lf[4], &agRect.lf[5],
                        &agRect.lf[6]);
                kDebug (s_area) << agRect.lf[0] << " " << agRect.lf[1] << " "  <<
                                    agRect.lf[2] << " " << agRect.lf[3] << " "  <<
                                    agRect.lf[4] << " " << agRect.lf[5] << " "  <<
                                    agRect.lf[6];

	      }
              // option SHADOW
	      else if (mystr.startsWith (" SHADOW ")  )
	      {
                kDebug (s_area) << "  Shadow  : ";
                mystr.remove (0, 12);
                sscanf ((const char *)mystr.latin1(), "<%d %d %d %d %d>",
                        &agRect.sh[0], &agRect.sh[1], &agRect.sh[2],
                        &agRect.sh[3], &agRect.sh[4]);
                kDebug (s_area) << agRect.sh[0] << " " << agRect.sh[1] << " "  <<
                                    agRect.sh[2] << " " << agRect.sh[3] << " "  <<
                                    agRect.sh[4];
	      }
              // option PARA
	      else if (mystr.startsWith (" PARA ")  )
	      {
                kDebug (s_area) << "  Para    : ";
                mystr.remove (0, 12);
                sscanf ((const char *)mystr.latin1(), "<%d %d %d %d %d %d %d %d>",
                        &agRect.pa[0], &agRect.pa[1], &agRect.pa[2],
                        &agRect.pa[3], &agRect.pa[4], &agRect.pa[5],
                        &agRect.pa[6], &agRect.pa[7]);
                kDebug (s_area) << agRect.pa[0] << " " << agRect.pa[1] << " "  <<
                                    agRect.pa[2] << " " << agRect.pa[3] << " "  <<
                                    agRect.pa[4] << " " << agRect.pa[5] << " "  <<
                                    agRect.pa[6] << " " << agRect.pa[7] ;

	      }
              // option THICKNESS
	      else if (mystr.startsWith (" THICKNESS ") )
	      {
                kDebug (s_area) << "  Thickness: ";
                mystr.remove (0, 11);
                sscanf ((const char *) mystr.latin1(), "%d", &agRect.thickNess);
                kDebug (s_area) << agRect.thickNess << endl;
	      }
              // option V_SPACE
	      else if (mystr.startsWith (" V_SPACE ") )
	      {
                kDebug (s_area) << "  V_Space : ";
                mystr.remove (0, 9);
                sscanf ((const char *)mystr.latin1(), "(%d %d %d)",
                         &agRect.vs[0], &agRect.vs[1], &agRect.vs[2]);
                kDebug (s_area) << agRect.vs[0] << " " << agRect.vs[1] << " " <<  agRect.vs[2];
	      }
              // option XYRAD
	      else if (mystr.startsWith (" XYRAD ") )
	      {
                kDebug (s_area) << "  XYRad   : ";
                mystr.remove (0, 7);
                sscanf ((const char *)mystr.latin1(), "<%d %d>",
                         &agRect.xr[0], &agRect.xr[1]);
                kDebug (s_area) << agRect.xr[0] << " " << agRect.xr[1];
	      }
              // option PNTS
	      else if (mystr.startsWith (" PNTS ") )
	      {
                kDebug (s_area) << "  Pnts    : ";
                mystr.remove (0, 6);
                sscanf ((const char *)mystr.latin1(), "(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)",
                        &agRect.ptX[0], &agRect.ptY[0], &agRect.ptX[1], &agRect.ptY[1],
                        &agRect.ptX[2], &agRect.ptY[2], &agRect.ptX[3], &agRect.ptY[3],
                        &agRect.ptX[4], &agRect.ptY[4]);
                kDebug (s_area) << agRect.ptX[0] << " " << agRect.ptY[0] << "  " <<
                                    agRect.ptX[1] << " " << agRect.ptY[1] << "   " <<
                                    agRect.ptX[2] << " " << agRect.ptY[2] << "  " <<
                                    agRect.ptX[3] << " " << agRect.ptY[3] << "  " <<
		                    agRect.ptX[4] << " " << agRect.ptY[4] << endl;
	      }

//checkSubElement() ende
	    }
	    while ((mystr[0] != '.') && (mystr[0] != 'E'));

            // An die Position zurueckspringen
            in.at (pos);

            // Werte in die Struktur einlagern
            str += "   <rectangle ";
            str += "x=\"" ;
            str += QString::number ((agRect.offX)/FAKTOR);
            str += "\" y=\"";
            str += QString::number ((agRect.offY)/FAKTOR);
            str += "\" width=\"" ;
            str += QString::number ((agRect.ptX[2])/FAKTOR);
            str += "\" height=\"";
            str += QString::number ((agRect.ptY[2])/FAKTOR);
            str += "\" >\n";
            str += "    <polyline arrow1=\"0\" arrow2=\"0\" >\n";

            str += "     <point x=\"" ;
            str += QString::number ((agRect.ptX[0] + agRect.offX)/FAKTOR)  ;
            str += "\" y=\"" ;
            str += QString::number ((agRect.ptY[0] + agRect.offY)/FAKTOR) ;
            str += "\" />\n";

            str += "     <point x=\"" ;
            str += QString::number ((agRect.ptX[0] + agRect.ptX[2] + agRect.offX)/FAKTOR)  ;
            str += "\" y=\"" ;
            str += QString::number ((agRect.ptY[0] + agRect.offY)/FAKTOR) ;
            str += "\" />\n";

            str += "     <point x=\"" ;
            str += QString::number ((agRect.ptX[0] + agRect.ptX[2] + agRect.offX)/FAKTOR)  ;
            str += "\" y=\"" ;
            str += QString::number ((agRect.ptY[0] + agRect.ptY[2] + agRect.offY)/FAKTOR) ;
            str += "\" />\n";

            str += "     <point x=\"" ;
            str += QString::number ((agRect.ptX[0] + agRect.offX)/FAKTOR)  ;
            str += "\" y=\"" ;
            str += QString::number ((agRect.ptY[0] + agRect.ptY[2] + agRect.offY)/FAKTOR) ;
            str += "\" />\n";


            str += "     <gobject fillstyle=\"0\" linewidth=\"1\" strokecolor=\"#000000\" strokestyle=\"1\" >\n";
            str += "      <matrix dx=\"0\" dy=\"0\" m21=\"0\" m22=\"1\" m11=\"1\" m12=\"0\" />\n";
            str += "     </gobject>\n";
            str += "    </polyline>\n";
            str += "   </rectangle>\n";






	  }
          /********************************************************************
           * Element ELL - Ellipse                                            *
           ********************************************************************/
          else if (mystr.startsWith ("ELL AT"))
	  {
            applixGraphicsRect agEll;

            kDebug (s_area) << " Habe ELL erkannt (keine Werte uebernommen " << endl;
            mystr.remove (0, 7);
            sscanf ((const char *)mystr.latin1(), "(%d,%d)",
                    &agEll.offX, &agEll.offY);
	    kDebug (s_area) << "  Offset ->   x:" << agEll.offX << "  y:" << agEll.offY << endl;

            do
	    {
              // Aktuelle Position bestimmen
              pos = in.at ();
              // read one line
              mystr = stream.readLine ();

//checkSubElement()
              // option RECOLOR
	      if      (mystr.startsWith (" RECOLOR ") )
	      {
                kDebug (s_area) << "  Recolor: ";
                mystr.remove (0, 9);
                if      (mystr == "ON")
                {
                  kDebug (s_area) << "On" << endl;
                  agEll.reColor = true;
                }
                else if (mystr == "OFF")
                {
                  kDebug (s_area) << "Off" << endl;
                  agEll.reColor = false;
                }
                else
                {
                   kDebug (s_area) << "!!!!!    Whats that <" << mystr << ">" << endl;
                }
	      }
	      else if (mystr.startsWith (" PNTS ") )
	      {
                kDebug (s_area) <<("  Pnts   : ");
                mystr.remove (0, 6);
		//
                sscanf ((const char *)mystr.latin1(), "(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)",
                        &agEll.ptX[0], &agEll.ptY[0], &agEll.ptX[1], &agEll.ptY[1],
                        &agEll.ptX[2], &agEll.ptY[2], &agEll.ptX[3], &agEll.ptY[3],
                        &agEll.ptX[4], &agEll.ptY[4]);
                kDebug (s_area) << agEll.ptX[0] << " " << agEll.ptY[0] << "  " <<
                                    agEll.ptX[1] << " " << agEll.ptY[1] << "   " <<
                                    agEll.ptX[2] << " " << agEll.ptY[2] << "  " <<
                                    agEll.ptX[3] << " " << agEll.ptY[3] << "  " <<
		                    agEll.ptX[4] << " " << agEll.ptY[4] << endl;

	      }
//checkSubElement() ende
	    }
	    while ((mystr[0] != '.') && (mystr[0] != 'E'));

            // An die Position zurueckspringen
            in.at (pos);

            // Werte in die Struktur einlagern
            // ???

	  }
          else
	  {
            kDebug (s_area) << "Unbekannt : " << mystr << endl;
	  }

	}


        if (i>step)
        {
            i=0;
            value+=2;
            emit sigProgress (value);
        }
    }
    emit sigProgress(100);

    str += "  </layer>\n";
    str += " </karbon>\n";
//  str += "</DOC>\n";

    kDebug (s_area) << "Text " << str.utf8() << endl;

    KoStoreDevice* out= m_chain->storageFile( "root", KoStore::Write );
    if (!out)
    {
        kError(s_area) << "Unable to open output file!" << endl;
        in.close  ();
        return KoFilter::StorageCreationError;
    }

    Q3CString cstring = str.utf8();
    out->write ( (const char*)cstring, cstring.size() - 1 );

    in.close  ();
    return KoFilter::OK;
}

#include <applixgraphicimport.moc>




