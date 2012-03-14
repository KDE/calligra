/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

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

#ifndef FOCELL_TOOL
#define FOCELL_TOOL

#include "koabstraction_export.h"

#include <sheets/part/CellTool.h>

#include <QPair>

namespace Calligra { namespace Sheets
{
    class Cell;
    class CellTool;
    class CellEditorBase;
} }

class KoAbstractApplicationController;
class KoExternalEditorInterface;
class FoCellEditor;

class KoCanvasBase;

/**
 * The tool to change cell ranges.
 */
class KOABSTRACTION_EXPORT KoCellTool : public Calligra::Sheets::CellTool
{
    Q_OBJECT

public:
    KoCellTool(KoAbstractApplicationController *controller, KoCanvasBase* canvas);
    ~KoCellTool();

    /*!
     * Function for selecting font size
     */
    void selectFontSize(int size);
    /*!
     * function for selecting font type
     */
    void selectFontType(const QString& font);
    /*!
     * Function for selecting text foreground color
     */
    void selectTextColor(const QColor& color);
    /*!
     * function for selecting text background color
     */
    void selectTextBackgroundColor(const QColor& color);
    /*!
     * function for retreiving font size
     */
    int getFontSize();
    /*!
     * function for retreiving font type
     */
    QString getFontType();
    /*!
     * function for bold check
     */
    bool isFontBold();
    /*!
     * function for italic check
     */
    bool isFontItalic();
    /*!
     * function for underline check
     */
    bool isFontUnderline();

    // custom cell editor
    virtual Calligra::Sheets::CellEditorBase* editor() const;
    virtual bool createEditor(bool clear=true, bool focus=true);

    virtual void deleteEditor(bool saveChanges, bool expandMatrix = false);

    KoExternalEditorInterface *externalEditor() const;

    virtual void keyPressEvent(QKeyEvent *event);
    virtual void mousePressEvent(KoPointerEvent* event);
    /*!
     * Used to initialize the find operations.
     */
    void initializeFind();
    /*!
     * Returns the current search statistics as
     * QPair<position of current focused cell, total number of matched cells>
     */
    QPair<int,int> currentSearchStatistics();
    /*!
     * Sets the case sensitive option.
     */
    void setCaseSensitive(bool isSensitive);

public slots:
    /*!
     * This is used to highlight a particular cell. This can be used as a goto also
     */
    void slotHighlight(QPoint goToCell);
    void findNext();
    void findPrevious();
    void slotSearchTextChanged(const QString &text);

private slots:
    void setText(const QString& text);

private:
    Q_DISABLE_COPY(KoCellTool)

    KoExternalEditorInterface* m_externalEditor;
    FoCellEditor *m_editor;
    QString m_editorContents;
    QString m_searchString;
    int m_currentfindPosition;
    QList<QPoint> m_matchedPosition;
    bool m_searchCaseSensitive;
    QRect m_findArea;

    int find();
};

#endif // FOCELL_TOOL
