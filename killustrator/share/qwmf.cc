/* Windows Meta File Loader/Painter Class Implementation
 *
 * Copyright (C) 1998 Stefan Taferner
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABLILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details. You should have received a copy
 * of the GNU General Public License along with this program; if not, write
 * to the Free Software Foundation, Inc, 675 Mass Ave, Cambridge, MA 02139,
 * USA.
 */

#include <qfile.h>
#include <qfileinfo.h>
#include <qdatastream.h>
#include <assert.h>
#include <qapplication.h>
#include <qpixmap.h>

bool qwmfDebug = FALSE;

#include <qwmf.h>
#include <wmfstruct.h>
#include <metafuncs.h>
#include <kdebug.h>

#define ABS(x) ((x)>=0?(x):-(x))

class WmfCmd
{
public:
  ~WmfCmd() { if (next) delete next; }
  WmfCmd* next;
  short funcIndex;
  WORD numParm;
  WORD* parm;
};


class WinObjHandle
{
public:
  virtual ~WinObjHandle () {}
  virtual void apply(QPainter& p) = 0;
};

class WinObjBrushHandle: public WinObjHandle
{
public:
  virtual void apply(QPainter& p);
  QBrush brush;
};

class WinObjPenHandle: public WinObjHandle
{
public:
  virtual void apply(QPainter& p);
  QPen pen;
};

void WinObjBrushHandle::apply(QPainter& p)
{
  QPen pen;
  pen.setColor(brush.color());

  p.setBrush(brush);
  p.setPen(pen);
}

void WinObjPenHandle::apply(QPainter& p)
{
  p.setPen(pen);
}

#define MAX_OBJHANDLE 64




//-----------------------------------------------------------------------------
QWinMetaFile::QWinMetaFile(): mPainter(), mPoints()
{
  mIsPlaceable = FALSE;
  mIsEnhanced  = FALSE;
  mFirstCmd    = NULL;
  mSingleStep  = FALSE;
  mObjHandleTab= NULL;
  mCalcBBox    = FALSE;
}


//-----------------------------------------------------------------------------
QWinMetaFile::~QWinMetaFile()
{
  if (mFirstCmd) delete mFirstCmd;
  if (mObjHandleTab) delete[] mObjHandleTab;
}


//-----------------------------------------------------------------------------
void QWinMetaFile::singleStep(bool ss)
{
  mSingleStep = ss;
}

