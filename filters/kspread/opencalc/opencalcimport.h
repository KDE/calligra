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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef OpenCalc_IMPORT_H__
#define OpenCalc_IMPORT_H__

#include <koFilter.h>
#include "kspread_format.h"

#include <qdict.h>
#include <qdom.h>

class KSpreadCell;
class KSpreadDoc;
class KSpreadSheet;
class KSpreadValidity;
class KoStyleStack;
class KSpreadConditional;
class KoStore;

class OpenCalcImport : public KoFilter
{
  Q_OBJECT
 public:
  OpenCalcImport( KoFilter * parent, const char * name, const QStringList & );
  virtual ~OpenCalcImport();

  virtual KoFilter::ConversionStatus convert( QCString const & from, QCString const & to );


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

  KSpreadDoc *    m_doc;
  KSpreadFormat * m_defaultLayout;

  QDomDocument    m_content;
  QDomDocument    m_meta;
  QDomDocument    m_settings;

  QDict<QDomElement>   m_styles;
  QDict<KSpreadFormat> m_defaultStyles;
  QDict<QString>       m_formats;
  QMap<QString,QDomElement> m_validationList;

  QStringList          m_namedAreas;

  int  readMetaData();
  bool parseBody( int numOfTables );
  void insertStyles( QDomElement const & element );
  bool createStyleMap( QDomDocument const & styles );
  bool readRowFormat( QDomElement & rowNode, QDomElement * rowStyle,
                      KSpreadSheet * table, int & row, int & number, bool last );
  bool readColLayouts( QDomElement & content, KSpreadSheet * table );
  bool readRowsAndCells( QDomElement & content, KSpreadSheet * table );
  bool readCells( QDomElement & rowNode, KSpreadSheet  * table, int row, int & columns );
  void convertFormula( QString & text, QString const & f ) const;
  void loadFontStyle( KSpreadFormat * layout, QDomElement const * font ) const;
  void readInStyle( KSpreadFormat * layout, QDomElement const & style );
  void loadStyleProperties( KSpreadFormat * layout, QDomElement const & property ) const;
  void loadBorder( KSpreadFormat * layout, QString const & borderDef, bPos pos ) const;
  void loadTableMasterStyle( KSpreadSheet * table, QString const & stylename );
  QString * loadFormat( QDomElement * element,
                        FormatType & formatType,
                        QString name );
  void checkForNamedAreas( QString & formula ) const;
  void loadOasisCellValidation( const QDomElement&body );
  void loadOasisValidation( KSpreadValidity* val, const QString& validationName );
  void loadOasisValidationCondition( KSpreadValidity* val,QString &valExpression );
  void loadOasisAreaName( const QDomElement&body );
  void loadOasisMasterLayoutPage( KSpreadSheet * table,KoStyleStack &styleStack );
    void loadOasisValidationValue( KSpreadValidity* val, const QStringList &listVal );
    QString translatePar( QString & par ) const;
    void loadCondition( KSpreadCell *cell,const QDomElement &property );
    void loadOasisCondition(KSpreadCell *cell,const QDomElement &property );
    void loadOasisConditionValue( const QString &styleCondition, KSpreadConditional &newCondition );
    void loadOasisCondition( QString &valExpression, KSpreadConditional &newCondition );
    void loadOasisValidationValue( const QStringList &listVal, KSpreadConditional &newCondition );
    KoFilter::ConversionStatus loadAndParse( QDomDocument& doc, const QString& fileName,KoStore *m_store );

  KoFilter::ConversionStatus openFile();
};

#endif // OpenCalc_IMPORT_H__

