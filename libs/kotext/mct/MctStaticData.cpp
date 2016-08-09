/* This file is part of the KDE project
 * Copyright (C) 2015-2016 MultiRacio Ltd. <multiracio@multiracio.com> (S.Schliszka, F.Novak, P.Rakyta)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "MctStaticData.h"
#include "MctChange.h"
#include "MctPosition.h"
#include "MctChangeTypes.h"
#include "MctEmbObjProperties.h"
#include "MctUndoGraph.h"
#include "MctRedoGraph.h"
#include "MctCell.h"
#include "MctChangeEntities.h"
#include "MctChangeTypes.h"

#include "kzip.h"

#include <QTextBlock>

#include <KoTextDocument.h>
#include <KoTextEditor.h>
#include "KoDocument.h"
#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>
#include <KoListStyle.h>

#include <algorithm>

#define MAPINSERT(item) map.insert(item, #item)

MctStaticData* MctStaticData::m_Instance = 0;

const ulong MctStaticData::UNDEFINED = ULONG_MAX;


const QString MctStaticData::EXPORTED_BY_SYSTEM = "exported by system on document save";

const QString MctStaticData::UNDOTAG = "ChangeGroup";
const QString MctStaticData::REDOTAG = "ChangeGroup";

const QString MctStaticData::UNDOCHANGES = "undo";
const QString MctStaticData::REDOCHANGES = "redo";

//names of the files containing undo/redo operations
const QString MctStaticData::UNDOFILE = "undo.xml";
const QString MctStaticData::REDOFILE = "redo.xml";

//static variables to parse position data from XML
const QString MctStaticData::POSSEPARATOR = "/";
const QString MctStaticData::POSSTART = "s";
const QString MctStaticData::POSEND = "e";
const QString MctStaticData::MPOSSTART = "ds";
const QString MctStaticData::MPOSEND = "de";
const QString MctStaticData::TABLE = "table";
const QString MctStaticData::CELL = "cell";
const QString MctStaticData::MOVEDTABLE = "dtable";
const QString MctStaticData::MOVEDCELL = "dcell";

//static variables to parse time data from XML
const QString MctStaticData::DATESEP = "-";
const QString MctStaticData::TIMESEP = ":";
const QString MctStaticData::DATETIMESEP = "T";

// static variables to store URL paths
const QString MctStaticData::GRAPHICOBJ = "vnd.sun.star.GraphicObject:";
const QString MctStaticData::GRAPHICOBJ_PATH = "Pictures/";

const QChar MctStaticData::REDOCHAR = 'r';

// static variables for xml namespaces (nsURI)
const XMLns MctStaticData::XMLNS = "xmlns";
const XMLns MctStaticData::NS_DC = "dc";
const XMLns MctStaticData::NS_C = "c";
const XMLns MctStaticData::NS_OFFICE = "office";

// custom static attribute strings for undo/redo XML
// xmlns:office
const QString MctStaticData::VERSION = "version";
// xmlns:dc
const QString MctStaticData::STARTINGSTRING = "started"; // mct enabled date
const QString MctStaticData::AUTHOR = "author";
const QString MctStaticData::COMMENT = "comment";
const QString MctStaticData::DATE = "date";
const QString MctStaticData::TYPE = "type";
// xmlns:c - custom attributes
const QString MctStaticData::ID = "id";
const QString MctStaticData::NAME = "name";
const QString MctStaticData::ROW = "row";
const QString MctStaticData::COL = "col";
const QString MctStaticData::URL = "URL";
const QString MctStaticData::POSX = "PositionX";
const QString MctStaticData::POSY = "PositionY";
const QString MctStaticData::PREVPOSX = "PrevPositionX";
const QString MctStaticData::PREVPOSY = "PrevPositionY";
const QString MctStaticData::HEIGHT = "Height";
const QString MctStaticData::WIDTH = "Width";
const QString MctStaticData::ROTATION = "Rotation";
const QString MctStaticData::PARENT = "parent";

// xml string constants (tags, attribute values, etc)
const QString MctStaticData::ADDED = "del";
const QString MctStaticData::REMOVED = "add";
const QString MctStaticData::MOVED = "move";
const QString MctStaticData::STRING = "text";
const QString MctStaticData::PARAGRAPH = "par";
const QString MctStaticData::STYLE = "style";
const QString MctStaticData::TEXTFRAME = "textframe";
const QString MctStaticData::TEXTGRAPHICOBJECT = "graphobj";
const QString MctStaticData::EMBEDDEDOBJECT = "embobj";
const QString MctStaticData::ROWCHANGE = "row";
const QString MctStaticData::COLCHANGE = "col";
//const QString MctStaticData::TABLE = "table";
const QString MctStaticData::FORMATTAG = "format";
const QString MctStaticData::PROPERTIESTAG = "properties";
const QString MctStaticData::TEXTTAG = "text";
const QString MctStaticData::PARAGRAPHTAG = "paragraph";
const QString MctStaticData::LIST = "list";
const QString MctStaticData::UNDEFINEDTAG = "undef";


//properties stored as integers
const QSet<QString> MctStaticData::TEXTGRAPHICOBJECTS_INTPROPS = QSet<QString>() << "BackColorTransparency"
 <<  "BackGraphicTransparency" << "VertOrientPosition" << "AdjustRed" << "HoriOrient" << "LeftBorderDistance"
 << "RightMargin" << "BackColorRGB" << "WrapInfluenceOnPosition" << "AdjustLuminance" << "AdjustGreen"
 << "Width" << "VertOrientRelation" << "HoriOrientPosition" << "RelativeHeight" << "BottomBorderDistance"
 << "AdjustContrast" << "HoriOrientRelation" << "VertOrient" << "ZOrder" << "BackColor" << "LeftMargin"
 << "TopMargin" << "AnchorPageNo" << "BottomMargin" << "TopBorderDistance" << "Transparency" << "GraphicRotation"
 << "RightBorderDistance" << "RelativeWidth" << "Height" << "AdjustBlue" << "BorderDistance";

//properties stored as structures
const QSet<QString> MctStaticData::TEXTGRAPHICOBJECTS_STRUCTPROPS = QSet<QString>() << "LeftBorder"
 << "GraphicCrop" << "TopBorder" << "RightBorder" << "ActualSize" << "UserDefinedAttributes"
 << "BackGraphicLocation" << "ShadowFormat" << "ImageMap" << "TextWrap" << "AnchorType" << "BottomBorder"
 << "GraphicColorMode" << "GraphicFilter" << "AnchorFrame" << "Surround" << "BackGraphicFilter"
 << "Size" << "AnchorTypes" << "LayoutSize" << "Graphic";

//properties stored as floats
const QSet<QString> MctStaticData::TEXTGRAPHICOBJECTS_FLOATPROPS = QSet<QString>() << "Gamma";

//properties stored as srtrings
const QSet<QString> MctStaticData::TEXTGRAPHICOBJECTS_STRINGPROPS = QSet<QString>() << "ReplacementGraphicURL"
 << "ContourPolyPolygon" << "HyperLinkName" << "Description" << "HyperLinkTarget" << "GraphicURL"
 << "FrameStyleName" << "Title" << "BackGraphicURL" << "HyperLinkURL" ;

//## properties stored as booleans
const QSet<QString> MctStaticData::TEXTGRAPHICOBJECTS_BOOLEANPROPS = QSet<QString>() << "SurroundAnchorOnly"
 << "GraphicIsInverted" << "BackTransparent" << "Opaque" << "ContourOutside" << "SizeProtected"
 << "IsSyncWidthToHeight" << "HoriMirroredOnEvenPages" << "HoriMirroredOnOddPages" << "IsFollowingTextFlow"
 << "ContentProtected" << "VertMirrored" << "PageToggle" << "IsSyncHeightToWidth" << "IsPixelContour"
 << "IsAutomaticContour" << "SurroundContour" << "PositionProtected" << "ServerMap" << "Print";

///properties of texttable
// properties stored as structures
const QSet<QString> MctStaticData::TEXTTABLE_STRUCTPROPS = QSet<QString>() << "TableBorder"
 << "ShadowFormat"<< "TableBorderDistances"<< "UserDefinedAttributes"<< "TableColumnSeparators";

// properties stored as structures
const QSet<QString> MctStaticData::TEXTTABLE_ENUMPROPS = QSet<QString>() << "AnchorType"
 << "AnchorTypes"<< "BackGraphicLocation"<< "BreakType"<< "TextSection" << "TextWrap";

const QSet<QString> MctStaticData::TEXT_PROPS_BOOLEAN = QSet<QString>() <<
  "FontItalic" << "AutoTextIndent" << "LineSpacingFromFont" << "BlockNonBreakableLines";

const QSet<QString> MctStaticData::TEXT_PROPS_INT = QSet<QString>() <<
  "FontWeight" << "FontPointSize" <<  "TextUnderlineStyle" << "UnderlineType" << "UnderlineStyle" <<
  "StrikeOutStyle" << "StrikeOutType" << "TextVerticalAlignment" << "FontCapitalization" <<
  "BlockAlignment" << "BreakAfter" << "BreakBefore" << "OrphanThreshold";

const QSet<QString> MctStaticData::TEXT_PROPS_DOUBLE = QSet<QString>() <<
  "BlockTopMargin" << "BlockBottomMargin" << "BlockLeftMargin" << "BlockRightMargin" <<
  "TextIndent" <<  "PercentLineHeight" << "FixedLineHeight" << "LineSpacing" << "LineHeight";
const QSet<QString> MctStaticData::TEXT_PROPS_STRING = QSet<QString>() <<
  "FontFamily" << "BackgroundBrush" << "ForegroundBrush";

const QMap<int, QString> MctStaticData::initTEXT_PROPS_AS_STRING(){
    QMap<int, QString> map;
    /*Paragraph and character props*/
     map.insert(QTextFormat::CssFloat,"CssFloat");
     map.insert(QTextFormat::LayoutDirection, "LayoutDirection");
     map.insert(QTextFormat::ForegroundBrush, "ForegroundBrush");
     map.insert(QTextFormat::BackgroundBrush, "BackgroundBrush");
     map.insert(QTextFormat::BackgroundImageUrl, "BackgroundImageUrl");
    /*Paragraph props*/
     map.insert(QTextFormat::BlockAlignment, "BlockAlignment"); map.insert(QTextFormat::BlockTopMargin, "BlockTopMargin");
     map.insert(QTextFormat::BlockBottomMargin, "BlockBottomMargin"); map.insert(QTextFormat::BlockLeftMargin, "BlockLeftMargin");
     map.insert(QTextFormat::BlockRightMargin, "BlockRightMargin"); map.insert(QTextFormat::TextIndent, "TextIndent");
     map.insert(QTextFormat::TabPositions, "TabPositions"); map.insert(QTextFormat::BlockIndent, "BlockIndent");
     map.insert(QTextFormat::LineHeight, "LineHeight"); map.insert(QTextFormat::BlockNonBreakableLines, "BlockNonBreakableLines");
    /*character props*/
     map.insert(QTextFormat::FontFamily, "FontFamily"); map.insert(QTextFormat::FontPointSize, "FontPointSize");
     map.insert(QTextFormat::FontPixelSize, "FontPixelSize"); map.insert(QTextFormat::FontSizeAdjustment, "FontSizeAdjustment");
     map.insert(QTextFormat::FontWeight, "FontWeight"); map.insert(QTextFormat::FontItalic, "FontItalic");
     map.insert(QTextFormat::FontUnderline, "FontUnderline"); map.insert(QTextFormat::FontOverline, "FontOverline");
     map.insert(QTextFormat::FontKerning, "FontKerning"); map.insert(QTextFormat::FontHintingPreference, "FontHintingPreference");
     map.insert(QTextFormat::FontStrikeOut, "FontStrikeOut"); map.insert(QTextFormat::TextUnderlineColor, "TextUnderlineColor");
     map.insert(QTextFormat::TextVerticalAlignment, "TextVerticalAlignment"); map.insert(QTextFormat::TextOutline, "TextOutline");
     map.insert(QTextFormat::TextUnderlineStyle, "TextUnderlineStyle"); map.insert(QTextFormat::TextToolTip, "TextToolTip");
     map.insert(QTextFormat::IsAnchor, "IsAnchor"); map.insert(QTextFormat::AnchorHref, "AnchorHref");
     map.insert(QTextFormat::AnchorName, "AnchorName"); map.insert(QTextFormat::ObjectType, "ObjectType");
     map.insert(QTextFormat::FontCapitalization, "FontCapitalization");
    /*list props*/
     map.insert(QTextFormat::ListStyle, "ListStyle"); map.insert(QTextFormat::ListIndent, "ListIndent");
     map.insert(QTextFormat::ListNumberPrefix, "ListNumberPrefix"); map.insert(QTextFormat::ListNumberSuffix, "ListNumberSuffix");
    /*table and frame props*/
     map.insert(QTextFormat::FrameBorder, "FrameBorder"); map.insert(QTextFormat::FrameMargin, "FrameMargin");
     map.insert(QTextFormat::FramePadding, "FramePadding"); map.insert(QTextFormat::FrameWidth, "FrameWidth");
     map.insert(QTextFormat::FrameHeight, "FrameHeight"); map.insert(QTextFormat::FrameTopMargin, "FrameTopMargin");
     map.insert(QTextFormat::FrameBottomMargin, "FrameBottomMargin"); map.insert(QTextFormat::FrameLeftMargin, "FrameLeftMargin");
     map.insert(QTextFormat::FrameRightMargin, "FrameRightMargin"); map.insert(QTextFormat::FrameBorderBrush, "FrameBorderBrush");
     map.insert(QTextFormat::FrameBorderStyle, "FrameBorderStyle"); map.insert(QTextFormat::TableColumns, "TableColumns");
     map.insert(QTextFormat::TableColumnWidthConstraints, "TableColumnWidthConstraints"); map.insert(QTextFormat::TableCellSpacing, "TableCellSpacing");
     map.insert(QTextFormat::TableCellPadding, "TableCellPadding"); map.insert(QTextFormat::TableHeaderRowCount, "TableHeaderRowCount");
     map.insert(QTextFormat::TableCellRowSpan, "TableCellRowSpan"); map.insert(QTextFormat::TableCellColumnSpan, "TableCellColumnSpan");
     map.insert(QTextFormat::TableCellTopPadding, "TableCellTopPadding"); map.insert(QTextFormat::TableCellBottomPadding, "TableCellBottomPadding");
     map.insert(QTextFormat::TableCellLeftPadding, "TableCellLeftPadding"); map.insert(QTextFormat::TableCellRightPadding, "TableCellRightPadding");
    /*image props*/
     map.insert(QTextFormat::ImageName, "ImageName"); map.insert(QTextFormat::ImageWidth, "ImageWidth");
     map.insert(QTextFormat::ImageHeight, "ImageHeight");
    return map;
}

