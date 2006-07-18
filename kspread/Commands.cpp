/* This file is part of the KDE project
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2004 Laurent Montel <montel@kde.org>

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

#include "Commands.h"
#include "Damages.h"
#include "Canvas.h"
#include "Doc.h"
#include "Locale.h"
#include "Map.h"
#include "Sheet.h"
#include "Undo.h"
#include "Util.h"

#include "SheetPrint.h"

using namespace KSpread;

// ----- UndoWrapperCommand -----

UndoWrapperCommand::UndoWrapperCommand( UndoAction* ua )
{
  undoAction = ua;
}

void UndoWrapperCommand::execute()
{
  // This is not really safe and functional, but UndoWrapperCommand
  // is a workaround anyway. So leave it as it it (Ariya)
  undoAction->redo();
}

void UndoWrapperCommand::unexecute()
{
  undoAction->undo();
}

QString UndoWrapperCommand::name() const
{
  return undoAction->getName();
}

// ----- MergeCellsCommand -----

MergeCellCommand::MergeCellCommand( Cell* c, int cs, int rs )
{
  cell = c;
  colSpan = cs;
  rowSpan = rs;
  oldColSpan = cell->extraXCells();
  oldRowSpan = cell->extraYCells();
  if( cell )
  {
    QRect area( cell->column(), cell->row(), cs+1, rs+1 );
    rangeName = util_rangeName( area );
  }
}

QString MergeCellCommand::name() const
{
  if( rangeName.isEmpty() )
    return i18n("Merge Cells");
  else
    return i18n("Merge Cells %1", rangeName );
}

void MergeCellCommand::execute()
{
  Sheet* sheet = cell->sheet();
  if( !sheet ) return;
  sheet->changeMergedCell( cell->column(), cell->row(), colSpan, rowSpan);
}

void MergeCellCommand::unexecute()
{
  Sheet* sheet = cell->sheet();
  if( !sheet ) return;
  sheet->changeMergedCell( cell->column(), cell->row(), oldColSpan, oldRowSpan);
}

// ----- DissociateCellCommand -----

DissociateCellCommand::DissociateCellCommand( Cell* c )
{
  cell = c;
  oldColSpan = cell->extraXCells();
  oldRowSpan = cell->extraYCells();
}

QString DissociateCellCommand::name() const
{
  return i18n("Dissociate Cell");
}

void DissociateCellCommand::execute()
{
  Sheet* sheet = cell->sheet();
  if( !sheet ) return;
  sheet->changeMergedCell( cell->column(), cell->row(), 0, 0 );
}

void DissociateCellCommand::unexecute()
{
  Sheet* sheet = cell->sheet();
  if( !sheet ) return;
  sheet->changeMergedCell( cell->column(), cell->row(), oldColSpan, oldRowSpan);
}

// ----- RenameSheetCommand -----

RenameSheetCommand::RenameSheetCommand( Sheet* s, const QString &name )
{
  sheet = s;
  if( s ) oldName = s->sheetName();
  newName = name;
}

QString RenameSheetCommand::name() const
{
  return i18n("Rename Sheet");
}

void RenameSheetCommand::execute()
{
  if( sheet )
    sheet->setSheetName( newName );
}

void RenameSheetCommand::unexecute()
{
  if( sheet )
    sheet->setSheetName( oldName );
}

// ----- HideSheetCommand -----

HideSheetCommand::HideSheetCommand( Sheet* sheet )
{
  doc = sheet->doc();
  sheetName = sheet->sheetName();
}

void HideSheetCommand::execute()
{
  Sheet* sheet = doc->map()->findSheet( sheetName );
  if( !sheet ) return;

  sheet->hideSheet( true );
}

void HideSheetCommand::unexecute()
{
  Sheet* sheet = doc->map()->findSheet( sheetName );
  if( !sheet ) return;

  sheet->hideSheet( false );
}

QString HideSheetCommand::name() const
{
    QString n =  i18n("Hide Sheet %1", sheetName );
    if( n.length() > 64 ) n = i18n("Hide Sheet");
    return n;
}

// ----- ShowSheetCommand -----

ShowSheetCommand::ShowSheetCommand( Sheet* sheet )
{
  doc = sheet->doc();
  sheetName = sheet->sheetName();
}

void ShowSheetCommand::execute()
{
  Sheet* sheet = doc->map()->findSheet( sheetName );
  if( !sheet ) return;

  sheet->hideSheet( false );
}

void ShowSheetCommand::unexecute()
{
  Sheet* sheet = doc->map()->findSheet( sheetName );
  if( !sheet ) return;

  sheet->hideSheet( true );
}

QString ShowSheetCommand::name() const
{
    QString n =  i18n("Show Sheet %1", sheetName );
    if( n.length() > 64 ) n = i18n("Show Sheet");
    return n;
}


// ----- AddSheetCommand -----

AddSheetCommand::AddSheetCommand( Sheet* s )
{
    sheet = s;
    doc = sheet->doc();
    doc->map()->addSheet( s );
}

void AddSheetCommand::execute()
{
    sheet->workbook()->insertSheet( sheet );
    doc->insertSheet( sheet );
}

void AddSheetCommand::unexecute()
{
    sheet->workbook()->takeSheet( sheet );
    doc->takeSheet( sheet );
}

QString AddSheetCommand::name() const
{
    return i18n("Add Sheet");
}


// ----- RemoveSheetCommand -----

RemoveSheetCommand::RemoveSheetCommand( Sheet* s )
{
    sheet = s;
    doc = sheet->doc();
}

void RemoveSheetCommand::execute()
{
    sheet->workbook()->takeSheet( sheet );
    doc->takeSheet( sheet );
}

void RemoveSheetCommand::unexecute()
{
    sheet->workbook()->insertSheet( sheet );
    doc->insertSheet( sheet );
}

QString RemoveSheetCommand::name() const
{
    return i18n("Remove Sheet");
}

// ----- SheetPropertiesCommand -----

SheetPropertiesCommand::SheetPropertiesCommand( Doc* d, Sheet* s )
{
    sheet = s;
    doc = d;
    oldDirection = newDirection = sheet->layoutDirection();
    oldAutoCalc = newAutoCalc = sheet->getAutoCalc();
    oldShowGrid = newShowGrid = sheet->getShowGrid();
    oldShowPageBorders = newShowPageBorders = sheet->isShowPageBorders();
    oldShowFormula = newShowFormula = sheet->getShowFormula();
    oldHideZero = newHideZero = sheet->getHideZero();
    oldShowFormulaIndicator = newShowFormulaIndicator = sheet->getShowFormulaIndicator();
    oldShowCommentIndicator = newShowCommentIndicator = sheet->getShowCommentIndicator();
    oldColumnAsNumber = newColumnAsNumber = sheet->getShowColumnNumber();
    oldLcMode = newLcMode = sheet->getLcMode();
    oldCapitalizeFirstLetter = newCapitalizeFirstLetter = sheet->getFirstLetterUpper();
}

QString SheetPropertiesCommand::name() const
{
    return i18n("Change Sheet Properties");
}

void SheetPropertiesCommand::setLayoutDirection( Sheet::LayoutDirection dir )
{
    newDirection = dir;
}

void SheetPropertiesCommand::setAutoCalc( bool b )
{
    newAutoCalc = b;
}

void SheetPropertiesCommand::setShowGrid( bool b )
{
    newShowGrid = b;
}

void SheetPropertiesCommand::setShowPageBorders( bool b )
{
    newShowPageBorders = b;
}

void SheetPropertiesCommand::setShowFormula( bool b )
{
    newShowFormula = b;
}

void SheetPropertiesCommand::setHideZero( bool b  )
{
    newHideZero = b;
}

void SheetPropertiesCommand::setShowFormulaIndicator( bool b )
{
    newShowFormulaIndicator = b;
}

void SheetPropertiesCommand::setShowCommentIndicator( bool b )
{
  newShowCommentIndicator = b;
}

void SheetPropertiesCommand::setColumnAsNumber( bool b  )
{
    newColumnAsNumber = b;
}

void SheetPropertiesCommand::setLcMode( bool b  )
{
    newLcMode = b;
}

void SheetPropertiesCommand::setCapitalizeFirstLetter( bool b )
{
    newCapitalizeFirstLetter = b;
}

void SheetPropertiesCommand::execute()
{
    sheet->setLayoutDirection( newDirection );
    sheet->setAutoCalc( newAutoCalc );
    sheet->setShowGrid( newShowGrid );
    sheet->setShowPageBorders( newShowPageBorders );
    sheet->setShowFormula( newShowFormula );
    sheet->setHideZero( newHideZero );
    sheet->setShowFormulaIndicator( newShowFormulaIndicator );
    sheet->setShowCommentIndicator( newShowCommentIndicator );
    sheet->setShowColumnNumber( newColumnAsNumber );
    sheet->setLcMode( newLcMode );
    sheet->setFirstLetterUpper( newCapitalizeFirstLetter );
    doc->addDamage( new SheetDamage( sheet, SheetDamage::PropertiesChanged ) );
}

void SheetPropertiesCommand::unexecute()
{
    sheet->setLayoutDirection( oldDirection );
    sheet->setAutoCalc( oldAutoCalc );
    sheet->setShowGrid( oldShowGrid );
    sheet->setShowPageBorders( oldShowPageBorders );
    sheet->setShowFormula( oldShowFormula );
    sheet->setHideZero( oldHideZero );
    sheet->setShowFormulaIndicator( oldShowFormulaIndicator );
    sheet->setShowCommentIndicator( oldShowCommentIndicator );
    sheet->setShowColumnNumber( oldColumnAsNumber );
    sheet->setLcMode( oldLcMode );
    sheet->setFirstLetterUpper( oldCapitalizeFirstLetter );
    doc->addDamage( new SheetDamage( sheet, SheetDamage::PropertiesChanged ) );
}


// ----- InsertColumnCommand -----

InsertColumnCommand::InsertColumnCommand( Sheet* s , unsigned int _column, unsigned int _nbCol )
{
  doc = s->doc();
  sheetName = s->sheetName();
  insertPosColumn = _column;
  nbColumnInserted = _nbCol;
}

void InsertColumnCommand::execute()
{
    Sheet* sheet = doc->map()->findSheet( sheetName );
    if( !sheet ) return;
    sheet->insertColumn( insertPosColumn,nbColumnInserted);
}

void InsertColumnCommand::unexecute()
{
    Sheet* sheet = doc->map()->findSheet( sheetName );
    if( !sheet ) return;
    sheet->removeColumn( insertPosColumn,nbColumnInserted );
}

QString InsertColumnCommand::name() const
{
    return i18n("Insert Columns");
}


// ----- DefinePrintRangeCommand -----


DefinePrintRangeCommand::DefinePrintRangeCommand( Sheet *s )
{
  doc = s->doc();
  sheetName = s->sheetName();
  printRange = s->print()->printRange();
}

void DefinePrintRangeCommand::execute()
{
    Sheet* sheet = doc->map()->findSheet( sheetName );
    if( !sheet ) return;
    sheet->print()->setPrintRange( printRangeRedo );

}

void DefinePrintRangeCommand::unexecute()
{
    Sheet* sheet = doc->map()->findSheet( sheetName );
    if( !sheet ) return;
    printRangeRedo = sheet->print()->printRange();
    sheet->print()->setPrintRange( printRange );
}

QString DefinePrintRangeCommand::name() const
{
    return i18n("Set Page Layout");
}

// ----- PaperLayoutCommand -----


PaperLayoutCommand::PaperLayoutCommand( Sheet *s )
{
  doc = s->doc();
  sheetName = s->sheetName();
  pl = s->print()->paperLayout();
  hf = s->print()->headFootLine();
  unit = doc->unit();
  printGrid = s->print()->printGrid();
  printCommentIndicator = s->print()->printCommentIndicator();
  printFormulaIndicator = s->print()->printFormulaIndicator();
  printRange = s->print()->printRange();
  printRepeatColumns = s->print()->printRepeatColumns();
  printRepeatRows = s->print()->printRepeatRows();
  zoom = s->print()->zoom();
  pageLimitX = s->print()->pageLimitX();
  pageLimitY = s->print()->pageLimitY();
}

void PaperLayoutCommand::execute()
{
    Sheet* sheet = doc->map()->findSheet( sheetName );
    if( !sheet ) return;
    SheetPrint* print = sheet->print();

    print->setPaperLayout( plRedo.ptLeft,  plRedo.ptTop,
                           plRedo.ptRight, plRedo.ptBottom,
                           plRedo.format, plRedo.orientation );

    print->setHeadFootLine( hfRedo.headLeft, hfRedo.headMid, hfRedo.headRight,
                            hfRedo.footLeft, hfRedo.footMid, hfRedo.footRight );

    doc->setUnit( unitRedo );

    print->setPrintGrid( printGridRedo );
    print->setPrintCommentIndicator( printCommentIndicatorRedo );
    print->setPrintFormulaIndicator( printFormulaIndicatorRedo );

    print->setPrintRange( printRangeRedo );
    print->setPrintRepeatColumns( printRepeatColumnsRedo );
    print->setPrintRepeatRows( printRepeatRowsRedo );

    print->setZoom( zoomRedo );

    print->setPageLimitX( pageLimitX );
    print->setPageLimitY( pageLimitY );
}

void PaperLayoutCommand::unexecute()
{
    Sheet* sheet = doc->map()->findSheet( sheetName );
    if( !sheet ) return;
    SheetPrint* print = sheet->print();
    plRedo = print->paperLayout();
    print->setPaperLayout( pl.ptLeft,  pl.ptTop,
                           pl.ptRight, pl.ptBottom,
                           pl.format,  pl.orientation );

    hfRedo = print->headFootLine();
    print->setHeadFootLine( hf.headLeft, hf.headMid, hf.headRight,
                            hf.footLeft, hf.footMid, hf.footRight );

    unitRedo = doc->unit();
    doc->setUnit( unit );

    printGridRedo = print->printGrid();
    print->setPrintGrid( printGrid );

    printCommentIndicatorRedo = print->printCommentIndicator();
    print->setPrintCommentIndicator( printCommentIndicator );

    printFormulaIndicatorRedo = print->printFormulaIndicator();
    print->setPrintFormulaIndicator( printFormulaIndicator );

    printRangeRedo = print->printRange();
    print->setPrintRange( printRange );

    printRepeatColumnsRedo = print->printRepeatColumns();
    print->setPrintRepeatColumns( printRepeatColumns );

    printRepeatRowsRedo = print->printRepeatRows();
    print->setPrintRepeatRows( printRepeatRows );

    zoomRedo = print->zoom();
    print->setZoom( zoom );

    pageLimitXRedo = print->pageLimitX();
    print->setPageLimitX( pageLimitX );

    pageLimitYRedo = print->pageLimitY();
    print->setPageLimitY( pageLimitY );

}

QString PaperLayoutCommand::name() const
{
    return i18n("Set Page Layout");
}

LinkCommand::LinkCommand( Cell* c, const QString& text, const QString& link )
{
  cell = c;
  oldText = cell->text();
  oldLink = cell->link();
  newText = text;
  newLink = link;

  Sheet* s = cell->sheet();
  if( s ) doc = s->doc();
}

void LinkCommand::execute()
{
  if( !cell ) return;

  if( !newText.isEmpty() )
    cell->setCellText( newText );
  cell->setLink( newLink  );

  doc->addDamage( new CellDamage( cell ) );
}

void LinkCommand::unexecute()
{
  if( !cell ) return;

  cell->setCellText( oldText );
  cell->setLink( oldLink );

  doc->addDamage( new CellDamage( cell ) );
}

QString LinkCommand::name() const
{
  return newLink.isEmpty() ? i18n("Remove Link") : i18n("Set Link");
}

ChangeObjectGeometryCommand::ChangeObjectGeometryCommand( EmbeddedObject *_obj, const KoPoint &_m_diff, const KoSize &_r_diff )
  : m_diff( _m_diff ), r_diff( _r_diff )
{
  obj = _obj;
  obj->incCmdRef();
  doc = obj->sheet()->doc();
}

ChangeObjectGeometryCommand::~ChangeObjectGeometryCommand()
{
  obj->decCmdRef();
}

void ChangeObjectGeometryCommand::execute()
{
    doc->repaint( obj->geometry() );

    KoRect geometry = obj->geometry();
    geometry.moveBy( m_diff.x(),  m_diff.y() );
    geometry.setWidth( geometry.width() + r_diff.width() );
    geometry.setHeight( geometry.height() + r_diff.height() );
    obj->setGeometry( geometry );

//     if ( object->isSelected())
//       doc->updateObjectStatusBarItem();
    doc->repaint( obj );
}

void ChangeObjectGeometryCommand::unexecute()
{
  doc->repaint( obj->geometry() );

  KoRect geometry = obj->geometry();
  geometry.moveBy( -m_diff.x(),  -m_diff.y() );
  geometry.setWidth( geometry.width() - r_diff.width() );
  geometry.setHeight( geometry.height() - r_diff.height() );
  obj->setGeometry( geometry );

//     if ( object->isSelected())
//       doc->updateObjectStatusBarItem();
  doc->repaint( obj );
}

QString ChangeObjectGeometryCommand::name() const
{
  /*if ( fabs( obj->geometry().width() - newGeometry.width() )<1e-3  && fabs( obj->geometry().height() - newGeometry.height() ) < 1e-3 )
    return i18n("Move Object");
  else */
    return i18n("Resize Object");
}

