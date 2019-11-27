/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
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

#ifndef MSOOXMLTHEME_H
#define MSOOXMLTHEME_H

#include <QHash>
#include <QVector>
#include <QColor>
#include <QMap>

#include <KoGenStyles.h>

#include "komsooxml_export.h"


namespace MSOOXML
{

class DrawingMLColorSchemeItem;
class DrawingMLColorSchemeSystemItem;

//! @todo add other classes inheriting DrawingMLColorSchemeItemBase
class KOMSOOXML_EXPORT DrawingMLColorSchemeItemBase
{
public:
    DrawingMLColorSchemeItemBase();
    virtual ~DrawingMLColorSchemeItemBase();
    DrawingMLColorSchemeItem* toColorItem();
    DrawingMLColorSchemeSystemItem* toSystemItem();
    virtual QColor value() const = 0;
    virtual DrawingMLColorSchemeItemBase* clone() const = 0;
};

class KOMSOOXML_EXPORT DrawingMLColorSchemeItem : public DrawingMLColorSchemeItemBase
{
public:
    DrawingMLColorSchemeItem();
    QColor value() const override { return color; }
    QColor color;
    DrawingMLColorSchemeItem* clone() const override { return new DrawingMLColorSchemeItem(*this); }
};

class KOMSOOXML_EXPORT DrawingMLColorSchemeSystemItem : public DrawingMLColorSchemeItemBase
{
public:
    DrawingMLColorSchemeSystemItem();
    QColor value() const override;

    QColor lastColor;
    QString systemColor; //!< ST_SystemColorVal (§20.1.10.58).;
    DrawingMLColorSchemeSystemItem* clone() const override { return new DrawingMLColorSchemeSystemItem(*this); }
};

typedef QHash<QString, DrawingMLColorSchemeItemBase*> DrawingMLColorSchemeItemHash;

//! Implements color scheme, based on hash. All items are owned by this object.
class KOMSOOXML_EXPORT DrawingMLColorScheme : public DrawingMLColorSchemeItemHash
{
public:
    DrawingMLColorScheme();
    ~DrawingMLColorScheme();

    DrawingMLColorSchemeItemBase* value(const QString& name) const {
        return DrawingMLColorSchemeItemHash::value(name);
    }

    /*! @return color value for index. Needed because while PPTX uses lookup by
        name: value(QString&), XLSX uses lookup by index. When index is
        invalid, 0 is returned. */
    DrawingMLColorSchemeItemBase* value(int index) const;

    DrawingMLColorScheme(const DrawingMLColorScheme& scheme);
    DrawingMLColorScheme& operator=(const DrawingMLColorScheme& scheme);
    //! Name of the color scheme
    QString name;
};

//! Font set for majorFont and minorFont.
//! @todo add more support for latin, ea and cs: charser, panose, pitchfamily attributes (21.1.2.3.3)
class KOMSOOXML_EXPORT DrawingMLFontSet
{
public:
    DrawingMLFontSet();
    //! A (script->typeface) hash with font definitions (20.1.4.1.16.)
    QHash<QString, QString> typefacesForScripts;

    //! Specifies that a Latin font be used for a specific run of text.
    QString latinTypeface;

    //! The possible values for this attribute are defined by the ST_TextTypeface simple type
    //! (§20.1.10.81).
    QString eaTypeface;
    //! The possible values for this attribute are defined by the ST_TextTypeface simple type
    //! (§20.1.10.81).
    QString csTypeface;
};

//! Defines the font scheme within the theme
//! The font scheme consists of a pair of major and minor fonts for which to use in a document.s
class KOMSOOXML_EXPORT DrawingMLFontScheme
{
public:
    DrawingMLFontScheme();
    DrawingMLFontSet majorFonts;
    DrawingMLFontSet minorFonts;
    QString name;
};


class KOMSOOXML_EXPORT DrawingMLFillBase
{
public:
    virtual ~DrawingMLFillBase();
    // This function will create the fill style and fill the appropriate styles
    // and filePath if needed.
    // Number is used to index to correct style, color is the color which should be used when making the styles
    virtual void writeStyles(KoGenStyles& styles, KoGenStyle *graphicStyle, const QColor &color) = 0;

    virtual DrawingMLFillBase* clone() const = 0;
};

class KOMSOOXML_EXPORT DrawingMLSolidFill : public DrawingMLFillBase
{
public:
    void writeStyles(KoGenStyles& styles, KoGenStyle *graphicStyle, const QColor &color) override;

    DrawingMLSolidFill* clone() const override { return new DrawingMLSolidFill(*this); }
};

class KOMSOOXML_EXPORT DrawingMLBlipFill : public DrawingMLFillBase
{
public:
    explicit DrawingMLBlipFill(const QString &filePath);
    void writeStyles(KoGenStyles& styles, KoGenStyle *graphicStyle, const QColor &color) override;

    DrawingMLBlipFill* clone() const override { return new DrawingMLBlipFill(*this); }

private:
    QString m_filePath;
};

class KOMSOOXML_EXPORT DrawingMLGradientFill : public DrawingMLFillBase
{
public:
    // Simplified gradient constructor
    DrawingMLGradientFill(const QVector<qreal> &shadeModifier, const QVector<qreal> &tintModifier, const QVector<qreal> &satModifier,
                          const QVector<int> &alphaModifier, const QVector<int> &gradPositions, const QString &gradAngle);
    void writeStyles(KoGenStyles& styles, KoGenStyle *graphicStyle, const QColor &color) override;

    DrawingMLGradientFill* clone() const override { return new DrawingMLGradientFill(*this); }

private:
    QVector<qreal> m_shadeModifier;
    QVector<qreal> m_tintModifier;
    QVector<qreal> m_satModifier;
    QVector<int> m_alphaModifier;
    QVector<int> m_gradPosition;
    QString m_gradAngle;
};

class KOMSOOXML_EXPORT DrawingMLFormatScheme
{
public:

    DrawingMLFormatScheme();
    ~DrawingMLFormatScheme();
    QString name;

    DrawingMLFormatScheme(const DrawingMLFormatScheme& format);
    DrawingMLFormatScheme& operator=(const DrawingMLFormatScheme& format);

    QMap<int, DrawingMLFillBase*> fillStyles;

    //! Stores the three line styles for use within a theme.
    QList<KoGenStyle> lnStyleLst;
};

//! Defines a single DrawingML theme.
//! @todo support objectDefaults and extraClrSchemeLst
class KOMSOOXML_EXPORT DrawingMLTheme
{
public:
    DrawingMLTheme();
    QString name;
    DrawingMLColorScheme colorScheme;
    DrawingMLFontScheme fontScheme;
    DrawingMLFormatScheme formatScheme;
};


} // namespace MSOOXML

#endif //MSOOXMLTHEME_H
