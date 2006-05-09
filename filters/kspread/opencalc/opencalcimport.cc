/* This file is part of the KDE project
   Copyright (C) 2002 Norbert Andres <nandres@web.de>
   Copyright (C) 2004 - 2005  Laurent Montel <montel@kde.org>

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

#include <float.h>
#include <math.h>

#include <qcolor.h>
#include <qfile.h>
#include <qfont.h>
#include <qpen.h>
#include <qxml.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3CString>

#include "opencalcimport.h"

#include <kdebug.h>
#include <KoDocumentInfo.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>
#include <kcodecs.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoUnit.h>
#include <KoStyleStack.h>
#include <KoDom.h>
#include <ooutils.h>

#include <kspread_cell.h>
#include <kspread_condition.h>
#include <kspread_doc.h>
#include <kspread_global.h>
#include <kspread_map.h>
#include <kspread_sheet.h>
#include <kspread_sheetprint.h>
#include <kspread_style.h>
#include <kspread_style_manager.h>
#include <kspread_util.h>
#include <kspread_value.h>

#define SECSPERDAY (24 * 60 * 60)

using namespace KSpread;

class OpenCalcImportFactory : KGenericFactory<OpenCalcImport>
{
public:
    OpenCalcImportFactory(void) : KGenericFactory<OpenCalcImport> ("kspreadopencalcimport")
    {}
protected:
    virtual void setupTranslations( void )
    {
        KGlobal::locale()->insertCatalog( "kofficefilters" );
    }
};

K_EXPORT_COMPONENT_FACTORY( libopencalcimport, OpenCalcImportFactory() )

OpenCalcImport::OpenCalcPoint::OpenCalcPoint( QString const & str )
  : isRange( false )
{
  bool inQuote = false;

  int l = str.length();
  int colonPos = -1;
  QString range;

  // replace '.' with '!'
  for ( int i = 0; i < l; ++i )
  {
    if ( str[i] == '$' )
      continue;
    if ( str[i] == '\'' )
    {
      inQuote = !inQuote;
    }
    else if ( str[i] == '.' )
    {
      if ( !inQuote )
      {
        if ( i != 0 && i != (colonPos + 1) ) // no empty table names
          range += '!';
      }
      else
        range += '.';
    }
    else if ( str[i] == ':' )
    {
      if ( !inQuote )
      {
        isRange  = true;
        colonPos = i;
      }
      range += ':';
    }
    else
      range += str[i];
  }

  translation = range;

  if ( isRange )
  {
	  KSpread::Range newRange( range );
    table    = newRange.sheetName();
    topLeft  = newRange.range().topLeft();
    botRight = newRange.range().bottomRight();
  }
  else
  {
    Point newPoint( range );
    table    = newPoint.sheetName();
    topLeft  = newPoint.pos();
    botRight = newPoint.pos();
  }
}


OpenCalcImport::OpenCalcImport( QObject* parent, const QStringList & )
  : KoFilter(parent),
    m_styles( 17, true ),
    m_defaultStyles( 17, true ),
    m_formats( 17, true )
{
  m_styles.setAutoDelete( true );
  m_defaultStyles.setAutoDelete( true );
  m_formats.setAutoDelete( true );
}

OpenCalcImport::~OpenCalcImport()
{
}

double timeToNum( int h, int m, int s )
{
  int secs = h * 3600 + m * 60 + s;
  return (double) secs / (double) SECSPERDAY;
}

bool OpenCalcImport::readRowFormat( QDomElement & rowNode, QDomElement * rowStyle,
                                    Sheet * table, int & row, int & number,
                                    bool isLast )
{
  if ( rowNode.isNull() )
    return false;

  QDomNode node;
  if ( rowStyle )
  {
    node = rowStyle->firstChild();
    kDebug(30518) << "RowStyle: " << rowStyle << ", " << rowStyle->tagName() << endl;
  }

  double height = -1.0;
  bool insertPageBreak = false;
  Format layout( table, table->doc()->styleManager()->defaultStyle() );

  while( !node.isNull() )
  {
    QDomElement property = node.toElement();

    kDebug(30518) << "Row: Child exists: " << property.tagName() << endl;
    if ( !property.isNull() && property.localName() == "properties" && property.namespaceURI() == ooNS::style )
    {
      if ( property.hasAttributeNS( ooNS::style, "row-height" ) )
      {
          height = KoUnit::parseValue( property.attributeNS( ooNS::style, "row-height", QString::null ) , -1 );
      }

      if ( property.hasAttributeNS( ooNS::fo, "break-before" ) )
      {
        if ( property.attributeNS( ooNS::fo, "break-before", QString::null ) == "page" )
        {
          insertPageBreak = true;
        }
      }

      loadStyleProperties( &layout, property );
    }

    node = node.nextSibling();
  }

  if ( rowNode.hasAttributeNS( ooNS::table, "number-rows-repeated" ) )
  {
    bool ok = true;
    int n = rowNode.attributeNS( ooNS::table, "number-rows-repeated", QString::null ).toInt( &ok );
    if ( ok )
      number = n;
    kDebug(30518) << "Row repeated: " << number << endl;
  }

  if ( isLast )
  {
    if ( number > 30 )
      number = 30;
  }
  else
  {
    if ( number > 256 )
      number = 256;
  }

  for ( int i = 0; i < number; ++i )
  {
    RowFormat * rowL = table->nonDefaultRowFormat( row );
    rowL->copy( layout );

    if ( height != -1 )
    {
      kDebug(30518) << "Setting row height to " << height << endl;
      rowL->setHeight( int( height ) );
    }

    // if ( insertPageBreak ) TODO:
    //   rowL->setPageBreak( true )

    //    kDebug(30518) << "Added RowFormat: " << row << endl;
    ++row;
  }

  return true;
}

QString OpenCalcImport::translatePar( QString & par ) const
{
  OpenCalcPoint point( par );
  kDebug(30518) << "   Parameter: " << par << ", Translation: " << point.translation << endl;

  return point.translation;
}

void OpenCalcImport::checkForNamedAreas( QString & formula ) const
{
  int l = formula.length();
  int i = 0;
  QString word;
  int start = 0;
  while ( i < l )
  {
    if ( formula[i].isLetterOrNumber() )
    {
      word += formula[i];
      ++i;
      continue;
    }
    if ( word.length() > 0 )
    {
      if ( m_namedAreas.find( word ) != m_namedAreas.end() )
      {
        formula = formula.replace( start, word.length(), "'" + word + "'" );
        l = formula.length();
        ++i;
        kDebug(30518) << "Formula: " << formula << ", L: " << l << ", i: " << i + 1 <<endl;
      }
    }

    ++i;
    word = "";
    start = i;
  }
  if ( word.length() > 0 )
  {
    if ( m_namedAreas.find( word ) != m_namedAreas.end() )
    {
      formula = formula.replace( start, word.length(), "'" + word + "'" );
      l = formula.length();
      ++i;
      kDebug(30518) << "Formula: " << formula << ", L: " << l << ", i: " << i + 1 <<endl;
    }
  }
}

void OpenCalcImport::convertFormula( QString & text, QString const & f ) const
{
  kDebug(30518) << "Parsing formula: " << f << endl;

  QString formula;
  QString parameter;

  int l = f.length();
  int p = 0;

  while ( p < l )
  {
    if ( f[p] == '(' || f[p] == '[' )
    {
      break;
    }

    formula += f[p];
    ++p;
  }

  if ( parameter.isEmpty() )
  {
    checkForNamedAreas( formula );
  }

  kDebug(30518) << "Formula: " << formula << ", Parameter: " << parameter << ", P: " << p << endl;


  // replace formula names here
  if ( formula == "=MULTIPLE.OPERATIONS" )
    formula = "=MULTIPLEOPERATIONS";

  QString par;
  bool isPar   = false;
  bool inQuote = false;

  while ( p < l )
  {
    if ( f[p] == '"' )
    {
      inQuote = !inQuote;
      parameter += '"';
    }
    else if ( f[p] == '[' )
    {
      if ( !inQuote )
        isPar = true;
      else
        parameter += '[';
    }
    else if ( f[p] == ']' )
    {
      if ( inQuote )
      {
        parameter += ']';
        continue;
      }

      isPar = false;
      parameter += translatePar( par );
      par = "";
    }
    else if ( isPar )
    {
      par += f[p];
    }
    else if ( f[p] == '=' ) // TODO: check if StarCalc has a '==' sometimes
    {
      if ( inQuote )
        parameter += '=';
      else
        parameter += "==";
    }
    else if ( f[p] == ')' )
    {
      if ( !inQuote )
        parameter += ")";
    }
    else
      parameter += f[p];

    ++p;
    if ( p == l )
      checkForNamedAreas( parameter );
  }

  text = formula + parameter;
  kDebug(30518) << "New formula: " << text << endl;
}

bool OpenCalcImport::readCells( QDomElement & rowNode, Sheet  * table, int row, int & columns )
{
  bool ok = true;
  int spanC = 1;
  int spanR = 1;
  //Cell* defCell = table->defaultCell();

  QDomNode cellNode = KoDom::namedItemNS( rowNode, ooNS::table, "table-cell" );

  while ( !cellNode.isNull() )
  {
    spanR = 1; spanC = 1;

    QDomElement e = cellNode.toElement();
    if ( e.isNull() )
    {
      ++columns;

      cellNode = cellNode.nextSibling();
      continue;
    }

    Cell* cell = 0;

    kDebug(30518) << " Cell: " << columns << ", " << row << endl;

    // ="3" table:number-rows-spanned="1"
    if ( e.hasAttributeNS( ooNS::table, "number-columns-spanned" ) )
    {
      int span = e.attributeNS( ooNS::table, "number-columns-spanned", QString::null ).toInt( &ok );
      if ( ok )
        spanC = span;
    }
    if ( e.hasAttributeNS( ooNS::table, "number-rows-spanned" ) )
    {
      int span = e.attributeNS( ooNS::table, "number-rows-spanned", QString::null ).toInt( &ok );
      if ( ok )
        spanR = span;
    }

    QString text;
    QDomElement textP = KoDom::namedItemNS( e, ooNS::text, "p" );
    if ( !textP.isNull() )
    {
      QDomElement subText = textP.firstChild().toElement(); // ## wrong
      if ( !subText.isNull() )
      {
        // something in <text:p>, e.g. links
        text = subText.text();

        if ( subText.hasAttributeNS( ooNS::xlink, "href" ) )
        {
          QString link = subText.attributeNS( ooNS::xlink, "href", QString::null );
          if ( link[0]=='#' )
              link=link.remove( 0, 1 );
          if ( !cell )
              cell = table->nonDefaultCell( columns, row );
          cell->setLink( link );
        }
      }
      else
        text = textP.text(); // our text, could contain formating for value or result of formula
    }
    QDomElement annotation = KoDom::namedItemNS( e, ooNS::office, "annotation" );
    if ( !annotation.isNull() )
    {
       QString comment;
        QDomNode node = annotation.firstChild();
        while( !node.isNull() )
        {
            QDomElement commentElement = node.toElement();
            if( !commentElement.isNull() )
                if ( commentElement.localName() == "p" && e.namespaceURI()==ooNS::text)
                {
                    if( !comment.isEmpty() ) comment.append( '\n' );
                    comment.append( commentElement.text() );
                }

            node = node.nextSibling();
        }

        if( !comment.isEmpty() )
        {
            if ( !cell )
                cell = table->nonDefaultCell( columns, row );
            kDebug(30518)<<" columns :"<<columns<<" row :"<<row<<endl;
            cell->format()->setComment( comment );
        }
    }

    kDebug(30518) << "Contains: " << text << endl;
    bool isFormula = false;

    if ( e.hasAttributeNS( ooNS::table, "style-name" ) )
    {
      if ( !cell )
        cell = table->nonDefaultCell( columns, row );

      QString psName( "Default" );
      if ( e.hasAttributeNS( ooNS::style, "parent-style-name" ) )
        psName = e.attributeNS( ooNS::style, "parent-style-name", QString::null );

      kDebug(30518) << "Default style: " << psName << endl;
      Format * layout = m_defaultStyles[psName];

      if ( layout )
        cell->format()->copy( *layout );

      QDomElement * st = 0;
      if ( e.hasAttributeNS( ooNS::table, "style-name" ) )
      {
        kDebug(30518) << "Style: " << e.attributeNS( ooNS::table, "style-name", QString::null ) << endl;
        st = m_styles[ e.attributeNS( ooNS::table, "style-name", QString::null ) ];
      }
      if ( st )
      {
        kDebug(30518) << "Style: adapting " << endl;
        QDomNode node = st->firstChild();
        bool foundValidation = false;
        while( !node.isNull() )
        {
          QDomElement property = node.toElement();
          if ( !property.isNull() )
          {
            kDebug(30518)<<"property.tagName() :"<<property.tagName()<<endl;
            if ( property.localName()=="map" && property.namespaceURI() == ooNS::style && !foundValidation)
            {
              loadCondition( cell, property );
              foundValidation = true;
            }
            if ( property.localName() == "properties" && property.namespaceURI() == ooNS::style )
            {
              loadStyleProperties( cell->format(), property );
              if ( cell->format()->getAngle( columns, row ) != 0 )
              {
                QFontMetrics fm( cell->format()->textFont( columns, row ) );
                int tmpAngle = cell->format()->getAngle( columns, row );
                int textHeight = static_cast<int>( cos( tmpAngle * M_PI / 180 )
                                                   * ( fm.ascent() + fm.descent() )
                                                   + abs ( ( int )(  fm.width( cell->strOutText() )
                                                                     * sin( tmpAngle * M_PI / 180 )  ) ) );
                /*
                  int textWidth = static_cast<int>( abs ( ( int ) ( sin( tmpAngle * M_PI / 180 )
                  * ( fm.ascent() + fm.descent() ) ) )
                  + fm.width( cell->strOutText() )
                  * cos( tmpAngle * M_PI / 180 ) );
                  */
                kDebug(30518) << "Rotation: height: " << textHeight << endl;

                RowFormat * l = table->rowFormat( row );
                if ( l->height() < textHeight )
                {
                  if ( l->isDefault() )
                    l = table->nonDefaultRowFormat( row );

                  l->setHeight( textHeight + 2 );
                }
              }
            }
          }
          node = node.nextSibling();
        }
      }
    }
    else
    {
      if ( !cell )
        cell = table->nonDefaultCell( columns, row );

      QString psName( "Default" );
      kDebug(30518) << "Default style: " << psName << endl;
      Format * layout = m_defaultStyles[psName];

      if ( layout )
        cell->format()->copy( *layout );
    }
    if ( e.hasAttributeNS( ooNS::table, "formula" ) )
    {
      isFormula = true;
      QString formula;
      convertFormula( formula, e.attributeNS( ooNS::table, "formula", QString::null ) );

      if ( !cell )
        cell = table->nonDefaultCell( columns, row );
      cell->setCellText( formula );
    }
    if ( e.hasAttributeNS( ooNS::table, "validation-name" ) )
    {
        kDebug(30518)<<" Celle has a validation :"<<e.attributeNS( ooNS::table, "validation-name", QString::null )<<endl;
        loadOasisValidation( cell->getValidity(), e.attributeNS( ooNS::table, "validation-name", QString::null ) );
    }
    if ( e.hasAttributeNS( ooNS::table, "value-type" ) )
    {
      if ( !cell )
        cell = table->nonDefaultCell( columns, row );

      cell->setCellText( text );

      QString value = e.attributeNS( ooNS::table, "value", QString::null );
      QString type  = e.attributeNS( ooNS::table, "value-type", QString::null );

      kDebug(30518) << "Value: " << value << ", type: " << type << endl;

      bool ok = false;
      double dv = 0.0;

      if ( ( type == "float" ) || ( type == "currency" ) )
      {
        dv = value.toDouble( &ok );
        if ( ok )
        {
          if ( !isFormula )
            cell->setValue( dv );

          if ( type == "currency" )
          {
            cell->format()->setCurrency( 1, e.attributeNS( ooNS::table, "currency", QString::null ) );
            cell->format()->setFormatType( Money_format );
          }
        }
      }
      else
        if ( type == "percentage" )
        {
          dv = value.toDouble( &ok );
          if ( ok )
          {
            if ( !isFormula )
              cell->setValue( dv );
            //TODO fixme
			//cell->setFactor( 100 );
            // TODO: replace with custom...
            cell->format()->setFormatType( Percentage_format );
          }
        }
        else if ( type == "boolean" )
        {
          if ( value.isEmpty() )
            value = e.attributeNS( ooNS::table, "boolean-value", QString::null );

          kDebug(30518) << "Type: boolean" << endl;
          if ( value == "true" )
            cell->setValue( true );
          else
            cell->setValue( false );
          ok = true;
          cell->format()->setFormatType( Custom_format );
        }
        else if ( type == "date" )
        {
          if ( value.isEmpty() )
            value = e.attributeNS( ooNS::table, "date-value", QString::null );
          kDebug(30518) << "Type: date, value: " << value << endl;

          // "1980-10-15"
          int year=0, month=0, day=0;
          ok = false;

          int p1 = value.find( '-' );
          if ( p1 > 0 )
            year  = value.left( p1 ).toInt( &ok );

          kDebug(30518) << "year: " << value.left( p1 ) << endl;

          int p2 = value.find( '-', ++p1 );

          if ( ok )
            month = value.mid( p1, p2 - p1  ).toInt( &ok );

          kDebug(30518) << "month: " << value.mid( p1, p2 - p1 ) << endl;

          if ( ok )
            day = value.right( value.length() - p2 - 1 ).toInt( &ok );

          kDebug(30518) << "day: " << value.right( value.length() - p2 ) << endl;

          if ( ok )
          {
            QDateTime dt( QDate( year, month, day ) );
            //            KSpreadValue kval( dt );
            // cell->setValue( kval );
            cell->setValue( QDate( year, month, day ) );
            kDebug(30518) << "Set QDate: " << year << " - " << month << " - " << day << endl;
          }
        }
        else if ( type == "time" )
        {
          if ( value.isEmpty() )
            value = e.attributeNS( ooNS::table, "time-value", QString::null );

          kDebug(30518) << "Type: time: " << value << endl;
          // "PT15H10M12S"
          int hours=0, minutes=0, seconds=0;
          int l = value.length();
          QString num;

          for ( int i = 0; i < l; ++i )
          {
            if ( value[i].isNumber() )
            {
              num += value[i];
              continue;
            }
            else if ( value[i] == 'H' )
              hours   = num.toInt( &ok );
            else if ( value[i] == 'M' )
              minutes = num.toInt( &ok );
            else if ( value[i] == 'S' )
              seconds = num.toInt( &ok );
            else
              continue;

            kDebug(30518) << "Num: " << num << endl;

            num = "";
            if ( !ok )
              break;
          }

          kDebug(30518) << "Hours: " << hours << ", " << minutes << ", " << seconds << endl;

          if ( ok )
          {
            // KSpreadValue kval( timeToNum( hours, minutes, seconds ) );
            // cell->setValue( kval );
            cell->setValue( QTime( hours % 24, minutes, seconds ) );
            cell->format()->setFormatType( Custom_format );
          }
        }


      if ( !ok ) // just in case we couldn't set the value directly
        cell->setCellText( text );
    }
    else if ( !text.isEmpty() )
    {
      if ( !cell )
        cell = table->nonDefaultCell( columns, row );
      cell->setCellText( text );
    }

    if ( spanR > 1 || spanC > 1 )
    {
      if ( !cell )
        cell = table->nonDefaultCell( columns, row );
      cell->mergeCells( columns, row, spanC - 1, spanR - 1 );
    }

    cellNode = cellNode.nextSibling();

    if ( e.hasAttributeNS( ooNS::table, "number-columns-repeated" ) )
    {
      // copy cell from left
      bool ok = false;
      int number = e.attributeNS( ooNS::table, "number-columns-repeated", QString::null ).toInt( &ok );
      Cell* cellDest = 0;

      // don't repeat more than 10 if it is the last cell and empty
      if ( !ok || cellNode.isNull() )
      {
        if ( number > 10 )
          number = 10;
      }

      for ( int i = 1; i < number; ++i )
      {
        ++columns;

        if ( cell )
        {
          cellDest = table->nonDefaultCell( columns, row );
          cellDest->copyAll( cell );
        }
      }
    }

    ++columns;
  }

  return true;
}


