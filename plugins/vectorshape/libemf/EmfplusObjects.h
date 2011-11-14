/*
  Copyright 2011 Inge Wallin <inge@lysator.liu.se>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either 
  version 2.1 of the License, or (at your option) any later version.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public 
  License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef EMFPLUSOBJECTS_H
#define EMFPLUSOBJECTS_H

#include <QDataStream>
#include <QRectF> // also provides QSizeF
#include <QString>

/**
   \file

   Objects used in various parts of EMFPLUS parser.
*/


// We need most of the WMF and EMF enums and flags as well in an EMFPLUS file.
#include <WmfEnums.h>
#include <EmfEnums.h>

using namespace Libwmf;
using namespace Libemf;

/**
   Namespace for Enhanced Metafile (EMF) classes
*/
namespace Libemf
{
   
// Forward declarations


// ----------------------------------------------------------------
//                 2.2.2 Structure Object Types
//
// NOTE: These are placed before the objects in '2.2.1 Graphics
//       Objects' because they are more primitive and are used in the
//       other object types.


// The Structure Objects specify containers for data structures that
// are embedded in EMF+ metafile records. Structure objects, unlike
// graphics objects, are not explicitly created; they are components
// that make up more complex structures.


/**
   EmfPlusARGB Object

   The EmfPlusARGB object specifies a color as a combination of red,
   green, blue. and alpha.

   See [MS-EMFPLUS] 2.2.2.1
*/

/*
Blue (1 byte): An 8-bit unsigned integer that specifies the relative
                   intensity of blue.
                   
Green (1 byte): An 8-bit unsigned integer that specifies the relative
                   intensity of green.
                   
Red (1 byte): An 8-bit unsigned integer that specifies the relative
                   intensity of red.

Alpha (1 byte): An 8-bit unsigned integer that specifies the
                   transparency of the background, ranging from 0 for
                   completely transparent to 0xFF for completely
                   opaque. See section 2.2.2 for the specification of
                   additional structure objects.
*/


/**
   EmfPlusBitmap Object

   The EmfPlusBitmap object specifies a bitmap that contains a
   graphics image.

   See [MS-EMFPLUS] 2.2.2.2
*/

/*
Width (4 bytes): A 32-bit signed integer that specifies the width in
                   pixels of the area occupied by the bitmap. If the
                   image is compressed, according to the Type field,
                   this value is undefined and MUST be ignored.

Height (4 bytes): A 32-bit signed integer that specifies the height in
                   pixels of the area occupied by the bitmap. If the
                   image is compressed, according to the Type field,
                   this value is undefined and MUST be ignored.

Stride (4 bytes): A 32-bit signed integer that specifies the byte
                   offset between the beginning of one scan-line and
                   the next. This value is the number of bytes per
                   pixel, which is specified in the PixelFormat field,
                   multiplied by the width in pixels, which is
                   specified in the Width field. The value of this
                   field MUST be a multiple of four. If the image is
                   compressed, according to the Type field, this value
                   is undefined and MUST be ignored.

PixelFormat (4 bytes): A 32-bit unsigned integer that specifies the
                   format of the pixels that make up the bitmap
                   image. The supported pixel formats are specified in
                   the PixelFormat enumeration (section 2.1.1.25). If
                   the image is compressed, according to the Type
                   field, this value is undefined and MUST be ignored.

X (1 bit): Reserved and MUST be ignored. N (1 bit): If set, the pixel
                   format is "canonical", which means that 32 bits per
                   pixel are supported, with 24-bits for color
                   components and an 8-bit alpha channel. If clear,
                   the pixel format is not canonical. E (1 bit): If
                   set, the pixel format supports extended colors in
                   16-bits per channel. If clear, extended colors are
                   not supported. P (1 bit): If set, each color
                   component in the pixel has been premultiplied by
                   the pixel's alpha transparency value. If clear,
                   each color component is multiplied by the pixel's
                   alpha transparency value when the source pixel is
                   blended with the destination pixel. A (1 bit): If
                   set, the pixel format includes an alpha
                   transparency component. If clear, the pixel format
                   does not include a component that specifies
                   transparency. G (1 bit): If set, the pixel format
                   is supported in Windows GDI. If clear, the pixel
                   format is not supported in Windows GDI. I (1 bit):
                   If set, the pixel values are indexes into a
                   palette. If clear, the pixel values are actual
                   colors.

BitsPerPixel (1 byte): The total number of bits per pixel. 

Index (1 byte): The pixel format enumeration index. 

Type (4 bytes): A 32-bit unsigned integer that specifies the type of
                   data in the BitmapData field. This value MUST be
                   defined in the BitmapDataType enumeration (section
                   2.1.1.2). BitmapData (variable): Variable-length
                   data that defines the bitmap data object specified
                   in the Type field. The content and format of the
                   data can be different for every bitmap
                   type. Graphics images are specified by EmfPlusImage
                   objects (section 2.2.1.4). An EmfPlusBitmap object
                   MUST be present in the ImageData field of an
                   EmfPlusImage object if ImageTypeBitmap is specified
                   in its Type field. This object is generic and is
                   used to specify different types of bitmap data,
                   including:
                   An EmfPlusBitmapData object (section 2.2.2.3).
                   An EmfPlusCompressedImage object (section 2.2.2.10);
                   and See section 2.2.2 for the specification of additional 
                   structure objects.  
*/

/**
   EmfPlusBitmapData Object

   The EmfPlusBitmapData object specifies a bitmap image with pixel
   data.

   See [MS-EMFPLUS] 2.2.2.3
*/


/*
Colors (variable): An optional EmfPlusPalette object (section
                   2.2.2.28), which specifies the palette of colors
                   used in the pixel data. This field MUST be present
                   if the I flag is set in the PixelFormat field of
                   the EmfPlusBitmap object.

PixelData (variable): An array of bytes that specify the pixel
                   data. The size and format of this data can be
                   computed from fields in the EmfPlusBitmap object,
                   including the pixel format from the PixelFormat
                   enumeration (section 2.1.1.25). Bitmaps are
                   specified by EmfPlusBitmap objects (section
                   2.2.2.2). An EmfPlusBitmapData object MUST be
                   present in the BitmapData field of an EmfPlusBitmap
                   object if BitmapDataTypePixel is specified in its
                   Type field. See section 2.2.2 for the specification
                   of additional structure objects.
*/

/**

   EmfPlusBlendColors Object

   The EmfPlusBlendColors object specifies positions and colors for
   the blend pattern of a gradient brush.

   See [MS-EMFPLUS] 2.2.2.4
*/


/*
PositionCount (4 bytes): A 32-bit unsigned integer that specifies the
                   number of positions in the BlendPositions field and
                   colors in the BlendColors field.

BlendPositions (variable): An array of PositionCount 32-bit
                   floating-point values that specify proportions of
                   distance along the gradient line. Each element MUST
                   be a number between 0.0 and 1.0 inclusive. For a
                   linear gradient brush, 0.0 represents the starting
                   point and 1.0 represents the ending point. For a
                   path gradient brush, 0.0 represents the midpoint
                   and 1.0 represents an endpoint.

BlendColors (variable): An array of PositionCount EmfPlusARGB objects
                   (section 2.2.2.1) that specify colors at the
                   positions defined in the BlendPositions
                   field. Gradient brushes are specified by
                   EmfPlusLinearGradientBrushData objects (section
                   2.2.2.24) and EmfPlusPathGradientBrushData objects
                   (section 2.2.2.29). Blend patterns are used to
                   smoothly shade the interiors of shapes filled by
                   gradient brushes. and can be defined by arrays of
                   positions and colors or positions and
                   factors. Positions and factors are specified by
                   EmfPlusBlendFactors objects (section 2.2.2.5). An
                   EmfPlusBlendColors object MUST be present in the
                   OptionalData field of an
                   EmfPlusLinearGradientBrushData object, if the
                   BrushDataPresetColors flag is set in its
                   BrushDataFlags field. An EmfPlusBlendColors object
                   MUST be present in the OptionalData field of an
                   EmfPlusPathGradientBrushData object, if the
                   BrushDataPresetColors flag is set in its
                   BrushDataFlags field. See section 2.2.2 for the
                   specification of additional structure objects.
*/


/**
   EmfPlusBlendFactors Object

   The EmfPlusBlendFactors object specifies positions and factors for
   the blend pattern of a gradient brush.

   See [MS-EMFPLUS] 2.2.2.5
*/


/*
PositionCount (4 bytes): A 32-bit unsigned integer that specifies the
                   number of positions in the BlendPositions field and
                   factors in the BlendFactors field.


BlendPositions (variable): An array of PositionCount 32-bit
                    floating-point values that specify proportions of
                    distance along the gradient line. Each value MUST
                    be a number between 0.0 and 1.0 inclusive. For a
                    linear gradient brush, 0.0 represents the starting
                    point and 1.0 represents the ending point. For a
                    path gradient brush, 0.0 represents the midpoint
                    and 1.0 represents an endpoint.

BlendFactors (variable): An array of PositionCount 32-bit floating
                    point values that specify proportions of colors at
                    the positions defined in the BlendPositions
                    field. Each value MUST be a number between 0.0 and
                    1.0 inclusive. For a linear gradient brush, 0.0
                    represents 0% starting color and 100% ending
                    color, and 1.0 represents 100% starting color and
                    0% ending color. For a path gradient brush, 0.0
                    represents 0% midpoint color and 100% endpoint
                    color, and 1.0 represents 100% midpoint color and
                    0% endpoint color. For example, if a linear
                    gradient brush specifies a position of 0.2 and a
                    factor of 0.3 along a gradient line that is 100
                    pixels long, the color that is 20 pixels along
                    that line consists of 30 percent starting color
                    and 70 percent ending color. Gradient brushes are
                    specified by EmfPlusLinearGradientBrushData
                    objects (section 2.2.2.24) and
                    EmfPlusPathGradientBrushData objects (section
                    2.2.2.29). Blend patterns are used to smoothly
                    shade the interiors of shapes filled by gradient
                    brushes. and can be defined by arrays of positions
                    and colors or positions and factors. Positions and
                    colors are specified by EmfPlusBlendColors objects
                    (section 2.2.2.4). An EmfPlusBlendFactors object
                    MUST be present in the OptionalData field of an
                    EmfPlusLinearGradientBrushData or
                    EmfPlusPathGradientBrushData object if either of
                    the flags BrushDataBlendFactorsH or
                    BrushDataBlendFactorsV is set in its
                    BrushDataFlags field. See section 2.2.2 for the
                    specification of additional structure objects.
*/


/**
   EmfPlusBoundaryPathData Object

   The EmfPlusBoundaryPathData object specifies a graphics path
   boundary for a gradient brush.

   See [MS-EMFPLUS] 2.2.2.6
*/

/*
BoundaryPathSize (4 bytes): A 32-bit signed integer that specifies the
                   size in bytes of the BoundaryPathData field.

BoundaryPathData (variable): An EmfPlusPath object (section 2.2.1.6)
                    that specifies the boundary of the brush. Boundary
                    path data is specified in the BoundaryData field
                    of an EmfPlusPathGradientBrushData object (section
                    2.2.2.29). See section 2.2.2 for the specification
                    of additional structure objects.
*/


/**
   EmfPlusBoundaryPointData Object

   The EmfPlusBoundaryPointData object specifies a closed cardinal
   spline boundary for a gradient brush.

   See [MS-EMFPLUS] 2.2.2.7
*/

/*
BoundaryPointCount (4 bytes): A 32-bit signed integer that specifies
                   the number of points in the BoundaryPointData field.

BoundaryPointData (variable): An array of BoundaryPointCount
                EmfPlusPointF objects that specify the boundary of the
                brush. Boundary point data is specified in the
                BoundaryData field of an EmfPlusPathGradientBrushData
                object (section 2.2.2.29). See section 2.2.2 for the
                specification of additional structure objects.
*/


/**
   EmfPlusCharacterRange Object

   The EmfPlusCharacterRange object specifies a range of character
   positions for a text string.

   See [MS-EMFPLUS] 2.2.2.8
*/

/*
First (4 bytes): A 32-bit signed integer that specifies the first
                   position of this range.

Length (4 bytes): A 32-bit signed integer that specifies the number of
                   positions in this range. Graphics strings are
                   specified by EmfPlusStringFormat objects (section
                   2.2.1.9). See section 2.2.2 for the specification
                   of additional structure objects.
*/


/**
   EmfPlusCompoundLineData Object

   The EmfPlusCompoundLineData object specifies line and space data
   for a compound line.

   See [MS-EMFPLUS] 2.2.2.9
*/

/*
CompoundLineDataSize (4 bytes): A 32-bit unsigned integer that
                   specifies the number of elements in the CompoundLineData field.

CompoundLineData (variable): An array of CompoundLineDataSize
                    floating-point values that specify the compound
                    line of a pen. The elements MUST be in increasing
                    order, and their values MUST be between 0.0 and
                    1.0, inclusive. Graphics pens are specified by
                    EmfPlusPen objects (section 2.2.1.7). An
                    EmfPlusCompoundLineData object MUST be present in
                    the OptionalData field of an EmfPlusPenData object
                    (section 2.2.2.33), if the PenDataCompoundLineData
                    flag is set in its PenDataFlags field. A compound
                    line is made up of a pattern of alternating
                    parallel lines and spaces of varying widths. The
                    values in the array specify the starting points of
                    each component of the compound line relative to
                    the total width. The first value specifies where
                    the first line component begins as a fraction of
                    the distance across the width of the pen. The
                    second value specifies where the first space
                    component begins as a fraction of the distance
                    across the width of the pen. The final value in
                    the array specifies where the last line component
                    ends. See section 2.2.2 for the specification of
                    additional structure objects.
*/


/**
   EmfPlusCompressedImage Object

   The EmfPlusCompressedImage object specifies an image with
   compressed data.

   See [MS-EMFPLUS] 2.2.2.10
*/

/*
CompressedImageData (variable): An array of bytes, which specify the
                    compressed image. The type of compression MUST be
                    determined from the data itself. Bitmaps are
                    specified by EmfPlusBitmap objects (section
                    2.2.2.2). An EmfPlusCompressedImage object MUST be
                    present in the BitmapData field of an
                    EmfPlusBitmap object if BitmapDataTypeCompressed
                    is specified in its Type field. This object is
                    generic and is used for different types of
                    compressed data, including: Exchangeable Image
                    File Format (EXIF), as specified in [EXIF];
                    Graphics Interchange Format (GIF), as specified
                    in [GIF]; Joint Photographic Experts Group
                    (JPEG), as specified in [JFIF]; Portable Network
                    Graphics (PNG), as specified in [RFC2083]and
                    [W3C-PNG]; and Tag Image File Format (TIFF), as
                    specified in [RFC3302] and [TIFF].

See section 2.2.2 for the specification of additional structure objects.
*/


/**
   EmfPlusCustomEndCapData Object

   The EmfPlusCustomEndCapData object specifies a custom line cap for
   the end of a line.

   See [MS-EMFPLUS] 2.2.2.11
*/

/**
CustomEndCapSize (4 bytes): A 32-bit unsigned integer that specifies
                   the size in bytes of the CustomEndCap field.

CustomEndCap (variable): A custom line cap that defines the shape to
                   draw at the end of a line. It can be any of various
                   shapes, including a square, circle, or
                   diamond. Custom line caps are specified by
                   EmfPlusCustomLineCap objects (section 2.2.1.2). An
                   EmfPlusCustomEndCapData object MUST be present in
                   the OptionalData field of an EmfPlusPenData object
                   (section 2.2.2.33), if the PenDataEndCap flag is
                   set in its PenDataFlags field. See section 2.2.2
                   for the specification of additional structure
                   objects.
*/


/**
   EmfPlusCustomLineCapArrowData Object

   The EmfPlusCustomLineCapArrowData object specifies adjustable arrow
   data for a custom line cap.

   See [MS-EMFPLUS] 2.2.2.12
*/

/*
Width (4 bytes): A 32-bit floating-point value that specifies the
                   width of the arrow cap. The width of the arrow cap
                   is scaled by the width of the EmfPlusPen object
                   (section 2.2.1.7) that is used to draw the line
                   being capped. For example, when drawing a capped
                   line with a pen that has a width of 5 pixels, and
                   the adjustable arrow cap object has a width of 3,
                   the actual arrow cap is drawn 15 pixels wide.

Height (4 bytes): A 32-bit floating-point value that specifies the
                   height of the arrow cap. The height of the arrow
                   cap is scaled by the width of the EmfPlusPen object
                   that is used to draw the line being capped. For
                   example, when drawing a capped line with a pen that
                   has a width of 5 pixels, and the adjustable arrow
                   cap object has a height of 3, the actual arrow cap
                   is drawn 15 pixels high.

MiddleInset (4 bytes): A 32-bit floating-point value that specifies
                   the number of pixels between the outline of the
                   arrow cap and the fill of the arrow cap.

FillState (4 bytes): A 32-bit Boolean value that specifies whether the
                   arrow cap is filled. If the arrow cap is not
                   filled, only the outline is drawn.

LineStartCap (4 bytes): A 32-bit unsigned integer that specifies the
                   value in the LineCap enumeration that indicates the
                   line cap to be used at the start of the line to be
                   drawn.

LineEndCap (4 bytes): A 32-bit unsigned integer that specifies the
                   value in the LineCap enumeration that indicates the
                   line cap to be used at the end of the line to be
                   drawn.

LineJoin (4 bytes): A 32-bit unsigned integer that specifies the value
                   in the LineJoin enumeration that specifies how to
                   join two lines that are drawn by the same pen and
                   whose ends meet. At the intersection of the two
                   line ends, a line join makes the connection look
                   more continuous.

LineMiterLimit (4 bytes): A 32-bit floating-point value that specifies
                   the limit of the thickness of the join on a mitered
                   corner by setting the maximum allowed ratio of
                   miter length to line width.

WidthScale (4 bytes): A 32-bit floating-point value that specifies the
                   amount by which to scale an EmfPlusCustomLineCap
                   object with respect to the width of the graphics
                   pen that is used to draw the lines.

FillHotSpot (8 bytes): An EmfPlusPointF object that is not currently
                   used. It MUST be set to {0.0, 0.0}.

LineHotSpot (8 bytes): An EmfPlusPointF object that is not currently
                   used. It MUST be set to {0.0, 0.0}.

 Custom line caps are specified by EmfPlusCustomLineCap objects
 (section 2.2.1.2). See section 2.2.2 for the specification of
 additional structure objects.
*/


/**
   EmfPlusCustomLineCapData Object

   The EmfPlusCustomLineCapData object specifies default data for a
   custom line cap.

   See [MS-EMFPLUS] 2.2.2.13
*/

/*
CustomLineCapDataFlags (4 bytes): A 32-bit unsigned integer that
                   specifies the data in the OptionalData field. This
                   value MUST be composed of CustomLineCapData flags
                   (section 2.1.2.2).

BaseCap (4 bytes): A 32-bit unsigned integer that specifies the value
                   from the LineCap enumeration (section 2.1.1.18) on
                   which the custom line cap is based.

BaseInset (4 bytes): A 32-bit floating-point value that specifies the
                   distance between the beginning of the line cap and
                   the end of the line.

StrokeStartCap (4 bytes): A 32-bit unsigned integer that specifies the
                   value in the LineCap enumeration that indicates the
                   line cap used at the start of the line to be drawn.

StrokeEndCap (4 bytes): A 32-bit unsigned integer that specifies the
                   value in the LineCap enumeration that indicates
                   what line cap is to be used at the end of the line
                   to be drawn.

StrokeJoin (4 bytes): A 32-bit unsigned integer that specifies the
                   value in the LineJoin enumeration (section
                   2.1.1.19), which specifies how to join two lines
                   that are drawn by the same pen and whose ends
                   meet. At the intersection of the two line ends, a
                   line join makes the connection look more
                   continuous.

StrokeMiterLimit (4 bytes): A 32-bit floating-point value that
                   contains the limit of the thickness of the join on
                   a mitered corner by setting the maximum allowed
                   ratio of miter length to line width.

WidthScale (4 bytes): A 32-bit floating-point value that specifies the
                   amount by which to scale the custom line cap with
                   respect to the width of the EmfPlusPen object
                   (section 2.2.1.7) that is used to draw the lines.

FillHotSpot (8 bytes): An EmfPlusPointF object that is not currently
                   used. It MUST be set to {0.0, 0.0}.

StrokeHotSpot (8 bytes): An EmfPlusPointF object that is not currently
                   used. It MUST be set to {0.0, 0.0}.

OptionalData (variable): An optional EmfPlusCustomLineCapOptionalData
                   object (section 2.2.2.14) that specifies additional
                   data for the custom graphics line cap. The specific
                   contents of this field are determined by the value
                   of the CustomLineCapDataFlags field. Custom line
                   caps are specified by EmfPlusCustomLineCap objects
                   (section 2.2.1.2). See section 2.2.2 for the
                   specification of additional structure objects.
*/


/**
   EmfPlusCustomLineCapOptionalData Object

   The EmfPlusCustomLineCapOptionalData object specifies optional fill
   and outline data for a custom line cap. Note Each field specified
   for this object is optional and might not be present in the
   OptionalData field of an EmfPlusCustomLineCapData object (section
   2.2.2.13), depending on the CustomLineCapData flags (section
   2.1.2.2) set in its CustomLineCapDataFlags field. Although it is
   not practical to represent every possible combination of fields
   present or absent, this section specifies their relative order in
   the object. The implementer is responsible for determining which
   fields are actually present in a given metafile record, and for
   unmarshaling the data for individual fields separately and
   appropriately.

   See [MS-EMFPLUS] 2.2.2.14
*/

/*
FillData (variable): An optional EmfPlusFillPath object (section
                    2.2.2.17) that specifies the path for filling a
                    custom graphics line cap. This field MUST be
                    present if the CustomLineCapDataFillPath flag is
                    set in the CustomLineCapDataFlags field of the
                    EmfPlusCustomLineCapData object.

OutlineData (variable): An optional EmfPlusLinePath object (section
                    2.2.2.26) that specifies the path for outlining a
                    custom graphics line cap. This field MUST be
                    present if the CustomLineCapDataLinePath flag is
                    set in the CustomLineCapDataFlags field of the
                    EmfPlusCustomLineCapData object. Custom line caps
                    are specified by EmfPlusCustomLineCap objects
                    (section 2.2.1.2). See section 2.2.2 for the
                    specification of additional structure objects.
*/


/**
   EmfPlusCustomStartCapData Object

   The EmfPlusCustomStartCapData object specifies a custom line cap
   for the start of a line.

   See [MS-EMFPLUS] 2.2.2.15
*/


/*
CustomStartCapSize (4 bytes): A 32-bit unsigned integer that specifies
                   the size in bytes of the CustomStartCap field.

CustomStartCap (variable): A custom graphics line cap that defines the
                   shape to draw at the start of a line. It can be any
                   of various shapes, including a square, circle or
                   diamond. Custom line caps are specified by
                   EmfPlusCustomLineCap objects (section 2.2.1.2). If
                   the PenDataStartCap flag is set in its PenDataFlags
                   field, an EmfPlusCustomStartCapData object MUST be
                   present in the OptionalData field of an
                   EmfPlusPenData object (section 2.2.2.33). See
                   section 2.2.2 for the specification of additional
                   structure objects.
*/


/**
   EmfPlusDashedLineData Object

   The EmfPlusDashedLineData object specifies properties of a dashed
   line for a graphics pen.

   See [MS-EMFPLUS] 2.2.2.16
*/


/*
DashedLineDataSize (4 bytes): A 32-bit unsigned integer that specifies
                    the number of elements in the DashedLineData field.

DashedLineData (variable): An array of DashedLineDataSize
                    floating-point values that specify the lengths of
                    the dashes and spaces in a dashed line. Graphics
                    pens are specified by EmfPlusPen objects (section
                    2.2.1.7). An EmfPlusDashedLineData object MUST be
                    present in the OptionalData field of an
                    EmfPlusPenData object (section 2.2.2.33), if the
                    PenDataDashedLine flag is set in its PenDataFlags
                    field. See section 2.2.2 for the specification of
                    additional structure objects.
*/


/**
   EmfPlusFillPath Object

   The EmfPlusFillPath object specifies a graphics path for filling a
   custom line cap.

   See [MS-EMFPLUS] 2.2.2.17
*/

/*
FillPathLength (4 bytes): A 32-bit signed integer that specifies the
                   length in bytes of the FillPath field.

FillPath (variable): An EmfPlusPath object (section 2.2.1.6) that
                   specifies the area to fill. Custom line caps are
                   specified by EmfPlusCustomLineCap objects (section
                   2.2.1.2). An EmfPlusFillPath object MUST be present
                   if the CustomLineCapDataFillPath flag is set in the
                   CustomLineCapDataFlags field of an
                   EmfPlusCustomLineCapData object (section
                   2.2.2.13). See section 2.2.2 for the specification
                   of additional structure objects.
*/


/**
   EmfPlusFocusScaleData Object

   The EmfPlusFocusScaleData object specifies focus scales for the
   blend pattern of a path gradient brush.

   See [MS-EMFPLUS] 2.2.2.18
*/

/*
FocusScaleCount (4 bytes): A 32-bit unsigned integer that specifies
                   the number of focus scales. This value MUST be 2.

FocusScaleX (4 bytes): A floating-point value that defines the
                   horizontal focus scale. The focus scale MUST be a
                   value between 0.0 and 1.0, exclusive.

FocusScaleY (4 bytes): A floating-point value that defines the
                   vertical focus scale. The focus scale MUST be a
                   value between 0.0 and 1.0, exclusive. By default,
                   the center color of a path gradient brush is
                   displayed only at the center point of an area
                   bounded by a path. Focus scales specify an inner
                   path inside that area, and the center color is
                   displayed everywhere inside it. The inner path is
                   the boundary path scaled by horizontal and vertical
                   scale factors. For example, focus scales of {0.2,
                   0.3} specifies a path that is the boundary path
                   scaled by a factor of 0.2 horizontally and 0.3
                   vertically. The area inside the scaled path MUST be
                   filled with the center color. Between the inner and
                   outer boundaries, the color MUST change gradually
                   from the center color to the boundary color. An
                   EmfPlusFocusScaleData object MUST be present in the
                   OptionalData field of an
                   EmfPlusPathGradientBrushData object, if the
                   BrushDataFocusScales flag is set in its
                   BrushDataFlags field. See section 2.2.2 for the
                   specification of additional structure objects.
*/


/**
   EmfPlusGraphicsVersion Object

   The EmfPlusGraphicsVersion object specifies the version of
   operating system graphics that is used to create an EMF+ metafile.

   See [MS-EMFPLUS] 2.2.2.19
*/

struct EmfPlusGraphicsVersion {
    quint32 metafileSignature;
    quint32 graphicsVersion;

