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

#ifndef KSPREAD_EMBEDDED_OBJECT_COMMANDS
#define KSPREAD_EMBEDDED_OBJECT_COMMANDS

#include <QList>
#include <QRect>
#include <QString>
#include <QUndoCommand>

#include <KoQueryTrader.h>

#include "Object.h"
/**
 * The KSpread namespace.
 */
namespace KSpread
{
class Doc;
class EmbeddedObject;

class ChangeObjectGeometryCommand : public QUndoCommand
{
  public:
    ChangeObjectGeometryCommand( EmbeddedObject *_obj, const QPointF &_m_diff, const QSizeF &_r_diff );
    ~ChangeObjectGeometryCommand();

    virtual void redo();
    virtual void undo();
    virtual QString name() const;

  protected:
    QPointF m_diff;
    QSizeF r_diff;
    EmbeddedObject *obj;
    Doc *doc;
};

class RemoveObjectCommand : public QUndoCommand
{
  public:
    explicit RemoveObjectCommand( EmbeddedObject *_obj, bool _cut = false );
    ~RemoveObjectCommand();

    virtual void redo();
    virtual void undo();
    virtual QString name() const;

  protected:
    EmbeddedObject *obj;
    Doc* doc;
    bool executed;
    bool cut;
};

class InsertObjectCommand : public QUndoCommand
{
  public:
    InsertObjectCommand( const QRectF& _geometry, KoDocumentEntry&, Canvas *_canvas ); //child
    InsertObjectCommand( const QRectF& _geometry, KoDocumentEntry&, const QRect& _data, Canvas *_canvas ); //chart
    InsertObjectCommand( const QRectF& _geometry, KUrl& _file, Canvas *_canvas ); //picture
    ~InsertObjectCommand();

    virtual void redo();
    virtual void undo();
    virtual QString name() const;

  protected:
    QRectF geometry;
    Canvas *canvas;
    bool executed;
    KoDocumentEntry entry;
    QRect data;
    ObjType type;
    KUrl file;
    EmbeddedObject *obj;
};

class RenameNameObjectCommand : public QUndoCommand
{
public:
    RenameNameObjectCommand( const QString &_name, const QString &_objectName, EmbeddedObject *_obj, Doc *_doc );
    ~RenameNameObjectCommand();
    void redo();
    void undo();
protected:
    QString oldObjectName, newObjectName;
    EmbeddedObject *object;
    Doc *doc;
    Sheet *m_page;
};


class GeometryPropertiesCommand : public QUndoCommand
{
public:
    enum KgpType { ProtectSize, KeepRatio};
    GeometryPropertiesCommand( const QString &name, QList<EmbeddedObject*> &objects,
                                  bool newValue, KgpType type, Doc *_doc );
    GeometryPropertiesCommand( const QString &name, QList<bool> &lst, QList<EmbeddedObject*> &objects,
                                  bool newValue, KgpType type, Doc *_doc );
    ~GeometryPropertiesCommand();

    virtual void redo();
    virtual void undo();

protected:
    QList<bool> m_oldValue;
    QList<EmbeddedObject*> m_objects;
    bool m_newValue;
    KgpType m_type;
    Doc *m_doc;
};

class MoveObjectByCmd : public QUndoCommand
{
public:
    MoveObjectByCmd( const QString &_name, const QPointF &_diff, QList<EmbeddedObject*> &_objects,
               Doc *_doc, Sheet *m_page );
    ~MoveObjectByCmd();

    virtual void redo();
    virtual void undo();

protected:

    QPointF diff;
    QList<EmbeddedObject*> objects;
    Doc *doc;
    Sheet *m_page;
};

} // namespace KSpread

#endif // KSPREAD_EMBEDDED_OBJECT_COMMANDS
