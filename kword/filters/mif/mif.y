/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

%{
#include "treebuild.h"
#include "generate_xml.h"
#include "backend.h"
#include <fstream>
int decl_done = 1; /* 1 if done, 0 otherwise */
int yyparse();
void warning( char*s, char* t );
%}

%token <real> REAL
%token <integer> INTEGER
%token <string> QSTRING ID SQSTRING COMMENT

%union {
	double real;
	int    integer;
	char*  string;
	int    cmd;
	Frame* mif_frame;
	FrameElement* mif_frameelement;
	FrameElementList* mif_frameelementlist;
	Tag* mif_tag;
	Fill* mif_fill;
	PenWidth* mif_penwidth;
	Separation* mif_separation;
	ObColor* mif_obcolor;
	DashedPattern* mif_dashedpattern;
	DashedPatternElement* mif_dashedpatternelement;
	DashedPatternElementList* mif_dashedpatternelementlist;
	DashedStyle* mif_dashedstyle;
	DashSegment* mif_dashsegment;
	NumSegments* mif_numsegments;
	Angle* mif_angle;
	RunAroundGap* mif_runaroundgap;
	RunAroundType* mif_runaroundtype;
	ShapeRect* mif_shaperect;
	BRect* mif_brect;
	FrameType* mif_frametype;
	NSOffset* mif_nsoffset;
	BLOffset* mif_bloffset;
	AnchorAlign* mif_anchoralign;
	Cropped* mif_cropped;
	ImportObject* mif_importobject;
	ImportObjectElement* mif_importobjectelement;
	ImportObjectElementList* mif_importobjectelementlist;
	ExternalData* mif_externaldata;
	BitmapDPI* mif_bitmapdpi;
	ImportObFile* mif_importobfile;
	FlipLR* mif_fliplr;
	NativeOrigin* mif_nativeorigin;
	Math* mif_math;
	MathElement* mif_mathelement;
	MathElementList* mif_mathelementlist;
	MathFullForm* mif_mathfullform;
	MathLineBreak* mif_mathlinebreak;
	MathOrigin* mif_mathorigin;
	MathAlignment* mif_mathalignment;
	MathSize* mif_mathsize;
	PageElementList* mif_pageelementlist;
	PageElement* mif_pageelement;
	TextRect* mif_textrect;
	TextLine* mif_textline;
	PolyLine* mif_polyline;
	PageNum* mif_pagenum;
	PageBackground* mif_pagebackground;
	Rectangle* mif_rectangle;
	Polygon* mif_polygon;
	Ellipse* mif_ellipse;
	EllipseElement* mif_ellipseelement;
	EllipseElementList* mif_ellipseelementlist;
	Group* mif_group;
	GroupElement* mif_groupelement;
	GroupElementList* mif_groupelementlist;
	GroupID* mif_groupid;
	RectangleElement* mif_rectangleelement;
	RectangleElementList* mif_rectangleelementlist;
	TextRectElement* mif_textrectelement;
	TextRectElementList* mif_textrectelementlist;
	TRNumColumns* mif_trnumcolumns;
	TRColumnGap* mif_trcolumngap;
	TRColumnBalance* mif_trcolumnbalance;
	TRSideheadWidth* mif_trsideheadwidth;
	TRSideheadGap* mif_trsideheadgap;
	TRSideheadPlacement* mif_trsideheadplacement;
	TRNext* mif_trnext;
	TextLineElement* mif_textlineelement;
	TextLineElementList* mif_textlineelementlist;
	TLOrigin* mif_tlorigin;
	TLAlignment* mif_tlalignment;
	PolyLineElement* mif_polylineelement;
	PolyLineElementList* mif_polylineelementlist;
	HeadCap* mif_headcap;
	TailCap* mif_tailcap;
	ArrowStyle* mif_arrowstyle;
	ArrowStyleElement* mif_arrowstyleelement;
	ArrowStyleElementList* mif_arrowstyleelementlist;
	TipAngle* mif_tipangle;
	BaseAngle* mif_baseangle;
	Length* mif_length;
	HeadType* mif_headtype;
	ScaleHead* mif_scalehead;
	ScaleFactor* mif_scalefactor;
	NumPoints* mif_numpoints;
	Point* mif_point;
	PolygonElement* mif_polygonelement;
	PolygonElementList* mif_polygonelementlist;
	KumihanCatalog* mif_kumihancatalog;
	MarkerTypeCatalog* mif_markertypecatalog;
	MType* mif_mtype;
	MTypeName* mif_mtypename;
	MText* mif_mtext;
	MCurrPage* mif_mcurrpage;
	TFTag* mif_tftag;
	TFAutoConnect* mif_tfautoconnect;
	TFSynchronized* mif_tfsynchronized;
	TFLineSpacing* mif_tflinespacing;
	TFMinHangHeight* mif_tfminhangheight;
		DocumentElementList* mif_document;
	FNoteStartNum* mif_fnotestartnum;
	DocumentElementList* mif_documentelementlist;
	DocumentElement* mif_documentelement;
	DocumentAcrobatBookmarksIncludeTagNames* mif_dacrobatbookmarksincludetagnames;
	DocumentAcrobatParagraphBookmarks* mif_dacrobatparagraphbookmarks;
	DocumentAutoChBars* mif_dautochbars;
	DocumentBordersOn* mif_dborderson;
	DocumentChBarColor* mif_dchbarcolor;
	DocumentChBarGap* mif_dchbargap;
	DocumentChBarPosition* mif_dchbarposition;
	DocumentChBarWidth* mif_dchbarwidth;
	DocumentCurrentView* mif_dcurrentview;
	DocumentDisplayOverrides* mif_ddisplayoverrides;
	DocumentFluid* mif_dfluid;
	DocumentFluidSideHeads* mif_dfluidsideheads;
	DocumentFNoteAnchorPos* mif_dfnoteanchorpos;
	DocumentFNoteAnchorPrefix* mif_dfnoteanchorprefix;
	DocumentFNoteAnchorSuffix* mif_dfnoteanchorsuffix;
	DocumentFNoteLabels* mif_dfnotelabels;
	DocumentFNoteMaxH* mif_dfnotemaxh;
	DocumentFNoteNumberPos* mif_dfnotenumberpos;
	DocumentFNoteNumberPrefix* mif_dfnotenumberprefix;
	DocumentFNoteNumberSuffix* mif_dfnotenumbersuffix;
	DocumentFNoteNumStyle* mif_dfnotenumstyle;
	DocumentFNoteRestart* mif_dfnoterestart;
	DocumentFNoteTag* mif_dfnotetag;
	DocumentFrozenPages* mif_dfrozenpages;
	DocumentFullRulers* mif_dfullrulers;
	DocumentGenerateAcrobatInfo* mif_dgenerateacrobatinfo;
	DocumentGraphicsOff* mif_dgraphicsoff;
	DocumentGridOn* mif_dgridon;
	DocumentLanguage* mif_dlanguage;
	DocumentLineBreakChars* mif_dlinebreakchars;
	DocumentLinkBoundariesOn* mif_dlinkboundarieson;
	DocumentMathAlphaCharFontFamily* mif_dmathalphacharfontfamily;
	DocumentMathCatalog* mif_dmathcatalog;
	DocumentMathFunctions* mif_dmathfunctions;
	DocumentMathGreek* mif_dmathgreek;
	DocumentMathLargeHoriz* mif_dmathlargehoriz;
	DocumentMathLargeIntegral* mif_dmathlargeintegral;
	DocumentMathLargeLevel1* mif_dmathlargelevel1;
	DocumentMathLargeLevel2* mif_dmathlargelevel2;
	DocumentMathLargeLevel3* mif_dmathlargelevel3;
	DocumentMathLargeSigma* mif_dmathlargesigma;
	DocumentMathLargeVert* mif_dmathlargevert;
	DocumentMathMediumHoriz* mif_dmathmediumhoriz;
	DocumentMathMediumIntegral* mif_dmathmediumintegral;
	DocumentMathMediumLevel1* mif_dmathmediumlevel1;
	DocumentMathMediumLevel2* mif_dmathmediumlevel2;
	DocumentMathMediumLevel3* mif_dmathmediumlevel3;
	DocumentMathMediumSigma* mif_dmathmediumsigma;
	DocumentMathMediumVert* mif_dmathmediumvert;
	DocumentMathNumbers* mif_dmathnumbers;
	DocumentMathShowCustom* mif_dmathshowcustom;
	DocumentMathSmallHoriz* mif_dmathsmallhoriz;
	DocumentMathSmallIntegral* mif_dmathsmallintegral;
	DocumentMathSmallLevel1* mif_dmathsmalllevel1;
	DocumentMathSmallLevel2* mif_dmathsmalllevel2;
	DocumentMathSmallLevel3* mif_dmathsmalllevel3;
	DocumentMathSmallSigma* mif_dmathsmallsigma;
	DocumentMathSmallVert* mif_dmathsmallvert;
	DocumentMathStrings* mif_dmathstrings;
	DocumentMathVariables* mif_dmathvariables;
	DocumentMenuBar* mif_dmenubar;
	DocumentNarrowRubiSpaceForJapanese* mif_dnarrowrubispaceforjapanese;
	DocumentNarrowRubiSpaceForOther* mif_dnarrowrubispaceforother;
	DocumentNextUnique* mif_dnextunique;
	DocumentPageGrid* mif_dpagegrid;
	DocumentPageNumStyle* mif_dpagenumstyle;
	DocumentPagePointStyle* mif_dpagepointstyle;
	DocumentPageRounding* mif_dpagerounding;
	DocumentPageScrolling* mif_dpagescrolling;
	DocumentPageSize* mif_dpagesize;
	DocumentParity* mif_dparity;
	DocumentPrintSeparations* mif_dprintseparations;
	DocumentPrintSkipBlankPages* mif_dprintskipblankpages;
	DocumentPunctuationChars* mif_dpunctuationchars;
	DocumentRubiAlignAtLineBounds* mif_drubialignatlinebounds;
	DocumentRubiOverhang* mif_drubioverhang;
	DocumentRubiSize* mif_drubisize;
	DocumentRulersOn* mif_drulerson;
	DocumentShowAllConditions* mif_dshowallconditions;
	DocumentSmallCapsSize* mif_dsmallcapssize;
	DocumentSmallCapsStretch* mif_dsmallcapsstretch;
	DocumentSmartQuotesOn* mif_dsmartquoteson;
	DocumentSmartSpacesOn* mif_dsmartspaceson;
	DocumentSnapGrid* mif_dsnapgrid;
	DocumentSnapRotation* mif_dsnaprotation;
	DocumentStartPage* mif_dstartpage;
	DocumentSubscriptShift* mif_dsubscriptshift;
	DocumentSubscriptSize* mif_dsubscriptsize;
	DocumentSubscriptStretch* mif_dsubscriptstretch;
	DocumentSuperscriptShift* mif_dsuperscriptshift;
	DocumentSuperscriptSize* mif_dsuperscriptsize;
	DocumentSuperscriptStretch* mif_dsuperscriptstretch;
	DocumentSymbolsOn* mif_dsymbolson;
	DocumentTblFNoteAnchorPos* mif_dtblfnoteanchorpos;
	DocumentTblFNoteAnchorPrefix* mif_dtblfnoteanchorprefix;
	DocumentTblFNoteAnchorSuffix* mif_dtblfnoteanchorsuffix;
	DocumentTblFNoteLabels* mif_dtblfnotelabels;
	DocumentTblFNoteNumberPos* mif_dtblfnotenumberpos;
	DocumentTblFNoteNumberPrefix* mif_dtblfnotenumberprefix;
	DocumentTblFNoteNumberSuffix* mif_dtblfnotenumbersuffix;
	DocumentTblFNoteNumStyle* mif_dtblfnotenumstyle;
	DocumentTblFNoteTag* mif_dtblfnotetag;
	DocumentTwoSides* mif_dtwosides;
	DocumentUpdateTextInsetsOnOpen* mif_dupdatetextinsetsonopen;
	DocumentUpdateXRefsOnOpen* mif_dupdatexrefsonopen;
	DocumentViewOnly* mif_dviewonly;
	DocumentViewOnlySelect* mif_dviewonlyselect;
	DocumentViewOnlyWinBorders* mif_dviewonlywinborders;
	DocumentViewOnlyWinMenubar* mif_dviewonlywinmenubar;
	DocumentViewOnlyWinPalette* mif_dviewonlywinpalette;
	DocumentViewOnlyWinPopup* mif_dviewonlywinpopup;
	DocumentViewOnlyXRef* mif_dviewonlyxref;
	DocumentViewRect* mif_dviewrect;
	DocumentViewScale* mif_dviewscale;
	DocumentVoMenuBar* mif_dvomenubar;
	DocumentWideRubiSpaceForJapanese* mif_dwiderubispaceforjapanese;
	DocumentWideRubiSpaceForOther* mif_dwiderubispaceforother;
	DocumentWindowRect* mif_dwindowrect;
	String* mif_string;
	Char* mif_char;
	TextRectID* mif_textrectid;
	Variable* mif_variable;
	VariableElement* mif_variableelement;
	VariableElementList* mif_variableelementlist;
	ATbl* mif_atbl;
	AFrame* mif_aframe;
	Marker* mif_marker;
	MarkerElement* mif_markerelement;
	MarkerElementList* mif_markerelementlist;
	XRefEnd* mif_xrefend;
	Para* mif_para;
	ParaElement* mif_paraelement;
	ParaElementList* mif_paraelementlist;
	ParaLine* mif_paraline;
	ParaLineElement* mif_paralineelement;
	ParaLineElementList* mif_paralineelementlist;
	FNote* mif_fnote;
	FNoteElement* mif_fnoteelement;
	FNoteElementList* mif_fnoteelementlist;
	Unique* mif_unique;
	FrameID* mif_id;
	Pen* mif_pen;
	Notes* mif_notes;
	NotesElement* mif_noteselement;
	NotesElementList* mif_noteselementlist;
	XRefSrcText* mif_xrefsrctext;
	XRefSrcFile* mif_xrefsrcfile;
	XRefSrcIsElem* mif_xrefsrciselem;
	XRefLastUpdate* mif_xreflastupdate;
	VariableName* mif_variablename;
	VariableDef* mif_variabledef;
	XRefName* mif_xrefname;
	XRef* mif_xref;
	XRefElement* mif_xrefelement;
	XRefElementList* mif_xrefelementlist;
	Font* mif_font;
	FontElementList* mif_fontelementlist;
	FontElement* mif_fontelement;
	Page* mif_page;
	PageType* mif_pagetype;
	PageTag* mif_pagetag;
	PageSize* mif_pagesize;
	PageOrientation* mif_pageorientation;
	PageAngle* mif_pageangle;
	TextFlow* mif_textflow;
	TextFlowElement* mif_textflowelement;
	TextFlowElementList* mif_textflowelementlist;
	TabStopElementList* mif_tabstopelementlist;
	TabStopElement* mif_tabstopelement;
	TSX_* mif_tsx;
	TSType* mif_tstype;
	TSLeaderStr* mif_tsleaderstr;
	TSDecimalChar* mif_tsdecimalchar;
	PgfFontElementList* mif_pgffontelementlist;
	PgfFontElement* mif_pgffontelement;
	FTag* mif_ftag;
	FPostScriptName* mif_fpostscriptname;
	FFamily* mif_ffamily;
	FVar* mif_fvar;
	FWeight* mif_fweight;
	FAngle* mif_fangle;
	FEncoding* mif_fencoding;
	FSize* mif_fsize;
	FUnderlining* mif_funderlining;
	FOverline* mif_foverline;
	FStrike* mif_fstrike;
	FChangeBar* mif_fchangebar;
	FOutline* mif_foutline;
	FShadow* mif_fshadow;
	FPairKern* mif_fpairkern;
	FTsume* mif_ftsume;
	FCase* mif_fcase;
	FPosition* mif_fposition;
	FDX_* mif_fdx;
	FDY_* mif_fdy;
	FDW_* mif_fdw;
	FStretch* mif_fstretch;
	FLanguage* mif_flanguage;
	FLocked* mif_flocked;
	FSeparation* mif_fseparation;
	FColor* mif_fcolor;
	Pgf* mif_pgf;
	PgfElementList* mif_pgfelementlist;
	PgfElement* mif_pgfelement;
	PgfTag* mif_pgftag;
	PgfUseNextTag* mif_pgfusenexttag;
	PgfNextTag* mif_pgfnexttag;
	PgfAlignment* mif_pgfalignment;
	PgfFIndent* mif_pgffindent;
	PgfFIndentRelative* mif_pgffindentrelative;
	PgfFIndentOffset* mif_pgffindentoffset;
	PgfLIndent* mif_pgflindent;
	PgfRIndent* mif_pgfrindent;
	PgfTopSeparator* mif_pgftopseparator;
	PgfTopSepAtIndent* mif_pgftopsepatindent;
	PgfTopSepOffset* mif_pgftopsepoffset;
	PgfBotSeparator* mif_pgfbotseparator;
	PgfBotSepAtIndent* mif_pgfbotsepatindent;
	PgfBotSepOffset* mif_pgfbotsepoffset;
	PgfPlacement* mif_pgfplacement;
	PgfPlacementStyle* mif_pgfplacementstyle;
	PgfRunInDefaultPunct* mif_pgfrunindefaultpunct;
	PgfSpBefore* mif_pgfspbefore;
	PgfSpAfter* mif_pgfspafter;
	PgfWithPrev* mif_pgfwithprev;
	PgfWithNext* mif_pgfwithnext;
	PgfBlockSize* mif_pgfblocksize;
	PgfFont* mif_pgffont;
	PgfLineSpacing* mif_pgflinespacing;
	PgfLeading* mif_pgfleading;
	PgfAutoNum* mif_pgfautonum;
	PgfNumTabs* mif_pgfnumtabs;
	PgfNumFormat* mif_pgfnumformat;
	PgfNumberFont* mif_pgfnumberfont;
	PgfNumAtEnd* mif_pgfnumatend;
	PgfHyphenate* mif_pgfhyphenate;
	HyphenMaxLines* mif_hyphenmaxlines;
	HyphenMinPrefix* mif_hyphenminprefix;
	HyphenMinSuffix* mif_hyphenminsuffix;
	HyphenMinWord* mif_hyphenminword;
	PgfNumString* mif_pgfnumstring;
	PgfLetterSpace* mif_pgfletterspace;
	PgfMinWordSpace* mif_pgfminwordspace;
	PgfMaxWordSpace* mif_pgfmaxwordspace;
	PgfOptWordSpace* mif_pgfoptwordspace;
	PgfLanguage* mif_pgflanguage;
	PgfCellAlignment* mif_pgfcellalignment;
	FPlatformName* mif_fplatformname;
	PgfMinJRomanLetterSpace* mif_pgfminjromanletterspace;
	PgfMaxJRomanLetterSpace* mif_pgfmaxjromanletterspace;
	PgfOptJRomanLetterSpace* mif_pgfoptjromanletterspace;
	PgfMinJLetterSpace* mif_pgfminjletterspace;
	PgfMaxJLetterSpace* mif_pgfmaxjletterspace;
	PgfOptJLetterSpace* mif_pgfoptjletterspace;
	PgfYakumonoType* mif_pgfyakumonotype;
	PgfAcrobatLevel* mif_pgfacrobatlevel;
	PgfCellMargins* mif_pgfcellmargins;
	PgfCellLMarginFixed* mif_pgfcelllmarginfixed;
	PgfCellRMarginFixed* mif_pgfcellrmarginfixed;
	PgfCellTMarginFixed* mif_pgfcelltmarginfixed;
	PgfCellBMarginFixed* mif_pgfcellbmarginfixed;
	PgfLocked* mif_pgflocked;
	TabStop* mif_tabstop;
}

%type <string> qstring
%type <integer> iexpr