    EmfPlusGraphicsVersion(QDataStream &stream);
};



/**
   EmfPlusHatchBrushData Object

   The EmfPlusHatchBrushData object specifies a hatch pattern for a
   graphics brush.

   See [MS-EMFPLUS] 2.2.2.20
*/

/*
HatchStyle (4 bytes): A 32-bit unsigned integer that specifies the
                   brush hatch style. It MUST be defined in the
                   HatchStyle enumeration.

ForeColor (4 bytes): A 32-bit EmfPlusARGB object that specifies the
                   color used to draw the lines of the hatch pattern.

BackColor (4 bytes): A 32-bit EmfPlusARGB object that specifies the
                   color used to paint the background of the hatch
                   pattern. Graphics brushes are specified by
                   EmfPlusBrush objects (section 2.2.1.1). A hatch
                   brush paints a background and draws a pattern of
                   lines, dots, dashes, squares, and crosshatch lines
                   over this background. The hatch brush defines two
                   colors: one for the background and one for the
                   pattern over the background. The color of the
                   background is called the background color, and the
                   color of the pattern is called the foreground
                   color. See section 2.2.2 for the specification of
                   additional structure objects.
*/


// EmfPlusInteger7 Object (2.2.2.21) not defined here, but handled inline

// EmfPlusInteger15 Object (2.2.2.22) not defined here, but handled inline


/**
   EmfPlusLanguageIdentifier Object

   The EmfPlusLanguageIdentifier object specifies a language
   identifier that corresponds to the natural language in a locale,
   including countries, geographical regions, and administrative
   districts. Each language identifier is an encoding of a primary
   language value and sublanguage value.

   See [MS-EMFPLUS] 2.2.2.23
*/

/*
SubLanguageId (6 bits): The country, geographic region or
                    administrative district for the natural language
                    specified in the PrimaryLanguageId
                    field. Sublanguage identifiers are
                    vendor-extensible. Vendor-defined sublanguage
                    identifiers MUST be in the range 0x20 to 0x3F,
                    inclusive.

PrimaryLanguageId (10 bits): The natural language. Primary language
                    identifiers are vendor-extensible. Vendor-defined
                    primary language identifiers MUST be in the range
                    0x0200 to 0x03FF, inclusive. The 16-bit encoded
                    language identifier value MUST be defined in the
                    LanguageIdentifier enumeration (section
                    2.1.1.17). See section 2.2.2 for the specification
                    of additional structure objects.
*/


/**
  EmfPlusLinearGradientBrushData Object

  The EmfPlusLinearGradientBrushData object specifies a linear
  gradient for a graphics brush.

   See [MS-EMFPLUS] 2.2.2.24
*/

/*
BrushDataFlags (4 bytes): A 32-bit unsigned integer that specifies the
                   data in the OptionalData field. This value MUST be
                   composed of BrushData flags (section 2.1.2.1). The
                   following flags are relevant to a linear gradient
                   brush:

                   Name BrushDataTransform BrushDataPresetColors
                   BrushDataBlendFactorsH BrushDataBlendFactorsV
                   BrushDataIsGammaCorrected Value 0x00000002
                   0x00000004 0x00000008 0x00000010 0x00000080


WrapMode (4 bytes): A 32-bit signed integer from the WrapMode
                   enumeration (section 2.1.1.34) that specifies
                   whether to paint the area outside the boundary of
                   the brush. When painting outside the boundary, the
                   wrap mode specifies how the color gradient is
                   repeated.

RectF (16 bytes): An EmfPlusRectF object (section 2.2.2.39) that
                   specifies the starting and ending points of the
                   gradient line. The upper-left corner of the
                   rectangle is the starting point. The lower-right
                   corner is the ending point.

StartColor (4 bytes): An EmfPlusARGB object (section 2.2.2.1) that
                   specifies the color at the starting boundary point
                   of the linear gradient brush.

EndColor (4 bytes): An EmfPlusARGB object that specifies the color at
                   the ending boundary point of the linear gradient
                   brush.

Reserved1 (4 bytes): This field is reserved and MUST be ignored. 

Reserved2 (4 bytes): This field is reserved and MUST be ignored. 

OptionalData (variable): An optional
                    EmfPlusLinearGradientBrushOptionalData object
                    (section 2.2.2.25) that specifies additional data
                    for the linear gradient brush. The specific
                    contents of this field are determined by the value
                    of the BrushDataFlags field. Graphics brushes are
                    specified by EmfPlusBrush objects (section
                    2.2.1.1). A linear gradient brush paints a color
                    gradient in which the color changes gradually
                    along a gradient line from a starting boundary
                    point to an ending boundary point, as specified by
                    the diagonal of a rectangle in the RectF
                    field. Gamma correction controls the overall
                    brightness and intensity of an image. Uncorrected
                    images can look either bleached out or too
                    dark. Varying the amount of gamma correction
                    changes not only the brightness but also the
                    ratios of red to green to blue. The need for gamma
                    correction arises because an output device might
                    not render colors in the same intensity as the
                    input image. See section 2.2.2 for the
                    specification of additional structure objects.
*/


/*
  EmfPlusLinearGradientBrushOptionalData Object

  The EmfPlusLinearGradientBrushOptionalData object specifies optional
  data for a linear gradient brush. Note Each field of this object is
  optional and might not be present in the OptionalData field of an
  EmfPlusLinearGradientBrushData object (section 2.2.2.24), depending
  on the BrushData flags (section 2.1.2.1) set in its BrushDataFlags
  field. Although it is not practical to represent every possible
  combination of fields present or absent, this section specifies
  their relative order in the object. The implementer is responsible
  for determining which fields are actually present in a given
  metafile record, and for unmarshaling the data for individual fields
  separately and appropriately.

   See [MS-EMFPLUS] 2.2.2.25
*/

/*
TransformMatrix (24 bytes): An optional EmfPlusTransformMatrix object
                    (section 2.2.2.47) that specifies a world space to
                    device space transform for the linear gradient
                    brush. This field MUST be present if the
                    BrushDataTransform flag is set in the
                    BrushDataFlags field of the
                    EmfPlusLinearGradientBrushData object.

 BlendPattern (variable): An optional blend pattern for the linear
                    gradient brush. If this field is present, it MUST
                    contain either an EmfPlusBlendColors object
                    (section 2.2.2.4), or one or two
                    EmfPlusBlendFactors objects (section 2.2.2.5), but
                    it MUST NOT contain both. The table below shows
                    the valid combinations of
                    EmfPlusLinearGradientBrushData BrushData flags and
                    the corresponding blend patterns: PresetColors
                    Clear BlendFactorsH Clear BlendFactorsV Clear
                    Blend Pattern This field MUST NOT be present in
                    the EmfPlusLinearGradientBrushOptionalData
                    object. An EmfPlusBlendColors object MUST be
                    present. An EmfPlusBlendFactors object along the
Set Clear

Clear Set

Clear Clear


PresetColors

BlendFactorsH

BlendFactorsV

Blend Pattern vertical gradient line MUST be present.

Clear Clear

Clear Set

Set Set

An EmfPlusBlendFactors object along the horizontal gradient line MUST
be present. An EmfPlusBlendFactors object along the vertical gradient
line and an EmfPlusBlendFactors object along the horizontal gradient
line MUST be present.

Graphics brushes are specified by EmfPlusBrush objects (section
2.2.1.1). See section 2.2.2 for the specification of additional
structure objects.
*/


/**
   EmfPlusLinePath Object

   The EmfPlusLinePath object specifies a graphics path for outlining
   a custom line cap.

   See [MS-EMFPLUS] 2.2.2.26
*/

/*
LinePathLength (4 bytes): A 32-bit signed integer that defines the
                   length in bytes of the LinePath field.

LinePath (variable): An EmfPlusPath object that defines the
                   outline. Custom line caps are specified by
                   EmfPlusCustomLineCap objects (section 2.2.1.2). An
                   EmfPlusLinePath object MUST be present if the
                   CustomLineCapDataLinePath flag is set in the
                   CustomLineCapDataFlags field of an
                   EmfPlusCustomLineCapData object (section
                   2.2.2.13). See section 2.2.2 for the specification
                   of additional structure objects.
*/


/**
   EmfPlusMetafile Object

   The EmfPlusMetafileData object specifies a metafile that contains a
   graphics image.

   See [MS-EMFPLUS] 2.2.2.27
*/

/*
Type (4 bytes): A 32-bit unsigned integer that specifies the type of
                   metafile that is embedded in the MetafileData
                   field. This value MUST be defined in the
                   MetafileDataType enumeration (section 2.1.1.21).

MetafileDataSize (4 bytes): A 32-bit unsigned integer that specifies
                   the size in bytes of the metafile data in the
                   MetafileData field.

MetafileData (variable): Variable-length data that specifies the
                   embedded metafile. The content and format of the
                   data can be different for each metafile
                   type. Graphics images are specified by EmfPlusImage
                   objects (section 2.2.1.4). An EmfPlusMetafile
                   object MUST be present in the ImageData field of an
                   EmfPlusImage object if ImageTypeMetafile is
                   specified in its Type field. This object is generic
                   and is used for different types of data, including:
                   A WMF metafile [MS-WMF]; WMF metafile which can
                   be placed; An EMF metafile [MS-EMF]; An EMF+
                   metafile that specifies graphics operations with
                   EMF+ records only; and An EMF+ metafile that
                   specifies graphics operations with both EMF+ and
                   EMF records. See section 2.2.2 for the
                   specification of additional structure objects.
*/


/*
  EmfPlusPalette Object

  The EmfPlusPalette object specifies the colors that make up a
  palette.

   See [MS-EMFPLUS] 2.2.2.28
*/

/*
PaletteStyleFlags (4 bytes): A 32-bit unsigned integer that specifies
                   the attributes of data in the palette. This value
                   MUST be composed of PaletteStyle flags.

PaletteCount (4 bytes): A 32-bit unsigned integer that specifies the
                   number of entries in the PaletteEntries array.

PaletteEntries (variable): An array of PaletteCount EmfPlusARGB
                   objects that specify the data in the palette. See
                   section 2.2.2 for the specification of additional
                   graphics objects.
*/


/**
   EmfPlusPathGradientBrushData Object

   The EmfPlusPathGradientBrushData object specifies a path gradient
   for a graphics brush.

   See [MS-EMFPLUS] 2.2.2.29
*/

/*
BrushDataFlags (4 bytes): A 32-bit unsigned integer that specifies the
                   data in the OptionalData field. This value MUST be
                   composed of BrushData flags (section 2.1.2.1). The
                   following flags are relevant to a path gradient
                   brush:

                   Name BrushDataPath BrushDataTransform
                   BrushDataPresetColors BrushDataBlendFactorsH
                   BrushDataFocusScales BrushDataIsGammaCorrected
                   Value 0x00000001 0x00000002 0x00000004 0x00000008
                   0x00000040 0x00000080

WrapMode (4 bytes): A 32-bit signed integer from the WrapMode
                   enumeration (section 2.1.1.34) that specifies
                   whether to paint the area outside the boundary of
                   the brush. When painting outside the boundary, the
                   wrap mode specifies how the color gradient is
                   repeated.

CenterColor (4 bytes): An EmfPlusARGB object (section 2.2.2.1) that
                   specifies the center color of the path gradient
                   brush, which is the color that appears at the
                   center point of the brush. The color of the brush
                   changes gradually from the boundary color to the
                   center color as it moves from the boundary to the
                   center point.

CenterPointF (8 bytes): An EmfPlusPointF object (section 2.2.2.36)
                   that specifies the center point of the path
                   gradient brush, which can be any location inside or
                   outside the boundary. The color of the brush
                   changes gradually from the boundary color to the
                   center color as it moves from the boundary to the
                   center point.

SurroundingColorCount (4 bytes): An unsigned 32-bit integer that
                   specifies the number of colors specified in the
                   SurroundingColor field. The surrounding colors are
                   colors specified for discrete points on the
                   boundary of the brush.

SurroundingColor (variable): An array of SurroundingColorCount
                   EmfPlusARGB objects that specify the colors for
                   discrete points on the boundary of the brush.

BoundaryData (variable): The boundary of the path gradient brush,
                   which is specified by either a path or a closed
                   cardinal spline. If the BrushDataPath flag is set
                   in the BrushDataFlags field, this field MUST
                   contain an EmfPlusBoundaryPathData object (section
                   2.2.2.6); otherwise, this field MUST contain an
                   EmfPlusBoundaryPointData object (section 2.2.2.7).

OptionalData (variable): An optional
                   EmfPlusPathGradientBrushOptionalData object
                   (section 2.2.2.30) that specifies additional data
                   for the path gradient brush. The specific contents
                   of this field are determined by the value of the
                   BrushDataFlags field. Graphics brushes are
                   specified by EmfPlusBrush objects (section
                   2.2.1.1). A path gradient brush paints a color
                   gradient in which the color changes gradually along
                   a gradient line from the center point outward to
                   the boundary, as specified by either a closed
                   cardinal spline or a path in the BoundaryData
                   field. Gamma correction controls the overall
                   brightness and intensity of an image. Uncorrected
                   images can look either bleached out or too
                   dark. Varying the amount of gamma correction
                   changes not only the brightness but also the ratios
                   of red to green to blue. The need for gamma
                   correction arises because an output device might
                   not render colors in the same intensity as the
                   input image. See section 2.2.2 for the
                   specification of additional structure objects.
*/


/**
   EmfPlusPathGradientBrushOptionalData Object

   The EmfPlusPathGradientBrushOptionalData object specifies optional
   data for a path gradient brush. Note Each field of this object is
   optional and might not be present in the OptionalData field of an
   EmfPlusPathGradientBrushData object (section 2.2.2.29), depending
   on the BrushData flags (section 2.1.2.1) set in its BrushDataFlags
   field. Although it is not practical to represent every possible
   combination of fields present or absent, this section specifies
   their relative order in the object. The implementer is responsible
   for determining which fields are actually present in a given
   metafile record, and for unmarshaling the data for individual
   fields separately and appropriately.

   See [MS-EMFPLUS] 2.2.2.30
*/

/*
TransformMatrix (24 bytes): An optional EmfPlusTransformMatrix object
                   (section 2.2.2.47) that specifies a world space to
                   device space transform for the path gradient
                   brush. This field MUST be present if the
                   BrushDataTransform flag is set in the
                   BrushDataFlags field of the
                   EmfPlusPathGradientBrushData object.

BlendPattern (variable): An optional blend pattern for the path
                   gradient brush. If this field is present, it MUST
                   contain either an EmfPlusBlendColors object
                   (section 2.2.2.4), or an EmfPlusBlendFactors object
                   (section 2.2.2.5), but it MUST NOT contain
                   both. The table below shows the valid combinations
                   of EmfPlusPathGradientBrushData BrushData flags and
                   the corresponding blend patterns: PresetColors
                   Clear Set Clear BlendFactorsH Clear Clear Set Blend
                   Patterns This field MUST NOT be present. An
                   EmfPlusBlendColors object MUST be present. An
                   EmfPlusBlendFactors object MUST be present.

FocusScaleData (12 bytes): An optional EmfPlusFocusScaleData object
                   (section 2.2.2.18) that specifies focus scales for
                   the path gradient brush. This field MUST be present
                   if the BrushDataFocusScales flag is set in the
                   BrushDataFlags field of the
                   EmfPlusPathGradientBrushData object. Graphics
                   brushes are specified by EmfPlusBrush objects
                   (section 2.2.1.1).

See section 2.2.2 for the specification of additional structure objects.
*/


/**
   EmfPlusPathPointType Object

   The EmfPlusPathPointType object specifies a type value associated
   with a point on a graphics path.

   See [MS-EMFPLUS] 2.2.2.31
*/

/*
Flags (4 bits): A 4-bit flag field that specifies properties of the
                   path point. This value MUST be one or more of the
                   PathPointType flags (section 2.1.2.6).

Type (4 bits): A 4-bit unsigned integer path point type. This value
                   MUST be defined in the PathPointType enumeration
                   (section 2.1.1.23). Graphics paths are specified by
                   EmfPlusPath objects (section 2.2.1.6). Every point
                   on a graphics path MUST have a type value
                   associated with it. See section 2.2.2 for the
                   specification of additional structure objects.
*/


/**
   EmfPlusPathPointTypeRLE Object

   The EmfPlusPathPointTypeRLE object specifies type values associated
   with points on a graphics path using RLE compression.

   See [MS-EMFPLUS] 2.2.2.32
*/

/*
B (1 bit): If set, the path points are on a Bezier curve. If clear,
                   the path points are on a graphics line.

RunCount (6 bits): The run count, which is the number of path points
                   to be associated with the type in the PointType field.

PointType (1 byte): An EmfPlusPathPointType object (section 2.2.2.31)
                   that specifies the type to associate with the path
                   points. Graphics paths are specified by EmfPlusPath
                   objects (section 2.2.1.6). Every point on a
                   graphics path MUST have a type value associated
                   with it. RLE compression makes it possible to
                   specify an arbitrary number of identical values
                   without a proportional increase in storage
                   requirements. See [MS-WMF] section 3.1.6 for more
                   information. See section 2.2.2 for the
                   specification of additional structure objects.
*/


/**
   EmfPlusPenData Object

   The EmfPlusPenData object specifies properties of a graphics pen.

   See [MS-EMFPLUS] 2.2.2.33
*/

/*
PenDataFlags (4 bytes): A 32-bit unsigned integer that specifies the
                   data in the OptionalData field. This value MUST be
                   composed of PenData flags (section 2.1.2.7).

PenUnit (4 bytes): A 32-bit unsigned integer that specifies the
                   measuring units for the pen. The value MUST be from
                   the UnitType enumeration (section 2.1.1.33).

PenWidth (4 bytes): A 32-bit floating-point value that specifies the
                   width of the line drawn by the pen in the units
                   specified by the PenUnit field. If a zero width is
                   specified, a minimum value is used, which is
                   determined by the units.

OptionalData (variable): An optional EmfPlusPenOptionalData object
                   (section 2.2.2.34) that specifies additional data
                   for the pen object. The specific contents of this
                   field are determined by the value of the
                   PenDataFlags field. Graphics pens are specified by
                   EmfPlusPen objects (section 2.2.1.7). See section
                   2.2.2 for the specification of additional structure
                   objects.
*/


/**
   EmfPlusPenOptionalData Object

   The EmfPlusPenOptionalData object specifies optional data for a
   graphics pen. Note Each field of this object is optional and might
   not be present in the OptionalData field of an EmfPlusPenData
   object (section 2.2.2.33), depending on the PenData flags (section
   2.1.2.7) set in its PenDataFlags field. Although it is not
   practical to represent every possible combination of fields present
   or absent, this section specifies their relative order in the
   object. The implementer is responsible for determining which fields
   are actually present in a given metafile record, and for
   unmarshaling the data for individual fields separately and
   appropriately.

   See [MS-EMFPLUS] 2.2.2.34
*/

/*
TransformMatrix (24 bytes): An optional EmfPlusTransformMatrix object
                   (section 2.2.2.47) that specifies a world space to
                   device space transform for the pen. This field MUST
                   be present if the PenDataTransform flag is set in
                   the PenDataFlags field of the EmfPlusPenData
                   object.

StartCap (4 bytes): An optional 32-bit signed integer that specifies
                   the shape for the start of a line in the
                   CustomStartCapData field. This field MUST be
                   present if the PenDataStartCap flag is set in the
                   PenDataFlags field of the EmfPlusPenData object,
                   and the value MUST be defined in the LineCapType
                   enumeration (section 2.1.1.18).

EndCap (4 bytes): An optional 32-bit signed integer that specifies the
                   shape for the end of a line in the CustomEndCapData
                   field. This field MUST be present if the
                   PenDataEndCap flag is set in the PenDataFlags field
                   of the EmfPlusPenData object, and the value MUST be
                   defined in the LineCapType enumeration.

Join (4 bytes): An optional 32-bit signed integer that specifies how
                   to join two lines that are drawn by the same pen
                   and whose ends meet. This field MUST be present if
                   the PenDataJoin flag is set in the PenDataFlags
                   field of the EmfPlusPenData object, and the value
                   MUST be defined in the LineJoinType enumeration
                   (section 2.1.1.19).

MiterLimit (4 bytes): An optional 32-bit floating-point value that
                   specifies the miter limit, which is the maximum
                   allowed ratio of miter length to line width. The
                   miter length is the distance from the intersection
                   of the line walls on the inside the join to the
                   intersection of the line walls outside the
                   join. The miter length can be large when the angle
                   between two lines is small. This field MUST be
                   present if the PenDataMiterLimit flag is set in the
                   PenDataFlags field of the EmfPlusPenData object.

LineStyle (4 bytes): An optional 32-bit signed integer that specifies
                   the style used for lines drawn with this pen
                   object. This field MUST be present if the
                   PenDataLineStyle flag is set in the PenDataFlags
                   field of the EmfPlusPenData object, and the value
                   MUST be defined in the LineStyle enumeration
                   (section 2.1.1.20).

DashedLineCapType (4 bytes): An optional 32-bit signed integer that
                   specifies the shape for both ends of each dash in a
                   dashed line. This field MUST be present if the
                   PenDataDashedLineCap flag is set in the
                   PenDataFlags field of the EmfPlusPenData object,
                   and the value MUST be defined in the
                   DashedLineCapType enumeration (section 2.1.1.10).

DashOffset (4 bytes): An optional 32-bit floating-point value that
                   specifies the distance from the start of a line to
                   the start of the first space in a dashed line
                   pattern. This field MUST be present if the
                   PenDataDashedLineOffset flag is set in the
                   PenDataFlags field of the EmfPlusPenData
                   object. DashedLineData (variable): An optional
                   EmfPlusDashedLineData object (section 2.2.2.16)
                   that specifies the lengths of dashes and spaces in
                   a custom dashed line. This field MUST be present if
                   the PenDataDashedLine flag is set in the
                   PenDataFlags field of the EmfPlusPenData object.

PenAlignment (4 bytes): An optional 32-bit signed integer that
                   specifies the distribution of the pen width with
                   respect to the coordinates of the line being
                   drawn. This field MUST be present if the
                   PenDataNonCenter flag is set in the PenDataFlags
                   field of the EmfPlusPenData object, and the value
                   MUST be defined in the PenAlignment enumeration
                   (section 2.1.1.24). For example, consider the
                   placement of a line. If the starting and ending
                   coordinates of the line are defined, it is possible
                   to think of a theoretical line between the two
                   points that is zero width. Center alignment means
                   that the pen width is distributed as evenly as
                   possible on either side of that theoretical line.

CompoundLineData (variable): An optional EmfPlusCompoundLineData
                   object (section 2.2.2.9) that specifies an array of
                   floating-point values that define the compound line
                   of a pen, which is made up of parallel lines and
                   spaces. This field MUST be present if the
                   PenDataCompoundLine flag is set in the PenDataFlags
                   field of the EmfPlusPenData object.

CustomStartCapData (variable): An optional EmfPlusCustomStartCapData
                   object (section 2.2.2.15) that defines the custom
                   start-cap shape, which is the shape to use at the
                   start of a line drawn with this pen. It can be any
                   of various shapes, such as a square, circle, or
                   diamond. This field MUST be present if the
                   PenDataCustomStartCap flag is set in the
                   PenDataFlags field of the EmfPlusPenData object.

CustomEndCapData (variable): An optional EmfPlusCustomEndCapData
                   object (section 2.2.2.11) that defines the custom
                   end-cap shape, which is the shape to use at the end
                   of a line drawn with this pen. It can be any of
                   various shapes, such as a square, circle, or
                   diamond. This field MUST be present if the
                   PenDataCustomEndCap flag is set in the PenDataFlags
                   field of the EmfPlusPenData object. Graphics pens
                   are specified by EmfPlusPen objects (section
                   2.2.1.7). See section 2.2.2 for the specification
                   of additional structure objects.
*/


/**
   EmfPlusPoint Object

   The EmfPlusPoint object specifies an ordered pair of integer (X,Y)
   values that define an absolute location in a coordinate space.

   See [MS-EMFPLUS] 2.2.2.35
*/

/*
X (2 bytes): A 16-bit signed integer that defines the horizontal coordinate. 

Y (2 bytes): A 16-bit signed integer that defines the vertical
                   coordinate. See section 2.2.2 for the specification
                   of additional structure objects.
*/


/**
   EmfPlusPointF Object

   The EmfPlusPointF object specifies an ordered pair of
   floating-point (X,Y) values that define an absolute location in a
   coordinate space.

   See [MS-EMFPLUS] 2.2.2.36
*/

/*
X (4 bytes): A 32-bit floating-point value that specifies the
                   horizontal coordinate.

Y (4 bytes): A 32-bit floating-point value that specifies the vertical
                   coordinate. See section 2.2.2 for the specification
                   of additional structure objects.
*/


/**
   EmfPlusPointR Object

   The EmfPlusPointR object specifies an ordered pair of integer (X,Y)
   values that define a relative location in a coordinate space.

   See [MS-EMFPLUS] 2.2.2.37
*/

/*
X (variable): A signed integer that specifies the horizontal
                   coordinate. This value MUST be specified by either
                   an EmfPlusInteger7 object (section 2.2.2.21) or an
                   EmfPlusInteger15 object (section 2.2.2.22).

Y (variable): A signed integer that specifies the vertical
                   coordinate. This value MUST be specified by either
                   an EmfPlusInteger7 object or an EmfPlusInteger15
                   object. Note The object that specifies the
                   horizontal coordinate is not required to be the
                   same type as the object that specifies the vertical
                   coordinate; that is, one can be 7 bits and the
                   other can be 15 bits. See section 2.2.2 for the
                   specification of additional structure objects.
*/


/**
   EmfPlusRect Object

   The EmfPlusRect object specifies a rectangle origin, height, and
   width as 16-bit signed integers.

   See [MS-EMFPLUS] 2.2.2.38
*/

/*
X (2 bytes): A 16-bit signed integer that specifies the horizontal
                   coordinate of the upper-left corner of the
                   rectangle.

Y (2 bytes): A 16-bit signed integer that specifies the vertical
                   coordinate of the upper-left corner of the
                   rectangle.

Width (2 bytes): A 16-bit signed integer that specifies the width of
                   the rectangle.

Height (2 bytes): A 16-bit signed integer that specifies the height of
                   the rectangle. See section 2.2.2 for the
                   specification of additional structure objects.
*/

QRectF emfPlusRectFromStream(QDataStream &stream);

/**
   EmfPlusRectF Object

   The EmfPlusRectF object specifies a rectangle's origin, height, and
   width as 32-bit floating-point values.

   See [MS-EMFPLUS] 2.2.2.39
*/



/*
X (4 bytes): A 32-bit floating-point value that specifies the
             horizontal coordinate of the upperleft corner of the
             rectangle.

Y (4 bytes): A 32-bit floating-point value that specifies the vertical
             coordinate of the upper-left corner of the rectangle.

Width (4 bytes): A 32-bit floating-point value that specifies the width of the rectangle. 

Height (4 bytes): A 32-bit floating-point value that specifies the
           height of the rectangle. See section 2.2.2 for the
           specification of additional structure objects.

*/



/**
   EmfPlusRegionNode Object

   The EmfPlusRegionNode object specifies nodes of a graphics region.

   See [MS-EMFPLUS] 2.2.2.40
*/

/*
Type (4 bytes): A 32-bit unsigned integer that specifies the type of
                   data in the RegionNodeData field. This value MUST
                   be defined in the RegionNodeDataType enumeration
                   (section 2.1.1.27).

RegionNodeData (variable): Optional, variable-length data that defines
                   the region node data object specified in the Type
                   field. The content and format of the data can be
                   different for every region node type. This field
                   MUST NOT be present if the node type is
                   RegionNodeDataTypeEmpty or
                   RegionNodeDataTypeInfinite. Graphics regions are
                   specified by EmfPlusRegion objects (section
                   2.2.1.8), which define a binary tree of region
                   nodes. Each node MUST either be a terminal node or
                   specify additional region nodes. This object is
                   generic and is used to specify different types of
                   region node data, including: An
                   EmfPlusRegionNodePath object (section 2.2.2.42),
                   for a terminal node; An EmfPlusRectF object
                   (section 2.2.2.39), for a terminal node; and An
                   EmfPlusRegionNodeChildNodes object (section
                   2.2.2.41), for a non-terminal node. See section
                   2.2.2 for the specification of additional structure
                   objects.
*/


/**
   EmfPlusRegionNodeChildNodes Object

   The EmfPlusRegionNodeChildNodes object specifies child nodes of a
   graphics region node.

   See [MS-EMFPLUS] 2.2.2.41
*/

/*
Left (variable): An EmfPlusRegionNode object that specifies the left
                   child node of this region node.

Right (variable): An EmfPlusRegionNode object that defines the right
                   child node of this region node. Graphics region
                   nodes are specified with EmfPlusRegionNode objects
                   (section 2.2.2.40). See section 2.2.2 for the
                   specification of additional structure objects.
*/


/**
   EmfPlusRegionNodePath Object

   The EmfPlusRegionNodePath object specifies a graphics path for
   drawing the boundary of a region node.

   See [MS-EMFPLUS] 2.2.2.42
*/

/*
RegionNodePathLength (4 bytes): A 32-bit signed integer that specifies
                   the length in bytes of the RegionNodePath field.

RegionNodePath (variable): An EmfPlusPath object (section 2.2.1.6)
                   that specifies the boundary of the region
                   node. Region nodes are specified by EmfPlusRegion
                   objects (section 2.2.1.8). An EmfPlusRegionNodePath
                   object MUST be present in the RegionNodeData field
                   of an EmfPlusRegionNode object (section 2.2.2.40)
                   if its Type field is set to the
                   RegionNodeDataTypePath value from the
                   RegionNodeDataType enumeration (section
                   2.1.1.27). See section 2.2.2 for the specification
                   of additional structure objects.
*/


/**
   EmfPlusSolidBrushData Object

   The EmfPlusSolidBrushData object specifies a solid color for a
   graphics brush.

   See [MS-EMFPLUS] 2.2.2.43
*/

/*
SolidColor (4 bytes): An EmfPlusARGB object (section 2.2.2.1) that
                   specifies the color of the brush. Graphics brushes
                   are specified by EmfPlusBrush objects (section
                   2.2.1.1). A solid color brush paints a background
                   in a solid color. See section 2.2.2 for the
                   specification of additional structure objects.
*/


/**
   EmfPlusStringFormatData Object

   The EmfPlusStringFormatData object specifies tab stops and
   character positions for a graphics string.

   See [MS-EMFPLUS] 2.2.2.44
*/

/*
TabStops (variable): An optional array of floating-point values that
                   specify the optional tab stop locations for this
                   object. Each tab stop value represents the number
                   of spaces between tab stops or, for the first tab
                   stop, the number of spaces between the beginning of
                   a line of text and the first tab stop. This field
                   MUST be present if the value of the TabStopCount
                   field in the EmfPlusStringFormat object is greater
                   than 0.

CharRange (variable): An optional array of RangeCount
                   EmfPlusCharacterRange objects that specify the
                   range of character positions within a string of
                   text. The bounding region is defined by the area of
                   the display that is occupied by a group of
                   characters specified by the character range. This
                   field MUST be present if the value of the
                   RangeCount field in the EmfPlusStringFormat object
                   is greater than 0. Graphics strings are specified
                   by EmfPlusStringFormat objects (section
                   2.2.1.9). See section 2.2.2 for the specification
                   of additional structure objects.
*/



/**
   EmfPlusTextureBrushData Object

   The EmfPlusTextureBrushData object specifies a texture image for a
   graphics brush.

   See [MS-EMFPLUS] 2.2.2.45
*/

/*
BrushDataFlags (4 bytes): A 32-bit unsigned integer that specifies the
                   data in the OptionalData field. This value MUST be
                   composed of BrushData flags (section 2.1.2.1). The
                   following flags are relevant to a texture brush:
                   Name BrushDataTransform BrushDataIsGammaCorrected
                   BrushDataDoNotTransform Value 0x00000002 0x00000080
                   0x00000100

WrapMode (4 bytes): A 32-bit signed integer from the WrapMode
                   enumeration (section 2.1.1.34) that specifies how
                   to repeat the texture image across a shape, when
                   the image is smaller than the area being filled.

OptionalData (variable): An optional EmfPlusTextureBrushOptionalData
                   object (section 2.2.2.46) that specifies additional
                   data for the texture brush. The specific contents
                   of this field are determined by the value of the
                   BrushDataFlags field. Graphics brushes are
                   specified by EmfPlusBrush objects (section
                   2.2.1.1). A texture brush paints an image, which in
                   this context is called a "texture". The texture
                   consists of either a portion of an image or a
                   scaled version of an image, as specified by an
                   EmfPlusImage object (section 2.2.1.4) in the
                   OptionalData field. Gamma correction controls the
                   overall brightness and intensity of an
                   image. Uncorrected images can look either bleached
                   out or too dark. Varying the amount of gamma
                   correction changes not only the brightness but also
                   the ratios of red to green to blue. The need for
                   gamma correction arises because an output device
                   might not render colors in the same intensity as
                   the input image. See section 2.2.2 for the
                   specification of additional structure objects.
*/


/**
   EmfPlusTextureBrushOptionalData Object

   The EmfPlusTextureBrushOptionalData object specifies optional data
   for a texture brush. Note Each field of this object is optional and
   might not be present in the OptionalData field of an
   EmfPlusTextureBrushData object (section 2.2.2.45), depending on the
   BrushData flags (section 2.1.2.1) set in its BrushDataFlags
   field. Although it is not practical to represent every possible
   combination of fields present or absent, this section specifies
   their relative order in the object. The implementer is responsible
   for determining which fields are actually present in a given
   metafile record, and for unmarshaling the data for individual
   fields separately and appropriately.

   See [MS-EMFPLUS] 2.2.2.46
*/

/*
TransformMatrix (24 bytes): An optional EmfPlusTransformMatrix object
                   (section 2.2.2.47) that specifies a world space to
                   device space transform for the texture brush. This
                   field MUST be present if the BrushDataTransform
                   flag is set in the BrushDataFlags field of the
                   EmfPlusTextureBrushData object.

ImageObject (variable): An optional EmfPlusImage object (section
                   2.2.1.4) that specifies the brush texture. This
                   field MUST be present if the size of the
                   EmfPlusObject record (section 2.3.5.1) that defines
                   this texture brush is large enough to accommodate
                   an EmfPlusImage object in addition to the required
                   fields of the EmfPlusTextureBrushData object and
                   optionally an EmfPlusTransformMatrix
                   object. Graphics brushes are specified by
                   EmfPlusBrush objects (section 2.2.1.1). See section
                   2.2.2 for the specification of additional structure
                   objects.
*/


/**
   EmfPlusTransformMatrix Object

   The EmfPlusTransformMatrix object specifies a world space to device
   space transform.

   See [MS-EMFPLUS] 2.2.2.47
*/


/*
TransformMatrix (24 bytes): This field specifies an affine transform,
                   which requires a 2x2 matrix for a linear
                   transformation and a 1x2 matrix for a
                   translation. These values map to the coordinates of
                   the transform matrix as follows:
                   TransformMatrix[0] Corresponds to m11, which is
                   the coordinate of the first row and first column of
                   the 2x2 matrix. TransformMatrix[1] Corresponds to
                   m12, which is the coordinate of the first row and
                   second column of the 2x2
                   matrix. TransformMatrix[2] Corresponds to m21,
                   which is the coordinate of the second row and first
                   column of the 2x2 matrix. TransformMatrix[3]
                   Corresponds to m22, which is the coordinate of the
                   second row and second column of the 2x2
                   matrix. TransformMatrix[4] Corresponds to dx,
                   which is the horizontal displacement in the 1x2
                   matrix. TransformMatrix[5] Corresponds to dy,
                   which is the vertical displacement in the 1x2
                   matrix. See section 2.2.2 for the specification of
                   additional structure objects.
*/


// ----------------------------------------------------------------
// 2.2.1 Graphics Object Types


/**
   EmfPlusBrush Object

   The EmfPlusBrush object specifies a graphics brush for filling regions.

   See [MS-EMFPLUS] 2.2.1.1
*/

struct EmfPlusBrush {
    EmfPlusGraphicsVersion   version;
    quint32                  type;
    //BrushData

