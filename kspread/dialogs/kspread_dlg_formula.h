/* This file is part of the KDE project
   Copyright (C) 2002-2003 Ariya Hidayat <ariya@kde.org>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 1999-2003 Laurent Montel <montel@kde.org>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2002 Daniel Herring <herring@eecs.ku.edu>
             (C) 2000-2001 Werner Trobin <trobin@kde.org>
             (C) 1998-2000 Torben Weis <weis@kde.org>

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

#ifndef __kspread_dlg_formula__
#define __kspread_dlg_formula__

#include <q3frame.h>
//Added by qt3to4:
#include <QLabel>
#include <QEvent>
#include <QCloseEvent>
#include <Q3ListBoxItem>
#include <kcompletion.h>
#include <kdialogbase.h>

class QComboBox;
class QLabel;
class QLineEdit;
class Q3ListBox;
class QPushButton;
class QTabWidget;
class Q3TextBrowser;
class KLineEdit;

namespace KSpread
{
class FunctionDescription;
class Sheet;
class View;

class FormulaDialog : public KDialogBase
{
    Q_OBJECT
public:
  FormulaDialog( View* parent, const char* name,const QString& formulaName=0);
  ~FormulaDialog();
private:
    /**
     * Turns the @p text into a parameter that koscript can understand. The type
     * of this parameter is extracted by looking at parameter number @p param in @ref #m_desc.
     */
    QString createParameter( const QString& _text, int param );
    /**
     * Reads the text out of @ref #firstElement and friends and creates a parameter
     * list for the function.
     */
    QString createFormula();

private slots:
    /**
     * Called by the Ok button.
     */
    void slotOk();
    /**
     * Called by the Close button.
     */
    void slotClose();
    /**
     * Called if a function name was selected but not double clicked.
     * This will just show the help page for the function.
     */
    void slotSelected( const QString& function );
    /**
     * Called if the user clicked on one of the "related function"
     * This will switch the active function and show help page
     * of the function as well.
     */
    void slotShowFunction( const QString& function );
    /**
     * Called if the user double clicked on some method name.
     * That will switch into editing mode, allowing the user
     * to enter the parameters for the function.
     */
    void slotDoubleClicked( Q3ListBoxItem* item );
    /**
     * Called if a category of methods has been selected.
     */
    void slotActivated(const QString& category );
    /**
     * Called if the text of @ref #firstElement, @ref #secondElement etc. changes.
     */
    void slotChangeText(const QString& text );
    /**
     * Connected to @ref View to get notified if the selection in the
     * sheet changes.
     */
    void slotSelectionChanged();
    /**
     * Called if the button @ref #selectFunction was clicked. That
     * insertes a new function call to the result.
     */
    void slotSelectButton();
    /**
     * Called if the user changes some character in @ref #searchFunct.
     */
    void slotSearchText(const QString& text );
    /**
     * Called if the user pressed return in @ref #searchFunct.
     */
    void slotPressReturn();

public:
    /**
     * Find out which widget got focus.
     */
    bool eventFilter( QObject* obj, QEvent* ev );
protected:
       virtual void closeEvent ( QCloseEvent * );
private:
    View* m_pView;

    QTabWidget* m_tabwidget;
    Q3TextBrowser* m_browser;
    QWidget* m_input;

    QPushButton *selectFunction;
    QComboBox *typeFunction;
    Q3ListBox *functions;
    QLineEdit *result;

    KLineEdit *searchFunct;
    KCompletion listFunct;

    QLabel* label1;
    QLabel* label2;
    QLabel* label3;
    QLabel* label4;
    QLabel* label5;
    QLineEdit *firstElement;
    QLineEdit *secondElement;
    QLineEdit *thirdElement;
    QLineEdit *fourElement;
    QLineEdit *fiveElement;
    /**
     * Tells which of the lineedits has the logical focus currently.
     * It may happen that a lineedit does not have qt focus but
     * logical focus but not the other way round.
     */
    QLineEdit* m_focus;

    int m_column;
    int m_row;
    QString m_oldText;

    QString m_funcName;
    QString m_sheetName;

    QString m_rightText;
    QString m_leftText;
    /**
     * A lock for @ref #slotChangeText.
     */
    bool refresh_result;

    KSpread::FunctionDescription* m_desc;
};

} // namespace KSpread

#endif