%token <cmd> ELLIPSE
%token <cmd> GROUP
%token <cmd> GROUPID
%token <cmd> POLYGON
%token <cmd> BITMAPDPI
%token <cmd> MATH
%token <cmd> MATHFULLFORM
%token <cmd> MATHLINEBREAK
%token <cmd> MATHORIGIN
%token <cmd> MATHALIGNMENT
%token <cmd> MATHSIZE
%token <cmd> RUNAROUNDGAP
%token <cmd> RUNAROUNDTYPE
%token <cmd> KUMIHANCATALOG
%token <cmd> XREF
%token <cmd> XREFEND
%token <cmd> XREFSRCTEXT
%token <cmd> XREFSRCISELEM
%token <cmd> XREFSRCFILE
%token <cmd> XREFLASTUPDATE
%token <cmd> MARKERTYPECATALOG
%token <cmd> MARKER
%token <cmd> MTYPE
%token <cmd> MTYPENAME
%token <cmd> MTEXT
%token <cmd> MCURRPAGE
%token <cmd> FNOTE
%token <cmd> PGFNUMSTRING
%token <cmd> ATBL
%token <cmd> AFRAME
%token <cmd> TFTAG
%token <cmd> TFAUTOCONNECT
%token <cmd> VARIABLE
%token <cmd> TEXTRECTID
%token <cmd> PGFLOCKED
%token <cmd> AFRAMES 
%token <cmd> ANCHORALIGN 
%token <cmd> ANGLE 
%token <cmd> ARROWSTYLE
%token <cmd> AUTONUMSERIES
%token <cmd> BASEANGLE
%token <cmd> BLOFFSET 
%token <cmd> BOOKCOMPONENT 
%token <cmd> BRECT
%token <cmd> CCOLOR 
%token <cmd> CELL 
%token <cmd> CELLBRULING
%token <cmd> CELLCOLOR
%token <cmd> CELLCOLUMNS 
%token <cmd> CELLCONTENT 
%token <cmd> CELLFILL
%token <cmd> CELLLRULING
%token <cmd> CELLROWS 
%token <cmd> CELLRRULING
%token <cmd> CELLTRULING
%token <cmd> CELLSEPARATION
%token <cmd> CHAR 
%token <cmd> CHARUNITS
%token <cmd> COLOR 
%token <cmd> COLORATTRIBUTE 
%token <cmd> COLORBLACK 
%token <cmd> COLORCATALOG 
%token <cmd> COLORCYAN 
%token <cmd> COLORLIBRARYFAMILYNAME
%token <cmd> COLORLIBRARYINKNAME
%token <cmd> COLORMAGENTA 
%token <cmd> COLORS 
%token <cmd> COLORTAG 
%token <cmd> COLORYELLOW
%token <cmd> COMBINEDFONTCATALOG
%token <cmd> CONDITION 
%token <cmd> CONDITIONCATALOG 
%token <cmd> CROPPED 
%token <cmd> CSEPARATION 
%token <cmd> CSTATE 
%token <cmd> CSTYLE 
%token <cmd> CTAG
%token <cmd> DACROBATBOOKMARKSINCLUDETAGNAMES 
%token <cmd> DACROBATPARAGRAPHBOOKMARKS
%token <cmd> DASHEDPATTERN 
%token <cmd> DASHEDSTYLE
%token <cmd> DASHSEGMENT
%token <cmd> DAUTOCHBARS 
%token <cmd> DBORDERSON 
%token <cmd> DCHBARCOLOR 
%token <cmd> DCHBARGAP 
%token <cmd> DCHBARPOSITION 
%token <cmd> DCHBARWIDTH 
%token <cmd> DCURRENTVIEW 
%token <cmd> DDISPLAYOVERRIDES 
%token <cmd> DERIVELINKS 
%token <cmd> DERIVETAG 
%token <cmd> DERIVETYPE
%token <cmd> DFLUID 
%token <cmd> DFLUIDSIDEHEADS 
%token <cmd> DFNOTEANCHORPOS
%token <cmd> DFNOTEANCHORPREFIX 
%token <cmd> DFNOTEANCHORSUFFIX 
%token <cmd> DFNOTELABELS 
%token <cmd> DFNOTEMAXH
%token <cmd> DFNOTENUMBERPOS 
%token <cmd> DFNOTENUMBERPREFIX 
%token <cmd> DFNOTENUMBERSUFFIX 
%token <cmd> DFNOTENUMSTYLE 
%token <cmd> DFNOTERESTART 
%token <cmd> DFNOTETAG 
%token <cmd> DFROZENPAGES 
%token <cmd> DFULLRULERS 
%token <cmd> DGENERATEACROBATINFO 
%token <cmd> DGRAPHICSOFF 
%token <cmd> DGRIDON 
%token <cmd> DICTIONARY 
%token <cmd> DLANGUAGE 
%token <cmd> DLINEBREAKCHARS 
%token <cmd> DLINKBOUNDARIESON 
%token <cmd> DMATHALPHACHARFONTFAMILY 
%token <cmd> DMATHCATALOG 
%token <cmd> DMATHFUNCTIONS 
%token <cmd> DMATHGREEK 
%token <cmd> DMATHLARGEHORIZ 
%token <cmd> DMATHLARGEINTEGRAL 
%token <cmd> DMATHLARGELEVEL1 
%token <cmd> DMATHLARGELEVEL2 
%token <cmd> DMATHLARGELEVEL3 
%token <cmd> DMATHLARGESIGMA 
%token <cmd> DMATHLARGEVERT 
%token <cmd> DMATHMEDIUMHORIZ 
%token <cmd> DMATHMEDIUMINTEGRAL 
%token <cmd> DMATHMEDIUMLEVEL1 
%token <cmd> DMATHMEDIUMLEVEL2 
%token <cmd> DMATHMEDIUMLEVEL3 
%token <cmd> DMATHMEDIUMSIGMA 
%token <cmd> DMATHMEDIUMVERT 
%token <cmd> DMATHNUMBERS 
%token <cmd> DMATHSHOWCUSTOM 
%token <cmd> DMATHSMALLHORIZ 
%token <cmd> DMATHSMALLINTEGRAL 
%token <cmd> DMATHSMALLLEVEL1 
%token <cmd> DMATHSMALLLEVEL2 
%token <cmd> DMATHSMALLLEVEL3 
%token <cmd> DMATHSMALLSIGMA 
%token <cmd> DMATHSMALLVERT 
%token <cmd> DMATHSTRINGS 
%token <cmd> DMATHVARIABLES 
%token <cmd> DMENUBAR 
%token <cmd> DNARROWRUBISPACEFORJAPANESE 
%token <cmd> DNARROWRUBISPACEFOROTHER 
%token <cmd> DNEXTUNIQUE 
%token <cmd> DNOPRINTSEPCOLOR 
%token <cmd> DOCUMENT 
%token <cmd> DPAGEGRID 
%token <cmd> DPAGENUMSTYLE 
%token <cmd> DPAGEPOINTSTYLE 
%token <cmd> DPAGEROUNDING 
%token <cmd> DPAGESCROLLING 
%token <cmd> DPAGESIZE 
%token <cmd> DPARITY 
%token <cmd> DPRINTSEPARATIONS 
%token <cmd> DPRINTSKIPBLANKPAGES 
%token <cmd> DPUNCTUATIONCHARS 
%token <cmd> DRUBIALIGNATLINEBOUNDS 
%token <cmd> DRUBIOVERHANG 
%token <cmd> DRUBISIZE 
%token <cmd> DRULERSON 
%token <cmd> DSHOWALLCONDITIONS 
%token <cmd> DSMALLCAPSSIZE 
%token <cmd> DSMALLCAPSSTRETCH 
%token <cmd> DSMARTQUOTESON 
%token <cmd> DSMARTSPACESON 
%token <cmd> DSNAPGRID 
%token <cmd> DSNAPROTATION 
%token <cmd> DSTARTPAGE 
%token <cmd> DSUBSCRIPTSHIFT 
%token <cmd> DSUBSCRIPTSIZE 
%token <cmd> DSUBSCRIPTSTRETCH 
%token <cmd> DSUPERSCRIPTSHIFT 
%token <cmd> DSUPERSCRIPTSIZE 
%token <cmd> DSUPERSCRIPTSTRETCH 
%token <cmd> DSYMBOLSON 
%token <cmd> DTBLFNOTEANCHORPOS 
%token <cmd> DTBLFNOTEANCHORPREFIX 
%token <cmd> DTBLFNOTEANCHORSUFFIX 
%token <cmd> DTBLFNOTELABELS 
%token <cmd> DTBLFNOTENUMBERPOS 
%token <cmd> DTBLFNOTENUMBERPREFIX 
%token <cmd> DTBLFNOTENUMBERSUFFIX 
%token <cmd> DTBLFNOTENUMSTYLE 
%token <cmd> DTBLFNOTETAG 
%token <cmd> DTWOSIDES 
%token <cmd> DUPDATETEXTINSETSONOPEN 
%token <cmd> DUPDATEXREFSONOPEN 
%token <cmd> DVIEWONLY 
%token <cmd> DVIEWONLYSELECT 
%token <cmd> DVIEWONLYWINBORDERS 
%token <cmd> DVIEWONLYWINMENUBAR 
%token <cmd> DVIEWONLYWINPALETTE 
%token <cmd> DVIEWONLYWINPOPUP 
%token <cmd> DVIEWONLYXREF 
%token <cmd> DVIEWRECT 
%token <cmd> DVIEWSCALE 
%token <cmd> DVOMENUBAR 
%token <cmd> DWIDERUBISPACEFORJAPANESE 
%token <cmd> DWIDERUBISPACEFOROTHER 
%token <cmd> DWINDOWRECT 
%token <cmd> FANGLE 
%token <cmd> FCASE 
%token <cmd> FCHANGEBAR 
%token <cmd> FCOLOR 
%token <cmd> FDW 
%token <cmd> FDX 
%token <cmd> FDY 
%token <cmd> FENCODING 
%token <cmd> FFAMILY
%token <cmd> FILENAME 
%token <cmd> FILENAMESUFFIX 
%token <cmd> FILL
%token <cmd> FLANGUAGE 
%token <cmd> FLIPLR 
%token <cmd> FLOCKED 
%token <cmd> FLOWTAG 
%token <cmd> FNOTESTARTNUM 
%token <cmd> FONT 
%token <cmd> FONTCATALOG 
%token <cmd> FOUTLINE 
%token <cmd> FOVERLINE 
%token <cmd> FPAIRKERN 
%token <cmd> FPLATFORMNAME
%token <cmd> FPOSITION 
%token <cmd> FPOSTSCRIPTNAME 
%token <cmd> FRAME 
%token <cmd> FRAMEID 
%token <cmd> FRAMETYPE 
%token <cmd> FSEPARATION 
%token <cmd> FSHADOW  
%token <cmd> FSIZE 
%token <cmd> FSTRETCH 
%token <cmd> FSTRIKE 
%token <cmd> FTAG
%token <cmd> FTSUME 
%token <cmd> FUNDERLINING 
%token <cmd> FVAR 
%token <cmd> FWEIGHT
%token <cmd> HEADCAP
%token <cmd> HEADTYPE
%token <cmd> HYPHENMAXLINES 
%token <cmd> HYPHENMINPREFIX 
%token <cmd> HYPHENMINSUFFIX 
%token <cmd> HYPHENMINWORD
%token <cmd> IMPORTOBFILE 
%token <cmd> IMPORTOBJECT 
%token <cmd> INITIALAUTONUMS
%token <cmd> LENGTH
%token <cmd> MIFFILE
%token <cmd> NATIVEORIGIN 
%token <cmd> NOTES 
%token <cmd> NSOFFSET 
%token <cmd> NUMCOUNTER
%token <cmd> NUMPOINTS
%token <cmd> NUMSEGMENTS
%token <cmd> OBCOLOR 
%token <cmd> OKWORD
%token <cmd> PAGE 
%token <cmd> PAGEANGLE 
%token <cmd> PAGEBACKGROUND
%token <cmd> PAGEORIENTATION 
%token <cmd> PAGENUM
%token <cmd> PAGESIZE 
%token <cmd> PAGETAG 
%token <cmd> PAGETYPE
%token <cmd> PARA 
%token <cmd> PARALINE 
%token <cmd> PEN 
%token <cmd> PENWIDTH
%token <cmd> PGF 
%token <cmd> PGFMINJROMANLETTERSPACE
%token <cmd> PGFOPTJROMANLETTERSPACE
%token <cmd> PGFMAXJROMANLETTERSPACE
%token <cmd> PGFMINJLETTERSPACE
%token <cmd> PGFOPTJLETTERSPACE
%token <cmd> PGFMAXJLETTERSPACE
%token <cmd> PGFYAKUMONOTYPE
%token <cmd> PGFACROBATLEVEL
%token <cmd> PGFALIGNMENT 
%token <cmd> PGFAUTONUM 
%token <cmd> PGFBLOCKSIZE 
%token <cmd> PGFBOTSEPARATOR
%token <cmd> PGFBOTSEPATINDENT
%token <cmd> PGFBOTSEPOFFSET
%token <cmd> PGFCATALOG 
%token <cmd> PGFCELLALIGNMENT 
%token <cmd> PGFCELLBMARGINFIXED
%token <cmd> PGFCELLLMARGINFIXED 
%token <cmd> PGFCELLMARGINS 
%token <cmd> PGFCELLRMARGINFIXED
%token <cmd> PGFCELLTMARGINFIXED 
%token <cmd> PGFFINDENT 
%token <cmd> PGFFINDENTOFFSET
%token <cmd> PGFFINDENTRELATIVE
%token <cmd> PGFFONT 
%token <cmd> PGFHYPHENATE
%token <cmd> PGFLANGUAGE 
%token <cmd> PGFLEADING 
%token <cmd> PGFLETTERSPACE 
%token <cmd> PGFLINDENT 
%token <cmd> PGFLINESPACING
%token <cmd> PGFMAXWORDSPACE 
%token <cmd> PGFMINWORDSPACE 
%token <cmd> PGFNEXTTAG 
%token <cmd> PGFNUMATEND 
%token <cmd> PGFNUMBERFONT 
%token <cmd> PGFNUMFORMAT 
%token <cmd> PGFNUMTABS 
%token <cmd> PGFOPTWORDSPACE
%token <cmd> PGFPLACEMENT 
%token <cmd> PGFPLACEMENTSTYLE 
%token <cmd> PGFRINDENT 
%token <cmd> PGFRUNINDEFAULTPUNCT
%token <cmd> PGFSPAFTER 
%token <cmd> PGFSPBEFORE 
%token <cmd> PGFTAG 
%token <cmd> PGFTOPSEPARATOR 
%token <cmd> PGFTOPSEPATINDENT
%token <cmd> PGFTOPSEPOFFSET
%token <cmd> PGFUSENEXTTAG
%token <cmd> PGFWITHNEXT 
%token <cmd> PGFWITHPREV
%token <cmd> POINT
%token <cmd> POLYLINE
%token <cmd> RECTANGLE
%token <cmd> ROW 
%token <cmd> ROWHEIGHT 
%token <cmd> ROWMAXHEIGHT
%token <cmd> ROWMINHEIGHT
%token <cmd> RULING 
%token <cmd> RULINGCATALOG 
%token <cmd> RULINGCOLOR 
%token <cmd> RULINGGAP 
%token <cmd> RULINGLINES
%token <cmd> RULINGPEN 
%token <cmd> RULINGPENWIDTH 
%token <cmd> RULINGSEPARATION 
%token <cmd> RULINGTAG
%token <cmd> SCALEFACTOR
%token <cmd> SCALEHEAD
%token <cmd> SEPARATION 
%token <cmd> SERIES 
%token <cmd> SHAPERECT 
%token <cmd> STRING
%token <cmd> TABSTOP 
%token <cmd> TAG
%token <cmd> TAILCAP
%token <cmd> TBL 
%token <cmd> TBLALIGNMENT 
%token <cmd> TBLALTSHADEPERIOD 
%token <cmd> TBLBLOCKSIZE
%token <cmd> TBLBODY 
%token <cmd> TBLBODYCOLOR 
%token <cmd> TBLBODYFILL 
%token <cmd> TBLBODYROWRULING 
%token <cmd> TBLBODYSEPARATION
%token <cmd> TBLBRULING 
%token <cmd> TBLCATALOG 
%token <cmd> TBLCELLMARGINS 
%token <cmd> TBLCOLUMN 
%token <cmd> TBLCOLUMNBODY 
%token <cmd> TBLCOLUMNF
%token <cmd> TBLCOLUMNH 
%token <cmd> TBLCOLUMNNUM 
%token <cmd> TBLCOLUMNRULING 
%token <cmd> TBLCOLUMNWIDTH
%token <cmd> TBLFORMAT 
%token <cmd> TBLH 
%token <cmd> TBLHFCOLOR 
%token <cmd> TBLHFFILL 
%token <cmd> TBLHFROWRULING 
%token <cmd> TBLHFSEPARATION
%token <cmd> TBLID 
%token <cmd> TBLINITNUMBODYROWS 
%token <cmd> TBLINITNUMCOLUMNS 
%token <cmd> TBLINITNUMFROWS 
%token <cmd> TBLINITNUMHROWS 
%token <cmd> TBLLASTBRULING 
%token <cmd> TBLLINDENT 
%token <cmd> TBLLOCKED
%token <cmd> TBLLRULING 
%token <cmd> TBLNUMBYCOLUMN 
%token <cmd> TBLNUMCOLUMNS 
%token <cmd> TBLPLACEMENT 
%token <cmd> TBLRINDENT 
%token <cmd> TBLRRULING 
%token <cmd> TBLRULINGPERIOD
%token <cmd> TBLS 
%token <cmd> TBLSEPARATORRULING 
%token <cmd> TBLSHADEBYCOLUMN 
%token <cmd> TBLSHADEPERIOD 
%token <cmd> TBLSPAFTER
%token <cmd> TBLSPBEFORE 
%token <cmd> TBLTAG
%token <cmd> TBLTITLE
%token <cmd> TBLTITLECONTENT 
%token <cmd> TBLTITLEGAP 
%token <cmd> TBLTITLEPGF1
%token <cmd> TBLTITLEPLACEMENT 
%token <cmd> TBLTRULING 
%token <cmd> TBLXCOLOR 
%token <cmd> TBLXCOLUMNNUM 
%token <cmd> TBLXCOLUMNRULING
%token <cmd> TBLXFILL 
%token <cmd> TBLXROWRULING 
%token <cmd> TBLXSEPARATION
%token <cmd> TEXTFLOW
%token <cmd> TEXTLINE
%token <cmd> TEXTRECT 
%token <cmd> TFLINESPACING
%token <cmd> TFMINHANGHEIGHT
%token <cmd> TFSYNCHRONIZED
%token <cmd> TIPANGLE
%token <cmd> TLALIGNMENT
%token <cmd> TLORIGIN
%token <cmd> TRCOLUMNBALANCE 
%token <cmd> TRCOLUMNGAP 
%token <cmd> TRCOLUMNS 
%token <cmd> TRNEXT
%token <cmd> TRNUMCOLUMNS 
%token <cmd> TRSIDEHEADGAP 
%token <cmd> TRSIDEHEADPLACEMENT 
%token <cmd> TRSIDEHEADWIDTH
%token <cmd> TSDECIMALCHAR 
%token <cmd> TSLEADERSTR 
%token <cmd> TSTYPE 
%token <cmd> TSX
%token <cmd> UNIQUE 
%token <cmd> UNITS
%token <cmd> UNIT_PC
%token <cmd> UNIT_PT
%token <cmd> VARIABLEDEF 
%token <cmd> VARIABLEFORMAT 
%token <cmd> VARIABLEFORMATS 
%token <cmd> VARIABLENAME
%token <cmd> VIEW 
%token <cmd> VIEWCUTOUT 
%token <cmd> VIEWINVISIBLE 
%token <cmd> VIEWNUMBER 
%token <cmd> VIEWS
%token <cmd> XREFDEF 
%token <cmd> XREFFORMAT 
%token <cmd> XREFFORMATS 
%token <cmd> XREFNAME

