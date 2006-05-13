/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres, nandres@web.de

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

#ifndef __kspread_style__
#define __kspread_style__

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QPen>

//#include "kspread_format.h"

class QDomDocument;
class QDomElement;
class KoGenStyles;
class KoOasisStyles;
class KoGenStyle;

namespace KSpread
{
class Style;
class CustomStyle;

// used for preloading OASIS auto styles
typedef QHash<QString, Style*>      Styles;
// needs to be ordered (QMap) for the style dialog
typedef QMap<QString, CustomStyle*> CustomStyles;


/**
 * @brief The Style class represents a cell style.
 * Built-in and custom styles, the ones shown in the StyleManager dialog,
 * are CustomStyles. Pure Styles have no name, for instance, and are only
 * used as AUTO styles.
 */
class Style
{
public:
  enum HAlign { Left = 1, Center = 2, Right = 3, HAlignUndefined = 4 };
  enum VAlign { Top = 1, Middle = 2, Bottom = 3, VAlignUndefined = 4 };
  enum FloatFormat { AlwaysSigned = 1, AlwaysUnsigned = 2, OnlyNegSigned = 3 };
  enum FloatColor { NegRed = 1, AllBlack = 2, NegBrackets = 3, NegRedBrackets = 4 };


    struct Currency
    {
      int type;
      QString symbol;
    };


  /// The style type
  enum StyleType
  {
    BUILTIN,   ///< built-in style (the default style)
    CUSTOM,    ///< custom style (defined in the StyleManager dialog)
    AUTO,      ///< automatically generated on cell format changes
    TENTATIVE  ///< @internal temporary state
  };

  enum FontFlags
    {
      FBold      = 0x01,
      FUnderline = 0x02,
      FItalic    = 0x04,
      FStrike    = 0x08
    };

  /// @see Format::FormatFlags
  enum Properties
    {
      PDontPrintText = 0x01,
      PCustomFormat  = 0x02,
      PNotProtected  = 0x04,
      PHideAll       = 0x08,
      PHideFormula   = 0x10,
      PMultiRow      = 0x20,
      PVerticalText  = 0x40
    };

    // TODO Stefan: merge with Format::Properties
    /// @see Format::Properties
    enum FlagsSet
    {
      SHAlign          = 0x01,
      SVAlign          = 0x02,
      //SFactor was here
      SPrefix          = 0x08,
      SPostfix         = 0x10,
      SLeftBorder      = 0x20,
      SRightBorder     = 0x40,
      STopBorder       = 0x80,
      SBottomBorder    = 0x100,
      SFallDiagonal    = 0x200,
      SGoUpDiagonal    = 0x400,
      SBackgroundBrush = 0x800,
      SFont            = 0x1000,
      STextPen         = 0x2000,
      SBackgroundColor = 0x4000,
      SFloatFormat     = 0x8000,
      SFloatColor      = 0x10000,
      SMultiRow        = 0x20000,
      SVerticalText    = 0x40000,
      SPrecision       = 0x80000,
      SFormatType      = 0x100000,
      SAngle           = 0x200000,
      SComment         = 0x400000,
      SIndent          = 0x800000,
      SDontPrintText   = 0x1000000,
      SCustomFormat    = 0x2000000,
      SNotProtected    = 0x4000000,
      SHideAll         = 0x8000000,
      SHideFormula     = 0x10000000,
      SFontSize        = 0x20000000,
      SFontFlag        = 0x40000000,
      SFontFamily      = 0x80000000
    };

  /**
   * Constructor.
   * Creates an empty automatic style.
   */
  Style();
  /**
   * Constructor.
   * Creates an automatic style.
   * If @p style is a custom or built-in style (e.g. the default style),
   * @p style becomes the parent style. In this case, features are NOT SET.
   * @param style The style which features are copied.
   */
  Style( Style* style );

  /**
   * Destructor.
   */
  virtual ~Style();

  /** Returns true if both styles have the same properties */
  bool operator == (const Style& style) const;

    static FormatType formatType( const QString &_format );

    /**
     * @return the name of the data style (number, currency, percentage, date,
     * boolean, text)
     */
    static QString saveOasisStyleNumeric( KoGenStyle &style, KoGenStyles &mainStyles, FormatType _style, const QString &_prefix, const QString &_postfix, int _precision, const QString& symbol );
    static QString saveOasisStyleNumericDate( KoGenStyles &mainStyles, FormatType _style );
    static QString saveOasisStyleNumericFraction( KoGenStyles &mainStyles, FormatType _style, const QString &_prefix, const QString _suffix );
    static QString saveOasisStyleNumericTime( KoGenStyles& mainStyles, FormatType _style );
    static QString saveOasisStyleNumericCustom( KoGenStyles&mainStyles, FormatType _style );
    static QString saveOasisStyleNumericScientific( KoGenStyles&mainStyles, FormatType _style, const QString &_prefix, const QString _suffix, int _precision );
    static QString saveOasisStyleNumericPercentage( KoGenStyles&mainStyles, FormatType _style, int _precision );
    static QString saveOasisStyleNumericMoney( KoGenStyles&mainStyles, FormatType _style, const QString& symbol, int _precision );
    static QString saveOasisStyleNumericText( KoGenStyles&mainStyles, FormatType _style, int _precision );
    static QString saveOasisStyleNumericNumber( KoGenStyles&mainStyles, FormatType _style, int _precision );


