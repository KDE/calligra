//========================================================================
//
// GfxState.h
//
// Copyright 1996-2002 Glyph & Cog, LLC
//
//========================================================================

#ifndef GFXSTATE_H
#define GFXSTATE_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "gtypes.h"
#include "Object.h"
#include "Function.h"

class Array;
class GfxFont;
class PDFRectangle;

//------------------------------------------------------------------------
// GfxColor
//------------------------------------------------------------------------

#define gfxColorMaxComps funcMaxOutputs

struct GfxColor {
  double c[gfxColorMaxComps];
};

//------------------------------------------------------------------------
// GfxRGB
//------------------------------------------------------------------------

struct GfxRGB {
  double r, g, b;
};

//------------------------------------------------------------------------
// GfxCMYK
//------------------------------------------------------------------------

struct GfxCMYK {
  double c, m, y, k;
};

//------------------------------------------------------------------------
// GfxColorSpace
//------------------------------------------------------------------------

enum GfxColorSpaceMode {
  csDeviceGray,
  csCalGray,
  csDeviceRGB,
  csCalRGB,
  csDeviceCMYK,
  csLab,
  csICCBased,
  csIndexed,
  csSeparation,
  csDeviceN,
  csPattern
};

class GfxColorSpace {
public:

  GfxColorSpace();
  virtual ~GfxColorSpace();
  virtual GfxColorSpace *copy() const = 0;
  virtual GfxColorSpaceMode getMode() const = 0;

  // Construct a color space.  Returns NULL if unsuccessful.
  static GfxColorSpace *parse(Object *csObj);

  // Convert to gray, RGB, or CMYK.
  virtual void getGray(const GfxColor *color, double *gray) const = 0;
  virtual void getRGB(const GfxColor *color, GfxRGB *rgb) const = 0;
  virtual void getCMYK(const GfxColor *color, GfxCMYK *cmyk) const = 0;

  // Return the number of color components.
  virtual int getNComps() const = 0;

  // Return the default ranges for each component, assuming an image
  // with a max pixel value of <maxImgPixel>.
  virtual void getDefaultRanges(double *decodeLow, double *decodeRange,
                                int maxImgPixel) const;

private:
};

//------------------------------------------------------------------------
// GfxDeviceGrayColorSpace
//------------------------------------------------------------------------

class GfxDeviceGrayColorSpace: public GfxColorSpace {
public:

  GfxDeviceGrayColorSpace();
  virtual ~GfxDeviceGrayColorSpace();
  virtual GfxColorSpace *copy() const;
  virtual GfxColorSpaceMode getMode() const { return csDeviceGray; }

  virtual void getGray(const GfxColor *color, double *gray) const;
  virtual void getRGB(const GfxColor *color, GfxRGB *rgb) const;
  virtual void getCMYK(const GfxColor *color, GfxCMYK *cmyk) const;

  virtual int getNComps() const { return 1; }

private:
};

//------------------------------------------------------------------------
// GfxCalGrayColorSpace
//------------------------------------------------------------------------

class GfxCalGrayColorSpace: public GfxColorSpace {
public:

  GfxCalGrayColorSpace();
  virtual ~GfxCalGrayColorSpace();
  virtual GfxColorSpace *copy() const;
  virtual GfxColorSpaceMode getMode() const { return csCalGray; }

  // Construct a CalGray color space.  Returns NULL if unsuccessful.
  static GfxColorSpace *parse(Array *arr);

  virtual void getGray(const GfxColor *color, double *gray) const;
  virtual void getRGB(const GfxColor *color, GfxRGB *rgb) const;
  virtual void getCMYK(const GfxColor *color, GfxCMYK *cmyk) const;

  virtual int getNComps() const { return 1; }

  // CalGray-specific access.
  double getWhiteX() const { return whiteX; }
  double getWhiteY() const { return whiteY; }
  double getWhiteZ() const { return whiteZ; }
  double getBlackX() const { return blackX; }
  double getBlackY() const { return blackY; }
  double getBlackZ() const { return blackZ; }
  double getGamma() const { return gamma; }

private:

  double whiteX, whiteY, whiteZ;    // white point
  double blackX, blackY, blackZ;    // black point
  double gamma;			    // gamma value
};

//------------------------------------------------------------------------
// GfxDeviceRGBColorSpace
//------------------------------------------------------------------------

class GfxDeviceRGBColorSpace: public GfxColorSpace {
public:

  GfxDeviceRGBColorSpace();
  virtual ~GfxDeviceRGBColorSpace();
  virtual GfxColorSpace *copy() const;
  virtual GfxColorSpaceMode getMode() const { return csDeviceRGB; }

  virtual void getGray(const GfxColor *color, double *gray) const;
  virtual void getRGB(const GfxColor *color, GfxRGB *rgb) const;
  virtual void getCMYK(const GfxColor *color, GfxCMYK *cmyk) const;

  virtual int getNComps() const{ return 3; }

private:
};

//------------------------------------------------------------------------
// GfxCalRGBColorSpace
//------------------------------------------------------------------------

class GfxCalRGBColorSpace: public GfxColorSpace {
public:

  GfxCalRGBColorSpace();
  virtual ~GfxCalRGBColorSpace();
  virtual GfxColorSpace *copy() const;
  virtual GfxColorSpaceMode getMode() const { return csCalRGB; }

  // Construct a CalRGB color space.  Returns NULL if unsuccessful.
  static GfxColorSpace *parse(Array *arr);

  virtual void getGray(const GfxColor *color, double *gray) const;
  virtual void getRGB(const GfxColor *color, GfxRGB *rgb) const;
  virtual void getCMYK(const GfxColor *color, GfxCMYK *cmyk) const;

  virtual int getNComps() const { return 3; }

  // CalRGB-specific access.
  double getWhiteX() const { return whiteX; }
  double getWhiteY() const { return whiteY; }
  double getWhiteZ() const { return whiteZ; }
  double getBlackX() const { return blackX; }
  double getBlackY() const { return blackY; }
  double getBlackZ() const { return blackZ; }
  double getGammaR() const { return gammaR; }
  double getGammaG() const { return gammaG; }
  double getGammaB() const { return gammaB; }
  const double *getMatrix() const { return mat; }

private:

  double whiteX, whiteY, whiteZ;    // white point
  double blackX, blackY, blackZ;    // black point
  double gammaR, gammaG, gammaB;    // gamma values
  double mat[9];		// ABC -> XYZ transform matrix
};

//------------------------------------------------------------------------
// GfxDeviceCMYKColorSpace
//------------------------------------------------------------------------

class GfxDeviceCMYKColorSpace: public GfxColorSpace {
public:

  GfxDeviceCMYKColorSpace();
  virtual ~GfxDeviceCMYKColorSpace();
  virtual GfxColorSpace *copy() const;
  virtual GfxColorSpaceMode getMode() const { return csDeviceCMYK; }

  virtual void getGray(const GfxColor *color, double *gray) const;
  virtual void getRGB(const GfxColor *color, GfxRGB *rgb) const;
  virtual void getCMYK(const GfxColor *color, GfxCMYK *cmyk) const;

  virtual int getNComps() const { return 4; }

private:
};

//------------------------------------------------------------------------
// GfxLabColorSpace
//------------------------------------------------------------------------

class GfxLabColorSpace: public GfxColorSpace {
public:

  GfxLabColorSpace();
  virtual ~GfxLabColorSpace();
  virtual GfxColorSpace *copy() const;
  virtual GfxColorSpaceMode getMode() const { return csLab; }

  // Construct a Lab color space.  Returns NULL if unsuccessful.
  static GfxColorSpace *parse(Array *arr);

  virtual void getGray(const GfxColor *color, double *gray) const;
  virtual void getRGB(const GfxColor *color, GfxRGB *rgb) const;
  virtual void getCMYK(const GfxColor *color, GfxCMYK *cmyk) const;

  virtual int getNComps() const { return 3; }

  virtual void getDefaultRanges(double *decodeLow, double *decodeRange,
				int maxImgPixel) const;