const QMap<int, QString> MctStaticData::TEXT_PROPS_AS_STRING = initTEXT_PROPS_AS_STRING();

const QMap<int, QString> MctStaticData::initTEXT_KO_CHAR_PROPS_AS_STRING(){
    QMap<int, QString> map;
    map.insert(KoCharacterStyle::StrikeOutStyle,"StrikeOutStyle"); map.insert(KoCharacterStyle::StrikeOutType, "StrikeOutType");
    map.insert(KoCharacterStyle::StrikeOutColor,"StrikeOutColor"); map.insert(KoCharacterStyle::StrikeOutWidth, "StrikeOutWidth");
    map.insert(KoCharacterStyle::StrikeOutWeight,"StrikeOutWeight"); map.insert(KoCharacterStyle::StrikeOutMode, "StrikeOutMode");
    map.insert(KoCharacterStyle::StrikeOutText,"StrikeOutText"); map.insert(KoCharacterStyle::OverlineStyle, "OverlineStyle");
    map.insert(KoCharacterStyle::OverlineType,"OverlineType"); map.insert(KoCharacterStyle::OverlineColor, "OverlineColor");
    map.insert(KoCharacterStyle::OverlineWidth,"OverlineWidth"); map.insert(KoCharacterStyle::OverlineWeight, "OverlineWeight");
    map.insert(KoCharacterStyle::OverlineMode,"OverlineMode"); map.insert(KoCharacterStyle::UnderlineStyle, "UnderlineStyle");
    map.insert(KoCharacterStyle::UnderlineType,"UnderlineType"); map.insert(KoCharacterStyle::UnderlineWidth, "UnderlineWidth");
    map.insert(KoCharacterStyle::UnderlineWeight,"UnderlineWeight"); map.insert(KoCharacterStyle::UnderlineMode, "UnderlineMode");
    map.insert(KoCharacterStyle::Language,"Language"); map.insert(KoCharacterStyle::Country, "Country");
    map.insert(KoCharacterStyle::FontCharset,"FontCharset"); map.insert(KoCharacterStyle::TextRotationAngle, "TextRotationAngle");
    map.insert(KoCharacterStyle::TextRotationScale,"TextRotationScale"); map.insert(KoCharacterStyle::TextScale, "TextScale");
    map.insert(KoCharacterStyle::InlineRdf,"InlineRdf"); map.insert(KoCharacterStyle::TextShadow, "TextShadow");
    map.insert(KoCharacterStyle::FontRelief,"FontRelief"); map.insert(KoCharacterStyle::TextEmphasizeStyle, "TextEmphasizeStyle");
    map.insert(KoCharacterStyle::TextEmphasizePosition,"TextEmphasizePosition"); map.insert(KoCharacterStyle::TextCombine, "TextCombine");
    map.insert(KoCharacterStyle::TextCombineStartChar,"TextCombineStartChar"); map.insert(KoCharacterStyle::TextCombineEndChar, "TextCombineEndChar");
    map.insert(KoCharacterStyle::HyphenationPushCharCount,"HyphenationPushCharCount"); map.insert(KoCharacterStyle::HyphenationRemainCharCount, "HyphenationRemainCharCount");
    map.insert(KoCharacterStyle::FontLetterSpacing,"FontLetterSpacing"); map.insert(KoCharacterStyle::PercentageFontSize, "PercentageFontSize");
    map.insert(KoCharacterStyle::AdditionalFontSize,"AdditionalFontSize"); map.insert(KoCharacterStyle::UseWindowFontColor, "UseWindowFontColor");
    map.insert(KoCharacterStyle::Blink,"Blink"); map.insert(KoCharacterStyle::AnchorType, "AnchorType");
    map.insert(KoCharacterStyle::InlineInstanceId,"InlineInstanceId"); map.insert(KoCharacterStyle::ChangeTrackerId, "ChangeTrackerId");
    map.insert(KoCharacterStyle::FontYStretch,"FontYStretch");
    return map;
}