void OpenCalcImport::loadCondition( Cell*cell,const QDomElement &property )
{
    kDebug(30518)<<"void OpenCalcImport::loadCondition( Cell*cell,const QDomElement &property )*******\n";
    loadOasisCondition( cell, property );
}

void OpenCalcImport::loadOasisCondition(Cell*cell,const QDomElement &property )
{
    QDomElement elementItem( property );
    StyleManager * manager = cell->sheet()->doc()->styleManager();

    Q3ValueList<Conditional> cond;
    while ( !elementItem.isNull() )
    {
        kDebug(30518)<<"elementItem.tagName() :"<<elementItem.tagName()<<endl;

        if ( elementItem.localName()== "map" && property.namespaceURI() == ooNS::style )
        {
            bool ok = true;
            kDebug(30518)<<"elementItem.attribute(style:condition ) :"<<elementItem.attributeNS( ooNS::style, "condition", QString::null )<<endl;
            Conditional newCondition;
            loadOasisConditionValue( elementItem.attributeNS( ooNS::style, "condition", QString::null ), newCondition );
            if ( elementItem.hasAttributeNS( ooNS::style, "apply-style-name" ) )
            {
                kDebug(30518)<<"elementItem.attribute( style:apply-style-name ) :"<<elementItem.attributeNS( ooNS::style, "apply-style-name", QString::null )<<endl;
                newCondition.styleName = new QString( elementItem.attributeNS( ooNS::style, "apply-style-name", QString::null ) );
                newCondition.style = manager->style( *newCondition.styleName );
                if ( !newCondition.style )
                    ok = false;
                else
                    ok = true;
            }

            if ( ok )
                cond.append( newCondition );
            else
                kDebug(30518) << "Error loading condition " << elementItem.nodeName()<< endl;
        }
        elementItem = elementItem.nextSibling().toElement();
    }
    if ( !cond.isEmpty() )
        cell->setConditionList( cond );
}

void OpenCalcImport::loadOasisConditionValue( const QString &styleCondition, Conditional &newCondition )
{
    QString val( styleCondition );
    if ( val.contains( "cell-content()" ) )
    {
        val = val.remove( "cell-content()" );
        loadOasisCondition( val,newCondition );
    }
    //GetFunction ::= cell-content-is-between(Value, Value) | cell-content-is-not-between(Value, Value)
    //for the moment we support just int/double value, not text/date/time :(
    if ( val.contains( "cell-content-is-between(" ) )
    {
        val = val.remove( "cell-content-is-between(" );
        val = val.remove( ")" );
        QStringList listVal = QStringList::split( "," , val );
        loadOasisValidationValue( listVal, newCondition );
        newCondition.cond = Conditional::Between;
    }
    if ( val.contains( "cell-content-is-not-between(" ) )
    {
        val = val.remove( "cell-content-is-not-between(" );
        val = val.remove( ")" );
        QStringList listVal = QStringList::split( ",", val );
        loadOasisValidationValue( listVal,newCondition );
        newCondition.cond = Conditional::Different;
    }

}


