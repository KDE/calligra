/*
    Copyright (C) 1998 Kai-Uwe Sattler <kus@iti.cs.uni-magdeburg.de>
    Copyright (C) 2001, Rob Buis <rwlbuis@wanadoo.nl>
    Copyright (C) 2003, Rob Buis <buis@kde.org>
    This file is part of the KDE project

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

DESCRIPTION
*/

#include <assert.h>
#include <fstream.h>
#include <limits.h>
#include <math.h>

#include <GDocument.h>
#include <GPage.h>
#include <GGroup.h>
#include <GPolyline.h>
#include <GOval.h>
#include <GPolygon.h>
#include <GText.h>
#include <xfigimport.h>
#include <xfigimport.moc>

#include <q3tl.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <kdebug.h>

#include <core/vdocument.h>
namespace std { };
using namespace std;

#define RAD_FACTOR 180.0 / M_PI

unsigned int colors[] = {
    0x000090,
    0x0000b0,
    0x0000d0,
    0x87ceff,
    0x009000,
    0x00b000,
    0x00d000,
    0x009090,
    0x00b0b0,
    0x00d0d0,
    0x900000,
    0xb00000,
    0xd00000,
    0x900090,
    0xb000b0,
    0xd000d0,
    0x803000,
    0xa04000,
    0xc06000,
    0xff8080,
    0xffa0a0,
    0xffc0c0,
    0xffe0e0,
    0xffd700
};

int arrow_ids[] = {
  6, 1, 2, 7
};

struct PSFont {
  const char* family;
  QFont::Weight weight;
  bool italic;
} psFontTable[] = {
  { "times", QFont::Normal, false },      // Times Roman
  { "times", QFont::Normal, true },       // Times Italic
  { "times", QFont::Bold, false },        // Times Bold
  { "times", QFont::Bold, true },         // Times Bold Italic
  { "avantgarde", QFont::Normal, false },   // AvantGarde Book
  { "avantgarde", QFont::Normal, true },    // AvantGarde Book Oblique
  { "avantgarde", QFont::DemiBold, false }, // AvantGarde Demi
  { "avantgarde", QFont::DemiBold, true },  // AvantGarde Demi Oblique
  { "bookman", QFont::Light, false },     // Bookman Light
  { "bookman", QFont::Light, true },      // Bookman Light Italic
  { "bookman", QFont::DemiBold, false },  // Bookman Demi
  { "bookman", QFont::DemiBold, true },   // Bookman Demi Italic
  { "courier", QFont::Normal, false },    // Courier
  { "courier", QFont::Normal, true },     // Courier Oblique
  { "courier", QFont::Bold, false },      // Courier Bold
  { "courier", QFont::Bold, true },       // Courier Bold Oblique
  { "helvetica", QFont::Normal, false },  // Helvetica
  { "helvetica", QFont::Normal, true },   // Helvetica Oblique
  { "helvetica", QFont::Bold, false },    // Helvetica Bold
  { "helvetica", QFont::Bold, true },     // Helvetica Bold Oblique
  { "helvetica", QFont::Normal, false },  // Helvetica Narrow
  { "helvetica", QFont::Normal, true },   // Helvetica Narrow Oblique
  { "helvetica", QFont::Bold, false },    // Helvetica Narrow Bold
  { "helvetica", QFont::Bold, true },     // Helvetica Narrow Bold Oblique
  { "newcenturyschoolbook", QFont::Normal, false },// New Century Schoolbook
  { "newcenturyschoolbook", QFont::Normal, true }, // New Century Italic
  { "newcenturyschoolbook", QFont::Bold, false },  // New Century Bold
  { "newcenturyschoolbook", QFont::Bold, true },   // New Century Bold Italic
  { "palatino", QFont::Normal, false },   // Palatino Roman
  { "palatino", QFont::Normal, true },    // Palatino Italic
  { "palatino", QFont::Bold, false },     // Palatino Bold
  { "palatino", QFont::Bold, true },      // Palatino Bold Italic
  { "symbol", QFont::Normal, false },     // Symbol
  { "zapfchancery", QFont::Normal, false }, // Zapf Chancery Medium Italic
  { "zapfdingbats", QFont::Normal, false }, // Zapf Dingbats
};

