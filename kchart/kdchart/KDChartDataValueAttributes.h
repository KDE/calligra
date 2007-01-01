
/****************************************************************************
 ** Copyright (C) 2005-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
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

#ifndef KDCHARTDATAVALUEATTRIBUTES_H
#define KDCHARTDATAVALUEATTRIBUTES_H

#include <Qt>
#include <QMetaType>
#include "KDChartGlobal.h"
#include "KDChartEnums.h"
#include "KDChartRelativePosition.h"

namespace KDChart {

  class TextAttributes;
  class BackgroundAttributes;
  class FrameAttributes;
  class MarkerAttributes;

  /**
   * \brief Diagram attributes dealing with data value labels.
   *
   * The DataValueAttributes group all properties that can be set
   * wrt data value labels and if and how they are displayed. This
   * includes things like the text attributes (font, color), what
   * markers are used, howmany decimal digits are displayed, etc.
   */
class KDCHART_EXPORT DataValueAttributes
{
public:
  DataValueAttributes();
  DataValueAttributes( const DataValueAttributes& );
  DataValueAttributes &operator= ( const DataValueAttributes& );
  bool operator==( const DataValueAttributes& ) const;
  inline bool operator!=( const DataValueAttributes& other ) const { return !operator==(other); }

  ~DataValueAttributes();

  static const DataValueAttributes& defaultAttributes();
  static const QVariant& defaultAttributesAsVariant();

  /** Set whether data value labels should be displayed.
   * \param visible Whether data value labels should be displayed.
   */
  void setVisible( bool visible );

  /**
   * @return Whether data value labels should be displayed.
   */
  bool isVisible() const;

  /**
   * Set the text attributes to use for the data value labels.
   * \param a The text attributes to set.
   * \see TextAttributes
   */
  void setTextAttributes( const TextAttributes &a );

  /**
   * \return The text attributes used for painting data value labels.
   */
  TextAttributes textAttributes() const;

  /**
   * Set the frame attributes to use for the data value labels area.
   * \param a The frame attributes to set.
   * \see FrameAttributes
   */
  void setFrameAttributes( const FrameAttributes &a );

  /**
   * \return The frame attributes used for painting the data
   * value labels area.
   * \see FrameAttributes
   */
  FrameAttributes frameAttributes() const;

  /**
   * Set the background attributes to use for the data value labels area.
   * \param a The background attributes to set.
   * \see BackgroundAttributes
   */
  void setBackgroundAttributes( const BackgroundAttributes &a );

  /**
   * \return The background attributes used for painting the data
   * value labels area.
   * \see BackgroundAttributes
   */
  BackgroundAttributes backgroundAttributes() const;

  /**
   * Set the marker attributes to use for the data values. This includes
   * the marker type.
   * \param a The marker attributes to set.
   * \see MarkerAttributes
   */
  void setMarkerAttributes( const MarkerAttributes &a );

  /**
   * \return The marker attributes used for decorating the data
   * values.
   * \see MarkerAttributes
   */
  MarkerAttributes markerAttributes() const;

  /**
   * Set how  many decimal digits to display when rendering the data value
   * labels. If there are no decimal digits it will not be displayed.
   * \param digits The number of decimal digits to use.
   */
  void setDecimalDigits( int digits );

  /**
   * \return The number of decimal digits displayed.
   */
  int decimalDigits() const;

  /**
   * Prepend a prefix to the data value text to be displayed
   *
   * \param prefix  The string to prepend.
   */
  void setPrefix( const QString prefix );

 /**
   * \return The string used as a prefix to the data value text.
   */
  QString prefix() const;

  /**
   * Append a suffix to the data value text to be displayed
   *
   * \param suffix  The string to append.
   */
  void setSuffix( const QString suffix );

 /**
   * \return The string used as a suffix to the data value text.
   */
  QString suffix() const;

/**
 *\display a string label instead of the original data value label
 *
 *\param label: the string to be displayed.
 */
  void setDataLabel( const QString label );

 /**
   * \return The string to be displayed instead of the data value text.
   */
  QString dataLabel() const;


  void setPowerOfTenDivisor( int powerOfTenDivisor );
  int powerOfTenDivisor() const;

  void setShowInfinite( bool infinite );
  bool showInfinite() const;

  void setNegativePosition( const RelativePosition& relPosition );
  RelativePosition negativePosition() const;

  void setPositivePosition( const RelativePosition& relPosition );
  RelativePosition positivePosition() const;

private:
  KDCHART_DECLARE_PRIVATE_BASE_VALUE( DataValueAttributes )

}; // End of class DataValueAttributes

}

#if !defined(QT_NO_DEBUG_STREAM)
KDCHART_EXPORT QDebug operator<<(QDebug, const KDChart::DataValueAttributes& );
#endif /* QT_NO_DEBUG_STREAM */

Q_DECLARE_METATYPE( KDChart::DataValueAttributes )
Q_DECLARE_TYPEINFO( KDChart::DataValueAttributes, Q_MOVABLE_TYPE );
KDCHART_DECLARE_SWAP_SPECIALISATION( KDChart::DataValueAttributes )

#endif // KDCHARTDATAVALUEATTRIBUTES_H