  // Lab-specific access.
  double getWhiteX() const { return whiteX; }
  double getWhiteY() const { return whiteY; }
  double getWhiteZ() const { return whiteZ; }
  double getBlackX() const { return blackX; }
  double getBlackY() const { return blackY; }
  double getBlackZ() const { return blackZ; }
  double getAMin() const { return aMin; }
  double getAMax() const { return aMax; }
  double getBMin() const { return bMin; }
  double getBMax() const { return bMax; }

private:

  double whiteX, whiteY, whiteZ;    // white point
  double blackX, blackY, blackZ;    // black point
  double aMin, aMax, bMin, bMax;    // range for the a and b components
  double kr, kg, kb;		    // gamut mapping mulitpliers
};

//------------------------------------------------------------------------
// GfxICCBasedColorSpace
//------------------------------------------------------------------------

class GfxICCBasedColorSpace: public GfxColorSpace {
public:

  GfxICCBasedColorSpace(int nCompsA, GfxColorSpace *altA,
			const Ref *iccProfileStreamA);
  virtual ~GfxICCBasedColorSpace();
  virtual GfxColorSpace *copy() const;
  virtual GfxColorSpaceMode getMode() const { return csICCBased; }

  // Construct an ICCBased color space.  Returns NULL if unsuccessful.
  static GfxColorSpace *parse(Array *arr);

  virtual void getGray(const GfxColor *color, double *gray) const;
  virtual void getRGB(const GfxColor *color, GfxRGB *rgb) const;
  virtual void getCMYK(const GfxColor *color, GfxCMYK *cmyk) const;

  virtual int getNComps() const { return nComps; }

  virtual void getDefaultRanges(double *decodeLow, double *decodeRange,
				int maxImgPixel) const;

  // ICCBased-specific access.
  const GfxColorSpace *getAlt() const { return alt; }

private:

  int nComps;			// number of color components (1, 3, or 4)
  GfxColorSpace *alt;		// alternate color space
  double rangeMin[4];		// min values for each component
  double rangeMax[4];		// max values for each component
  Ref iccProfileStream;		// the ICC profile
};

//------------------------------------------------------------------------
// GfxIndexedColorSpace
//------------------------------------------------------------------------

class GfxIndexedColorSpace: public GfxColorSpace {
public:

  GfxIndexedColorSpace(GfxColorSpace *baseA, int indexHighA);
  virtual ~GfxIndexedColorSpace();
  virtual GfxColorSpace *copy() const;
  virtual GfxColorSpaceMode getMode() const { return csIndexed; }

  // Construct a Lab color space.  Returns NULL if unsuccessful.
  static GfxColorSpace *parse(Array *arr);

  virtual void getGray(const GfxColor *color, double *gray) const;
  virtual void getRGB(const GfxColor *color, GfxRGB *rgb) const;
  virtual void getCMYK(const GfxColor *color, GfxCMYK *cmyk) const;

  virtual int getNComps() const { return 1; }

  virtual void getDefaultRanges(double *decodeLow, double *decodeRange,
				int maxImgPixel) const;

  // Indexed-specific access.
  const GfxColorSpace *getBase() const { return base; }
  int getIndexHigh() const { return indexHigh; }
  const Guchar *getLookup() const { return lookup; }

private:

  GfxColorSpace *base;		// base color space
  int indexHigh;		// max pixel value
  Guchar *lookup;		// lookup table
};

//------------------------------------------------------------------------
// GfxSeparationColorSpace
//------------------------------------------------------------------------

class GfxSeparationColorSpace: public GfxColorSpace {
public:

  GfxSeparationColorSpace(GString *nameA, GfxColorSpace *altA,
			  Function *funcA);
  virtual ~GfxSeparationColorSpace();
  virtual GfxColorSpace *copy() const;
  virtual GfxColorSpaceMode getMode() const { return csSeparation; }

  // Construct a Separation color space.  Returns NULL if unsuccessful.
  static GfxColorSpace *parse(Array *arr);

