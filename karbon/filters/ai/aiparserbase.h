/***************************************************************************
                          aiparserbase.h  -  description
                             -------------------
    begin                : Thu Jul 19 2001
    copyright            : (C) 2001 by Dirk Schönberger
    email                : schoenberger@signsoft.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef AIPARSERBASE_H
#define AIPARSERBASE_H

#include "ailexer.h"

#include <qstring.h>
#include <qvaluestack.h>
#include <qvaluevector.h>

#include "aielement.h"
#include "aicolor.h"

const void arraytoa (const QValueVector<AIElement> &data);
const void elementtoa (const AIElement &data);
const void stacktoa (const QValueStack<AIElement> &data);
const void stacktoa2 (const QValueStack<QValueVector<AIElement> >&data);

/**
  *@author Dirk Schönberger
  */

typedef enum { PET_MoveTo, PET_LineTo, PET_CurveTo, PET_CurveToOmitC1, PET_CurveToOmitC2 } PathElementType;
typedef enum { PT_Smooth, PT_Corner } PointType;
typedef enum { TO_FillStroke, TO_Fill, TO_Append, TO_Ignore, TO_Stroke } TextOperation;
typedef enum { FM_NonZero } FillMode;
typedef enum { FS_Roman, FS_Kanji } FontScript;

typedef enum { DS_Array, DS_Other } DataSink;

typedef enum {
  /* AI 88 */
  AIO_SetFillColorCMYK, AIO_SetStrokeColorCMYK,
  AIO_SetFillColorGray, AIO_SetStrokeColorGray,
  AIO_SetFillColorCustom, AIO_SetStrokeColorCustom,
  AIO_SetFillPattern, AIO_SetStrokePattern,
  AIO_SetFillOverprinting, AIO_SetStrokeOverprinting,
  AIO_SetFlatness, AIO_SetLineCap, AIO_SetLineJoin,
  AIO_SetLineWidth, AIO_SetMiterLimit, AIO_SetDash,
  AIO_BeginGroupClip, AIO_EndGroupClip,
  AIO_MoveTo,
  AIO_LineToCorner,
  AIO_LineToSmooth,
  AIO_CurveToSmooth,
  AIO_CurveToCorner,
  AIO_CurveToOmitC1Smooth,
  AIO_CurveToOmitC1Corner,
  AIO_CurveToOmitC2Smooth,
  AIO_CurveToOmitC2Corner,

  AIO_PathIgnoreNoReset, AIO_PathIgnoreNoResetClose,
  AIO_PathClipPath,
  AIO_PathIgnoreReset, AIO_PathIgnoreResetClose,
  AIO_PathFillNonZero, AIO_PathFillNonZeroClose,
  AIO_PathStroke, AIO_PathStrokeClose,
  AIO_PathFillNoReset, AIO_PathFillNoResetClose,

/**/ AIO_FontEncoding,
  AIO_PatternDefinition,

/**/ AIO_SetCurrentText,
/**/ AIO_TextBlockFillStroke,
/**/ AIO_TextBlockFill,
/**/ AIO_TextBlockAppend,
/**/ AIO_TextBlockIgnore,
/**/ AIO_TextBlockStroke,
/**/ AIO_TextOutput,
/**/ AIO_TextBlockEnd,

/**/ AIO_GsaveIncludeDocument,
/**/ AIO_Grestore,

  AIO_LockElement,

  /* AI 3 */
  AIO_SetWindingOrder,

  AIO_Other
} AIOperation;

typedef enum {
  PSO_Get,
  PSO_Exec,
  PSO_Other
} PSOperation;

typedef enum {
  CU_BlackWhite,
  CU_Color,
  CU_Unknown
} ColorUsage;

typedef enum {
  CO_BeginSetup, CO_EndSetup,
  CO_BeginProlog, CO_EndProlog,
  CO_BeginProcSet, CO_EndProcSet,
  CO_BeginEncoding, CO_EndEncoding,
  CO_BeginPattern, CO_EndPattern,
/**/  CO_IncludeFile,
  CO_BeginDocument, CO_EndDocument,

  CO_Trailer,
  CO_BoundingBox,
  CO_TemplateBox,
  CO_Margin,
  CO_Title,
  CO_Creator,
  CO_Other,
/**/  CO_For,
/**/  CO_CreationDate,
/**/  CO_DocumentFonts,
/**/  CO_DocumentFiles,
/**/  CO_ColorUsage,
/**/  CO_DocumentProcSets,
/**/  CO_DocumentSuppliedProcSets,
/**/  CO_DocumentProcessColors,
/**/  CO_DocumentCustomColors,
/**/  CO_CMYKCustomColor,
/**/  CO_TileBox,
  CO_Continuation,
/**/  CO_Note,

  // AI88 Win
/**/  CO_Template,
/**/  CO_PageOrigin,
/**/  CO_PrinterName,
/**/  CO_PrinterRect,

  // AI8
  CO_BeginBrushPattern, CO_EndBrushPattern,
  CO_BeginGradient, CO_EndGradient,
  CO_BeginPalette, CO_EndPalette,

  // other
  CO_BeginResource, CO_EndResource,
  CO_IncludeFont,

  CO_Ignore
} CommentOperation;

