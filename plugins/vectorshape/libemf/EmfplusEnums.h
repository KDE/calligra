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

#ifndef EMFPLUSENUMS_H
#define EMFPLUSENUMS_H

#include <QDataStream>
#include <QRect> // also provides QSize
#include <QString>

/**
   \file

   Enumerations used in various parts of EMFPLUS files.  Many of these
   enumerations are the same as those defined in EmfEnums.h
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
   

// ----------------------------------------------------------------
// 2.1.1 Enumeration constant types


/**
   BitmapDataType Enumeration

   The BitmapDataType enumeration defines types of bitmap data formats.

   See [MS-EMFPLUS] 2.1.1.2
*/

enum BitmapDataType {
    BitmapDataTypePixel      = 0x00000000,
    BitmapDataTypeCompressed = 0x00000001
};


/**
   BrushType Enumeration

   The BrushType enumeration defines types of graphics brushes, which
   are used to fill graphics regions.

   See [MS-EMFPLUS] 2.1.1.3
*/


enum BrushType {
    BrushTypeSolidColor     = 0x00000000,
    BrushTypeHatchFill      = 0x00000001,
    BrushTypeTextureFill    = 0x00000002,
    BrushTypePathGradient   = 0x00000003,
    BrushTypeLinearGradient = 0x00000004
};


/**
   CombineMode Enumeration

   The CombineMode enumeration defines modes for combining two graphics regions.

   See [MS-EMFPLUS] 2.1.1.4
*/

enum CombineMode {
    CombineModeReplace    = 0x00000000,
    CombineModeIntersect  = 0x00000001,
    CombineModeUnion      = 0x00000002,
    CombineModeXOR        = 0x00000003,
    CombineModeExclude    = 0x00000004,
    CombineModeComplement = 0x00000005
};


/**
   CompositingMode Enumeration

   The CompositingMode enumeration defines modes for combining source
   colors with background colors. The compositing mode represents the
   enable state of alpha blending.

   See [MS-EMFPLUS] 2.1.1.5
*/


enum CompositingMode {
    CompositingModeSourceOver = 0x00,
    CompositingModeSourceCopy = 0x01
};


/**
   CompositingQuality Enumeration

   The CompositingQuality enumeration defines levels of quality for creating composite images.

   See [MS-EMFPLUS] 2.1.1.6
*/


enum CompositingQuality {
    CompositingQualityDefault        = 0x01,
    CompositingQualityHighSpeed      = 0x02,
    CompositingQualityHighQuality    = 0x03,
    CompositingQualityGammaCorrected = 0x04,
    CompositingQualityAssumeLinear   = 0x05
};


/**
   CurveAdjustments Enumeration

   The CurveAdjustments enumeration defines adjustments that can be
   applied to the color curve of an image.

   See [MS-EMFPLUS] 2.1.1.7
*/

enum CurveAdjustments {
    AdjustExposure        = 0x00000000,
    AdjustDensity         = 0x00000001,
    AdjustContrast        = 0x00000002,
    AdjustHighlight       = 0x00000003,
    AdjustShadow          = 0x00000004,
    AdjustMidtone         = 0x00000005,
    AdjustWhiteSaturation = 0x00000006,
    AdjustBlackSaturation = 0x00000007
};


/**
   CurveChannel Enumeration

   The CurveChannel enumeration defines color channels that can be
   affected by a color curve effect adjustment to an image.

   See [MS-EMFPLUS] 2.1.1.8
 */

enum CurveChannel {
    CurveChannelAll   = 0x00000000,
    CurveChannelRed   = 0x00000001,
    CurveChannelGreen = 0x00000002,
    CurveChannelBlue  = 0x00000003
};


/**
   CustomLineCapDataType Enumeration

   The CustomLineCapDataType enumeration defines types of custom line
   cap data, which specify styles and shapes for the ends of graphics
   lines.

   See [MS-EMFPLUS] 2.1.1.9
*/

enum CustomLineCapDataType {
    CustomLineCapDataTypeDefault         = 0x00000000,
    CustomLineCapDataTypeAdjustableArrow = 0x00000001
};


/**
   DashedLineCapType Enumeration

   The DashedLineCapType enumeration defines types of line caps to use
   at the ends of dashed lines that are drawn with graphics pens.

   See [MS-EMFPLUS] 2.1.1.10
*/

enum DashedLineCapType {
    DashedLineCapTypeFlat     = 0x00000000,
    DashedLineCapTypeRound    = 0x00000002,
    DashedLineCapTypeTriangle = 0x00000003
};