  virtual void getGray(const GfxColor *color, double *gray) const;
  virtual void getRGB(const GfxColor *color, GfxRGB *rgb) const;
  virtual void getCMYK(const GfxColor *color, GfxCMYK *cmyk) const;

  virtual int getNComps() const { return 1; }

  // Separation-specific access.
  const GString *getName() const { return name; }
  const GfxColorSpace *getAlt() const { return alt; }
  const Function *getFunc() const { return func; }

private:

  GString *name;		// colorant name
  GfxColorSpace *alt;		// alternate color space
  Function *func;		// tint transform (into alternate color space)
};

//------------------------------------------------------------------------
// GfxDeviceNColorSpace
//------------------------------------------------------------------------

class GfxDeviceNColorSpace: public GfxColorSpace {
public:

  GfxDeviceNColorSpace(int nComps, GfxColorSpace *alt, Function *func);
  virtual ~GfxDeviceNColorSpace();
  virtual GfxColorSpace *copy() const;
  virtual GfxColorSpaceMode getMode() const { return csDeviceN; }

  // Construct a DeviceN color space.  Returns NULL if unsuccessful.
  static GfxColorSpace *parse(Array *arr);

  virtual void getGray(const GfxColor *color, double *gray) const;
  virtual void getRGB(const GfxColor *color, GfxRGB *rgb) const;
  virtual void getCMYK(const GfxColor *color, GfxCMYK *cmyk) const;

  virtual int getNComps() const { return nComps; }

  // DeviceN-specific access.
  const GString *getColorantName(int i) const { return names[i]; }
  const GfxColorSpace *getAlt() const { return alt; }
  const Function *getTintTransformFunc() const { return func; }

private:

  int nComps;			// number of components
  GString			// colorant names
    *names[gfxColorMaxComps];
  GfxColorSpace *alt;		// alternate color space
  Function *func;		// tint transform (into alternate color space)

};

//------------------------------------------------------------------------
// GfxPatternColorSpace
//------------------------------------------------------------------------

class GfxPatternColorSpace: public GfxColorSpace {
public:

  GfxPatternColorSpace(GfxColorSpace *underA);
  virtual ~GfxPatternColorSpace();
  virtual GfxColorSpace *copy() const;
  virtual GfxColorSpaceMode getMode() const { return csPattern; }

  // Construct a Pattern color space.  Returns NULL if unsuccessful.
  static GfxColorSpace *parse(Array *arr);

  virtual void getGray(const GfxColor *color, double *gray) const;
  virtual void getRGB(const GfxColor *color, GfxRGB *rgb) const;
  virtual void getCMYK(const GfxColor *color, GfxCMYK *cmyk) const;

  virtual int getNComps() const { return 0; }

  // Pattern-specific access.
  const GfxColorSpace *getUnder() const { return under; }

private:

  GfxColorSpace *under;		// underlying color space (for uncolored
				//   patterns)
};

//------------------------------------------------------------------------
// GfxPattern
//------------------------------------------------------------------------

class GfxPattern {
public:

  GfxPattern(int typeA);
  virtual ~GfxPattern();

  static GfxPattern *parse(Object *obj);

  virtual GfxPattern *copy() = 0;

  int getType() { return type; }

private:

  int type;
};

//------------------------------------------------------------------------
// GfxTilingPattern
//------------------------------------------------------------------------

class GfxTilingPattern: public GfxPattern {
public:

  GfxTilingPattern(Dict *streamDict, Object *stream);
  virtual ~GfxTilingPattern();

  virtual GfxPattern *copy();

  int getPaintType() { return paintType; }
  int getTilingType() { return tilingType; }
  double *getBBox() { return bbox; }
  double getXStep() { return xStep; }
  double getYStep() { return yStep; }
  Dict *getResDict()
    { return resDict.isDict() ? resDict.getDict() : (Dict *)NULL; }
  double *getMatrix() { return matrix; }
  Object *getContentStream() { return &contentStream; }

private:

  GfxTilingPattern(GfxTilingPattern *pat);