//-----------------------------------------------------------------------------
bool QWinMetaFile::load(const QString &aFileName)
{
  QFile file(aFileName);
  QFileInfo fi(file);
  QDataStream st;
  WmfEnhMetaHeader eheader;
  WmfMetaHeader header;
  WmfPlaceableHeader pheader;
  WORD checksum;
  int filePos, idx, i;
  WmfCmd *cmd, *last;
  DWORD rdSize;
  WORD rdFunc;

  if (mFirstCmd) delete mFirstCmd;
  mFirstCmd = NULL;

  if (!file.exists())
  {
      kdWarning(38000) << "File " << aFileName << " does not exist" << endl;
      return FALSE;
  }
  if (!file.open(IO_ReadOnly))
  {
      kdWarning(38000) << "Cannot open file " << aFileName << endl;
      return FALSE;
  }
  st.setDevice(&file);
  st.setByteOrder(QDataStream::LittleEndian); // Great, I love Qt !

  //----- Read placeable metafile header
  st >> pheader.key;
  mIsPlaceable = (pheader.key==(DWORD) APMHEADER_KEY);
  if (mIsPlaceable)
  {
    st >> pheader.hmf;
    st >> pheader.bbox.left;
    st >> pheader.bbox.top;
    st >> pheader.bbox.right;
    st >> pheader.bbox.bottom;
    st >> pheader.inch;
    st >> pheader.reserved;
    st >> pheader.checksum;
    checksum = calcCheckSum(&pheader);
    if (pheader.checksum!=checksum) mIsPlaceable = FALSE;

    if (mSingleStep)
    {
        kdDebug(38000) << "WMF Placeable Header ( << " <<  sizeof(pheader) << "): " << endl;
        kdDebug(38000) << "  key=" << pheader.key << endl;
        kdDebug(38000) << "  hmf=" << pheader.hmf << endl;
        kdDebug(38000) << "  bbox=(" << pheader.bbox.left << "; " << pheader.bbox.top << "; "
                  <<  pheader.bbox.right << "; " << pheader.bbox.bottom << ")" << endl;
        kdDebug(38000) << "  inch=" << pheader.inch << endl;
        kdDebug(38000) << "  checksum=" << pheader.checksum << " (" << (pheader.checksum==checksum?"ok":"wrong") << ")" << endl;
    }

    mCalcBBox = FALSE;
    mDpi = pheader.inch;
    mBBox.setLeft(QMIN(pheader.bbox.left,pheader.bbox.right));
    mBBox.setTop(QMIN(pheader.bbox.top,pheader.bbox.bottom));
    mBBox.setWidth(ABS(pheader.bbox.right - pheader.bbox.left));
    mBBox.setHeight(ABS(pheader.bbox.bottom - pheader.bbox.top));
  }
  else
  {
    mCalcBBox = TRUE;
    mDpi = 576;
    file.at(0);
    mBBox.setRect(0,0,0,0);
  }

  //----- Read as enhanced metafile header
  filePos = file.at();
  st >> eheader.iType;
  st >> eheader.nSize;
  st >> eheader.rclBounds.left;
  st >> eheader.rclBounds.top;
  st >> eheader.rclBounds.right;
  st >> eheader.rclBounds.bottom;
  st >> eheader.rclFrame.left;
  st >> eheader.rclFrame.top;
  st >> eheader.rclFrame.right;
  st >> eheader.rclFrame.bottom;
  st >> eheader.dSignature;
  mIsEnhanced = (eheader.dSignature==ENHMETA_SIGNATURE);
  if (mIsEnhanced) // is it really enhanced ?
  {
    st >> eheader.nVersion;
    st >> eheader.nBytes;
    st >> eheader.nRecords;
    st >> eheader.nHandles;
    st >> eheader.sReserved;
    st >> eheader.nDescription;
    st >> eheader.offDescription;
    st >> eheader.nPalEntries;
    st >> eheader.szlDevice;
    st >> eheader.szlMillimeters;

    /*
    debug("WMF Extended Header");
    if (mSingleStep)
    {
      debug("  iType=%d", eheader.iType);
      debug("  nSize=%d", eheader.nSize);
      debug("  rclBounds=(%ld;%ld;%ld;%ld)",
            eheader.rclBounds.left, eheader.rclBounds.top,
            eheader.rclBounds.right, eheader.rclBounds.bottom);
      debug("  rclFrame=(%ld;%ld;%ld;%ld)",
            eheader.rclFrame.left, eheader.rclFrame.top,
            eheader.rclFrame.right, eheader.rclFrame.bottom);
      debug("  dSignature=%d", eheader.dSignature);
      debug("  nVersion=%d", eheader.nVersion);
      debug("  nBytes=%d", eheader.nBytes);
    }
    debug("NOT YET IMPLEMENTED, SORRY.");
    */
    return FALSE;
  }
  else // no, not enhanced
  {
    //    debug("WMF Header");
    //----- Read as enhanced metafile header
    file.at(filePos);
    st >> header.mtType;
    st >> header.mtHeaderSize;
    st >> header.mtVersion;
    st >> header.mtSize;
    st >> header.mtNoObjects;
    st >> header.mtMaxRecord;
    st >> header.mtNoParameters;
    /*
    if (mSingleStep)
    {
      debug("  mtType=%u", header.mtType);
      debug("  mtHeaderSize=%u", header.mtHeaderSize);
      debug("  mtVersion=%u", header.mtVersion);
      debug("  mtSize=%ld", header.mtSize);
    }
    */
  }

  //----- Read bits
  last = NULL;
  while (!st.eof())
  {
    st >> rdSize;
    st >> rdFunc;
    if (rdFunc==0) break;

    idx = findFunc(rdFunc);
    rdSize -= 3;

    cmd = new WmfCmd;
    cmd->next = NULL;
    if (last) last->next = cmd;
    else mFirstCmd = cmd;
    cmd->funcIndex = idx;
    cmd->numParm = rdSize;
    cmd->parm = new WORD[rdSize];
    last = cmd;

    for (i=0; i<rdSize && !st.eof(); i++)
      st >> cmd->parm[i];

    if (i<rdSize)
    {
        kdDebug(38000) << "file truncated: " << aFileName << endl;
      return FALSE;
    }
  }

  file.close();
  return TRUE;
}


//-----------------------------------------------------------------------------
unsigned short QWinMetaFile::calcCheckSum(WmfPlaceableHeader* apmfh)
{
  WORD*  lpWord;
  WORD   wResult, i;

  // Start with the first word
  wResult = *(lpWord = (WORD*)(apmfh));
  // XOR in each of the other 9 words
  for(i=1;i<=9;i++)
  {
    wResult ^= lpWord[i];
  }
  return wResult;
}