/**
   FilterType Enumeration

   The FilterType enumeration defines types of filtering algorithms
   that can be used for text and graphics quality enhancement and
   image rendering.

   See [MS-EMFPLUS] 2.1.1.11
*/

enum FilterType {
    FilterTypeNone          = 0x00,
    FilterTypePoint         = 0x01,
    FilterTypeLinear        = 0x02,
    FilterTypeTriangle      = 0x03,
    FilterTypeBox           = 0x04,
    FilterTypePyramidalQuad = 0x06,
    FilterTypeGaussianQuad  = 0x07
};


/**
   GraphicsVersion Enumeration

   The GraphicsVersion enumeration defines versions of operating
   system graphics that are used to create EMF+ metafiles.

   See [MS-EMFPLUS] 2.1.1.12
*/

enum GraphicsVersion {
    GraphicsVersion1 = 0x0001,
    GraphicsVersion1_1 = 0x0002
};


/**
   HatchStyle Enumeration

   The HatchStyle enumeration defines hatch patterns used by graphics
   brushes. A hatch pattern consists of a solid background color and
   lines drawn over the background.

   See [MS-EMFPLUS] 2.1.1.13
*/

enum  HatchStyle {
    HatchStyleHorizontal             = 0x00000000,
    HatchStyleVertical               = 0x00000001,
    HatchStyleForwardDiagonal        = 0x00000002,
    HatchStyleBackwardDiagonal       = 0x00000003,
    HatchStyleLargeGrid              = 0x00000004,
    HatchStyleDiagonalCross          = 0x00000005,
    HatchStyle05Percent              = 0x00000006,
    HatchStyle10Percent              = 0x00000007,
    HatchStyle20Percent              = 0x00000008,
    HatchStyle25Percent              = 0x00000009,
    HatchStyle30Percent              = 0x0000000A,
    HatchStyle40Percent              = 0x0000000B,
    HatchStyle50Percent              = 0x0000000C,
    HatchStyle60Percent              = 0x0000000D,
    HatchStyle70Percent              = 0x0000000E,
    HatchStyle75Percent              = 0x0000000F,
    HatchStyle80Percent              = 0x00000010,
    HatchStyle90Percent              = 0x00000011,
    HatchStyleLightDownwardDiagonal  = 0x00000012,
    HatchStyleLightUpwardDiagonal    = 0x00000013,
    HatchStyleDarkDownwardDiagonal   = 0x00000014,
    HatchStyleDarkUpwardDiagonal     = 0x00000015,
    HatchStyleWideDownwardDiagonal   = 0x00000016,
    HatchStyleWideUpwardDiagonal     = 0x00000017,
    HatchStyleLightVertical          = 0x00000018,
    HatchStyleLightHorizontal        = 0x00000019,
    HatchStyleNarrowVertical         = 0x0000001A,
    HatchStyleNarrowHorizontal       = 0x0000001B,
    HatchStyleDarkVertical           = 0x0000001C,
    HatchStyleDarkHorizontal         = 0x0000001D,
    HatchStyleDashedDownwardDiagonal = 0x0000001E,
    HatchStyleDashedUpwardDiagonal   = 0x0000001F,
    HatchStyleDashedHorizontal       = 0x00000020,
    HatchStyleDashedVertical         = 0x00000021,
    HatchStyleSmallConfetti          = 0x00000022,
    HatchStyleLargeConfetti          = 0x00000023,
    HatchStyleZigZag                 = 0x00000024,
    HatchStyleWave                   = 0x00000025,
    HatchStyleDiagonalBrick          = 0x00000026,
    HatchStyleHorizontalBrick        = 0x00000027,
    HatchStyleWeave                  = 0x00000028,
    HatchStylePlaid                  = 0x00000029,
    HatchStyleDivot                  = 0x0000002A,
    HatchStyleDottedGrid             = 0x0000002B,
    HatchStyleDottedDiamond          = 0x0000002C,
    HatchStyleShingle                = 0x0000002D,
    HatchStyleTrellis                = 0x0000002E,
    HatchStyleSphere                 = 0x0000002F,
    HatchStyleSmallGrid              = 0x00000030,
    HatchStyleSmallCheckerBoard      = 0x00000031,
    HatchStyleLargeCheckerBoard      = 0x00000032,
    HatchStyleOutlinedDiamond        = 0x00000033,
    HatchStyleSolidDiamond           = 0x00000034
};