  int paintType;
  int tilingType;
  double bbox[4];
  double xStep, yStep;
  Object resDict;
  double matrix[6];
  Object contentStream;
};

//------------------------------------------------------------------------
// GfxShading
//------------------------------------------------------------------------

class GfxShading {
public:

  GfxShading();
  virtual ~GfxShading();

  static GfxShading *parse(Object *obj);

  int getType() { return type; }
  GfxColorSpace *getColorSpace() { return colorSpace; }
  GfxColor *getBackground() { return &background; }
  GBool getHasBackground() { return hasBackground; }
  void getBBox(double *xMinA, double *yMinA, double *xMaxA, double *yMaxA)
    { *xMinA = xMin; *yMinA = yMin; *xMaxA = xMax; *yMaxA = yMax; }
  GBool getHasBBox() { return hasBBox; }

private:

  int type;
  GfxColorSpace *colorSpace;
  GfxColor background;
  GBool hasBackground;
  double xMin, yMin, xMax, yMax;
  GBool hasBBox;
};

//------------------------------------------------------------------------
// GfxAxialShading
//------------------------------------------------------------------------

class GfxAxialShading: public GfxShading {
public:

  GfxAxialShading(double x0A, double y0A,
		  double x1A, double y1A,
		  double t0A, double t1A,
		  Function **funcsA, int nFuncsA,
		  GBool extend0A, GBool extend1A);
  virtual ~GfxAxialShading();

  static GfxAxialShading *parse(Dict *dict);

  void getCoords(double *x0A, double *y0A, double *x1A, double *y1A)
    { *x0A = x0; *y0A = y0; *x1A = x1; *y1A = y1; }
  double getDomain0() { return t0; }
  double getDomain1() { return t1; }
  void getColor(double t, GfxColor *color);
  GBool getExtend0() { return extend0; }
  GBool getExtend1() { return extend1; }

private:

  double x0, y0, x1, y1;
  double t0, t1;
  Function *funcs[gfxColorMaxComps];
  int nFuncs;
  GBool extend0, extend1;
};

//------------------------------------------------------------------------
// GfxRadialShading
//------------------------------------------------------------------------

class GfxRadialShading: public GfxShading {
public:

  GfxRadialShading(double x0A, double y0A, double r0A,
		   double x1A, double y1A, double r1A,
		   double t0A, double t1A,
		   Function **funcsA, int nFuncsA,
		   GBool extend0A, GBool extend1A);
  virtual ~GfxRadialShading();

  static GfxRadialShading *parse(Dict *dict);

  void getCoords(double *x0A, double *y0A, double *r0A,
		 double *x1A, double *y1A, double *r1A)
    { *x0A = x0; *y0A = y0; *r0A = r0; *x1A = x1; *y1A = y1; *r1A = r1; }
  double getDomain0() { return t0; }
  double getDomain1() { return t1; }
  void getColor(double t, GfxColor *color);
  GBool getExtend0() { return extend0; }
  GBool getExtend1() { return extend1; }

private:

  double x0, y0, r0, x1, y1, r1;
  double t0, t1;
  Function *funcs[gfxColorMaxComps];
  int nFuncs;
  GBool extend0, extend1;
};

//------------------------------------------------------------------------
// GfxImageColorMap
//------------------------------------------------------------------------

class GfxImageColorMap {
public:

  // Constructor.
  GfxImageColorMap(int bitsA, Object *decode, GfxColorSpace *colorSpaceA);

  // Destructor.
  ~GfxImageColorMap();

  // Is color map valid?
  GBool isOk() { return ok; }

  // Get the color space.
  GfxColorSpace *getColorSpace() { return colorSpace; }

  // Get stream decoding info.
  int getNumPixelComps() { return nComps; }
  int getBits() { return bits; }

  // Get decode table.
  double getDecodeLow(int i) { return decodeLow[i]; }
  double getDecodeHigh(int i) { return decodeLow[i] + decodeRange[i]; }

  // Convert an image pixel to a color.
  void getGray(Guchar *x, double *gray);
  void getRGB(Guchar *x, GfxRGB *rgb);
  void getCMYK(Guchar *x, GfxCMYK *cmyk);

private:

  GfxColorSpace *colorSpace;	// the image color space
  int bits;			// bits per component
  int nComps;			// number of components in a pixel
  const GfxColorSpace *colorSpace2;	// secondary color space
  int nComps2;			// number of components in colorSpace2
  double *lookup;		// lookup table
  double			// minimum values for each component
    decodeLow[gfxColorMaxComps];
  double			// max - min value for each component
    decodeRange[gfxColorMaxComps];
  GBool ok;
};

//------------------------------------------------------------------------
// GfxSubpath and GfxPath
//------------------------------------------------------------------------

class GfxSubpath {
public:

  // Constructor.
  GfxSubpath(double x1, double y1);

  // Destructor.
  ~GfxSubpath();

  // Copy.
  GfxSubpath *copy() { return new GfxSubpath(this); }

  // Get points.
  int getNumPoints() { return n; }
  double getX(int i) { return x[i]; }
  double getY(int i) { return y[i]; }
  GBool getCurve(int i) { return curve[i]; }

  // Get last point.
  double getLastX() { return x[n-1]; }
  double getLastY() { return y[n-1]; }

  // Add a line segment.
  void lineTo(double x1, double y1);

  // Add a Bezier curve.
  void curveTo(double x1, double y1, double x2, double y2,
	       double x3, double y3);

  // Close the subpath.
  void close();
  GBool isClosed() { return closed; }

private:

  double *x, *y;		// points
  GBool *curve;			// curve[i] => point i is a control point
				//   for a Bezier curve
  int n;			// number of points
  int size;			// size of x/y arrays
  GBool closed;			// set if path is closed

  GfxSubpath(GfxSubpath *subpath);
};

class GfxPath {
public:

  // Constructor.
  GfxPath();

  // Destructor.
  ~GfxPath();

  // Copy.
  GfxPath *copy()
    { return new GfxPath(justMoved, firstX, firstY, subpaths, n, size); }

  // Is there a current point?
  GBool isCurPt() { return n > 0 || justMoved; }

  // Is the path non-empty, i.e., is there at least one segment?
  GBool isPath() { return n > 0; }

  // Get subpaths.
  int getNumSubpaths() { return n; }
  GfxSubpath *getSubpath(int i) { return subpaths[i]; }

  // Get last point on last subpath.
  double getLastX() { return subpaths[n-1]->getLastX(); }
  double getLastY() { return subpaths[n-1]->getLastY(); }

  // Move the current point.
  void moveTo(double x, double y);

  // Add a segment to the last subpath.
  void lineTo(double x, double y);

  // Add a Bezier curve to the last subpath
  void curveTo(double x1, double y1, double x2, double y2,
	       double x3, double y3);

  // Close the last subpath.
  void close();

private:

  GBool justMoved;		// set if a new subpath was just started
  double firstX, firstY;	// first point in new subpath
  GfxSubpath **subpaths;	// subpaths
  int n;			// number of subpaths
  int size;			// size of subpaths array

  GfxPath(GBool justMoved1, double firstX1, double firstY1,
	  GfxSubpath **subpaths1, int n1, int size1);
};

//------------------------------------------------------------------------
// GfxState
//------------------------------------------------------------------------

class GfxState {
public:

  // Construct a default GfxState, for a device with resolution <dpi>,
  // page box <pageBox>, page rotation <rotate>, and coordinate system
  // specified by <upsideDown>.
  GfxState(double dpi, PDFRectangle *pageBox, int rotate,
	   GBool upsideDown);

  // Destructor.
  ~GfxState();

  // Copy.
  GfxState *copy() const { return new GfxState(this); }