//-----------------------------------------------------------------------------
int QWinMetaFile::findFunc(unsigned short aFunc) const
{
  int i;

  for (i=0; metaFuncTab[i].name; i++)
    if (metaFuncTab[i].func == aFunc) return i;

  return -1;
}


//-----------------------------------------------------------------------------
bool QWinMetaFile::paint(const QPaintDevice* aTarget)
{
  int idx, i/*, size*/;
  WmfCmd* cmd;
  //  char dummy[16];

  assert(aTarget!=NULL);
  if (mPainter.isActive()) return FALSE;

  if (mObjHandleTab) delete[] mObjHandleTab;
  mObjHandleTab = new WinObjHandle*[MAX_OBJHANDLE];
  for (i=MAX_OBJHANDLE-1; i>=0; i--)
    mObjHandleTab[i] = NULL;

  mPainter.resetXForm();
  mWinding = FALSE;

  mPainter.begin(aTarget);
  for (cmd=mFirstCmd; cmd; cmd=cmd->next)
  {
    idx = cmd->funcIndex;
    if (idx < 0)
    {
        kdWarning(38000) << "invalid index " << idx << endl;
      continue;
    }

    if (mSingleStep || metaFuncTab[idx].method==0)
    {
      fprintf(stderr, "QWinMetaFile: %s%s",
              metaFuncTab[idx].method==0?"unimplemented ":"",
              metaFuncTab[idx].name);
      for (i=0; i<cmd->numParm; i++)
        fprintf(stderr," %d", cmd->parm[i]);

      if (mSingleStep)
      {
        fflush(stderr);
        qApp->processEvents(500);
        //      gets(dummy);
      }
      else fprintf(stderr, "\n");
    }
    if (metaFuncTab[idx].method!=0)
    {
      (this->*(metaFuncTab[idx].method))(cmd->numParm, cmd->parm);
      if (mSingleStep) qApp->processEvents(1000);
    }
  }
  mPainter.end();

  if (mCalcBBox)
  {
    int a, b;
    a = mBBox.left();
    b = mBBox.right();
    mBBox.setLeft(QMIN(a, b));
    mBBox.setWidth(ABS(b - a));
    a = mBBox.top();
    b = mBBox.bottom();
    mBBox.setTop(QMIN(a, b));
    mBBox.setHeight(ABS(b - a));
  }

  return TRUE;
}


//-----------------------------------------------------------------------------
int QWinMetaFile::handleIndex(void) const
{
  int i;
  for (i=0; i<MAX_OBJHANDLE; i++)
    if (mObjHandleTab[i]==NULL) return i;

  kdDebug(38000) << "QWinMetaFile error: handle table full !" << endl;
  return -1;
}


//-----------------------------------------------------------------------------
WinObjPenHandle* QWinMetaFile::createPen(void)
{
  WinObjPenHandle* handle = new WinObjPenHandle;
  int idx = handleIndex();
  if (idx >= 0) mObjHandleTab[idx] = handle;
  return handle;
}


//-----------------------------------------------------------------------------
WinObjBrushHandle* QWinMetaFile::createBrush(void)
{
  WinObjBrushHandle* handle = new WinObjBrushHandle;
  int idx = handleIndex();
  if (idx >= 0) mObjHandleTab[idx] = handle;
  return handle;
}


//-----------------------------------------------------------------------------
void QWinMetaFile::deleteHandle(int idx)
{
  if (idx >= 0 && idx < MAX_OBJHANDLE && mObjHandleTab[idx])
  {
    delete mObjHandleTab[idx];
    mObjHandleTab[idx] = NULL;
  }
}


//-----------------------------------------------------------------------------
void QWinMetaFile::bboxCoords(int x, int y)
{
  if (mBBox.left() > x) mBBox.setLeft(x);
  else if (mBBox.right() < x) mBBox.setRight(x);
  if (mBBox.top() > y) mBBox.setTop(y);
  else if (mBBox.bottom() < y) mBBox.setBottom(y);
}


//-----------------------------------------------------------------------------
QPointArray* QWinMetaFile::pointArray(short num, short* parm)
{
  int i;

  mPoints.resize(num);

  for (i=0; i<num; i++, parm+=2)
    mPoints.setPoint(i, parm[0], parm[1]);

  return &mPoints;
}

//-----------------------------------------------------------------------------
unsigned int QWinMetaFile::toDWord(short* parm)
{
  unsigned int l;

#if defined(LITTLE_ENDIAN)
  l = *(unsigned int*)parm;
#else
  char swap[4];
  char *bytes;
  bytes = (char*)parm;
  swap[0] = bytes[2];
  swap[1] = bytes[3];
  swap[2] = bytes[0];
  swap[3] = bytes[1];
  l = *(unsigned int*)swap;
#endif

  return l;
}


