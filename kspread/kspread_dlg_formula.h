/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Montel Laurent <montell@club-internet.fr>

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
#include <qdialog.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <klineedit.h>
#include <kcompletion.h>

class QTextBrowser;
class QTabWidget;

class KSpreadView;
class KSpreadTable;
class KSpreadCell;

enum KSpreadParameterType { type_double, type_string, type_logic, type_int };

struct KSpreadFunctionDescription
{
  int nb_param;
  QString firstElementLabel;
  KSpreadParameterType firstElementType;

  QString secondElementLabel;
  KSpreadParameterType secondElementType;

  QString thirdElementLabel;
  KSpreadParameterType thirdElementType;

  QString fourElementLabel;
  KSpreadParameterType fourElementType;

  QString fiveElementLabel;
  KSpreadParameterType fiveElementType;
  bool multiple;
  QString help;
};

class KSpreadDlgFormula : public QDialog
{
    Q_OBJECT
public:
    KSpreadDlgFormula( KSpreadView* parent, const char* name,const QString& formulaName=0);
    
private:
    void changeFunction();
    
    /**
     * Turns the @p text into a parameter of the desired type. The returned string is
     * of a form that can be understood by kscript.
     */
    QString createParameter( const QString& _text,KSpreadParameterType elementType );
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
     * table changes.
     */
    void slotSelectionChanged( KSpreadTable* _table, const QRect& _selection );
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
    
private:
    KSpreadView* m_pView;
    QPushButton* m_pOk;
    QPushButton* m_pClose;
    
    QTabWidget* m_tabwidget;
    QTextBrowser* m_browser;
    QWidget* m_input;
    
    QPushButton *selectFunction;
    QComboBox *typeFunction;
    QListBox *functions;
    QLineEdit *result;

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
  QString m_tableName;
  KSpreadFunctionDescription funct;

  QString m_rightText;
  QString m_leftText;
    /**
     * A lock for @ref #slotChangeText.
     */
    bool refresh_result;

  KLineEdit *searchFunct;
  KCompletion listFunct;
};

#endif