  // Accessors.
  const double *getCTM() const { return ctm; }
  double getX1() const { return px1; }
  double getY1() const { return py1; }
  double getX2() const { return px2; }
  double getY2() const { return py2; }
  double getPageWidth() const { return pageWidth; }
  double getPageHeight() const { return pageHeight; }
  const GfxColor *getFillColor() const { return &fillColor; }
  const GfxColor *getStrokeColor() const { return &strokeColor; }
  void getFillGray(double *gray) const
    { fillColorSpace->getGray(&fillColor, gray); }
  void getStrokeGray(double *gray) const
    { strokeColorSpace->getGray(&fillColor, gray); }
  void getFillRGB(GfxRGB *rgb) const
    { fillColorSpace->getRGB(&fillColor, rgb); }
  void getStrokeRGB(GfxRGB *rgb) const
    { strokeColorSpace->getRGB(&strokeColor, rgb); }
  void getFillCMYK(GfxCMYK *cmyk) const
    { fillColorSpace->getCMYK(&fillColor, cmyk); }
  void getStrokeCMYK(GfxCMYK *cmyk) const
    { strokeColorSpace->getCMYK(&strokeColor, cmyk); }
  GfxColorSpace *getFillColorSpace() const { return fillColorSpace; }
  GfxColorSpace *getStrokeColorSpace() const { return strokeColorSpace; }
  GfxPattern *getFillPattern() const { return fillPattern; }
  GfxPattern *getStrokePattern() const { return strokePattern; }
  double getFillOpacity() const { return fillOpacity; }
  double getStrokeOpacity() const { return strokeOpacity; }
  double getLineWidth() const { return lineWidth; }
  void getLineDash(double **dash, int *length, double *start) const
    { *dash = lineDash; *length = lineDashLength; *start = lineDashStart; }
  int getFlatness() const { return flatness; }
  int getLineJoin() const { return lineJoin; }
  int getLineCap() const { return lineCap; }
  double getMiterLimit() const { return miterLimit; }
  GfxFont *getFont() const { return font; }
  double getFontSize() const { return fontSize; }
  const double *getTextMat() const { return textMat; }
  double getCharSpace() const { return charSpace; }
  double getWordSpace() const { return wordSpace; }
  double getHorizScaling() const { return horizScaling; }
  double getLeading() const { return leading; }
  double getRise() const { return rise; }
  int getRender() const { return render; }
  GfxPath *getPath() const { return path; }
  double getCurX() const { return curX; }
  double getCurY() const { return curY; }
  void getClipBBox(double *xMin, double *yMin, double *xMax, double *yMax) const
    { *xMin = clipXMin; *yMin = clipYMin; *xMax = clipXMax; *yMax = clipYMax; }
  void getUserClipBBox(double *xMin, double *yMin, double *xMax, double *yMax) const;
  double getLineX() const { return lineX; }
  double getLineY() const { return lineY; }

  // Is there a current point/path?
  GBool isCurPt() const { return path->isCurPt(); }
  GBool isPath() const { return path->isPath(); }

  // Transforms.
  void transform(double x1, double y1, double *x2, double *y2)
    { *x2 = ctm[0] * x1 + ctm[2] * y1 + ctm[4];
      *y2 = ctm[1] * x1 + ctm[3] * y1 + ctm[5]; }
  void transformDelta(double x1, double y1, double *x2, double *y2)
    { *x2 = ctm[0] * x1 + ctm[2] * y1;
      *y2 = ctm[1] * x1 + ctm[3] * y1; }
  void textTransform(double x1, double y1, double *x2, double *y2)
    { *x2 = textMat[0] * x1 + textMat[2] * y1 + textMat[4];
      *y2 = textMat[1] * x1 + textMat[3] * y1 + textMat[5]; }
  void textTransformDelta(double x1, double y1, double *x2, double *y2)
    { *x2 = textMat[0] * x1 + textMat[2] * y1;
      *y2 = textMat[1] * x1 + textMat[3] * y1; }
  double transformWidth(double w);
  double getTransformedLineWidth()
    { return transformWidth(lineWidth); }
  double getTransformedFontSize();
  void getFontTransMat(double *m11, double *m12, double *m21, double *m22);

