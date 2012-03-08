/* This file is part of the KDE project
   Copyright 2010 Gopalakrishna Bhat A <gopalakbhat@gmail.com>

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
#ifndef FO_CELLEDITOR_H
#define FO_CELLEDITOR_H

#include <QTextEdit>

#include <sheets/ui/CellEditorBase.h>

class QString;
class QFont;
class KoViewConverter;

namespace Calligra { namespace Tables
{
    class CellToolBase;
    class Selection;
} }

class FoCellEditor : public QTextEdit, public Calligra::Tables::CellEditorBase
{
    Q_OBJECT

public:
    explicit FoCellEditor(Calligra::Tables::CellToolBase *cellTool, QWidget *parent=0);
    ~FoCellEditor();

    QWidget* widget() { return this; }
    void selectionChanged();
    void setEditorFont(QFont const & font, bool updateSize, const KoViewConverter *viewConverter);
    void setText(const QString& text, int cursorPos = -1);
    QString toPlainText() const;
    void setCursorPosition(int pos);
    int cursorPosition() const;
    void cut() { return QTextEdit::cut(); }
    void copy() { return QTextEdit::copy(); }
    void paste() { return QTextEdit::paste(); }
    void setActiveSubRegion(int index);
    virtual void focusInEvent(QFocusEvent *event);


signals:
    void textChanged(const QString &);

public slots:
    void slotTextChanged();
    void slotCursorPositionChanged();

protected:
    virtual void keyPressEvent(QKeyEvent *event);

private:
    Calligra::Tables::Selection *m_selection;
    QTextEdit *m_editor;
    Calligra::Tables::CellToolBase *m_cellTool;
};

#endif // FO_CELLEDITOR_H
