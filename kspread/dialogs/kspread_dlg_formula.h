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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __kspread_dlg_formula__
#define __kspread_dlg_formula__

#include <qframe.h>
#include <kdialogbase.h>

#include <kcompletion.h>

class QTextBrowser;
class QTabWidget;

class KSpreadView;
class KSpreadSheet;
class KSpreadCell;
class KSpreadFunctionDescription;
class QLineEdit;
class QLabel;
class QListBox;
class QComboBox;
class QPushButton;
class KLineEdit;

class KSpreadDlgFormula : public KDialogBase
{
    Q_OBJECT
public:
    KSpreadDlgFormula( KSpreadView* parent, const char* name,const QString& formulaName=0);
    ~KSpreadDlgFormula();
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
    void slotDoubleClicked( QListBoxItem* item );
    /**
     * Called if a category of methods has been selected.
     */
    void slotActivated(const QString& category );
    /**
     * Called if the text of @ref #firstElement, @ref #secondElement etc. changes.
     */
    void slotChangeText(const QString& text );
    /**
     * Connected to @ref KSpreadView to get notified if the selection in the
     * sheet changes.
     */
    void slotSelectionChanged( KSpreadSheet* _sheet, const QRect& _selection );
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
    KSpreadView* m_pView;

    QTabWidget* m_tabwidget;
    QTextBrowser* m_browser;
    QWidget* m_input;

    QPushButton *selectFunction;
    QComboBox *typeFunction;
    QListBox *functions;
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

    KSpreadFunctionDescription* m_desc;
};

#endif