int hexstrToInt (const char *str) {
    const int fak[] = { 16, 1 };
   int value = 0, v;

   for (int i = 0; i < 2; i++) {
       if (str[i] >= '0' && str[i] <= '9')
           v = str[i] - '0';
       else
           v = str[i] - 'a' + 10;
       value += v * fak[i];
   }

   return value;
}

XFIGImport::XFIGImport( QObject* parent ) : KoFilter(parent, name)
{
  fig_resolution = 1200.0 / 72.0;
  coordinate_system = 2;

  colorTable.insert (0, new QColor (Qt::black));
  colorTable.insert (1, new QColor (Qt::blue));
  colorTable.insert (2, new QColor (Qt::green));
  colorTable.insert (3, new QColor (Qt::cyan));
  colorTable.insert (4, new QColor (Qt::red));
  colorTable.insert (5, new QColor (Qt::magenta));
  colorTable.insert (6, new QColor (Qt::yellow));
  colorTable.insert (7, new QColor (Qt::white));

  for (int i = 0; i <= 23; i++)
      colorTable.insert (i + 8, new QColor (colors[i]));

  objList.clear ();
}

XFIGImport::~XFIGImport()
{
}

bool XFIGImport::filterImport( const QString &file, KoDocument *doc,
                              const QString &from, const QString &to,
                              const QString & ) {

    if( to != "application/x-karbon" || from != "image/x-xfig" )
        return false;

  char buf[255];
  int value;
  KoPageLayout layout;

  ifstream fin( file.local8Bit() );
  if (! fin)
    return false;

    KIllustratorDocument *kidoc = (KIllustratorDocument *) doc;
    GDocument *gdoc             = kidoc->gdoc();
    //GPage *activePage           = gdoc->activePage();

  layout = gdoc->activePage()->pageLayout ();

  fin.getline (buf, 255);
  if (::strncmp (buf, "#FIG 3", 6)) {
    kDebug() << "ERROR: no xfig file or wrong header" << endl;
    return false;
  }

  if (buf[7] == '2') {
      version = 320;
  }
  else if (buf[7] == '1') {
      version = 310;
  }
  else {
    kDebug() << "ERROR: unsupported xfig version" << endl;
    return false;
  }

  /*
   * read the header
   */

  // orientation
  fin.getline (buf, 255);
  if (::strcmp (buf, "Landscape") == 0)
      layout.orientation = PG_LANDSCAPE;
  else if (::strcmp (buf, "Portrait") == 0)
      layout.orientation = PG_PORTRAIT;
  else
      kDebug() << "ERROR: invalid orientation" << endl;

  // justification (don't know how to handle this)
  fin.getline (buf, 255);

  // units
  fin.getline (buf, 255);
  if (::strcmp (buf, "Metric") == 0)
      layout.unit = PG_MM;
  else if (::strcmp (buf, "Inches") == 0)
      layout.unit = PG_INCH;
  else
      kDebug() << "ERROR: invalid units" << endl;

  if (version >= 320) {
      // paper size (don't know how to handle this)
      fin.getline (buf, 255);

      // magnification
      float magnification;
      fin >> magnification;
      fin.ignore (INT_MAX, '\n');

      //multiple page (not supported yet)
      fin.getline (buf, 255);

      // transparent color (not supported yet)
      int transColor;
      fin >> transColor;
      fin.ignore (INT_MAX, '\n');
  }

  // resolution and coordinate system
  fin >> value >> coordinate_system;
  fig_resolution = value / 72.0;
  fin.ignore (INT_MAX, '\n');

  // now read in the objects
  while (! fin.atEnd()) {
    int tag = -1;
    fin >> tag;
    if (tag == -1) {
      // EOF
      buildDocument (gdoc);
      return true;
    }

    switch (tag) {
    case 0:
      // a color pseudo object
      parseColorObject (fin);
      break;
    case 1:
      // a ellipse
      parseEllipse (fin, gdoc);
      break;
    case 2:
      // a polyline
      parsePolyline (fin, gdoc);
      break;
    case 3:
      // a spline
      parseSpline (fin, gdoc);
      break;
    case 4:
      // a text
      parseText (fin, gdoc);
      break;
    case 5:
      // an arc
      parseArc (fin, gdoc);
      break;
    case 6:
      // a compound object
      parseCompoundObject (fin, gdoc);
      break;
    case -6:
      // end of compound object --> ignore it
      break;
    default:
      // should not occur
      kDebug() << "unknown object type: " << tag << endl;
      break;
    }
  }
  buildDocument (gdoc);
  return true;
}