  // Change state parameters.
  void setCTM(double a, double b, double c,
	      double d, double e, double f);
  void concatCTM(double a, double b, double c,
		 double d, double e, double f);
  void setFillColorSpace(GfxColorSpace *colorSpace);
  void setStrokeColorSpace(GfxColorSpace *colorSpace);
  void setFillColor(GfxColor *color) { fillColor = *color; }
  void setStrokeColor(GfxColor *color) { strokeColor = *color; }
  void setFillPattern(GfxPattern *pattern);
  void setStrokePattern(GfxPattern *pattern);
  void setFillOpacity(double opac) { fillOpacity = opac; }
  void setStrokeOpacity(double opac) { strokeOpacity = opac; }
  void setLineWidth(double width) { lineWidth = width; }
  void setLineDash(double *dash, int length, double start);
  void setFlatness(int flatness1) { flatness = flatness1; }
  void setLineJoin(int lineJoin1) { lineJoin = lineJoin1; }
  void setLineCap(int lineCap1) { lineCap = lineCap1; }
  void setMiterLimit(double limit) { miterLimit = limit; }
  void setFont(GfxFont *fontA, double fontSizeA)
    { font = fontA; fontSize = fontSizeA; }
  void setTextMat(double a, double b, double c,
		  double d, double e, double f)
    { textMat[0] = a; textMat[1] = b; textMat[2] = c;
      textMat[3] = d; textMat[4] = e; textMat[5] = f; }
  void setCharSpace(double space)
    { charSpace = space; }
  void setWordSpace(double space)
    { wordSpace = space; }
  void setHorizScaling(double scale)
    { horizScaling = 0.01 * scale; }
  void setLeading(double leadingA)
    { leading = leadingA; }
  void setRise(double riseA)
    { rise = riseA; }
  void setRender(int renderA)
    { render = renderA; }

  // Add to path.
  void moveTo(double x, double y)
    { path->moveTo(curX = x, curY = y); }
  void lineTo(double x, double y)
    { path->lineTo(curX = x, curY = y); }
  void curveTo(double x1, double y1, double x2, double y2,
	       double x3, double y3)
    { path->curveTo(x1, y1, x2, y2, curX = x3, curY = y3); }
  void closePath()
    { path->close(); curX = path->getLastX(); curY = path->getLastY(); }
  void clearPath();

  // Update clip region.
  void clip();

  // Text position.
  void textMoveTo(double tx, double ty)
    { lineX = tx; lineY = ty; textTransform(tx, ty, &curX, &curY); }
  void textShift(double tx, double ty);
  void shift(double dx, double dy);

  // Push/pop GfxState on/off stack.
  GfxState *save();
  GfxState *restore();
  GBool hasSaves() { return saved != NULL; }

private:

  double ctm[6];		// coord transform matrix
  double px1, py1, px2, py2;	// page corners (user coords)
  double pageWidth, pageHeight;	// page size (pixels)

  GfxColorSpace *fillColorSpace;   // fill color space
  GfxColorSpace *strokeColorSpace; // stroke color space
  GfxColor fillColor;		// fill color
  GfxColor strokeColor;		// stroke color
  GfxPattern *fillPattern;	// fill pattern
  GfxPattern *strokePattern;	// stroke pattern
  double fillOpacity;		// fill opacity
  double strokeOpacity;		// stroke opacity

  double lineWidth;		// line width
  double *lineDash;		// line dash
  int lineDashLength;
  double lineDashStart;
  int flatness;			// curve flatness
  int lineJoin;			// line join style
  int lineCap;			// line cap style
  double miterLimit;		// line miter limit

  GfxFont *font;		// font
  double fontSize;		// font size
  double textMat[6];		// text matrix
  double charSpace;		// character spacing
  double wordSpace;		// word spacing
  double horizScaling;		// horizontal scaling
  double leading;		// text leading
  double rise;			// text rise
  int render;			// text rendering mode

  GfxPath *path;		// array of path elements
  double curX, curY;		// current point (user coords)
  double lineX, lineY;		// start of current text line (text coords)

  double clipXMin, clipYMin,	// bounding box for clip region
         clipXMax, clipYMax;

  GfxState *saved;		// next GfxState on stack

  GfxState(const GfxState *state);
};

#endif