%type <mif_aframe> tag_aframe
%type <mif_atbl> tag_atbl
%type <mif_char> tag_char
%type <mif_documentelement> document_element
%type <mif_dacrobatbookmarksincludetagnames> tag_dacrobatbookmarksincludetagnames 
%type <mif_dacrobatparagraphbookmarks> tag_dacrobatparagraphbookmarks
%type <mif_dautochbars> tag_dautochbars 
%type <mif_dborderson> tag_dborderson 
%type <mif_dchbarcolor> tag_dchbarcolor 
%type <mif_dchbargap> tag_dchbargap 
%type <mif_dchbarposition> tag_dchbarposition 
%type <mif_dchbarwidth> tag_dchbarwidth 
%type <mif_dcurrentview> tag_dcurrentview 
%type <mif_ddisplayoverrides> tag_ddisplayoverrides 
%type <mif_dfluid> tag_dfluid 
%type <mif_dfluidsideheads> tag_dfluidsideheads 
%type <mif_dfnoteanchorpos> tag_dfnoteanchorpos
%type <mif_dfnoteanchorprefix> tag_dfnoteanchorprefix 
%type <mif_dfnoteanchorsuffix> tag_dfnoteanchorsuffix 
%type <mif_dfnotelabels> tag_dfnotelabels 
%type <mif_dfnotemaxh> tag_dfnotemaxh
%type <mif_dfnotenumberpos> tag_dfnotenumberpos 
%type <mif_dfnotenumberprefix> tag_dfnotenumberprefix 
%type <mif_dfnotenumbersuffix> tag_dfnotenumbersuffix 
%type <mif_dfnotenumstyle> tag_dfnotenumstyle 
%type <mif_dfnoterestart> tag_dfnoterestart 
%type <mif_dfnotetag> tag_dfnotetag 
%type <mif_dfrozenpages> tag_dfrozenpages 
%type <mif_dfullrulers> tag_dfullrulers 
%type <mif_dgenerateacrobatinfo> tag_dgenerateacrobatinfo 
%type <mif_dgraphicsoff> tag_dgraphicsoff 
%type <mif_dgridon> tag_dgridon 
%type <mif_dlanguage> tag_dlanguage 
%type <mif_dlinebreakchars> tag_dlinebreakchars 
%type <mif_dlinkboundarieson> tag_dlinkboundarieson 
%type <mif_dmathalphacharfontfamily> tag_dmathalphacharfontfamily 
%type <mif_dmathcatalog> tag_dmathcatalog 
%type <mif_dmathfunctions> tag_dmathfunctions 
%type <mif_dmathgreek> tag_dmathgreek 
%type <mif_dmathlargehoriz> tag_dmathlargehoriz 
%type <mif_dmathlargeintegral> tag_dmathlargeintegral 
%type <mif_dmathlargelevel1> tag_dmathlargelevel1 
%type <mif_dmathlargelevel2> tag_dmathlargelevel2 
%type <mif_dmathlargelevel3> tag_dmathlargelevel3 
%type <mif_dmathlargesigma> tag_dmathlargesigma 
%type <mif_dmathlargevert> tag_dmathlargevert 
%type <mif_dmathmediumhoriz> tag_dmathmediumhoriz 
%type <mif_dmathmediumintegral> tag_dmathmediumintegral 
%type <mif_dmathmediumlevel1> tag_dmathmediumlevel1 
%type <mif_dmathmediumlevel2> tag_dmathmediumlevel2 
%type <mif_dmathmediumlevel3> tag_dmathmediumlevel3 
%type <mif_dmathmediumsigma> tag_dmathmediumsigma 
%type <mif_dmathmediumvert> tag_dmathmediumvert 
%type <mif_dmathnumbers> tag_dmathnumbers 
%type <mif_dmathshowcustom> tag_dmathshowcustom 
%type <mif_dmathsmallhoriz> tag_dmathsmallhoriz 
%type <mif_dmathsmallintegral> tag_dmathsmallintegral 
%type <mif_dmathsmalllevel1> tag_dmathsmalllevel1 
%type <mif_dmathsmalllevel2> tag_dmathsmalllevel2 
%type <mif_dmathsmalllevel3> tag_dmathsmalllevel3 
%type <mif_dmathsmallsigma> tag_dmathsmallsigma 
%type <mif_dmathsmallvert> tag_dmathsmallvert 
%type <mif_dmathstrings> tag_dmathstrings 
%type <mif_dmathvariables> tag_dmathvariables 
%type <mif_dmenubar> tag_dmenubar 
%type <mif_dnarrowrubispaceforjapanese> tag_dnarrowrubispaceforjapanese 
%type <mif_dnarrowrubispaceforother> tag_dnarrowrubispaceforother 
%type <mif_dnextunique> tag_dnextunique 
%type <mif_dpagegrid> tag_dpagegrid 
%type <mif_dpagenumstyle> tag_dpagenumstyle 
%type <mif_dpagepointstyle> tag_dpagepointstyle 
%type <mif_dpagerounding> tag_dpagerounding 
%type <mif_dpagescrolling> tag_dpagescrolling 
%type <mif_dpagesize> tag_dpagesize 
%type <mif_dparity> tag_dparity 
%type <mif_dprintseparations> tag_dprintseparations 
%type <mif_dprintskipblankpages> tag_dprintskipblankpages 
%type <mif_dpunctuationchars> tag_dpunctuationchars 
%type <mif_drubialignatlinebounds> tag_drubialignatlinebounds 
%type <mif_drubioverhang> tag_drubioverhang 
%type <mif_drubisize> tag_drubisize 
%type <mif_drulerson> tag_drulerson 
%type <mif_dshowallconditions> tag_dshowallconditions 
%type <mif_dsmallcapssize> tag_dsmallcapssize 
%type <mif_dsmallcapsstretch> tag_dsmallcapsstretch 
%type <mif_dsmartquoteson> tag_dsmartquoteson 
%type <mif_dsmartspaceson> tag_dsmartspaceson 
%type <mif_dsnapgrid> tag_dsnapgrid 
%type <mif_dsnaprotation> tag_dsnaprotation 
%type <mif_dstartpage> tag_dstartpage 
%type <mif_dsubscriptshift> tag_dsubscriptshift 
%type <mif_dsubscriptsize> tag_dsubscriptsize 
%type <mif_dsubscriptstretch> tag_dsubscriptstretch 
%type <mif_dsuperscriptshift> tag_dsuperscriptshift 
%type <mif_dsuperscriptsize> tag_dsuperscriptsize 
%type <mif_dsuperscriptstretch> tag_dsuperscriptstretch 
%type <mif_dsymbolson> tag_dsymbolson 
%type <mif_dtblfnoteanchorpos> tag_dtblfnoteanchorpos 
%type <mif_dtblfnoteanchorprefix> tag_dtblfnoteanchorprefix 
%type <mif_dtblfnoteanchorsuffix> tag_dtblfnoteanchorsuffix 
%type <mif_dtblfnotelabels> tag_dtblfnotelabels 
%type <mif_dtblfnotenumberpos> tag_dtblfnotenumberpos 
%type <mif_dtblfnotenumberprefix> tag_dtblfnotenumberprefix 
%type <mif_dtblfnotenumbersuffix> tag_dtblfnotenumbersuffix 
%type <mif_dtblfnotenumstyle> tag_dtblfnotenumstyle 
%type <mif_dtblfnotetag> tag_dtblfnotetag 
%type <mif_dtwosides> tag_dtwosides 
%type <mif_dupdatetextinsetsonopen> tag_dupdatetextinsetsonopen 
%type <mif_dupdatexrefsonopen> tag_dupdatexrefsonopen 
%type <mif_dviewonly> tag_dviewonly 
%type <mif_dviewonlyselect> tag_dviewonlyselect 
%type <mif_dviewonlywinborders> tag_dviewonlywinborders 
%type <mif_dviewonlywinmenubar> tag_dviewonlywinmenubar 
%type <mif_dviewonlywinpalette> tag_dviewonlywinpalette 
%type <mif_dviewonlywinpopup> tag_dviewonlywinpopup 
%type <mif_dviewonlyxref> tag_dviewonlyxref 
%type <mif_dviewrect> tag_dviewrect 
%type <mif_dviewscale> tag_dviewscale 
%type <mif_dvomenubar> tag_dvomenubar 
%type <mif_dwiderubispaceforjapanese> tag_dwiderubispaceforjapanese 
%type <mif_dwiderubispaceforother> tag_dwiderubispaceforother 
%type <mif_dwindowrect> tag_dwindowrect 
%type <mif_fangle> tag_fangle
%type <mif_fcase> tag_fcase
%type <mif_fchangebar> tag_fchangebar
%type <mif_fcolor> tag_fcolor
%type <mif_fdw> tag_fdw
%type <mif_fdx> tag_fdx
%type <mif_fdy> tag_fdy
%type <mif_fencoding> tag_fencoding
%type <mif_ffamily> tag_ffamily
%type <mif_flanguage> tag_flanguage
%type <mif_flocked> tag_flocked
%type <mif_fnoteelementlist> fnote_elements
%type <mif_fnoteelement> fnote_element
%type <mif_fnote> fnotereference
%type <mif_fnote> tag_fnote
%type <mif_fnotestartnum> tag_fnotestartnum
%type <mif_foutline> tag_foutline
%type <mif_foverline> tag_foverline
%type <mif_fpairkern> tag_fpairkern
%type <mif_fplatformname> tag_fplatformname
%type <mif_fposition> tag_fposition
%type <mif_fpostscriptname> tag_fpostscriptname
%type <mif_fseparation> tag_fseparation
%type <mif_fshadow> tag_fshadow
%type <mif_fsize> tag_fsize
%type <mif_fstretch> tag_fstretch
%type <mif_fstrike> tag_fstrike
%type <mif_ftag> tag_ftag
%type <mif_ftsume> tag_ftsume
%type <mif_funderlining> tag_funderlining
%type <mif_fvar> tag_fvar
%type <mif_fweight> tag_fweight
%type <mif_fontelementlist> font_elements
%type <mif_fontelement> font_element
%type <mif_font> tag_font
%type <mif_hyphenmaxlines> tag_hyphenmaxlines
%type <mif_hyphenminprefix> tag_hyphenminprefix
%type <mif_hyphenminsuffix> tag_hyphenminsuffix
%type <mif_hyphenminword> tag_hyphenminword
%type <mif_id> tag_frameid
%type <mif_mcurrpage> tag_mcurrpage
%type <mif_mtext> tag_mtext
%type <mif_mtypename> tag_mtypename
%type <mif_mtype> tag_mtype
%type <mif_markerelementlist> marker_elements
%type <mif_markerelement> marker_element
%type <mif_marker> tag_marker
%type <mif_noteselementlist> notes_elements
%type <mif_noteselement> notes_element
%type <mif_notes> tag_notes
%type <mif_paraelementlist> para_elements
%type <mif_paraelement> para_element
%type <mif_paralineelementlist> paraline_elements
%type <mif_paralineelement> paraline_element
%type <mif_paraline> tag_paraline
%type <mif_para> tag_para
%type <mif_pen> tag_pen
%type <mif_pgfalignment> tag_pgfalignment
%type <mif_pgfautonum> tag_pgfautonum
%type <mif_pgfblocksize> tag_pgfblocksize
%type <mif_pgfbotsepatindent> tag_pgfbotsepatindent
%type <mif_pgfbotsepoffset> tag_pgfbotsepoffset
%type <mif_pgfbotseparator> tag_pgfbotseparator
%type <mif_pgfcellalignment> tag_pgfcellalignment
%type <mif_pgfcellbmarginfixed> tag_pgfcellbmarginfixed
%type <mif_pgfcelllmarginfixed> tag_pgfcelllmarginfixed
%type <mif_pgfcellmargins> tag_pgfcellmargins
%type <mif_pgfcellrmarginfixed> tag_pgfcellrmarginfixed
%type <mif_pgfcelltmarginfixed> tag_pgfcelltmarginfixed
%type <mif_pgfelement> pgf_element
%type <mif_pgffindent> tag_pgffindent
%type <mif_pgffindentoffset> tag_pgffindentoffset
%type <mif_pgffindentrelative> tag_pgffindentrelative
%type <mif_pgffontelementlist> pgffontelements
%type <mif_pgffontelement> pgffontelement
%type <mif_pgffont> tag_pgffont
%type <mif_pgfhyphenate> tag_pgfhyphenate
%type <mif_pgflindent> tag_pgflindent
%type <mif_pgflanguage> tag_pgflanguage
%type <mif_pgfleading> tag_pgfleading
%type <mif_pgfletterspace> tag_pgfletterspace
%type <mif_pgflinespacing> tag_pgflinespacing
%type <mif_pgflocked> tag_pgflocked
%type <mif_pgfmaxjletterspace> tag_pgfmaxjletterspace
%type <mif_pgfmaxjromanletterspace> tag_pgfmaxjromanletterspace
%type <mif_pgfmaxwordspace> tag_pgfmaxwordspace
%type <mif_pgfminjletterspace> tag_pgfminjletterspace
%type <mif_pgfminjromanletterspace> tag_pgfminjromanletterspace
%type <mif_pgfminwordspace> tag_pgfminwordspace
%type <mif_pgfnexttag> tag_pgfnexttag
%type <mif_pgfnumatend> tag_pgfnumatend
%type <mif_pgfnumformat> tag_pgfnumformat
%type <mif_pgfnumstring> tag_pgfnumstring
%type <mif_pgfnumtabs> tag_pgfnumtabs
%type <mif_pgfnumberfont> tag_pgfnumberfont
%type <mif_pgfoptjletterspace> tag_pgfoptjletterspace
%type <mif_pgfoptjromanletterspace> tag_pgfoptjromanletterspace
%type <mif_pgfoptwordspace> tag_pgfoptwordspace
%type <mif_pgfplacement> tag_pgfplacement
%type <mif_pgfplacementstyle> tag_pgfplacementstyle
%type <mif_pgfrindent> tag_pgfrindent
%type <mif_pgfrunindefaultpunct> tag_pgfrunindefaultpunct
%type <mif_pgfspafter> tag_pgfspafter
%type <mif_pgfspbefore> tag_pgfspbefore
%type <mif_pgftag> tag_pgftag
%type <mif_pgftopsepatindent> tag_pgftopsepatindent
%type <mif_pgftopsepoffset> tag_pgftopsepoffset
%type <mif_pgftopseparator> tag_pgftopseparator
%type <mif_pgfusenexttag> tag_pgfusenexttag
%type <mif_pgfwithnext> tag_pgfwithnext
%type <mif_pgfwithprev> tag_pgfwithprev
%type <mif_pgfyakumonotype> tag_pgfyakumonotype
%type <mif_pgfacrobatlevel> tag_pgfacrobatlevel
%type <mif_string> tag_string
%type <mif_tfautoconnect> tag_tfautoconnect
%type <mif_tftag> tag_tftag
%type <mif_tfsynchronized> tag_tfsynchronized
%type <mif_tflinespacing> tag_tflinespacing
%type <mif_tfminhangheight> tag_tfminhangheight
%type <mif_tsdecimalchar> tag_tsdecimalchar
%type <mif_tsleaderstr> tag_tsleaderstr
%type <mif_tstype> tag_tstype
%type <mif_tsx> tag_tsx
%type <mif_tabstopelementlist> tabstop_elements
%type <mif_tabstopelement> tabstop_element
%type <mif_tabstop> tag_tabstop
%type <mif_textflowelementlist> textflow_elements
%type <mif_textflowelement> textflow_element
%type <mif_textrectid> tag_textrectid
%type <mif_unique> tag_unique
%type <mif_variabledef> tag_variabledef
%type <mif_variableelementlist> variable_elements
%type <mif_variableelement> variable_element
%type <mif_variablename> tag_variablename
%type <mif_variable> tag_variable
%type <mif_xref> tag_xref
%type <mif_xrefelementlist> xref_elements
%type <mif_xrefelement> xref_element
%type <mif_xrefend> tag_xrefend
%type <mif_xreflastupdate> tag_xreflastupdate
%type <mif_xrefname> tag_xrefname
%type <mif_xrefsrcfile> tag_xrefsrcfile
%type <mif_xrefsrciselem> tag_xrefsrciselem
%type <mif_xrefsrctext> tag_xrefsrctext
%type <mif_pgf> tag_pgf
%type <mif_pgfelementlist> pgf_elements
%type <mif_textflow> tag_textflow
%type <mif_page> tag_page
%type <mif_pageelementlist> page_elements
%type <mif_pageelement> page_element
%type <mif_pagetype> tag_pagetype
%type <mif_pagetag> tag_pagetag
%type <mif_pagesize> tag_pagesize
%type <mif_pageorientation> tag_pageorientation
%type <mif_pageangle> tag_pageangle
%type <string> unit
%type <mif_textrect> tag_textrect
%type <mif_textline> tag_textline
%type <mif_polyline> tag_polyline
%type <mif_frame> tag_frame
%type <mif_frameelementlist> frame_elements
%type <mif_frameelement> frame_element
%type <mif_penwidth> tag_penwidth
%type <mif_fill> tag_fill
%type <mif_separation> tag_separation
%type <mif_obcolor> tag_obcolor
%type <mif_dashedpattern> tag_dashedpattern
%type <mif_dashedstyle> tag_dashedstyle
%type <mif_dashedpatternelementlist> dashedpattern_elements
%type <mif_dashedpatternelement> dashedpattern_element
%type <mif_angle> tag_angle
%type <mif_shaperect> tag_shaperect
%type <mif_brect> tag_brect
%type <mif_frametype> tag_frametype
%type <mif_nsoffset> tag_nsoffset
%type <mif_bloffset> tag_bloffset
%type <mif_anchoralign> tag_anchoralign
%type <mif_cropped> tag_cropped
%type <mif_importobject> tag_importobject
%type <mif_tag> tag_tag
%type <mif_runaroundgap> tag_runaroundgap
%type <mif_runaroundtype> tag_runaroundtype
%type <mif_math> tag_math
%type <mif_numsegments> tag_numsegments
%type <mif_dashsegment> tag_dashsegment
%type <mif_pagenum> tag_pagenum
%type <mif_importobjectelementlist> importobject_elements
%type <mif_importobjectelement> importobject_element
%type <mif_importobfile> tag_importobfile
%type <mif_fliplr> tag_fliplr
%type <mif_nativeorigin> tag_nativeorigin
%type <mif_bitmapdpi> tag_bitmapdpi
%type <mif_externaldata> externaldata
%type <mif_pagebackground> tag_pagebackground
%type <mif_mathelementlist> math_elements
%type <mif_mathelement> math_element
%type <mif_mathfullform> tag_mathfullform
%type <mif_mathlinebreak> tag_mathlinebreak
%type <mif_mathorigin> tag_mathorigin
%type <mif_mathalignment> tag_mathalignment
%type <mif_mathsize> tag_mathsize
%type <mif_rectangle> tag_rectangle
%type <mif_polygon> tag_polygon
%type <mif_group> tag_group
%type <mif_ellipse> tag_ellipse
%type <mif_rectangleelementlist> rectangle_elements
%type <mif_rectangleelement> rectangle_element
%type <mif_groupid> tag_groupid
%type <mif_textrectelementlist> textrect_elements
%type <mif_textrectelement> textrect_element
%type <mif_trnumcolumns> tag_trnumcolumns
%type <mif_trcolumngap> tag_trcolumngap
%type <mif_trcolumnbalance> tag_trcolumnbalance
%type <mif_trsideheadgap> tag_trsideheadgap
%type <mif_trsideheadwidth> tag_trsideheadwidth
%type <mif_trsideheadplacement> tag_trsideheadplacement
%type <mif_trnext> tag_trnext
%type <mif_textlineelementlist> textline_elements
%type <mif_textlineelement> textline_element
%type <mif_tlorigin> tag_tlorigin
%type <mif_tlalignment> tag_tlalignment
%type <mif_polylineelementlist> polyline_elements
%type <mif_polylineelement> polyline_element
%type <mif_headcap> tag_headcap
%type <mif_tailcap> tag_tailcap
%type <mif_arrowstyle> tag_arrowstyle
%type <mif_arrowstyleelementlist> arrowstyle_elements
%type <mif_arrowstyleelement> arrowstyle_element
%type <mif_numpoints> tag_numpoints
%type <mif_point> tag_point
%type <mif_tipangle> tag_tipangle
%type <mif_baseangle> tag_baseangle
%type <mif_length> tag_length
%type <mif_headtype> tag_headtype
%type <mif_scalehead> tag_scalehead
%type <mif_scalefactor> tag_scalefactor
%type <mif_polygonelementlist> polygon_elements
%type <mif_polygonelement> polygon_element
%type <mif_ellipseelementlist> ellipse_elements
%type <mif_ellipseelement> ellipse_element
%type <mif_groupelementlist> group_elements
%type <mif_groupelement> group_element