    EmfPlusBrush(QDataStream &stream, quint32 size);
};

/*

Version (4 bytes): An EmfPlusGraphicsVersion object (section 2.2.2.19)
                   that specifies the version of operating system
                   graphics that was used to create this object.

Type (4 bytes): A 32-bit unsigned integer that specifies the type of
                brush, which determines the contents of the BrushData
                field. This value MUST be defined in the BrushType
                enumeration (section 2.1.1.3). BrushData (variable):
                Variable-length data that defines the brush object
                specified in the Type field. The content and format of
                the data can be different for every brush type. This
                object is generic and is used to specify different
                types of brush data, including the following objects:
                EmfPlusHatchBrushData (section 2.2.2.20)
                EmfPlusLinearGradientBrushData object (section
                2.2.2.24) EmfPlusPathGradientBrushData object
                (section 2.2.2.29) EmfPlusSolidBrushData object
                (section 2.2.2.43) EmfPlusTextureBrushData object
                (section 2.2.2.45) See section 2.2.1 for the
                specification of additional graphics objects.
*/


/**
   EmfPlusCustomLineCap Object

   The EmfPlusCustomLineCap object specifies the shape to use at the
   ends of a line drawn by a graphics pen.

   See [MS-EMFPLUS] 2.2.1.2
*/

/*
Version Type CustomLineCapData (variable) ...

Version (4 bytes): An EmfPlusGraphicsVersion object (section 2.2.2.19)
                   that specifies the version of operating system
                   graphics that was used to create this object.

Type (4 bytes): A 32-bit signed integer that specifies the type of
                custom line cap object, which determines the contents
                of the CustomLineCapData field. This value MUST be
                defined in the CustomLineCapDataType enumeration
                (section 2.1.1.9). CustomLineCapData (variable):
                Variable-length data that defines the custom line cap
                data object specified in the Type field. The content
                and format of the data can be different for every
                custom line cap type. This object is generic and is
                used to specify different types of custom line cap
                data, including: An EmfPlusCustomLineCapArrowData
                object (section 2.2.2.12); and An
                EmfPlusCustomLineCapData object (section
                2.2.2.13). See section 2.2.1 for the specification of
                additional graphics objects.

*/

/**
   EmfPlusFont Object

   The EmfPlusFont object specifies properties that determine the
   appearance of text, including typeface, size, and style.

   See [MS-EMFPLUS] 2.2.1.3
*/


/*
Version (4 bytes): An EmfPlusGraphicsVersion object (section 2.2.2.19)
                   that specifies the version of operating system
                   graphics that was used to create this object.

EmSize (4 bytes):  A 32-bit floating-point value that specifies the em
                   size of the font in units specified by the SizeUnit field.

SizeUnit (4 bytes): A 32-bit unsigned integer that specifies the units
                    used for the EmSize field. These are typically the
                    units that were employed when designing the
                    font. The value MUST be in the UnitType
                    enumeration (section 2.1.1.33).<10>

FontStyleFlags (4 bytes): A 32-bit signed integer that specifies
                          attributes of the character glyphs that
                          affect the appearance of the font, such as
                          bold and italic. This value MUST be composed
                          of FontStyle flags (section 2.1.2.4).

Reserved (4 bytes): A 32-bit unsigned integer that is reserved and
                    MUST be ignored.

Length (4 bytes): A 32-bit unsigned integer that specifies the number
                  of characters in the FamilyName field. FamilyName
                  (variable): A string of Length Unicode characters
                  that contains the name of the font family. See
                  section 2.2.1 for the specification of additional
                  graphics objects.
*/

/**
   EmfPlusImage Object

   The EmfPlusImage object specifies a graphics image in the form of a
   bitmap or metafile.

   See [MS-EMFPLUS] 2.2.1.4
*/

/*
Version (4 bytes): An EmfPlusGraphicsVersion object (section 2.2.2.19)
                   that specifies the version of operating system
                   graphics that was used to create this object.

Type (4 bytes): A 32-bit unsigned integer that specifies the type of
                data in the ImageData field. This value MUST be
                defined in the ImageDataType enumeration (section
                2.1.1.15). ImageData (variable): Variable-length data
                that defines the image data specified in the Type
                field. The content and format of the data can be
                different for every image type. This object is generic
                and is used to specify different types of image data,
                including: An EmfPlusBitmap object (section
                2.2.2.2); and An EmfPlusMetafile object (section
                2.2.2.27). See section 2.2.1 for the specification of
                additional graphics objects.
*/


/**
   EmfPlusImageAttributes Object

   The EmfPlusImageAttributes object specifies how bitmap image colors
   are manipulated during rendering.

   See [MS-EMFPLUS] 2.2.1.5
*/

/*
  Version (4 bytes): An EmfPlusGraphicsVersion object (section
                     2.2.2.19) that specifies the version of operating
                     system graphics that was used to create this
                     object.

Reserved1 (4 bytes): A 32-bit field that is not used and MUST be ignored. 

WrapMode (4 bytes): A 32-bit unsigned integer that specifies how to
                    handle edge conditions with a value from the
                    WrapMode enumeration (section 2.1.1.34).

ClampColor (4 bytes): An EmfPlusARGB (section 2.2.2.1) object that
                    specifies the edge color to use when the WrapMode
                    value is WrapModeClamp. This color is visible when
                    the source rectangle processed by an
                    EmfPlusDrawImage (section 2.3.4.8) record is
                    larger than the image itself.

ObjectClamp (4 bytes): A 32-bit signed integer that specifies the
                    object clamping behavior. It is not used until
                    this object is applied to an image being
                    drawn. This value MUST be one of the values
                    defined in the following table.

Value
  RectClamp 0x00000000    Meaning The object is clamped to a rectangle.
  BitmapClamp 0x00000001  The object is clamped to a bitmap.

Reserved2 (4 bytes): A value that SHOULD be set to zero and MUST be
                    ignored upon receipt. See section 2.2.1 for the
                    specification of additional graphics objects.
*/

/**
   EmfPlusPath Object

   The EmfPlusPath object specifies a series of line and curve
   segments that form a graphics path.
   
   See [MS-EMFPLUS] 2.2.1.6
*/

/*
Version (4 bytes): An EmfPlusGraphicsVersion object (section 2.2.2.19)
                   that specifies the version of operating system
                   graphics that was used to create this object.

PathPointCount (4 bytes): A 32-bit unsigned integer that specifies the
                   number of points and associated point types that
                   are defined by this object.

PathPointFlags (2 bytes): A 32-bit unsigned integer that specifies how
                   to interpret the points and associated point types
                   that are defined by this object.

C (1 bit): If set, the PathPoints array specifies absolute locations
                   in the coordinate space with 16-bit integer
                   coordinates. If clear, the PathPoints array
                   specifies absolute locations in the coordinate
                   space with 32-bit floating-point coordinates. Note
                   If the P flag (below) is set, this flag MAY <11> be
                   clear and MUST be ignored. R (1 bit): If set, the
                   point types in the PathPointTypes array are
                   specified by EmfPlusPathPointTypeRLE objects
                   (section 2.2.2.32), which use run-length encoding
                   (RLE) compression, and/or EmfPlusPathPointType
                   objects (section 2.2.2.31). See [MS-WMF] section
                   3.1.6 for more information on RLE compression. If
                   clear, the point types in the PathPointTypes array
                   are specified by EmfPlusPathPointType objects. P (1
                   bit): If set, each element in the PathPoints array
                   specifies a location in the coordinate space that
                   is relative to the location specified by the
                   previous element in the array. In the case of the
                   first element in PathPoints, a previous location at
                   coordinates (0,0) is assumed.

                   If clear, each element in the PathPoints array
                   specifies an absolute location.

Reserved (2 bytes): This field is reserved and MUST be
                   ignored. PathPoints (variable): An array of
                   PathPointCount points that specify the path. The
                   type of objects in this array are specified by the
                   PathPointFlags field, as follows: If the P flag
                   is set, the points are relative locations that are
                   specified by EmfPlusPointR objects (section
                   2.2.2.37). If the P flag is clear and the C flag
                   is set, the points are absolute locations that are
                   specified by EmfPlusPoint objects (section
                   2.2.2.35). If the P flag is clear and the C flag
                   is clear, the points are absolute locations that
                   are specified by EmfPlusPointF objects (section
                   2.2.2.36). PathPointTypes (variable): An array that
                   specifies how the points in the PathPoints field
                   are used to draw the path. The type of objects in
                   this array is specified by the R flag in the
                   PathPointFlags field. See section 2.2.1 for the
                   specification of additional graphics objects.
*/


/**
  EmfPlusPen Object

  The EmfPlusPen object specifies a graphics pen for the drawing of lines.

  See [MS-EMFPLUS] 2.2.1.7
*/

/*
Version (4 bytes): An EmfPlusGraphicsVersion object (section 2.2.2.19)
                   that specifies the version of operating system
                   graphics that was used to create this object.

Type (4 bytes): This field MUST be set to zero. PenData (variable): An
                   EmfPlusPenData object that specifies properties of
                   the graphics pen. BrushObject (variable): An
                   EmfPlusBrush object that specifies a graphics brush
                   associated with the pen. See section 2.2.1 for the
                   specification of additional graphics objects.
*/


/**
   EmfPlusRegion Object

   The EmfPlusRegion object specifies line and curve segments that
   define a nonrectilinear shape.

   See [MS-EMFPLUS] 2.2.1.8
*/


/*
Version (4 bytes): An EmfPlusGraphicsVersion object (section 2.2.2.19)
                   that specifies the version of operating system
                   graphics that was used to create this object.

RegionNodeCount (4 bytes): A 32-bit unsigned integer that specifies
                   the number of child nodes in the RegionNode
                   field. RegionNode (variable): An array of
                   RegionNodeCount+1 EmfPlusRegionNode objects
                   (section 2.2.2.40). Regions are specified as a
                   binary tree of region nodes, and each node MUST
                   either be a terminal node or specify one or two
                   child nodes. RegionNode MUST contain at least one
                   element. See section 2.2.1 for the specification of
                   additional graphics objects.
*/


/**
   EmfPlusStringFormat Object

   The EmfPlusStringFormat object specifies text layout, display
   manipulations, and language identification.

   See [MS-EMFPLUS] 2.2.1.9
*/


/*
Version (4 bytes): An EmfPlusGraphicsVersion object (section 2.2.2.19)
                   that specifies the version of operating system
                   graphics that was used to create this object.

StringFormatFlags (4 bytes): A 32-bit unsigned integer that specifies
                   text layout options for formatting, clipping and
                   font handling. This value MUST be composed of
                   StringFormat flags (section 2.1.2.8).

Language (4 bytes): An EmfPlusLanguageIdentifier object (section
                   2.2.2.23) that specifies the language to use for
                   the string.

StringAlignment (4 bytes): A 32-bit unsigned integer that specifies
                   how to align the string horizontally in the layout
                   rectangle. This value MUST be defined in the
                   StringAlignment enumeration (section 2.1.1.29).

LineAlign (4 bytes): A 32-bit unsigned integer that specifies how to
                   align the string vertically in the layout
                   rectangle. This value MUST be defined in the
                   StringAlignment enumeration.

DigitSubstitution (4 bytes): A 32-bit unsigned integer that specifies
                   how to substitute numeric digits in the string
                   according to a locale or language. This value MUST
                   be defined in the StringDigitSubstitution
                   enumeration (section 2.1.1.30).

DigitLanguage (4 bytes): An EmfPlusLanguageIdentifier object that
                   specifies the language to use for numeric digits in
                   the string. For example, if this string contains
                   Arabic digits, this field MUST contain a language
                   identifier that specifies an Arabic language.

FirstTabOffset (4 bytes): A 32-bit floating-point value that specifies
                   the number of spaces between the beginning of a
                   text line and the first tab stop.

HotkeyPrefix (4 bytes): A 32-bit signed integer that specifies the
                   type of processing that is performed on a string
                   when a keyboard shortcut prefix (that is, an
                   ampersand) is encountered. Basically, this field
                   specifies whether to display keyboard shortcut
                   prefixes that relate to text. The value MUST be
                   defined in the HotkeyPrefix enumeration (section
                   2.1.1.14).

LeadingMargin (4 bytes): A 32-bit floating-point value that specifies
                   the length of the space to add to the starting
                   position of a string. The default is 1/6 inch; for
                   typographic fonts, the default value is 0.

TrailingMargin (4 bytes): A 32-bit floating-point value that specifies
                   the length of the space to leave following a
                   string. The default is 1/6 inch; for typographic
                   fonts, the default value is 0.

Tracking (4 bytes): A 32-bit floating-point value that specifies the
                   ratio of the horizontal space allotted to each
                   character in a specified string to the font-defined
                   width of the character. Large values for this
                   property specify ample space between characters;
                   values less than 1 can produce character
                   overlap. The default is 1.03; for typographic
                   fonts, the default value is 1.00.

Trimming (4 bytes): Specifies how to trim characters from a string
                   that is too large to fit into a layout
                   rectangle. This value MUST be defined in the
                   StringTrimming enumeration (section 2.1.1.31).

TabStopCount (4 bytes): A 32-bit signed integer that specifies the
                   number of tab stops defined in the StringFormatData field.

RangeCount (4 bytes): A 32-bit signed integer that specifies the
                   number of EmfPlusCharacterRange objects (section
                   2.2.2.8) defined in the StringFormatData
                   field. StringFormatData (variable): An
                   EmfPlusStringFormatData object (section 2.2.2.44)
                   that specifies optional text layout data. See
                   section 2.2.1 for the specification of additional
                   graphics objects.
*/


// ----------------------------------------------------------------
// 2.2.3 Image Effects Object Types

/**
The Image Effects Objects specify parameters for graphics image
effects, which can be applied to bitmap images.<13> Parameters are
specified for the following image effects: Name BlurEffect
BrightnessContrastEffect Section 2.2.3.1 2.2.3.2 Description Specifies
a decrease in the difference in intensity between pixels in an
image. Specifies an expansion or contraction of the lightest and
darkest areas of an image.
*/

/**
   BlurEffect Object

   The BlurEffect object specifies a decrease in the difference in
   intensity between pixels in an image.

   See [MS-EMFPLUS] 2.2.3.1
*/

struct BlurEffect {
    float   blurRadius;
    quint32 expandEdge;