/**
   HotkeyPrefix Enumeration

   The HotkeyPrefix enumeration defines output options for hotkey
   prefixes in graphics text.

   See [MS-EMFPLUS] 2.1.1.14
*/

enum HotkeyPrefix {
    HotkeyPrefixNone = 0x00000000,
    HotkeyPrefixShow = 0x00000001,
    HotkeyPrefixHide = 0x00000002
};


/**
   ImageDataType Enumeration

   The ImageDataType enumeration defines types of image data formats.

   See [MS-EMFPLUS] 2.1.1.15
*/

enum ImageDataType {
    ImageDataTypeUnknown  = 0x00000000,
    ImageDataTypeBitmap   = 0x00000001,
    ImageDataTypeMetafile = 0x00000002
};


/**
   InterpolationMode Enumeration

   The InterpolationMode enumeration defines ways to perform scaling,
   including stretching and shrinking.

   See [MS-EMFPLUS] 2.1.1.16
*/

enum InterpolationMode {
    InterpolationModeDefault             = 0x00,
    InterpolationModeLowQuality          = 0x01,
    InterpolationModeHighQuality         = 0x02,
    InterpolationModeBilinear            = 0x03,
    InterpolationModeBicubic             = 0x04,
    InterpolationModeNearestNeighbor     = 0x05,
    InterpolationModeHighQualityBilinear = 0x06,
    InterpolationModeHighQualityBicubic  = 0x07
};


/**
   LanguageIdentifier Enumeration

   The LanguageIdentifier enumeration defines identifiers for natural
   languages in locales, including countries, geographical regions,
   and administrative districts.

   See [MS-EMFPLUS] 2.1.1.17
*/

