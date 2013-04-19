/* This file is part of the KDE project
 *
 * Copyright (C) 2013 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
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

#ifndef FORMULA_TOOL
#define FORMULA_TOOL

#include <KoToolBase.h>

#include <QTimer>

class QTextEdit;
class FormulaShape;
class FormulaCursor;

class FormulaTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit FormulaTool(KoCanvasBase* canvas);

    /// reimplemented from KoToolBase
    virtual void paint(QPainter&, const KoViewConverter&);
    /// reimplemented from KoToolBase
    virtual void mousePressEvent(KoPointerEvent*);
    /// reimplemented from superclass
    virtual void mouseDoubleClickEvent(KoPointerEvent *event);
    /// reimplemented from KoToolBase
    virtual void mouseMoveEvent(KoPointerEvent*) {}
    /// reimplemented from KoToolBase
    virtual void mouseReleaseEvent(KoPointerEvent*) {}
    
    /// reimplemented from superclass
    virtual void keyPressEvent(QKeyEvent *event);

    /// reimplemented from KoToolBase
    virtual void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes);
    /// reimplemented from KoToolBase
    virtual void deactivate();

protected:
    /// reimplemented from KoToolBase
    virtual QWidget *createOptionWidget();

signals:
    void formulaContentChanged(const QString &content);

private slots:
    void insertPressed();

private:
    // Pointer to the shape that we manipulate.
    FormulaShape *m_formulaShape;
    QTextEdit *m_textEdit;

    FormulaCursor *cursor();
};

#endif
