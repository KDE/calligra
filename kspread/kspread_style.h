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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#ifndef __kspread_style__
#define __kspread_style__

class QDomDocument;
class QDomElement;

#include <qbrush.h>
#include <qcolor.h>
#include <qfont.h>
#include <qpen.h>

#include <koOasisStyles.h>

#include "kspread_format.h"

class KSpreadCustomStyle;
class KoGenStyles;

class KSpreadStyle
{
 public:
  typedef enum E1 { BUILTIN, CUSTOM, AUTO, TENTATIVE } StyleType;

  enum FontFlags
    {
      FBold      = 0x01,
      FUnderline = 0x02,
      FItalic    = 0x04,
      FStrike    = 0x08
    };

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

  enum FlagsSet
    {
      SAlignX          = 0x01,
      SAlignY          = 0x02,
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
      SIndent          = 0x400000,
      SDontPrintText   = 0x800000,
      SCustomFormat    = 0x1000000,
      SNotProtected    = 0x2000000,
      SHideAll         = 0x4000000,
      SHideFormula     = 0x8000000,
      SFontSize        = 0x10000000,
      SFontFlag        = 0x20000000,
      SFontFamily      = 0x40000000
    };

  KSpreadStyle();
  KSpreadStyle( KSpreadStyle * style );

  virtual ~KSpreadStyle();

    static QString saveOasisStyleNumeric( KoGenStyles &mainStyles, FormatType _style, const QString &_prefix, const QString &_postfix);
    static QString saveOasisStyleNumericDate( KoGenStyles &mainStyles, FormatType _style );
    static QString saveOasisStyleNumericFraction( KoGenStyles &mainStyles, FormatType _style, const QString &_prefix, const QString _suffix );
    static QString saveOasisStyleNumericTime( KoGenStyles& mainStyles, FormatType _style );
    static QString saveOasisStyleNumericCustom( KoGenStyles&mainStyles, FormatType _style );
    static QString saveOasisStyleNumericScientific( KoGenStyles&mainStyles, FormatType _style, const QString &_prefix, const QString _suffix );
    static QString saveOasisStyleNumericPercentage( KoGenStyles&mainStyles, FormatType _style );
    static QString saveOasisStyleNumericMoney( KoGenStyles&mainStyles, FormatType _style );
    static QString saveOasisStyleNumericText( KoGenStyles&mainStyles, FormatType _style );
    static QString saveOasisStyleNumericNumber( KoGenStyles&mainStyles, FormatType _style );


  StyleType type() const { return m_type; }

  void saveXML( QDomDocument & doc, QDomElement & format ) const;
  bool loadXML( QDomElement & format );

    QString saveOasisStyle( KoGenStyle &style, KoGenStyles &mainStyles );
    void loadOasisStyle( KoOasisStyles& oasisStyles, const QDomElement & element );


  bool release();
  void addRef();

  int usage() const { return m_usageCount; }

  bool   hasProperty( Properties p ) const;
  bool   hasFeature( FlagsSet f, bool withoutParent ) const;

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

  KSpreadFormat::Align       alignX()      const;
  KSpreadFormat::AlignY      alignY()      const;
  KSpreadFormat::FloatFormat floatFormat() const;
  KSpreadFormat::FloatColor  floatColor()  const;
  FormatType  formatType()  const;

  KSpreadFormat::Currency const & currency() const;

  QFont  font()        const;
  uint   fontFlags()   const;
  int    fontSize()    const;
  int    precision()   const;
  int    rotateAngle() const;
  double indent()      const;

  KSpreadStyle * setAlignX( KSpreadFormat::Align  alignX );
  KSpreadStyle * setAlignY( KSpreadFormat::AlignY alignY );
  KSpreadStyle * setFont( QFont const & f );
  KSpreadStyle * setFontFamily( QString const & fam );
  KSpreadStyle * setFontFlags( uint flags );
  KSpreadStyle * setFontSize( int size );
  KSpreadStyle * setPen( QPen const & pen );
  KSpreadStyle * setBgColor( QColor const & color );
  KSpreadStyle * setRightBorderPen( QPen const & pen );
  KSpreadStyle * setBottomBorderPen( QPen const & pen );
  KSpreadStyle * setLeftBorderPen( QPen const & pen );
  KSpreadStyle * setTopBorderPen( QPen const & pen );
  KSpreadStyle * setFallDiagonalPen( QPen const & pen );
  KSpreadStyle * setGoUpDiagonalPen( QPen const & pen );
  KSpreadStyle * setRotateAngle( int angle );
  KSpreadStyle * setIndent( double indent );
  KSpreadStyle * setBackGroundBrush( QBrush const & brush );
  KSpreadStyle * setFloatFormat( KSpreadFormat::FloatFormat format );
  KSpreadStyle * setFloatColor( KSpreadFormat::FloatColor color );
  KSpreadStyle * setFormatType( FormatType format );
  KSpreadStyle * setStrFormat( QString const & strFormat );
  KSpreadStyle * setPrecision( int precision );
  KSpreadStyle * setPrefix( QString const & prefix );
  KSpreadStyle * setPostfix( QString const & postfix );
  KSpreadStyle * setCurrency( KSpreadFormat::Currency const & currency );
  KSpreadStyle * setProperty( Properties p );
  KSpreadStyle * clearProperty( Properties p );

  KSpreadCustomStyle * parent() const;
  QString const & parentName() const { return m_parentName; }
  void setParent( KSpreadCustomStyle * parent );

 protected:

  KSpreadCustomStyle * m_parent;
  QString        m_parentName;
  StyleType      m_type;
  uint           m_usageCount;
  uint           m_featuresSet;

  /**
   * Alignment of the text
   */
  KSpreadFormat::Align m_alignX;
  /**
   * Aligment of the text at top middle or bottom
   */
  KSpreadFormat::AlignY m_alignY;

  KSpreadFormat::FloatFormat m_floatFormat;
  /**
   * The color format of a floating point value
   */
  KSpreadFormat::FloatColor m_floatColor;

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
  KSpreadFormat::Currency m_currency;

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

class KSpreadCustomStyle : public KSpreadStyle
{
 public:
  KSpreadCustomStyle( KSpreadStyle * parent, QString const & name );
  KSpreadCustomStyle( QString const & name, KSpreadCustomStyle * parent );
  ~KSpreadCustomStyle();

  QString const & name()       const { return m_name;       }

  void save( QDomDocument & doc, QDomElement & styles );
    void saveOasis( KoGenStyles &mainStyles );
    void loadOasis( KoOasisStyles& oasisStyles, const QDomElement & style, const QString & name );

 bool loadXML( QDomElement const & style, QString const & name );

  void setType( StyleType type ) { m_type = type; }

  void setName( QString const & name );
  void refreshParentName();
  bool definesAll() const;

  void changeAlignX( KSpreadFormat::Align  alignX );
  void changeAlignY( KSpreadFormat::AlignY alignY );
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
  void changeFloatFormat( KSpreadFormat::FloatFormat format );
  void changeFloatColor( KSpreadFormat::FloatColor color );
  void changeFormatType( FormatType format );
  void changeStrFormat( QString const & strFormat );
  void changePrecision( int precision );
  void changePrefix( QString const & prefix );
  void changePostfix( QString const & postfix );
  void changeCurrency( KSpreadFormat::Currency const & currency );

  void addProperty( Properties p );
  void removeProperty( Properties p );

 private:
  friend class KSpreadStyleManager;

  QString              m_name;

  KSpreadCustomStyle();
};

#endif