enum LanguageIdentifiers {
    LANG_NEUTRAL = 0x0000,
    zh_CHS = 0x0004,
    LANG_INVARIANT = 0x007F,
    LANG_NEUTRAL_USER_DEFAULT = 0x0400,
    ar_SA = 0x0401,
    bg_BG = 0x0402,
    ca_ES = 0x0403,
    zh_CHT = 0x0404,
    cs_CZ = 0x0405,
    da_DK = 0x0406,
    de_DE = 0x0407,
    el_GR = 0x0408,
    en_US = 0x0409,
    es_Tradnl_ES = 0x040A,
    fi_FI = 0x040B,
    fr_FR = 0x040C,
    he_IL = 0x040D,
    hu_HU = 0x040E,
    is_IS = 0x040F,
    it_IT = 0x0410,
    ja_JA = 0x0411,
    ko_KR = 0x0412,
    nl_NL = 0x0413,
    nb_NO = 0x0414,
    pl_PL = 0x0415,
    pt_BR = 0x0416,
    rm_CH = 0x0417,
    ro_RO = 0x0418,
    ru_RU = 0x0419,
    hr_HR = 0x041A,
    sk_SK = 0x041B,
    sq_AL = 0x041C,
    sv_SE = 0x041D,
    th_TH = 0x041E,
    tr_TR = 0x041F,
    ur_PK = 0x0420,
    id_ID = 0x0421,
    uk_UA = 0x0422,
    be_BY = 0x0423,
    sl_SI = 0x0424,
    et_EE = 0x0425,
    lv_LV = 0x0426,
    lt_LT = 0x0427,
    tg_TJ = 0x0428,
    fa_IR = 0x0429,
    vi_VN = 0x042A,
    hy_AM = 0x042B,
    az_Latn_AZ = 0x042C,
    eu_ES = 0x042D,
    wen_DE = 0x042E,
    mk_MK = 0x042F,
    st_ZA = 0x0430,
    tn_ZA = 0x0432,
    xh_ZA = 0x0434,
    zu_ZA = 0x0435,
    af_ZA = 0x0436,
    ka_GE = 0x0437,
    fa_FA = 0x0438,
    hi_IN = 0x0439,
    mt_MT = 0x043A,
    se_NO = 0x043B,
    ga_GB = 0x043C,
    ms_MY = 0x043E,
    kk_KZ = 0x043F,
    ky_KG = 0x0440,
    sw_KE = 0x0441,
    tk_TM = 0x0442,
    uz_Latn_UZ = 0x0443,
    tt_Ru = 0x0444,
    bn_IN = 0x0445,
    pa_IN = 0x0446,
    gu_IN = 0x0447,
    or_IN = 0x0448,
    ta_IN = 0x0449,
    te_IN = 0x044A,
    kn_IN = 0x044B,
    ml_IN = 0x044C,
    as_IN = 0x044D,
    mr_IN = 0x044E,
    sa_IN = 0x044F,
    mn_MN = 0x0450,
    bo_CN = 0x0451,
    cy_GB = 0x0452,
    km_KH = 0x0453,
    lo_LA = 0x0454,
    gl_ES = 0x0456,
    kok_IN = 0x0457,
    sd_IN = 0x0459,
    syr_SY = 0x045A,
    si_LK = 0x045B,
    iu_Cans_CA = 0x045D,
    am_ET = 0x045E,
    ne_NP = 0x0461,
    fy_NL = 0x0462,
    ps_AF = 0x0463,
    fil_PH = 0x0464,
    div_MV = 0x0465,
    ha_Latn_NG = 0x0468,
    yo_NG = 0x046A,
    quz_BO = 0x046B,
    nzo_ZA = 0x046C,
    ba_RU = 0x046D,
    lb_LU = 0x046E,
    kl_GL = 0x046F,
    ig_NG = 0x0470,
    so_SO = 0x0477,
    ii_CN = 0x0478,
    arn_CL = 0x047A,
    moh_CA = 0x047C,
    br_FR = 0x047E,
    ug_CN = 0x0480,
    mi_NZ = 0x0481,
    oc_FR = 0x0482,
    co_FR = 0x0483,
    gsw_FR = 0x0484,
    sah_RU = 0x0485,
    qut_GT = 0x0486,
    rw_RW = 0x0487,
    wo_SN = 0x0488,
    gbz_AF = 0x048C,
    LANG_NEUTRAL_SYS_DEFAULT = 0x0800,
    ar_IQ = 0x0801,
    zh_CN = 0x0804,
    de_CH = 0x0807,
    en_GB = 0x0809,
    es_MX = 0x080A,
    fr_BE = 0x080C,
    it_CH = 0x0810,
    ko_Johab_KR = 0x0812,
    nl_BE = 0x0813,
    nn_NO = 0x0814,
    pt_PT = 0x0816,
    sr_Latn_SP = 0x081A,
    sv_FI = 0x081D,
    ur_IN = 0x0820,
    lt_C_LT = 0x0827,
    az_Cyrl_AZ = 0x082C,
    wee_DE = 0x082E,
    se_SE = 0x083B,
    ga_IE = 0x083C,
    ms_BN = 0x083E,
    uz_Cyrl_UZ = 0x0843,
    bn_BD = 0x0845,
    mn_Mong_CN = 0x0850,
    sd_PK = 0x0859,
    iu_Latn_CA = 0x085D,
    tzm_Latn_DZ = 0x085F,
    quz_EC = 0x086B,
    LANG_NEUTRAL_CUSTOM_DEFAULT = 0x0C00,
    ar_EG = 0x0C01,
    zh_HK = 0x0C04,
    de_AT = 0x0C07,
    en_AU = 0x0C09,
    es_ES = 0x0C0A,
    fr_CA = 0x0C0C,
    sr_Cyrl_CS = 0x0C1A,
    se_FI = 0x0C3B,
    quz_PE = 0x0C6B,
    LANG_NEUTRAL_CUSTOM = 0x1000,
    ar_LY = 0x1001,
    zh_SG = 0x1004,
    de_LU = 0x1007,
    en_CA = 0x1009,
    es_GT = 0x100A,
    fr_CH = 0x100C,
    hr_BA = 0x101A,
    smj_NO = 0x103B,
    LANG_NEUTRAL_CUSTOM_DEFAULT_MUI = 0x1400,
    ar_DZ = 0x1401,
    zh_MO = 0x1404,
    de_LI = 0x1407,
    en_NZ = 0x1409,
    es_CR = 0x140A,
    fr_LU = 0x140C,
    bs_Latn_BA = 0x141A,
    smj_SE = 0x143B,
    ar_MA = 0x1801,
    en_IE = 0x1809,
    es_PA = 0x180A,
    ar_MC = 0x180C,
    sr_Latn_BA = 0x181A,
    sma_NO = 0x183B,
    ar_TN = 0x1C01,
    en_ZA = 0x1C09,
    es_DO = 0x1C0A,
    sr_Cyrl_BA = 0x1C1A,
    sma_SE = 0x1C3B,
    ar_OM = 0x2001,
    el_2_GR = 0x2008,
    en_JM = 0x2009,
    es_VE = 0x200A,
    bs_Cyrl_BA = 0x201A,
    sms_FI = 0x203B,
    ar_YE = 0x2401,
    ar_029 = 0x2409,
    es_CO = 0x240A,
    smn_FI = 0x243B,
    ar_SY = 0x2801,
    en_BZ = 0x2809,
    es_PE = 0x280A,
    ar_JO = 0x2C01,
    en_TT = 0x2C09,
    es_AR = 0x2C0A,
    ar_LB = 0x3001,
    en_ZW = 0x3009,
    es_EC = 0x300A,
    ar_KW = 0x3401,
    en_PH = 0x3409,
    es_CL = 0x340A,
    ar_AE = 0x3801,
    es_UY = 0x380A,
    ar_BH = 0x3C01,
    es_PY = 0x3C0A,
    ar_QA = 0x4001,
    en_IN = 0x4009,
    es_BO = 0x400A,
    en_MY = 0x4409,
    es_SV = 0x440A,
    en_SG = 0x4809,
    es_HN = 0x480A,
    es_NI = 0x4C0A,
    es_PR = 0x500A,
    es_US = 0x540A,
    zh_Hant = 0x7C04
};


