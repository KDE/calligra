
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
		void prevStackPage();
		void nextStackPage();

	private:
		KFormDesigner::WidgetList		m_classes;
		QWidget *m_widget;
		KFormDesigner::Container *m_container;
};

#endif
