/***************************************************************************
                          kexiformbase.h  -  description
                             -------------------
    begin                : Mon Jun 17 2002
    copyright            : (C) 2002 by lucijan busch
			   (C) 2002 by Joseph Wenninger <jowenn@kde.org>
    email                : lucijan@gmx.at
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KEXIFORMBASE_H
#define KEXIFORMBASE_H

#include "kexidialogbase.h"
#include <qpixmap.h>

/**
  *@author Joseph Wenninger
  *@author lucijan busch
  */

template <class type> class QPtrList;

class KAction;

class KexiFormBaseResizeHandle : public QWidget
{
Q_OBJECT
public:
	enum HandlePos {TopLeft=0,TopCenter=2,TopRight=4,LeftCenter=8,RightCenter=16,BottomLeft=32,BottomCenter=64,BottomRight=128};
        KexiFormBaseResizeHandle(QWidget *parent,QWidget *buddy, HandlePos pos);
        virtual ~KexiFormBaseResizeHandle();

protected:
                void mousePressEvent(QMouseEvent *ev);
                void mouseMoveEvent(QMouseEvent *ev);
                void mouseReleaseEvent(QMouseEvent *ev);
protected slots:
	bool eventFilter(QObject *obj, QEvent *ev);
	void updatePos();	

private:
	HandlePos m_pos;
	QWidget *m_buddy;
	bool m_dragging;
	int m_x;
	int m_y;
};

class KexiFormBaseResizeHandleSet: public QObject
{
Q_OBJECT
public:
	KexiFormBaseResizeHandleSet(QWidget *modify);
	~KexiFormBaseResizeHandleSet();
	QWidget *widget(){return m_widget;};
private:
	QGuardedPtr<KexiFormBaseResizeHandle> handles[8];
	QGuardedPtr<QWidget> m_widget;
	
};


class KexiFormBase : public KexiDialogBase
{

	Q_OBJECT
   
	public: 
		KexiFormBase(QWidget *parent=0, const char *name=0, QString identifier=QString::null);
		~KexiFormBase();
		
		void setActions(QPtrList<KAction> *actions);
		void unregisterActions(QPtrList<KAction> *actions);
		static int dotSpacing();
	protected:
		void initActions();
		void registerAction(KAction *action);

		void paintEvent(QPaintEvent *ev);
		void mouseMoveEvent(QMouseEvent *ev);
		void mouseReleaseEvent(QMouseEvent *ev);
		void resizeEvent(QResizeEvent *ev);
		
		void insertWidget(QWidget *widget, int x, int y, int w, int h);
		void installEventFilterRecursive(QObject *obj);

		void setResizeHandles(QWidget *m_activeWidget);
		void activateWidget(QWidget *widget);
		
		QWidget	*m_pendingWidget;
		
		QPixmap	m_dotBg;
		
		int	m_dotSpacing;

		bool	m_widgetRectRequested;
		bool	m_widgetRect;
	
		int	m_widgetRectBX;
		int	m_widgetRectBY;
		int	m_widgetRectEX;
		int	m_widgetRectEY;

		int m_moveBX;
		int m_moveBY;

		QWidget *m_activeWidget;
		QWidget *m_activeMoveWidget;
		KexiFormBaseResizeHandleSet *m_resizeHandleSet;
	protected slots:
		void slotWidgetLineEdit();
		void slotWidgetPushButton();
		void slotWidgetURLRequester();
		
		bool eventFilter(QObject *obj, QEvent *ev);

};

#endif
