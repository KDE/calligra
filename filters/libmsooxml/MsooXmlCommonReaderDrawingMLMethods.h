/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * Copyright (C) 2011-2012 Matus Uzak (matus.uzak@gmail.com).
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

// This is not a normal header, *don't* add include guards to it.
// This will cause the compiler to get wrong offsets and to corrupt
// the stack.

protected:

#include "MsooXmlDrawingMLShared.h"

//! When dealing with colors there's no way to know what type of
//! attribute we are setting. While MSOOXML doesn't need to know the
//! context in which a color is used, ODF does need to know this.
enum ColorType {
    BackgroundColor,
    OutlineColor,
    TextColor,
    GradientColor
};

enum blipFillCaller {
    blipFill_lockedCanvas = 'a',
    blipFill_spPr = 'a',
    blipFill_pic = 'p', //dml in pptx; for dml in docx use 'pic'
    blipFill_rPr = 'p', //dml
    blipFill_bgPr = 'a', // pptx
    blipFill_grpSpPr = 'a'
};

enum cNvPrCaller {
    cNvPr_nvSpPr,
    cNvPr_nvPicPr,
    cNvPr_nvCxnSpPr
};

enum txBodyCaller {
    DrawingML_txBody_tc,
    DrawingML_txBody_txSp,
    DrawingML_txBody_sp
};

enum spacingType {
    spacingMarginTop,
    spacingLines,
    spacingMarginBottom
};

struct GroupProp {
    qint64 svgXOld;
    qint64 svgYOld;
    qreal svgWidthOld;
    qreal svgHeightOld;
    qreal svgXChOld;
    qreal svgYChOld;
    qreal svgWidthChOld;
    qreal svgHeightChOld;
};

void initDrawingML();

/**
 * @return true in case a custom or supported predefined shape is
 * being processed and false in case of a text-box and rectangle.
 */
bool isCustomShape();

/**
 * @return true in case an unsupported shape is being processed.
 */
bool unsupportedPredefinedShape();

/**
 * TODO:
 */
void preReadSp();

/**
 * Write the parent el. in {draw:line, draw:custom-shape, draw:frame}
 * to the body writer and create a graphic style for a shape.
 */
void generateFrameSp();

/**
 * Write the draw:enhanced-geometry element to the body writer in case
 * of a draw:custom-shape.
 */
void writeEnhancedGeometry();

// ----------------------------------------
// MSOOXML_CURRENT_NS "dgm"
// ----------------------------------------
KoFilter::ConversionStatus read_relIds();

// ----------------------------------------
// MSOOXML_CURRENT_NS "c"
// ----------------------------------------
KoFilter::ConversionStatus read_chart();

// ----------------------------------------
// MSOOXML_CURRENT_NS "lc"
// ----------------------------------------
KoFilter::ConversionStatus read_lockedCanvas();

// ----------------------------------------
// MSOOXML_CURRENT_NS == DRAWINGML_PIC_NS
// ----------------------------------------
// The following elements can have a namespace in {a,pic,p,xdr}.
KoFilter::ConversionStatus read_cNvPicPr(); //done
KoFilter::ConversionStatus read_cNvPr(cNvPrCaller caller); //done
KoFilter::ConversionStatus read_nvPicPr(); //done
KoFilter::ConversionStatus read_pic(); //done
KoFilter::ConversionStatus read_nvSpPr(); //done
KoFilter::ConversionStatus read_grpSp(); //done
KoFilter::ConversionStatus read_grpSpPr(); //done
KoFilter::ConversionStatus read_nvCxnSpPr(); //done
KoFilter::ConversionStatus read_cNvSpPr(); //done
KoFilter::ConversionStatus read_cxnSp(); //done
KoFilter::ConversionStatus read_sp(); //done
KoFilter::ConversionStatus read_spPr(); //done
KoFilter::ConversionStatus read_style(); //done

// ----------------------------------------
// MSOOXML_CURRENT_NS "a"
// ----------------------------------------
KoFilter::ConversionStatus read_lnRef();
KoFilter::ConversionStatus read_masterClrMapping();
KoFilter::ConversionStatus read_overrideClrMapping();
KoFilter::ConversionStatus read_DrawingML_p();
KoFilter::ConversionStatus read_DrawingML_r();
KoFilter::ConversionStatus read_DrawingML_br();
KoFilter::ConversionStatus read_endParaRPr();
KoFilter::ConversionStatus read_DrawingML_rPr();
KoFilter::ConversionStatus read_DrawingML_pPr();
KoFilter::ConversionStatus read_hlinkClick();
KoFilter::ConversionStatus read_custGeom();
KoFilter::ConversionStatus read_xfrm();
KoFilter::ConversionStatus read_off();
KoFilter::ConversionStatus read_chOff();
KoFilter::ConversionStatus read_ext();
KoFilter::ConversionStatus read_chExt();
KoFilter::ConversionStatus read_blip();
KoFilter::ConversionStatus read_stretch();
KoFilter::ConversionStatus read_biLevel();
KoFilter::ConversionStatus read_grayscl();
KoFilter::ConversionStatus read_lum();
KoFilter::ConversionStatus read_duotone();
KoFilter::ConversionStatus read_tile();
KoFilter::ConversionStatus read_srcRect();
KoFilter::ConversionStatus read_fillRect();
KoFilter::ConversionStatus read_graphic();
KoFilter::ConversionStatus read_graphicData();
KoFilter::ConversionStatus read_blipFill(blipFillCaller caller);
KoFilter::ConversionStatus read_txSp();

//! Convert attributes of Text Run Properties to ODF equivalents.
void handleRprAttributes(const QXmlStreamAttributes& attrs);

