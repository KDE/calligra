/* This file is part of the Calligra project

   Copyright (C) 2009 Benjamin Cail <cricketc@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef PARAGRAPH_H
#define PARAGRAPH_H

#include <QBuffer>
#include <QList>
#include <QStack>
#include <KoGenStyle.h>
#include <KoGenStyles.h>
#include <KoXmlWriter.h>

#include <wv2/src/styles.h>
#include <wv2/src/paragraphproperties.h>
#include <wv2/src/parser.h>

class Paragraph
{
public:
    explicit Paragraph(KoGenStyles* mainStyles, const QString& bgColor, bool inStylesDotXml = false,
                       bool isHeading = false, bool inHeader = false, int outlineLevel = 0);
    ~Paragraph();

    /**
     * Write the paragraph content into the @writer.
     *
     * While applying the paragraph style, store the tab leader (leader-text)
     * character into @tabLeader if requested by the calling handler.
     *
     * @return the name of the last KoGenStyle inserted into the styles
     * collection.
     */
    QString writeToFile(KoXmlWriter* writer, bool openNewTextBox, QChar* tabLeader=0);

    void addRunOfText(QString text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp, const QString &fontName,
                      const wvWare::StyleSheet& styles, bool addCompleteElement=false);

    /**
     * TODO:
     */
    void openInnerParagraph();

    /**
     * TODO:
     */
    void closeInnerParagraph();

    /**
     * TODO:
     */
    int strings() const;

    /**
     * TODO:
     */
    QString string(int index) const;

    /**
     * Set the paragraph properties (PAP) that apply to the paragraph.
     */
    void setParagraphProperties(wvWare::SharedPtr<const wvWare::ParagraphProperties> pap);

    /**
     * Set the character properties (CHP) that apply to the paragraph.
     *
     * @param CHPs provided by wv2 for empty paragraphs to set proper
     * font-size, line-height, etc. into text-properties.
     */
    void setCharacterProperties(wvWare::SharedPtr<const wvWare::Word97::CHP> chp) {
        m_characterProperties = chp;
    }

    /**
     * Set the built-in (named) style that applies to the paragraph.
     */
    void setParagraphStyle(const wvWare::Style* paragraphStyle);

    /**
     * @return the built-in (named) style that applies to the paragraph.
     */
    const wvWare::Style* paragraphStyle() const { return m_paragraphStyle; }

    /**
     * @return the KoGenStyle of family Paragraph prepared for the current
     * paragraph to other handlers.
     */
    KoGenStyle* koGenStyle() const { return m_odfParagraphStyle; };

    /**
     * @return true in case this paragraph is a heading
     */
    bool isHeading() const { return m_isHeading; };

    // DropCaps related
    typedef enum { NoDropCap, IsDropCapPara, HasDropCapIntegrated }  DropCapStatus;

    DropCapStatus dropCapStatus() const;
    void getDropCapData(QString *string, int *type, int *lines, qreal *distance, QString *style) const;
    void addDropCap(QString &string, int type, int lines, qreal distance, const QString &style);

    /**
     * @return true in case the current paragraph contains a field of type in
     * {PAGE, NUMPAGES}.
     */
    bool containsPageNumberField() const { return m_containsPageNumberField; }

    /**
     * Provide the information that a field of type in {PAGE, NUMPAGES} was
     * detected in the current paragraph.
     */
    void setContainsPageNumberField(bool containsPageNumberField) {
        m_containsPageNumberField = containsPageNumberField;
    }

    /**
     * Set the combined characters flag.
     */
    void setCombinedCharacters(bool isCombined) {
        m_combinedCharacters = isCombined;
    }

    // Static functions which process wvWare properties and store them into
    // corresponding properties of a KoGenStyle.
    static void applyParagraphProperties(const wvWare::ParagraphProperties& properties,
                                         KoGenStyle* style, const wvWare::Style* parentStyle,
                                         bool setDefaultAlign, Paragraph *paragraph,
                                         QChar* tabLeader = 0,
                                         const QString& bgColor = QString());

    static void applyCharacterProperties(const wvWare::Word97::CHP* chp,
                                         KoGenStyle* style, const wvWare::Style* parentStyle,
                                         bool suppressFontSize = false, bool combineCharacters = false,
                                         const QString& bgColor = QString());

    /**
     * Add a color item to the background-color stack.
     * @param color in the format "#RRGGBB"
     */
    static void pushBgColor(const QString& val) { m_bgColors.push(val); }

    /**
     * Remove the last item from the background-color stack.
     */
    static void popBgColor(void);

    /**
     * Update the last item of the background-color stack.
     * @param color in the format "#RRGGBB"
     */
    static void updateBgColor(const QString& val);

    /**
     * @return the background color in the format "#RRGGBB" or an empty string.
     */
    static QString currentBgColor(void) { return m_bgColors.isEmpty() ? QString() : m_bgColors.top(); }

    /**
     * NOTE: DEPRECATED
     * A special purpose method, which creates a KoGenStyle for a <text:span>
     * element and inserts it into the styles collection.  Use this function if
     * you have to create XML snippets.  In any other case use addRunOfText.
     * @return the style name.
     */
    QString createTextStyle(wvWare::SharedPtr<const wvWare::Word97::CHP> chp, const wvWare::StyleSheet& styles);

private:
    wvWare::SharedPtr<const wvWare::ParagraphProperties> m_paragraphProperties;
    wvWare::SharedPtr<const wvWare::ParagraphProperties> m_paragraphProperties2;
    wvWare::SharedPtr<const wvWare::Word97::CHP> m_characterProperties;

    // ODF styles.  The MS equivalents are below.
    KoGenStyle* m_odfParagraphStyle; //pointer to Calligra structure for paragraph formatting
    KoGenStyle* m_odfParagraphStyle2; //place to store original style when we have an inner paragraph
    KoGenStyles* m_mainStyles; //pointer to style collection for this document

    // MS Styles
    const wvWare::Style* m_paragraphStyle;  // style for the paragraph
    const wvWare::Style* m_paragraphStyle2; // style when in inner paragraph

    //std::vector<QString> m_textStrings; // list of text strings within a paragraph
    //std::vector<QString> m_textStrings2; // original list when in inner paragraph
    QList<QString> m_textStrings;  // list of text strings within a paragraph
    QList<QString> m_textStrings2; // original list when in inner paragraph
    QList<const KoGenStyle*> m_textStyles;  // list of styles for text within a paragraph
    QList<const KoGenStyle*> m_textStyles2; // original list when in inner paragraph
    std::vector<bool> m_addCompleteElement; // flags controlling if the paragraph should be processed
    std::vector<bool> m_addCompleteElement2; // original list when in inner paragraph

    bool m_inStylesDotXml; //let us know if we're in content.xml or styles.xml
    bool m_isHeading; //information for writing a heading instead of a paragraph
    bool m_inHeaderFooter;

    int m_outlineLevel;

    DropCapStatus  m_dropCapStatus; // True if this paragraph has a dropcap
    QString m_dropCapStyleName;
    qreal m_dropCapDistance;
    int m_dcs_fdct;
    int m_dcs_lines;

    bool m_containsPageNumberField;
    bool m_combinedCharacters;            // is true when the next characters are combined

    //A stack for background-colors, which represents a background color context
    //for automatic colors.
    static QStack<QString> m_bgColors;

}; //end class Paragraph
#endif //PARAGRAPH_H
