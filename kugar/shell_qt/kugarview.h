/**************************************************************************
						  KugarView.h  -  description
							 -------------------
	begin				 : 2003-03-13 11:12:40
	copyright			 : (C) 2003 by Joris Marcillac
	email				 : joris@marcillac.org
**************************************************************************/

/**************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*	This program is distributed in the hope that it will be useful,	  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of	  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU *
*	Library General Public License for more details.		  *
*                                                                         *
*	You should have received a copy of the GNU Library General Public *
*	License along with this library; if not, write to the Free	  *
*	Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,	  *
*	MA 02111-1307, USA						  *
*                                                                         *
**************************************************************************/
#if !defined(KUGARVIEW_H)
#define KUGARVIEW_H

#include <qglobal.h>

//#if defined(Q_WS_WIN) 
//  #if defined(MAKELIB)
//    #define MY_LIB_EXPORT  __declspec(dllexport)
//  #else
//    #define MY_LIB_EXPORT  __declspec(dllimport)
//  #endif
//#else
  #define MY_LIB_EXPORT
//#endif

class QWidget;
class QString;
class QIODevice;
class KugarViewPrivate;

class MY_LIB_EXPORT KugarView
{
public:
	KugarView(QWidget *parent=0, const char *name=0);
	KugarView( const KugarView& other );
	KugarView& operator=( const KugarView& other );
	virtual ~KugarView();

	bool setReportData(const QString &);
	bool setReportData(QIODevice *);
	bool setReportTemplate(const QString &);
	bool setReportTemplate(QIODevice *);
	bool renderReport();
	void clearReport();
	void printReport();
	void show();

private:
	void deref();
	KugarViewPrivate* d;
};
#endif // !defined(KUGARVIEW_H)