/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef INSERTINLINEOBJECTACTIONBASE_H
#define INSERTINLINEOBJECTACTIONBASE_H

#include <QAction>

class KoCanvasBase;
class KoInlineObject;

/**
 * helper class
 */
class InsertInlineObjectActionBase : public QAction
{
    Q_OBJECT
public:
    InsertInlineObjectActionBase(KoCanvasBase *canvas, const QString &name);
    ~InsertInlineObjectActionBase() override;

private Q_SLOTS:
    void activated();

protected:
    virtual KoInlineObject *createInlineObject() = 0;

    KoCanvasBase *m_canvas;
};

#endif