//-----------------------------------------------------------------------------
QColor QWinMetaFile::color(short* parm)
{
  unsigned int colorRef;
  int red, green, blue;

  colorRef = toDWord(parm) & 0xffffff;
  red      = colorRef & 255;
  green    = (colorRef>>8) & 255;
  blue     = (colorRef>>16) & 255;

  return QColor(red, green, blue);
}


//-----------------------------------------------------------------------------
void QWinMetaFile::setWindowOrg(short , short* parm)
{
  QRect r = mPainter.window();
  mPainter.setWindow(parm[1],parm[0],r.width(),r.height());
  if (mCalcBBox)
  {
    mBBox.setLeft(parm[1]);
    mBBox.setTop(parm[0]);
  }
}


//-----------------------------------------------------------------------------
void QWinMetaFile::setWindowExt(short, short* parm)
{
  QRect r = mPainter.window();
  mPainter.setWindow(r.left(),r.top(),parm[1],parm[0]);
  if (mCalcBBox)
  {
    mBBox.setWidth(parm[1]);
    mBBox.setHeight(parm[0]);
  }
}


//-----------------------------------------------------------------------------
void QWinMetaFile::lineTo(short, short* parm)
{
  mPainter.lineTo(parm[0],parm[1]);
}


//-----------------------------------------------------------------------------
void QWinMetaFile::moveTo(short, short* parm)
{
  mPainter.moveTo(parm[0],parm[1]);
}


//-----------------------------------------------------------------------------
void QWinMetaFile::selectObject(short, short* parm)
{
  int idx = parm[0];
  if (idx>=0 && idx < MAX_OBJHANDLE && mObjHandleTab[idx])
    mObjHandleTab[idx]->apply(mPainter);
}


//-----------------------------------------------------------------------------
void QWinMetaFile::deleteObject(short, short* parm)
{
  deleteHandle(parm[0]);
}


//-----------------------------------------------------------------------------
void QWinMetaFile::ellipse(short, short* parm)
{
  mPainter.drawEllipse(parm[0],parm[1],parm[2]-parm[0],parm[3]-parm[1]);
}


//-----------------------------------------------------------------------------
void QWinMetaFile::polypolygon(short num, short* parm)
{
  QPointArray* pa;
  int i;
  int polyCount = parm[0];
  int *vertices = new int[polyCount];
  bool bgMode = FALSE;
  Qt::RasterOp rop = mPainter.rasterOp();
  QBrush fgBrush(mPainter.brush());
  QBrush bgBrush(mPainter.brush());
  bgBrush.setColor(mPainter.backgroundColor());

  parm++;
  for (i=0,num--; i<polyCount; i++)
  {
    vertices[i] = *parm++;
    num--;
  }

  for (i=0; i<polyCount; i++)
  {
    pa = pointArray(vertices[i], parm);
    mPainter.drawPolygon(*pa, mWinding);
    parm += vertices[i]<<1;
    if (mSingleStep)
    {
        kdDebug(38000) <<*parm << endl;
        fflush(stderr);
        qApp->processEvents(100);
        //      gets(dummy);
    }
    if (bgMode) mPainter.setBrush(fgBrush);
    else mPainter.setBrush(bgBrush);
    bgMode = !bgMode;
    // if (mPainter.rasterOp()==rop) mPainter.setRasterOp(NotCopyROP);
    // else mPainter.setRasterOp(rop);
  }
  mPainter.setRasterOp(rop);
  delete [] vertices;
}


//-----------------------------------------------------------------------------
void QWinMetaFile::polygon(short /*num*/, short* parm)
{
  QPointArray* pa;

  pa = pointArray(parm[0], parm+1);
  mPainter.drawPolygon(*pa, mWinding);
}


//-----------------------------------------------------------------------------
void QWinMetaFile::polyline(short /*num*/, short* parm)
{
  QPointArray* pa;

  pa = pointArray(parm[0], parm+1);
  mPainter.drawPolyline(*pa);
}


//-----------------------------------------------------------------------------
void QWinMetaFile::setPolyFillMode(short /*num*/, short* parm)
{
  mWinding = parm[0];
}


//-----------------------------------------------------------------------------
void QWinMetaFile::escape(short /*num*/, short* parm)
{
  if (parm[0]==15)
  {
    // simply ignore comments
  }
  else kdDebug(38000) << "QWinMetaFile: unimplemented ESCAPE command " << parm[0] << endl;
}


