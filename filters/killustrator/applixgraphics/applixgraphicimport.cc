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
#define FAKTOR 39.4  // 1000 dots/inch / 2.54 cm/inch    =   394 dots/cm  = 39.4 dots/mm
#define PI 3.1415926

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <qtextstream.h>
#include <qmessagebox.h>
#include <applixgraphicimport.h>
#include <applixgraphicimport.moc>
#include <kdebug.h>
#include <math.h>




/******************************************************************************
 *  class: applixGraphicsLine          funktion:  constructor                 *
 ******************************************************************************
 *                                                                            *
 *  Short description : - Inialize all variables                              *
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
 *  class: applixGraphicsRect          funktion:  constructor                 *
 ******************************************************************************
 *                                                                            *
 *  Short description : - Inialize all variables                              *
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
APPLIXGRAPHICImport::APPLIXGRAPHICImport (KoFilter *parent, const char *name) :
                     KoFilter (parent, name)
{

}



/******************************************************************************
 *  class: APPLIXGRAPHICImport          funktion:  filter                     *
 ******************************************************************************
 *                                                                            *
 *  Short description : - Read inputfile,                                     *
 *                      - convert it to killustrator fileformat and           *
 *                      - save it                                             *
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
bool
APPLIXGRAPHICImport::filter (const QString &fileIn,
                             const QString &fileOut,
                             const QString &from,
                             const QString &to,
                             const QString &)
{

    // Check MIME Types
    if (to!="application/x-killustrator" || from!="application/x-applixgraphic")
        return false;

    // Open Inputfile
    QFile in (fileIn);
    if (!in.open (IO_ReadOnly) )
    {
        kdError(30502) << "Unable to open input file!" << endl;
        in.close ();
        return false;
    }

    QString str;

    // Create killstrator header
    str += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    str += "<!DOCTYPE killustrator>\n";
    str += " <killustrator mime=\"application/x-killustrator\" version=\"2\" editor=\"APPLIXGRAPHICS import filter\">\n";
    str += "  <head>\n";
    str += "   <layout width=\"210\" lmargin=\"0\" format=\"a4\" bmargin=\"0\" height=\"297\" rmargin=\"0\" tmargin=\"0\" orientation=\"portrait\"/>\n";
    str += "   <grid dx=\"20\" dy=\"20\" align=\"0\">\n";
    str += "    <helplines align=\"0\"/>\n";
    str += "   </grid>\n";
    str += "  </head>\n";
    str += "  <layer>\n";

    QTextStream stream(&in);
    int step  = in.size()/50;
    int value = 0;
    int i     = 0;
    int merker= 0;
    int pos   = 0;
    int vers[3] = { 0, 0, 0 };
    int rueck;  // Check scanf inputs
    QString mystr;

    // Read Headline
    mystr = stream.readLine ();
    rueck = sscanf ((const char *) mystr.latin1() ,
                    "*BEGIN GRAPHICS VERSION=%d/%d ENCODING=%dBIT",
	             &vers[0], &vers[1], &vers[2]);
    printf ("Versions info: %d %d %d\n", vers[0], vers[1], vers[2]);

    // Check the headline
    if (rueck <= 0)
    {
      printf ("Header not correkt - May be it is not an applixgraphics file\n");
      printf ("Headerline: <%s>\n", (const char *) mystr.latin1());

      QMessageBox::critical (0L, "Applixgraphics header problem",
                                  QString ("The Applixgraphics header is not correct. "
                                           "May be it is not an applixgraphics file! <BR>"
                                           "This is the header line I did read:<BR><B>%1</B>").arg(mystr.latin1()),
				    "Comma");

      // i18n( "What is the separator used in this file ? First line is \n%1" ).arg(firstLine),
      return false;
    }

    while (!stream.atEnd ())
    {
        ++i;

        // Read one line
        mystr = stream.readLine ();
        printf ("<<%s>>\n", (const char *) mystr.latin1());

        if      (mystr == "PICTURE")     merker = 1;
        else if (mystr == "END PICTURE") merker = 0;


        // Detect Point at the first place of the Linie
        else if ((mystr[0] == '.') && (merker == 1))
	{
          // Delete  Point at the first place of the Linie
          mystr.remove (0, 1);
          printf ("StartPoint recognized <%s>\n", (const char *) mystr.latin1() );

          /********************************************************************
           * Element LINE                                                     *
           ********************************************************************/
          if (mystr.startsWith ("LINE AT") )
	  {
            // Define
            applixGraphicsLine agLine;

            //str += agLine.read (int, mystr);

            printf (" Linie recognized: \n");
            mystr.remove (0, 8);

            rueck = sscanf ((const char *) mystr.latin1(), "(%d,%d)",
                            &agLine.offX, &agLine.offY);
	    printf ("  Offset ->   x:%2d  y:%2d\n", agLine.offX, agLine.offY);
            if (rueck <= 0)
	    {
              printf ("LINE tag not correkt \n");
              printf ("LINE: <%s>\n", (const char *) mystr.latin1());
              return false;
	    }

            do
	    {
              // Akutelle Position bestimmen
              pos = in.at ();
              // Zeile einlesen
              mystr = stream.readLine ();
	      if      (mystr.startsWith (" RECOLOR ") )
	      {
                printf ("  Recolor  ");
                mystr.remove (0, 9);
                if      (mystr == "ON")
                {
                  printf ("On\n");
                  agLine.reColor = true;
                }
                else if (mystr == "OFF")
                {
                  printf ("Off\n");
                  agLine.reColor = false;
                }
                else
                {
                   printf ("!!!!!    Whats that <%s>\n", (const char *) mystr.latin1() );
                }
	      }
	      else if (mystr.startsWith (" THICKNESS ") )
	      {
                printf ("  Thickness: ");
                mystr.remove (0, 11);
                sscanf ((const char *) mystr.latin1(), "%d", &agLine.thickNess);
                printf ("%d\n", agLine.thickNess);
	      }
	      else if (mystr.startsWith (" PNTS ") )
	      {
                printf ("  Pnts    : ");
                mystr.remove (0, 6);
                sscanf ((const char *) mystr.latin1(), "(%d,%d)(%d,%d)",
                        &agLine.ptX[0], &agLine.ptY[0], &agLine.ptX[1], &agLine.ptY[1]);
                printf ("%d %d %d %d\n",
                         agLine.ptX[0],  agLine.ptY[0],  agLine.ptX[1],  agLine.ptY[1]);
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

            printf (" RPOL recognized: \n");
            mystr.remove (0, 8);

            rueck = sscanf ((const char *) mystr.latin1(), "(%d,%d)",
                            &agLine.offX, &agLine.offY);
	    printf ("  Offset ->   x:%2d  y:%2d\n", agLine.offX, agLine.offY);
            if (rueck <= 0)
	    {
              printf ("RPOL tag not correkt \n");
              printf ("RPOL: <%s>\n", (const char *) mystr.latin1());
              return false;
	    }

            do
	    {
              // Akutelle Position bestimmen
              pos = in.at ();
              // Zeile einlesen
              mystr = stream.readLine ();
	      if      (mystr.startsWith (" RECOLOR ") )
	      {
                printf ("  Recolor  ");
                mystr.remove (0, 9);
                if      (mystr == "ON")
                {
                  printf ("On\n");
                  agLine.reColor = true;
                }
                else if (mystr == "OFF")
                {
                  printf ("Off\n");
                  agLine.reColor = false;
                }
                else
                {
                   printf ("!!!!!    Whats that <%s>\n", (const char *) mystr.latin1() );
                }
	      }
	      else if (mystr.startsWith (" NSIDES") )
	      {
                printf ("  NSIDES: ");
                mystr.remove (0, 8);
                pos = sscanf ((const char *) mystr.latin1(), "%d", &nsides);
                printf ("%d(%d)\n", &nsides, pos);
	      }
	      else if (mystr.startsWith (" PNTS ") )
	      {
                printf ("  Pnts    : ");
                mystr.remove (0, 6);
                sscanf ((const char *) mystr.latin1(), "(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)",
                        &agLine.ptX[0], &agLine.ptY[0], &agLine.ptX[1], &agLine.ptY[1],
                        &agLine.ptX[2], &agLine.ptY[2], &agLine.ptX[3], &agLine.ptY[3],
                        &agLine.ptX[4], &agLine.ptY[4]);
                printf ("%d %d   %d %d   %d %d   %d %d   %d %d\n",
                         agLine.ptX[0],  agLine.ptY[0],  agLine.ptX[1],  agLine.ptY[1],
                         agLine.ptX[2],  agLine.ptY[2],  agLine.ptX[3],  agLine.ptY[3],
                         agLine.ptX[4],  agLine.ptY[4]);
	      }

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
              x[i] = a * cos (wink*PI/180);
              y[i] = b * sin (wink*PI/180);
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

            printf (" Habe Text erkannt (keine Werte uebernommen)\n");
            mystr.remove (0, 7);
            sscanf ((const char *) mystr.latin1(), "(%d,%d)",
                    &agText.offX, &agText.offY);
	    printf ("  Offset ->   x:%2d  y:%2d\n",
                    agText.offX,   agText.offY);


            do
	    {
              // Akutelle Position bestimmen
              pos = in.at ();
              // Zeile einlesen
              mystr = stream.readLine ();
	      if      (mystr.startsWith (" RECOLOR ") )
	      {
                printf ("  Recolor  : ");
                mystr.remove (0, 9);
                if      (mystr == "ON")
                {
                  printf ("On\n");
                  agText.reColor = true;
                }
                else if (mystr == "OFF")
                {
                  printf ("Off\n");
                  agText.reColor = false;
                }
                else
                {
                   printf ("!!!!!     Whats that <%s>\n", (const char *) mystr.latin1() );
                }
	      }
	      else if (mystr.startsWith (" .STR") )
	      {
                printf (" Textstring: ");

                // Zeile einlesen
                agText.str = stream.readLine ();
                agText.str.remove (0, 3);  // delete ront part
                printf ("%s\n", (const char *) agText.str.latin1());
	      }
	      else if (mystr.startsWith (" THICKNESS ") )
	      {
                printf ("  Thickness: ");
                mystr.remove (0, 11);
                sscanf ((const char *) mystr.latin1(), "%d", &agText.thickNess);
                printf ("%d\n", agText.thickNess);
	      }
	      else if (mystr.startsWith (" PNTS ") )
	      {
                printf ("  Pnts    : ");
                mystr.remove (0, 6);
                sscanf ((const char *) mystr.latin1(), "(%d,%d)(%d,%d)",
                        &agText.ptX[0], &agText.ptY[0], &agText.ptX[1], &agText.ptY[1]);
                printf ("%d %d %d %d",
                         agText.ptX[0],  agText.ptY[0],  agText.ptX[1],  agText.ptY[1]);
	      }

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

            printf (" Habe Rectangle erkannt \n");
            mystr.remove (0, 8);
            rueck = sscanf ((const char *)mystr.latin1(), "(%d,%d)",    &agRect.offX, &agRect.offY);
	    if (rueck < 1) printf ("Fehler im String <%s>\n", (const char *)mystr.latin1());
	    printf ("  Offset ->   x:%2d  y:%2d\n", agRect.offX, agRect.offY);


            do
	    {
              // Akutelle Position bestimmen
              pos = in.at ();
              // read one line
              mystr = stream.readLine ();

              // option RECOLOR
	      if      (mystr.startsWith (" RECOLOR ") )
	      {
                printf ("  Recolor : ");
                mystr.remove (0, 9);
                if      (mystr == "ON")
                {  printf ("On\n");  agRect.reColor = true;}
                else if (mystr == "OFF")
                { printf ("Off\n");  agRect.reColor = false;}
                else
                {
                   printf ("!!!!!     Whats that <%s>\n", (const char *) mystr.latin1() );
                }
	      }
              // option BACKFILL
	      else if (mystr.startsWith (" BACKFILL ") )
	      {
                printf ("  Backfill: ");
                mystr.remove (0, 12);
                sscanf ((const char *)mystr.latin1(), "<%d %d %d %d %d %d %d>",
                        &agRect.bf[0], &agRect.bf[1], &agRect.bf[2],
                        &agRect.bf[3], &agRect.bf[4], &agRect.bf[5],
                        &agRect.bf[6]);
                printf ("%d %d %d %d %d %d %d\n",
                        agRect.bf[0], agRect.bf[1], agRect.bf[2],
                        agRect.bf[3], agRect.bf[4], agRect.bf[5],
                        agRect.bf[6]);
	      }
              // option LINEFILL
	      else if (mystr.startsWith (" LINEFILL ") )
	      {
                printf ("  Linefill: ");
                mystr.remove (0, 12);
                sscanf ((const char *)mystr.latin1(), "<%d %d %d %d %d %d %d>",
                        &agRect.lf[0], &agRect.lf[1], &agRect.lf[2],
                        &agRect.lf[3], &agRect.lf[4], &agRect.lf[5],
                        &agRect.lf[6]);
                printf ("%d %d %d %d %d %d %d\n",
                        agRect.lf[0], agRect.lf[1], agRect.lf[2],
                        agRect.lf[3], agRect.lf[4], agRect.lf[5],
                        agRect.lf[6]);
	      }
              // option SHADOW
	      else if (mystr.startsWith (" SHADOW ")  )
	      {
                printf ("  Shadow  : ");
                mystr.remove (0, 12);
                sscanf ((const char *)mystr.latin1(), "<%d %d %d %d %d>",
                        &agRect.sh[0], &agRect.sh[1], &agRect.sh[2],
                        &agRect.sh[3], &agRect.sh[4]);
                printf ("%d %d %d %d %d\n",
                        agRect.sh[0], agRect.sh[1], agRect.sh[2],
                        agRect.sh[3], agRect.sh[4]);
	      }
              // option PARA
	      else if (mystr.startsWith (" PARA ")  )
	      {
                printf ("  Para    : ");
                mystr.remove (0, 12);
                sscanf ((const char *)mystr.latin1(), "<%d %d %d %d %d %d %d %d>",
                        &agRect.pa[0], &agRect.pa[1], &agRect.pa[2],
                        &agRect.pa[3], &agRect.pa[4], &agRect.pa[5],
                        &agRect.pa[6], &agRect.pa[7]);
                printf ("%d %d %d %d %d %d %d %d\n",
                        agRect.pa[0], agRect.pa[1], agRect.pa[2],
                        agRect.pa[3], agRect.pa[4], agRect.pa[5],
                        agRect.pa[6], agRect.pa[7]);
	      }
              // option THICKNESS
	      else if (mystr.startsWith (" THICKNESS ") )
	      {
                printf ("  Thickness: ");
                mystr.remove (0, 11);
                sscanf ((const char *) mystr.latin1(), "%d", &agRect.thickNess);
                printf ("%d\n", agRect.thickNess);
	      }
              // option V_SPACE
	      else if (mystr.startsWith (" V_SPACE ") )
	      {
                printf ("  V_Space : ");
                mystr.remove (0, 9);
                sscanf ((const char *)mystr.latin1(), "(%d %d %d)",
                         &agRect.vs[0], &agRect.vs[1], &agRect.vs[2]);
                printf ("%d %d %d\n",
                          agRect.vs[0],  agRect.vs[1],  agRect.vs[2]);
	      }
              // option XYRAD
	      else if (mystr.startsWith (" XYRAD ") )
	      {
                printf ("  XYRad   : ");
                mystr.remove (0, 7);
                sscanf ((const char *)mystr.latin1(), "<%d %d>",
                         &agRect.xr[0], &agRect.xr[1]);
                printf ("%d %d\n",
                          agRect.xr[0],  agRect.xr[1]);
	      }
              // option PNTS
	      else if (mystr.startsWith (" PNTS ") )
	      {
                printf ("  Pnts    : ");
                mystr.remove (0, 6);
                sscanf ((const char *)mystr.latin1(), "(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)",
                        &agRect.ptX[0], &agRect.ptY[0], &agRect.ptX[1], &agRect.ptY[1],
                        &agRect.ptX[2], &agRect.ptY[2], &agRect.ptX[3], &agRect.ptY[3],
                        &agRect.ptX[4], &agRect.ptY[4]);
                printf ("%d %d  %d %d   %d %d  %d %d  %d %d\n",
                        agRect.ptX[0], agRect.ptY[0], agRect.ptX[1], agRect.ptY[1],
                        agRect.ptX[2], agRect.ptY[2], agRect.ptX[3], agRect.ptY[3],
                        agRect.ptX[4], agRect.ptY[4]);
	      }

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

            printf (" Habe ELL erkannt (keine Werte uebernommen)\n");
            mystr.remove (0, 7);
            sscanf ((const char *)mystr.latin1(), "(%d,%d)",
                    &agEll.offX, &agEll.offY);
	    printf ("  Offset ->   x:%2d  y:%2d\n", agEll.offX, agEll.offY);

            do
	    {
              // Akutelle Position bestimmen
              pos = in.at ();
              // read one line
              mystr = stream.readLine ();

              // option RECOLOR
	      if      (mystr.startsWith (" RECOLOR ") )
	      {
                printf ("  Recolor: ");
                mystr.remove (0, 9);
                if      (mystr == "ON")
                {
                  printf ("On\n");
                  agEll.reColor = true;
                }
                else if (mystr == "OFF")
                {
                  printf ("Off\n");
                  agEll.reColor = false;
                }
                else
                {
                   printf ("!!!!!    Whats that <%s>\n", (const char *) mystr.latin1() );
                }
	      }
	      else if (mystr.startsWith (" PNTS ") )
	      {
                printf ("  Pnts   : ");
                mystr.remove (0, 6);
		//
                sscanf ((const char *)mystr.latin1(), "(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)",
                        &agEll.ptX[0], &agEll.ptY[0], &agEll.ptX[1], &agEll.ptY[1],
                        &agEll.ptX[2], &agEll.ptY[2], &agEll.ptX[3], &agEll.ptY[3],
                        &agEll.ptX[4], &agEll.ptY[4]);
                printf ("%d %d  %d %d   %d %d  %d %d  %d %d\n",
                        agEll.ptX[0], agEll.ptY[0], agEll.ptX[1], agEll.ptY[1],
                        agEll.ptX[2], agEll.ptY[2], agEll.ptX[3], agEll.ptY[3],
                        agEll.ptX[4], agEll.ptY[4]);
	      }
	    }
	    while ((mystr[0] != '.') && (mystr[0] != 'E'));

            // An die Position zurueckspringen
            in.at (pos);

            // Werte in die Struktur einlagern
            // ???

	  }
          else
	  {
            printf ("Unbekannt : %s \n", (const char *) mystr.latin1() );
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
    str += " </killustrator>\n";
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

