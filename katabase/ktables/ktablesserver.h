/***************************************************************************
                          ktablesserver.h  -  description                              
                             -------------------                                         
    begin                : Thu Jul 8 1999                                           
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


#ifndef KTABLESSERVER_H
#define KTABLESSERVER_H

#include <qwidget.h>

class QLineEdit;

/**
  *@author Ørn E. Hansen
  */

class KtablesServer : public QWidget  {
	Q_OBJECT
	
private:
  QLineEdit *_host;
  QLineEdit *_user;
  QLineEdit *_base;

public: 
	KtablesServer(QWidget *, const char *);
	~KtablesServer();
	
signals:
  void serverSelected();
  void errorMessage(const char *);

protected slots:
	void tryConnect();
	void cancelMe();

};

#endif