//-----------------------------------------------------------------------------
void QWinMetaFile::setBkColor(short /*num*/, short* parm)
{
  mPainter.setBackgroundColor(color(parm));
}


//-----------------------------------------------------------------------------
void QWinMetaFile::setBkMode(short /*num*/, short* parm)
{
  if (parm[0]==1) mPainter.setBackgroundMode(Qt::TransparentMode);
  else mPainter.setBackgroundMode(Qt::OpaqueMode);
}


//-----------------------------------------------------------------------------
void QWinMetaFile::setRop(short /*num*/, short* parm)
{
  Qt::RasterOp opTab[] =
  {
    Qt::CopyROP/*none*/,
    Qt::CopyROP, Qt::CopyROP, Qt::EraseROP, Qt::NotCopyROP,    /*  1...4 */
    Qt::CopyROP, Qt::NotROP, Qt::XorROP, Qt::CopyROP,          /*  5...8 */
    Qt::NotEraseROP, Qt::NotXorROP, Qt::CopyROP, Qt::NotOrROP, /*  9..12 */
    Qt::CopyROP, Qt::CopyROP, Qt::OrROP, Qt::CopyROP           /* 13..16 */
  };
  Qt::RasterOp rop;

  if (parm[0]>0 && parm[0]<=16) rop = opTab[parm[0]];
  else rop = Qt::CopyROP;

  mPainter.setRasterOp(rop);
}


//-----------------------------------------------------------------------------
void QWinMetaFile::createBrushIndirect(short /*num*/, short* parm)
{
  static Qt::BrushStyle hatchedStyleTab[] =
  {
    Qt::HorPattern,
    Qt::FDiagPattern,
    Qt::BDiagPattern,
    Qt::CrossPattern,
    Qt::DiagCrossPattern
  };
  static Qt::BrushStyle styleTab[] =
  { Qt::SolidPattern,
    Qt::NoBrush,
    Qt::FDiagPattern,   /* hatched */
    Qt::Dense4Pattern,  /* should be custom bitmap pattern */
    Qt::HorPattern,     /* should be BS_INDEXED (?) */
    Qt::VerPattern,     /* should be device-independend bitmap */
    Qt::Dense6Pattern,  /* should be device-independend packed-bitmap */
    Qt::Dense2Pattern,  /* should be BS_PATTERN8x8 */
    Qt::Dense3Pattern   /* should be device-independend BS_DIBPATTERN8x8 */
  };
  Qt::BrushStyle style;
  short arg;
  WinObjBrushHandle* handle = createBrush();

  arg = parm[0];
  if (arg==2)
  {
    arg = parm[3];
    if (arg>=0 && arg<6) style = hatchedStyleTab[arg];
    else
    {
        kdDebug(38000) << "QWinMetaFile::createBrushIndirect: invalid hatched brush " << arg << endl;
        style = Qt::SolidPattern;
    }
  }
  else if (arg>=0 && arg<9)
    style = styleTab[arg];
  else
  {
      kdDebug(38000) << "QWinMetaFile::createBrushIndirect: invalid brush " << arg << endl;
      style = Qt::SolidPattern;
  }
  //  debug("createBrushIndirect: %d/%d -> %d", parm[0], parm[3], (short)style);

  handle->brush.setStyle(style);
  handle->brush.setColor(color(parm+1));
}


//-----------------------------------------------------------------------------
void QWinMetaFile::createPenIndirect(short /*num*/, short* parm)
{
  static Qt::PenStyle styleTab[] =
  { Qt::SolidLine, Qt::DashLine, Qt::DotLine, Qt::DashDotLine, Qt::DashDotDotLine,
    Qt::NoPen, Qt::SolidLine };
  Qt::PenStyle style;
  WinObjPenHandle* handle = createPen();

  if (parm[0]>=0 && parm[0]<6) style=styleTab[parm[0]];
  else
  {
      kdDebug(38000) << "QWinMetaFile::createPenIndirect: invalid pen " << parm[0] << endl;
      style = Qt::SolidLine;
  }
  // if (parm[1]<=0) style=NoPen;

  handle->pen.setStyle(style);
  handle->pen.setWidth(parm[1]);
  handle->pen.setColor(color(parm+3));
}

//-----------------------------------------------------------------------------
void QWinMetaFile::saveDC(short , short* )
{
  mPainter.save ();
}

//-----------------------------------------------------------------------------
void QWinMetaFile::restoreDC(short, short* )
{
  mPainter.restore ();
}
