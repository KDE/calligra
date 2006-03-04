/* This file is part of the KDE project
   Copyright (C) 2002, Dirk Schönberger <dirk.schoenberger@sz-online.de>

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

#ifndef AIPARSERBASE_H
#define AIPARSERBASE_H

#include "ailexer.h"

#include <qstring.h>
#include <qvaluestack.h>
#include <qvaluevector.h>
#include <qvaluelist.h>

#include "aielement.h"
#include "aicolor.h"


const void arraytoa (const QValueVector<AIElement> &data);
const void elementtoa (const AIElement &data);
const void stacktoa (const QValueStack<AIElement> &data);
const void stacktoa2 (const QValueStack<QValueVector<AIElement> >&data);

class GStateHandlerBase;
class StructureHandlerBase;
class PathHandlerBase;
class MiscGStateHandlerBase;
class DocumentHandlerBase;
class ModuleHandlerBase;
class EmbeddedHandlerBase;
class TextHandlerBase;
class AI88Handler;
class AI3Handler;

/**
  *@author Dirk Schönberger
  */

typedef enum { PET_MoveTo, PET_LineTo, PET_CurveTo, PET_CurveToOmitC1, PET_CurveToOmitC2 } PathElementType;
typedef enum { PT_Smooth, PT_Corner } PointType;
typedef enum { TO_FillStroke, TO_Fill, TO_Append, TO_Ignore, TO_Stroke } TextOperation;
typedef enum { FM_NonZero=0, FM_EvenOdd=1 } FillMode;
typedef enum { FS_Roman, FS_Kanji } FontScript;

typedef enum { DS_Array, DS_Block, DS_Other } DataSink;
typedef enum { ST_Setup, ST_Prolog, ST_ProcSet, ST_Encoding, ST_Pattern, ST_Document, ST_BrushPattern, ST_Gradient, ST_Palette, ST_Resource } SectionType;

typedef enum { TA_HLeft, TA_HCenter, TA_HRight, TA_VTop, TA_VCenter, TA_VBottom } TextAlign;

#define PC_Cyan    0x0001
#define PC_Magenta 0x0002
#define PC_Yellow  0x0004
#define PC_Black   0x0008

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

  AIO_FontEncoding,
  AIO_PatternDefinition,

  AIO_SetCurrentText,
  AIO_TextBlockFillStroke,
  AIO_TextBlockFill,
  AIO_TextBlockAppend,
  AIO_TextBlockIgnore,
  AIO_TextBlockStroke,
  AIO_TextOutput,
  AIO_TextBlockEnd,

  AIO_GsaveIncludeDocument,
  AIO_Grestore,

  AIO_LockElement,

  /* AI 3 */
  AIO_SetWindingOrder,
  AIO_SetFillMode,

  AIO_BeginGroupNoClip, AIO_EndGroupNoClip,
  AIO_BeginCombination, AIO_EndCombination,

  AIO_Other
} AIOperation;

typedef enum {
  PSO_Get,
  PSO_Exec,
  PSO_Def,
  PSO_String,
  PSO_Bind,
  PSO_Userdict,
  PSO_Dict,
  PSO_Dup,
  PSO_Begin,
  PSO_Put,
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
  CO_IncludeFile,
  CO_BeginDocument, CO_EndDocument,

  CO_Trailer,
  CO_BoundingBox,
  CO_TemplateBox,
  CO_Margin,
  CO_Title,
  CO_Creator,
  CO_Other,
/**/  CO_For,
  CO_CreationDate,
/**/  CO_DocumentFonts,
/**/  CO_DocumentFiles,
/**/  CO_ColorUsage,
/**/  CO_DocumentProcSets,
/**/  CO_DocumentSuppliedProcSets,
  CO_DocumentProcessColors,
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
  CO_IncludeResource,
  CO_DocumentNeededResources,

  CO_Ignore
} CommentOperation;