RemoveObjectCommand::RemoveObjectCommand( EmbeddedObject *_obj, bool _cut )
{
  obj = _obj;
  cut = _cut;
  doc = obj->sheet()->doc();
}

RemoveObjectCommand::~RemoveObjectCommand()
{
  if ( !executed )
    return;
  //kDebug() << "*********Deleting object..." << endl;
  if ( obj->getType() == OBJECT_CHART )
  {
    EmbeddedKOfficeObject *chart = dynamic_cast<EmbeddedKOfficeObject *>(obj);
    chart->embeddedObject()->setDeleted(true);
  }

  delete obj;
}

void RemoveObjectCommand::execute()
{

//  I don't think we need this:
//       Make sure that this child has no active embedded view -> activate ourselfs
//       doc()->emitBeginOperation( false );
//       partManager()->setActivePart( koDocument(), this );
//       partManager()->setSelectedPart( 0 );
//       doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );

  doc->embeddedObjects().removeAll( obj );
  if ( obj->getType() == OBJECT_CHART ||  obj->getType()== OBJECT_KOFFICE_PART)
  {
    EmbeddedKOfficeObject *eko = dynamic_cast<EmbeddedKOfficeObject *>(obj);
    eko->embeddedObject()->setDeleted(true);
  }

  obj->setSelected( false );
  doc->repaint( obj );
  executed = true;
}