const QMap<int, QString> MctStaticData::TEXT_KO_CHAR_PROPS_AS_STRING = initTEXT_KO_CHAR_PROPS_AS_STRING();

const QMap<int, QString> MctStaticData::initTEXT_KO_PAR_PROPS_AS_STRING() {
    QMap<int, QString> map;
    map.insert(KoParagraphStyle::PercentLineHeight,"PercentLineHeight"); map.insert(KoParagraphStyle::FixedLineHeight, "FixedLineHeight");
    map.insert(KoParagraphStyle::MinimumLineHeight,"MinimumLineHeight"); map.insert(KoParagraphStyle::LineSpacing, "LineSpacing");
    map.insert(KoParagraphStyle::LineSpacingFromFont,"LineSpacingFromFont"); map.insert(KoParagraphStyle::AlignLastLine, "AlignLastLine");
    map.insert(KoParagraphStyle::WidowThreshold,"WidowThreshold"); map.insert(KoParagraphStyle::OrphanThreshold, "OrphanThreshold");
    map.insert(KoParagraphStyle::DropCaps,"DropCaps"); map.insert(KoParagraphStyle::DropCapsLength, "DropCapsLength");
    map.insert(KoParagraphStyle::DropCapsLines,"DropCapsLines"); map.insert(KoParagraphStyle::DropCapsDistance, "DropCapsDistance");
    map.insert(KoParagraphStyle::DropCapsTextStyle,"DropCapsTextStyle"); map.insert(KoParagraphStyle::FollowDocBaseline, "FollowDocBaseline");
    map.insert(KoParagraphStyle::HasLeftBorder,"HasLeftBorder"); map.insert(KoParagraphStyle::HasTopBorder, "HasTopBorder");
    map.insert(KoParagraphStyle::HasRightBorder,"HasRightBorder"); map.insert(KoParagraphStyle::HasBottomBorder, "HasBottomBorder");
    map.insert(KoParagraphStyle::BorderLineWidth,"BorderLineWidth"); map.insert(KoParagraphStyle::SecondBorderLineWidth, "SecondBorderLineWidth");
    map.insert(KoParagraphStyle::DistanceToSecondBorder,"DistanceToSecondBorder"); map.insert(KoParagraphStyle::LeftPadding, "LeftPadding");
    map.insert(KoParagraphStyle::TopPadding,"TopPadding"); map.insert(KoParagraphStyle::RightPadding, "RightPadding");
    map.insert(KoParagraphStyle::BottomPadding,"BottomPadding"); map.insert(KoParagraphStyle::LeftBorderWidth, "LeftBorderWidth");
    map.insert(KoParagraphStyle::LeftInnerBorderWidth,"LeftInnerBorderWidth"); map.insert(KoParagraphStyle::LeftBorderSpacing, "LeftBorderSpacing");
    map.insert(KoParagraphStyle::LeftBorderStyle,"LeftBorderStyle"); map.insert(KoParagraphStyle::LeftBorderColor, "LeftBorderColor");
    map.insert(KoParagraphStyle::TopBorderWidth,"TopBorderWidth"); map.insert(KoParagraphStyle::TopInnerBorderWidth, "TopInnerBorderWidth");
    map.insert(KoParagraphStyle::TopBorderSpacing,"TopBorderSpacing"); map.insert(KoParagraphStyle::FollowDocBaseline, "FollowDocBaseline");
    map.insert(KoParagraphStyle::TopBorderColor,"TopBorderColor"); map.insert(KoParagraphStyle::RightBorderWidth, "RightBorderWidth");
    map.insert(KoParagraphStyle::RightInnerBorderWidth,"RightInnerBorderWidth"); map.insert(KoParagraphStyle::RightBorderSpacing, "RightBorderSpacing");
    map.insert(KoParagraphStyle::RightBorderStyle,"RightBorderStyle"); map.insert(KoParagraphStyle::RightBorderColor, "RightBorderColor");
    map.insert(KoParagraphStyle::BottomBorderWidth,"BottomBorderWidth"); map.insert(KoParagraphStyle::BottomInnerBorderWidth, "BottomInnerBorderWidth");
    map.insert(KoParagraphStyle::BottomBorderSpacing,"BottomBorderSpacing"); map.insert(KoParagraphStyle::BottomBorderStyle, "BottomBorderStyle");
    map.insert(KoParagraphStyle::BottomBorderColor,"BottomBorderColor"); map.insert(KoParagraphStyle::ListStyleId, "ListStyleId");
    map.insert(KoParagraphStyle::ListStartValue,"ListStartValue"); map.insert(KoParagraphStyle::RestartListNumbering, "RestartListNumbering");
    map.insert(KoParagraphStyle::ListLevel,"ListLevel"); map.insert(KoParagraphStyle::IsListHeader, "IsListHeader");
    map.insert(KoParagraphStyle::UnnumberedListItem,"UnnumberedListItem"); map.insert(KoParagraphStyle::AutoTextIndent, "AutoTextIndent");
    map.insert(KoParagraphStyle::TabStopDistance,"TabStopDistance"); map.insert(KoParagraphStyle::TabPositions, "TabPositions");
    map.insert(KoParagraphStyle::TextProgressionDirection,"TextProgressionDirection"); map.insert(KoParagraphStyle::MasterPageName, "MasterPageName");
    map.insert(KoParagraphStyle::OutlineLevel,"OutlineLevel"); map.insert(KoParagraphStyle::DefaultOutlineLevel, "DefaultOutlineLevel");
    map.insert(KoParagraphStyle::LineNumbering,"LineNumbering"); map.insert(KoParagraphStyle::LineNumberStartValue, "LineNumberStartValue");
    map.insert(KoParagraphStyle::BreakBefore, "BreakBefore"); map.insert(KoParagraphStyle::BreakAfter, "BreakAfter");
    ///TODO
    return map;
}

