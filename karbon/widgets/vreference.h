/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002, The Karbon Developers
*/

/* This is the color slider widget that is used to select color or color components */

/* vreference.h */
#ifndef VREFERENCE_H
#define VREFERENCE_H

#include <qframe.h>

class QButtonGroup;

enum Choice
{
	TopLeft,
	Left,
	BottomLeft,
	Top,
	Center,
	Bottom,
	TopRight,
	Right,
	BottomRight
};

class VReference : public QFrame
{
	Q_OBJECT
public:
	VReference( QWidget *parent = 0L, const char *name = 0L );
	int referencePoint();
public slots:
	virtual void setReferencePoint ( int );

private:
	int m_referencePoint;
	QButtonGroup* mButtonGroup;
signals:
	void referencePointChanged( int referencePoint );
};

#endif

