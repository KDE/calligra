// QwViewport widget for Qt C++ class library.
//
// Author: Warwick Allison (warwick@cs.uq.oz.au)
//   Date: May 24, 1995
//
// Copyright (C) 1995 by Warwick Allison.
//

#ifndef QVIEWP_H
#define QVIEWP_H

#include <qwidget.h>
#include <qscrbar.h>

class QwViewport : public QWidget
{
	Q_OBJECT
public:
	QwViewport(QWidget *parent=0, const char *name=0, WFlags f=0 );

	// One QWidget can be the child of the porthole.
	QWidget* portHole();

	// The component scrollbars are made available to allow
	// accelerators, autoscrolling, etc., and to allow changing
	// of arrow scrollrates: bar->setSteps(rate, bar->pageStep()).
	// They should not be otherwise manipulated.
	QScrollBar& horizontalScrollBar();
	QScrollBar& verticalScrollBar();

	// Overrides (for implementation purposes)
	virtual void show();

	void ensureVisible(int x, int y, int xmargin=50, int ymargin=50);
	void centerOn(int x, int y);
	void centralize(int x, int y, float xmargin=0.5, float ymargin=0.5);

	// Visual properties.

	// Default 16
	virtual int scrollBarWidth() const;

	// Default false.
	virtual bool scrollBarOnLeft() const;

	// Default false.
	virtual bool scrollBarOnTop() const;

	// Corner between scrollbars to be empty when both are visible.
	// Default true.
	virtual bool emptyCorner() const;

	// Corner between scrollbars to always be empty.
	// Default false.
	virtual bool alwaysEmptyCorner() const;

public slots:
	// Call this if above properties change dynamically.
	void resizeScrollBars();

protected:
	virtual void resizeEvent(QResizeEvent*);

	// The child of the porthole.
	QWidget* viewedWidget() const;

private:
	QScrollBar hbar;
	QScrollBar vbar;
	QWidget porthole;
	static bool signal_choke;

private slots:
	void hslide(int);
	void vslide(int);
};

inline QWidget* QwViewport::portHole() { return &porthole; }
inline QScrollBar& QwViewport::horizontalScrollBar() { return hbar; }
inline QScrollBar& QwViewport::verticalScrollBar() { return vbar; }

#endif
