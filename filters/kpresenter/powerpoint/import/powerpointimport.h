/* This file is part of the KDE project
   Copyright (C) 2005 Yolla Indria <yolla.indria@gmail.com>

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

#ifndef POWERPOINTIMPORT_H
#define POWERPOINTIMPORT_H

#include <KoFilter.h>
#include <KoStore.h>
#include <KoGenStyles.h>

#include "libppt.h"

using namespace Libppt;

class KoXmlWriter;

class PowerPointImport : public KoFilter
{
    Q_OBJECT

public:
    PowerPointImport(QObject *parent, const QStringList&);
    virtual ~PowerPointImport();
    virtual KoFilter::ConversionStatus convert(const QByteArray& from,
            const QByteArray& to);

private:
    class Private;
    Private* d;

    void createMainStyles(KoGenStyles& styles);
    QByteArray createContent(KoGenStyles& styles);

    void processSlideForBody(unsigned slideNo, Slide* slide, KoXmlWriter* xmlWriter);
    void processObjectForBody(Object* object, KoXmlWriter* xmlWriter);
    void processGroupObjectForBody(GroupObject* groupObject, KoXmlWriter* xmlWriter);
    void processDrawingObjectForBody(DrawObject* drawObject, KoXmlWriter* xmlWriter);
    void processTextObjectForBody(TextObject* textObject, KoXmlWriter* xmlWriter);

    void processSlideForStyle(unsigned slideNo, Slide* slide, KoGenStyles &styles);
    void processObjectForStyle(Object* object, KoGenStyles &styles);
    void processGroupObjectForStyle(GroupObject* groupObject, KoGenStyles &styles);
    void processDrawingObjectForStyle(DrawObject* drawObject, KoGenStyles &styles);
    void processTextObjectForStyle(TextObject* textObject, KoGenStyles &styles);

    /**
    * @brief Write itendation element (text:list and text:list-item) specified
    * amount, so that the last list doesn't contain a list-item.
    * @param xmlWriter XML Writer to write text:list and text:list-item elements
    * @param count How many elements to write
    * @param style name of the style to apply to the first text:list element
    */
    void writeTextObjectIndent(KoXmlWriter* xmlWriter,
                               const unsigned int count,
                               const QString &style);

    /**
    * @brief Write text deindentations the specified amount. Actually it just
    * closes elements.
    *
    * Doesn't close the last text:list-item though.
    * @param xmlWriter XML writer to write closing tags
    * @param count how many lists and list items to close (excluding last
    * list item)
    */
    void writeTextObjectDeIndent(KoXmlWriter* xmlWriter,
                                 const unsigned int count);

    /**
    * @brief Write text all text within specified paragraph
    *
    * @param xmlWriter XML writer to write with
    * @param pf Text paragraph exception to define style for the text paragraph
    * @param textObject text object whose contents to write
    * @param textPos position of current paragraph within all the text in
    * textObject
    * @param indent Current intendation level
    * @param listStyle current list style
    */
    void writeTextPFException(KoXmlWriter* xmlWriter,
                              TextPFRun *pf,
                              TextObject *textObject,
                              const unsigned int textPos,
                              unsigned int &indent,
                              QString &listStyle);

    /**
    * @brief Write specified line within specified paragraph with xmlWriter
    * @param xmlWriter XML writer to write with
    * @param pf Text paragraph exception to define paragraph style for the line
    * @param textObject text object whose contents to write
    * @param text Line to write
    * @param linePosition what is the index of the whole text (within textObject)
    * that this line starts from
    */
    void writeTextLine(KoXmlWriter* xmlWriter,
                       TextPFException *pf,
                       TextObject *textObject,
                       const QString &text,
                       const unsigned int linePosition);

    /**
    * @brief write part of a line (bound by the same text character exception)
    * @param xmlWriter XML writer to write with
    * @param cf character exception that applies to text we are about to write
    * @param pf paragraph exception that applies to text we are about to write
    * @param textObject text object that contains the text
    * @param text text to write
    */
    void writeTextCFException(KoXmlWriter* xmlWriter,
                              TextCFException *cf,
                              TextPFException *pf,
                              TextObject *textObject,
                              const QString &text);

    /**
    * @brief Parse all styles from given TextCFRun and TextPFRun pair
    * @param cf TextCFRun that provides character style
    * @param pf TextPFRun that provides paragraph style
    * @param styles KoGenStyles to store styles to
    * @param textObject TextObject to cache style names to
    */
    void processTextExceptionsForStyle(TextCFRun *cf,
                                       TextPFRun *pf,
                                       KoGenStyles &styles,
                                       TextObject* textObject);

    /**
    * @brief Helper method to find specified TextParagraphException from
    * MainMasterContainer
    * @param text type of the text whose style to get. See TextTypeEnum in
    * [MS-PPT].pdf
    * @param level TextMasterStyleLevel index (indentation level)
    */
    TextPFException *masterTextPFException(int type, unsigned int level);

    /**
    * @brief Helper method to find specified TextCharacterException from
    * MainMasterContainer
    * @param text type of the text whose style to get. See TextTypeEnum in
    * [MS-PPT].pdf
    * @param level TextMasterStyleLevel index (indentation level)
    */
    TextCFException *masterTextCFException(int type, unsigned int level);

    /**
    * @brief Convert paraspacing value to centimeters
    *
    * ParaSpacing is a 2-byte signed integer that specifies text paragraph
    * spacing. It MUST be a value from the following table:
    * Range                     Meaning
    * 0 to 13200, inclusive.    The value specifies spacing as a percentage of the
    *                           text line height.
    * Less than 0.              The absolute value specifies spacing in master
    *                           units.
    *
    * master unit: A unit of linear measurement that is equal to 1/576 inch.
    *
    * @brief value Value to convert
    * @return value converted to centimeters
    */
    QString paraSpacingToCm(int value) const;

    /**
    * @brief Convert TextAlignmentEnum value to a string from ODF specification
    * An enumeration that specifies paragraph alignments.
    * Name                              Value       Meaning
    * Tx_ALIGNLeft              0x0000    For horizontal text, left aligned.
    *                                     For vertical text, top aligned.
    * Tx_ALIGNCenter            0x0001    For horizontal text, centered.
    *                                     For vertical text, middle aligned.
    * Tx_ALIGNRight             0x0002    For horizontal text, right aligned.
    *                                     For vertical text, bottom aligned.
    * Tx_ALIGNJustify           0x0003    For horizontal text, flush left and
    *                                     right.
    *                                     For vertical text, flush top and bottom.
    * Tx_ALIGNDistributed       0x0004    Distribute space between characters.
    * Tx_ALIGNThaiDistributed   0x0005    Thai distribution justification.
    * Tx_ALIGNJustifyLow        0x0006    Kashida justify low.
    *
    * NOTE values Tx_ALIGNDistributed, Tx_ALIGNThaiDistributed and
    * Tx_ALIGNJustifyLow were not found in ODF specification v1.1 and are
    * ignored at the moment. Values "end" and "start" from ODF specification v1.1
    * were not found in [MS-PPT].pdf and are also ignored.
    *
    * @param value TextAlignmentEnum value to convert
    * @return value as string from ODF specification or an empty string if
    * conversion failed
    */
    QString textAlignmentToString(unsigned int value) const;

    /**
    * @brief convert Master Unit values to centimeters
    *
    * From [MS-PPT].pdf:
    * master unit: A unit of linear measurement that is equal to 1/576 inch.
    *
    * @param value master unit value to convert
    * @return value as centimeters with cm appended to the end
    */
    QString pptMasterUnitToCm(unsigned int value) const;

    /**
    * @brief Converts ColorIndexStruct to QColor
    *
    * Color index struct can either contain rgb values or a index to color
    * in MainMasterContainer's SlideSchemeColorSchemeAtom. This method
    * returns the rgb values the specified struct refers to.
    * @param color Color to convert
    * @return QColor value
    */
    QColor colorIndexStructToQColor(const ColorIndexStruct &color);

    void processEllipse(DrawObject* drawObject, KoXmlWriter* xmlWriter);
    void processRectangle(DrawObject* drawObject, KoXmlWriter* xmlWriter);
    void processRoundRectangle(DrawObject* drawObject, KoXmlWriter* xmlWriter);
    void processDiamond(DrawObject* drawObject, KoXmlWriter* xmlWriter);
    void processTriangle(DrawObject* drawObject, KoXmlWriter* xmlWriter);
    void processTrapezoid(DrawObject* drawObject, KoXmlWriter* xmlWriter);
    void processParallelogram(DrawObject* drawObject, KoXmlWriter* xmlWriter);
    void processHexagon(DrawObject* drawObject, KoXmlWriter* xmlWriter);
    void processOctagon(DrawObject* drawObject, KoXmlWriter* xmlWriter);
    void processArrow(DrawObject* drawObject, KoXmlWriter* xmlWriter);
    void processLine(DrawObject* drawObject, KoXmlWriter* xmlWriter);
    void processSmiley(DrawObject* drawObject, KoXmlWriter* xmlWriter);
    void processHeart(DrawObject* drawObject, KoXmlWriter* xmlWriter);
    void processFreeLine(DrawObject* drawObject, KoXmlWriter* xmlWriter);
    void processPictureFrame(DrawObject* drawObject, KoXmlWriter* xmlWriter);
};

#endif // POWERPOINTIMPORT_H