/**
   LineCapType Enumeration

   The LineCapType enumeration defines types of line caps to use at
   the ends of lines that are drawn with graphics pens.

   See [MS-EMFPLUS] 2.1.1.18
*/

enum LineCapType {
    LineCapTypeFlat          = 0x00000000,
    LineCapTypeSquare        = 0x00000001,
    LineCapTypeRound         = 0x00000002,
    LineCapTypeTriangle      = 0x00000003,
    LineCapTypeNoAnchor      = 0x00000010,
    LineCapTypeSquareAnchor  = 0x00000011,
    LineCapTypeRoundAnchor   = 0x00000012,
    LineCapTypeDiamondAnchor = 0x00000013,
    LineCapTypeArrowAnchor   = 0x00000014,
    LineCapTypeAnchorMask    = 0x000000F0,
    LineCapTypeCustom        = 0x000000FF
};


/**
   LineJoinType Enumeration

   The LineJoinType enumeration defines ways to join two lines that
   are drawn by the same graphics pen and whose ends meet.

   See [MS-EMFPLUS] 2.1.1.19
*/

enum LineJoinType {
    LineJoinTypeMiter        = 0x00000000,
    LineJoinTypeBevel        = 0x00000001,
    LineJoinTypeRound        = 0x00000002,
    LineJoinTypeMiterClipped = 0x00000003
};


/**
   LineStyle Enumeration

   The LineStyle enumeration defines styles of lines that are drawn
   with graphics pens.

   See [MS-EMFPLUS] 2.1.1.20
*/

enum LineStyle {
    LineStyleSolid      = 0x00000000,
    LineStyleDash       = 0x00000001,
    LineStyleDot        = 0x00000002,
    LineStyleDashDot    = 0x00000003,
    LineStyleDashDotDot = 0x00000004,
    LineStyleCustom     = 0x00000005
};


/**
   MetafileDataType Enumeration

   The MetafileDataType enumeration defines types of metafiles data
   that can be embedded in an EMF+ metafile.

   See [MS-EMFPLUS] 2.1.1.21
*/

enum MetafileDataType {
    MetafileDataTypeWmf          = 0x00000001,
    MetafileDataTypeWmfPlaceable = 0x00000002,
    MetafileDataTypeEmf          = 0x00000003,
    MetafileDataTypeEmfPlusOnly  = 0x00000004,
    MetafileDataTypeEmfPlusDual  = 0x00000005
};


/**
   ObjectType Enumeration

   The ObjectType enumeration defines types of graphics objects that
   can be created and used in graphics operations.

   See [MS-EMFPLUS] 2.1.1.22
*/

enum ObjectType {
    ObjectTypeInvalid         = 0x00000000,
    ObjectTypeBrush           = 0x00000001,
    ObjectTypePen             = 0x00000002,
    ObjectTypePath            = 0x00000003,
    ObjectTypeRegion          = 0x00000004,
    ObjectTypeImage           = 0x00000005,
    ObjectTypeFont            = 0x00000006,
    ObjectTypeStringFormat    = 0x00000007,
    ObjectTypeImageAttributes = 0x00000008,
    ObjectTypeCustomLineCap   = 0x00000009
};