void OpenCalcImport::loadOasisCondition( QString &valExpression, Conditional &newCondition )
{
    QString value;
    if (valExpression.find( "<=" )==0 )
    {
        value = valExpression.remove( 0,2 );
        newCondition.cond = Conditional::InferiorEqual;
    }
    else if (valExpression.find( ">=" )==0 )
    {
        value = valExpression.remove( 0,2 );
        newCondition.cond = Conditional::SuperiorEqual;
    }
    else if (valExpression.find( "!=" )==0 )
    {
        //add Differentto attribute
        value = valExpression.remove( 0,2 );
        newCondition.cond = Conditional::DifferentTo;
    }
    else if ( valExpression.find( "<" )==0 )
    {
        value = valExpression.remove( 0,1 );
        newCondition.cond = Conditional::Inferior;
    }
    else if(valExpression.find( ">" )==0 )
    {
        value = valExpression.remove( 0,1 );
        newCondition.cond = Conditional::Superior;
    }
    else if (valExpression.find( "=" )==0 )
    {
        value = valExpression.remove( 0,1 );
        newCondition.cond = Conditional::Equal;
    }
    else
        kDebug(30518)<<" I don't know how to parse it :"<<valExpression<<endl;
    kDebug(30518)<<" value :"<<value<<endl;
    bool ok = false;
    newCondition.val1 = value.toDouble(&ok);
    if ( !ok )
    {
        newCondition.val1 = value.toInt(&ok);
        if ( !ok )
        {
            newCondition.strVal1 = new QString( value );
            kDebug(30518)<<" Try to parse this value :"<<value<<endl;
        }

    }
}


void OpenCalcImport::loadOasisValidationValue( const QStringList &listVal, Conditional &newCondition )
{
    bool ok = false;
    kDebug(30518)<<" listVal[0] :"<<listVal[0]<<" listVal[1] :"<<listVal[1]<<endl;

    newCondition.val1 = listVal[0].toDouble(&ok);
    if ( !ok )
    {
        newCondition.val1 = listVal[0].toInt(&ok);
        if ( !ok )
        {
            newCondition.strVal1 = new QString( listVal[0] );
            kDebug(30518)<<" Try to parse this value :"<<listVal[0]<<endl;
        }
    }
    ok=false;
    newCondition.val2 = listVal[1].toDouble(&ok);
    if ( !ok )
    {
        newCondition.val2 = listVal[1].toInt(&ok);
        if ( !ok )
        {
            newCondition.strVal2 = new QString( listVal[1] );
            kDebug(30518)<<" Try to parse this value :"<<listVal[1]<<endl;
        }
    }
}


bool OpenCalcImport::readRowsAndCells( QDomElement & content, Sheet * table )
{
  kDebug(30518) << endl << "Reading in rows " << endl;

  int i   = 1;
  int row = 1;
  int columns = 1;
  int backupRow = 1;
  QDomElement * rowStyle = 0;
  //Cell* cell = 0;
  //Cell* cellDest = 0;
  //Cell* defCell = table->defaultCell();
  QDomNode rowNode = KoDom::namedItemNS( content, ooNS::table, "table-row" );

  while ( !rowNode.isNull() )
  {
    bool collapsed = false;

    int number = 1;
    QDomElement r = rowNode.toElement();

    if ( r.isNull() )
      return false;

    if ( r.hasAttributeNS( ooNS::table, "style-name" ) )
    {
      QString style = r.attributeNS( ooNS::table, "style-name", QString::null );
      rowStyle = m_styles[ style ];
      kDebug(30518) << "Row style: " << style << endl;
    }

    collapsed = ( r.attributeNS( ooNS::table, "visibility", QString::null ) == "collapse" );

    backupRow = row;

    rowNode = rowNode.nextSibling();

    if ( !readRowFormat( r, rowStyle, table, row, number, rowNode.isNull() ) ) // updates "row"
      return false;

    if ( !readCells( r, table, backupRow, columns ) )
      return false;

    RowFormat * srcLayout = table->nonDefaultRowFormat( backupRow );
    RowFormat * layout = 0;

    if ( collapsed )
      srcLayout->setHide( true );

    for ( i = 1; i < number; ++i )
    {
      layout = table->nonDefaultRowFormat( backupRow + i );

      layout->copy( *srcLayout );

      /*
       * TODO: Test: do we need to copy the cells, too?
       *       if so we will probably also need to copy them for repeated col layouts.
      for ( j = 1; j <= columns; ++j )
      {
        Cell* cell = table->cellAt( j, backupRow );

        kDebug(30518) << "Cell: " << cell << "DefCell: " << defCell << endl;
        if ( cell && (cell != defCell) )
        {
          cellDest = table->nonDefaultCell( j, backupRow + i );
          cellDest->copyAll( cell );
        }
      }
      */
    }

    rowStyle = 0;
    columns = 1;
  }

  kDebug(30518) << "Reading in rows done" << endl << endl;

  return true;
}

bool OpenCalcImport::readColLayouts( QDomElement & content, Sheet * table )
{
  kDebug(30518) << endl << "Reading in columns..." << endl;

  QDomNode colLayout = KoDom::namedItemNS( content, ooNS::table, "table-column" );
  int column = 1;

  while ( !colLayout.isNull() )
  {
    if ( colLayout.nodeName() != "table:table-column" )
      return true; // all cols read in.

    QDomElement e = colLayout.toElement();

    if ( e.isNull() )
      return false; // error, that's it...

    kDebug(30518) << "New column: " << column << endl;

    int number     = 1;
    double width   = colWidth;
    bool collapsed = ( e.attributeNS( ooNS::table, "visibility", QString::null ) == "collapse" );
    bool insertPageBreak = false;
    Format styleLayout( table, table->doc()->styleManager()->defaultStyle() );

    kDebug(30518) << "Check table:number-columns-repeated" << endl;
    if ( e.hasAttributeNS( ooNS::table, "number-columns-repeated" ) )
    {
      bool ok = true;
      number = e.attributeNS( ooNS::table, "number-columns-repeated", QString::null ).toInt( &ok );
      if ( !ok )
        number = 1;

      kDebug(30518) << "Repeated: " << number << endl;
    }

    kDebug(30518) << "Checking table:default-cell-style-name" << endl;
    if ( e.hasAttributeNS( ooNS::table, "default-cell-style-name" ) )
    {
      QString n( e.attributeNS( ooNS::table, "default-cell-style-name", QString::null ) );
      kDebug(30518) << "Has attribute default-cell-style-name: " << n << endl;
      Format * defaultStyle = m_defaultStyles[ n ];
      if ( !defaultStyle )
      {
        QString name = e.attributeNS( ooNS::table, "default-cell-style-name", QString::null );
        QDomElement * st = m_styles[ name ];

        kDebug(30518) << "Default cell style: " << name << endl;

        if ( st && !st->isNull() )
        {
          Format * layout = new Format( 0, m_doc->styleManager()->defaultStyle() );

          readInStyle( layout, *st );

          m_defaultStyles.insert( name, layout );
          kDebug(30518) << "Insert default cell style: " << name << endl;

          defaultStyle = layout;
        }
      }

      if ( defaultStyle )
      {
        //        kDebug(30518) << "Copying default style, Font: " << defaultStyle->font().toString() << endl;
        styleLayout.copy( *defaultStyle );
      }
    }

    QDomElement * colStyle = 0;
    if ( e.hasAttributeNS( ooNS::table, "style-name" ) )
    {
      QString style = e.attributeNS( ooNS::table, "style-name", QString::null );
      colStyle = m_styles[ style ];

      kDebug(30518) << "Col Style: " << style << endl;
    }

    QDomNode node;

    if ( colStyle )
      node = colStyle->firstChild();

    while( !node.isNull() )
    {
      QDomElement property = node.toElement();
      if ( !property.isNull() && property.localName() == "properties" && property.namespaceURI() == ooNS::style )
      {
        if ( property.hasAttributeNS( ooNS::style, "column-width" ) )
        {
          QString sWidth = property.attributeNS( ooNS::style, "column-width", QString::null );
          width = KoUnit::parseValue( property.attributeNS( ooNS::style, "column-width", QString::null ), width );
          kDebug(30518) << "Col Width: " << sWidth << endl;
        }

        if ( property.hasAttributeNS( ooNS::fo, "break-before" ) )
        {
          if ( property.attributeNS( ooNS::fo, "break-before", QString::null ) == "page" )
          {
            insertPageBreak = true;
          }
        }

        loadStyleProperties( &styleLayout, property );
      }

      node = node.nextSibling();
    }

    colLayout = colLayout.nextSibling();

    if ( colLayout.isNull() && ( number > 30 ) )
      number = 30;

    for ( int i = 0; i < number; ++i )
    {
      kDebug(30518) << "Inserting colLayout: " << column << endl;

      ColumnFormat * col = new ColumnFormat( table, column );
      col->copy( styleLayout );
      col->setWidth( int( width ) );

      // if ( insertPageBreak )
      //   col->setPageBreak( true )

      if ( collapsed )
        col->setHide( true );

      table->insertColumnFormat( col );
      ++column;
    }
  }

  return true;
}

void replaceMacro( QString & text, QString const & old, QString const & newS )
{
  int n = text.find( old );
  if ( n != -1 )
    text = text.replace( n, old.length(), newS );
}

QString getPart( QDomNode const & part )
{
  QString result;
  QDomElement e = KoDom::namedItemNS( part, ooNS::text, "p" );
  while ( !e.isNull() )
  {
    QString text = e.text();
    kDebug(30518) << "PART: " << text << endl;

    QDomElement macro = KoDom::namedItemNS( e, ooNS::text, "time" );
    if ( !macro.isNull() )
      replaceMacro( text, macro.text(), "<time>" );

    macro = KoDom::namedItemNS( e, ooNS::text, "date" );
    if ( !macro.isNull() )
      replaceMacro( text, macro.text(), "<date>" );

    macro = KoDom::namedItemNS( e, ooNS::text, "page-number" );
    if ( !macro.isNull() )
      replaceMacro( text, macro.text(), "<page>" );

    macro = KoDom::namedItemNS( e, ooNS::text, "page-count" );
    if ( !macro.isNull() )
      replaceMacro( text, macro.text(), "<pages>" );

    macro = KoDom::namedItemNS( e, ooNS::text, "sheet-name" );
    if ( !macro.isNull() )
      replaceMacro( text, macro.text(), "<sheet>" );

    macro = KoDom::namedItemNS( e, ooNS::text, "title" );
    if ( !macro.isNull() )
      replaceMacro( text, macro.text(), "<name>" );

    macro = KoDom::namedItemNS( e, ooNS::text, "file-name" );
    if ( !macro.isNull() )
      replaceMacro( text, macro.text(), "<file>" );

    if ( !result.isEmpty() )
      result += '\n';
    result += text;
    e = e.nextSibling().toElement();
  }

  return result;
}