%left '+' '-' '*' '/' UMINUS

%start mif_file

%%

/* A MIF file consists of one or more tags */
mif_file:			tag
				|	mif_file tag
				;

/* A toplevel tag is one of the following */
tag:				tag_aframes
				|	tag_bookcomponent
				|   tag_charunits
				|	tag_colorcatalog
				|	tag_combinedfontcatalog
				|	tag_conditioncatalog
				|	tag_dictionary
				|	tag_document
				|	tag_fontcatalog
				|	tag_initialautonums
				|	tag_kumihancatalog
				|	tag_markertypecatalog
				|	tag_miffile
				|	tag_page
					{ pages.push_front( $1 ); }
				|	tag_pgfcatalog
				|	tag_rulingcatalog
				|	tag_tblcatalog
				|	tag_tbls
				|	tag_textflow
					{ textflows.push_front( $1 ); }
				|	tag_units
				|	tag_variableformats	
				|	tag_views
				|	tag_xrefformats
				;

/*****
 * KumihanCatalog-related stuff
 *****/
tag_kumihancatalog:		'<' KUMIHANCATALOG '>'
;

/******************************************
 * Color-related stuff
 ******************************************/

/* Tag: Color */
tag_color:			'<' COLOR color_elements '>'
				;

/* Elements of a Color tag */
color_elements:		color_element
				|	color_elements color_element
				;

color_element:		tag_colortag
				|	tag_coloryellow
				|	tag_colorcyan
				|	tag_colormagenta
				|	tag_colorblack
				|	tag_colorattribute
	            |   tag_colorlibraryfamilyname
	            |   tag_colorlibraryinkname
				;

/* Tag: ColorTag */
tag_colortag:		'<' COLORTAG SQSTRING '>'
				;

tag_coloryellow:	'<' COLORYELLOW REAL '>'
				;

tag_colorcyan:	'<' COLORCYAN REAL '>'
				;

tag_colormagenta:	'<' COLORMAGENTA REAL '>'
				;

tag_colorblack:	'<' COLORBLACK REAL '>'
				;

tag_colorattribute:	'<' COLORATTRIBUTE ID '>'
				;

tag_colorlibraryfamilyname: '<' COLORLIBRARYFAMILYNAME SQSTRING '>'
	;

tag_colorlibraryinkname: '<' COLORLIBRARYINKNAME SQSTRING '>'
	;

/* Tag: ColorCatalog: a sequence of colors */
tag_colorcatalog:	'<' COLORCATALOG colorcatalog_elements '>'
				;

/* Elements of a ColorCatalog: a sequence of colors */
colorcatalog_elements:	tag_color
					|	colorcatalog_elements tag_color
					;

/****************************************
 * condition-related stuff
 ****************************************/

/* Tag: ConditionCatalog: a sequence of conditions */
tag_conditioncatalog:	'<' CONDITIONCATALOG conditioncatalog_elements '>'
					;

/* Elements of a ConditionCatalog: a sequence of conditions */
conditioncatalog_elements:	tag_condition
						|	conditioncatalog_elements tag_condition
						;

/* Tag: Condition */
tag_condition:			'<' CONDITION condition_elements '>'
				;

/* Elements of a Condition tag */
condition_elements:		condition_element
				|	condition_elements condition_element
				;

condition_element:		tag_ctag
				|	tag_cstate
				|	tag_cstyle
				|	tag_cseparation
				|	tag_ccolor
				;

/* Tag: CTag */
tag_ctag:				'<' CTAG SQSTRING '>'
				;

/* Tag: CStyle */
tag_cstyle:				'<' CSTYLE ID '>'
				;

/* Tag: CState */
tag_cstate:				'<' CSTATE ID '>'
					;

/* Tag: CSeparation */
tag_cseparation:		'<' CSEPARATION INTEGER '>'
						;

/* Tag: CColor */
tag_ccolor:				'<' CCOLOR SQSTRING '>'
						;

/***********************************************
 * PGF-related stuff
 ***********************************************/

/* Tag: PgfCatalog: a sequence of pgfs */
tag_pgfcatalog:	'<' PGFCATALOG pgfcatalog_elements '>'
					;

/* Elements of a PgfCatalog: a sequence of pgfs */
pgfcatalog_elements:	tag_pgf
					|	pgfcatalog_elements tag_pgf
						;

/* Tag: Pgf */
tag_pgf:			'<' PGF pgf_elements '>' 
					{ $$ = new Pgf( $3 ); }
				;

/* Elements of a Pgf tag */
pgf_elements:		pgf_element
					{	PgfElementList* list = new PgfElementList;
						list->push_front( $1 ); $$ = list; }
				|	pgf_element pgf_elements
					{	$2->push_front( $1 ); $$ = $2; }
				;

pgf_element:			tag_pgftag
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfusenexttag
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfnexttag
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfalignment
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgffindent
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgffindentoffset
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgffindentrelative
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgflindent
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfrindent
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgftopseparator
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgftopsepatindent
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgftopsepoffset
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfbotseparator
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfbotsepatindent
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfbotsepoffset
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfplacement
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfplacementstyle
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfrunindefaultpunct
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfspbefore
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfspafter
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfwithprev
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfwithnext
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfblocksize
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgffont
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgflinespacing
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfleading
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfautonum
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfnumtabs
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfnumformat
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfnumberfont
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfnumatend
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfhyphenate
						{ $$ = new PgfElement( $1 ); }
					|	tag_hyphenmaxlines
						{ $$ = new PgfElement( $1 ); }
					|	tag_hyphenminprefix
						{ $$ = new PgfElement( $1 ); }
					|	tag_hyphenminsuffix
						{ $$ = new PgfElement( $1 ); }
					|	tag_hyphenminword
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfletterspace
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfminwordspace
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfmaxwordspace
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfoptwordspace
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgflanguage
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfcellalignment
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfminjromanletterspace
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfoptjromanletterspace
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfmaxjromanletterspace
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfminjletterspace
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfoptjletterspace
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfmaxjletterspace
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfyakumonotype
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfacrobatlevel
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfcellmargins
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfcelllmarginfixed
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfcellrmarginfixed
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfcelltmarginfixed
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgfcellbmarginfixed
						{ $$ = new PgfElement( $1 ); }
					|	tag_pgflocked
						{ $$ = new PgfElement( $1 ); }
					|	tag_tabstop
						{ $$ = new PgfElement( $1 ); } ;

tag_pgftag:				'<' PGFTAG SQSTRING '>'
						{ $$ = new PgfTag( $3 ); }
					;

tag_pgfnumformat:		'<' PGFNUMFORMAT SQSTRING '>'
						{ $$ = new PgfNumFormat( $3 ); }
					;


tag_pgfusenexttag:		'<' PGFUSENEXTTAG ID '>'
						{ $$ = new PgfUseNextTag( $3 ); }
					;

tag_pgfnexttag:			'<' PGFNEXTTAG SQSTRING '>'
						{ $$ = new PgfNextTag( $3 ); }
					;

tag_pgfalignment:		'<' PGFALIGNMENT ID '>'
						{ $$ = new PgfAlignment( $3 ); }
					;

tag_pgffindent:			'<' PGFFINDENT REAL unit '>'
						{ $$ = new PgfFIndent( $3, $4 ); }
;

tag_pgflindent:			'<' PGFLINDENT REAL unit '>'
						{ $$ = new PgfLIndent( $3, $4 ); }
					;

tag_pgfrindent:			'<' PGFRINDENT REAL unit '>'
						{ $$ = new PgfRIndent( $3, $4 ); }
					;

tag_pgffindentrelative:	'<' PGFFINDENTRELATIVE ID '>'
						{ $$ = new PgfFIndentRelative( $3 ); }
;

tag_pgffindentoffset:    '<' PGFFINDENTOFFSET REAL unit '>'
						{ $$ = new PgfFIndentOffset( $3, $4 ); }
;

tag_pgftopseparator:	'<' PGFTOPSEPARATOR SQSTRING '>'
						{ $$ = new PgfTopSeparator( $3 ); }
					;
		
tag_pgftopsepatindent:	'<' PGFTOPSEPATINDENT ID '>'
						{ $$ = new PgfTopSepAtIndent( $3 ); }
;

tag_pgftopsepoffset:	'<' PGFTOPSEPOFFSET REAL unit '>'
						{ $$ = new PgfTopSepOffset( $3, $4 ); }
;

tag_pgfbotseparator:	'<' PGFBOTSEPARATOR SQSTRING '>'
						{ $$ = new PgfBotSeparator( $3 ); }
					;
		
tag_pgfbotsepatindent:	'<' PGFBOTSEPATINDENT ID '>'
						{ $$ = new PgfBotSepAtIndent( $3 ); }
;

tag_pgfbotsepoffset:	'<' PGFBOTSEPOFFSET REAL unit '>'
						{ $$ = new PgfBotSepOffset( $3, $4 ); }
;

tag_pgfplacement:			'<' PGFPLACEMENT ID '>'
							{ $$ = new PgfPlacement( $3 ); }
					;

tag_pgfplacementstyle:		'<' PGFPLACEMENTSTYLE ID '>'
							{ $$ = new PgfPlacementStyle( $3 ); }
					;

tag_pgfrunindefaultpunct:		'<'	PGFRUNINDEFAULTPUNCT SQSTRING '>'
								{ $$ = new PgfRunInDefaultPunct( $3 ); }
								;

tag_pgfspbefore:		'<' PGFSPBEFORE REAL unit '>'
						{ $$ = new PgfSpBefore( $3, $4 ); }
						;

tag_pgfspafter:		'<' PGFSPAFTER REAL unit '>'
						{ $$ = new PgfSpAfter( $3, $4 ); }
						;

tag_pgfwithprev:	'<' PGFWITHPREV ID '>'
						{ $$ = new PgfWithPrev( $3 ); }
						;

tag_pgfwithnext:		'<' PGFWITHNEXT ID '>'
						{ $$ = new PgfWithNext( $3 ); }
						;

tag_pgfblocksize:		'<' PGFBLOCKSIZE INTEGER '>'
						{ $$ = new PgfBlockSize( $3 ); }
						;

tag_pgffont:			'<' PGFFONT pgffontelements '>'
						{ $$ = new PgfFont( $3 ); }
						;

tag_pgflinespacing:		'<' PGFLINESPACING ID '>'
						{ $$ = new PgfLineSpacing( $3 ); }
						;

tag_pgfleading:			'<'	PGFLEADING REAL unit '>'
						{ $$ = new PgfLeading( $3, $4 ); }
						;

tag_pgfautonum:			'<' PGFAUTONUM ID '>'
						{ $$ = new PgfAutoNum( $3 ); }
						;

tag_pgfnumtabs:			'<' PGFNUMTABS INTEGER '>'
						{ $$ = new PgfNumTabs( $3 ); }
						;

tag_pgfnumberfont:		'<' PGFNUMBERFONT SQSTRING '>'
						{ $$ = new PgfNumberFont( $3 ); }
						;

tag_pgfnumatend:		'<' PGFNUMATEND ID '>'
						{ $$ = new PgfNumAtEnd( $3 ); }
						;

tag_pgfhyphenate:		'<' PGFHYPHENATE ID '>'
						{ $$ = new PgfHyphenate( $3 ); }
						;

tag_hyphenmaxlines:		'<' HYPHENMAXLINES INTEGER '>'
						{ $$ = new HyphenMaxLines( $3 ); }
						;

tag_hyphenminprefix:	'<' HYPHENMINPREFIX INTEGER '>'
						{ $$ = new HyphenMinPrefix( $3 ); }
						;

tag_hyphenminsuffix:	'<'	HYPHENMINSUFFIX INTEGER '>'
						{ $$ = new HyphenMinSuffix( $3 ); }
						;

tag_hyphenminword:		'<' HYPHENMINWORD INTEGER '>'
						{ $$ = new HyphenMinWord( $3 ); }
						;

tag_pgfletterspace:		'<' PGFLETTERSPACE ID '>'
						{ $$ = new PgfLetterSpace( $3 ); }
						;

tag_pgfminwordspace:	'<' PGFMINWORDSPACE INTEGER '>'
						{ $$ = new PgfMinWordSpace( $3 ); }
						;

tag_pgfmaxwordspace:	'<' PGFMAXWORDSPACE INTEGER '>'
						{ $$ = new PgfMaxWordSpace( $3 ); }
						;

tag_pgfoptwordspace:	'<' PGFOPTWORDSPACE INTEGER '>'
						{ $$ = new PgfOptWordSpace( $3 ); }
						;

tag_pgflanguage:		'<' PGFLANGUAGE ID '>'
						{ $$ = new PgfLanguage( $3 ); }
						;

tag_pgfcellalignment:	'<' PGFCELLALIGNMENT ID '>'
						{ $$ = new PgfCellAlignment( $3 ); }
						;

tag_pgfcellmargins:		'<' PGFCELLMARGINS REAL unit REAL unit REAL unit REAL unit '>'
						{ $$ = new PgfCellMargins( $3, $4, $5, $6, $7, $8, $9, $10 ); }
						;

tag_pgfcelllmarginfixed:		'<' PGFCELLLMARGINFIXED ID '>'
						{ $$ = new PgfCellLMarginFixed( $3 ); }
						;

tag_pgfcellrmarginfixed:		'<' PGFCELLRMARGINFIXED ID '>'
						{ $$ = new PgfCellRMarginFixed( $3 ); }
						;

tag_pgfcelltmarginfixed:		'<' PGFCELLTMARGINFIXED ID '>'
						{ $$ = new PgfCellTMarginFixed( $3 ); }
						;

tag_pgfcellbmarginfixed:		'<' PGFCELLBMARGINFIXED ID '>'
						{ $$ = new PgfCellBMarginFixed( $3 ); }
						;

tag_pgfminjromanletterspace:	'<' PGFMINJROMANLETTERSPACE INTEGER '>'
						{ $$ = new PgfMinJRomanLetterSpace( $3 ); }
;

tag_pgfoptjromanletterspace:	'<' PGFOPTJROMANLETTERSPACE INTEGER '>'
						{ $$ = new PgfOptJRomanLetterSpace( $3 ); }
;

tag_pgfmaxjromanletterspace:	'<' PGFMAXJROMANLETTERSPACE INTEGER '>'
						{ $$ = new PgfMaxJRomanLetterSpace( $3 ); }
;

tag_pgfminjletterspace:		'<' PGFMINJLETTERSPACE INTEGER '>'
						{ $$ = new PgfMinJLetterSpace( $3 ); }
;

tag_pgfoptjletterspace:		'<' PGFOPTJLETTERSPACE INTEGER '>'
						{ $$ = new PgfOptJLetterSpace( $3 ); }
;

tag_pgfmaxjletterspace:		'<' PGFMAXJLETTERSPACE INTEGER '>'
						{ $$ = new PgfMaxJLetterSpace( $3 ); }
;

tag_pgfyakumonotype:	'<' PGFYAKUMONOTYPE ID '>'
						{ $$ = new PgfYakumonoType( $3 ); }
;

tag_pgfacrobatlevel:	'<' PGFACROBATLEVEL INTEGER '>'
						{ $$ = new PgfAcrobatLevel( $3 ); }
;

tag_pgflocked:	'<' PGFLOCKED ID '>'
						{ $$ = new PgfLocked( $3 ); }
;

tag_pgfnumstring:		'<' PGFNUMSTRING SQSTRING '>'
						{ $$ = new PgfNumString( $3 ); }
;

pgffontelements:		pgffontelement
						{ PgfFontElementList* list = new PgfFontElementList; 
						  list->push_front( $1 ); $$ = list; }				
				|		pgffontelement pgffontelements
						{ $2->push_front( $1 ); $$ = $2; }
						;

pgffontelement:			tag_ftag
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_fpostscriptname
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_fplatformname
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_ffamily
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_fvar
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_fweight
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_fangle
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_fencoding
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_fsize
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_funderlining
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_foverline
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_fstrike
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_fchangebar
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_foutline
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_fshadow
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_fpairkern
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_ftsume
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_fcase
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_fposition
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_fdx
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_fdy
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_fdw
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_fstretch
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_flanguage
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_flocked
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_fseparation
						{ $$ = new PgfFontElement( $1 ); }
				|		tag_fcolor
						{ $$ = new PgfFontElement( $1 ); }
				;

tag_ftag:		'<' FTAG SQSTRING '>'
				{	$$ = new FTag( $3 ); }
				;

tag_fpostscriptname:	'<' FPOSTSCRIPTNAME SQSTRING '>'
						{ $$ = new FPostScriptName( $3 ); }
						;

tag_fplatformname:		'<' FPLATFORMNAME SQSTRING '>'
						{ $$ = new FPlatformName( $3 ); }
				;

tag_ffamily:	'<' FFAMILY SQSTRING '>'
						{ $$ = new FFamily( $3 ); }
				;

tag_fvar:	'<' FVAR SQSTRING '>'
						{ $$ = new FVar( $3 ); }
				;

tag_fweight:	'<' FWEIGHT SQSTRING '>'
						{ $$ = new FWeight( $3 ); }
				;

tag_fangle:	'<' FANGLE SQSTRING '>'
						{ $$ = new FAngle( $3 ); }
				;

tag_fencoding:	'<' FENCODING SQSTRING '>'
						{ $$ = new FEncoding( $3 ); }
				;

tag_fsize:		'<' FSIZE REAL unit '>'
						{ $$ = new FSize( $3, $4 ); }
				;

tag_funderlining:		'<' FUNDERLINING ID '>'
						{ $$ = new FUnderlining( $3 ); }
				;

tag_foverline:		'<' FOVERLINE ID '>'
						{ $$ = new FOverline( $3 ); }
				;

tag_fstrike:		'<' FSTRIKE ID '>'
						{ $$ = new FStrike( $3 ); }
				;

tag_fchangebar:		'<' FCHANGEBAR ID '>'
						{ $$ = new FChangeBar( $3 ); }
				;

tag_foutline:		'<' FOUTLINE ID '>'
						{ $$ = new FOutline( $3 ); }
				;

tag_fshadow:		'<' FSHADOW ID '>'
						{ $$ = new FShadow( $3 ); }
				;

tag_fpairkern:		'<' FPAIRKERN ID '>'
						{ $$ = new FPairKern( $3 ); }
				;

tag_ftsume:		'<' FTSUME ID '>'
						{ $$ = new FTsume( $3 ); }
				;

tag_fcase:		'<' FCASE ID '>'
						{ $$ = new FCase( $3 ); }
				;

tag_fposition:		'<' FPOSITION ID '>'
						{ $$ = new FPosition( $3 ); }
				;

tag_fdx:		'<' FDX REAL  '%' '>'
						{ $$ = new FDX_( $3 ); }
				;

tag_fdy:		'<' FDY REAL '%' '>'
						{ $$ = new FDY_( $3 ); }
				;

tag_fdw:		'<' FDW REAL '%' '>'
						{ $$ = new FDW_( $3 ); }
				;

tag_fstretch:		'<' FSTRETCH REAL '%' '>'
						{ $$ = new FStretch( $3 ); }
				;

tag_flanguage:	'<' FLANGUAGE ID '>'
						{ $$ = new FLanguage( $3 ); }
				;

tag_flocked:	'<' FLOCKED ID '>'
						{ $$ = new FLocked( $3 ); }
				;

tag_fseparation:		'<' FSEPARATION INTEGER '>'
						{ $$ = new FSeparation( $3 ); }
				;

tag_fcolor:		'<' FCOLOR SQSTRING '>'
						{ $$ = new FColor( $3 ); }
				;


tag_tabstop:	'<' TABSTOP tabstop_elements '>'
				{ $$ = new TabStop( $3 ); }
				;

tabstop_elements:		tabstop_element
						{ TabStopElementList* list = new TabStopElementList;
						  list->push_front( $1 ); $$ = list; }
					|	tabstop_element tabstop_elements
						{ $2->push_front( $1 ); $$ = $2; }
						;

tabstop_element:		tag_tsx
						{ $$ = new TabStopElement( $1 ); }
					|	tag_tstype
						{ $$ = new TabStopElement( $1 ); }
					|	tag_tsleaderstr
						{ $$ = new TabStopElement( $1 ); }
					|	tag_tsdecimalchar
						{ $$ = new TabStopElement( $1 ); }
					;

