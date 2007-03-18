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
   Boston, MA 02110-1301, USA.
*/

#include "Canvas.h"
#include "Doc.h"
#include "Map.h"
#include "Object.h"
#include "Sheet.h"

#include "EmbeddedObjectCommands.h"

using namespace KSpread;

ChangeObjectGeometryCommand::ChangeObjectGeometryCommand( EmbeddedObject *_obj, const QPointF &_m_diff, const QSizeF &_r_diff )
  : m_diff( _m_diff ), r_diff( _r_diff )
{
  obj = _obj;
  obj->incCmdRef();
  doc = obj->doc();
}

ChangeObjectGeometryCommand::~ChangeObjectGeometryCommand()
{
  obj->decCmdRef();
}

void ChangeObjectGeometryCommand::redo()
{
    doc->repaint( obj->geometry() );

    QRectF geometry = obj->geometry();
    geometry.translate( m_diff.x(),  m_diff.y() );
    geometry.setWidth( geometry.width() + r_diff.width() );
    geometry.setHeight( geometry.height() + r_diff.height() );
    obj->setGeometry( geometry );

//     if ( object->isSelected())
//       doc->updateObjectStatusBarItem();
    doc->repaint( obj );
}

void ChangeObjectGeometryCommand::undo()
{
  doc->repaint( obj->geometry() );

  QRectF geometry = obj->geometry();
  geometry.translate( -m_diff.x(),  -m_diff.y() );
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
  doc = obj->doc();
}

RemoveObjectCommand::~RemoveObjectCommand()
{
  if ( !executed )
    return;
  //kDebug() << "*********Deleting object..." << endl;
  if ( obj->getType() == OBJECT_CHART )
  {
    EmbeddedKOfficeObject *chart = dynamic_cast<EmbeddedKOfficeObject *>(obj);
    if ( chart )
      chart->embeddedObject()->setDeleted(true);
  }

  delete obj;
}

void RemoveObjectCommand::redo()
{

//  I don't think we need this:
//       Make sure that this child has no active embedded view -> activate ourselfs
//       doc()->emitBeginOperation( false );
//       partManager()->setActivePart( koDocument(), this );
//       partManager()->setSelectedPart( 0 );
//       doc()->emitEndOperation( d->canvas->visibleCells() );

  doc->embeddedObjects().removeAll( obj );
  if ( obj->getType() == OBJECT_CHART ||  obj->getType()== OBJECT_KOFFICE_PART)
  {
    EmbeddedKOfficeObject *eko = dynamic_cast<EmbeddedKOfficeObject *>(obj);
    if ( eko )
      eko->embeddedObject()->setDeleted(true);
  }

  obj->setSelected( false );
  doc->repaint( obj );
  executed = true;
}

void RemoveObjectCommand::undo()
{
  doc->embeddedObjects().append( obj );
  if ( obj->getType() == OBJECT_CHART ||  obj->getType()== OBJECT_KOFFICE_PART)
  {
    EmbeddedKOfficeObject *eko = dynamic_cast<EmbeddedKOfficeObject *>(obj);
    if ( eko )
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

InsertObjectCommand::InsertObjectCommand( const QRectF& _geometry, KoDocumentEntry& _entry, Canvas *_canvas ) //child
{
  geometry = _geometry;
  entry = _entry;
  canvas = _canvas;
  type = OBJECT_KOFFICE_PART;
  obj = 0;
}

InsertObjectCommand::InsertObjectCommand(const QRectF& _geometry, KoDocumentEntry& _entry, const QRect& _data, Canvas *_canvas ) //chart
{
  geometry = _geometry;
  entry = _entry;
  data = _data;
  canvas = _canvas;
  type = OBJECT_CHART;
  obj = 0;
}

InsertObjectCommand::InsertObjectCommand( const QRectF& _geometry , KUrl& _file, Canvas *_canvas ) //picture
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
    if ( chart )
      chart->embeddedObject()->setDeleted(true);
  }

  delete obj;
}

void InsertObjectCommand::redo()
{
  if ( obj ) //restore the object which was removed from the object list in InsertObjectCommand::undo()
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

void InsertObjectCommand::undo()
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
    QUndoCommand( _name ),
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

void RenameNameObjectCommand::redo()
{
    object->setObjectName( newObjectName );
    m_page->unifyObjectName( object );

//     doc->updateSideBarItem( m_page );
}

void RenameNameObjectCommand::undo()
{
    object->setObjectName( oldObjectName );

//     doc->updateSideBarItem( m_page );
}

GeometryPropertiesCommand::GeometryPropertiesCommand( const QString &name, QList<EmbeddedObject*> &objects,
                                                            bool newValue, KgpType type, Doc *_doc )
: QUndoCommand( name )
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
: QUndoCommand( name )
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

void GeometryPropertiesCommand::redo()
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

void GeometryPropertiesCommand::undo()
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

MoveObjectByCmd::MoveObjectByCmd( const QString &_name, const QPointF &_diff,
                                  QList<EmbeddedObject*> &_objects,
                                  Doc *_doc, Sheet *_page )
    : QUndoCommand( _name ), diff( _diff ), objects( _objects )
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

void MoveObjectByCmd::redo()
{
    QRect oldRect;

    for ( int i = 0; i < objects.count(); i++ ) {
        doc->repaint( objects.at( i )->geometry() );

        QRectF r = objects.at( i )->geometry();
        r.translate( diff.x(), diff.y() );
        objects.at( i )->setGeometry( r );

        doc->repaint( objects.at( i ) );
    }
}

void MoveObjectByCmd::undo()
{
    QRect oldRect;

    for ( int i = 0; i < objects.count(); i++ ) {
        doc->repaint( objects.at( i )->geometry() );

        QRectF r = objects.at( i )->geometry();
        r.translate( -diff.x(), -diff.y() );
        objects.at( i )->setGeometry( r );

        doc->repaint( objects.at( i ) );
    }
}