void OpenCalcImport::loadTableMasterStyle( Sheet * table,
                                           QString const & stylename )
{
  kDebug(30518) << "Loading table master style: " << stylename << endl;

  QDomElement * style = m_styles[ stylename ];

  if ( !style )
  {
    kDebug(30518) << "Master style not found! " << endl;
    return;
  }

  QDomNode header = KoDom::namedItemNS( *style, ooNS::style, "header" );
  kDebug(30518) << "Style header " << endl;

  QString hleft, hmiddle, hright;
  QString fleft, fmiddle, fright;

  if ( !header.isNull() )
  {
    kDebug(30518) << "Header exists" << endl;
    QDomNode part = KoDom::namedItemNS( header, ooNS::style, "region-left" );
    if ( !part.isNull() )
    {
      hleft = getPart( part );
      kDebug(30518) << "Header left: " << hleft << endl;
    }
    else
      kDebug(30518) << "Style:region:left doesn't exist!" << endl;
    part = KoDom::namedItemNS( header, ooNS::style, "region-center" );
    if ( !part.isNull() )
    {
      hmiddle = getPart( part );
      kDebug(30518) << "Header middle: " << hmiddle << endl;
    }
    part = KoDom::namedItemNS( header, ooNS::style, "region-right" );
    if ( !part.isNull() )
    {
      hright = getPart( part );
      kDebug(30518) << "Header right: " << hright << endl;
    }
  }

  QDomNode footer = KoDom::namedItemNS( *style, ooNS::style, "footer" );

  if ( !footer.isNull() )
  {
    QDomNode part = KoDom::namedItemNS( footer, ooNS::style, "region-left" );
    if ( !part.isNull() )
    {
      fleft = getPart( part );
      kDebug(30518) << "Footer left: " << fleft << endl;
    }
    part = KoDom::namedItemNS( footer, ooNS::style, "region-center" );
    if ( !part.isNull() )
    {
      fmiddle = getPart( part );
      kDebug(30518) << "Footer middle: " << fmiddle << endl;
    }
    part = KoDom::namedItemNS( footer, ooNS::style, "region-right" );
    if ( !part.isNull() )
    {
      fright = getPart( part );
      kDebug(30518) << "Footer right: " << fright << endl;
    }
  }

  table->print()->setHeadFootLine( hleft, hmiddle, hright,
                                   fleft, fmiddle, fright );
  if ( style->hasAttributeNS( ooNS::style, "page-master-name" ) )
  {
      QString masterPageLayoutStyleName=style->attributeNS( ooNS::style, "page-master-name", QString::null );
      kDebug(30518)<<"masterPageLayoutStyleName :"<<masterPageLayoutStyleName<<endl;
      QDomElement *masterLayoutStyle = m_styles[masterPageLayoutStyleName];
      kDebug(30518)<<"masterLayoutStyle :"<<masterLayoutStyle<<endl;
      if ( !masterLayoutStyle )
          return;
      KoStyleStack styleStack( ooNS::style, ooNS::fo );
      styleStack.push( *masterLayoutStyle );
      loadOasisMasterLayoutPage( table, styleStack );
  }
}

void OpenCalcImport::loadOasisMasterLayoutPage( Sheet * table,KoStyleStack &styleStack )
{
    float left = 0.0;
    float right = 0.0;
    float top = 0.0;
    float bottom = 0.0;
    float width = 0.0;
    float height = 0.0;
    QString orientation = "Portrait";
    QString format;

    // Laurent : Why we stored layout information as Millimeter ?!!!!!
    // kspread used point for all other attribute
    // I don't understand :(
    if ( styleStack.hasAttributeNS( ooNS::fo, "page-width" ) )
    {
        width = KoUnit::toMM(KoUnit::parseValue( styleStack.attributeNS( ooNS::fo, "page-width" ) ) );
    }
    if ( styleStack.hasAttributeNS( ooNS::fo, "page-height" ) )
    {
        height = KoUnit::toMM( KoUnit::parseValue( styleStack.attributeNS( ooNS::fo, "page-height" ) ) );
    }
    if ( styleStack.hasAttributeNS( ooNS::fo, "margin-top" ) )
    {
        top = KoUnit::toMM(KoUnit::parseValue( styleStack.attributeNS( ooNS::fo, "margin-top" ) ) );
    }
    if ( styleStack.hasAttributeNS( ooNS::fo, "margin-bottom" ) )
    {
        bottom = KoUnit::toMM(KoUnit::parseValue( styleStack.attributeNS( ooNS::fo, "margin-bottom" ) ) );
    }
    if ( styleStack.hasAttributeNS( ooNS::fo, "margin-left" ) )
    {
        left = KoUnit::toMM(KoUnit::parseValue( styleStack.attributeNS( ooNS::fo, "margin-left" ) ) );
    }
    if ( styleStack.hasAttributeNS( ooNS::fo, "margin-right" ) )
    {
        right = KoUnit::toMM(KoUnit::parseValue( styleStack.attributeNS( ooNS::fo, "margin-right" ) ) );
    }
    if ( styleStack.hasAttributeNS( ooNS::style, "writing-mode" ) )
    {
        kDebug(30518)<<"styleStack.hasAttribute( style:writing-mode ) :"<<styleStack.hasAttributeNS( ooNS::style, "writing-mode" )<<endl;
    }
    if ( styleStack.hasAttributeNS( ooNS::style, "print-orientation" ) )
    {
        orientation = ( styleStack.attributeNS( ooNS::style, "print-orientation" )=="landscape" ) ? "Landscape" : "Portrait" ;
    }
    if ( styleStack.hasAttributeNS( ooNS::style, "num-format" ) )
    {
        kDebug(30518)<<" num-format :"<<styleStack.attributeNS( ooNS::style, "num-format" )<<endl;
        //todo fixme
    }
        if ( styleStack.hasAttributeNS( ooNS::fo, "background-color" ) )
    {
        //todo
        kDebug(30518)<<" fo:background-color :"<<styleStack.attributeNS( ooNS::fo, "background-color" )<<endl;
    }
    if ( styleStack.hasAttributeNS( ooNS::style, "print" ) )
    {
        //todo parsing
        QString str = styleStack.attributeNS( ooNS::style, "print" );
        kDebug(30518)<<" style:print :"<<str<<endl;

        if (str.contains( "headers" ) )
        {
            //todo implement it into kspread
        }
        if ( str.contains( "grid" ) )
        {
            table->print()->setPrintGrid( true );
        }
        if ( str.contains( "annotations" ) )
        {
            //todo it's not implemented
        }
        if ( str.contains( "objects" ) )
        {
            //todo it's not implemented
        }
        if ( str.contains( "charts" ) )
        {
            //todo it's not implemented
        }
        if ( str.contains( "drawings" ) )
        {
            //todo it's not implemented
        }
        if ( str.contains( "formulas" ) )
        {
            table->setShowFormula(true);
        }
        if ( str.contains( "zero-values" ) )
        {
            //todo it's not implemented
        }
    }
    if ( styleStack.hasAttributeNS( ooNS::style, "table-centering" ) )
    {
        QString str = styleStack.attributeNS( ooNS::style, "table-centering" );
        //not implemented into kspread
        kDebug(30518)<<" styleStack.attribute( style:table-centering ) :"<<str<<endl;
#if 0
        if ( str == "horizontal" )
        {
        }
        else if ( str == "vertical" )
        {
        }
        else if ( str == "both" )
        {
        }
        else if ( str == "none" )
        {
        }
        else
            kDebug(30518)<<" table-centering unknown :"<<str<<endl;
#endif
    }
    format = QString( "%1x%2" ).arg( width ).arg( height );
    kDebug(30518)<<" format : "<<format<<endl;
    table->print()->setPaperLayout( left, top, right, bottom, format, orientation );

    kDebug(30518)<<" left margin :"<<left<<" right :"<<right<<" top :"<<top<<" bottom :"<<bottom<<endl;
//<style:properties fo:page-width="21.8cm" fo:page-height="28.801cm" fo:margin-top="2cm" fo:margin-bottom="2.799cm" fo:margin-left="1.3cm" fo:margin-right="1.3cm" style:writing-mode="lr-tb"/>
//          QString format = paper.attribute( "format" );
//      QString orientation = paper.attribute( "orientation" );
//        m_pPrint->setPaperLayout( left, top, right, bottom, format, orientation );
//      }
}


bool OpenCalcImport::parseBody( int numOfTables )
{
  QDomElement content = m_content.documentElement();
  QDomNode body = KoDom::namedItemNS( content, ooNS::office, "body" );

  if ( body.isNull() )
    return false;

  loadOasisAreaName( body.toElement() );
  loadOasisCellValidation( body.toElement() );

  Sheet * table;
  QDomNode sheet = KoDom::namedItemNS( body, ooNS::table, "table" );

  kDebug()<<" sheet :"<<sheet.isNull()<<endl;
  if ( sheet.isNull() )
    return false;

  while ( !sheet.isNull() )
  {
    QDomElement t = sheet.toElement();
    if ( t.isNull() )
    {
      sheet = sheet.nextSibling();
      continue;
    }
    if ( t.nodeName() != "table:table" )
    {
      sheet = sheet.nextSibling();
      continue;
    }

    table = m_doc->map()->addNewSheet();

    table->setSheetName( t.attributeNS( ooNS::table, "name", QString::null ), true, false );
    kDebug()<<" table->name()"<<table->name()<<endl;
    sheet = sheet.nextSibling();
  }

  sheet = body.firstChild();

  int step = (int) ( 80 / numOfTables );
  int progress = 15;

  Format::setGlobalColWidth( MM_TO_POINT( 22.7 ) );
  Format::setGlobalRowHeight( MM_TO_POINT( 4.3 ) );
  kDebug(30518) << "Global Height: " << MM_TO_POINT( 4.3 ) << ", Global width: " << MM_TO_POINT( 22.7) << endl;

  while ( !sheet.isNull() )
  {
    QDomElement t = sheet.toElement();
    if ( t.isNull() )
    {
      KMessageBox::sorry( 0, i18n( "The file seems to be corrupt. Skipping a table." ) );
      sheet = sheet.nextSibling();
      continue;
    }
    if ( t.nodeName() != "table:table" )
    {
      sheet = sheet.nextSibling();
      continue;
    }

    table = m_doc->map()->findSheet( t.attributeNS( ooNS::table, "name", QString::null ) );
    if ( !table )
    {
      KMessageBox::sorry( 0, i18n( "Skipping a table." ) );
      sheet = sheet.nextSibling();
      continue;
    }

    Format * defaultStyle = m_defaultStyles[ "Default" ];
    if ( defaultStyle )
    {
      Cell* defaultCell = table->defaultCell();
      kDebug(30518) << "Copy default style to default cell" << endl;
      defaultCell->format()->copy( *defaultStyle );
    }
    table->setDefaultHeight( MM_TO_POINT( 4.3 ) );
    table->setDefaultWidth( MM_TO_POINT( 22.7 ) );

    kDebug(30518) << "Added table: " << t.attributeNS( ooNS::table, "name", QString::null ) << endl;

    if ( t.hasAttributeNS( ooNS::table, "style-name" ) )
    {
      QString style = t.attributeNS( ooNS::table, "style-name", QString::null );
      QDomElement * tableStyle = m_styles[ style ];

      QDomNode node;

      if ( tableStyle )
        node = tableStyle->firstChild();

      while( !node.isNull() )
      {
        QDomElement property = node.toElement();
        if ( property.localName() == "properties" && property.namespaceURI() == ooNS::style )
        {
          if ( property.hasAttributeNS( ooNS::table, "display" ) )
          {
            bool visible = (property.attributeNS( ooNS::table, "display", QString::null ) == "true" ? true : false );
            table->hideSheet( !visible );
            kDebug(30518) << "Table: " << table->tableName() << ", hidden: " << !visible << endl;
          }
        }

        node = node.nextSibling();
      }

      if ( tableStyle && tableStyle->hasAttributeNS( ooNS::style, "master-page-name" ) )
      {
        QString stylename = "pm" + tableStyle->attributeNS( ooNS::style, "master-page-name", QString::null );

        loadTableMasterStyle( table, stylename );

      }
    }
    if ( t.hasAttributeNS( ooNS::table, "print-ranges" ) )
    {
      // e.g.: Sheet4.A1:Sheet4.E28
      QString range = t.attributeNS( ooNS::table, "print-ranges", QString::null );
      OpenCalcPoint point( range );

      kDebug(30518) << "Print range: " << point.translation << endl;
      KSpread::Range p( point.translation );

      kDebug(30518) << "Print table: " << p.sheetName() << endl;

      if ( table->sheetName() == p.sheetName() )
        table->print()->setPrintRange( p.range() );
    }

    if ( !readColLayouts( t, table ) )
      return false;

    if ( !readRowsAndCells( t, table ) )
      return false;

    if ( t.hasAttributeNS( ooNS::table, "protected" ) )
    {
      Q3CString passwd( "" );
      if ( t.hasAttributeNS( ooNS::table, "protection-key" ) )
      {
        QString p = t.attributeNS( ooNS::table, "protection-key", QString::null );
        Q3CString str( p.latin1() );
        kDebug(30518) << "Decoding password: " << str << endl;
        passwd = KCodecs::base64Decode( str );
      }
      kDebug(30518) << "Password hash: '" << passwd << "'" << endl;
      table->setProtected( passwd );
    }

    progress += step;
    emit sigProgress( progress );
    sheet = sheet.nextSibling();
  }

  QDomElement b = body.toElement();
  if ( b.hasAttributeNS( ooNS::table, "structure-protected" ) )
  {
    Q3CString passwd( "" );
    if ( b.hasAttributeNS( ooNS::table, "protection-key" ) )
    {
      QString p = b.attributeNS( ooNS::table, "protection-key", QString::null );
      Q3CString str( p.latin1() );
      kDebug(30518) << "Decoding password: " << str << endl;
      passwd = KCodecs::base64Decode( str );
    }
    kDebug(30518) << "Password hash: '" << passwd << "'" << endl;

    m_doc->map()->setProtected( passwd );
  }

  emit sigProgress( 98 );

  return true;
}

