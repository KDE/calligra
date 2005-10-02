/* This file is part of the KDE project

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

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <qstring.h>

struct xfrec
{
	Q_UINT16 ifnt, ifmt, attr, align, indent, borderStyle, sideBColor;
	Q_UINT32 topBColor;
	Q_UINT16 cellColor;
};

struct fontrec
{
	Q_UINT16 dyHeight, grbit, icv, bls, sss;
	Q_UINT8 uls, bFamily, bCharSet, reserved, cch;
	QString rgch;
};

struct formatrec
{
	Q_UINT8 cch;
	char *rgch;
};

static const int BIFF_5_7 = 0x0500;
static const int BIFF_8 = 0x0600;

const char *const LineWeightText[] =
{
	"hairline", "normal", "medium", "wide"
};

const char *const LinePatternText[] =
{
	"solid", "dashed", "doted", "dash doted",
	"dash dot doted", "invisible", "dark gray",
	"medium gray", "light gray"
};

const char *const ChartSeriesText[] =
{
	"Dates", "Numbers", "Sequences", "Strings"
};

const char *const ChartBlankText[] =
{
	"Skip blanks", "Blanks are zero", "Interpolate blanks"
};

const char *const AxisText[] =
{
	"X axis", "Y axis", "Series axis"
};

const char *const LegendLocationText[] =
{
	"bottom", "corner", "top", "left", "right",
	"invalid1", "invalid2", "Not docked"
};

typedef enum
{
	WGT_MODE_MIN        = -2,
	WGT_MODE_HAIRLINE   = -1,
	WGT_MODE_NORMAL     = 0,
	WGT_MODE_MEDIUM     = 1,
	WGT_MODE_WIDE       = 2,
	WGT_MODE_MAX        = 3
} LineWeight;

typedef enum
{
	PAT_MODE_SOLID          = 0,
	PAT_MODE_DASH           = 1,
	PAT_MODE_DOT            = 2,
	PAT_MODE_DASH_DOT       = 3,
	PAT_MODE_DASH_DOT_DOT   = 4,
	PAT_MODE_NONE           = 5,
	PAT_MODE_DARK_GRAY      = 6,
	PAT_MODE_MED_GRAY       = 7,
	PAT_MODE_LIGHT_GRAY     = 8,
	PAT_MODE_MAX            = 9
} LinePattern;

typedef enum
{
	CHART_SERIES_DATES      = 0,
	CHART_SERIES_NUMBERS    = 1,
	CHART_SERIES_SEQUENCES  = 2,
	CHART_SERIES_STRINGS    = 3,
	CHART_SERIES_MAX        = 4
} ChartSeries;

typedef enum
{
	CHART_BLANK_SKIP        = 0,
	CHART_BLANK_ZERO        = 1,
	CHART_BLANK_INTERPOLATE = 2,
	CHART_BLANK_MAX         = 3
} ChartBlank;

typedef enum
{
	AXIS_X      = 0,
	AXIS_Y      = 1,
	AXIS_SERIES = 2,
	AXIS_MAX    = 3
} Axis;

typedef enum
{
	LEGEND_LOCATION_BOTTOM      = 0,
	LEGEND_LOCATION_CORNER      = 1,
	LEGEND_LOCATION_TOP         = 2,
	LEGEND_LOCATION_LEFT        = 3,
	LEGEND_LOCATION_RIGHT       = 4,
	LEGEND_LOCATION_INVALID1    = 5,
	LEGEND_LOCATION_INVALID2    = 6,
	LEGEND_LOCATION_NOT_DOCKED  = 7,
	LEGEND_LOCATION_MAX         = 8
} LegendLocation;

typedef enum
{
	STYLE_BORDER_NONE                   = 0,
	STYLE_BORDER_THIN                   = 1,
	STYLE_BORDER_MEDIUM                 = 2,
	STYLE_BORDER_DASHED                 = 3,
	STYLE_BORDER_DOTTED                 = 4,
	STYLE_BORDER_THICK                  = 5,
	STYLE_BORDER_DOUBLE                 = 6,
	STYLE_BORDER_HAIR                   = 7,
	STYLE_BORDER_MEDIUM_DASH            = 8,
	STYLE_BORDER_DASH_DOT               = 9,
	STYLE_BORDER_MEDIUM_DASH_DOT        = 10,
	STYLE_BORDER_DASH_DOT_DOT           = 11,
	STYLE_BORDER_MEDIUM_DASH_DOT_DOT    = 12,
	STYLE_BORDER_SLANTED_DASH_DOT       = 13
} BorderStyle;

typedef enum
{
	D_XF 		= 0,
	D_SST		= 1,
	D_FONT 		= 2,
	D_FORMAT	= 3
} Dictionary;

const char *const palette[65] =
{
	"#000000", "#ffffff", "#ffffff", "#ffffff", "#ffffff", "#ffffff",
	"#ffffff", "#ffffff", "#000000", "#ffffff", "#ff0000", "#00ff00",
	"#0000ff", "#ffff00", "#ff00ff", "#00ffff", "#800000", "#008000",
	"#000080", "#808000", "#800080", "#008080", "#c0c0c0", "#808080",
	"#9999ff", "#993366", "#ffffcc", "#ccffff", "#660066", "#ff8080",
	"#0066cc", "#ccccff", "#000080", "#ff00ff", "#ffff00", "#00ffff",
	"#800080", "#800000", "#008080", "#0000ff", "#00ccff", "#ccffff",
	"#ccffcc", "#ffff99", "#99ccff", "#ff99cc", "#cc99ff", "#ffcc99",
	"#3366ff", "#33cccc", "#99cc00", "#ffcc00", "#ff9900", "#ff6600",
	"#666699", "#969696", "#003366", "#339966", "#003300", "#333300",
	"#993300", "#993366", "#333399", "#333333", "#ffffff"
};

const int ndays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const int ldays[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#endif