  StyleType type() const { return m_type; }

  void saveXML( QDomDocument & doc, QDomElement & format ) const;
  bool loadXML( QDomElement & format );

  /**
   * Saves an OASIS automatic style.
   * Reimplemented by CustomStyle for OASIS user styles.
   * @return always QString::null
   */
  virtual QString saveOasis( KoGenStyle& style, KoGenStyles& mainStyles);
  void loadOasisStyle( KoOasisStyles& oasisStyles, const QDomElement & element );
  static QString saveOasisBackgroundStyle( KoGenStyles &mainStyles, const QBrush &brush );

  /**
   * Releases this style. The internal reference counter is decremented.
   * @return true, if this style is not used anymore and should be deleted.
   */
  bool release();
  /**
   * Marks this style as used. The internal reference counter is incremented.
   */
  void addRef();
  /**
   * @return the number of references to this style.
   */
  int usage() const { return m_usageCount; }

  bool   hasProperty( Style::FlagsSet p ) const;
  bool   hasFeature( FlagsSet f, bool withoutParent ) const;
  uint   features() const { return m_featuresSet; }

  uint bottomPenValue() const { return m_bottomPenValue; }
  uint rightPenValue() const { return m_rightPenValue; }
  uint leftPenValue() const { return m_leftPenValue; }
  uint topPenValue() const { return m_topPenValue; }

  QPen    const & pen()             const;
  QColor  const & bgColor()         const;
  QPen    const & rightBorderPen()  const;
  QPen    const & bottomBorderPen() const;
  QPen    const & leftBorderPen()   const;
  QPen    const & topBorderPen()    const;
  QPen    const & fallDiagonalPen() const;
  QPen    const & goUpDiagonalPen() const;
  QBrush  const & backGroundBrush() const;
  QString const & strFormat()       const;
  QString const & prefix()          const;
  QString const & postfix()         const;
  QString const & fontFamily()      const;

  HAlign      halign()      const;
  VAlign      valign()      const;
  FloatFormat floatFormat() const;
  FloatColor  floatColor()  const;
  FormatType  formatType()  const;

  Currency const & currency() const;

  QFont  font()        const;
  uint   fontFlags()   const;
  int    fontSize()    const;
  int    precision()   const;
  int    rotateAngle() const;
  double indent()      const;

  Style * setHAlign( HAlign  align );
  Style * setVAlign( VAlign align );
  Style * setFont( QFont const & f );
  Style * setFontFamily( QString const & fam );
  Style * setFontFlags( uint flags );
  Style * setFontSize( int size );
  Style * setPen( QPen const & pen );
  Style * setBgColor( QColor const & color );
  Style * setRightBorderPen( QPen const & pen );
  Style * setBottomBorderPen( QPen const & pen );
  Style * setLeftBorderPen( QPen const & pen );
  Style * setTopBorderPen( QPen const & pen );
  Style * setFallDiagonalPen( QPen const & pen );
  Style * setGoUpDiagonalPen( QPen const & pen );
  Style * setRotateAngle( int angle );
  Style * setIndent( double indent );
  Style * setBackGroundBrush( QBrush const & brush );
  Style * setFloatFormat( FloatFormat format );
  Style * setFloatColor( FloatColor color );
  Style * setFormatType( FormatType format );
  Style * setStrFormat( QString const & strFormat );
  Style * setPrecision( int precision );
  Style * setPrefix( QString const & prefix );
  Style * setPostfix( QString const & postfix );
  Style * setCurrency( Currency const & currency );
  Style * setProperty( FlagsSet p );
  Style * clearProperty( FlagsSet p );

  CustomStyle * parent() const;
  QString const & parentName() const { return m_parentName; }
  void setParent( CustomStyle * parent );

  /**
   * Returns the name of a colour.  This is the same as returned by QColor::name, but an internal cache
   * is used to reduce the overhead when asking for the name of the same colour.
   */
  static QString colorName( const QColor& color );

protected:
  /**
   * Helper function for saveOasis
   * Does the real work by determining the used attributes.
   */
  void saveOasisStyle( KoGenStyle &style, KoGenStyles &mainStyles );


  CustomStyle * m_parent;
  QString        m_parentName;
  StyleType      m_type;
  uint           m_usageCount;
  uint           m_featuresSet;

