/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREADAPPLICATIONSETTINGS
#define KSPREADAPPLICATIONSETTINGS

#include <KCompletion>

#include <QColor>
#include <QObject>

#include "Global.h"
#include "sheets_odf_export.h"

namespace Calligra
{
namespace Sheets
{

/**
 * Visual settings.
 */
class CALLIGRA_SHEETS_ODF_EXPORT ApplicationSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool showVerticalScrollBar READ showVerticalScrollBar WRITE setShowVerticalScrollBar)
    Q_PROPERTY(bool showHorizontalScrollBar READ showHorizontalScrollBar WRITE setShowHorizontalScrollBar)
    Q_PROPERTY(bool showColumnHeader READ showColumnHeader WRITE setShowColumnHeader)
    Q_PROPERTY(bool showRowHeader READ showRowHeader WRITE setShowRowHeader)
    Q_PROPERTY(bool showStatusBar READ showStatusBar WRITE setShowStatusBar)
    Q_PROPERTY(bool showTabBar READ showTabBar WRITE setShowTabBar)

public:
    /**
     * Constructor.
     */
    ApplicationSettings();

    /**
     * Destructor.
     */
    ~ApplicationSettings() override;

    void load();
    void save() const;

    /**
     * If \c enable is true, vertical scrollbar is visible, otherwise
     * it will be hidden.
     */
    void setShowVerticalScrollBar(bool enable);

    /**
     * Returns true if vertical scroll bar is visible.
     */
    bool showVerticalScrollBar() const;

    /**
     * If \c enable is true, horizontal scrollbar is visible, otherwise
     * it will be hidden.
     */
    void setShowHorizontalScrollBar(bool enable);

    /**
     * Returns true if horizontal scroll bar is visible.
     */
    bool showHorizontalScrollBar() const;

    /**
     * If \c enable is true, column header is visible, otherwise
     * it will be hidden.
     */
    void setShowColumnHeader(bool enable);

    /**
     * Returns true if column header is visible.
     */
    bool showColumnHeader() const;

    /**
     * If \c enable is true, row header is visible, otherwise
     * it will be hidden.
     */
    void setShowRowHeader(bool enable);

    /**
     * Returns true if row header is visible.
     */
    bool showRowHeader() const;

    /**
     * Sets the color of the grid.
     */
    void setGridColor(const QColor& color);

    /**
     * Returns the color of the grid.
     */
    QColor gridColor() const;

    /**
     * Sets the indentation value.
     */
    void setIndentValue(double val);

    /**
     * Returns the indentation value.
     */
    double indentValue() const;

    /**
     * If \c enable is true, status bar is visible, otherwise
     * it will be hidden.
     */
    void setShowStatusBar(bool enable);

    /**
     * Returns true if status bar is visible.
     */
    bool showStatusBar() const;

    /**
     * If \c enable is true, tab bar is visible, otherwise
     * it will be hidden.
     */
    void setShowTabBar(bool enable);

    /**
     * Returns true if tab bar is visible.
     */
    bool showTabBar() const;

    /**
     * @return completion mode
     */
    KCompletion::CompletionMode completionMode() const;

    /**
     * Sets the completion mode.
     * @param mode the mode to be set
     */
    void setCompletionMode(KCompletion::CompletionMode mode);

    Calligra::Sheets::MoveTo moveToValue() const;
    void setMoveToValue(Calligra::Sheets::MoveTo moveTo);

    /**
     * Method of calc
     */
    void setTypeOfCalc(MethodOfCalc calc);
    MethodOfCalc getTypeOfCalc() const;

    QColor pageOutlineColor() const;
    void changePageOutlineColor(const QColor& color);

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // KSPREADAPPLICATIONSETTINGS