typedef enum {
  CM_DocumentFonts,
  CM_DocumentFiles,
  CM_DocumentCustomColors,
  CM_CMYKCustomColor,
  CM_None
} ContinuationMode;

typedef enum {
/**/  PDO_ColorDefinition,
/**/  PDO_TileDefinition
} PatternDefinitionOperation;

/**/
#define PatternTileFilledDefiniton "_"
#define TextSizeUnknown -1

/* typedef QValueVector<AElement> ElementArray;
typedef QValueStack<AIElement> ElementStack;
typedef QValueStack<ElementArray> ElementArrayStack; */

typedef struct {
  PathElementType petype;
  union {
    struct {
      float x, y;
    } pointdata;
    struct {
      float x1, y1, x2, y2, x3, y3;
    } bezierdata;
  } pevalue;
  PointType pttype;
} PathElement;

class AIParserBase : protected AILexer  {
private:
  bool m_ignoring;
  bool m_debug;
  QValueStack<AIElement> m_stack;
  QValueStack<QValueVector<AIElement> > m_arrayStack;
  DataSink m_sink;
  ContinuationMode m_continuationMode;

  const double getDoubleValue(void);
  const int getIntValue(void);
  const bool getBoolValue(void);
  const QString& getStringValue(void);
  const QString& getReferenceValue(void);
  const QString& getOperatorValue(void);

  void _handleSetDash();
  void _handleSetStrokeColorCMYK();
  void _handleSetFillColorCMYK();
  void _handleSetStrokeColorGray();
  void _handleSetFillColorGray();
  void _handleSetStrokeColorCustom();
  void _handleSetFillColorCustom();
  void _handleSetFillPattern();
  void _handleSetStrokePattern();

  void _handlePatternDefinition();

  void _handlePSGet();
  void _handlePSExec();

  void _handleDocumentFonts(const char *data);
  void _handleDocumentFiles(const char *data);
  void _handleDocumentCustomColors(const char *data);
  void _handleCMYKCustomColor(const char *data);

  AIOperation getAIOperation (const char *operand);
  PSOperation getPSOperation (const char *operand);
  CommentOperation getCommentOperation (const char *command);

  bool handlePS (const char *operand);

  const char *getValue (const char *input);
  bool getRectangle (const char* input, int &llx, int &lly, int &urx, int &ury);
  bool getPoint (const char* input, int &x, int &y);
protected:
  void gotComment (const char *value);
  void gotIntValue (int value);
  void gotDoubleValue (double value);
  void gotStringValue (const char *value);
  void gotToken (const char *value);
  void gotReference (const char *value);
  void gotBlockStart ();
  void gotBlockEnd ();
  void gotArrayStart ();
  void gotArrayEnd ();
  void gotByte (uchar value);
  void gotByteArray (const QByteArray &data);

  virtual void gotFillColor (AIColor &color);
  virtual void gotStrokeColor (AIColor &color);
  virtual void gotFillPattern (const char *pname, double px, double py, double sx, double sy, double angle, double rf, double r, double k, double ka, const QValueVector<AIElement>& transformData);
  virtual void gotStrokePattern (const char *pname, double px, double py, double sx, double sy, double angle, double rf, double r, double k, double ka, const QValueVector<AIElement>& transformData);
  virtual void gotFlatness (double val);
  virtual void gotLineWidth (double val);
  virtual void gotLineCaps (int val);
  virtual void gotLineJoin (int val);
  virtual void gotMiterLimit (double val);
  virtual void gotWindingOrder (int val);
  virtual void gotDash (const QValueVector<AIElement>& dashData, double phase);
  virtual void gotBeginGroup (bool clipping);
  virtual void gotEndGroup (bool clipping);
  virtual void gotPathElement (PathElement &element);
  virtual void gotFillPath (bool closed, bool reset, FillMode fm = FM_NonZero);
  virtual void gotStrokePath (bool closed);
  virtual void gotIgnorePath (bool closed, bool reset);
  virtual void gotClipPath (bool closed);
  virtual void gotLockNextObject (bool value);
  virtual void gotFillOverprinting (bool value);
  virtual void gotStrokeOverprinting (bool value);
  virtual void gotBoundingBox (int llx, int lly, int urx, int ury);
  virtual void gotTemplateBox (int llx, int lly, int urx, int ury);
  virtual void gotMargin (int llx, int lly, int urx, int ury);
  virtual void gotPrinterRect (int llx, int lly, int urx, int ury);
  virtual void gotPrinterName (const char *data);
  virtual void gotPageOrigin (int x, int y);
  virtual void gotTemplate (const char *data);
  virtual void gotTitle (const char *data);
  virtual void gotCreator (const char *data);
  virtual void gotPatternDefinition (const char *name, const QValueVector<AIElement>& layerData, double llx, double lly, double urx, double ury);

private:
  void handleElement (AIElement &element);
public:
	AIParserBase();
	~AIParserBase();

  bool parse (QIODevice& fin);
};

const void aiotoa (AIOperation &data);

#endif