/**
   PathPointType Enumeration

   The PathPointType enumeration defines types of points on a graphics
   path.

   See [MS-EMFPLUS] 2.1.1.23
*/

enum PathPointType {
    PathPointTypeStart  = 0x00,
    PathPointTypeLine   = 0x01,
    PathPointTypeBezier = 0x03
};


/**
   PenAlignment Enumeration

   The PenAlignment enumeration defines the distribution of the width
   of the pen with respect to the line being drawn.

   See [MS-EMFPLUS] 2.1.1.24
*/

enum PenAlignment {
    PenAlignmentCenter = 0x00000000,
    PenAlignmentInset  = 0x00000001,
    PenAlignmentLeft   = 0x00000002,
    PenAlignmentOutset = 0x00000003,
    PenAlignmentRight  = 0x00000004
};


/**
   PixelFormat Enumeration

   The PixelFormat enumeration defines pixel formats that are
   supported in EMF+ bitmaps.

   See [MS-EMFPLUS] 2.1.1.25
*/

enum PixelFormat {
    PixelFormatUndefined      = 0x00000000,
    PixelFormat1bppIndexed    = 0x00030101,
    PixelFormat4bppIndexed    = 0x00030402,
    PixelFormat8bppIndexed    = 0x00030803,
    PixelFormat16bppGrayScale = 0x00101004,
    PixelFormat16bppRGB555    = 0x00021005,
    PixelFormat16bppRGB565    = 0x00021006,
    PixelFormat16bppARGB1555  = 0x00061007,
    PixelFormat24bppRGB       = 0x00021808,
    PixelFormat32bppRGB       = 0x00022009,
    PixelFormat32bppARGB      = 0x0026200A,
    PixelFormat32bppPARGB     = 0x000E200B,
    PixelFormat48bppRGB       = 0x0010300C,
    PixelFormat64bppARGB      = 0x0034400D,
    PixelFormat64bppPARGB     = 0x001A400E
};


/**
   PixelOffsetMode Enumeration

   The PixelOffsetMode enumeration defines how pixels are offset,
   which specifies the trade-off between rendering speed and quality.

   See [MS-EMFPLUS] 2.1.1.26
*/

enum PixelOffsetMode {
    PixelOffsetModeDefault     = 0x00,
    PixelOffsetModeHighSpeed   = 0x01,
    PixelOffsetModeHighQuality = 0x02,
    PixelOffsetModeNone        = 0x03,
    PixelOffsetModeHalf        = 0x04
};


/**
   RegionNodeDataType Enumeration

   The RegionNodeDataType enumeration defines types of region node
   data.

   See [MS-EMFPLUS] 2.1.1.27
*/

enum RegionNodeDataType {
    RegionNodeDataTypeAnd        = 0x00000001,
    RegionNodeDataTypeOr         = 0x00000002,
    RegionNodeDataTypeXor        = 0x00000003,
    RegionNodeDataTypeExclude    = 0x00000004,
    RegionNodeDataTypeComplement = 0x00000005,
    RegionNodeDataTypeRect       = 0x10000000,
    RegionNodeDataTypePath       = 0x10000001,
    RegionNodeDataTypeEmpty      = 0x10000002,
    RegionNodeDataTypeInfinite   = 0x10000003
};


/**
   SmoothingMode Enumeration

   The SmoothingMode enumeration defines smoothing modes to apply to
   lines, curves, and the edges of filled areas to make them appear
   more continuous or sharply defined.

   See [MS-EMFPLUS] 2.1.1.28
*/

enum SmoothingMode {
    SmoothingModeDefault      = 0x00,
    SmoothingModeHighSpeed    = 0x01,
    SmoothingModeHighQuality  = 0x02,
    SmoothingModeNone         = 0x03,
    SmoothingModeAntiAlias8x4 = 0x04,
    SmoothingModeAntiAlias8x8 = 0x05
};


/**
   StringAlignment Enumeration

   The StringAlignment enumeration defines ways to align strings with
   respect to a text layout rectangle.

   See [MS-EMFPLUS] 2.1.1.29
*/

enum StringAlignment {
    StringAlignmentNear   = 0x00000000,
    StringAlignmentCenter = 0x00000001,
    StringAlignmentFar    = 0x00000002
};


/**
   StringDigitSubstitution Enumeration

   The StringDigitSubstitution enumeration defines ways to substitute
   digits in a string according to a user's locale or language.

   See [MS-EMFPLUS] 2.1.1.30
*/

