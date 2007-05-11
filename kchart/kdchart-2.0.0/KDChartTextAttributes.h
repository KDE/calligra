/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2005-2006 Klarävdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Chart licenses may use this file in
 ** accordance with the KD Chart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdchart for
 **   information about KD Chart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#ifndef KDCHARTTEXTATTRIBUTES_H
#define KDCHARTTEXTATTRIBUTES_H

#include <QDebug>
#include <QMetaType>
#include "KDChartGlobal.h"
#include "KDChartMeasure.h"

class QPen;
class QFont;

namespace KDChart {

    /**
     * \brief A set of text attributes.
     *
     * TextAttributes encapsulates settings that have to do with
     * text. This includes font, fontsize, color, whether the text
     * is rotated, etc
     */
class KDCHART_EXPORT TextAttributes
{
public:
  TextAttributes();
  TextAttributes( const TextAttributes& );
  TextAttributes &operator= ( const TextAttributes& );
  bool operator==( const TextAttributes& ) const;
  inline bool operator!=( const TextAttributes& other ) const
  { return !operator==(other); }

  ~TextAttributes();

  /**
   * Set whether the text is to be rendered at all.
   * \param visible Whether the text is visible.
   */
  void setVisible( bool visible );

  /**
   * \return Whether the text is visible.
   */
  bool isVisible() const;

  /**
   * Set the font to be used for rendering the text.
   * \param font The font to use.
   */
  void setFont( const QFont& font );

  /**
   * \return The font that is used for rendering text.
   */
  QFont font() const;

  /**
   * Set the size of the font used for rendering text.
   * \param measure The measure to use.
   * \see Measure
   */
  void setFontSize( const Measure & measure );

  /**
   * \return The measure used for the font size.
   */
  Measure fontSize() const;

  /**
   * Set the minimal size of the font used for rendering text.
   * \param measure The measure to use.
   * \see Measure
   */
  void setMinimalFontSize( const Measure & measure );

  /**
   * \return The measure used for the minimal font size.
   */
  Measure minimalFontSize() const;

  /**
   * \return Whether the text has an absolute font size set.
   */
  bool hasAbsoluteFontSize() const;

  /**
   * Set whether the text should be automatically rotated as
   * needed when space is constraint.
   * \param autoRotate Whether text should be automatically rotated.
   */
  void setAutoRotate( bool autoRotate );

  /**
   * \return Whether text is automatically rotated when space is
   * constrained.
   */
  bool autoRotate() const;

  /**
   * Set whether the text should automatically be shrunk, if
   * space is constraint.
   * \param autoShrink Whether text should be auto-shrunk.
   */
  void setAutoShrink( bool autoShrink );

  /**
   * \return Whether text is automatically shrunk if space is
   * constraint.
   */
  bool autoShrink() const;

  /**
   * Set the rotation angle to use for the text.
   * \param rotation The rotation angle.
   */
  void setRotation( int rotation );

  /**
   * \return The rotation angle used for rendering the text.
   */
  int rotation() const;

  /**
   * Set the pen to use for rendering the text.
   * \param rotation The pen to use.
   */
  void setPen( const QPen& pen );

  /**
   * \return The pen used for rendering the text.
   */
  QPen pen() const;

  // FIXME KDChartEnums::TextLayoutPolicy?

private:
  KDCHART_DECLARE_PRIVATE_BASE_VALUE( TextAttributes )

}; // End of class TextAttributes

}

#if !defined(QT_NO_DEBUG_STREAM)
KDCHART_EXPORT QDebug operator<<(QDebug, const KDChart::TextAttributes& );
#endif /* QT_NO_DEBUG_STREAM */

KDCHART_DECLARE_SWAP_SPECIALISATION( KDChart::TextAttributes )
Q_DECLARE_METATYPE( KDChart::TextAttributes )
Q_DECLARE_TYPEINFO( KDChart::TextAttributes, Q_MOVABLE_TYPE );

#endif // KDCHARTTEXTATTRIBUTES_H