const QMap<int, QString> MctStaticData::TEXT_KO_PAR_PROPS_AS_STRING = initTEXT_KO_PAR_PROPS_AS_STRING();

const QMap<int, QString> MctStaticData::initTEXT_KO_LIST_PROPS_AS_STRING(){
    QMap<int, QString> map;
    map.insert(KoListStyle::ListItemPrefix,"ListItemPrefix"); map.insert(KoListStyle::ListItemSuffix, "ListItemSuffix");
    map.insert(KoListStyle::StartValue,"StartValue"); map.insert(KoListStyle::Level, "Level"); map.insert(KoListStyle::DisplayLevel, "DisplayLevel");
    map.insert(KoListStyle::CharacterStyleId,"CharacterStyleId"); map.insert(KoListStyle::CharacterProperties, "CharacterProperties");
    map.insert(KoListStyle::BulletCharacter,"BulletCharacter"); map.insert(KoListStyle::RelativeBulletSize, "RelativeBulletSize");
    map.insert(KoListStyle::Alignment,"Alignment"); map.insert(KoListStyle::MinimumWidth, "MinimumWidth");
    map.insert(KoListStyle::ListId,"ListId"); map.insert(KoListStyle::IsOutline, "IsOutline");
    map.insert(KoListStyle::LetterSynchronization,"LetterSynchronization"); map.insert(KoListStyle::StyleId, "StyleId");
    map.insert(KoListStyle::Indent,"Indent"); map.insert(KoListStyle::MinimumDistance, "MinimumDistance");
    map.insert(KoListStyle::Width,"Width"); map.insert(KoListStyle::Height, "Height");
    map.insert(KoListStyle::BulletImage,"BulletImage"); map.insert(KoListStyle::Margin, "Margin");
    map.insert(KoListStyle::TextIndent,"TextIndent"); map.insert(KoListStyle::AlignmentMode, "AlignmentMode");
    map.insert(KoListStyle::LabelFollowedBy,"LabelFollowedBy"); map.insert(KoListStyle::TabStopPosition, "TabStopPosition");
    return map;
}

const QMap<int, QString> MctStaticData::TEXT_KO_LIST_PROPS_AS_STRING = initTEXT_KO_LIST_PROPS_AS_STRING();


int MctStaticData::getTextPropAsInt(QString propertyname)
{
    foreach (int key, TEXT_PROPS_AS_STRING.keys()) {
        if(TEXT_PROPS_AS_STRING.value(key) == propertyname) {
            return key;
        }
    }
    foreach (int key, TEXT_KO_CHAR_PROPS_AS_STRING.keys()) {
        if(TEXT_KO_CHAR_PROPS_AS_STRING.value(key) == propertyname) {
            return key;
        }
    }
    foreach (int key, TEXT_KO_PAR_PROPS_AS_STRING.keys()) {
        if(TEXT_KO_PAR_PROPS_AS_STRING.value(key) == propertyname) {
            return key;
        }
    }
    foreach (int key, TEXT_KO_LIST_PROPS_AS_STRING.keys()) {
        if(TEXT_KO_LIST_PROPS_AS_STRING.value(key) == propertyname) {
            return key;
        }
    }
    return -1;
}

QString MctStaticData::getTextPropetyString(int key, int type)
{    
    QString propName = "";
    if(type == CharacterProperty){
        if(TEXT_PROPS_AS_STRING.contains(key)) {
            propName = TEXT_PROPS_AS_STRING[key];
        } else if (TEXT_KO_CHAR_PROPS_AS_STRING.contains(key)) {
            propName = TEXT_KO_CHAR_PROPS_AS_STRING[key];
        }
    } else if (type == ParagraphProperty) {
        if(TEXT_PROPS_AS_STRING.contains(key)) {
            propName = TEXT_PROPS_AS_STRING[key];
        } else if (TEXT_KO_PAR_PROPS_AS_STRING.contains(key)) {
            propName = TEXT_KO_PAR_PROPS_AS_STRING[key];
        }
    } else if (type == ListProperty) {
        if(TEXT_PROPS_AS_STRING.contains(key)) {
            propName = TEXT_PROPS_AS_STRING[key];
        } else if (TEXT_KO_LIST_PROPS_AS_STRING.contains(key)) {
            propName = TEXT_KO_LIST_PROPS_AS_STRING[key];
        }
    } else {
        propName = "unknown";
    }
/*
    if(TEXT_PROPS_AS_STRING.contains(key)) {
        qDebug() << "TEXT_PROPS_AS_STRING contains key: " << key << " name: " << TEXT_PROPS_AS_STRING[key];
    }
    if (TEXT_KO_CHAR_PROPS_AS_STRING.contains(key)) {
        qDebug() << "TEXT_KO_CHAR_PROPS_AS_STRING contains key: " << key << " name: " << TEXT_KO_CHAR_PROPS_AS_STRING[key];
    }
    if (TEXT_KO_PAR_PROPS_AS_STRING.contains(key)) {
        qDebug() << "TEXT_KO_PAR_PROPS_AS_STRING contains key: " << key << " name: " << TEXT_KO_PAR_PROPS_AS_STRING[key];
    }
    if (TEXT_KO_LIST_PROPS_AS_STRING.contains(key)) {
        qDebug() << "TEXT_KO_LIST_PROPS_AS_STRING contains key: " << key << " name: " << TEXT_KO_LIST_PROPS_AS_STRING[key];
    }*/

    return propName;
}

