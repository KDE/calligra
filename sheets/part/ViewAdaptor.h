/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006 Fredrik Edemar <f_edemar@linux.se>
   SPDX-FileCopyrightText: 2002-2003 Joseph Wenninger <jowenn@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 2001-2002 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_VIEW_ADAPTOR
#define CALLIGRA_SHEETS_VIEW_ADAPTOR

#ifdef WITH_QTDBUS
#include "sheets_part_export.h"
#include <QColor>
#include <QDBusAbstractAdaptor>
#include <QRect>
#include <QString>

namespace Calligra
{
namespace Sheets
{
class View;
class CellProxy;

/**
 * The ViewAdaptor class provides access to a view on a Calligra Sheets document.
 */
class CALLIGRA_SHEETS_PART_EXPORT ViewAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.calligra.spreadsheet.view")
public:
    explicit ViewAdaptor(View *);
    ~ViewAdaptor() override;

public Q_SLOTS: // METHODS

    /** Return the name of the document the view looks on. */
    virtual QString doc() const;
    /** Return the name of the map for the document. An map offers
    additional functionality to deal with the content of a document. */
    virtual QString map() const;
    /** Return the name of the active sheet. */
    virtual QString sheet() const;

    virtual void changeNbOfRecentFiles(int _nb);

    /** Hide the view. */
    virtual void hide();
    /** Show the view. */
    virtual void show();

    /** Set the range of cells that should be selected. */
    virtual void setSelection(const QRect &selection);
    /** Return the range of cells that is selected. */
    virtual QRect selection();

    virtual void preference();
    /** Select the next sheet as active sheet. */
    virtual void nextSheet();
    /** Select the previous sheet as active sheet. */
    virtual void previousSheet();
    /** Select the sheet with name \p sheetName as active sheet. */
    virtual bool showSheet(const QString &sheetName);
    /** Copy the content of the range of cells that is selected. */
    virtual void copyAsText();

    /** Set the text color of all selected cells to the defined color value. */
    virtual void setSelectionTextColor(const QColor &txtColor);
    /** Set the background color of all selected cells to the defined color value. */
    virtual void setSelectionBgColor(const QColor &bgColor);
    /** Set the border color of all selected cells to the defined color value. */
    virtual void setSelectionBorderColor(const QColor &bdColor);

    /** Set the color of the left border. */
    virtual void setLeftBorderColor(const QColor &color);
    /** Set the color of the top border. */
    virtual void setTopBorderColor(const QColor &color);
    /** Set the color of the right border. */
    virtual void setRightBorderColor(const QColor &color);
    /** Set the color of the bottom border. */
    virtual void setBottomBorderColor(const QColor &color);
    /** Set the color of the all borders. */
    virtual void setAllBorderColor(const QColor &color);
    /** Set the color of the outline border. */
    virtual void setOutlineBorderColor(const QColor &color);

Q_SIGNALS:

    /** This signal got emitted if this view got activated or deactivated. */
    void activated(bool active);

    /** This signal got emitted if this view got selected or unselected. */
    void selected(bool select);

    /** This signal got emitted if the selection changed. */
    void selectionChanged();

private:
    View *m_view;
    CellProxy *m_proxy;
};

} // namespace Sheets
} // namespace Calligra
#endif // WITH_QTDBUS

#endif // CALLIGRA_SHEETS_VIEW_ADAPTOR
