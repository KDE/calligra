
#ifndef CONTAINERFACTORY_H
#define CONTAINERFACTORY_H


#include "widgetfactory.h"
#include "container.h"

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
		virtual QWidget				*create(const QString &, QWidget *, const char *,
					 		 KFormDesigner::Container *);
		virtual void				createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container);
		virtual void		startEditing(const QString &classname, QWidget *w, KFormDesigner::Container *container){;}

	public slots:
		void AddTabPage();
		void chTitle();

	private:
		KFormDesigner::WidgetList		m_classes;
		QWidget *m_widget;
		KFormDesigner::Container *m_container;
};

#endif