void RemoveObjectCommand::unexecute()
{
  doc->embeddedObjects().append( obj );
  if ( obj->getType() == OBJECT_CHART ||  obj->getType()== OBJECT_KOFFICE_PART)
  {
    EmbeddedKOfficeObject *eko = dynamic_cast<EmbeddedKOfficeObject *>(obj);
    eko->embeddedObject()->setDeleted(false);
  }
  doc->repaint( obj );
  executed = false;
}

QString RemoveObjectCommand::name() const
{
  if ( cut )
    return i18n("Cut Object");
  else
    return i18n("Remove Object");
}

InsertObjectCommand::InsertObjectCommand( const KoRect& _geometry, KoDocumentEntry& _entry, Canvas *_canvas ) //child
{
  geometry = _geometry;
  entry = _entry;
  canvas = _canvas;
  type = OBJECT_KOFFICE_PART;
  obj = 0;
}

InsertObjectCommand::InsertObjectCommand(const KoRect& _geometry, KoDocumentEntry& _entry, const QRect& _data, Canvas *_canvas ) //chart
{
  geometry = _geometry;
  entry = _entry;
  data = _data;
  canvas = _canvas;
  type = OBJECT_CHART;
  obj = 0;
}

InsertObjectCommand::InsertObjectCommand( const KoRect& _geometry , KUrl& _file, Canvas *_canvas ) //picture
{
  //In the case of pictures, only the top left point of the rectangle is relevant
  geometry = _geometry;
  file = _file;
  canvas = _canvas;
  type = OBJECT_PICTURE;
  obj = 0;
}