const QMap<MctChangeTypes, QString> MctStaticData::initCHANGE_AS_STRING()
{
    QMap<MctChangeTypes, QString> map;
    MAPINSERT(AddedString);
    MAPINSERT(RemovedString);
    MAPINSERT(RemovedStringInTable);
    MAPINSERT(MovedStringInTable);
    MAPINSERT(ParagraphBreak);
    MAPINSERT(ParagraphBreakInTable);
    MAPINSERT(DelParagraphBreak);
    MAPINSERT(DelParagraphBreakInTable);
    MAPINSERT(StyleChange);
    MAPINSERT(StyleChangeInTable);
    MAPINSERT(AddedTextFrame);
    MAPINSERT(AddedTextFrameInTable);
    MAPINSERT(RemovedTextFrameInTable);
    MAPINSERT(AddedTextGraphicObject);
    MAPINSERT(AddedTextGraphicObjectInTable);
    MAPINSERT(RemovedTextGraphicObject);
    MAPINSERT(RemovedTextGraphicObjectInTable);
    MAPINSERT(AddedEmbeddedObject);
    MAPINSERT(AddedEmbeddedObjectInTable);
    MAPINSERT(RemovedEmbeddedObject);
    MAPINSERT(RemovedEmbeddedObjectInTable);
    MAPINSERT(AddedTextTable);
    MAPINSERT(AddedTextTableInTable);
    MAPINSERT(RemovedTextTableInTable);
    MAPINSERT(AddedRowInTable);
    MAPINSERT(RemovedRowInTable);
    MAPINSERT(AddedColInTable);
    MAPINSERT(RemovedColInTable);
    return map;
}

const QMap<MctChangeTypes, QString> MctStaticData::CHANGE_AS_STRING = initCHANGE_AS_STRING();

int MctStaticData::currentRevision = -1;

MctStaticData::MctStaticData() {
    redograph = NULL;
    undograph = NULL;
    changes = new QList<MctChange *>();
    kodoc = NULL;
    mctState = false;
}

MctUndoGraph* MctStaticData::getUndoGraph()
{
    return undograph;
}

void MctStaticData::setUndoGraph(MctUndoGraph *graph)
{
    undograph = graph;
}

MctRedoGraph* MctStaticData::getRedoGraph()
{
    return redograph;
}

void MctStaticData::setRedoGraph(MctRedoGraph *graph)
{
    redograph = graph;
}

template <typename T> void MctStaticData::reverseList(QList<T> **list)
{
    QList<T> *result = new QList<T>;
    result->reserve((*list)->size());
    std::reverse_copy((*list)->begin(), (*list)->end(), std::back_inserter(*result));
    delete *list;
    *list = result;
}

template <typename T> void MctStaticData::reverseList2(QList<T> *list)
{
    QList<T> result;
    result.reserve(list->size());
    std::reverse_copy(list->begin(), list->end(), std::back_inserter(result));
    *list = result;
}

/// functions.py
//-------------------------------------------------------------------
/**
 * @brief This creates the temporary file path of the MCT tracket document
 * @param url The original URL of the document
 * @return Returns with the temporary file path.
 */
QString MctStaticData::tempMctFile(QString url)
{
    QString outFile = url + "_tmp.odt";
    return outFile;
}

/**
 * @brief This zips embedded files into the Odt when graph is exported.
 * @param delemet Dictionary of embedded objects stored in staticvars
 */
void MctStaticData::includeEmbeddedFiles(QMap<QString, MctEmbObjProperties *> delemet)
{
    foreach (MctEmbObjProperties* value, delemet.values()) {
        if(value->isBackedup()) {
            if( ! value->wasInserted()) {
                value->restoreBackupFile();
            }
        }
    }
}

/**
 * @brief This backs up embedded files from the Odt when graph is exported.
 * @param delemet Dictionary of embedded objects stored in staticvars
 */
void MctStaticData::backupEmbeddedFiles(QMap<QString, MctEmbObjProperties *> delemet)
{
    foreach (MctEmbObjProperties* value, delemet.values()) {
        if( ! value->isBackedup()) {
            value->backupFileFromOdt(value->innerUrl(), value->odtURL());
        }
    }
}

/**
 * @brief his deletes embedded files corresponding to container delement.
 * @param delemet Dictionary of embedded objects stored in staticvars
 */
void MctStaticData::deleteEmbeddedFiles(QMap<QString, MctEmbObjProperties *> delemet)
{
    foreach (MctEmbObjProperties* value, delemet.values()) {
        if( value->isBackedup()) {
            value->deleteBackupFile();
        }
    }
}


void MctStaticData::exportGraphs()
{
    KoTextDocument* koTextDoc = undograph->koTextDoc();
    QMap<ulong, ulong> *indexes = getFrameIndexes(koTextDoc);

    QDomDocument copyUndoDoc = undograph->doc()->cloneNode(true).toDocument();

    //undograph->correctBlockPositionForExport(&copyUndoDoc, indexes);


    QDomDocument copyRedoDoc = redograph->doc()->cloneNode(true).toDocument();

    //redograph->correctBlockPositionForExport(&copyRedoDoc, indexes);


    /* Csak egy másolatot exportálunk, mert az exportálás előtt szükség van
     * a block pozíciók korrigálására */
    undograph->exportGraph(&copyUndoDoc);
    redograph->exportGraph(&copyRedoDoc);
    qCritical() << "TODO";
//TODO
    //includeEmbeddedFiles(dTextFrames);
    //includeEmbeddedFiles(dTextGraphicObjects);
    //includeEmbeddedFiles(dTextEmbeddedObjects);
    //includeEmbeddedFiles(dRemovedObjects);

    //backupEmbeddedFiles(dTextFrames);
    //backupEmbeddedFiles(dTextGraphicObjects);
    //backupEmbeddedFiles(dTextEmbeddedObjects);
}

void MctStaticData::attachListeners()
{
    // TODO
}

void MctStaticData::fillUpWithSuppliers()
{
    // TODO
}

void MctStaticData::fillUpWithSupplier()
{
    // TODO
}

void MctStaticData::fillUpWithSupplier2()
{
    // TODO
}

/**
 * @brief MctStaticData::stopSupport turns off MCT support
 * @param URL
 */
void MctStaticData::stopSupport(QString URL)
{
    if (URL.isEmpty()) {
        qDebug() << "The given URL is empty!";
    } else {
        // TODO
    }
}

//---------------------------------------------------------------------

/// functions2.py
//---------------------------------------------------------------------

/**
 * @brief This function gets data from position tag and arrange it into a list
 * @param postag The position tag
 * @return Returns with the list of data extracted from position tag
 */
QList<QString> MctStaticData::getDataFromPosTag(const QString &postag)
{
    QList<QString> data;
    int idx = 0;
    while(true) {
        int idx_new = postag.indexOf(POSSEPARATOR, idx + 1);
        if(idx_new == -1) {
            data.append(postag.mid(idx + 1));
            break;
        } else {
            data.append(postag.mid(idx + 1, idx_new - (idx + 1)));
            idx = idx_new;
        }
    }
    return data;
}