void OpenCalcImport::insertStyles( QDomElement const & element )
{
  if ( element.isNull() )
    return;

  QDomElement e;
  forEachElement( e, element )
  {
    if ( e.isNull() || !e.hasAttributeNS( ooNS::style, "name" ) )
    {
      continue;
    }

    QString name = e.attributeNS( ooNS::style, "name", QString::null );
    kDebug(30518) << "Style: '" << name << "' loaded " << endl;
    m_styles.insert( name, new QDomElement( e ) );
  }
}


void OpenCalcImport::loadOasisAreaName( const QDomElement&body )
{
  QDomNode namedAreas = KoDom::namedItemNS( body, ooNS::table, "named-expressions" );
  if ( !namedAreas.isNull() )
  {
    QDomElement e;
    forEachElement( e, namedAreas )
    {
      if ( e.isNull() || !e.hasAttributeNS( ooNS::table, "name" ) || !e.hasAttributeNS( ooNS::table, "cell-range-address" ) )
      {
        kDebug(30518) << "Reading in named area failed" << endl;
        continue;
      }

      // TODO: what is: table:base-cell-address
      QString name  = e.attributeNS( ooNS::table, "name", QString::null );
      QString areaPoint = e.attributeNS( ooNS::table, "cell-range-address", QString::null );

      m_namedAreas.append( name );
      kDebug(30518) << "Reading in named area, name: " << name << ", area: " << areaPoint << endl;

      OpenCalcPoint point( areaPoint );
      kDebug(30518) << "Area: " << point.translation << endl;

      QString range( point.translation );

      if ( point.translation.find( ':' ) == -1 )
      {
        Point p( point.translation );

        int n = range.find( '!' );
        if ( n > 0 )
          range = range + ":" + range.right( range.length() - n - 1);

        kDebug(30518) << "=> Area: " << range << endl;
      }

      KSpread::Range p( range );

      m_doc->addAreaName( p.range(), name, p.sheetName() );
      kDebug(30518) << "Area range: " << p.sheetName() << endl;
    }
  }
}

void OpenCalcImport::loadOasisCellValidation( const QDomElement&body )
{
    QDomNode validation = KoDom::namedItemNS( body, ooNS::table, "content-validations" );
    if ( !validation.isNull() )
    {
        QDomElement element;
        forEachElement( element, validation )
        {
            if ( element.localName() ==  "content-validation" ) {
                m_validationList.insert( element.attributeNS( ooNS::table, "name", QString::null ), element);
                kDebug(30518)<<" validation found :"<<element.attributeNS( ooNS::table, "name", QString::null )<<endl;
            }
            else {
                kDebug(30518)<<" Tag not recognize :"<<element.tagName()<<endl;
            }
        }
    }
}


QString * OpenCalcImport::loadFormat( QDomElement * element,
                                      FormatType & formatType,
                                      QString name )
{
  if ( !element )
    return 0;

  int  i;
  bool ok;

  QString * format = 0;
  QDomElement e = element->firstChild( ).toElement();
  int precision = 0;
  int leadingZ  = 1;
  bool thousandsSep = false;
  bool negRed = false;

  if ( element->localName() == "time-style" )
    formatType = Custom_format;
  else if ( element->localName() == "date-style" )
    formatType = Custom_format;
  else if ( element->localName() == "percentage-style" )
    formatType = Custom_format;
  else if ( element->localName() == "number-style" )
    formatType = Custom_format;
  else if ( element->localName() == "currency-style" )
    formatType = Custom_format;
  else if ( element->localName() == "boolean-style" )
    formatType = Custom_format;

  if ( !e.isNull() )
    format = new QString();

  // TODO (element):
  // number:automatic-order="true"
  // number:truncate-on-overflow="false"
  // style:volatile="true"

  while ( !e.isNull() )
  {
    if ( e.localName() == "properties" && e.namespaceURI() == ooNS::style )
    {
      if ( e.hasAttributeNS( ooNS::fo, "color" ) )
        negRed = true; // we only support red...
    }
    else if ( e.localName() == "text" && e.namespaceURI()==ooNS::number)
    {
      if ( negRed && ( e.text() == "-" ) )
        ;
      else
        format->append( e.text() );
    }
    else if ( e.localName() == "currency-symbol" && e.namespaceURI()==ooNS::number)
    {
      QString sym( e.text() );
      kDebug(30518) << "Currency: " << sym << endl;
      format->append( sym );
      // number:language="de" number:country="DE">€</number:currency-symbol>
    }
    else if ( e.localName() == "day-of-week" && e.namespaceURI()==ooNS::number)
    {
      if ( e.hasAttributeNS( ooNS::number, "style" ) )
      {
        if ( e.attributeNS( ooNS::number, "style", QString::null ) == "long" )
          format->append( "dddd" );
        else
          format->append( "ddd" );
      }
      else
        format->append( "ddd" );
    }
    else if ( e.localName() == "day" && e.namespaceURI()==ooNS::number)
    {
      if ( e.hasAttributeNS( ooNS::number, "style" ) )
      {
        if ( e.attributeNS( ooNS::number, "style", QString::null ) == "long" )
          format->append( "dd" );
        else
          format->append( "d" );
      }
      else
        format->append( "d" );
    }
    else if ( e.localName() == "month" && e.namespaceURI()==ooNS::number)
    {
      if ( e.hasAttributeNS( ooNS::number, "textual" ) )
      {
        if ( e.attributeNS( ooNS::number, "textual", QString::null ) == "true" )
          format->append( "mm" );
      }

      if ( e.hasAttributeNS( ooNS::number, "style" ) )
      {
        if ( e.attributeNS( ooNS::number, "style", QString::null ) == "long" )
          format->append( "mm" );
        else
          format->append( "m" );
      }
      else
        format->append( "m" );
    }
    else if ( e.localName() == "year" && e.namespaceURI()==ooNS::number)
    {
      if ( e.hasAttributeNS( ooNS::number, "style" ) )
      {
        if ( e.attributeNS( ooNS::number, "style", QString::null ) == "long" )
          format->append( "yyyy" );
        else
          format->append( "yy" );
      }
      else
        format->append( "yy" );
    }
    else if ( e.localName() == "hours" && e.namespaceURI()==ooNS::number)
    {
      if ( e.hasAttributeNS( ooNS::number, "style" ) )
      {
        if ( e.attributeNS( ooNS::number, "style", QString::null ) == "long" )
          format->append( "hh" );
        else
          format->append( "h" );
      }
      else
        format->append( "h" );
    }
    else if ( e.localName() == "minutes" && e.namespaceURI()==ooNS::number)
    {
      if ( e.hasAttributeNS( ooNS::number, "style" ) )
      {
        if ( e.attributeNS( ooNS::number, "style", QString::null ) == "long" )
          format->append( "mm" );
        else
          format->append( "m" );
      }
      else
        format->append( "m" );
    }
    else if ( e.localName() == "seconds" && e.namespaceURI()==ooNS::number)
    {
      if ( e.hasAttributeNS( ooNS::number, "style" ) )
      {
        if ( e.attributeNS( ooNS::number, "style", QString::null ) == "long" )
          format->append( "ss" );
        else
          format->append( "s" );
      }
      else
        format->append( "s" );
    }
    else if ( e.localName() == "am-pm" && e.namespaceURI()==ooNS::number)
    {
      format->append( "AM/PM" );
    }
    else if ( e.localName() == "number" && e.namespaceURI()==ooNS::number)
    {
      // TODO: number:grouping="true"

      if ( e.hasAttributeNS( ooNS::number, "decimal-places" ) )
      {
        int d = e.attributeNS( ooNS::number, "decimal-places", QString::null ).toInt( &ok );
        if ( ok )
          precision = d;
      }

      if ( e.hasAttributeNS( ooNS::number, "min-integer-digits" ) )
      {
        int d = e.attributeNS( ooNS::number, "min-integer-digits", QString::null ).toInt( &ok );
        if ( ok )
          leadingZ = d;
      }

      if ( thousandsSep && leadingZ <= 3 )
      {
        format->append( "#," );
        for ( i = leadingZ; i <= 3; ++i )
          format->append( '#' );
      }

      for ( i = 1; i <= leadingZ; ++i )
      {
        format->append( '0' );
        if ( ( i % 3 == 0 ) && thousandsSep )
          format->append( ',' );
      }

      format->append( '.' );
      for ( i = 0; i < precision; ++i )
        format->append( '0' );
    }
    else if ( e.localName() == "scientific-number" && e.namespaceURI()==ooNS::number)
    {
      int exp = 2;

      if ( e.hasAttributeNS( ooNS::number, "decimal-places" ) )
      {
        int d = e.attributeNS( ooNS::number, "decimal-places", QString::null ).toInt( &ok );
        if ( ok )
          precision = d;
      }

      if ( e.hasAttributeNS( ooNS::number, "min-integer-digits" ) )
      {
        int d = e.attributeNS( ooNS::number, "min-integer-digits", QString::null ).toInt( &ok );
        if ( ok )
          leadingZ = d;
      }

      if ( e.hasAttributeNS( ooNS::number, "min-exponent-digits" ) )
      {
        int d = e.attributeNS( ooNS::number, "min-exponent-digits", QString::null ).toInt( &ok );
        if ( ok )
          exp = d;
        if ( exp <= 0 )
          exp = 1;
      }

      if ( thousandsSep && leadingZ <= 3 )
      {
        format->append( "#," );
        for ( i = leadingZ; i <= 3; ++i )
          format->append( '#' );
      }

      for ( i = 1; i <= leadingZ; ++i )
      {
        format->append( '0' );
        if ( ( i % 3 == 0 ) && thousandsSep )
          format->append( ',' );
      }

      format->append( '.' );
      for ( i = 0; i < precision; ++i )
        format->append( '0' );

      format->append( "E+" );
      for ( i = 0; i < exp; ++i )
        format->append( '0' );

      formatType = Custom_format;
    }
    else if ( e.localName() == "fraction" && e.namespaceURI()==ooNS::number)
    {
      int integer = 0;
      int numerator = 1;
      int denominator = 1;

      if ( e.hasAttributeNS( ooNS::number, "min-integer-digits" ) )
      {
        int d = e.attributeNS( ooNS::number, "min-integer-digits", QString::null ).toInt( &ok );
        if ( ok )
          integer = d;
      }
      if ( e.hasAttributeNS( ooNS::number, "min-numerator-digits" ) )
      {
        int d = e.attributeNS( ooNS::number, "min-numerator-digits", QString::null ).toInt( &ok );
        if ( ok )
          numerator = d;
      }
      if ( e.hasAttributeNS( ooNS::number, "min-denominator-digits" ) )
      {
        int d = e.attributeNS( ooNS::number, "min-denominator-digits", QString::null ).toInt( &ok );
        if ( ok )
          denominator = d;
      }

      for ( i = 0; i <= integer; ++i )
        format->append( '#' );

      format->append( ' ' );

      for ( i = 0; i <= numerator; ++i )
        format->append( '?' );

      format->append( '/' );

      for ( i = 0; i <= denominator; ++i )
        format->append( '?' );
    }
    // Not needed:
    //  <style:map style:condition="value()&gt;=0" style:apply-style-name="N106P0"/>
    // we handle painting negative numbers in red differently

    e = e.nextSibling().toElement();
  }

  if ( negRed )
  {
    QString f( *format );
    format->append( ";[Red]" );
    format->append( f );
  }

  kDebug(30518) << "*** New FormatString: " << *format << endl << endl;

  m_formats.insert( name, format );

  return format;
}