// ----------------------------------------
// MSOOXML_CURRENT_NS == DRAWINGML_NS
// ----------------------------------------
// The following elements can have a namespace in {a,wp}.
KoFilter::ConversionStatus read_lstStyle();
KoFilter::ConversionStatus read_latin();
KoFilter::ConversionStatus read_DrawingML_highlight();
KoFilter::ConversionStatus read_gradFill();
KoFilter::ConversionStatus read_gradFillRpr();
KoFilter::ConversionStatus read_lin();
KoFilter::ConversionStatus read_gsLst();
KoFilter::ConversionStatus read_gs();
KoFilter::ConversionStatus read_noFill();
KoFilter::ConversionStatus read_prstGeom();
KoFilter::ConversionStatus read_avLst();
KoFilter::ConversionStatus read_gd();
KoFilter::ConversionStatus read_effectLst();
KoFilter::ConversionStatus read_outerShdw();
KoFilter::ConversionStatus lvlHelper(const QString& level);
KoFilter::ConversionStatus read_lvl1pPr();
KoFilter::ConversionStatus read_lvl2pPr();
KoFilter::ConversionStatus read_lvl3pPr();
KoFilter::ConversionStatus read_lvl4pPr();
KoFilter::ConversionStatus read_lvl5pPr();
KoFilter::ConversionStatus read_lvl6pPr();
KoFilter::ConversionStatus read_lvl7pPr();
KoFilter::ConversionStatus read_lvl8pPr();
KoFilter::ConversionStatus read_lvl9pPr();
KoFilter::ConversionStatus read_buBlip();
KoFilter::ConversionStatus read_buChar();
KoFilter::ConversionStatus read_buClr();
KoFilter::ConversionStatus read_buClrTx();
KoFilter::ConversionStatus read_buSzPct();
KoFilter::ConversionStatus read_buSzPts();
KoFilter::ConversionStatus read_buFont();
KoFilter::ConversionStatus read_buNone();
KoFilter::ConversionStatus read_buAutoNum();
KoFilter::ConversionStatus read_fld();
KoFilter::ConversionStatus read_spcBef();
KoFilter::ConversionStatus read_spcAft();
KoFilter::ConversionStatus read_lnSpc();
KoFilter::ConversionStatus read_spcPts();
KoFilter::ConversionStatus read_spcPct();
KoFilter::ConversionStatus read_defRPr();
KoFilter::ConversionStatus read_bodyPr();
KoFilter::ConversionStatus read_normAutofit();
KoFilter::ConversionStatus read_spAutoFit();

//! Inherit default body properties
void inheritDefaultBodyProperties();

void algnToODF(const char *odfEl, const QString& emuValue);

//! Sets the fo:margin-* attribute of graphic-properties for read_anchor().
void distToODF(const char *odfEl, const QString& emuValue);

// ----------------------------------------
// MSOOXML_CURRENT_NS == (DRAWINGML_NS || DRAWINGML_TXBODY_NS)
// ----------------------------------------

KoFilter::ConversionStatus read_DrawingML_txBody(txBodyCaller caller);

// ----------------------------------------

MSOOXML::Utils::autoFitStatus m_normAutofit; // Whether text should be fitted to fit the shape

bool m_isLockedCanvas;

KoGenStyle m_referredFont;

int m_gradPosition;
QString m_gradAngle;
bool m_xfrm_read;
bool m_insideTable;
qreal m_maxParaFontPt; // Largest font size used in the paragraph
qreal m_minParaFontPt; // minimum font size used in the paragraph
read_p_args m_read_DrawingML_p_args;

bool m_contentAvLstExists; // whether avLst exists
QMap<QString, QString> m_avModifiers;

QString m_customPath;
QString m_customEquations;
QString m_textareas;

QString m_contentType; // read in prstGeom

spacingType m_currentSpacingType; // determines how spcPct and spcPts should behave

QString m_shapeTextPosition;
QString m_shapeTextTopOff;
QString m_shapeTextBottomOff;
QString m_shapeTextLeftOff;
QString m_shapeTextRightOff;

bool m_listStylePropertiesAltered;
QString m_prevListStyleName;

int m_prevListLevel; //! set by drawingML_ppr
int m_currentListLevel; //! set by drawingML_ppr

// Map of level keys and xml:id values of text:list elements to continue
// automatic numbering.
QMap<quint16, QString> m_lvlXmlIdMap;

// true - continue numbered list, false - restart numbering
QMap<quint16, bool> m_continueListNumbering;

//! set by read_drawing() to indicate if we have encountered
//! drawing/anchor, used by read_pic()
bool m_drawing_anchor;

//! set by read_drawing() to indicate if we have encountered
//! drawing/inline, used by read_pic()
bool m_drawing_inline;

// Shape properties
qint64 m_svgX; //!< set by read_off()
qint64 m_svgY; //!< set by read_off()
int m_svgWidth; //! set by read_ext()
int m_svgHeight; //! set by read_ext()
int m_svgChX; //!< set by read_chOff()
int m_svgChY; //!< set by read_chOff()
int m_svgChWidth; //! set by read_chExt()
int m_svgChHeight; //! set by read_chExt()

bool m_inGrpSpPr; //Whether we are in group shape, affects transformations
bool m_flipH; //! set by read_xfrm()
bool m_flipV; //! set by read_xfrm()

int m_rot; //! set by read_xfrm()

QVector<GroupProp> m_svgProp; //! value of the parent

QString m_xlinkHref; //!< set by read_blip()
bool m_ignoreLinkHref; //!< to ignore picture with a style which has picture fill
QString m_cNvPrId; //!< set by read_cNvPr()
QString m_cNvPrName; //!< set by read_cNvPr()
QString m_cNvPrDescr; //!< set by read_cNvPr()

bool m_hyperLink;
QString m_hyperLinkTarget;

QString m_recentDestName; // recent image

