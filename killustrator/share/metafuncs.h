/* WMF Metafile Function Description Table
 * Author: Stefan Taferner <taferner@kde.org>
 */
#ifndef metafunc_h
#define metafunc_h

class QWinMetaFile;

struct MetaFuncRec
{
  const char* name;
  unsigned short func;
  void (QWinMetaFile::*method)(short,short*);
} metaFuncTab[] =
{
  { "SETBKCOLOR",           0x0201, &QWinMetaFile::setBkColor },
  { "SETBKMODE",            0x0102, &QWinMetaFile::setBkMode },
  { "SETMAPMODE",           0x0103, &QWinMetaFile::noop },
  { "SETROP2",              0x0104, &QWinMetaFile::setRop },
  { "SETRELABS",            0x0105, 0 },
  { "SETPOLYFILLMODE",      0x0106, &QWinMetaFile::setPolyFillMode },
  { "SETSTRETCHBLTMODE",    0x0107, 0 },
  { "SETTEXTCHAREXTRA",     0x0108, 0 },
  { "SETTEXTCOLOR",         0x0209, 0 },
  { "SETTEXTJUSTIFICATION", 0x020A, 0 },
  { "SETWINDOWORG",         0x020B, &QWinMetaFile::setWindowOrg },
  { "SETWINDOWEXT",         0x020C, &QWinMetaFile::setWindowExt },
  { "SETVIEWPORTORG",       0x020D, 0 },
  { "SETVIEWPORTEXT",       0x020E, 0 },
  { "OFFSETWINDOWORG",      0x020F, 0 },
  { "SCALEWINDOWEXT",       0x0410, 0 },
  { "OFFSETVIEWPORTORG",    0x0211, 0 },
  { "SCALEVIEWPORTEXT",     0x0412, 0 },
  { "LINETO",               0x0213, &QWinMetaFile::lineTo },
  { "MOVETO",               0x0214, &QWinMetaFile::moveTo },
  { "EXCLUDECLIPRECT",      0x0415, 0 },
  { "INTERSECTCLIPRECT",    0x0416, 0 },
  { "ARC",                  0x0817, 0 },
  { "ELLIPSE",              0x0418, &QWinMetaFile::ellipse },
  { "FLOODFILL",            0x0419, 0 },
  { "PIE",                  0x081A, 0 },
  { "RECTANGLE",            0x041B, 0 },
  { "ROUNDRECT",            0x061C, 0 },
  { "PATBLT",               0x061D, 0 },
  { "SAVEDC",               0x001E, &QWinMetaFile::saveDC },
  { "SETPIXEL",             0x041F, 0 },
  { "OFFSETCLIPRGN",        0x0220, 0 },
  { "TEXTOUT",              0x0521, 0 },
  { "BITBLT",               0x0922, 0 },
  { "STRETCHBLT",           0x0B23, 0 },
  { "POLYGON",              0x0324, &QWinMetaFile::polygon },
  { "POLYLINE",             0x0325, &QWinMetaFile::polyline },
  { "ESCAPE",               0x0626, &QWinMetaFile::escape },
  { "RESTOREDC",            0x0127, &QWinMetaFile::restoreDC },
  { "FILLREGION",           0x0228, 0 },
  { "FRAMEREGION",          0x0429, 0 },
  { "INVERTREGION",         0x012A, 0 },
  { "PAINTREGION",          0x012B, 0 },
  { "SELECTCLIPREGION",     0x012C, 0 },
  { "SELECTOBJECT",         0x012D, &QWinMetaFile::selectObject },
  { "SETTEXTALIGN",         0x012E, 0 },
  { "CHORD",                0x0830, 0 },
  { "SETMAPPERFLAGS",       0x0231, 0 },
  { "EXTTEXTOUT",           0x0a32, 0 },
  { "SETDIBTODEV",          0x0d33, 0 },
  { "SELECTPALETTE",        0x0234, 0 },
  { "REALIZEPALETTE",       0x0035, 0 },
  { "ANIMATEPALETTE",       0x0436, 0 },
  { "SETPALENTRIES",        0x0037, 0 },
  { "POLYPOLYGON",          0x0538, &QWinMetaFile::polypolygon },
  { "RESIZEPALETTE",        0x0139, 0 },
  { "DIBBITBLT",            0x0940, 0 },
  { "DIBSTRETCHBLT",        0x0b41, 0 },
  { "DIBCREATEPATTERNBRUSH",0x0142, 0 },
  { "STRETCHDIB",           0x0f43, 0 },
  { "EXTFLOODFILL",         0x0548, 0 },
  { "DELETEOBJECT",         0x01f0, &QWinMetaFile::deleteObject },
  { "CREATEPALETTE",        0x00f7, 0 },
  { "CREATEPATTERNBRUSH",   0x01F9, 0 },
  { "CREATEPENINDIRECT",    0x02FA, &QWinMetaFile::createPenIndirect },
  { "CREATEFONTINDIRECT",   0x02FB, 0 },
  { "CREATEBRUSHINDIRECT",  0x02FC, &QWinMetaFile::createBrushIndirect },
  { "CREATEREGION",         0x06FF, 0 },
  { NULL,                   0,      0 }
};

#endif /*metafunc_h*/
