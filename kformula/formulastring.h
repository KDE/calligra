/* This file is part of the KDE project
   Copyright (C) 2002 Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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

#ifndef FORMULASTRING_H
#define FORMULASTRING_H

#include <QDialog>

class QLabel;
class QPushButton;
class QTextEdit;
class QVBoxLayout;
class QHBoxLayout;

class KFormulaPartView;

class FormulaString : public QDialog
{
    Q_OBJECT

  public:
    /** The constructor */
    FormulaString( KFormulaPartView* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );

    /** The destructor */
    ~FormulaString();

    /**
     * Set the @p text to be displayed in m_textEdit
     * @param text the text to be displayed
     */
    void setEditText( const QString& text );

  protected slots:
    virtual void accept ();
    void helpButtonClicked();
    void cursorPositionChanged();

  private:
    KFormulaPartView* view;
    QTextEdit* m_textEdit;
    QVBoxLayout* m_widgetLayout;
    QHBoxLayout* m_btnLayout;
    QPushButton* m_btnHelp;
    QPushButton* m_btnOk;
    QPushButton* m_btnCancel;
    QLabel* m_position;
};

#endif // FORMULASTRING_H