InsertObjectCommand::~InsertObjectCommand()
{
  if ( executed )
    return;
  //kDebug() << "*********Deleting object..." << endl;
  if ( obj->getType() == OBJECT_CHART )
  {
    EmbeddedKOfficeObject *chart = dynamic_cast<EmbeddedKOfficeObject *>(obj);
    chart->embeddedObject()->setDeleted(true);
  }

  delete obj;
}

void InsertObjectCommand::execute()
{
  if ( obj ) //restore the object which was removed from the object list in InsertObjectCommand::unexecute()
  {
    canvas->doc()->embeddedObjects().append( obj );
    canvas->doc()->repaint( obj );
  }
  else
  {
    bool success = false;
    switch ( type )
    {
      case OBJECT_CHART:
      {
        success = canvas->activeSheet()->insertChart( geometry, entry, data, canvas );
        break;
      }
      case OBJECT_KOFFICE_PART:
      {
        success = canvas->activeSheet()->insertChild( geometry, entry, canvas );
        break;
      }
      case OBJECT_PICTURE:
      {
        success = canvas->activeSheet()->insertPicture( geometry.topLeft(), file );
        break;
      }
      default:
        break;
    }
    if ( success )
    {
      obj = canvas->doc()->embeddedObjects().last();
      obj->sheet()->unifyObjectName( obj );
    }
    else
      obj = 0;
  }
  executed = true;
}