tag_tsx:		'<' TSX REAL unit  '>'
				{ $$ = new TSX_( $3, $4 ); }

tag_tstype:		'<' TSTYPE ID '>'
				{ $$ = new TSType( $3 ); }
				;

tag_tsleaderstr:	'<' TSLEADERSTR SQSTRING '>'
					{ $$ = new TSLeaderStr( $3 ); }
				;

tag_tsdecimalchar:		'<' TSDECIMALCHAR INTEGER '>'
						{ $$ = new TSDecimalChar( $3 ); }
						;


/***********************************************
 * Font-related stuff
 ***********************************************/

/* Tag: CombinedFontCatalog. PENDING(kalle) This is not complete. */
tag_combinedfontcatalog: '<' COMBINEDFONTCATALOG '>'
;

/* Tag: FontCatalog: a sequence of fonts */
tag_fontcatalog:	'<' FONTCATALOG fontcatalog_elements '>'
					;

/* Elements of a FontCatalog: a sequence of fonts */
fontcatalog_elements:	tag_font
					|	fontcatalog_elements tag_font
						;

/* Tag: Font */
tag_font:			'<' FONT font_elements '>'
					{ $$ = new Font( $3 ); }
				;

/* Elements of a Font tag */
font_elements:		font_element
					{ FontElementList* list = new FontElementList;
					  list->push_front( $1 ); $$ = list; }
				|	font_element font_elements
					{ $2->push_front( $1 ); $$ = $2; }
				;

font_element:			tag_ftag
						{ $$ = new FontElement( $1 ); }
				|		tag_fpostscriptname
							{ $$ = new FontElement( $1 ); }
			|		tag_fplatformname
						{ $$ = new FontElement( $1 ); }
				|		tag_ffamily
						{ $$ = new FontElement( $1 ); }
				|		tag_fvar
						{ $$ = new FontElement( $1 ); }
				|		tag_fweight
						{ $$ = new FontElement( $1 ); }
				|		tag_fangle
						{ $$ = new FontElement( $1 ); }
				|		tag_fencoding
						{ $$ = new FontElement( $1 ); }
				|		tag_fsize
						{ $$ = new FontElement( $1 ); }
				|		tag_funderlining
						{ $$ = new FontElement( $1 ); }
				|		tag_foverline
						{ $$ = new FontElement( $1 ); }
				|		tag_fstrike
						{ $$ = new FontElement( $1 ); }
				|		tag_fchangebar
						{ $$ = new FontElement( $1 ); }
				|		tag_foutline
						{ $$ = new FontElement( $1 ); }
				|		tag_fshadow
						{ $$ = new FontElement( $1 ); }
				|		tag_fpairkern
						{ $$ = new FontElement( $1 ); }
				|		tag_ftsume
						{ $$ = new FontElement( $1 ); }
				|		tag_fcase
						{ $$ = new FontElement( $1 ); }
				|		tag_fposition
						{ $$ = new FontElement( $1 ); }
				|		tag_fdx
						{ $$ = new FontElement( $1 ); }
				|		tag_fdy
						{ $$ = new FontElement( $1 ); }
				|		tag_fdw
						{ $$ = new FontElement( $1 ); }
				|		tag_fstretch
						{ $$ = new FontElement( $1 ); }
				|		tag_flanguage
						{ $$ = new FontElement( $1 ); }
				|		tag_flocked
						{ $$ = new FontElement( $1 ); }
				|		tag_fseparation
						{ $$ = new FontElement( $1 ); }
				|		tag_fcolor
						{ $$ = new FontElement( $1 ); }
				;


/***********************************************
 * Ruling-related stuff
 ***********************************************/

/* Tag: RulingCatalog: a sequence of rulings */
tag_rulingcatalog:	'<' RULINGCATALOG rulingcatalog_elements '>'
					;

/* Elements of a RulingCatalog: a sequence of ruling */
rulingcatalog_elements:	tag_ruling
					|	rulingcatalog_elements tag_ruling
						;

/* Tag: Ruling */
tag_ruling:			'<' RULING ruling_elements '>'
				;

/* Elements of a Ruling tag */
ruling_elements:		ruling_element
				|	ruling_elements ruling_element
				;

/* The various Ruling elements */
ruling_element:			tag_rulingtag
				|		tag_rulingpenwidth
				|		tag_rulinggap
				|		tag_rulingseparation
				|		tag_rulingcolor
				|		tag_rulingpen
				|		tag_rulinglines
				;

tag_rulingtag:	'<' RULINGTAG SQSTRING '>'
				;

tag_rulingpenwidth:		'<' RULINGPENWIDTH REAL unit '>'
						;
		
tag_rulinggap:			'<' RULINGGAP REAL unit '>'
						;

tag_rulingseparation:	'<' RULINGSEPARATION INTEGER '>'
						;

tag_rulingcolor:		'<' RULINGCOLOR SQSTRING '>'
						;

tag_rulinglines:		'<' RULINGLINES INTEGER '>'
						;

tag_rulingpen:		'<' RULINGPEN INTEGER '>'
						;


/***********************************************
 * Table-related stuff
 ***********************************************/

/* Tag: TblCatalog: a sequence of tables */
tag_tblcatalog:	'<' TBLCATALOG tblcatalog_elements '>'
					;

/* Elements of a TblCatalog: a sequence of tables */
tblcatalog_elements:	tag_tblformat
					|	tblcatalog_elements tag_tblformat
						;

/* Tag: TblFormat */
tag_tblformat:			'<' TBLFORMAT tblformat_elements '>'
				;

/* Elements of a TblFormat tag */
tblformat_elements:		tblformat_element
				|	tblformat_elements tblformat_element
				;

tblformat_element:				tag_tbltag
						|		tag_tblcolumn
						|		tag_tblcellmargins
						|		tag_tbllindent
						|		tag_tblrindent
						|		tag_tblalignment
						|		tag_tblplacement
						|		tag_tblspbefore
						|		tag_tblspafter
						|		tag_tblblocksize
						|		tag_tblhffill
						|		tag_tblhfseparation
						|		tag_tblhfcolor
						|		tag_tblbodyfill
						|		tag_tblbodyseparation
						|		tag_tblbodycolor
						|		tag_tblshadebycolumn
						|		tag_tbllocked
						|		tag_tblshadeperiod
						|		tag_tblxfill
						|		tag_tblxseparation
						|		tag_tblxcolor
						|		tag_tblaltshadeperiod
						|		tag_tbllruling
						|		tag_tblrruling
						|		tag_tblbruling
						|		tag_tbltruling
						|		tag_tblcolumnruling
						|		tag_tblxcolumnruling
						|		tag_tblbodyrowruling
						|		tag_tblxrowruling
						|		tag_tblhfrowruling
						|		tag_tblseparatorruling
						|		tag_tblxcolumnnum
						|		tag_tblrulingperiod
						|		tag_tbllastbruling
						|		tag_tbltitleplacement
						|		tag_tbltitlepgf1
						|		tag_tbltitlegap
						|		tag_tblinitnumcolumns
						|		tag_tblinitnumhrows
						|		tag_tblinitnumbodyrows
						|		tag_tblinitnumfrows
						|		tag_tblnumbycolumn
						;

tag_tbltag:		'<' TBLTAG SQSTRING '>'
				;

tag_tblcolumn:	'<' TBLCOLUMN tblcolumn_elements '>'
				;

tag_tblcellmargins:		'<' TBLCELLMARGINS REAL unit REAL unit REAL unit REAL unit '>'
				;

tag_tbllindent:	'<'	TBLLINDENT REAL unit '>'
				;

tag_tblrindent:	'<'	TBLRINDENT REAL unit '>'
				;

tag_tblalignment: '<' TBLALIGNMENT ID '>'
				;

tag_tblplacement: '<' TBLPLACEMENT ID '>'
				;

tag_tblspbefore: '<' TBLSPBEFORE REAL unit '>'
				;

tag_tblspafter: '<' TBLSPAFTER REAL unit '>'
				;

tag_tblblocksize: '<' TBLBLOCKSIZE INTEGER '>'
				;

tag_tblhffill:	'<' TBLHFFILL INTEGER '>'
				;

tag_tblhfseparation:	'<' TBLHFSEPARATION INTEGER '>'
				;

tag_tblhfcolor:	'<' TBLHFCOLOR SQSTRING '>'
				;

tag_tblbodyfill: '<' TBLBODYFILL INTEGER '>'
				;

tag_tblbodyseparation: '<' TBLBODYSEPARATION INTEGER '>'
				;

tag_tblbodycolor:		'<' TBLBODYCOLOR SQSTRING '>'
				;

tag_tblshadebycolumn:	'<' TBLSHADEBYCOLUMN ID '>'
				;

tag_tbllocked:	'<' TBLLOCKED ID '>'
				;

tag_tblshadeperiod:		'<' TBLSHADEPERIOD INTEGER '>'
				;

tag_tblxfill:	'<' TBLXFILL INTEGER '>'
				;

tag_tblxseparation:		'<' TBLXSEPARATION INTEGER '>'
				;

tag_tblxcolor:	'<' TBLXCOLOR SQSTRING '>'
				;

tag_tblaltshadeperiod:	'<' TBLALTSHADEPERIOD INTEGER '>'
						;

tag_tbllruling:			'<' TBLLRULING SQSTRING '>'
						;

tag_tblrruling:			'<' TBLRRULING SQSTRING '>'
						;

tag_tbltruling:			'<' TBLTRULING SQSTRING '>'
						;

tag_tblbruling:			'<' TBLBRULING SQSTRING '>'
						;

tag_tblcolumnruling:	'<'	TBLCOLUMNRULING SQSTRING '>'
						;

tag_tblxcolumnruling:	'<'	TBLXCOLUMNRULING SQSTRING '>'
						;

tag_tblbodyrowruling:	'<'	TBLBODYROWRULING SQSTRING '>'
						;

tag_tblxrowruling:	'<'	TBLXROWRULING SQSTRING '>'
						;

tag_tblhfrowruling:	'<'	TBLHFROWRULING SQSTRING '>'
						;

tag_tblseparatorruling:	'<'	TBLSEPARATORRULING SQSTRING '>'
						;

tag_tblxcolumnnum:		'<' TBLXCOLUMNNUM INTEGER '>'
						;

tag_tblrulingperiod:	'<' TBLRULINGPERIOD INTEGER '>'
						;

tag_tbllastbruling:		'<' TBLLASTBRULING ID '>'
						;

tag_tbltitleplacement:	'<' TBLTITLEPLACEMENT ID '>'
						;

tag_tbltitlepgf1:		'<' TBLTITLEPGF1 tblcolumnhfbody_elements '>'
						;

tag_tbltitlegap:		'<' TBLTITLEGAP REAL unit '>'
						;

tag_tblinitnumcolumns:	'<' TBLINITNUMCOLUMNS INTEGER '>'
						;

tag_tblinitnumhrows:	'<' TBLINITNUMHROWS INTEGER '>'
						;

tag_tblinitnumbodyrows:	'<' TBLINITNUMBODYROWS INTEGER '>'
						;

tag_tblinitnumfrows:	'<' TBLINITNUMFROWS INTEGER '>'
						;

tag_tblnumbycolumn:		'<' TBLNUMBYCOLUMN ID '>'
						;

tblcolumn_elements:		tblcolumn_element
					|	tblcolumn_elements tblcolumn_element
					;

tblcolumn_element:		tag_tblcolumnnum
					|	tag_tblcolumnwidth
					|	tag_tblcolumnh
					|	tag_tblcolumnbody
					|	tag_tblcolumnf
					;

tag_tblcolumnnum:		'<' TBLCOLUMNNUM INTEGER '>'
						;

tag_tblcolumnwidth:		'<' TBLCOLUMNWIDTH REAL unit '>'
						;

tag_tblcolumnh:			'<' TBLCOLUMNH tblcolumnhfbody_elements '>'
						;

tag_tblcolumnbody:		'<' TBLCOLUMNBODY tblcolumnhfbody_elements '>'
						;

tag_tblcolumnf:			'<' TBLCOLUMNF tblcolumnhfbody_elements '>'
						;

tblcolumnhfbody_elements:	tblcolumnhfbody_element
					|	tblcolumnhfbody_elements tblcolumnhfbody_elements
					;

tblcolumnhfbody_element:				tag_pgftag
						|		tag_pgf
						|		/* nothing */
						;


/***********************************************
 * View-related stuff
 ***********************************************/

/* Tag: Views: a sequence of views */
tag_views:	'<' VIEWS views_elements '>'
					;

/* Elements of a Views: a sequence of views */
views_elements:	tag_view
					|	views_elements tag_view
						;

/* Tag: View */
tag_view:			'<' VIEW view_elements '>'
				;

/* Elements of a View tag */
view_elements:		view_element
				|	view_elements view_element
				;

view_element:		tag_viewnumber
				|	tag_viewinvisible
				|	tag_viewcutout
				;

tag_viewnumber:	'<' VIEWNUMBER INTEGER '>'
				;

tag_viewcutout:	'<' VIEWCUTOUT SQSTRING '>'
				;

tag_viewinvisible:		'<' VIEWINVISIBLE SQSTRING '>'
						;


/***********************************************
 * Variable-related stuff
 ***********************************************/

/* Tag: VariableFormats: a sequence of variableformats */
tag_variableformats:	'<' VARIABLEFORMATS variableformats_elements '>'
					;

/* Elements of a VariableFormats: a sequence of variableformats */
variableformats_elements:	tag_variableformat
					|	variableformats_elements tag_variableformat
						;

/* Tag: VariableFormat */
tag_variableformat:			'<' VARIABLEFORMAT variableformat_elements '>'
				;

/* Elements of a VariableFormat tag */
variableformat_elements:		variableformat_element
				|	variableformat_elements variableformat_element
				;

variableformat_element:			tag_variablename
						|		tag_variabledef
						;

tag_variablename:		'<' VARIABLENAME SQSTRING '>'
						{ $$ = new VariableName( $3 ); }
						;

tag_variabledef:		'<' VARIABLEDEF SQSTRING '>'
						{ $$ = new VariableDef( $3 ); }
						;


/***********************************************
 * XRef-related stuff
 ***********************************************/

/* Tag: XRefFormats: a sequence of xrefformats */
tag_xrefformats:	'<' XREFFORMATS xrefformats_elements '>'
					;

/* Elements of a XRefFormats: a sequence of xrefformats */
xrefformats_elements:	tag_xrefformat
					|	xrefformats_elements tag_xrefformat
						;

/* Tag: XRefFormat */
tag_xrefformat:			'<' XREFFORMAT xrefformat_elements '>'
				;

/* Elements of a XRefFormat tag */
xrefformat_elements:		xrefformat_element
				|	xrefformat_elements xrefformat_element
				;

xrefformat_element:		tag_xrefname
					|	tag_xrefdef
					;

tag_xrefname:	'<' XREFNAME SQSTRING '>'
				{ $$ = new XRefName( $3 ); }
				;

tag_xrefdef:	'<' XREFDEF SQSTRING '>'
				;

tag_xref:		'<' XREF xref_elements '>'
				{ $$ = new XRef( $3 ); }
;

xref_elements:	xref_element
				{ XRefElementList* list = new XRefElementList;
				  list->push_front( $1 ); $$ = list; }
		|		xref_element xref_elements
				{ $2->push_front( $1 ); $$ = $2; }
;

xref_element:	tag_xrefname
				{ $$ = new XRefElement( $1 ); }
		|		tag_xrefsrctext
				{ $$ = new XRefElement( $1 ); }
		|		tag_xrefsrcfile
				{ $$ = new XRefElement( $1 ); }
		|		tag_xrefsrciselem
				{ $$ = new XRefElement( $1 ); }
		|		tag_xreflastupdate
				{ $$ = new XRefElement( $1 ); }
		|		tag_unique
				{ $$ = new XRefElement( $1 ); }
;

tag_xrefsrctext: '<' XREFSRCTEXT SQSTRING '>'
				{ $$ = new XRefSrcText( $3 ); }
;

tag_xrefsrciselem:		'<' XREFSRCISELEM ID '>'
						{ $$ = new XRefSrcIsElem( $3 ); }
;

tag_xrefsrcfile:		'<' XREFSRCFILE SQSTRING '>'
						{ $$ = new XRefSrcFile( $3 ); }
;

tag_xreflastupdate:		'<' XREFLASTUPDATE INTEGER INTEGER '>'
						{ $$ = new XRefLastUpdate( $3 ); }
;



/********************************
* Document-related stuff
********************************/

tag_document:	'<' DOCUMENT document_elements '>'
				;

document_elements:		document_element
						{ 
							documentelements.push_front( $1 );
						}
					|	document_element document_elements
						{
							documentelements.push_front( $1 );
						}			 
					;

document_element:		tag_dacrobatbookmarksincludetagnames 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dacrobatparagraphbookmarks
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dautochbars 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dborderson 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dchbarcolor 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dchbargap 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dchbarposition 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dchbarwidth	
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dcurrentview 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_ddisplayoverrides 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dfnoteanchorpos
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dfnoteanchorprefix
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dfnoteanchorsuffix 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dfnotelabels 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dfnotemaxh
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dfnotenumstyle 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dfnotenumberpos 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dfnotenumberprefix 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dfnotenumbersuffix 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dfnoterestart 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dfnotetag 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dfluid 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dfluidsideheads 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dfrozenpages 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dfullrulers 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dgenerateacrobatinfo 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dgraphicsoff 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dgridon 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dlanguage 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dlinebreakchars 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dlinkboundarieson 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathalphacharfontfamily 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathcatalog 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathfunctions 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathgreek 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathlargehoriz 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathlargeintegral 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathlargelevel1 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathlargelevel2 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathlargelevel3 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathlargesigma 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathlargevert 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathmediumhoriz 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathmediumintegral 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathmediumlevel1 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathmediumlevel2 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathmediumlevel3 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathmediumsigma 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathmediumvert 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathnumbers
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathshowcustom 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathsmallhoriz 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathsmallintegral 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathsmalllevel1 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathsmalllevel2 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathsmalllevel3 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathsmallsigma 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathsmallvert 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathstrings 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmathvariables 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dmenubar 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dnarrowrubispaceforjapanese 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dnarrowrubispaceforother 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dnextunique 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dpagegrid 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dpagenumstyle 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dpagepointstyle 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dpagerounding 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dpagescrolling 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dpagesize 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dparity 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dprintseparations 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dprintskipblankpages
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dpunctuationchars 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_drubialignatlinebounds 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_drubioverhang 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_drubisize 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_drulerson 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dshowallconditions 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dsmallcapssize 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dsmallcapsstretch 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dsmartquoteson 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dsmartspaceson 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dsnapgrid 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dsnaprotation 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dstartpage 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dsubscriptshift 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dsubscriptsize 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dsubscriptstretch 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dsuperscriptshift 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dsuperscriptsize 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dsuperscriptstretch
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dsymbolson 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dtblfnoteanchorpos 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dtblfnoteanchorprefix 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dtblfnoteanchorsuffix 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dtblfnotelabels 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dtblfnotenumstyle 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dtblfnotenumberpos 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dtblfnotenumberprefix 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dtblfnotenumbersuffix 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dtblfnotetag 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dtwosides 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dupdatetextinsetsonopen 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dupdatexrefsonopen 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dviewonly 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dviewonlyselect 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dviewonlywinborders 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dviewonlywinmenubar 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dviewonlywinpalette 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dviewonlywinpopup 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dviewonlyxref 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dviewrect 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dviewscale 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dvomenubar 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dwiderubispaceforjapanese 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dwiderubispaceforother 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_dwindowrect 
						{ $$ = new DocumentElement( $1 ); }
					|	tag_fnotestartnum
						{ $$ = new DocumentElement( $1 ); }
					;

tag_dacrobatbookmarksincludetagnames:	'<' DACROBATBOOKMARKSINCLUDETAGNAMES ID '>' 
	{ $$ = new DocumentAcrobatBookmarksIncludeTagNames( $3 ); }
;

tag_dacrobatparagraphbookmarks:	'<' DACROBATPARAGRAPHBOOKMARKS ID '>'
	{ $$ = new DocumentAcrobatParagraphBookmarks( $3 ); }
;