void XFIGImport::parseColorObject (istream& fin) {
    int number, Qt::red, Qt::green, Qt::blue;
    char buf[20], red_str[3], green_str[3], blue_str[3];

    fin >> number >> buf;
    strncpy (red_str, &buf[1], 2); red_str[2] = '\0';
    strncpy (green_str, &buf[3], 2); green_str[2] = '\0';
    strncpy (blue_str, &buf[5], 2); blue_str[2] = '\0';

    Qt::red = hexstrToInt (red_str);
    Qt::green = hexstrToInt (green_str);
    Qt::blue = hexstrToInt (blue_str);

    colorTable.insert (number, new QColor (Qt::red, Qt::green, Qt::blue));
}

void XFIGImport::parseArc (istream& fin, GDocument* doc) {
  int sub_type, line_style, thickness, pen_color, fill_color,
    depth, pen_style, area_fill, cap_style, direction,
    forward_arrow, backward_arrow, x1, y1, x2, y2, x3, y3;
  float center_x, center_y;
  float style_val;
  GOval *obj = new GOval (doc);

  // first line
  fin >> sub_type >> line_style >> thickness >> pen_color >> fill_color
      >> depth >> pen_style >> area_fill >> style_val >> cap_style
      >> direction >> forward_arrow >> backward_arrow
      >> center_x >> center_y >> x1 >> y1 >> x2 >> y2 >> x3 >> y3;

  if (forward_arrow > 0) {
    // forward arow line
    fin.ignore (INT_MAX, '\n');
  }

  if (backward_arrow > 0) {
    // backward arrow line
    fin.ignore (INT_MAX, '\n');
  }

  // compute radius
  float dx = x1 - center_x;
  float dy = y1 - center_y;
  float radius = sqrt (dx * dx + dy * dy);

  if (radius==0) {
    delete obj;
    return;
  }

  Coord p1 ((center_x - radius) / fig_resolution,
            (center_y - radius) / fig_resolution);
  Coord p2 ((center_x + radius) / fig_resolution,
            (center_y + radius) / fig_resolution);

  obj->setStartPoint (p1);
  obj->setEndPoint (p2);

  if (sub_type == 0)
      obj->setOutlineShape (GObject::OutlineInfo::PieShape);
  else if (sub_type == 1)
      obj->setOutlineShape (GObject::OutlineInfo::ArcShape);

  p1 = Coord (center_x / fig_resolution, center_y /fig_resolution);
  float m;

  float angle1;
  p2 = Coord (x1 / fig_resolution, y1 /fig_resolution);
  if (p2.x () == p1.x ()) {
    if (p2.y () > p1.y ())
      angle1 = 90;
    else
      angle1 = -90;
  }
  else {
    m = ((p2.y () - p1.y ()) / (p2.x () - p1.x ()));
    if ( p2.x () > p1.x ())
      angle1 = atan (m) * RAD_FACTOR;
    else
      angle1 = 180 + atan (m) * RAD_FACTOR;
  }

  float angle2;
  p2 = Coord (x3 / fig_resolution, y3 /fig_resolution);
  if (p2.x () == p1.x ()) {
    if (p2.y () > p1.y ())
      angle2 = 90;
    else
      angle2 = -90;
  }
  else {
    m = ((p2.y () - p1.y ()) / (p2.x () - p1.x ()));
    if ( p2.x () > p1.x ())
      angle2 = atan (m) * RAD_FACTOR;
    else
      angle2 = 180 + atan (m) * RAD_FACTOR;
  }

  if (direction==0) // clockwise
    obj->setAngles (angle2, angle1);
  else if (direction==1) // counterclockwise
    obj->setAngles (angle1, angle2);

  // now set the properties
  setProperties (obj, pen_color, pen_style, thickness, area_fill, fill_color);
  objList.append( GObjectListItem( depth, obj ) );
}

