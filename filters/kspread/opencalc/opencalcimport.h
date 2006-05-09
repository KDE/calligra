/* This file is part of the KDE project
   Copyright (C) 2002 Norbert Andres <nandres@web.de>
   Copyright (C) 2004 Montel Laurent <montel@kde.org>

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

#ifndef OpenCalc_IMPORT_H__
#define OpenCalc_IMPORT_H__

#include <KoFilter.h>
#include "kspread_format.h"

#include <q3dict.h>
#include <qdom.h>
//Added by qt3to4:
#include <Q3CString>

class KoStyleStack;
class KoStore;

namespace KSpread
{
class Cell;
class Conditional;
class Doc;
class Sheet;
class Validity;
}

class OpenCalcImport : public KoFilter
{
  Q_OBJECT
 public:
  OpenCalcImport( QObject * parent, const QStringList & );
  virtual ~OpenCalcImport();

  virtual KoFilter::ConversionStatus convert( QByteArray const & from, QByteArray const & to );


 private:

  class OpenCalcPoint
  {
   public:
    OpenCalcPoint( QString const & str );

    QString table;
    QString translation;
    QPoint  topLeft;
    QPoint  botRight;
    bool    isRange;
  };

  enum bPos { Left, Top, Right, Bottom, Fall, GoUp, Border };

  KSpread::Doc *    m_doc;
  KSpread::Format * m_defaultLayout;

  QDomDocument    m_content;
  QDomDocument    m_meta;
  QDomDocument    m_settings;

  Q3Dict<QDomElement>   m_styles;
  Q3Dict<KSpread::Format> m_defaultStyles;
  Q3Dict<QString>       m_formats;
  QMap<QString,QDomElement> m_validationList;

  QStringList          m_namedAreas;

  int  readMetaData();
  bool parseBody( int numOfTables );
  void insertStyles( QDomElement const & element );
  bool createStyleMap( QDomDocument const & styles );
  bool readRowFormat( QDomElement & rowNode, QDomElement * rowStyle,
                      KSpread::Sheet * table, int & row, int & number, bool last );
  bool readColLayouts( QDomElement & content, KSpread::Sheet * table );
  bool readRowsAndCells( QDomElement & content, KSpread::Sheet * table );
  bool readCells( QDomElement & rowNode, KSpread::Sheet  * table, int row, int & columns );
  void convertFormula( QString & text, QString const & f ) const;
  void loadFontStyle( KSpread::Format * layout, QDomElement const * font ) const;
  void readInStyle( KSpread::Format * layout, QDomElement const & style );
  void loadStyleProperties( KSpread::Format * layout, QDomElement const & property ) const;
  void loadBorder( KSpread::Format * layout, QString const & borderDef, bPos pos ) const;
  void loadTableMasterStyle( KSpread::Sheet * table, QString const & stylename );
  QString * loadFormat( QDomElement * element,
                        KSpread::FormatType & formatType,
                        QString name );
  void checkForNamedAreas( QString & formula ) const;
  void loadOasisCellValidation( const QDomElement&body );
  void loadOasisValidation( KSpread::Validity* val, const QString& validationName );
  void loadOasisValidationCondition( KSpread::Validity* val,QString &valExpression );
  void loadOasisAreaName( const QDomElement&body );
  void loadOasisMasterLayoutPage( KSpread::Sheet * table,KoStyleStack &styleStack );
  void loadOasisValidationValue( KSpread::Validity* val, const QStringList &listVal );
    QString translatePar( QString & par ) const;
    void loadCondition( KSpread::Cell*cell,const QDomElement &property );
    void loadOasisCondition(KSpread::Cell*cell,const QDomElement &property );
    void loadOasisConditionValue( const QString &styleCondition, KSpread::Conditional &newCondition );
    void loadOasisCondition( QString &valExpression, KSpread::Conditional &newCondition );
    void loadOasisValidationValue( const QStringList &listVal, KSpread::Conditional &newCondition );
    KoFilter::ConversionStatus loadAndParse( QDomDocument& doc, const QString& fileName,KoStore *m_store );

  KoFilter::ConversionStatus openFile();
};

#endif // OpenCalc_IMPORT_H__