void OpenCalcImport::loadFontStyle( Format * layout, QDomElement const * font ) const
{
  if ( !font || !layout )
    return;

  kDebug(30518) << "Copy font style from the layout " << font->tagName() << ", " << font->nodeName() << endl;

  if ( font->hasAttributeNS( ooNS::fo, "font-family" ) )
    layout->setTextFontFamily( font->attributeNS( ooNS::fo, "font-family", QString::null ) );
  if ( font->hasAttributeNS( ooNS::fo, "color" ) )
    layout->setTextColor( QColor( font->attributeNS( ooNS::fo, "color", QString::null ) ) );
  if ( font->hasAttributeNS( ooNS::fo, "font-size" ) )
      layout->setTextFontSize( int( KoUnit::parseValue( font->attributeNS( ooNS::fo, "font-size", QString::null ), 10 ) ) );
  else
    layout->setTextFontSize( 10 );
  if ( font->hasAttributeNS( ooNS::fo, "font-style" ) )
  {
    kDebug(30518) << "italic" << endl;
    layout->setTextFontItalic( true ); // only thing we support
  }
  if ( font->hasAttributeNS( ooNS::fo, "font-weight" ) )
    layout->setTextFontBold( true ); // only thing we support
  if ( font->hasAttributeNS( ooNS::fo, "text-underline" ) || font->hasAttributeNS( ooNS::style, "text-underline" ) )
    layout->setTextFontUnderline( true ); // only thing we support
  if ( font->hasAttributeNS( ooNS::style, "text-crossing-out" ) )
    layout->setTextFontStrike( true ); // only thing we support
  if ( font->hasAttributeNS( ooNS::style, "font-pitch" ) )
  {
    // TODO: possible values: fixed, variable
  }
  // TODO:
  // text-underline-color
}

void OpenCalcImport::loadBorder( Format * layout, QString const & borderDef, bPos pos ) const
{
  if ( borderDef == "none" )
    return;

  int p = borderDef.find( ' ' );
  if ( p < 0 )
    return;

  QPen pen;
  QString w = borderDef.left( p );
  pen.setWidth( (int) KoUnit::parseValue( w ) );


  ++p;
  int p2 = borderDef.find( ' ', p );
  QString s = borderDef.mid( p, p2 - p );

  kDebug(30518) << "Borderstyle: " << s << endl;

  if ( s == "solid" || s == "double" )
    pen.setStyle( Qt::SolidLine );
  else
  {
#if 0
    // TODO: not supported by oocalc
    pen.setStyle( Qt::DashLine );
    pen.setStyle( Qt::DotLine );
    pen.setStyle( Qt::DashDotLine );
    pen.setStyle( Qt::DashDotDotLine );
#endif
    pen.setStyle( Qt::SolidLine ); //default.
  }

  ++p2;
  p = borderDef.find( ' ', p2 );
  if ( p == -1 )
    p = borderDef.length();

  pen.setColor( QColor( borderDef.right( p - p2 ) ) );

  if ( pos == Qt::DockLeft )
    layout->setLeftBorderPen( pen );
  else if ( pos == Qt::DockTop )
    layout->setTopBorderPen( pen );
  else if ( pos == Qt::DockRight )
    layout->setRightBorderPen( pen );
  else if ( pos == Qt::DockBottom )
    layout->setBottomBorderPen( pen );
  else if ( pos == Border )
  {
    layout->setLeftBorderPen( pen );
    layout->setTopBorderPen( pen );
    layout->setRightBorderPen( pen );
    layout->setBottomBorderPen( pen );
  }
  // TODO Diagonals not supported by oocalc
}

void OpenCalcImport::loadStyleProperties( Format * layout, QDomElement const & property ) const
{
  kDebug(30518) << "*** Loading style properties *****" << endl;

  if ( property.hasAttributeNS( ooNS::style, "decimal-places" ) )
  {
    bool ok = false;
    int p = property.attributeNS( ooNS::style, "decimal-places", QString::null ).toInt( &ok );
    if (ok )
      layout->setPrecision( p );
  }

  if ( property.hasAttributeNS( ooNS::style, "font-name" ) )
  {
    QDomElement * font = m_styles[ property.attributeNS( ooNS::style, "font-name", QString::null ) ];
    loadFontStyle( layout, font ); // generell font style
  }

  loadFontStyle( layout, &property ); // specific font style

  // TODO:
  //   diagonal: fall + goup
  //   fo:direction="ltr"
  //   style:text-align-source  ("fix")
  //   style:shadow
  //   style:text-outline
  //   indents from right, top, bottom
  //   style:condition="cell-content()=15"
  //     => style:apply-style-name="Result" style:base-cell-address="Sheet6.A5"/>

  if ( property.hasAttributeNS( ooNS::style, "rotation-angle" ) )
  {
    bool ok = false;
    int a = property.attributeNS( ooNS::style, "rotation-angle", QString::null ).toInt( &ok );
    if ( ok )
      layout->setAngle( -a + 1 );
  }

  if ( property.hasAttributeNS( ooNS::fo, "direction" ) )
  {
      layout->setVerticalText( true );
  }
  if ( property.hasAttributeNS( ooNS::fo, "text-align" ) )
  {
    QString s = property.attributeNS( ooNS::fo, "text-align", QString::null );
    if ( s == "center" )
      layout->setAlign( Style::Center );
    else if ( s == "end" )
      layout->setAlign( Style::Right );
    else if ( s == "start" )
      layout->setAlign( Style::Left );
    else if ( s == "justify" ) // TODO in KSpread!
      layout->setAlign( Style::Center );
  }
  if (  property.hasAttributeNS( ooNS::fo, "margin-left" ) )
  {
      kDebug(30518)<<"margin-left :"<<KoUnit::parseValue( property.attributeNS( ooNS::fo, "margin-left", QString::null ),0.0 )<<endl;
      layout->setIndent( KoUnit::parseValue( property.attributeNS( ooNS::fo, "margin-left", QString::null ),0.0 ) );
  }
  if ( property.hasAttributeNS( ooNS::fo, "background-color" ) )
    layout->setBgColor( QColor( property.attributeNS( ooNS::fo, "background-color", QString::null ) ) );

  if ( property.hasAttributeNS( ooNS::style, "print-content" ) )
  {
    if ( property.attributeNS( ooNS::style, "print-content", QString::null ) == "false" )
      layout->setDontPrintText( false );
  }
  if ( property.hasAttributeNS( ooNS::style, "cell-protect" ) )
  {
    QString prot( property.attributeNS( ooNS::style, "cell-protect", QString::null ) );
    if ( prot == "none" )
    {
      layout->setNotProtected( true );
      layout->setHideFormula( false );
      layout->setHideAll( false );
    }
    else if ( prot == "formula-hidden" )
    {
      layout->setNotProtected( true );
      layout->setHideFormula( true );
      layout->setHideAll( false );
    }
    else if ( prot == "protected formula-hidden" )
    {
      layout->setNotProtected( false );
      layout->setHideFormula( true );
      layout->setHideAll( false );
    }
    else if ( prot == "hidden-and-protected" )
    {
      layout->setNotProtected( false );
      layout->setHideFormula( false );
      layout->setHideAll( true );
    }
    else if ( prot == "protected" )
    {
      layout->setNotProtected( false );
      layout->setHideFormula( false );
      layout->setHideAll( false );
    }
    kDebug(30518) << "Cell " << prot << endl;
  }

  if ( property.hasAttributeNS( ooNS::fo, "padding-left" ) )
    layout->setIndent(  KoUnit::parseValue(property.attributeNS( ooNS::fo, "padding-left", QString::null ) ) );

  if ( property.hasAttributeNS( ooNS::fo, "vertical-align" ) )
  {
    QString s = property.attributeNS( ooNS::fo, "vertical-align", QString::null );
    if ( s == "middle" )
      layout->setAlignY( Style::Middle );
    else if ( s == "bottom" )
      layout->setAlignY( Style::Bottom );
    else
      layout->setAlignY( Style::Top );
  }
  else
      layout->setAlignY( Style::Bottom );

  if ( property.hasAttributeNS( ooNS::fo, "wrap-option" ) )
  {
    layout->setMultiRow( true );

    /* we do not support anything else yet
      QString s = property.attributeNS( ooNS::fo, "wrap-option", QString::null );
      if ( s == "wrap" )
      layout->setMultiRow( true );
    */
  }

  if ( property.hasAttributeNS( ooNS::fo, "border-bottom" ) )
  {
    loadBorder( layout, property.attributeNS( ooNS::fo, "border-bottom", QString::null ), Bottom );
    // TODO: style:border-line-width-bottom if double!
  }

  if ( property.hasAttributeNS( ooNS::fo, "border-right" ) )
  {
    loadBorder( layout, property.attributeNS( ooNS::fo, "border-right", QString::null ), Right );
    // TODO: style:border-line-width-right
  }

  if ( property.hasAttributeNS( ooNS::fo, "border-top" ) )
  {
    loadBorder( layout, property.attributeNS( ooNS::fo, "border-top", QString::null ), Top );
    // TODO: style:border-line-width-top
  }

  if ( property.hasAttributeNS( ooNS::fo, "border-left" ) )
  {
    loadBorder( layout, property.attributeNS( ooNS::fo, "border-left", QString::null ), Left );
    // TODO: style:border-line-width-left
  }

  if ( property.hasAttributeNS( ooNS::fo, "border" ) )
  {
    loadBorder( layout, property.attributeNS( ooNS::fo, "border", QString::null ), Border );
    // TODO: style:border-line-width-left
  }
}

