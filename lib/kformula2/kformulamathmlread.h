/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.
 
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
 
   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KFORMULAMATHMLREAD_H
#define __KFORMULAMATHMLREAD_H

#include <qobject.h>
#include <qdom.h>

/**
 This class converts MathML to KFormula XML.
 Right now is implemented only the PresentationMarkup.
*/

class MathMl2KFormula : QObject
{
Q_OBJECT    
    public:
	/**
	 * Build a MathML 2 KFormula converter.
	 * call @startConversion to convert and wait for 
	 * a @conversionFinished signal, than call
	 * @getKFormulaDom to get the converted DOM
	 */
	MathMl2KFormula(QDomDocument *mmldoc);
	
	/*
	 * Get the just created DOM.
	 */
	QDomDocument getKFormulaDom();
    
	bool isDone();
	

    public slots:
	void startConversion();

    signals:
	void conversionFinished();

    private:

	bool processElement(QDomNode *node,QDomDocument *doc,QDomNode *docnode);

	bool done;
	bool conversion;
	QDomDocument *origdoc;
	QDomDocument formuladoc;
};

#endif // __KFORMULAMATHMLREAD_H