/**
 * @brief This function make position class from string extracted from the XML node.
 * @param change The XML node.
 * @param moved True if moved position is wanted, False otherwise
 * @return Returns with the created position class.
 */
MctPosition* MctStaticData::getPosFromElementreeNode(const QDomElement &change, bool moved)
{
    MctPosition *pos = getPosFromElementreeNodeNew(change, moved);

    if(pos == NULL) {
        pos = getPosFromElementreeNodeOld(change, moved);
    }

    return pos;
}

/**
 * @brief This function make position class from string extracted from the XML node. --- new
 * The index start from 1 in the xml, we need to compensate that.
 * @param change The XML node.
 * @param moved True if moved position is wanted, False otherwise
 * @return Returns with the created position class.
 */
MctPosition* MctStaticData::getPosFromElementreeNodeNew(const QDomElement &change, bool moved)
{
    MctPosition * pos = nullptr;
    MctPosition * pos_parent = nullptr;
    QList<QString> between_separators_start;
    QList<QString> between_separators_end;
    QString start;
    QString end;

    ulong startchar;
    ulong startpar;
    ulong endchar;
    ulong endpar;

    MctCell * cellinf;
    MctCell * cellinfend;

    if(moved) {
        start = change.attribute(MPOSSTART);
        end = change.attribute(MPOSEND);
    } else {
        start = change.attribute(POSSTART);
        end = change.attribute(POSEND);
    }

    if(start.isNull() || end.isNull()){
        return NULL;
    }

    if(!start.startsWith(POSSEPARATOR) || !end.startsWith(POSSEPARATOR)) {
        qDebug() << "Wrong position attribute in XML";
        return NULL;
    }

    between_separators_start = getDataFromPosTag(start);
    between_separators_end = getDataFromPosTag(end);

    reverseList2<QString>(&between_separators_start);
    reverseList2<QString>(&between_separators_end);

    if(between_separators_start.length() != between_separators_end.length()) {
        qDebug() << "Wrong position attribute in XML";
        return NULL;
    }

    if(between_separators_start.length() == 2) {
        startchar = between_separators_start.at(0).toULong() - 1;
        startpar = between_separators_start.at(1).toULong() - 1;
        endchar = between_separators_end.at(0).toULong() - 1;
        endpar = between_separators_end.at(1).toULong() - 1;
        pos = new MctPosition(startpar, startchar, endpar, endchar);
        return pos;
    } else if(between_separators_start.length() == 4) {
        cellinf = new MctCell(between_separators_start.at(1), between_separators_start.at(0));
        cellinfend = new MctCell(between_separators_end.at(1), between_separators_end.at(0));
        pos = new MctPosition(0,0,0,0, cellinf, cellinfend);

        startchar = between_separators_start.at(2).toULong() - 1;
        startpar = between_separators_start.at(3).toULong() - 1;
        endchar = between_separators_end.at(2).toULong() - 1;
        endpar = between_separators_end.at(3).toULong() - 1;
        MctPosition *pos_tmp = new MctPosition(startpar, startchar, endpar, endchar);
        pos->setAnchored(pos_tmp);
        return pos;
    }

    if(between_separators_start.length() % 4 == 0) {
        cellinf = new MctCell(between_separators_start.at(1).toInt() - 1, between_separators_start.at(0).toInt() - 1);
        cellinfend = new MctCell(between_separators_end.at(1).toInt() - 1, between_separators_end.at(0).toInt() - 1);
        pos = new MctPosition(0,0,0,0, cellinf, cellinfend);
        pos_parent = pos;
        between_separators_start.removeFirst();
        between_separators_start.removeFirst();

        between_separators_end.removeFirst();
        between_separators_end.removeFirst();
    } else if (between_separators_start.length() % 4 == 2) {
        pos = NULL;
    } else {
        return NULL;
    }

    MctPosition *pos_tmp = NULL;
    if (between_separators_start.length() % 4 == 2 && between_separators_start.length() > 2) {
        between_separators_start = getDataFromPosTag(start);
        between_separators_end = getDataFromPosTag(end);
        MctPosition *pos_parent = NULL;

        while (between_separators_start.length() > 2) {
            startchar = between_separators_start.at(1).toULong() - 1;
            startpar = between_separators_start.at(0).toULong() - 1;
            endchar = between_separators_end.at(1).toULong() - 1;
            endpar = between_separators_end.at(0).toULong() - 1;

            cellinf = new MctCell(between_separators_start.at(2).toInt() - 1, between_separators_start.at(3).toInt() - 1);
            cellinfend = new MctCell(between_separators_end.at(2).toInt() - 1, between_separators_end.at(3).toInt() - 1);

            pos_tmp = new MctPosition(startpar, startchar, endpar, endchar, cellinf, cellinfend);

            if(pos_parent == NULL) {
                pos = pos_tmp;
                pos_parent = pos_tmp;
            } else {
                pos_parent->setAnchored(pos_tmp);
                pos_parent = pos_tmp;
            }

            for(int i=0; i<4; i++) {
                between_separators_start.removeFirst();
                between_separators_end.removeFirst();
            }
        }
        startchar = between_separators_start.at(1).toULong() - 1;
        startpar = between_separators_start.at(0).toULong() - 1;
        endchar = between_separators_end.at(1).toULong() - 1;
        endpar = between_separators_end.at(0).toULong() - 1;

        pos_tmp = new MctPosition(startpar, startchar, endpar, endchar);
        pos_parent->setAnchored(pos_tmp);

    } else {
        startchar = between_separators_start.at(0).toULong() - 1;
        startpar = between_separators_start.at(1).toULong() - 1;
        endchar = between_separators_end.at(0).toULong() - 1;
        endpar = between_separators_end.at(1).toULong() - 1;


        pos = new MctPosition(startpar, startchar, endpar, endchar);
    }

    return pos;

}

/**
 * @brief This function make position class from string extracted from the XML node. --- Obsolete
 * @param change The XML node.
 * @param moved True if moved position is wanted, False otherwise
 * @return Returns with the created position class.
 */
MctPosition* MctStaticData::getPosFromElementreeNodeOld(const QDomElement &change, bool moved)
{
    //TODO
    qCritical() << "Obsolete getPosFromElementreeNode called!";
    return NULL;
}

/**
 * @brief This recursively adds position attribute to the node of the graph.
 * The index start from 0 in Calligra, and start from 1 in the xml.
 * @param change The change node in the XML
 * @param pos The position
 * @param moved If Ture, moved position is affected.
 * @param clearfirst if True, position attributes in the XML node are cleared
 */