void InsertObjectCommand::unexecute()
{
  if ( !obj )
    return;

  canvas->doc()->embeddedObjects().removeAll( obj );
  obj->setSelected( false );
  canvas->doc()->repaint( obj );

  executed = false;
}

QString InsertObjectCommand::name() const
{
  return i18n("Insert Object");
}

RenameNameObjectCommand::RenameNameObjectCommand( const QString &_name, const QString &_objectName,
                                            EmbeddedObject *_obj, Doc *_doc ):
    KNamedCommand( _name ),
    newObjectName( _objectName ),
    object( _obj ),
    doc( _doc )
{
    oldObjectName = object->getObjectName();

    m_page = object->sheet()/*doc->findPage( object )*/;
}

RenameNameObjectCommand::~RenameNameObjectCommand()
{
}

void RenameNameObjectCommand::execute()
{
    object->setObjectName( newObjectName );
    m_page->unifyObjectName( object );

//     doc->updateSideBarItem( m_page );
}

void RenameNameObjectCommand::unexecute()
{
    object->setObjectName( oldObjectName );

//     doc->updateSideBarItem( m_page );
}

GeometryPropertiesCommand::GeometryPropertiesCommand( const QString &name, QList<EmbeddedObject*> &objects,
                                                            bool newValue, KgpType type, Doc *_doc )