void XFIGImport::parseEllipse (istream& fin, GDocument* doc) {
  int sub_type, line_style, thickness, pen_color, fill_color,
    depth, pen_style, area_fill, direction, center_x, center_y,
    radius_x, radius_y, start_x, start_y, end_x, end_y;
  float style_val, angle;
  GOval *obj = new GOval (doc);

  // first line
  fin >> sub_type >> line_style >> thickness >> pen_color >> fill_color
      >> depth >> pen_style >> area_fill >> style_val >> direction
      >> angle >> center_x >> center_y >> radius_x >> radius_y
      >> start_x >> start_y >> end_x >> end_y;
  Coord p1, p2;

  p1 = Coord ((center_x - radius_x) /fig_resolution,
              (center_y - radius_y) /fig_resolution);
  p2 = Coord ((center_x + radius_x) /fig_resolution,
              (center_y + radius_y) /fig_resolution);

  obj->setStartPoint (p1);
  obj->setEndPoint (p2);

  // now set the properties
  setProperties (obj, pen_color, pen_style, thickness, area_fill, fill_color);
  objList.append( GObjectListItem( depth, obj ) );
}

void XFIGImport::parsePolyline (istream& fin, GDocument* doc) {
  int sub_type, line_style, thickness, pen_color, fill_color,
    depth, pen_style, area_fill, join_style, cap_style, radius,
    forward_arrow, backward_arrow, npoints;
  float style_val;
  GPolyline *obj = NULL;

  // first line
  fin >> sub_type >> line_style >> thickness >> pen_color >> fill_color
      >> depth >> pen_style >> area_fill >> style_val >> join_style
      >> cap_style >> radius >> forward_arrow >> backward_arrow
      >> npoints;
  fin.ignore (INT_MAX, '\n');

  switch (sub_type) {
  case 1: // polyline
    obj = new GPolyline (doc);
    break;
  case 2: // box
    obj = new GPolygon (doc);
    break;
  case 3: // polygon
    obj = new GPolygon (doc);
    break;
  case 4: // arc-box
    obj = new GPolygon (doc);
    break;
  case 5: // imported picture
    return;
    break;
  default:
    // doesn't occur
    kDebug() << "unknown subtype: " << sub_type << endl;
    break;
  }

  assert (obj != NULL);

  int arrow_type, arrow_style;
  float arrow_thickness, arrow_width, arrow_height;
  GObject::OutlineInfo oinfo;
  oinfo.mask = GObject::OutlineInfo::Custom;
  oinfo.startArrowId = oinfo.endArrowId = 0;

  if (forward_arrow > 0) {

    // forward arrow line
    fin >> arrow_type >> arrow_style >> arrow_thickness
        >> arrow_width >> arrow_height;
    oinfo.endArrowId = arrow_ids[arrow_type];
    if (oinfo.endArrowId == 1 && arrow_style == 0)
      oinfo.endArrowId = 4;
    fin.ignore (INT_MAX, '\n');
  }

  if (backward_arrow > 0) {
    // backward arrow line
    fin >> arrow_type >> arrow_style >> arrow_thickness
        >> arrow_width >> arrow_height;
    oinfo.startArrowId = arrow_ids[arrow_type];
    if (oinfo.startArrowId == 1 && arrow_style == 0)
      oinfo.startArrowId = 4;
    fin.ignore (INT_MAX, '\n');
  }
  // points line
  for (int i = 0; i < npoints; i++) {
    int x, y;
    fin >> x >> y;
    if ((sub_type == 2 || sub_type == 3) && i == npoints -1)
      // first point == last point
      break;

    Coord p (x / fig_resolution, y / fig_resolution);
    obj->_addPoint (i, p);
  }

  if (oinfo.startArrowId || oinfo.endArrowId)
    obj->setOutlineInfo (oinfo);

  // now set the properties
  setProperties (obj, pen_color, line_style, thickness, area_fill, fill_color);

  // and insert the object
  objList.append( GObjectListItem( depth, obj ) );
}