tag_dautochbars:	'<' DAUTOCHBARS ID '>'
	{ $$ = new DocumentAutoChBars( $3 ); }
;

tag_dborderson:	'<' DBORDERSON ID '>' 
	{ $$ = new DocumentBordersOn( $3 ); }
;

tag_dchbarcolor:	'<' DCHBARCOLOR SQSTRING '>' 
	{ $$ = new DocumentChBarColor( $3 ); }
;

tag_dchbargap:	'<' DCHBARGAP REAL unit '>' 
	{ $$ = new DocumentChBarGap( $3, $4 ); }
;

tag_dchbarposition:	'<' DCHBARPOSITION ID '>' 
	{ $$ = new DocumentChBarPosition( $3 ); }
;

tag_dchbarwidth:	'<' DCHBARWIDTH REAL unit '>'	
	{ $$ = new DocumentChBarWidth( $3, $4 ); }
;

tag_dcurrentview:	'<' DCURRENTVIEW INTEGER '>' 
	{ $$ = new DocumentCurrentView( $3 ); }
;

tag_ddisplayoverrides:	'<' DDISPLAYOVERRIDES ID '>' 
	{ $$ = new DocumentDisplayOverrides( $3 ); }
;

tag_dfnoteanchorpos:	'<' DFNOTEANCHORPOS ID '>'
	{ $$ = new DocumentFNoteAnchorPos( $3 ); }
;

tag_dfnoteanchorprefix:	'<' DFNOTEANCHORPREFIX SQSTRING '>'
	{ $$ = new DocumentFNoteAnchorPrefix( $3 ); }
;

tag_dfnoteanchorsuffix:	'<' DFNOTEANCHORSUFFIX SQSTRING '>' 
	{ $$ = new DocumentFNoteAnchorSuffix( $3 ); }
;

tag_dfnotelabels:	'<' DFNOTELABELS SQSTRING '>' 
	{ $$ = new DocumentFNoteLabels( $3 ); }
;

tag_dfnotemaxh:	'<' DFNOTEMAXH REAL unit '>'
	{ $$ = new DocumentFNoteMaxH( $3, $4 ); }
;

tag_dfnotenumstyle:	'<' DFNOTENUMSTYLE ID '>' 
	{ $$ = new DocumentFNoteNumStyle( $3 ); }
;

tag_dfnotenumberpos:	'<' DFNOTENUMBERPOS ID '>' 
	{ $$ = new DocumentFNoteNumberPos( $3 ); }
;

tag_dfnotenumberprefix:	'<' DFNOTENUMBERPREFIX SQSTRING '>' 
	{ $$ = new DocumentFNoteNumberPrefix( $3 ); }
;

tag_dfnotenumbersuffix:	'<' DFNOTENUMBERSUFFIX SQSTRING '>' 
	{ $$ = new DocumentFNoteNumberSuffix( $3 ); }
;

tag_dfnoterestart:	'<' DFNOTERESTART ID '>' 
	{ $$ = new DocumentFNoteRestart( $3 ); }
;

tag_dfnotetag:	'<' DFNOTETAG SQSTRING '>' 
	{ $$ = new DocumentFNoteTag( $3 ); }
;

tag_dfluid:	'<' DFLUID ID '>' 
	{ $$ = new DocumentFluid( $3 ); }
;

tag_dfluidsideheads:	'<' DFLUIDSIDEHEADS ID '>' 
	{ $$ = new DocumentFluidSideHeads( $3 ); }
;

tag_dfrozenpages:	'<' DFROZENPAGES ID '>' 
	{ $$ = new DocumentFrozenPages( $3 ); }
;

tag_dfullrulers:	'<' DFULLRULERS ID '>' 
	{ $$ = new DocumentFullRulers( $3 ); }
;

tag_dgenerateacrobatinfo:	'<' DGENERATEACROBATINFO ID '>' 
	{ $$ = new DocumentGenerateAcrobatInfo( $3 ); }
;

tag_dgraphicsoff:	'<' DGRAPHICSOFF ID '>' 
	{ $$ = new DocumentGraphicsOff( $3 ); }
;

tag_dgridon:	'<' DGRIDON ID '>' 
	{ $$ = new DocumentGridOn( $3 ); }
;

tag_dlanguage:	'<' DLANGUAGE ID '>' 
	{ $$ = new DocumentLanguage( $3 ); }
;

tag_dlinebreakchars:	'<' DLINEBREAKCHARS SQSTRING '>' 
	{ $$ = new DocumentLineBreakChars( $3 ); }
;

tag_dlinkboundarieson:	'<' DLINKBOUNDARIESON ID '>' 
	{ $$ = new DocumentLinkBoundariesOn( $3 ); }
;

tag_dmathalphacharfontfamily:	'<' DMATHALPHACHARFONTFAMILY SQSTRING '>' 
	{ $$ = new DocumentMathAlphaCharFontFamily( $3 ); }
;

tag_dmathcatalog:	'<' DMATHCATALOG '>' 
	{ $$ = new DocumentMathCatalog(); }
;

tag_dmathfunctions:	'<' DMATHFUNCTIONS SQSTRING '>' 
	{ $$ = new DocumentMathFunctions( $3 ); }
;

tag_dmathgreek:	'<' DMATHGREEK SQSTRING '>' 
	{ $$ = new DocumentMathGreek( $3 ); }
;

tag_dmathlargehoriz:	'<' DMATHLARGEHORIZ REAL unit '>' 
	{ $$ = new DocumentMathLargeHoriz( $3, $4 ); }
;

tag_dmathlargeintegral:	'<' DMATHLARGEINTEGRAL REAL unit '>' 
	{ $$ = new DocumentMathLargeIntegral( $3, $4 ); }
;

tag_dmathlargelevel1:	'<' DMATHLARGELEVEL1 REAL unit '>' 
	{ $$ = new DocumentMathLargeLevel1( $3, $4 ); }
;

tag_dmathlargelevel2:	'<' DMATHLARGELEVEL2 REAL unit '>' 
	{ $$ = new DocumentMathLargeLevel2( $3, $4 ); }
;

tag_dmathlargelevel3:	'<' DMATHLARGELEVEL3 REAL unit '>' 
	{ $$ = new DocumentMathLargeLevel3( $3, $4 ); }
;

tag_dmathlargesigma:	'<' DMATHLARGESIGMA REAL unit '>' 
	{ $$ = new DocumentMathLargeSigma( $3, $4 ); }
;

tag_dmathlargevert:	'<' DMATHLARGEVERT REAL unit '>' 
	{ $$ = new DocumentMathLargeVert( $3, $4 ); }
;

tag_dmathmediumhoriz:	'<' DMATHMEDIUMHORIZ REAL unit '>' 
	{ $$ = new DocumentMathMediumHoriz( $3, $4 ); }
;

tag_dmathmediumintegral:	'<' DMATHMEDIUMINTEGRAL REAL unit '>' 
	{ $$ = new DocumentMathMediumIntegral( $3, $4 ); }
;

tag_dmathmediumlevel1:	'<' DMATHMEDIUMLEVEL1 REAL unit '>' 
	{ $$ = new DocumentMathMediumLevel1( $3, $4 ); }
;

tag_dmathmediumlevel2:	'<' DMATHMEDIUMLEVEL2 REAL unit '>' 
	{ $$ = new DocumentMathMediumLevel2( $3, $4 ); }
;

tag_dmathmediumlevel3:	'<' DMATHMEDIUMLEVEL3 REAL unit '>' 
	{ $$ = new DocumentMathMediumLevel3( $3, $4 ); }
;

tag_dmathmediumsigma:	'<' DMATHMEDIUMSIGMA REAL unit '>' 
	{ $$ = new DocumentMathMediumSigma( $3, $4 ); }
;

tag_dmathmediumvert:	'<' DMATHMEDIUMVERT REAL unit '>' 
	{ $$ = new DocumentMathMediumVert( $3, $4 ); }
;

tag_dmathnumbers:	'<' DMATHNUMBERS SQSTRING '>'
	{ $$ = new DocumentMathNumbers( $3 ); }
;

tag_dmathshowcustom:	'<' DMATHSHOWCUSTOM ID '>' 
	{ $$ = new DocumentMathShowCustom( $3 ); }
;

tag_dmathsmallhoriz:	'<' DMATHSMALLHORIZ REAL unit '>' 
	{ $$ = new DocumentMathSmallHoriz( $3, $4 ); }
;

tag_dmathsmallintegral:	'<' DMATHSMALLINTEGRAL REAL unit '>' 
	{ $$ = new DocumentMathSmallIntegral( $3, $4 ); }
;

tag_dmathsmalllevel1:	'<' DMATHSMALLLEVEL1 REAL unit '>' 
	{ $$ = new DocumentMathSmallLevel1( $3, $4 ); }
;

tag_dmathsmalllevel2:	'<' DMATHSMALLLEVEL2 REAL unit '>' 
	{ $$ = new DocumentMathSmallLevel2( $3, $4 ); }
;

tag_dmathsmalllevel3:	'<' DMATHSMALLLEVEL3 REAL unit '>' 
	{ $$ = new DocumentMathSmallLevel3( $3, $4 ); }
;

tag_dmathsmallsigma:	'<' DMATHSMALLSIGMA REAL unit '>' 
	{ $$ = new DocumentMathSmallSigma( $3, $4 ); }
;

tag_dmathsmallvert:	'<' DMATHSMALLVERT REAL unit '>' 
	{ $$ = new DocumentMathSmallVert( $3, $4 ); }
;

tag_dmathstrings:	'<' DMATHSTRINGS SQSTRING '>' 
	{ $$ = new DocumentMathStrings( $3 ); }
;

tag_dmathvariables:	'<' DMATHVARIABLES SQSTRING '>' 
	{ $$ = new DocumentMathVariables( $3 ); }
;

tag_dmenubar:	'<' DMENUBAR SQSTRING '>' 
	{ $$ = new DocumentMenuBar( $3 ); }
;

tag_dnarrowrubispaceforjapanese:	'<' DNARROWRUBISPACEFORJAPANESE ID '>' 
	{ $$ = new DocumentNarrowRubiSpaceForJapanese( $3 ); }
;

tag_dnarrowrubispaceforother:	'<' DNARROWRUBISPACEFOROTHER ID '>' 
	{ $$ = new DocumentNarrowRubiSpaceForOther( $3 ); }
;

tag_dnextunique:	'<' DNEXTUNIQUE INTEGER '>' 
	{ $$ = new DocumentNextUnique( $3 ); }
;

tag_dpagegrid:	'<' DPAGEGRID REAL unit '>' 
	{ $$ = new DocumentPageGrid( $3, $4 ); }
;

tag_dpagenumstyle:	'<' DPAGENUMSTYLE ID '>' 
	{ $$ = new DocumentPageNumStyle( $3 ); }
;

tag_dpagepointstyle:	'<' DPAGEPOINTSTYLE ID '>' 
	{ $$ = new DocumentPagePointStyle( $3 ); }
;

tag_dpagerounding:	'<' DPAGEROUNDING ID '>' 
	{ $$ = new DocumentPageRounding( $3 ); }
;

/* Problem here: variable is a possible value for DPageScrolling tag 
   and a tag itself. */
tag_dpagescrolling:	'<' DPAGESCROLLING VARIABLE '>' 
	{ $$ = new DocumentPageScrolling( $3 ); }
;

tag_dpagesize:  '<' DPAGESIZE REAL unit REAL unit '>'
{ $$ = new DocumentPageSize( $3, $4, $5, $6 ); }
;


tag_dparity:	'<' DPARITY ID '>' 
	{ $$ = new DocumentParity( $3 ); }
;

tag_dprintseparations:	'<' DPRINTSEPARATIONS ID '>' 
	{ $$ = new DocumentPrintSeparations( $3 ); }
;

tag_dprintskipblankpages:	'<' DPRINTSKIPBLANKPAGES ID '>'
	{ $$ = new DocumentPrintSkipBlankPages( $3 ); }
;

tag_dpunctuationchars:	'<' DPUNCTUATIONCHARS SQSTRING '>' 
	{ $$ = new DocumentPunctuationChars( $3 ); }
;

tag_drubialignatlinebounds:	'<' DRUBIALIGNATLINEBOUNDS ID '>' 
	{ $$ = new DocumentRubiAlignAtLineBounds( $3 ); }
;

tag_drubioverhang:	'<' DRUBIOVERHANG ID '>' 
	{ $$ = new DocumentRubiOverhang( $3 ); }
;

tag_drubisize:	'<' DRUBISIZE REAL '%' '>' 
	{ $$ = new DocumentRubiSize( $3 ); }
;

tag_drulerson:	'<' DRULERSON ID '>' 
	{ $$ = new DocumentRulersOn( $3 ); }
;

tag_dshowallconditions:	'<' DSHOWALLCONDITIONS ID '>' 
	{ $$ = new DocumentShowAllConditions( $3 ); }
;

tag_dsmallcapssize:	'<' DSMALLCAPSSIZE REAL '%' '>' 
	{ $$ = new DocumentSmallCapsSize( $3 ); }
;

tag_dsmallcapsstretch:	'<' DSMALLCAPSSTRETCH REAL '%' '>' 
	{ $$ = new DocumentSmallCapsStretch( $3 ); }
;

tag_dsmartquoteson:	'<' DSMARTQUOTESON ID '>' 
	{ $$ = new DocumentSmartQuotesOn( $3 ); }
;

tag_dsmartspaceson:	'<' DSMARTSPACESON ID '>' 
	{ $$ = new DocumentSmartSpacesOn( $3 ); }
;

tag_dsnapgrid:	'<' DSNAPGRID REAL unit '>' 
	{ $$ = new DocumentSnapGrid( $3, $4 ); }
;

tag_dsnaprotation:	'<' DSNAPROTATION REAL '>' 
	{ $$ = new DocumentSnapRotation( $3 ); }
;

tag_dstartpage:	'<' DSTARTPAGE INTEGER '>' 
	{ $$ = new DocumentStartPage( $3 ); }
;

tag_dsubscriptshift:	'<' DSUBSCRIPTSHIFT REAL '%' '>' 
	{ $$ = new DocumentSubscriptShift( $3 ); }
;

tag_dsubscriptsize:	'<' DSUBSCRIPTSIZE REAL '%' '>' 
	{ $$ = new DocumentSubscriptSize( $3 ); }
;

tag_dsubscriptstretch:	'<' DSUBSCRIPTSTRETCH REAL '%' '>' 
	{ $$ = new DocumentSubscriptStretch( $3 ); }
;

tag_dsuperscriptshift:	'<' DSUPERSCRIPTSHIFT REAL '%' '>' 
	{ $$ = new DocumentSuperscriptShift( $3 ); }
;

tag_dsuperscriptsize:	'<' DSUPERSCRIPTSIZE REAL '%' '>' 
	{ $$ = new DocumentSuperscriptSize( $3 ); }
;

tag_dsuperscriptstretch:	'<' DSUPERSCRIPTSTRETCH REAL '%' '>'
	{ $$ = new DocumentSuperscriptStretch( $3 ); }
;

tag_dsymbolson:	'<' DSYMBOLSON ID '>' 
	{ $$ = new DocumentSymbolsOn( $3 ); }
;

tag_dtblfnoteanchorpos:	'<' DTBLFNOTEANCHORPOS ID '>' 
	{ $$ = new DocumentTblFNoteAnchorPos( $3 ); }
;

tag_dtblfnoteanchorprefix:	'<' DTBLFNOTEANCHORPREFIX SQSTRING '>' 
	{ $$ = new DocumentTblFNoteAnchorPrefix( $3 ); }
;

tag_dtblfnoteanchorsuffix:	'<' DTBLFNOTEANCHORSUFFIX SQSTRING '>'  
	{ $$ = new DocumentTblFNoteAnchorSuffix( $3 ); }
;

tag_dtblfnotelabels:	'<' DTBLFNOTELABELS SQSTRING '>' 
	{ $$ = new DocumentTblFNoteLabels( $3 ); }
;

tag_dtblfnotenumstyle:	'<' DTBLFNOTENUMSTYLE ID '>' 
	{ $$ = new DocumentTblFNoteNumStyle( $3 ); }
;

tag_dtblfnotenumberpos:	'<' DTBLFNOTENUMBERPOS ID '>' 
	{ $$ = new DocumentTblFNoteNumberPos( $3 ); }
;

tag_dtblfnotenumberprefix:	'<' DTBLFNOTENUMBERPREFIX SQSTRING '>' 
	{ $$ = new DocumentTblFNoteNumberPrefix( $3 ); }
;

tag_dtblfnotenumbersuffix:	'<' DTBLFNOTENUMBERSUFFIX SQSTRING '>' 
	{ $$ = new DocumentTblFNoteNumberSuffix( $3 ); }
;

tag_dtblfnotetag:	'<' DTBLFNOTETAG SQSTRING '>' 
	{ $$ = new DocumentTblFNoteTag( $3 ); }
;

tag_dtwosides:	'<' DTWOSIDES ID '>' 
	{ $$ = new DocumentTwoSides( $3 ); }
;

tag_dupdatetextinsetsonopen:	'<' DUPDATETEXTINSETSONOPEN ID '>' 
	{ $$ = new DocumentUpdateTextInsetsOnOpen( $3 ); }
;

tag_dupdatexrefsonopen:	'<' DUPDATEXREFSONOPEN ID '>' 
	{ $$ = new DocumentUpdateXRefsOnOpen( $3 ); }
;

tag_dviewonly:	'<' DVIEWONLY ID '>' 
	{ $$ = new DocumentViewOnly( $3 ); }
;

tag_dviewonlyselect:	'<' DVIEWONLYSELECT ID '>' 
	{ $$ = new DocumentViewOnlySelect( $3 ); }
;

tag_dviewonlywinborders:	'<' DVIEWONLYWINBORDERS ID '>' 
	{ $$ = new DocumentViewOnlyWinBorders( $3 ); }
;

tag_dviewonlywinmenubar:	'<' DVIEWONLYWINMENUBAR ID '>' 
	{ $$ = new DocumentViewOnlyWinMenubar( $3 ); }
;

tag_dviewonlywinpalette:	'<' DVIEWONLYWINPALETTE ID '>' 
	{ $$ = new DocumentViewOnlyWinPalette( $3 ); }
;

tag_dviewonlywinpopup:	'<' DVIEWONLYWINPOPUP ID '>' 
	{ $$ = new DocumentViewOnlyWinPopup( $3 ); }
;

tag_dviewonlyxref:	'<' DVIEWONLYXREF ID '>' 
	{ $$ = new DocumentViewOnlyXRef( $3 ); }
;

tag_dviewrect:	'<' DVIEWRECT INTEGER INTEGER INTEGER INTEGER '>' 
	{ $$ = new DocumentViewRect( $3, $4, $5, $6 ); }
;

tag_dviewscale:	'<' DVIEWSCALE REAL '%' '>' 
	{ $$ = new DocumentViewScale( $3 ); }
;

tag_dvomenubar:	'<' DVOMENUBAR SQSTRING '>' 
	{ $$ = new DocumentVoMenuBar( $3 ); }
;

tag_dwiderubispaceforjapanese:	'<' DWIDERUBISPACEFORJAPANESE ID '>' 
	{ $$ = new DocumentWideRubiSpaceForJapanese( $3 ); }
;

tag_dwiderubispaceforother:	'<' DWIDERUBISPACEFOROTHER ID '>' 
	{ $$ = new DocumentWideRubiSpaceForOther( $3 ); }
;

tag_dwindowrect:	'<' DWINDOWRECT INTEGER INTEGER INTEGER INTEGER '>' 
	{ $$ = new DocumentWindowRect( $3, $4, $5, $6 ); }
;

tag_fnotestartnum:		'<' FNOTESTARTNUM INTEGER '>'
	{ $$ = new FNoteStartNum( $3 ); }
;


/*******************************************
 * BookComponent-related stuff 
 ******************************************/
tag_bookcomponent:		'<' BOOKCOMPONENT bookcomponent_elements '>'
;

bookcomponent_elements:			bookcomponent_element
						|		bookcomponent_elements bookcomponent_element
;

bookcomponent_element:			tag_filename
						|		tag_unique
						|		tag_filenamesuffix
						|		tag_derivelinks
						|		tag_derivetype
						|		tag_derivetag
;