typedef enum {
  CM_DocumentFonts = 1,
  CM_DocumentFiles = 2,
  CM_DocumentCustomColors = 3,
  CM_CMYKCustomColor = 4,
  CM_DocumentNeededResources = 5,
  CM_None = -1
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
  friend class AI88Handler;
  friend class AI3Handler;

protected:
  bool m_debug;
private:
  bool m_ignoring;
  QValueStack<AIElement> m_stack;
  QValueStack<QValueVector<AIElement> > m_arrayStack;
  QValueStack<QValueVector<AIElement> > m_blockStack;
  DataSink m_sink;
  QValueList<QString> m_modules;
  ContinuationMode m_continuationMode;

  const double getDoubleValue(void);
  const int getIntValue(void);
  const bool getBoolValue(void);
  const QString getStringValue(void);
  const QString& getReferenceValue(void);
  const QString getOperatorValue(void);

/*Ai88*/  // void _handleSetDash();
/*Ai88*/  // void _handleGsaveIncludeDocument();
/*Ai88*/  // void _handleSetStrokeColorCMYK();
/*Ai88*/  // void _handleSetFillColorCMYK();
/*Ai88*/  // void _handleSetStrokeColorGray();
/*Ai88*/  // void _handleSetFillColorGray();
/*Ai88*/  // void _handleSetStrokeColorCustom();
/*Ai88*/  // void _handleSetFillColorCustom();
/*Ai88*/  // void _handleSetFillPattern();
/*Ai88*/  // void _handleSetStrokePattern();

/*Ai88*/  // void _handlePatternDefinition();
/*Ai88*/  // void _handleFontEncoding();

  void _handlePSGet();
  void _handlePSExec();
  void _handlePSDef();
  void _handlePSString();
  void _handlePSBind();
  void _handlePSUserdict();
  void _handlePSDict();
  void _handlePSDup();
  void _handlePSBegin();
  void _handlePSPut();

  void _handleDocumentFonts(const char *data);
  void _handleDocumentFiles(const char *data);
  void _handleDocumentCustomColors(const char *data);
  void _handleCMYKCustomColor(const char *data);
  void _handleDocumentProcessColors(const char *data);
  void _handleDocumentNeededResources(const char *data);
  void _handleIncludeResource(const char *data);

/*Ai88*/  // void _handleSetCurrentText();
/*Ai88*/  // void _handleTextBlock (TextOperation to);
/*Ai88*/  // void _handleTextOutput ();

  void _handleCreationDate (const char *data);

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

  GStateHandlerBase *m_gstateHandler;
  StructureHandlerBase *m_structureHandler;
  PathHandlerBase *m_pathHandler;
  MiscGStateHandlerBase *m_miscGStateHandler;
  DocumentHandlerBase *m_documentHandler;
  ModuleHandlerBase *m_moduleHandler;
  EmbeddedHandlerBase *m_embeddedHandler;
  TextHandlerBase *m_textHandler;
  AI88Handler *m_ai88Handler;
  AI3Handler *m_ai3Handler;

private:
  void handleElement (AIElement &element);
  void cleanupArrays();
public:
	AIParserBase();
	~AIParserBase();

  bool parse (QIODevice& fin);
};

const void aiotoa (AIOperation &data);
const void sttoa (SectionType &data, bool begin);

class GStateHandlerBase
{
  public:
   	 GStateHandlerBase() {}
	  virtual ~GStateHandlerBase() {}

    virtual void gotFillColor (AIColor &) {}
    virtual void gotStrokeColor (AIColor &) {}
    virtual void gotFillPattern (const char *pname, double px, double py, double sx, double sy, double angle, double rf, double r, double k, double ka, const QValueVector<AIElement>& transformData);
    virtual void gotStrokePattern (const char *pname, double px, double py, double sx, double sy, double angle, double rf, double r, double k, double ka, const QValueVector<AIElement>& transformData);
    virtual void gotFlatness (double) {}
    virtual void gotLineWidth (double) {}
    virtual void gotLineCaps (int) {}
    virtual void gotLineJoin (int) {}
    virtual void gotMiterLimit (double) {}
    virtual void gotWindingOrder (int) {}
    virtual void gotDash (const QValueVector<AIElement>& /*dashData*/, double /*phase*/) {}
};