  /**
   * Alignment of the text
   */
  HAlign m_alignX;
  /**
   * Aligment of the text at top middle or bottom
   */
  VAlign m_alignY;

  FloatFormat m_floatFormat;
  /**
   * The color format of a floating point value
   */
  FloatColor m_floatColor;

  FormatType m_formatType;

  /**
   * The font used to draw the text
   */
  QString   m_fontFamily;
  uint      m_fontFlags;
  int       m_fontSize;

  /**
   * The pen used to draw the text
   */
  QPen m_textPen;
  /**
   * The background color
   */
  QColor m_bgColor;

  /**
   * The pen used to draw the right border
   */
  QPen m_rightBorderPen;

  /**
   * The pen used to draw the bottom border
   */
  QPen m_bottomBorderPen;

  /**
   * The pen used to draw the left border
   */
  QPen m_leftBorderPen;

  /**
   * The pen used to draw the top border
   */
  QPen m_topBorderPen;

  /**
   * The pen used to draw the diagonal
   */
  QPen m_fallDiagonalPen;
  /**
   * The pen used to draw the the diagonal which go up
   */
  QPen m_goUpDiagonalPen;

  /**
   * The brush used to draw the background.
   */
  QBrush m_backGroundBrush;

  int m_rotateAngle;
  /**
   * Give indent
   */
  double m_indent;
  /**
   * Format of the content, e.g. #.##0.00, dd/mmm/yyyy,...
   */
  QString m_strFormat;
  /**
   * The precision of the floating point representation
   * If precision is -1, this means that no precision is specified.
   */
  int m_precision;
  /**
   * The prefix of a numeric value ( for example "$" )
   * May be empty.
   */
  QString m_prefix;
  /**
   * The postfix of a numeric value ( for example "DM" )
   * May be empty.
   */
  QString m_postfix;
  /**
   * Currency information:
   * about which currency from which country
   */
  Currency m_currency;

  /**
   * Stores information like: DonPrint, DontShowFormula, Protected...
   */
  uint m_properties;

  uint m_bottomPenValue;
  uint m_rightPenValue;
  uint m_leftPenValue;
  uint m_topPenValue;

  bool featureSet( FlagsSet f ) const { return ( !m_parent || ( m_featuresSet & (uint) f ) ); }
};

/**
 * @brief Built-in or custom style defined in StyleManager dialog.
 */
class CustomStyle : public Style
{
public:
  /**
   * Constructor.
   * Creates a custom style.
   * @param style The style which's features are copied.
   * @param name The name of this style.
   */
  CustomStyle( Style * style, QString const & name );
  CustomStyle( QString const & name, CustomStyle * parent );
  ~CustomStyle();

  QString const & name() const { return m_name; }

  void save( QDomDocument & doc, QDomElement & styles );
  /**
   * @reimp
   * Stores an OASIS user style.
   * @return the OASIS style's name
   */
  virtual QString saveOasis( KoGenStyle& style, KoGenStyles &mainStyles );
  /**
   * Loads the style properties from @p style .
   * Determines also the parent's name.
   * @param oasisStyles map of all styles
   * @param style the DOM element defining the style
   * @param name the style's new name
   */
  void loadOasis( KoOasisStyles& oasisStyles, const QDomElement & style, const QString & name );

  bool loadXML( QDomElement const & style, QString const & name );

  void setType( StyleType type ) { m_type = type; }

  void setName( QString const & name );
  void refreshParentName();
  bool definesAll() const;

  void changeHAlign( HAlign  alignX );
  void changeVAlign( VAlign alignY );
  void changeFont( QFont const & f );
  void changeFontFamily( QString const & fam );
  void changeFontSize( int size );
  void changeFontFlags( uint flags );
  void changePen( QPen const & pen );
  void changeTextColor( QColor const & color );
  void changeBgColor( QColor const & color );
  void changeRightBorderPen( QPen const & pen );
  void changeBottomBorderPen( QPen const & pen );
  void changeLeftBorderPen( QPen const & pen );
  void changeTopBorderPen( QPen const & pen );
  void changeFallBorderPen( QPen const & pen );
  void changeGoUpBorderPen( QPen const & pen );
  void changeRotateAngle( int angle );
  void changeIndent( double indent );
  void changeBackGroundBrush( QBrush const & brush );
  void changeFloatFormat( FloatFormat format );
  void changeFloatColor( FloatColor color );
  void changeFormatType( FormatType format );
  void changeStrFormat( QString const & strFormat );
  void changePrecision( int precision );
  void changePrefix( QString const & prefix );
  void changePostfix( QString const & postfix );
  void changeCurrency( Currency const & currency );

  void addProperty( FlagsSet p );
  void removeProperty( FlagsSet p );

 private:
  friend class StyleManager;

  QString              m_name;

  /**
   * Constructor.
   * Constructs the default cell style.
   */
  CustomStyle();
};

} // namespace KSpread

#endif
