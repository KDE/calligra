/* This file is part of the KDE project
 * Copyright (C) 2015-2016 MultiRacio Ltd. <multiracio@multiracio.com> (S.Schliszka, F.Novak, P.Rakyta)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef MCTMAIN_H
#define MCTMAIN_H

#include "MctChangeTypes.h"
#include "MctChangeEntities.h"

#include <QWidget>
#include <QTextCursor>
#include <KoShape.h>

#include "KoGenChange.h"
#include "kundo2magicstring.h"
#include "KoShapeStroke.h"
#include "KoShapeShadow.h"

class MctUndoClass;
class MctRedoClass;
class MctChange;
class MctChangeset;
class MctAbstractGraph;
class MctPosition;

class KWDocument;
class KoTextEditor;
class KoTextDocument;

class MctMain : public QObject
{
    Q_OBJECT
public:
    MctMain(KWDocument *document, QString fileUrl);
    ~MctMain();

    void createRevision(QString author, QString comment);
    void restoreRevision(QString target);
    void updateRedoRevision(int revision);
    void restoreUndoRevision(int revision);
    int undoRevCount();
    int redoRevCount();
    void removeRevision(QString target);
    void clearRevisionHistory();
    MctUndoClass* undoop();
    MctRedoClass* redoop();

    void normailizeChangebuffer();
    QString fileUrl() const;
    void documentSavedAs(QString fileUrl);

    void connectSignals();
    void disconnectSignals();
signals:
    void adjustListOfRevisions();

public slots:
    void createMctChange(QTextCursor &selection, MctChangeTypes changeType, const KUndo2MagicString title, QTextFormat format, QTextFormat prevFormat);
    void addGraphicMctChange(KoShape &selection, MctChangeTypes changeType, const KUndo2MagicString title, QString fileUrl, ChangeAction action);
    void shapeOperationSlot(KoShape *shape, ChangeAction action);
    void createShapeMctChange(QString type, QPointF pos, KoShape &shape, ChangeAction action, QPointF *prevPos = nullptr);
    void createShapePositionChanged(KoShape *selectedShape, QPointF point, QPointF *prevPos);
    void createShapeStyleChanged(QString type, QPointF pos, KoShape &shape, KoShapeStroke *newStroke, KoShapeShadow *newShadow, QPointF *prevPos, QSizeF prevSize, double rotation);

private:    
    KWDocument *m_doc;
    KoTextDocument *m_koTextDoc;
    KoTextEditor *m_editor;

    //QMetaObject::Connection editorConnection;

    MctUndoClass *m_undoop;
    MctRedoClass *m_redoop;

    QString m_fileURL;

    QList<MctChange*> m_changebuffer;

    void setAnchorPosition(MctPosition *anchor, MctPosition *InnerAnchor, MctChangeTypes changeType);
    bool posCheckInTable(MctPosition *lastpos, MctPosition *pos, MctChangeTypes changeType);
    void collectChildren(MctChangeset *change, QList<ulong> *childrenIDs, MctAbstractGraph *graph);
    MctPosition* createPositionInTable(QTextCursor cursor);
    //void correctBlockPositions();
};

#endif // MCTMAIN_H
