
/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDIALOGBASE_H
#define KEXIDIALOGBASE_H

#include <qwidget.h>
#include <kxmlguiclient.h>
#include <qptrlist.h>

class KexiView;
class KexiProject;
class QDockWindow;
class QGridLayout;

class KPrinter;
class KToggleAction;
class KexiProjectHandler;
class KexiProjectHandlerItem;

typedef QPtrList<QWidget> Widgets;


class KEXICORE_EXPORT KexiDialogBase : public QWidget, public KXMLGUIClient
{
	Q_OBJECT

	public:

		KexiDialogBase(KexiView* view, QString identifier, QWidget *parent = 0, const char *name = 0);
		KexiDialogBase(KexiView *view, KexiProjectHandlerItem *item, QWidget *parent = 0, const char *name = 0 );
		~KexiDialogBase();

		virtual KXMLGUIClient *guiClient();

		virtual void activateActions();
		virtual void deactivateActions();
		KexiView *mainWindow()const{return m_mainWindow;};

		static KexiDialogBase *activeDocumentView() { return s_activeDocumentWindow;};
		KexiProject *kexiProject()const;
		KexiView *kexiView()const;
		bool isRegistering(){return m_registering;}

		void	setContextHelp(const QString &title, const QString &message);
		const QString& contextHelpTitle() const;
		const QString& contextHelpMessage() const;

		virtual void setupPrinter(KPrinter &/*printer*/){;}
		virtual void print(KPrinter &/*printer*/) {; }
		void aboutToShow();
		void aboutToHide();

//		QDockWindow *dock() { return w; }

		void plugToggleAction(KToggleAction *toggle_action);

		virtual QString windowTypeName();

		/*! Returns main grid layout that can be used to add contents widget(s) */
		QGridLayout* gridLayout() { return m_gridLyr; }

		KexiProjectHandlerItem *partItem() { return m_partItem; }
		KexiProjectHandler *part();

		virtual QSize sizeHint () const;
		const QString& identifier() {return m_identifier;}
	public slots:
		virtual void show();
		virtual void hide();
		virtual void setVisible(bool on);

		virtual void setCustomCaption( const QString &caption );

	signals:
		void closing(KexiDialogBase *);
		void hidden(); //! signalled on hide event
		void shown(); //! signalled on show event

	protected:
		void init();
		void updateCaption();
		void setCustomWindowTypeName( const QString &tn );

		virtual void focusInEvent ( QFocusEvent *);
		enum WindowType {ToolWindow, DocumentWindow};
		void registerAs(KexiDialogBase::WindowType wt, const QString &identifier=QString::null);
		void registerChild(QWidget *w);
		virtual void closeEvent(QCloseEvent *ev);
		virtual void hideEvent(QHideEvent *ev);
		virtual void showEvent(QShowEvent *ev);
		virtual void finishUpForClosing(){;}
		static KexiDialogBase *s_activeDocumentWindow;
		static KexiDialogBase *s_activeToolWindow;
		static QPtrList<KexiDialogBase> *s_DocumentWindows;
		static QPtrList<KexiDialogBase> *s_ToolWindows;

//		class KDockWidget *myDock;
//	private:
		KexiView *m_mainWindow;
//		KexiProject *m_project;
		KexiView *m_view;
		KexiProjectHandlerItem *m_partItem;
		QString	m_identifier; //! window is registered to the view with this identifier

		/*! (i18n-ed) data type of this window (by default: none).
			change this if window will show data of given type, like 'Table',
			thus window's caption will be in the style of "<title> - <typename>".
			Else use setCustomCaption() to set custom static caption like in QWidget.
			\sa set
		*/
		QString m_windowTypeName;
		QDockWindow *w;
		bool	m_registered;
		enum	WindowType m_wt;
		bool	m_registering;
		QString	m_contextTitle;
		QString	m_contextMessage;
		Widgets	m_widgets;

		KToggleAction *m_toggleAction; //! (optional) show/hide action for this window

		QString m_customCaption;
		QGridLayout *m_gridLyr;
};

#endif