    BlurEffect(QDataStream &stream);
};

/*
BlurRadius (4 bytes): A 32-bit floating-point number that specifies
                   the blur radius in pixels, which determines the
                   number of pixels involved in calculating the new
                   value of a given pixel. This value MUST be in the
                   range 0.0 through 255.0. As this value increases,
                   the number of pixels involved in the calculation
                   increases, and the resulting bitmap SHOULD become
                   more blurry.

ExpandEdge (4 bytes): A 32-bit Boolean value that specifies whether
                   the bitmap expands by an amount equal to the value
                   of the BlurRadius to produce soft edges. This value
                   MUST be one of the following: Value FALSE
                   0x00000000 Meaning The size of the bitmap MUST NOT
                   change, and its soft edges SHOULD be clipped to the
                   size of the BlurRadius.

                   Value TRUE 0x00000001

                   Meaning The size of the bitmap SHOULD expand by an
                   amount equal to the BlurRadius to produce soft
                   edges.

                   Bitmap images are specified by EmfPlusBitmap
                   objects (section 2.2.2.2). See section 2.2.3 for
                   the specification of additional image effects
                   parameter objects.
*/


/**
   BrightnessContrastEffect Object

   The BrightnessContrastEffect object specifies an expansion or
   contraction of the lightest and darkest areas of an image.

   See [MS-EMFPLUS] 2.2.3.2
*/

/*
BrightnessLevel (4 bytes): A 32-bit signed integer that specifies the
                   brightness level. This value MUST be in the range
                   -255 through 255, with effects as follows: Value
                   -255 ≤ value < 0 0 0 < value ≤ 255 Meaning As the
                   value decreases, the brightness of the image SHOULD
                   decrease. A value of 0 specifies that the
                   brightness MUST NOT change. As the value increases,
                   the brightness of the image SHOULD increase.

ContrastLevel (4 bytes): A 32-bit signed integer that specifies the
                   contrast level. This value MUST be in the range
                   -100 through 100, with effects as follows: Value
                   -100 ≤ value < 0 0 0 < value ≤ 100 Meaning As the
                   value decreases, the contrast of the image SHOULD
                   decrease. A value of 0 specifies that the contrast
                   MUST NOT change. As the value increases, the
                   contrast of the image SHOULD increase.

                   Bitmap images are specified by EmfPlusBitmap
                   objects (section 2.2.2.2). See section 2.2.3 for
                   the specification of additional image effects
                   parameter objects.
*/


/**
   ColorBalanceEffect Object

   The ColorBalanceEffect object specifies adjustments to the relative
   amounts of red, green, and blue in an image.

   See [MS-EMFPLUS] 2.2.3.3
*/

/*
CyanRed (4 bytes): A 32-bit signed integer that specifies a change in
                   the amount of red in the image. This value MUST be
                   in the range -100 through 100, with effects as
                   follows: Value -100 ≤ value <0 0 0 < value ≤ 100
                   Meaning As the value decreases, the amount of red
                   in the image SHOULD decrease and the amount of cyan
                   SHOULD increase. A value of 0 specifies that the
                   amounts of red and cyan MUST NOT change. As the
                   value increases, the amount of red in the image
                   SHOULD increase and the amount of cyan SHOULD
                   decrease.

MagentaGreen (4 bytes): A 32-bit signed integer that specifies a
                   change in the amount of green in the image. This
                   value MUST be in the range -100 through 100, with
                   effects as follows: Value -100 ≤ value <0 0 0 <
                   value ≤ 100 Meaning As the value decreases, the
                   amount of green in the image SHOULD decrease and
                   the amount of magenta SHOULD increase. A value of 0
                   specifies that the amounts of green and magenta
                   MUST NOT change. As the value increases, the amount
                   of green in the image SHOULD increase and the
                   amount of magenta SHOULD decrease.


YellowBlue (4 bytes): A 32-bit signed integer that specifies a change
                   in the amount of blue in the image. This value MUST
                   be in the range -100 through 100, with effects as
                   follows: Value -100 ≤ value <0 0 0 < value ≤ 100
                   Meaning As the value decreases, the amount of blue
                   in the image SHOULD decrease and the amount of
                   yellow SHOULD increase. A value of 0 specifies that
                   the amounts of blue and yellow MUST NOT change. As
                   the value increases, the amount of blue in the
                   image SHOULD increase and the amount of yellow
                   SHOULD decrease.

Bitmap images are specified by EmfPlusBitmap objects (section
2.2.2.2). See section 2.2.3 for the specification of additional image
effects parameter objects.  
*/


/**
   ColorCurveEffect Object

   The ColorCurveEffect object specifies one of eight adjustments to
   the color curve of an image.

   See [MS-EMFPLUS] 2.2.3.4
*/

/*
CurveAdjustment (4 bytes): A 32-bit unsigned integer that specifies
                   the curve adjustment to apply to the colors in
                   bitmap. This value MUST be defined in the
                   CurveAdjustments enumeration (section 2.1.1.7).

CurveChannel (4 bytes): A 32-bit unsigned integer that specifies the
                   color channel to which the curve adjustment
                   applies. This value MUST be defined in the
                   CurveChannel enumeration (section 2.1.1.8).

AdjustmentIntensity (4 bytes): A 32-bit signed integer that specifies
                   the intensity of the curve adjustment to the color
                   channel specified by CurveChannel. The ranges of
                   meaningful values for this field vary according to
                   the CurveAdjustment value, as follows: Exposure
                   adjustment range: Value -255 ≤ value < 0 0 0 <
                   value ≤ 255 Meaning As the value decreases, the
                   exposure of the image SHOULD decrease. A value of 0
                   specifies that the exposure MUST NOT change. As the
                   value increases, the exposure of the image SHOULD
                   increase.

Density adjustment range: Value -255 ≤ value < 0 0 0 < value ≤ 255
Meaning As the value decreases, the density of the image SHOULD
decrease, resulting in a darker image. A value of 0 specifies that the
density MUST NOT change. As the value increases, the density of the
image SHOULD increase.

Contrast adjustment range: Value -100 ≤ value < 0 Meaning As the value
decreases, the contrast of the image SHOULD decrease. Value 0 0 <
value ≤ 100 Meaning A value of 0 specifies that the contrast MUST NOT
change. As the value increases, the contrast of the image SHOULD
increase.

Highlight adjustment range: Value -100 ≤ value < 0 0 0 < value ≤ 100
Meaning As the value decreases, the light areas of the image SHOULD
appear darker. A value of 0 specifies that the highlight MUST NOT
change. As the value increases, the light areas of the image SHOULD
appear lighter.

Shadow adjustment range: Value -100 ≤ value < 0 0 0 < value ≤ 100
Meaning As the value decreases, the dark areas of the image SHOULD
appear darker. A value of 0 specifies that the shadow MUST NOT
change. As the value increases, the dark areas of the image SHOULD
appear lighter.

White saturation adjustment range: Value 0— 255 Meaning As the value
increases, the upper limit of the range of color channel intensities
increases.

Black saturation adjustment range: Value 0— 255 Meaning As the value
increases, the lower limit of the range of color channel intensities
increases.

Bitmap images are specified by EmfPlusBitmap objects (section
2.2.2.2). See section 2.2.3 for the specification of additional image
effects parameter objects.
*/

/**
   ColorLookupTableEffect Object

   The ColorLookupTableEffect object specifies adjustments to the
   colors in an image.

   See [MS-EMFPLUS] 2.2.3.5
*/

/*
BlueLookupTable (256 bytes): An array of 256 bytes that specifies the
                    adjustment for the blue color channel.

GreenLookupTable (256 bytes): An array of 256 bytes that specifies the
                    adjustment for the green color channel.

RedLookupTable (256 bytes): An array of 256 bytes that specifies the
                    adjustment for the red color channel.
                    
AlphaLookupTable (256 bytes): An array of 256 bytes that specifies the
                    adjustment for the a lpha color channel. Bitmap
                    images are specified by EmfPlusBitmap objects
                    (section 2.2.2.2). See section 2.2.3 for the
                    specification of additional image effects
                    parameter objects.
*/


/**
   ColorMatrixEffect Object

   The ColorMatrixEffect object specifies an affine transform to be
   applied to an image.

   See [MS-EMFPLUS] 2.2.3.6
*/

/*
 Bitmap images are specified by EmfPlusBitmap objects (section
2.2.2.2). A color matrix effect is performed by multiplying a color
vector by a ColorMatrixEffect object. A 5x5 color matrix can perform a
linear transform, including reflection, rotation, shearing, or scaling
followed by a translation. See section 2.2.3 for the specification of
additional image effects parameter objects.
*/


/**
   HueSaturationLightnessEffect Object

   The HueSaturationLightnessEffect object specifies adjustments to
   the hue, saturation, and lightness of an image.

   See [MS-EMFPLUS] 2.2.3.7
*/

/*
HueLevel (4 bytes): Specifies the adjustment to the hue. Value -180 ≤
                    value < 0 0 0 < value ≤ 180 Meaning Negative
                    values specify clockwise rotation on the color
                    wheel. A value of 0 specifies that the hue MUST
                    NOT change. Positive values specify
                    counter-clockwise rotation on the color wheel.

SaturationLevel (4 bytes): Specifies the adjustment to the saturation.
                    Value -100 ≤ value < 0 0 0 < value ≤ 100 Meaning
                    Negative values specify decreasing saturation. A
                    value of 0 specifies that the saturation MUST NOT
                    change. Positive values specify increasing
                    saturation.


LightnessLevel (4 bytes): Specifies the adjustment to the
                    lightness. Value -100 ≤ value < 0 0 0 < value ≤
                    100 Meaning Negative values specify decreasing
                    lightness. A value of 0 specifies that the
                    lightness MUST NOT change. Positive values specify
                    increasing lightness.

 Bitmap images are specified by EmfPlusBitmap objects (section
 2.2.2.2). See section 2.2.3 for the specification of additional image
 effects parameter objects.
*/


/**
   LevelsEffect Object

   The LevelsEffect object specifies adjustments to the highlights,
   midtones, and shadows of an image.

   See [MS-EMFPLUS] 2.2.3.8
*/

/*
Highlight (4 bytes): Specifies how much to lighten the highlights of
                    an image. The color channel values at the high end
                    of the intensity range are altered more than
                    values near the middle or low ends, which means an
                    image can be lightened without losing the contrast
                    between the darker portions of the image.  Value 0
                    ≤ value < Meaning Specifies that highlights with a
                    percent of intensity above this threshold SHOULD
                    Value 100 100 Meaning be increased. Specifies that
                    highlights MUST NOT change.

MidTone (4 bytes): Specifies how much to lighten or darken the
                    midtones of an image. Color channel values in the
                    middle of the intensity range are altered more
                    than values near the high or low ends, which means
                    an image can be lightened or darkened without
                    losing the contrast between the darkest and
                    lightest portions of the image.  Value -100 ≤
                    value < 0 0 0 < value ≤ 100 Meaning Specifies that
                    midtones are made darker. Specifies that midtones
                    MUST NOT change. Specifies that midtones are made
                    lighter.

Shadow (4 bytes): Specifies how much to darken the shadows of an
                    image. Color channel values at the low end of the
                    intensity range are altered more than values near
                    the middle or high ends, which means an image can
                    be darkened without losing the contrast between
                    the lighter portions of the image.  Value 0 0 <
                    value ≤ 100 Meaning Specifies that shadows MUST
                    NOT change. Specifies that shadows with a percent
                    of intensity below this threshold are made darker.

Bitmap images are specified by EmfPlusBitmap objects (section
2.2.2.2). See section 2.2.3 for the specification of additional image
effects parameter objects.
*/


/**
   RedEyeCorrectionEffect Object

   The RedEyeCorrectionEffect object specifies areas of an image to
   which a red-eye correction is applied.

   See [MS-EMFPLUS] 2.2.3.9
*/

/*
NumberOfAreas (4 bytes): A 32-bit signed integer that specifies the
                    number of rectangles in the Areas field.

Areas (variable): An array of NumberOfAreas WMF RectL objects,
                    specified in [MS-WMF] section 2.2.2.19. Each
                    rectangle specifies an area of the bitmap image to
                    which the red-eye correction effect SHOULD be
                    applied. Bitmap images are specified by
                    EmfPlusBitmap objects (section 2.2.2.2). See
                    section 2.2.3 for the specification of additional
                    image effects parameter objects.
*/


/**
   SharpenEffect Object

   The SharpenEffect object specifies an increase in the difference in
   intensity between pixels in an image.

   See [MS-EMFPLUS] 2.2.3.10
*/

/*
Radius (4 bytes): A 32-bit floating-point number that specifies the
                    sharpening radius in pixels, which determines the
                    number of pixels involved in calculating the new
                    value of a given pixel. As this value increases,
                    the number of pixels involved in the calculation
                    increases, and the resulting bitmap SHOULD become
                    sharper.

Amount (4 bytes): A 32-bit floating-point number that specifies the
                    difference in intensity between a given pixel and
                    the surrounding pixels.  Value 0 0 < value ≤ 100
                    Meaning Specifies that sharpening MUST NOT be
                    performed. As this value increases, the difference
                    in intensity between pixels SHOULD increase.

Bitmap images are specified by EmfPlusBitmap objects (section
2.2.2.2). See section 2.2.3 for the specification of additional image
effects parameter objects.
*/


/**
   TintEffect Object

   The TintEffect object specifies an addition of black or white to a
   specified hue in an image.

   See [MS-EMFPLUS] 2.2.3.11
*/

/*
Hue (4 bytes): A 32-bit signed integer that specifies the hue to which
                    the tint effect is applied. Value -180 ≤ value < 0
                    0 0 < value ≤ 180 Meaning The color at a specified
                    counter-clockwise rotation of the color wheel,
                    starting from blue. A value of 0 specifies the
                    color blue on the color wheel. The color at a
                    specified clockwise rotation of the color wheel,
                    starting from blue.


Amount (4 bytes): A 32-bit signed integer that specifies how much the
                    hue is strengthened or weakened. Value -100 ≤
                    value <0 0 0 < value ≤ 100 Meaning Negative values
                    specify how much the hue is weakened, which
                    equates to the addition of black. A value of 0
                    specifies that the tint MUST NOT change. Positive
                    values specify how much the hue is strengthened,
                    which equates to the addition of white.

Bitmap images are specified by EmfPlusBitmap objects (section
2.2.2.2). See section 2.2.3 for the specification of additional image
effects parameter objects.
*/

}


#endif
