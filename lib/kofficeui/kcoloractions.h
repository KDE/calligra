/* This file is part of the KDE libraries
    Copyright (C) 2000 Reginald Stadlbauer <reggie@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef kcoloractions_h
#define kcoloractions_h

#include <kaction.h>
#include <qvaluelist.h>
#include <qwidget.h>

class QMouseEvent;
class QPaintEvent;

/**
 * An action whose pixmap is automatically generated from a color
 * It knows three types of pixmaps: text color, frame color and background color
 */
class KColorAction : public KAction
{
    Q_OBJECT

public:
    enum Type {
	TextColor,
	FrameColor,
	BackgroundColor
    };

    // Create default (text) color action
    KColorAction( const QString& text, int accel = 0, QObject* parent = 0, const char* name = 0 );
    KColorAction( const QString& text, int accel,
		  QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );
    KColorAction( QObject* parent = 0, const char* name = 0 );

    // Create a color action of a given type
    KColorAction( const QString& text, Type type, int accel = 0,
		  QObject* parent = 0, const char* name = 0 );
    KColorAction( const QString& text, Type type, int accel,
		  QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );

    virtual void setColor( const QColor &c );
    QColor color() const;

    virtual void setType( Type type );
    Type type() const;

private:
    void init();
    void createPixmap();

    QColor col;
    Type typ;

};

/**
 * An improved version of KColorAction, which also features a delayed popupmenu
 * for changing the color (make sure to test color() in your slot).
 */
class KSelectColorAction : public KColorAction
{
  Q_OBJECT
public:
    KSelectColorAction( const QString& text, Type type,
                        int accel = 0, QObject* parent = 0, const char* name = 0 );
    KSelectColorAction( const QString& text, Type type, int accel,
                        QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );

    ~KSelectColorAction();

    virtual int plug( QWidget* widget, int index = -1 );
    KPopupMenu* popupMenu()    { return m_popup; }

public slots:
    /**
     * Displays the color chooser dialog
     */
    void changeColor();

protected:
    void initPopup();

private:
    KPopupMenu *m_popup;
};

/**
 *  class KColorBar
 *
 *  Simple toolbar with little color buttons. This Bar can be placed like a normal toolbar.
 *
 *  @short Toolbar with color buttons.
 */
class KColorBar : public QWidget
{
    Q_OBJECT

public:
    /**
     *  Constructor.
     *
     *  Initializes the color bar with a list of colors.
     */
    KColorBar( const QValueList<QColor> &cols, QWidget *parent, const char *name );

    Orientation orientation() const { return orient; }
    
public slots:
    void orientationChanged( Orientation o );
    
protected:
    void mousePressEvent( QMouseEvent *e );
    void paintEvent( QPaintEvent *e );

private:
    QValueList<QColor> colors;
    Orientation orient;
    
signals:
    /**
     *  This signal gets emitted when the left mouse button is clicked on a color button.
     */
    void leftClicked( const QColor &c );
    /**
     *  This signal gets emitted when the right mouse button is clicked on a color button.
     */
    void rightClicked( const QColor &c );

};

class KColorBarAction : public KAction
{
    Q_OBJECT

public:
    KColorBarAction( const QString &text, int accel,
		     QObject *receiver, const char *leftClickSlot, const char *rightClickSlot,
		     const QValueList<QColor> &cols, QObject *parent, const char *name );

    int plug( QWidget*, int index = -1 );

signals:
    void leftClicked( const QColor &c );
    void rightClicked( const QColor &c );

private:
    QValueList<QColor> colors;
    QObject *receiver;
    QCString leftClickSlot;
    QCString rightClickSlot;
};

#endif