void MctStaticData::addPos2change(QDomElement *change, MctPosition *pos, bool moved, bool clearfirst)
{
    QString startTag, endTag;

    if (moved) {
        startTag = MPOSSTART;
        endTag = MPOSEND;
    } else {
        startTag = POSSTART;
        endTag = POSEND;
    }

    if (clearfirst) {
        change->setAttribute(MctStaticData::attributeNS(startTag, MctStaticData::NS_C), ""); // FIXME: attribute name should be in MctStaticData
        change->setAttribute(MctStaticData::attributeNS(endTag, MctStaticData::NS_C), "");
    }

    QString posStringStart = change->attribute(startTag, "");
    /*
    if (pos->getStartPar() != UNDEFINED)
        posStringStart = POSSEPARATOR +QString::number(pos->getStartPar() + 1) + POSSEPARATOR + QString::number(pos->getStartChar() + 1) + posStringStart;
    if (pos->getCellInfo() != NULL) {
        pos->getCellInfo()->convertCellName2CellPos();
        posStringStart = POSSEPARATOR + QString::number(pos->getCellInfo()->getRow() + 1) + POSSEPARATOR + QString::number(pos->getCellInfo()->getCol() + 1) + posStringStart;
    }
    */
    if (pos->startPar() != UNDEFINED)
        posStringStart += POSSEPARATOR +QString::number(pos->startPar() + 1) + POSSEPARATOR + QString::number(pos->startChar() + 1);
    if (pos->startCellInfo() != NULL) {
        pos->startCellInfo()->convertCellName2CellPos();
        posStringStart += POSSEPARATOR + QString::number(pos->startCellInfo()->row() + 1) + POSSEPARATOR + QString::number(pos->startCellInfo()->col() + 1);
    }
    change->setAttribute(MctStaticData::attributeNS(startTag, MctStaticData::NS_C), posStringStart);

    QString posStringEnd = change->attribute(endTag, "");
    if (pos->endPar() != UNDEFINED)
        posStringEnd += POSSEPARATOR + QString::number(pos->endPar() + 1) + POSSEPARATOR + QString::number(pos->endChar() + 1);
    if (pos->startCellInfo() != NULL) {
        pos->endCellInfoEnd()->convertCellName2CellPos();
        posStringEnd += POSSEPARATOR + QString::number(pos->endCellInfoEnd()->row() + 1) + POSSEPARATOR + QString::number(pos->endCellInfoEnd()->col() + 1);
    }
    change->setAttribute(MctStaticData::attributeNS(endTag, MctStaticData::NS_C), posStringEnd);

    if (pos->anchoredPos() != NULL)
        addPos2change(change, pos->anchoredPos(), moved, false);
}

/**
 * @brief MctStaticData::isTableChange Decide, whether the chage node represents table change or not
 * @param changeNode
 * @return Returns with True if the changenode is related to table change, False otherwise
 */
bool MctStaticData::isTableChange(const MctChange *changeNode)
{
    MctChangeTypes changeType = changeNode->changeType();

    switch (changeType) {
    case MctChangeTypes::AddedStringInTable:
    case MctChangeTypes::RemovedStringInTable:
    case MctChangeTypes::MovedStringInTable:
    case MctChangeTypes::StyleChangeInTable:
    case MctChangeTypes::AddedTextGraphicObjectInTable:
    case MctChangeTypes::RemovedTextGraphicObjectInTable:
    case MctChangeTypes::ParagraphBreakInTable:
    case MctChangeTypes::DelParagraphBreakInTable:
    case MctChangeTypes::AddedTextTableInTable:
    case MctChangeTypes::RemovedTextTableInTable:
    case MctChangeTypes::AddedRowInTable:
    case MctChangeTypes::RemovedRowInTable:
    case MctChangeTypes::RemovedColInTable:
        return true;
    default:
        return false;
    }
}

QTextCursor * MctStaticData::CreateCursorFromRecursivePos(KoTextDocument *doc, MctPosition *position)
{
    if (!doc) qCritical("nullptr");
    QTextCursor tmpcursor = *(doc->textEditor()->cursor());
    tmpcursor.movePosition(QTextCursor::Start);
    int blocknumber = position->startPar();
    /*MctPosition *tmpPos = position->getAnchoredPos();
    while(tmpPos) {
        blocknumber = tmpPos->getStartPar();
        tmpPos = tmpPos->getAnchoredPos();
    }*/
    QTextBlock startBlock = tmpcursor.document()->findBlockByNumber(blocknumber);
    QTextCursor *cursor = new QTextCursor(startBlock);
    return cursor;
}

//---------------------------------------------------------------------

/// functions3.py
//---------------------------------------------------------------------

/**
 * @brief MctStaticData::findNewLines Gather indexes of newline characters in the parameter string
 * @param string
 * @return list of indexes
 */
QList<int> * MctStaticData::findNewLines(QString string)
{
    QList<int> *newlineIndexList = new QList<int>();
    QChar NL = '\n'; // TODO talan lehetne static const is?
    int start = 0;
    int index;

    while (true) {
        index = string.indexOf(NL, start);
        if (index == -1) {
            break;
        } else {
            newlineIndexList->append(index);
            start = index + 1;
        }
    }

    return newlineIndexList;
}

/**
 * @brief This creates changeNode for redo operation
 * @param changeNode The undo changeNode.
 * @param doc
 * @return
 */
MctChange* MctStaticData::createRedoChangeNode(KoTextDocument *doc, MctChange *changeNode)
{
    MctPosition * pos = changeNode->position();
    MctChangeTypes changeType = changeNode->changeType();
    MctNode *changeEntity = changeNode->changeEntity();

    MctNode *redochangeEntity = NULL;
    MctChange *redoChangeNode = NULL;

    QTextCursor *cursor;
    if(pos != NULL) {
        cursor = CreateCursorFromRecursivePos(doc, pos);
    }

    qDebug() << "creating redoChangeNode of type: " << changeType;

    if(changeType == MctChangeTypes::AddedString) {
        //TODO: megnézni hogy a cursortól lekért string megfelelő-e
        //cursor->selectedText();
        qDebug() << "TODO";
        QString text = cursor->block().text();
        redochangeEntity = new MctStringChange(text);
        redoChangeNode = new MctChange(pos, changeType, redochangeEntity);
    } else if (changeType == MctChangeTypes::AddedStringInTable) {
        QString text = cursor->block().text();  //todo
        MctTable *tableEntity = dynamic_cast<MctTable*>(changeEntity);
        redochangeEntity = new MctStringChangeInTable(text, tableEntity->cellName(), tableEntity->tableName());
        redoChangeNode = new MctChange(pos, changeType, redochangeEntity);
    } else if (changeType == MctChangeTypes::RemovedString) {
        //TODO: megnézni hogy ez a pozíció jó-e
        qDebug() << "TODO";
        int p = cursor->block().position();
        cursor->setPosition(p);
        redochangeEntity = new MctStringChange("");
        redoChangeNode = new MctChange(pos, changeType, redochangeEntity);
    } else if (changeType == MctChangeTypes::RemovedStringInTable) {
        int p = cursor->block().position(); //todo
        cursor->setPosition(p);
        MctTable *tableEntity = dynamic_cast<MctTable*>(changeEntity);
        redochangeEntity = new MctStringChangeInTable("", tableEntity->cellName(), tableEntity->tableName());
        redoChangeNode = new MctChange(pos, changeType, redochangeEntity);
    } else if (changeType == MctChangeTypes::MovedString) {
        redochangeEntity = new MctStringChange("");
        redoChangeNode = new MctChange(pos, changeType, redochangeEntity, changeNode->movedPosition());
    } else if (changeType == MctChangeTypes::MovedStringInTable) {
        MctTable *tableEntity = dynamic_cast<MctTable*>(changeEntity);
        redochangeEntity = new MctStringChangeInTable("", tableEntity->cellName(), tableEntity->tableName());
        redoChangeNode = new MctChange(pos, changeType, redochangeEntity, changeNode->movedPosition());
    } else if (changeType == MctChangeTypes::ParagraphBreak) {
        redochangeEntity = new MctParagraphBreak();
        redoChangeNode = new MctChange(pos, changeType, redochangeEntity);
    } else if (changeType == MctChangeTypes::DelParagraphBreak) {
        redochangeEntity = new MctDelParagraphBreak();
        redoChangeNode = new MctChange(pos, changeType, redochangeEntity);
    } else if (changeType == MctChangeTypes::StyleChange) {
        redochangeEntity = changeEntity;
        redoChangeNode = new MctChange(pos, changeType, redochangeEntity);
    } else if (changeType == MctChangeTypes::StyleChangeInTable) {
        redochangeEntity = changeEntity;
        redoChangeNode = new MctChange(pos, changeType, redochangeEntity);
    } else if (changeType == MctChangeTypes::AddedTextFrame) {
    } else if (changeType == MctChangeTypes::RemovedTextFrame) {
    } else if (changeType == MctChangeTypes::AddedTextGraphicObject) {
        redochangeEntity = changeEntity;
        redoChangeNode = new MctChange(pos, changeType, redochangeEntity);
    } else if (changeType == MctChangeTypes::AddedTextGraphicObjectInTable) {
        redochangeEntity = changeEntity;
        redoChangeNode = new MctChange(pos, changeType, redochangeEntity);
    } else if (changeType == MctChangeTypes::RemovedTextGraphicObject) {
        redochangeEntity = changeEntity;
        redoChangeNode = new MctChange(pos, changeType, redochangeEntity);
    } else if (changeType == MctChangeTypes::RemovedTextGraphicObjectInTable) {
        redochangeEntity = changeEntity;
        redoChangeNode = new MctChange(pos, changeType, redochangeEntity);
    } else if (changeType == MctChangeTypes::AddedEmbeddedObject) {
    } else if (changeType == MctChangeTypes::RemovedEmbeddedObject) {
    }

    return redoChangeNode;
}