: KNamedCommand( name )
, m_objects( objects )
, m_newValue( newValue )
, m_type( type )
    , m_doc( _doc )
{
    foreach ( EmbeddedObject* object, m_objects )
    {
        object->incCmdRef();
        if ( m_type == ProtectSize )
            m_oldValue.append( object->isProtect() );
        else if ( m_type == KeepRatio)
            m_oldValue.append( object->isKeepRatio() );
    }
}

GeometryPropertiesCommand::GeometryPropertiesCommand( const QString &name, QList<bool> &lst,
                                                            QList<EmbeddedObject*> &objects, bool newValue,
                                                            KgpType type, Doc *_doc)
: KNamedCommand( name )
, m_oldValue( lst )
, m_objects( objects )
, m_newValue( newValue )
, m_type( type )
, m_doc ( _doc )
{
    foreach ( EmbeddedObject* object, m_objects )
      object->incCmdRef();
}

GeometryPropertiesCommand::~GeometryPropertiesCommand()
{
    foreach ( EmbeddedObject* object, m_objects )
        object->decCmdRef();
}

void GeometryPropertiesCommand::execute()
{
    foreach ( EmbeddedObject* object, m_objects )
    {
        if ( m_type == ProtectSize )
        {
            object->setProtect( m_newValue );
            if ( object->isSelected() )
                m_doc->repaint( object );
        }
        else if ( m_type == KeepRatio)
            object->setKeepRatio( m_newValue );
    }
}

void GeometryPropertiesCommand::unexecute()
{
    EmbeddedObject *obj = 0;
    for ( int i = 0; i < m_objects.count(); ++i ) {
        obj = m_objects.at( i );
        if ( m_type == ProtectSize )
        {
            obj->setProtect( m_oldValue.at(i) );
            if ( obj->isSelected() )
                m_doc->repaint( obj );
        }
        else if ( m_type == KeepRatio)
            obj->setKeepRatio( m_oldValue.at(i) );
    }
}

MoveObjectByCmd::MoveObjectByCmd( const QString &_name, const KoPoint &_diff,
                                  QList<EmbeddedObject*> &_objects,
                                  Doc *_doc, Sheet *_page )
    : KNamedCommand( _name ), diff( _diff ), objects( _objects )
{
    doc = _doc;
    m_page=_page;
    foreach ( EmbeddedObject* object, objects )
    {
        object->incCmdRef();
    }
}

MoveObjectByCmd::~MoveObjectByCmd()
{
    foreach ( EmbeddedObject* object, objects )
        object->decCmdRef();
}

void MoveObjectByCmd::execute()
{
    QRect oldRect;

    for ( int i = 0; i < objects.count(); i++ ) {
        doc->repaint( objects.at( i )->geometry() );

        KoRect r = objects.at( i )->geometry();
        r.moveBy( diff.x(), diff.y() );
        objects.at( i )->setGeometry( r );

        doc->repaint( objects.at( i ) );
    }
}

void MoveObjectByCmd::unexecute()
{
    QRect oldRect;

    for ( int i = 0; i < objects.count(); i++ ) {
        doc->repaint( objects.at( i )->geometry() );

        KoRect r = objects.at( i )->geometry();
        r.moveBy( -diff.x(), -diff.y() );
        objects.at( i )->setGeometry( r );

        doc->repaint( objects.at( i ) );
    }
}