enum StringDigitSubstitution {
    StringDigitSubstitutionUser        = 0x00000000,
    StringDigitSubstitutionNone        = 0x00000001,
    StringDigitSubstitutionNational    = 0x00000002,
    StringDigitSubstitutionTraditional = 0x00000003
};


/**
   StringTrimming Enumeration

   The StringTrimming enumeration defines how to trim characters from
   a string that is too large for the text layout rectangle.

   See [MS-EMFPLUS] 2.1.1.31
*/

enum StringTrimming {
    StringTrimmingNone              = 0x00000000,
    StringTrimmingCharacter         = 0x00000001,
    StringTrimmingWord              = 0x00000002,
    StringTrimmingEllipsisCharacter = 0x00000003,
    StringTrimmingEllipsisWord      = 0x00000004,
    StringTrimmingEllipsisPath      = 0x00000005
};


/**
   TextRenderingHint Enumeration

   The TextRenderingHint enumeration defines types of text hinting and
   anti-aliasing, which affects the quality of text rendering.

   See [MS-EMFPLUS] 2.1.1.32
*/

enum TextRenderingHint {
    TextRenderingHintSystemDefault            = 0x00,
    TextRenderingHintSingleBitPerPixelGridFit = 0x01,
    TextRenderingHintSingleBitPerPixel        = 0x02,
    TextRenderingHintAntialiasGridFit         = 0x03,
    TextRenderingHintAntialias                = 0x04,
    TextRenderingHintClearTypeGridFit         = 0x05
};


/**
   UnitType Enumeration

   The UnitType enumeration defines units of measurement in different
   coordinate systems.

   See [MS-EMFPLUS] 2.1.1.33
*/

enum UnitType {
    UnitTypeWorld      = 0x00,
    UnitTypeDisplay    = 0x01,
    UnitTypePixel      = 0x02,
    UnitTypePoint      = 0x03,
    UnitTypeInch       = 0x04,
    UnitTypeDocument   = 0x05,
    UnitTypeMillimeter = 0x06
};


/*
  WrapMode Enumeration

  The WrapMode enumeration defines how the pattern from a texture or
  gradient brush is tiled across a shape or at shape boundaries, when
  it is smaller than the area being filled.

  See [MS-EMFPLUS] 2.1.1.34
*/

enum WrapMode {
    WrapModeTile       = 0x00000000,
    WrapModeTileFlipX  = 0x00000001,
    WrapModeTileFlipY  = 0x00000002,
    WrapModeTileFlipXY = 0x00000003,
    WrapModeClamp      = 0x00000004
};


// ----------------------------------------------------------------
// 2.1.2 Bit flag constant types


/**
   BrushData Flags

   The BrushData flags specify properties of graphics brushes,
   including the presence of optional data fields. These flags can be
   combined to specify multiple options.

   See [MS-EMFPLUS] 2.1.2.1
*/

const uint BrushDataPath             = 0x00000001;
const uint BrushDataTransform        = 0x00000002;
const uint BrushDataPresetColors     = 0x00000004;
const uint BrushDataBlendFactorsH    = 0x00000008;
const uint BrushDataBlendFactorsV    = 0x00000010;
const uint BrushDataFocusScales      = 0x00000040;
const uint BrushDataIsGammaCorrected = 0x00000080;
const uint BrushDataDoNotTransform   = 0x00000100;


/**
   CustomLineCapData Flags

   The CustomLineCapData flags specify data for custom line
   caps. These flags can be combined to specify multiple options.

   See [MS-EMFPLUS] 2.1.2.2
*/

const uint  CustomLineCapDataFillPath = 0x00000001;
const uint  CustomLineCapDataLinePath = 0x00000002;

/**
   DriverStringOptions Flags

   The DriverStringOptions flags specify properties of graphics text
   positioning and rendering. These flags can be combined to specify
   multiple options.

   See [MS-EMFPLUS] 2.1.2.3
*/
const uint  DriverStringOptionsCmapLookup      = 0x00000001;
const uint  DriverStringOptionsVertical        = 0x00000002;
const uint  DriverStringOptionsRealizedAdvance = 0x00000004;
const uint  DriverStringOptionsLimitSubpixel   = 0x00000008;

/**
   FontStyle Flags

   The FontStyle flags specify styles of graphics font
   typefaces. These flags can be combined to specify multiple options.

   See [MS-EMFPLUS] 2.1.2.4
*/