tag_filename:	'<' FILENAME SQSTRING '>'
;

tag_filenamesuffix:		'<' FILENAMESUFFIX SQSTRING '>'
;

tag_unique:		'<' UNIQUE INTEGER '>'
				{ $$ = new Unique( $3 ); }
;

tag_derivelinks:		'<' DERIVELINKS ID '>'
;

tag_derivetype:			'<' DERIVETYPE ID '>'
;

tag_derivetag:			'<' DERIVETAG SQSTRING '>'
;


/****************************************
 * InitialAutoNums-related stuff
 ***************************************/
tag_initialautonums:	'<' INITIALAUTONUMS initialautonums_elements '>'
;

initialautonums_elements:		initialautonums_element
						|		initialautonums_elements initialautonums_element
						| /* can be null */
;

initialautonums_element:			tag_autonumseries
;

tag_autonumseries:		'<' AUTONUMSERIES autonumseries_elements '>'
;

autonumseries_elements:	autonumseries_element
					|	autonumseries_elements autonumseries_elements
;

autonumseries_element:			tag_flowtag
						|		tag_series
						|		tag_numcounter
;

tag_flowtag:	'<' FLOWTAG SQSTRING '>'
;

tag_series:		'<' SERIES SQSTRING '>'
;

tag_numcounter:	'<' NUMCOUNTER INTEGER '>'
;


/**************************************
 * Dictionary-related stuff
 *************************************/
tag_dictionary:	'<' DICTIONARY dictionary_elements '>'
;

dictionary_elements:	dictionary_element
				|		dictionary_elements dictionary_element
				|		/* can be null */
;

dictionary_element:		tag_okword
;

tag_okword:		'<' OKWORD SQSTRING '>'
;



/*************************************
 * AFrames-related stuff
 ************************************/
tag_aframes:	'<' AFRAMES aframes_elements '>'
;

aframes_elements:		aframes_element
					|	aframes_elements aframes_element
;


aframes_element:		tag_frame
;

tag_frame:		'<' FRAME frame_elements '>'
				{ $$ = new Frame( $3 ); }
;

frame_elements:	frame_element
				{ FrameElementList* list = new FrameElementList;
				  list->push_front( $1 ); $$ = list; }
		|		frame_element frame_elements
				{ $2->push_front( $1 ); $$ = $2; }
;


frame_element:			tag_frameid
						{ $$ = new FrameElement( $1 ); }
				|		tag_unique
						{ $$ = new FrameElement( $1 ); }
				|		tag_pen
						{ $$ = new FrameElement( $1 ); }
				|		tag_penwidth
						{ $$ = new FrameElement( $1 ); }
				|		tag_fill
						{ $$ = new FrameElement( $1 ); }
				|		tag_separation
						{ $$ = new FrameElement( $1 ); }
				|		tag_obcolor
						{ $$ = new FrameElement( $1 ); }
				|		tag_dashedpattern
						{ $$ = new FrameElement( $1 ); }
				|		tag_angle
						{ $$ = new FrameElement( $1 ); }
				|		tag_shaperect
						{ $$ = new FrameElement( $1 ); }
				|		tag_brect
						{ $$ = new FrameElement( $1 ); }
				|		tag_frametype
						{ $$ = new FrameElement( $1 ); }
				|		tag_nsoffset
						{ $$ = new FrameElement( $1 ); }
				|		tag_bloffset
						{ $$ = new FrameElement( $1 ); }
				|		tag_anchoralign
						{ $$ = new FrameElement( $1 ); }
				|		tag_cropped
						{ $$ = new FrameElement( $1 ); }
				|		tag_importobject
						{ $$ = new FrameElement( $1 ); }
				|		tag_textrect
						{ $$ = new FrameElement( $1 ); }
				|		tag_tag
						{ $$ = new FrameElement( $1 ); }
				|		tag_polyline
						{ $$ = new FrameElement( $1 ); }
				|		tag_runaroundgap
						{ $$ = new FrameElement( $1 ); }
				|		tag_runaroundtype
						{ $$ = new FrameElement( $1 ); }
				|		tag_math
						{ $$ = new FrameElement( $1 ); }
;

tag_frameid:	'<' FRAMEID INTEGER '>'
				{ $$ = new FrameID( $3 ); }
;

tag_tag:		'<' TAG SQSTRING '>'
				{ $$ = new Tag( $3 ); }
;

tag_unique:		'<' UNIQUE INTEGER '>'
				{ $$ = new Unique( $3 ); }
;

tag_pen:		'<' PEN INTEGER '>'
				{ $$ = new Pen( $3 ); }
;

tag_fill:		'<' FILL INTEGER '>'
				{ $$ = new Fill( $3 ); }
;

tag_penwidth:	'<' PENWIDTH REAL unit '>'
				{ $$ = new PenWidth( $3, $4 ); }
;

tag_separation:	'<' SEPARATION INTEGER '>'
				{ $$ = new Separation( $3 ); }
;

tag_obcolor:	'<' OBCOLOR SQSTRING '>'
				{ $$ = new ObColor( $3 ); }
;

tag_dashedpattern:		'<' DASHEDPATTERN dashedpattern_elements '>'
						{ $$ = new DashedPattern( $3 ); }
;

dashedpattern_elements: dashedpattern_element
						{ DashedPatternElementList* list = new DashedPatternElementList;
						  list->push_front( $1 ); $$ = list; }
					|	dashedpattern_element dashedpattern_elements
						{ $2->push_front( $1 ); $$ = $2; }
;

dashedpattern_element:	tag_dashedstyle
						{ $$ = new DashedPatternElement( $1 ); }
					|	tag_numsegments
						{ $$ = new DashedPatternElement( $1 ); }
					|	tag_dashsegment
						{ $$ = new DashedPatternElement( $1 ); }
;


tag_dashedstyle:		'<' DASHEDSTYLE ID '>'
						{ $$ = new DashedStyle( $3 ); }
;


tag_numsegments:		'<' NUMSEGMENTS INTEGER '>'
						{ $$ = new NumSegments( $3 ); }
;

tag_dashsegment:		'<' DASHSEGMENT REAL unit '>'
						{ $$ = new DashSegment( $3, $4 ); }
;


tag_angle:		'<' ANGLE REAL '>'
						{ $$ = new Angle( $3 ); }
;

tag_runaroundgap:		'<' RUNAROUNDGAP REAL unit '>'
						{ $$ = new RunAroundGap( $3, $4 ); }
;

tag_runaroundtype:		'<' RUNAROUNDTYPE ID '>'
						{ $$ = new RunAroundType( $3 ); }
;

tag_shaperect:	'<' SHAPERECT REAL unit REAL unit REAL unit REAL unit '>'
						{ $$ = new ShapeRect( $3, $4, $5, $6, $7, $8, $9, $10 ); }
;

tag_brect:	'<' BRECT REAL unit REAL unit REAL unit REAL unit '>'
						{ $$ = new BRect( $3, $4, $5, $6, $7, $8, $9, $10 ); }
;

tag_frametype:	'<' FRAMETYPE ID '>'
						{ $$ = new FrameType( $3 ); }
;

tag_nsoffset:	'<' NSOFFSET REAL unit '>'
						{ $$ = new NSOffset( $3, $4 ); }
;

tag_bloffset:	'<' BLOFFSET REAL unit '>'
						{ $$ = new BLOffset( $3, $4 ); }
;

tag_anchoralign:		'<' ANCHORALIGN ID '>'
						{ $$ = new AnchorAlign( $3 ); }
;

tag_cropped:	'<' CROPPED ID '>'
						{ $$ = new Cropped( $3 ); }
;

tag_importobject:		'<' IMPORTOBJECT importobject_elements '>'
						{ $$ = new ImportObject( $3 ); }
;

importobject_elements:	importobject_element
						{ ImportObjectElementList* list = new ImportObjectElementList;
						  list->push_front( $1 ); $$ = list; }
				|		importobject_element importobject_elements
						{ $2->push_front( $1 ); $$ = $2; }
;

importobject_element:	tag_unique
						{ $$ = new ImportObjectElement( $1 ); }
				|		tag_importobfile
						{ $$ = new ImportObjectElement( $1 ); }
				|		tag_shaperect
						{ $$ = new ImportObjectElement( $1 ); }
				|		tag_brect
						{ $$ = new ImportObjectElement( $1 ); }
				|		tag_fliplr
						{ $$ = new ImportObjectElement( $1 ); }
				|		tag_nativeorigin
						{ $$ = new ImportObjectElement( $1 ); }
				|		tag_bitmapdpi
						{ $$ = new ImportObjectElement( $1 ); }
				|		tag_runaroundtype
						{ $$ = new ImportObjectElement( $1 ); }
				|		tag_runaroundgap
						{ $$ = new ImportObjectElement( $1 ); }
				|		externaldata
						{ $$ = new ImportObjectElement( $1 ); }
;

/* PENDING(kalle) Do something about this */
externaldata:
		{ $$ = 0; }
;

tag_bitmapdpi:	'<' BITMAPDPI INTEGER '>'
				{ $$ = new BitmapDPI( $3 ); }
;

tag_importobfile:		'<' IMPORTOBFILE SQSTRING '>'
				{ $$ = new ImportObFile( $3 ); }
;

tag_fliplr:		'<' FLIPLR ID '>'
				{ $$ = new FlipLR( $3 ); }
;

tag_nativeorigin:		'<' NATIVEORIGIN REAL unit REAL unit '>'
				{ $$ = new NativeOrigin( $3, $4, $5, $6 ); }
;



/*******************
 * Math-related stuff
 *******************/
tag_math:		'<' MATH math_elements '>'
				{ $$ = new Math( $3 ); }
;

math_elements:	math_element
				{ MathElementList* list = new MathElementList;
				  list->push_front( $1 ); $$ = list; }
				| math_element math_elements
				{ $2->push_front( $1 ); $$ = $2;  }
;

math_element:	tag_unique
				{ $$ = new MathElement( $1 ); }
			|	tag_brect
				{ $$ = new MathElement( $1 ); }
			|	tag_mathfullform
				{ $$ = new MathElement( $1 ); }
			|	tag_mathlinebreak
				{ $$ = new MathElement( $1 ); }
			|	tag_mathorigin
				{ $$ = new MathElement( $1 ); }
			|	tag_mathalignment
				{ $$ = new MathElement( $1 ); }
			|	tag_mathsize
				{ $$ = new MathElement( $1 ); }
;

tag_mathfullform:		'<' MATHFULLFORM SQSTRING '>'
						{ $$ = new MathFullForm( $3 ); }
;

tag_mathlinebreak:		'<' MATHLINEBREAK REAL unit '>'
						{ $$ = new MathLineBreak( $3, $4 ); }
;

tag_mathorigin:			'<' MATHORIGIN REAL unit REAL unit '>'
						{ $$ = new MathOrigin( $3, $4, $5, $6 ); }
;

tag_mathalignment:		'<' MATHALIGNMENT ID '>'
						{ $$ = new MathAlignment( $3 ); }
;

tag_mathsize:			'<' MATHSIZE ID '>'
						{ $$ = new MathSize( $3 ); }
;


/*************************************
 * Tbls-related stuff
 ************************************/
tag_tbls:		'<' TBLS tbls_elements '>'
;

tbls_elements:	tag_tbl
		|		tbls_elements tag_tbl
;

tag_tbl:		'<' TBL tbl_elements '>'
;

tbl_elements:	tbl_element
		|		tbl_elements tbl_element
;

tbl_element:	tag_tblid
		|		tag_tbltag
		|		tag_tblformat
		|		tag_tblnumcolumns
		|		tag_unique
		|		tag_tblcolumnwidth
		|		tag_tblh
		|		tag_tblbody
		|		tag_tbltitle
;


tag_tbltitle:	'<' TBLTITLE tbltitle_elements '>'
;

tbltitle_elements:		tbltitle_element
				|		tbltitle_elements tbltitle_element
;

tbltitle_element:		tag_tbltitlecontent
;

tag_tbltitlecontent:	'<' TBLTITLECONTENT tbltitlecontent_elements '>'
;

tbltitlecontent_elements:		tbltitlecontent_element
						|		tbltitlecontent_elements tbltitlecontent_element
;

tbltitlecontent_element:		tag_notes
						|		tag_para
;

tag_tblid:	'<' TBLID INTEGER '>'
;

tag_tblnumcolumns:		'<' TBLNUMCOLUMNS INTEGER '>'
;

tag_tblh:		'<' TBLH tblh_elements '>'
;

tblh_elements:	tag_row
		|		tblh_elements tag_row
;

tag_tblbody:	'<' TBLBODY tblbody_elements '>'
;

tblbody_elements:		tag_row
				|		tblbody_elements tag_row
;

tag_row:		'<' ROW row_elements '>'
;

row_elements:	row_element
		|		row_elements row_element
;

row_element:	tag_rowmaxheight
		|		tag_rowminheight
		|		tag_rowheight
		|		tag_cell
;

tag_rowmaxheight:		'<' ROWMAXHEIGHT REAL unit '>'
;

tag_rowminheight:		'<' ROWMINHEIGHT REAL unit '>'
;

tag_rowheight:	'<' ROWHEIGHT REAL unit '>'
;

tag_cell:		'<' CELL cell_elements '>'
;

cell_elements:	cell_element
		|		cell_elements cell_element
;

cell_element:	tag_cellcontent
		|		tag_cellrows
		|		tag_cellcolumns
		|		tag_cellfill
		|		tag_cellcolor
		|		tag_cellseparation
		|		tag_cellrruling
		|		tag_celltruling
		|		tag_cellbruling
		|		tag_celllruling
;


tag_cellcolor:	'<' CELLCOLOR SQSTRING '>'
;

tag_cellseparation:		'<' CELLSEPARATION INTEGER '>'
;

tag_cellrruling:		'<' CELLRRULING SQSTRING '>'
;

tag_celltruling:		'<' CELLTRULING SQSTRING '>'
;

tag_cellbruling:		'<' CELLBRULING SQSTRING '>'
;

tag_celllruling:		'<' CELLLRULING SQSTRING '>'
;

tag_cellfill:	'<' CELLFILL INTEGER '>'
;

tag_cellrows:	'<' CELLROWS INTEGER '>'
;

tag_cellcolumns:		'<' CELLCOLUMNS INTEGER '>'
;

tag_cellcontent:		'<' CELLCONTENT cellcontent_elements '>'
;

cellcontent_elements:	cellcontent_element
				|		cellcontent_elements cellcontent_element
;

cellcontent_element:	tag_notes
				|		tag_para
;

tag_notes:		'<' NOTES notes_elements '>'
				{ $$ = new Notes( $3 ); }
;

notes_elements:	notes_element
				{ NotesElementList* list = new NotesElementList;
				  list->push_front( $1 ); $$ = list; }
		|		notes_element notes_elements
				{ $2->push_front( $1 ); $$ = $2; }
		|		/* nothing */
				{ NotesElementList* list = new NotesElementList;
				  $$ = list; }
;

notes_element:	tag_fnote
				{ $$ = new NotesElement( $1 ); }
;

tag_fnote:		'<' FNOTE fnote_elements '>'
				{ $$ = new FNote( $3 ); }
;

fnote_elements:	fnote_element
				{ FNoteElementList* list = new FNoteElementList;
				  list->push_front( $1 ); $$ = list; }
			|	fnote_element fnote_elements
				{ $2->push_front( $1 ); $$ = $2; }
;

fnote_element:	tag_frameid
				{ $$ = new FNoteElement( $1 ); }
			|	tag_para
				{ $$ = new FNoteElement( $1 ); }
;


/*****************************
 * Para-related stuff
 ****************************/
tag_para:		'<' PARA para_elements '>'
				{	$$ = new Para( $3 ); }
;

para_elements:	para_element
				{	ParaElementList* list = new ParaElementList;
					list->push_front( $1 ); $$ = list; }
		|		para_element para_elements
				{	$2->push_front( $1 ); $$ = $2; }
;

para_element:	tag_unique
				{	$$ = new ParaElement( $1 ); }
			|	tag_pgf
				{	$$ = new ParaElement( $1 ); }
			|	tag_pgftag
				{	$$ = new ParaElement( $1 ); }
			|	tag_pgfnumstring
				{	$$ = new ParaElement( $1 ); }
			|	tag_paraline
				{	$$ = new ParaElement( $1 ); }
;

tag_paraline:	'<' PARALINE paraline_elements '>'
				{ $$ = new ParaLine( $3 ); }
;

paraline_elements:	paraline_element
					{ ParaLineElementList* list = new ParaLineElementList;
					  list->push_front( $1 ); $$ = list; }
		|			paraline_element paraline_elements;
					{ $2->push_front( $1 ); $$ = $2; }

paraline_element:		tag_string
						{ $$ = new ParaLineElement( $1 ); }
				|		tag_char
						{ $$ = new ParaLineElement( $1 ); }
				|		tag_textrectid
						{ $$ = new ParaLineElement( $1 ); }
				|		tag_variable
						{ $$ = new ParaLineElement( $1 ); }
				|		tag_atbl
						{ $$ = new ParaLineElement( $1 ); }
				|		tag_aframe
						{ $$ = new ParaLineElement( $1 ); }
				|		tag_font
						{ $$ = new ParaLineElement( $1 ); }
				|		tag_marker
						{ $$ = new ParaLineElement( $1 ); }
				|		tag_xref
						{ $$ = new ParaLineElement( $1 ); }
				|		tag_xrefend
						{ $$ = new ParaLineElement( $1 ); }
				|		fnotereference
						{ $$ = new ParaLineElement( $1 ); }
				|		/* nothing */
						{ $$ = new ParaLineElement(); }
;

tag_string: '<' STRING SQSTRING '>'
		{ $$ = new String( fixup_singlequoted_string( $3 ) ); }
;

tag_char:	'<' CHAR ID '>'
		{ $$ = new Char( fixup_id( $3 ) ); }
;

tag_textrectid:	'<' TEXTRECTID INTEGER '>'
		{ $$ = new TextRectID( $3 ); }
;

tag_variable:	'<' VARIABLE variable_elements '>'
		{ $$ = new Variable( $3 ); }
;

variable_elements:		variable_element
						{ VariableElementList* list = new VariableElementList;
						  list->push_front( $1 ); $$ = list; }
				|		variable_element variable_elements
						{ $2->push_front( $1 ); $$ = $2; }
;

variable_element:		tag_variablename
						{ $$ = new VariableElement( $1 ); }
				|		tag_unique
						{ $$ = new VariableElement( $1 ); }
;

tag_atbl:		'<' ATBL INTEGER '>'
				{ $$ = new ATbl( $3 ); }
;

tag_aframe:		'<' AFRAME INTEGER '>'
				{ $$ = new AFrame( $3 ); }
;

fnotereference:		'<' FNOTE INTEGER '>'
					{ $$ = new FNote( $3 ); }
;

tag_xrefend:	'<' XREFEND '>'
				{ $$ = new XRefEnd; }
;


/*******************************
 * Page-related stuff
 ******************************/
tag_page:		'<' PAGE page_elements '>'
				{ $$ = new Page( $3 ); }
;

page_elements:	page_element
				{ PageElementList* list = new PageElementList;
				  list->push_front( $1 ); $$ = list; }
		|		page_element page_elements
				{ $2->push_front( $1 ); $$ = $2; }
;

page_element:	tag_unique
				{ $$ = new PageElement( $1 ); }
		|		tag_pagetype
				{ $$ = new PageElement( $1 ); }
		|		tag_pagetag
				{ $$ = new PageElement( $1 ); }
		|		tag_pagesize
				{ $$ = new PageElement( $1 ); }
		|		tag_pageorientation
				{ $$ = new PageElement( $1 ); }
		|		tag_pageangle
				{ $$ = new PageElement( $1 ); }
		|		tag_textrect
				{ $$ = new PageElement( $1 ); }
		|		tag_textline
				{ $$ = new PageElement( $1 ); }
		|		tag_polyline
				{ $$ = new PageElement( $1 ); }
		|		tag_frame
				{ $$ = new PageElement( $1 ); }
		|		tag_pagenum
				{ $$ = new PageElement( $1 ); }
		|		tag_pagebackground
				{ $$ = new PageElement( $1 ); }
		|		tag_rectangle
				{ $$ = new PageElement( $1 ); }
		|		tag_runaroundtype
				{ $$ = new PageElement( $1 ); }
		|		tag_runaroundgap
				{ $$ = new PageElement( $1 ); }
		|		tag_polygon
				{ $$ = new PageElement( $1 ); }
		|		tag_group
				{ $$ = new PageElement( $1 ); }
		|		tag_ellipse
				{ $$ = new PageElement( $1 ); }