void XFIGImport::parseSpline (istream& fin, GDocument* doc)
{
  int sub_type, line_style, thickness, pen_color, fill_color, depth,
    pen_style, area_fill, cap_style, forward_arrow, backward_arrow, npoints;
  float style_val;

  // this should be a spline
  GPolyline *obj = 0L;
  fin >> sub_type >> line_style >> thickness >> pen_color >> fill_color
      >>  depth >> pen_style >> area_fill >> style_val >> cap_style
      >> forward_arrow >> backward_arrow >> npoints;
  if (sub_type == 1 || sub_type == 3 || sub_type == 5)
    obj = new GPolygon (doc);
  else
    obj = new GPolyline (doc);

  int arrow_type, arrow_style;
  float arrow_thickness, arrow_width, arrow_height;
  GObject::OutlineInfo oinfo;
  oinfo.mask = GObject::OutlineInfo::Custom;
  oinfo.startArrowId = oinfo.endArrowId = 0;

  if (forward_arrow > 0) {

    // forward arrow line
    fin >> arrow_type >> arrow_style >> arrow_thickness
        >> arrow_width >> arrow_height;
    oinfo.endArrowId = arrow_ids[arrow_type];
    if (oinfo.endArrowId == 1 && arrow_style == 0)
      oinfo.endArrowId = 4;
    fin.ignore (INT_MAX, '\n');
  }

  if (backward_arrow > 0) {
    // backward arrow line
    fin >> arrow_type >> arrow_style >> arrow_thickness
        >> arrow_width >> arrow_height;
    oinfo.startArrowId = arrow_ids[arrow_type];
    if (oinfo.startArrowId == 1 && arrow_style == 0)
      oinfo.startArrowId = 4;
    fin.ignore (INT_MAX, '\n');
  }

  // points line
  for (int i = 0; i < npoints; i++) {
    int x, y;
    fin >> x >> y;
    if ((sub_type == 1 || sub_type == 3 || sub_type == 5) && i == npoints -1)
      // first point == last point
      break;

    Coord p (x / fig_resolution, y / fig_resolution);
    obj->_addPoint (i, p);
  }

  // control points line
  for (int i = 0; i < npoints; i++) {
    float fac;
    fin >> fac;
    // ignore it now
    // fin.ignore (INT_MAX, '\n');
  }

  if (oinfo.startArrowId || oinfo.endArrowId)
    obj->setOutlineInfo (oinfo);

  // now set the properties
  setProperties (obj, pen_color, line_style, thickness, area_fill, fill_color);

  // and insert the object
  objList.append( GObjectListItem( depth, obj ) );
}

