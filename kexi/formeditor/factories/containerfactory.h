
#ifndef CONTAINERFACTORY_H
#define CONTAINERFACTORY_H

#include <kcommand.h>

#include "widgetfactory.h"
#include "container.h"

namespace KFormDesigner
{
	class Form;
	class Container;
}

class InsertPageCommand : public KCommand
{
	public:
		InsertPageCommand(KFormDesigner::Container *container, QWidget *widget);

		virtual void execute();
		virtual void unexecute();
		virtual QString name() const;

	protected:
		KFormDesigner::Form *m_form;
		QString  m_containername;
		QString  m_name;
		QString  m_parentname;
		int      m_pageid;
};

/**
 *
 * Lucijan Busch
 **/
class ContainerFactory : public KFormDesigner::WidgetFactory
{
	Q_OBJECT

	public:
		ContainerFactory(QObject *parent, const char *name, const QStringList &args);
		~ContainerFactory();

		virtual QString				name();
		virtual KFormDesigner::WidgetList	classes();
		virtual QWidget				*create(const QString &, QWidget *, const char *, KFormDesigner::Container *);
		virtual bool				createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container);
		virtual void		startEditing(const QString &classname, QWidget *w, KFormDesigner::Container *container);
		virtual bool		showProperty(const QString &classname, QWidget *w, const QString &property, bool multiple) { return !multiple;}
		virtual void     saveSpecialProperty(const QString &classname, const QString &name, const QVariant &value, QWidget *w,
		                        QDomElement &parentNode, QDomDocument &parent);
		virtual void            readSpecialProperty(const QString &classname, QDomElement &node, QWidget *w);
		virtual QStringList     autoSaveProperties(const QString &classname);

	protected:
		virtual void  changeText(const QString &newText);

	public slots:
		void AddTabPage();
		void AddStackPage();
		void renameTabPage();
		void removeTabPage();
		void removeStackPage();
		void prevStackPage();
		void nextStackPage();

	private:
		KFormDesigner::WidgetList		m_classes;
		QWidget *m_widget;
		KFormDesigner::Container *m_container;
};

#endif
