/* This library is distributed under the conditions of the GNU LGPL.
 * WMF Metafile Structures
 * Author: 2002/2003 thierry lorthiois
 */
#ifndef _KOWMFSTRUCT_H_
#define _KOWMFSTRUCT_H_

#include <qglobal.h>
#include <qnamespace.h>

#define APMHEADER_KEY 0x9AC6CDD7
#define ENHMETA_SIGNATURE       0x464D4520

struct WmfMetaHeader
{
  Q_UINT16  fileType;      // Type of metafile (0=memory, 1=disk)
  Q_UINT16  headerSize;    // always 9
  Q_UINT16  version;
  Q_UINT32  fileSize;      // Total size of the metafile in WORDs
  Q_UINT16  numOfObjects;    // Maximum Number of objects in the stack
  Q_UINT32  maxRecordSize;   // The size of largest record in WORDs
  Q_UINT16  numOfParameters; // not used (always 0)
};


struct WmfPlaceableHeader
{
  Q_UINT32  key;        // Magic number (always 9AC6CDD7h)
  Q_UINT16  handle;     // Metafile HANDLE number (always 0)
  Q_INT16   left;       // Left coordinate in metafile units
  Q_INT16   top;
  Q_INT16   right;
  Q_INT16   bottom;
  Q_UINT16  inch;       // Number of metafile units per inch
  Q_UINT32  reserved;
  Q_UINT16  checksum;   // Checksum value for previous 10 WORDs
};


struct WmfEnhMetaHeader
{
  Q_UINT32  recordType;       // Record type (is always 00000001h)
  Q_UINT32  recordSize;       // Record size in bytes.  This may be greater
                              // than the sizeof( ENHMETAHEADER ).
  Q_INT32   boundsLeft;       // Inclusive-inclusive bounds in device units
  Q_INT32   boundsTop;
  Q_INT32   boundsRight;
  Q_INT32   boundsBottom;
  Q_INT32   frameLeft;        // Inclusive-inclusive Picture Frame
  Q_INT32   frameTop;
  Q_INT32   frameRight;
  Q_INT32   frameBottom;
  Q_UINT32  signature;        // Signature.  Must be ENHMETA_SIGNATURE.
  Q_UINT32  version;          // Version number
  Q_UINT32  size;             // Size of the metafile in bytes
  Q_UINT32  numOfRecords;     // Number of records in the metafile
  Q_UINT16  numHandles;       // Number of handles in the handle table
  // Handle index zero is reserved.
  Q_UINT16  reserved;         // always 0
  Q_UINT32  sizeOfDescription;   // Number of chars in the unicode description string
                                 // This is 0 if there is no description string
  Q_UINT32  offsetOfDescription; // Offset to the metafile description record.
                                 // This is 0 if there is no description string
  Q_UINT32  numPaletteEntries;   // Number of color palette entries
  Q_INT32   widthDevicePixels;   // Size of the reference device in pixels
  Q_INT32   heightDevicePixels;
  Q_INT32   widthDeviceMM;       // Size of the reference device in millimeters
  Q_INT32   heightDeviceMM;
};


struct WmfMetaRecord
{
  Q_UINT32  size;         // Total size of the record in WORDs
  Q_UINT16  function;     // Record function number
  Q_UINT16  param[ 1 ];   // Q_UINT16 array of parameters
};


struct WmfEnhMetaRecord
{
  Q_UINT32  function;     // Record function number
  Q_UINT32  size;         // Record size in bytes
  Q_UINT32  param[ 1 ];   // Q_UINT32 array of parameters
};

// Static data
    static const struct OpTab
    {
        Q_UINT32  winRasterOp;
        Qt::RasterOp  qtRasterOp;
    } koWmfOpTab32[] =
    {
        { 0x00CC0020, Qt::CopyROP },
        { 0x00EE0086, Qt::OrROP },
        { 0x008800C6, Qt::AndROP },
        { 0x00660046, Qt::XorROP },
        { 0x00440328, Qt::AndNotROP },
        { 0x00330008, Qt::NotCopyROP },
        { 0x001100A6, Qt::NandROP },
        { 0x00C000CA, Qt::CopyROP },
        { 0x00BB0226, Qt::NotOrROP },
        { 0x00F00021, Qt::CopyROP },
        { 0x00FB0A09, Qt::CopyROP },
        { 0x005A0049, Qt::CopyROP },
        { 0x00550009, Qt::NotROP },
        { 0x00000042, Qt::ClearROP },
        { 0x00FF0062, Qt::SetROP }
    };

    static const Qt::RasterOp koWmfOpTab16[] =
    {
        Qt::CopyROP,
        Qt::ClearROP, Qt::NandROP, Qt::NotAndROP, Qt::NotCopyROP,
        Qt::AndNotROP, Qt::NotROP, Qt::XorROP, Qt::NorROP,
        Qt::AndROP, Qt::NotXorROP, Qt::NopROP, Qt::NotOrROP,
        Qt::CopyROP, Qt::OrNotROP, Qt::OrROP, Qt::SetROP
    };

    static const Qt::BrushStyle koWmfHatchedStyleBrush[] =
    {
        Qt::HorPattern,
        Qt::VerPattern,
        Qt::FDiagPattern,
        Qt::BDiagPattern,
        Qt::CrossPattern,
        Qt::DiagCrossPattern
    };

    static const Qt::BrushStyle koWmfStyleBrush[] =
    { Qt::SolidPattern,
      Qt::NoBrush,
      Qt::FDiagPattern,   /* hatched */
      Qt::Dense4Pattern,  /* should be custom bitmap pattern */
      Qt::HorPattern,     /* should be BS_INDEXED (?) */
      Qt::VerPattern,     /* should be device-independent bitmap */
      Qt::Dense6Pattern,  /* should be device-independent packed-bitmap */
      Qt::Dense2Pattern,  /* should be BS_PATTERN8x8 */
      Qt::Dense3Pattern   /* should be device-independent BS_DIBPATTERN8x8 */
    };

    static const Qt::PenStyle koWmfStylePen[] =
    { Qt::SolidLine, Qt::DashLine, Qt::DotLine, Qt::DashDotLine, Qt::DashDotDotLine,
      Qt::NoPen, Qt::SolidLine };

#endif