class StructureHandlerBase {
  public:
   	 StructureHandlerBase() {}
	  virtual ~StructureHandlerBase() {}

    virtual void gotBeginGroup (bool /*clipping*/) {}
    virtual void gotEndGroup (bool /*clipping*/) {}
    virtual void gotBeginCombination () {}
    virtual void gotEndCombination () {}
};

class PathHandlerBase {
  public:
   	 PathHandlerBase() {}
	  virtual ~PathHandlerBase() {}

    virtual void gotPathElement (PathElement &) {}
    virtual void gotFillPath (bool /*closed*/, bool /*reset*/) {}
    virtual void gotStrokePath (bool /*closed*/) {}
    virtual void gotIgnorePath (bool /*closed*/, bool /*reset*/) {}
    virtual void gotClipPath (bool /*closed*/) {}
    virtual void gotFillMode (FillMode) {}
};

class MiscGStateHandlerBase {
  public:
   	 MiscGStateHandlerBase() {}
	  virtual ~MiscGStateHandlerBase() {}

    virtual void gotLockNextObject (bool /*value*/) {}
    virtual void gotFillOverprinting (bool /*value*/) {}
    virtual void gotStrokeOverprinting (bool /*value*/) {}
};

class DocumentHandlerBase {
  public:
   	 DocumentHandlerBase() {}
	  virtual ~DocumentHandlerBase() {}

    virtual void gotBoundingBox (int /*llx*/, int /*lly*/, int /*urx*/, int /*ury*/) {}
    virtual void gotTemplateBox (int /*llx*/, int /*lly*/, int /*urx*/, int /*ury*/) {}
    virtual void gotMargin (int /*llx*/, int /*lly*/, int /*urx*/, int /*ury*/) {}
    virtual void gotPrinterRect (int /*llx*/, int /*lly*/, int /*urx*/, int /*ury*/) {}
    virtual void gotPrinterName (const char *) {}
    virtual void gotPageOrigin (int /*x*/, int /*y*/) {}
    virtual void gotTemplate (const char *) {}
    virtual void gotTitle (const char *) {}
    virtual void gotCreator (const char *) {}
    virtual void gotPatternDefinition (const char */*name*/, const QValueVector<AIElement>& /*layerData*/, double /*llx*/, double /*lly*/, double /*urx*/, double /*ury*/) {}
    virtual void gotCreationDate (const char */*val1*/,const char */*val2*/) {}
    virtual void gotProcessColors (int /*colors*/) {}
};

class ModuleHandlerBase {
  public:
   	 ModuleHandlerBase() {}
	  virtual ~ModuleHandlerBase() {}

    virtual void gotBeginSection (SectionType, const char *) {}
    virtual void gotEndSection (SectionType, const char *) {}
};

class EmbeddedHandlerBase {
  public:
   	 EmbeddedHandlerBase() {}
	  virtual ~EmbeddedHandlerBase() {}

    virtual void gotGsaveIncludeDocument (const QValueVector<AIElement>& /*transData*/, int /*llx*/, int /*lly*/, int /*urx*/, int /*ury*/, const char*/*fileName*/) {}
    virtual void gotGrestore () {}
};

class TextHandlerBase {
  public:
   	 TextHandlerBase() {}
 	   virtual ~TextHandlerBase() {}

    virtual void gotFontEncoding (const QValueVector<AIElement>& encodingData, const char*oldFontName, const char*newFontName);
    virtual void gotFontDefinition (const char*fontName, double size, double leading, double kerning, TextAlign align);
    virtual void gotTextBlockBegin (const QValueVector<AIElement>& transData, TextOperation mode);
    virtual void gotTextOutput (const char*text, int length=-1);
    virtual void gotTextBlockEnd ();
};

#endif

