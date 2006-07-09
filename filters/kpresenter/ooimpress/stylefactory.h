/* This file is part of the KDE project
   Copyright (C) 2003 Percy Leonhardt

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


#ifndef STYLEFACTORY_H
#define STYLEFACTORY_H

#include <qptrlist.h>
#include <qstring.h>

#include <qdom.h>

class StyleFactory;

class StrokeDashStyle
{
public:
    StrokeDashStyle( int style );
    ~StrokeDashStyle() {};

    void toXML( QDomDocument & doc, QDomElement & e ) const;
    QString name() const { return m_name; };

private:
    StrokeDashStyle() {};

    QString m_name, m_style, m_dots1, m_dots2, m_dots1_length, m_dots2_length,
        m_distance;
};

class GradientStyle
{
public:
    GradientStyle( QDomElement & gradient, int index );
    ~GradientStyle() {};

    void toXML( QDomDocument & doc, QDomElement & e ) const;
    QString name() const { return m_name; };

private:
    GradientStyle() {};

    QString m_name, m_style, m_cx, m_cy, m_start_color, m_end_color,
        m_start_intensity, m_end_intensity, m_angle, m_border;
};

class MarkerStyle
{
public:
    MarkerStyle( int style );
    ~MarkerStyle() {};

    void toXML( QDomDocument & doc, QDomElement & e ) const;
    QString name() const { return m_name; };

private:
    MarkerStyle() {};

    QString m_name, m_viewBox, m_d;
};

class HatchStyle
{
public:
    HatchStyle( int style, QString & color );
    ~HatchStyle() {};

    void toXML( QDomDocument & doc, QDomElement & e ) const;
    QString name() const { return m_name; };

private:
    HatchStyle() {};

    QString m_name, m_style, m_color, m_distance, m_rotation;
};

class FillImageStyle
{
public:
    FillImageStyle( QString & name );
    ~FillImageStyle() {};

    void toXML( QDomDocument & doc, QDomElement & e ) const;

private:
    FillImageStyle() {};

    QString m_name, m_href, m_type, m_show, m_actuate;
};

class PageMasterStyle
{
public:
    PageMasterStyle( QDomElement & e, const uint index );
    ~PageMasterStyle() {};

    void toXML( QDomDocument & doc, QDomElement & e ) const;
    bool operator==( const PageMasterStyle & pageMasterStyle ) const;
    QString name() const { return m_name; };
    QString style() const { return m_style; };

private:
    PageMasterStyle() {};

    QString m_name, m_page_width, m_page_height, m_orientation, m_style;
    QString m_margin_top, m_margin_bottom, m_margin_left, m_margin_right;
};

class PageStyle
{
public:
    PageStyle( StyleFactory * styleFactory, QDomElement & e, const uint index );
    ~PageStyle() {};

    void toXML( QDomDocument & doc, QDomElement & e ) const;
    bool operator==( const PageStyle & pageStyle ) const;
    QString name() const { return m_name; };

private:
    PageStyle() {};

    QString m_name, m_bg_visible, m_bg_objects_visible, m_fill, m_fill_color,
        m_fill_image_name, m_fill_image_width, m_fill_image_height,
        m_fill_image_ref_point, m_fill_gradient_name, m_repeat, m_page_effect,
        m_page_duration;
};

class TextStyle
{
public:
    TextStyle( QDomElement & e, const uint index );
    ~TextStyle() {};

    void toXML( QDomDocument & doc, QDomElement & e ) const;
    bool operator==( const TextStyle & textStyle ) const;
    QString name() const { return m_name; };

private:
    TextStyle() {};

    QString m_name, m_font_size, m_font_family, m_font_family_generic,
        m_color, m_font_pitch, m_font_style, m_font_weight, m_text_shadow,
        m_text_underline, m_text_underline_color, m_text_crossing_out;
};

class GraphicStyle
{
public:
    GraphicStyle( StyleFactory * styleFactory, QDomElement & e, const uint index );
    GraphicStyle( const char * name,
                  const char * stroke, const char * stroke_color,
                  const char * stroke_width, const char * shadow,
                  const char * shadow_offset_x, const char * shadow_offset_y,
                  const char * shadow_color, const char * margin_left,
                  const char * margin_right, const char * margin_top,
                  const char * margin_bottom, const char * color,
                  const char * text_outline, const char * text_crossing_out,
                  const char * font_family, const char * font_size,
                  const char * font_style, const char * text_shadow,
                  const char * text_underline, const char * font_weight,
                  const char * line_height, const char * text_align,
                  const char * fill, const char * fill_color,
                  const char * enable_numbering );
    ~GraphicStyle() {};

    void toXML( QDomDocument & doc, QDomElement & e ) const;
    bool operator==( const GraphicStyle & graphicStyle ) const;
    QString name() const { return m_name; };

private:
    GraphicStyle() {};

    QString m_name, m_stroke, m_stroke_color, m_stroke_width, m_shadow,
        m_shadow_offset_x, m_shadow_offset_y, m_shadow_color, m_margin_left,
        m_margin_right, m_margin_top, m_margin_bottom, m_color, m_text_outline,
        m_text_crossing_out, m_font_family, m_font_size, m_font_style,
        m_text_shadow, m_text_underline, m_font_weight, m_line_height,
        m_text_align, m_fill, m_fill_color, m_enable_numbering, m_stroke_dash,
        m_fill_hatch_name, m_marker_start, m_marker_start_width,
        m_marker_end, m_marker_end_width, m_fill_gradient_name, m_transparency, m_textAlignment,
	m_textMarginLeft, m_textMarginBottom, m_textMarginTop, m_textMarginRight;
};

class ParagraphStyle
{
public:
    ParagraphStyle( QDomElement & e, const uint index );
    ~ParagraphStyle() {};

    void toXML( QDomDocument & doc, QDomElement & e ) const;
    bool operator==( const ParagraphStyle & paragraphStyle ) const;
    QString name() const { return m_name; };

private:
    ParagraphStyle() {};
    QString parseBorder( QDomElement e );

    QString m_name, m_margin_left, m_margin_right, m_text_indent, m_text_align,
        m_enable_numbering,  m_text_shadow,  m_margin_top, m_margin_bottom,
        m_border_left, m_border_right, m_border_top, m_border_bottom,
        m_line_height, m_line_height_at_least, m_line_spacing;
};

class ListStyle
{
public:
    ListStyle( QDomElement & e, const uint index );
    ~ListStyle() {};

    void toXML( QDomDocument & doc, QDomElement & e ) const;
    bool operator==( const ListStyle & listStyle ) const;
    QString name() const { return m_name; };

private:
    ListStyle() {};

    typedef enum {
        LLS_NUMBER,
        LLS_BULLET
    } list_level_style_t;

    float m_min_label_width;
    list_level_style_t m_listLevelStyle;
    QString m_name, m_num_suffix, m_num_format, m_bullet_char, m_color,
        m_font_size, m_font_family;
};

class StyleFactory
{
public:
    StyleFactory();
    ~StyleFactory();

    void addOfficeStyles( QDomDocument & doc, QDomElement & styles );
    void addOfficeMaster( QDomDocument & doc, QDomElement & master );
    void addOfficeAutomatic( QDomDocument & doc, QDomElement & automatic );
    void addAutomaticStyles( QDomDocument & doc, QDomElement & autoStyles );

    QString createStrokeDashStyle( int style );
    QString createGradientStyle( QDomElement & gradient );
    QString createMarkerStyle( int style );
    QString createHatchStyle( int style, QString & color );
    QString createListStyle( QDomElement & e );
    QString createPageStyle( QDomElement & e );
    QString createTextStyle( QDomElement & e );
    QString createGraphicStyle( QDomElement & e );
    QString createParagraphStyle( QDomElement & e );
    QString createPageMasterStyle( QDomElement & e );

    static QString toCM( const QString & point );

private:
    QPtrList<StrokeDashStyle>   m_strokeDashStyles;
    QPtrList<GradientStyle>     m_gradientStyles;
    QPtrList<HatchStyle>        m_hatchStyles;
    QPtrList<MarkerStyle>       m_markerStyles;
    QPtrList<FillImageStyle>    m_fillImageStyles;
    QPtrList<ListStyle>         m_listStyles;
    QPtrList<PageStyle>         m_pageStyles;
    QPtrList<TextStyle>         m_textStyles;
    QPtrList<GraphicStyle>      m_graphicStyles;
    QPtrList<ParagraphStyle>    m_paragraphStyles;
    QPtrList<PageMasterStyle>   m_pageMasterStyles;
};

#endif