void XFIGImport::parseText (istream& fin, GDocument* doc)
{
  int sub_type, color, depth, pen_style, font, font_flags, x, y;
  float font_size, angle, height, length;
  GText *obj = new GText (doc);
  char c;
  char ocode[4];
  bool finished = false;
  QString text;
  QFont qfont;

  fin >> sub_type >> color >> depth >> pen_style >> font >> font_size
      >> angle >> font_flags >> height >> length >> x >> y;

  if (font_flags & 4) {
    // PostScript font
    if (font == -1)
      font = 0;
    qfont = QFont (psFontTable[font].family, qRound (font_size),
                   psFontTable[font].weight, psFontTable[font].italic);
  }
  else {
    // LaTeX font
    switch (font) {
    case 1: // Roman
      qfont.setFamily ("times");
      break;
    case 2: // Bold
      qfont.setBold (true);
      break;
    case 3: // Italic
      qfont.setItalic (true);
      break;
    case 4: // Sans Serif
      qfont.setFamily ("helvetica");
      break;
    case 5: // Typewriter
      qfont.setFamily ("Courier");
      break;
    default:
      break;
    }
  }
  qfont.setPointSize (qRound (font_size));
  obj->setFont (qfont);

  while (! finished) {
    fin.get (c);
    if (c == '\\') {
      fin.get (ocode, 4);
      int code = (ocode[0] - '0') * 64 +
        (ocode[1] - '0') * 8 +
        (ocode[2] - '0');
      if (code == 1)
        finished = true;
      else
        text += (char) code;
    }
    else
      text += c;
  }
  obj->setText (text);

  if (sub_type == 1) {
    GText::TextInfo ti = obj->getTextInfo ();
    ti.align = GText::TextInfo::AlignCenter;
    obj->setTextInfo (ti);
  }
  else if (sub_type == 2) {
    GText::TextInfo ti = obj->getTextInfo ();
    ti.align = GText::TextInfo::AlignRight;
    obj->setTextInfo (ti);
  }
  Coord origin (x / fig_resolution, y / fig_resolution - qfont.pointSize ());
  obj->setOrigin (origin);

  if (angle != 0) {
    // rotate the text
    float nangle = angle * RAD_FACTOR;
    QMatrix m1, m2, m3;
    Coord rotCenter;

    if (sub_type == 0) {
      rotCenter = Coord (obj->boundingBox ().left (),
                         obj->boundingBox ().bottom ());
    }
    else if (sub_type == 1) {
      rotCenter = Coord (obj->boundingBox ().width () / 2,
                         obj->boundingBox ().bottom ());
    }
    else if (sub_type == 2) {
      rotCenter = Coord (obj->boundingBox ().right (),
                         obj->boundingBox ().bottom ());
    }
    m1.translate (-rotCenter.x (), -rotCenter.y ());
    m2.rotate (-nangle);
    m3.translate (rotCenter.x (), rotCenter.y ());
    obj->transform (m1);
    obj->transform (m2);
    obj->transform (m3, true);
  }

  objList.append( GObjectListItem( depth, obj ) );
}

void XFIGImport::parseCompoundObject (istream& fin, GDocument* /*doc*/) {
  int upperright_x, upperright_y, lowerleft_x, lowerleft_y;

  fin >> upperright_x >> upperright_y >> lowerleft_x >> lowerleft_y;
  fin.ignore (INT_MAX, '\n');
}


/**
 * Copy all parsed objects from the sorted list to the document.
 */
void XFIGImport::buildDocument (GDocument *doc) {
  doc->setAutoUpdate (false);
  // This will sort all object, by decreasing depth
  qBubbleSort(objList);

  // Now all we need to do is insert them in the document, in that order
  Q3ValueList<GObjectListItem>::Iterator it=objList.begin();
  for ( ; it != objList.end() ; ++it )
  {
      //kDebug() << "Inserting object with depth=" << (*it).depth << endl;
      GObject* obj = (*it).object;
      obj->ref ();
      doc->activePage()->insertObject (obj);
  }
  doc->setAutoUpdate (true);
  objList.clear(); // save memory
}

void XFIGImport::setProperties (GObject* obj, int pen_color, int style,
                                int thickness, int area_fill, int fill_color) {
  if (pen_color >= 0)
    obj->setOutlineColor (*colorTable[pen_color]);

  if (style < 1)
    obj->setOutlineStyle (Qt::SolidLine);
  else if (style == 1)
    obj->setOutlineStyle (Qt::DashLine);
  else if (style == 2)
    obj->setOutlineStyle (Qt::DotLine);

  obj->setOutlineWidth (thickness * 72.0 / 80.0);

  if (area_fill == -1)
    obj->setFillStyle (GObject::FillInfo::NoFill);
  else {
    obj->setFillStyle (GObject::FillInfo::SolidFill);
    if (fill_color < 1) {
      // for BLACK or DEFAULT color
      int val = qRound ((20 - area_fill) * 255.0 / 20.0);
      obj->setFillColor (QColor (val, val, val));
    }
    else if (fill_color == 7) {
      // for WHITE color
      int val = qRound ( area_fill * 255.0 / 20.0);
      obj->setFillColor (QColor (val, val, val));
    }
    else
      obj->setFillColor (*colorTable[fill_color]);
  }
}