void OpenCalcImport::readInStyle( Format * layout, QDomElement const & style )
{
  kDebug(30518) << "** Reading Style: " << style.tagName() << "; " << style.attributeNS( ooNS::style, "name", QString::null) << endl;
  if ( style.localName() == "style" && style.namespaceURI()==ooNS::style)
  {
    if ( style.hasAttributeNS( ooNS::style, "parent-style-name" ) )
    {
      Format * cp
        = m_defaultStyles.find( style.attributeNS( ooNS::style, "parent-style-name", QString::null ) );
      kDebug(30518) << "Copying layout from " << style.attributeNS( ooNS::style, "parent-style-name", QString::null ) << endl;

      if ( cp != 0 )
        layout->copy( *cp );
    }
    else if ( style.hasAttributeNS( ooNS::style, "family") )
    {
      QString name = style.attribute( "style-family" ) + "default";
      Format * cp = m_defaultStyles.find( name );

      kDebug(30518) << "Copying layout from " << name << ", " << !cp << endl;

      if ( cp != 0 )
        layout->copy( *cp );
    }

    if ( style.hasAttributeNS( ooNS::style, "data-style-name" ) )
    {
      QString * format = m_formats[ style.attributeNS( ooNS::style, "data-style-name", QString::null ) ];
      FormatType formatType;

      if ( !format )
      {
        // load and convert it
        QString name( style.attributeNS( ooNS::style, "data-style-name", QString::null ) );
        format = loadFormat( m_styles[ name ], formatType, name );
      }

      if ( format )
      {
        layout->setFormatString( *format );
        layout->setFormatType( formatType );
      }

      // <number:currency-symbol number:language="de" number:country="DE">€</number:currency-symbol>
    }
  }

  QDomElement property;
  forEachElement( property, style )
  {
    if ( property.localName() == "properties" && property.namespaceURI() == ooNS::style )
      loadStyleProperties( layout, property );

    kDebug(30518) << layout->textFontFamily( 0, 0 ) << endl;
  }
}

bool OpenCalcImport::createStyleMap( QDomDocument const & styles )
{
  QDomElement content  = styles.documentElement();
  QDomNode docStyles   = KoDom::namedItemNS( content, ooNS::office, "document-styles" );

  if ( content.hasAttributeNS( ooNS::office, "version" ) )
  {
    bool ok = true;
    double d = content.attributeNS( ooNS::office, "version", QString::null ).toDouble( &ok );

    if ( ok )
    {
      kDebug(30518) << "OpenCalc version: " << d << endl;
      if ( d > 1.0 )
      {
        QString message( i18n("This document was created with OpenOffice.org version '%1'. This filter was written for version 1.0. Reading this file could cause strange behavior, crashes or incorrect display of the data. Do you want to continue converting the document?") );
        message.arg( content.attributeNS( ooNS::office, "version", QString::null ) );
        if ( KMessageBox::warningYesNo( 0, message, i18n( "Unsupported document version" ) ) == KMessageBox::No )
          return false;
      }
    }
  }

  QDomNode fontStyles = KoDom::namedItemNS( content, ooNS::office, "font-decls" );

  if ( !fontStyles.isNull() )
  {
    kDebug(30518) << "Starting reading in font-decl..." << endl;

    insertStyles( fontStyles.toElement() );
  }
  else
    kDebug(30518) << "No items found" << endl;

  kDebug(30518) << "Starting reading in auto:styles" << endl;

  QDomNode autoStyles = KoDom::namedItemNS( content, ooNS::office, "automatic-styles" );
  if ( !autoStyles.isNull() )
    insertStyles( autoStyles.toElement() );
  else
    kDebug(30518) << "No items found" << endl;


  kDebug(30518) << "Reading in master styles" << endl;

  QDomNode masterStyles = KoDom::namedItemNS( content, ooNS::office, "master-styles" );

  if ( masterStyles.isNull() )
  {
    kDebug(30518) << "Nothing found " << endl;
  }

  QDomElement master = KoDom::namedItemNS( masterStyles, ooNS::style, "master-page");
  if ( !master.isNull() )
  {
    QString name( "pm" );
    name += master.attributeNS( ooNS::style, "name", QString::null );
    kDebug(30518) << "Master style: '" << name << "' loaded " << endl;
    m_styles.insert( name, new QDomElement( master ) );

    master = master.nextSibling().toElement();
  }


  kDebug(30518) << "Starting reading in office:styles" << endl;

  QDomNode fixedStyles = KoDom::namedItemNS( content, ooNS::office, "styles" );

  kDebug(30518) << "Reading in default styles" << endl;

  QDomNode def = KoDom::namedItemNS( fixedStyles, ooNS::style, "default-style" );
  kDebug()<<" def !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! :"<<def.isNull()<<endl;
  while ( !def.isNull() )
  {
    QDomElement e = def.toElement();
    kDebug(30518) << "Style found " << e.nodeName() << ", tag: " << e.tagName() << endl;

    if ( e.nodeName() != "style:default-style" )
    {
      def = def.nextSibling();
      continue;
    }

    if ( !e.isNull() )
    {
      Format * layout = new Format( 0, m_doc->styleManager()->defaultStyle() );

      readInStyle( layout, e );
      kDebug(30518) << "Default style " << e.attributeNS( ooNS::style, "family", QString::null ) << "default" << " loaded " << endl;

      m_defaultStyles.insert( e.attributeNS( ooNS::style, "family", QString::null ) + "default", layout );
      //      QFont font = layout->font();
      //      kDebug(30518) << "Font: " << font.family() << ", " << font.toString() << endl;
    }

    def = def.nextSibling();
  }

  QDomElement defs = KoDom::namedItemNS( fixedStyles, ooNS::style, "style" );
  while ( !defs.isNull() )
  {
    if ( defs.nodeName() != "style:style" )
      break; // done

    if ( !defs.hasAttributeNS( ooNS::style, "name" ) )
    {
      // ups...
      defs = defs.nextSibling().toElement();
      continue;
    }

    Format * layout = new Format( 0, m_doc->styleManager()->defaultStyle() );
    readInStyle( layout, defs );
    kDebug(30518) << "Default style " << defs.attributeNS( ooNS::style, "name", QString::null ) << " loaded " << endl;

    m_defaultStyles.insert( defs.attributeNS( ooNS::style, "name", QString::null ), layout );
    //    kDebug(30518) << "Font: " << layout->font().family() << ", " << layout->font().toString() << endl;

    defs = defs.nextSibling().toElement();
  }

  if ( !fixedStyles.isNull() )
    insertStyles( fixedStyles.toElement() );

  kDebug(30518) << "Starting reading in automatic styles" << endl;

  content = m_content.documentElement();
  autoStyles = KoDom::namedItemNS( content, ooNS::office, "automatic-styles" );

  if ( !autoStyles.isNull() )
    insertStyles( autoStyles.toElement() );

  fontStyles = KoDom::namedItemNS( content, ooNS::office, "font-decls" );

  if ( !fontStyles.isNull() )
  {
    kDebug(30518) << "Starting reading in special font decl" << endl;

    insertStyles( fontStyles.toElement() );
  }

  kDebug(30518) << "Styles read in." << endl;

  return true;
}

void OpenCalcImport::loadOasisValidation( Validity* val, const QString& validationName )
{
    kDebug(30518)<<"validationName:"<<validationName<<endl;
    QDomElement element = m_validationList[validationName];
    if ( element.hasAttributeNS( ooNS::table, "condition" ) )
    {
        QString valExpression = element.attributeNS( ooNS::table, "condition", QString::null );
        kDebug(30518)<<" element.attribute( table:condition ) "<<valExpression<<endl;
        //Condition ::= ExtendedTrueCondition | TrueFunction 'and' TrueCondition
        //TrueFunction ::= cell-content-is-whole-number() | cell-content-is-decimal-number() | cell-content-is-date() | cell-content-is-time()
        //ExtendedTrueCondition ::= ExtendedGetFunction | cell-content-text-length() Operator Value
        //TrueCondition ::= GetFunction | cell-content() Operator Value
        //GetFunction ::= cell-content-is-between(Value, Value) | cell-content-is-not-between(Value, Value)
        //ExtendedGetFunction ::= cell-content-text-length-is-between(Value, Value) | cell-content-text-length-is-not-between(Value, Value)
        //Operator ::= '<' | '>' | '<=' | '>=' | '=' | '!='
        //Value ::= NumberValue | String | Formula
        //A Formula is a formula without an equals (=) sign at the beginning. See section 8.1.3 for more information.
        //A String comprises one or more characters surrounded by quotation marks.
        //A NumberValue is a whole or decimal number. It must not contain comma separators for numbers of 1000 or greater.

        //ExtendedTrueCondition
        if ( valExpression.contains( "cell-content-text-length()" ) )
        {
            //"cell-content-text-length()>45"
            valExpression = valExpression.remove("cell-content-text-length()" );
            kDebug(30518)<<" valExpression = :"<<valExpression<<endl;
            val->m_restriction = Restriction::TextLength;

            loadOasisValidationCondition( val, valExpression );
        }
        //cell-content-text-length-is-between(Value, Value) | cell-content-text-length-is-not-between(Value, Value)
        else if ( valExpression.contains( "cell-content-text-length-is-between" ) )
        {
            val->m_restriction = Restriction::TextLength;
            val->m_cond = Conditional::Between;
            valExpression = valExpression.remove( "cell-content-text-length-is-between(" );
            kDebug(30518)<<" valExpression :"<<valExpression<<endl;
            valExpression = valExpression.remove( ")" );
            QStringList listVal = QStringList::split( ",", valExpression );
            loadOasisValidationValue( val, listVal );
        }
        else if ( valExpression.contains( "cell-content-text-length-is-not-between" ) )
        {
            val->m_restriction = Restriction::TextLength;
            val->m_cond = Conditional::Different;
            valExpression = valExpression.remove( "cell-content-text-length-is-not-between(" );
            kDebug(30518)<<" valExpression :"<<valExpression<<endl;
            valExpression = valExpression.remove( ")" );
            kDebug(30518)<<" valExpression :"<<valExpression<<endl;
            QStringList listVal = QStringList::split( ",", valExpression );
            loadOasisValidationValue( val, listVal );

        }
        //TrueFunction ::= cell-content-is-whole-number() | cell-content-is-decimal-number() | cell-content-is-date() | cell-content-is-time()
        else
        {
            if (valExpression.contains( "cell-content-is-whole-number()" ) )
            {
                val->m_restriction =  Restriction::Number;
                valExpression = valExpression.remove( "cell-content-is-whole-number() and " );
            }
            else if (valExpression.contains( "cell-content-is-decimal-number()" ) )
            {
                val->m_restriction = Restriction::Integer;
                valExpression = valExpression.remove( "cell-content-is-decimal-number() and " );
            }
            else if (valExpression.contains( "cell-content-is-date()" ) )
            {
                val->m_restriction = Restriction::Date;
                valExpression = valExpression.remove( "cell-content-is-date() and " );
            }
            else if (valExpression.contains( "cell-content-is-time()" ) )
            {
                val->m_restriction = Restriction::Time;
                valExpression = valExpression.remove( "cell-content-is-time() and " );
            }
            kDebug(30518)<<"valExpression :"<<valExpression<<endl;

            if ( valExpression.contains( "cell-content()" ) )
            {
                valExpression = valExpression.remove( "cell-content()" );
                loadOasisValidationCondition( val, valExpression );
            }
            //GetFunction ::= cell-content-is-between(Value, Value) | cell-content-is-not-between(Value, Value)
            //for the moment we support just int/double value, not text/date/time :(
            if ( valExpression.contains( "cell-content-is-between(" ) )
            {
                valExpression = valExpression.remove( "cell-content-is-between(" );
                valExpression = valExpression.remove( ")" );
                QStringList listVal = QStringList::split( "," , valExpression );
                loadOasisValidationValue( val, listVal );

                val->m_cond = Conditional::Between;
            }
            if ( valExpression.contains( "cell-content-is-not-between(" ) )
            {
                valExpression = valExpression.remove( "cell-content-is-not-between(" );
                valExpression = valExpression.remove( ")" );
                QStringList listVal = QStringList::split( ",", valExpression );
                loadOasisValidationValue( val, listVal );
                val->m_cond = Conditional::Different;
            }
        }
    }
    if ( element.hasAttributeNS( ooNS::table, "allow-empty-cell" ) )
    {
        val->allowEmptyCell = ( ( element.attributeNS( ooNS::table, "allow-empty-cell", QString::null )=="true" ) ? true : false );

    }
    if ( element.hasAttributeNS( ooNS::table, "base-cell-address" ) )
    {
        //todo what is it ?
    }

    QDomElement help = KoDom::namedItemNS( element, ooNS::table, "help-message" );
    if ( !help.isNull() )
    {
        if ( help.hasAttributeNS( ooNS::table, "title" ) )
             val->titleInfo = help.attributeNS( ooNS::table, "title", QString::null );
        if ( help.hasAttributeNS( ooNS::table, "display" ) )
            val->displayValidationInformation = ( ( help.attributeNS( ooNS::table, "display", QString::null )=="true" ) ? true : false );
        QDomElement attrText = KoDom::namedItemNS( help, ooNS::text, "p" );
        if ( !attrText.isNull() )
            val->messageInfo = attrText.text();
    }

    QDomElement error = KoDom::namedItemNS( element, ooNS::table, "error-message" );
    if ( !error.isNull() )
    {
        if ( error.hasAttributeNS( ooNS::table, "title" ) )
            val->title = error.attributeNS( ooNS::table, "title", QString::null );
        if ( error.hasAttributeNS( ooNS::table, "message-type" ) )
        {
            QString str = error.attributeNS( ooNS::table, "message-type", QString::null );
            if ( str == "warning" )
              val->m_action = Action::Warning;
            else if ( str == "information" )
              val->m_action = Action::Information;
            else if ( str == "stop" )
              val->m_action = Action::Stop;
            else
                kDebug(30518)<<"validation : message type unknown  :"<<str<<endl;
        }

        if ( error.hasAttributeNS( ooNS::table, "display" ) )
        {
            kDebug(30518)<<" display message :"<<error.attributeNS( ooNS::table, "display", QString::null )<<endl;
            val->displayMessage = (error.attributeNS( ooNS::table, "display", QString::null )=="true");
        }
        QDomElement attrText = KoDom::namedItemNS( error, ooNS::text, "p" );
        if ( !attrText.isNull() )
            val->message = attrText.text();
    }
}

