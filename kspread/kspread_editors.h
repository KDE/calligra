/* This file is part of the KDE project

   Copyright 1999-2004 The KSpread Team <koffice-devel@mail.kde.org>

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


#ifndef __kspread_editors_h__
#define __kspread_editors_h__

#include <kcompletion.h>
#include <qwidget.h>
#include <kcombobox.h>

class KSpreadCell;
class KSpreadCanvas;
class KSpreadView;

class KSpreadLocationEditWidget;


class QFont;
class QButton;
class KLineEdit;

class KSpreadCellEditor : public QWidget
{
    Q_OBJECT
public:
    KSpreadCellEditor( KSpreadCell*, KSpreadCanvas* _parent = 0, const char* _name = 0 );
    ~KSpreadCellEditor();

    KSpreadCell* cell()const { return m_pCell; }

    virtual void handleKeyPressEvent( QKeyEvent* _ev ) = 0;
    virtual void handleIMEvent( QIMEvent * _ev ) = 0;
    virtual void setEditorFont(QFont const & font, bool updateSize) = 0;
    virtual QString text() const = 0;
    virtual void setText(QString text) = 0;
    virtual int cursorPosition() const = 0;
    virtual void setCursorPosition(int pos) = 0;
    // virtual void setFocus() = 0;
    virtual void insertFormulaChar(int c) = 0;
    virtual void cut(){};
    virtual void paste(){};
    virtual void copy(){};
    KSpreadCanvas* canvas()const { return m_pCanvas; }

private:
    KSpreadCell* m_pCell;
    KSpreadCanvas* m_pCanvas;
};

class KSpreadTextEditor : public KSpreadCellEditor
{
    Q_OBJECT
public:
    KSpreadTextEditor( KSpreadCell*, KSpreadCanvas* _parent = 0, const char* _name = 0 );
    ~KSpreadTextEditor();

    virtual void handleKeyPressEvent( QKeyEvent* _ev );
    virtual void handleIMEvent( QIMEvent * _ev );
    virtual void setEditorFont(QFont const & font, bool updateSize);
    virtual QString text() const;
    virtual void setText(QString text);
    virtual int cursorPosition() const;
    virtual void setCursorPosition(int pos);
    // virtual void setFocus();
    virtual void insertFormulaChar(int c);
    virtual void cut();
    virtual void paste();
    virtual void copy();
    bool checkChoose();
    void blockCheckChoose( bool b ) { m_blockCheck = b; }
    bool sizeUpdate() const { return m_sizeUpdate; }

private slots:
    void slotTextChanged( const QString& text );
    void  slotCompletionModeChanged(KGlobalSettings::Completion _completion);
protected:
    void resizeEvent( QResizeEvent* );
    /**
     * Steals some key events from the QLineEdit and sends
     * it to the @ref KSpreadCancvas ( its parent ) instead.
     */
    bool eventFilter( QObject* o, QEvent* e );

private:
    //QLineEdit* m_pEdit;
    KLineEdit* m_pEdit;
    bool m_blockCheck;
    bool m_sizeUpdate;
    uint m_length;
    int  m_fontLength;
};


class KSpreadComboboxLocationEditWidget : public KComboBox
{
    Q_OBJECT
public:
    KSpreadComboboxLocationEditWidget( QWidget *_parent, KSpreadView * _canvas );

public slots:
    void slotAddAreaName( const QString & );
    void slotRemoveAreaName( const QString & );

private:
    KSpreadLocationEditWidget *m_locationWidget;
};


 /**
 * A widget that allows the user to enter an arbitrary
 * cell location to goto or cell selection to highlight
 */
class KSpreadLocationEditWidget : public QLineEdit
{
	Q_OBJECT
public:
	KSpreadLocationEditWidget( QWidget *_parent, KSpreadView * _canvas );
	KSpreadView * view() const { return m_pView;}
protected:
	virtual void keyPressEvent( QKeyEvent * _ev );
private:
	KSpreadView * m_pView;
signals:
	void gotoLocation( int, int );
};

/**
 * The widget that appears above the sheet and allows to
 * edit the cells content.
 */
class KSpreadEditWidget : public QLineEdit
{
    Q_OBJECT
public:
    KSpreadEditWidget( QWidget *parent, KSpreadCanvas *canvas,
                       QButton *cancelButton, QButton *okButton);

    virtual void setText( const QString& t );

    // Go into edit mode (enable the buttons)
    void setEditMode( bool mode );

    void showEditWidget(bool _show);
public slots:
    void slotAbortEdit();
    void slotDoneEdit();

protected:
    virtual void keyPressEvent ( QKeyEvent* _ev );
    virtual void focusOutEvent( QFocusEvent* ev );

private:
    QButton* m_pCancelButton;
    QButton* m_pOkButton;
    KSpreadCanvas* m_pCanvas;
    bool isArray;
};


#endif
