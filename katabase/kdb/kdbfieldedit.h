/***************************************************************************
                          fieldedit.h  -  description                              
                             -------------------                                         
    begin                : Fri Jun 11 1999                                           
    copyright            : (C) 1999 by Ørn E. Hansen                         
    email                : hanseno@mail.bip.net                                     
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/


#ifndef KDB_FIELDEDIT_H
#define KDB_FIELDEDIT_H

#include <qlineedit.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qdatetime.h>

#include <kdbdataset.h>

/**
	*@short A uniform widget, to edit fields.
  *@author Ørn E. Hansen
  */

class kdbFieldEdit : public QWidget {
	Q_OBJECT
private:
	kdbDataField *_field;
	kdbDataSet   *_set;
	QString       _fieldName;
	QString       _label;
	QString       _temp;
	QWidget      *_w;
	QLabel       *_l;
	bool          _acceptSignals;
	int           _column;
	
	void init(kdbDataSet *, const QString&);
	
public: 
	kdbFieldEdit(QWidget *, const char *, kdbDataSet *, const char *);
	~kdbFieldEdit();
	
	void setLabel(const QString&);
	void setLabelWidth(int);
	void setGeometry(int, int);
	void setColumn(int);
	void updateText();
	
	const QString& text();
	const QString& label();
	
	int labelWidth();
	int column();
	
	QSize       sizeHint() const;
	QSizePolicy sizePolicy() const;
	
protected:
	bool event(QEvent *);
		
private slots:
	void postChange(const QString&);
	void textChanged(const QString&);
	void toggled(bool);
	void dateChanged(const QDate&);
	
public slots:
	void setText(const QString&);
	
};

#endif
