void OpenCalcImport::loadOasisValidationValue( Validity* val, const QStringList &listVal )
{
    bool ok = false;
    kDebug(30518)<<" listVal[0] :"<<listVal[0]<<" listVal[1] :"<<listVal[1]<<endl;

    if ( val->m_restriction == Restriction::Date )
    {
        val->dateMin = QDate::fromString( listVal[0] );
        val->dateMax = QDate::fromString( listVal[1] );
    }
    else if ( val->m_restriction == Restriction::Time )
    {
        val->timeMin = QTime::fromString( listVal[0] );
        val->timeMax = QTime::fromString( listVal[1] );
    }
    else
    {
        val->valMin = listVal[0].toDouble(&ok);
        if ( !ok )
        {
            val->valMin = listVal[0].toInt(&ok);
            if ( !ok )
                kDebug(30518)<<" Try to parse this value :"<<listVal[0]<<endl;

#if 0
            if ( !ok )
                val->valMin = listVal[0];
#endif
        }
        ok=false;
        val->valMax = listVal[1].toDouble(&ok);
        if ( !ok )
        {
            val->valMax = listVal[1].toInt(&ok);
            if ( !ok )
                kDebug(30518)<<" Try to parse this value :"<<listVal[1]<<endl;

#if 0
            if ( !ok )
                val->valMax = listVal[1];
#endif
        }
    }
}


void OpenCalcImport::loadOasisValidationCondition( Validity* val,QString &valExpression )
{
    QString value;
    if (valExpression.contains( "<=" ) )
    {
        value = valExpression.remove( "<=" );
        val->m_cond = Conditional::InferiorEqual;
    }
    else if (valExpression.contains( ">=" ) )
    {
        value = valExpression.remove( ">=" );
        val->m_cond = Conditional::SuperiorEqual;
    }
    else if (valExpression.contains( "!=" ) )
    {
        //add Differentto attribute
        value = valExpression.remove( "!=" );
        val->m_cond = Conditional::DifferentTo;
    }
    else if ( valExpression.contains( "<" ) )
    {
        value = valExpression.remove( "<" );
        val->m_cond = Conditional::Inferior;
    }
    else if(valExpression.contains( ">" ) )
    {
        value = valExpression.remove( ">" );
        val->m_cond = Conditional::Superior;
    }
    else if (valExpression.contains( "=" ) )
    {
        value = valExpression.remove( "=" );
        val->m_cond = Conditional::Equal;
    }
    else
        kDebug(30518)<<" I don't know how to parse it :"<<valExpression<<endl;
    kDebug(30518)<<" value :"<<value<<endl;
    if ( val->m_restriction == Restriction::Date )
    {
        val->dateMin = QDate::fromString( value );
    }
    else if ( val->m_restriction == Restriction::Date )
    {
        val->timeMin = QTime::fromString( value );
    }
    else
    {
        bool ok = false;
        val->valMin = value.toDouble(&ok);
        if ( !ok )
        {
            val->valMin = value.toInt(&ok);
            if ( !ok )
                kDebug(30518)<<" Try to parse this value :"<<value<<endl;

#if 0
            if ( !ok )
                val->valMin = value;
#endif
        }
    }
}


int OpenCalcImport::readMetaData()
{
  int result = 5;
  KoDocumentInfo * docInfo          = m_doc->documentInfo();
  KoDocumentInfoAbout  * aboutPage  = static_cast<KoDocumentInfoAbout *>(docInfo->page( "about" ));
  KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor*>(docInfo->page( "author" ));

  QDomNode meta   = KoDom::namedItemNS( m_meta, ooNS::office, "document-meta" );
  QDomNode office = KoDom::namedItemNS( meta, ooNS::office, "meta" );

  if ( office.isNull() )
    return 2;

  QDomElement e = KoDom::namedItemNS( office, ooNS::dc, "creator" );
  if ( !e.isNull() && !e.text().isEmpty() )
    authorPage->setFullName( e.text() );

  e = KoDom::namedItemNS( office, ooNS::dc, "title" );
  if ( !e.isNull() && !e.text().isEmpty() )
    aboutPage->setTitle( e.text() );

  e = KoDom::namedItemNS( office, ooNS::dc, "description" );
  if ( !e.isNull() && !e.text().isEmpty() )
    aboutPage->setAbstract( e.text() );

  e = KoDom::namedItemNS( office, ooNS::dc, "subject" );
  if ( !e.isNull() && !e.text().isEmpty() )
    aboutPage->setSubject( e.text() );

  e= KoDom::namedItemNS( office, ooNS::meta, "keywords" );
  if ( !e.isNull() )
  {
      e = KoDom::namedItemNS( e,  ooNS::meta, "keyword" );
      if ( !e.isNull() && !e.text().isEmpty() )
          aboutPage->setKeywords( e.text() );
  }

  e = KoDom::namedItemNS( office, ooNS::meta, "document-statistic" );
  if ( !e.isNull() && e.hasAttributeNS( ooNS::meta, "table-count" ) )
  {
    bool ok = false;
    result = e.attributeNS( ooNS::meta, "table-count", QString::null ).toInt( &ok );
    if ( !ok )
      result = 5;
  }

  m_meta.clear(); // not needed anymore

  return result;
}

KoFilter::ConversionStatus OpenCalcImport::convert( QByteArray const & from, QByteArray const & to )
{
  kDebug(30518) << "Entering OpenCalc Import filter: " << from << " - " << to << endl;

  KoDocument * document = m_chain->outputDocument();
  if ( !document )
    return KoFilter::StupidError;

  if (  !::qt_cast<const KSpread::Doc *>( document ) )  // it's safer that way :)
  {
    kWarning(30518) << "document isn't a KSpread::Doc but a " << document->className() << endl;
    return KoFilter::NotImplemented;
  }

  if ( ( from != "application/vnd.sun.xml.calc" && from != "application/vnd.sun.xml.calc.template") || to != "application/x-kspread" )
  {
    kWarning(30518) << "Invalid mimetypes " << from << " " << to << endl;
    return KoFilter::NotImplemented;
  }

  m_doc = ( Doc * ) document;

  if ( m_doc->mimeType() != "application/x-kspread" )
  {
    kWarning(30518) << "Invalid document mimetype " << m_doc->mimeType() << endl;
    return KoFilter::NotImplemented;
  }

  kDebug(30518) << "Opening file " << endl;

  KoFilter::ConversionStatus preStatus = openFile();

  if ( preStatus != KoFilter::OK )
    return preStatus;

  emit sigProgress( 13 );
  int tables = readMetaData();

  emit sigProgress( 15 );

  if ( !parseBody( tables ) )
    return KoFilter::StupidError;

  emit sigProgress( 100 );
  return KoFilter::OK;
}

KoFilter::ConversionStatus OpenCalcImport::openFile()
{
  KoStore * store = KoStore::createStore( m_chain->inputFile(), KoStore::Read);

  kDebug(30518) << "Store created" << endl;

  if ( !store )
  {
    kWarning(30518) << "Couldn't open the requested file." << endl;
    return KoFilter::FileNotFound;
  }

  kDebug(30518) << "Trying to open content.xml" << endl;
  QString messageError;
  loadAndParse( m_content, "content.xml", store);
  kDebug(30518) << "Opened" << endl;

  QDomDocument styles;
  kDebug(30518) << "file content.xml loaded " << endl;

  loadAndParse( styles, "styles.xml", store);

  loadAndParse( m_meta, "meta.xml", store);
  loadAndParse( m_settings, "settings.xml", store);

  delete store;

  emit sigProgress( 10 );

  if ( !createStyleMap( styles ) )
    return KoFilter::UserCancelled;

  return KoFilter::OK;
}

KoFilter::ConversionStatus OpenCalcImport::loadAndParse( QDomDocument& doc, const QString& fileName,KoStore *m_store )
{
    return OoUtils::loadAndParse( fileName, doc, m_store);
}

#include "opencalcimport.moc"