;


tag_pagenum:	'<' PAGENUM SQSTRING '>'
				{ $$ = new PageNum( $3 ); }
;

tag_pagebackground:		'<' PAGEBACKGROUND SQSTRING '>'
						{ $$ = new PageBackground( $3 ); }
;

tag_pagetype:	'<' PAGETYPE ID '>'
						{ $$ = new PageType( $3 ); }
;

tag_pagetag:	'<' PAGETAG SQSTRING '>'
						{ $$ = new PageTag( $3 ); }
;

tag_pagesize:	'<'	PAGESIZE REAL unit REAL unit '>'
						{ $$ = new PageSize( $3, $4, $5, $6 ); }
;

tag_pageorientation:	'<' PAGEORIENTATION ID '>'
						{ $$ = new PageOrientation( $3 ); }
;

tag_pageangle:	'<' PAGEANGLE REAL '>'
						{ $$ = new PageAngle( $3 ); }
;


tag_rectangle:	'<' RECTANGLE rectangle_elements '>'
						{ $$ = new Rectangle( $3 ); }
;

rectangle_elements:		rectangle_element
						{ RectangleElementList* list = new RectangleElementList;
						  list->push_front( $1 ); $$ = list; }
				|		rectangle_element rectangle_elements
						{ $2->push_front( $1 ); $$ = $2; }
;

rectangle_element:		tag_unique
						{ $$ = new RectangleElement( $1 ); }
				|		tag_fill
						{ $$ = new RectangleElement( $1 ); }
				|		tag_pen
						{ $$ = new RectangleElement( $1 ); }
				|		tag_penwidth
						{ $$ = new RectangleElement( $1 ); }
				|		tag_separation
						{ $$ = new RectangleElement( $1 ); }
				|		tag_obcolor
						{ $$ = new RectangleElement( $1 ); }
				|		tag_dashedpattern
						{ $$ = new RectangleElement( $1 ); }
				|		tag_runaroundgap
						{ $$ = new RectangleElement( $1 ); }
				|		tag_runaroundtype
						{ $$ = new RectangleElement( $1 ); }
				|		tag_shaperect
						{ $$ = new RectangleElement( $1 ); }
				|		tag_brect
						{ $$ = new RectangleElement( $1 ); }
				|		tag_groupid
						{ $$ = new RectangleElement( $1 ); }
;

tag_textrect:	'<'	TEXTRECT textrect_elements '>'
				{ $$ = new TextRect( $3 ); }
;

textrect_elements:		textrect_element
						{ TextRectElementList* list = new TextRectElementList;
						  list->push_front( $1 ); $$ = list; }
				|		textrect_element textrect_elements
						{ $2->push_front( $1 ); $$ = $2; }
;

textrect_element:		tag_frameid
						{ $$ = new TextRectElement( $1 ); }
				|		tag_unique
						{ $$ = new TextRectElement( $1 ); }
				|		tag_pen
						{ $$ = new TextRectElement( $1 ); }
				|		tag_fill
						{ $$ = new TextRectElement( $1 ); }
				|		tag_penwidth
						{ $$ = new TextRectElement( $1 ); }
				|		tag_separation
						{ $$ = new TextRectElement( $1 ); }
				|		tag_obcolor
						{ $$ = new TextRectElement( $1 ); }
				|		tag_dashedpattern
						{ $$ = new TextRectElement( $1 ); }
				|		tag_angle
						{ $$ = new TextRectElement( $1 ); }
				|		tag_shaperect
						{ $$ = new TextRectElement( $1 ); }
				|		tag_brect
						{ $$ = new TextRectElement( $1 ); }
				|		tag_trnumcolumns
						{ $$ = new TextRectElement( $1 ); }
				|		tag_trcolumngap
						{ $$ = new TextRectElement( $1 ); }
				|		tag_trcolumnbalance
						{ $$ = new TextRectElement( $1 ); }
				|		tag_trsideheadwidth
						{ $$ = new TextRectElement( $1 ); }
				|		tag_trsideheadgap
						{ $$ = new TextRectElement( $1 ); }
				|		tag_trsideheadplacement
						{ $$ = new TextRectElement( $1 ); }
				|		tag_trnext
						{ $$ = new TextRectElement( $1 ); }
				|		tag_runaroundgap
						{ $$ = new TextRectElement( $1 ); }
				|		tag_runaroundtype
						{ $$ = new TextRectElement( $1 ); }
;

tag_trnumcolumns:		'<' TRNUMCOLUMNS INTEGER '>'
						{ $$ = new TRNumColumns( $3 ); }
;

tag_trcolumngap:		'<' TRCOLUMNGAP REAL unit '>'
						{ $$ = new TRColumnGap( $3, $4 ); }
;

tag_trcolumnbalance:	'<' TRCOLUMNBALANCE ID '>'
						{ $$ = new TRColumnBalance( $3 ); }
;

tag_trsideheadwidth:	'<' TRSIDEHEADWIDTH REAL unit '>'
						{ $$ = new TRSideheadWidth( $3, $4 ); }
;

tag_trsideheadgap:		'<'	TRSIDEHEADGAP REAL unit '>'
						{ $$ = new TRSideheadGap( $3, $4 ); }
;

tag_trsideheadplacement:		'<' TRSIDEHEADPLACEMENT ID '>'
						{ $$ = new TRSideheadPlacement( $3 ); }
;

tag_trnext:		'<' TRNEXT INTEGER '>'
						{ $$ = new TRNext( $3 ); }
;

tag_textline:	'<' TEXTLINE textline_elements '>'
						{ $$ = new TextLine( $3 ); }
;

textline_elements:		textline_element
						{ TextLineElementList* list = new TextLineElementList;
						  list->push_front( $1 ); $$ = list; }
				|		textline_element textline_elements
						{ $2->push_front( $1 ); $$ = $2; }
;

textline_element:		tag_unique
						{ $$ = new TextLineElement( $1 ); }
				|		tag_separation
						{ $$ = new TextLineElement( $1 ); }
				|		tag_obcolor
						{ $$ = new TextLineElement( $1 ); }
				|		tag_dashedpattern
						{ $$ = new TextLineElement( $1 ); }
				|		tag_tlorigin
						{ $$ = new TextLineElement( $1 ); }
				|		tag_tlalignment
						{ $$ = new TextLineElement( $1 ); }
				|		tag_font
						{ $$ = new TextLineElement( $1 ); }
				|		tag_string
						{ $$ = new TextLineElement( $1 ); }
				|		tag_runaroundgap
						{ $$ = new TextLineElement( $1 ); }
				|		tag_runaroundtype
						{ $$ = new TextLineElement( $1 ); }
				|		tag_groupid
						{ $$ = new TextLineElement( $1 ); }
;

tag_tlorigin:	'<' TLORIGIN REAL unit REAL unit '>'
				{ $$ = new TLOrigin( $3, $4, $5, $6 ); }
;

tag_tlalignment:		'<' TLALIGNMENT ID '>'
				{ $$ = new TLAlignment( $3 ); }
;

tag_polyline:	'<' POLYLINE polyline_elements '>'
				{ $$ = new PolyLine( $3 ); }
;

polyline_elements:		polyline_element
						{ PolyLineElementList* list = new PolyLineElementList;
						  list->push_front( $1 ); $$ = list; }
				|		polyline_element polyline_elements
						{ $2->push_front( $1 ); $$ = $2; }
;

polyline_element:		tag_unique
						{ $$ = new PolyLineElement( $1 ); }
				|		tag_pen
						{ $$ = new PolyLineElement( $1 ); }
				|		tag_penwidth
						{ $$ = new PolyLineElement( $1 ); }
				|		tag_headcap
						{ $$ = new PolyLineElement( $1 ); }
				|		tag_tailcap
						{ $$ = new PolyLineElement( $1 ); }
				|		tag_arrowstyle
						{ $$ = new PolyLineElement( $1 ); }
				|		tag_numpoints
						{ $$ = new PolyLineElement( $1 ); }
				|		tag_point
						{ $$ = new PolyLineElement( $1 ); }
				|		tag_groupid
						{ $$ = new PolyLineElement( $1 ); }
				|		tag_runaroundgap
						{ $$ = new PolyLineElement( $1 ); }
				|		tag_runaroundtype
						{ $$ = new PolyLineElement( $1 ); }
				|		tag_fill
						{ $$ = new PolyLineElement( $1 ); }
				|		tag_separation
						{ $$ = new PolyLineElement( $1 ); }
				|		tag_obcolor
						{ $$ = new PolyLineElement( $1 ); }
;

tag_headcap:	'<' HEADCAP ID '>'
				{ $$ = new HeadCap( $3 ); }
;

tag_tailcap:	'<' TAILCAP ID '>'
				{ $$ = new TailCap( $3 ); }
;

tag_arrowstyle:	'<' ARROWSTYLE arrowstyle_elements '>'
				{ $$ = new ArrowStyle( $3 ); }
;

arrowstyle_elements:	arrowstyle_element
						{ ArrowStyleElementList* list = new ArrowStyleElementList;
						  list->push_front( $1 ); $$ = list; }
				|		arrowstyle_element arrowstyle_elements
						{ $2->push_front( $1 ); $$ = $2; }
;

arrowstyle_element:		tag_tipangle
						{ $$ = new ArrowStyleElement( $1 ); }
				|		tag_baseangle
						{ $$ = new ArrowStyleElement( $1 ); }
				|		tag_length
						{ $$ = new ArrowStyleElement( $1 ); }
				|		tag_headtype
						{ $$ = new ArrowStyleElement( $1 ); }
				|		tag_scalehead
						{ $$ = new ArrowStyleElement( $1 ); }
				|		tag_scalefactor
						{ $$ = new ArrowStyleElement( $1 ); }
;

tag_tipangle:	'<' TIPANGLE INTEGER '>'
				{ $$ = new TipAngle( $3 ); }
;

tag_baseangle:	'<'	BASEANGLE INTEGER '>'
				{ $$ = new BaseAngle( $3 ); }
;

tag_length:		'<' LENGTH REAL unit '>'
				{ $$ = new Length( $3, $4 ); }
;

tag_headtype:	'<'	HEADTYPE ID '>'
				{ $$ = new HeadType( $3 ); }
;

tag_scalehead:	'<'	SCALEHEAD ID '>'
				{ $$ = new ScaleHead( $3 ); }
;

tag_scalefactor:		'<' SCALEFACTOR REAL unit '>'
				{ $$ = new ScaleFactor( $3, $4 ); }
;

tag_numpoints:	'<' NUMPOINTS INTEGER '>'
				{ $$ = new NumPoints( $3 ); }
;

tag_point:		'<' POINT REAL unit REAL unit '>'
				{ $$ = new Point( $3, $4, $5, $6 ); }
;

tag_polygon:	'<' POLYGON polygon_elements '>'
				{ $$ = new Polygon( $3 ); }
;

polygon_elements:		polygon_element
						{ PolygonElementList* list = new PolygonElementList;
						  list->push_front( $1 ); $$ = list; }
				|		polygon_element polygon_elements
						{ $2->push_front( $1 ); $$ = $2; }
;

polygon_element:		tag_unique
						{ $$ = new PolygonElement( $1 ); }
				|		tag_groupid
						{ $$ = new PolygonElement( $1 ); }
				|		tag_pen
						{ $$ = new PolygonElement( $1 ); }
				|		tag_fill
						{ $$ = new PolygonElement( $1 ); }
				|		tag_penwidth
						{ $$ = new PolygonElement( $1 ); }
				|		tag_separation
						{ $$ = new PolygonElement( $1 ); }
				|		tag_obcolor
						{ $$ = new PolygonElement( $1 ); }
				|		tag_dashedpattern
						{ $$ = new PolygonElement( $1 ); }
				|		tag_runaroundgap
						{ $$ = new PolygonElement( $1 ); }
				|		tag_runaroundtype
						{ $$ = new PolygonElement( $1 ); }
				|		tag_numpoints
						{ $$ = new PolygonElement( $1 ); }
				|		tag_point
						{ $$ = new PolygonElement( $1 ); }
;


tag_ellipse:	'<' ELLIPSE ellipse_elements '>'
						{ $$ = new Ellipse( $3 ); }
;

ellipse_elements:		ellipse_element
						{ EllipseElementList* list = new EllipseElementList;
						  list->push_front( $1 ); $$ = list; }
				|		ellipse_element ellipse_elements
						{ $2->push_front( $1 ); $$ = $2; }
;

ellipse_element:		tag_unique
						{ $$ = new EllipseElement( $1 ); }
				|		tag_groupid
						{ $$ = new EllipseElement( $1 ); }
				|		tag_pen
						{ $$ = new EllipseElement( $1 ); }
				|		tag_fill
						{ $$ = new EllipseElement( $1 ); }
				|		tag_separation
						{ $$ = new EllipseElement( $1 ); }
				|		tag_obcolor
						{ $$ = new EllipseElement( $1 ); }
				|		tag_shaperect
						{ $$ = new EllipseElement( $1 ); }
				|		tag_brect
						{ $$ = new EllipseElement( $1 ); }
;


tag_groupid:	'<' GROUPID INTEGER '>'
				{ $$ = new GroupID( $3 ); }
;


tag_group:		'<' GROUP group_elements '>'
				{ $$ = new Group( $3 ); }
;

group_elements:	group_element
				{ GroupElementList* list = new GroupElementList;
				  list->push_front( $1 ); $$ = list; }
			|	group_element group_elements
				{ $2->push_front( $1 ); $$ = $2; }
;

group_element:	tag_frameid
				{ $$ = new GroupElement( $1 ); }
		|		tag_unique
				{ $$ = new GroupElement( $1 ); }
		|		tag_runaroundgap
				{ $$ = new GroupElement( $1 ); }
		|		tag_runaroundtype
				{ $$ = new GroupElement( $1 ); }
		|		tag_groupid
				{ $$ = new GroupElement( $1 ); }
;


/*********************************
 * TextFlow-related stuff
 ********************************/
tag_textflow:	'<' TEXTFLOW textflow_elements '>'
					{ $$ = new TextFlow( $3 ); }
;

textflow_elements:		textflow_element
						{ TextFlowElementList* list = new TextFlowElementList;
						  list->push_front( $1 ); $$ = list; }
					|	textflow_element textflow_elements
						{ $2->push_front( $1 ); $$ = $2; }
;

textflow_element:		tag_notes
						{ $$ = new TextFlowElement( $1 ); }
				|		tag_para
						{ $$ = new TextFlowElement( $1 ); }
				|		tag_tftag
						{ $$ = new TextFlowElement( $1 ); }
				|		tag_tfautoconnect
						{ $$ = new TextFlowElement( $1 ); }
				|		tag_tfsynchronized
						{ $$ = new TextFlowElement( $1 ); }
				|		tag_tflinespacing
						{ $$ = new TextFlowElement( $1 ); }
				|		tag_tfminhangheight
						{ $$ = new TextFlowElement( $1 ); }
;

tag_tftag:		'<' TFTAG SQSTRING '>'
				{ $$ = new TFTag( $3 ); }
;

tag_tfautoconnect:		'<' TFAUTOCONNECT ID '>'
						{ $$ = new TFAutoConnect( $3 ); }
;


tag_tfsynchronized:		'<' TFSYNCHRONIZED ID '>'
						{ $$ = new TFSynchronized( $3 ); }
;

tag_tflinespacing:		'<' TFLINESPACING REAL unit '>'
						{ $$ = new TFLineSpacing( $3, $4 ); }

tag_tfminhangheight:	'<' TFMINHANGHEIGHT REAL unit '>'
						{ $$ = new TFMinHangHeight( $3, $4 ); }
;


/****************************
 * Marker-related stuff
 ***************************/
tag_marker:		'<' MARKER marker_elements '>'
				{ $$ = new Marker( $3 ); }
;

marker_elements:		marker_element
						{ MarkerElementList* list = new MarkerElementList;
						  list->push_front( $1 ); $$ = list; }
				|		marker_element marker_elements
						{ $2->push_front( $1 ); $$ = $2; }
;

marker_element:	tag_mtype
				{ $$ = new MarkerElement( $1 ); }
		|		tag_mtypename
				{ $$ = new MarkerElement( $1 ); }
		|		tag_mtext
				{ $$ = new MarkerElement( $1 ); }
		|		tag_mcurrpage
				{ $$ = new MarkerElement( $1 ); }
		|		tag_unique
				{ $$ = new MarkerElement( $1 ); }
;

tag_mtype:		'<' MTYPE INTEGER '>'
				{ $$ = new MType( $3 ); }
;

tag_mtypename:	'<' MTYPENAME SQSTRING '>'
				{ $$ = new MTypeName( $3 ); }
;

tag_mtext:		'<' MTEXT SQSTRING '>'
				{ $$ = new MText( $3 ); }
;

tag_mcurrpage:	'<' MCURRPAGE SQSTRING '>'
				{ $$ = new MCurrPage( $3 ); }
;


tag_markertypecatalog:	'<' MARKERTYPECATALOG tag_mtypename '>'
;



/* Tag: MIFFile */
tag_miffile:		'<' MIFFILE REAL '>'
				;

/* Tag: Units */
tag_units:			'<' UNITS ID '>'
				;

/* Tag: CharUnits*/
tag_charunits:		'<' CHARUNITS ID '>'
				;

/* quoted strings */
qstring:		QSTRING { $$ = $1; }
			;

/* integer expressions */
iexpr:		INTEGER
		|	iexpr '+' iexpr
			{ $$ = $1 + $3; }
		|	iexpr '-' iexpr
			{ $$ = $1 - $3; }
		|	iexpr '*' iexpr
			{ $$ = $1 * $3; }
		|	iexpr '/' iexpr
			{ if( $3 ) 
				$$ = $1 / $3;
			else {
				cerr << "divide by zero";
				yyerror;
			}
			}
		|	'-' iexpr %prec UMINUS
			{ $$ = - $2; }
		|	'(' iexpr ')'
			{ $$ = $2; }
		;

/* Units */
unit:	UNIT_PT
		{ $$ = "pt"; }
	|	UNIT_PC
		{ $$ = "pc"; }
		;

%%

#include <limits.h>

int lineno = 0;
char curfile[_POSIX_PATH_MAX];
char outfile[_POSIX_PATH_MAX];
char *progname = "mif2sgml";
char *usage = "%s: usage infile [outfile]\n";
#define DEFAULT_OUTFILE "mif2sgml.out"

#include "lex.yy.c"

yyerror( char* s )
{
    fprintf(stderr, "%s in line %d\n", s, lineno+1, curfile );
}


main(int argc, char **argv)
{
	char *infile;
	extern FILE *yyin;
    
	progname = argv[0];
    
	if(argc > 3 || argc <= 1 )
	{
        	fprintf(stderr,usage, progname);
		exit(1);
	}

    fileList = argv+1;
	nFiles = argc-1;

	if(argc > 1)
	{
		infile = argv[1];
		/* open for read */
		yyin = fopen(infile,"r");
		if(yyin == NULL) /* open failed */
		{
			fprintf(stderr,"%s: cannot open %s\n", 
				progname, infile);
			exit(1);
		}
	}

	if(argc > 2)
	{
		strcpy( outfile, argv[2] );
	}
	else
	{
      		strcpy( outfile, DEFAULT_OUTFILE );
	}
    
	/* normal interaction on yyin from now on */
    
	yyparse();
    
	/* now check EOF condition */
	if(!decl_done) /* in the middle of a tag */
	{
        	warning("Premature EOF",(char *)0);
		unlink(outfile); /* remove bad file */
		exit(1);
	}

	/* Change this for doing other things than generating SGML. */
	//	generate_sgml( outfile );

	generate_xml( outfile );

	exit(0); /* no error */
}

void warning(char *s, char *t) /* print warning message */
{
	fprintf(stderr, "%s: %s", progname, s);
	if (t)
		fprintf(stderr, " %s", t);
	fprintf(stderr, " line %d\n", lineno);
}