const uint  FontStyleBold      = 0x00000001;
const uint  FontStyleItalic    = 0x00000002;
const uint  FontStyleUnderline = 0x00000004;
const uint  FontStyleStrikeout = 0x00000008;

/**
   PaletteStyle Flags

   The PaletteStyle flags specify properties of graphics
   palettes. These flags can be combined to specify multiple options.

  See [MS-EMFPLUS] 2.1.2.5
*/

const uint  PaletteStyleHasAlpha  = 0x00000001;
const uint  PaletteStyleGrayScale = 0x00000002;
const uint  PaletteStyleHalftone  = 0x00000004;


/**
   PathPointType Flags

   The PathPointType flags specify type properties of points on
   graphics paths. These flags can be combined to specify multiple
   options.

   See [MS-EMFPLUS] 2.1.2.6
*/

const uint  PathPointTypeDashMode     = 0x01;
const uint  PathPointTypePathMarker   = 0x02;
const uint  PathPointTypeCloseSubpath = 0x08;


/**
   PenData Flags

   The PenData flags specify properties of graphics pens, including
   the presence of optional data fields. These flags can be combined
   to specify multiple options.

   See [MS-EMFPLUS] 2.1.2.7
*/

const uint  PenDataTransform        = 0x00000001;
const uint  PenDataStartCap         = 0x00000002;
const uint  PenDataEndCap           = 0x00000004;
const uint  PenDataJoin             = 0x00000008;
const uint  PenDataMiterLimit       = 0x00000010;
const uint  PenDataLineStyle        = 0x00000020;
const uint  PenDataDashedLineCap    = 0x00000040;
const uint  PenDataDashedLineOffset = 0x00000080;
const uint  PenDataDashedLine       = 0x00000100;
const uint  PenDataNonCenter        = 0x00000200;
const uint  PenDataCompoundLine     = 0x00000400;
const uint  PenDataCustomStartCap   = 0x00000800;
const uint  PenDataCustomEndCap     = 0x00001000;


/**
   StringFormat Flags

   The StringFormat flags specify options for graphics text layout,
   including direction, clipping and font handling. These flags can be
   combined to specify multiple options.

   See [MS-EMFPLUS] 2.1.2.8
*/
const uint  StringFormatDirectionRightToLeft  = 0x00000001;
const uint  StringFormatDirectionVertical     = 0x00000002;
const uint  StringFormatNoFitBlackBox         = 0x00000004;
const uint  StringFormatDisplayFormatControl  = 0x00000020;
const uint  StringFormatNoFontFallback        = 0x00000400;
const uint  StringFormatMeasureTrailingSpaces = 0x00000800;
const uint  StringFormatNoWrap                = 0x00001000;
const uint  StringFormatLineLimit             = 0x00002000;
const uint  StringFormatNoClip                = 0x00004000;
const uint  StringFormatBypassGDI             = 0x80000000;


// ----------------------------------------------------------------
// 2.1.3 standard identifier constant types


/**
   ImageEffects Identifiers

   The ImageEffects identifiers define standard GUIDs for specifying
   graphics image effects. These identifiers are used by device
   drivers to publish their levels of support for these effects.

   See [MS-EMFPLUS] 2.1.3.1
*/

#if 0
 BlurEffectGuid {633C80A4-1843-482B-9EF2-BE2834C5FDD4};
 BrightnessContrastEffectGuid {D3A1DBE1-8EC4-4C17-9F4C-EA97AD1C343D};
 ColorBalanceEffectGuid {537E597D-251E-48DA-9664-29CA496B70F8};
 ColorCurveEffectGuid {DD6A0022-58E4-4A67-9D9B-D48EB881A53D};
 ColorLookupTableEffectGuid {A7CE72A9-0F7F-40D7-B3CC-D0C02D5C3212};
 ColorMatrixEffectGuid {718F2615-7933-40E3-A511-5F68FE14DD74};
 HueSaturationLightnessEffectGuid {8B2DD6C3-EB07-4D87-A5F0-7108E26A9C5F};
 LevelsEffectGuid {99C354EC-2A31-4F3A-8C34-17A803B33A25};
 RedEyeCorrectionEffectGuid {74D29D05-69A4-4266-9549-3CC52836B632};
 SharpenEffectGuid {63CBF3EE-C526-402C-8F71-62C540BF5142};
 TintEffectGuid {1077AF00-2848-4441-9489-44AD4C2D7A2C};
#endif

// ----------------------------------------------------------------

}


#endif