void MctStaticData::insertTextGraphicObjects(MctChange *changeNode, FIXME_Doc *doc, bool withprops, FIXME_TextInterface *xtext_interface)
{
    // TODO
}

void MctStaticData::instertTextTable(MctChange *changeNode, bool withprops, FIXME_TextInterface *xtext_interface)
{
    // TODO
}

/**
 * @brief MctStaticData::getNoneDefaultProps This gets nondefault properties of textgraphic object.
 * @param props
 * @return
 */
FIXME_PropertyDictionary *MctStaticData::getNoneDefaultProps(MctEmbObjProperties *props)
{
    // TODO
    return NULL;
}

//---------------------------------------------------------------------

/**
 * @brief MctStaticData::recursiveDirectoryCopy Copies the archive directory recursievly into another Zip file.
 * @param dir KArchiveDirectory that will be copied
 * @param path Path of the input dir. Should not end neither with the name of dir nor path separator (eg. "/").
 * @param destinationZip KZip object that must be writeable!
 */
void MctStaticData::recursiveDirectoryCopy(const KArchiveDirectory *dir, const QString &path, KZip *destinationZip)
{
    foreach (const QString &it, dir->entries()) {
        const KArchiveEntry *entry = dir->entry(it);
        qDebug() << "actual entry: " << entry->name();
        if (entry->isFile()) {
            Q_ASSERT(entry && entry->isFile());
            const KArchiveFile *fileEntry = static_cast<const KArchiveFile *>(entry);

            if (entry->symLinkTarget().isEmpty()) {
                destinationZip->writeFile(path + entry->name(), QString(""), QString(""), fileEntry->data().constData(), fileEntry->data().size());
            } else {
                destinationZip->writeSymLink(path + entry->name(), entry->symLinkTarget(),QString(""), QString(""));
            }
        } else if (entry->isDirectory()) {
            recursiveDirectoryCopy(static_cast<const KArchiveDirectory *>(entry), path + entry->name() + QDir::separator(), destinationZip);
        }
    }
}

void MctStaticData::textPropMapTester(int key)
{
    if (!TEXT_PROPS_AS_STRING.contains(key) && !TEXT_KO_CHAR_PROPS_AS_STRING.contains(key)
        && !TEXT_KO_PAR_PROPS_AS_STRING.contains(key) && !TEXT_KO_LIST_PROPS_AS_STRING.contains(key)) {
        qWarning() << "The following key is not mapped yet: " << key << "[0x" << QString::number(key, 16) << "]";
    }
}

/**
 * @brief MctStaticData::hasMctSupport check wether the given odt file has undo/redo.xml in the package
 * @param odt The odt file to check
 * @return Returns true if the odt has mct support, false otherwise
 */
bool MctStaticData::hasMctSupport(QString odt)
{
    KZip *zip = new KZip(odt);
    zip->open(QIODevice::ReadOnly);
    QStringList entries = zip->directory()->entries();
    bool found = false;
    foreach (QString entry, entries) {
        qDebug() << entry;
        if((entry == UNDOCHANGES + ".xml") || (entry == REDOCHANGES + ".xml")) {
            found = true;
            return found;
        }
    }
    zip->close();
    delete zip;
    return found;
}

void MctStaticData::setFileURL(const QString &url)
{
    fileURL = url;
}

QString MctStaticData::getFileURL()
{
    return fileURL;
}

void MctStaticData::setChanges(QList<MctChange *> *change)
{
    changes = change;
}

QList<MctChange *> *MctStaticData::getChanges()
{
    return changes;
}

void MctStaticData::clearChanges()
{
    changes->clear();
}

void MctStaticData::setKoDocument(KoDocument *doc)
{
    kodoc = doc;
}

KoDocument* MctStaticData::getKoDocument()
{
    return kodoc;
}

/**
 * @brief MctStaticData::getFrameIndexes creates a map to the frames of the document
 * which contains from which block how much position correction is needed
 * @param koTextDoc
 * @return
 */
QMap<ulong, ulong> * MctStaticData::getFrameIndexes(const KoTextDocument *koTextDoc)
{
    QMap<ulong, ulong> *indexes = new QMap<ulong,ulong>();

    QTextFrame * root = koTextDoc->document()->rootFrame();
    QTextFrame::iterator it = root->begin();
    int delta = 0;
    do {
        QTextBlock block = it.currentBlock();
        QTextTable *table = qobject_cast<QTextTable*>(it.currentFrame());
        QTextFrame *subFrame = it.currentFrame();
        if (block.blockFormat().hasProperty(KoParagraphStyle::HiddenByTable)) {            
            ++delta;
        }
        if (table) {            
            delta += table->lastCursorPosition().blockNumber() - table->firstCursorPosition().blockNumber();
            indexes->insert(table->firstCursorPosition().blockNumber(), delta);
        } else if(subFrame) {            
            delta += subFrame->lastCursorPosition().blockNumber() - subFrame->firstCursorPosition().blockNumber();
            indexes->insert(subFrame->firstCursorPosition().blockNumber(), delta);
        }

        if (!it.atEnd()) {
            ++it;
        }
    } while (!it.atEnd());

    return indexes;
}

bool MctStaticData::getMctState() const
{
    return mctState;
}

void MctStaticData::setMctState(bool state)
{
    mctState = state;
}

QString MctStaticData::getAddedShapeType()
{
    return addedShapeType;
}

void MctStaticData::setAddedShapeType(const QString &shapeType)
{
    this->addedShapeType = shapeType;
}

QString MctStaticData::attributeNS(const QString &attribute, const XMLns &ns)
{
    return ns + ":" + attribute;
}
