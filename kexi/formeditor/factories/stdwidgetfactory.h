#ifndef STDWIDGETFACTORY_H
#define STDWIDGETFACTORY_H


#include "widgetfactory.h"
#include "container.h"

/**
 *
 * Lucijan Busch
 **/
class StdWidgetFactory : public KFormDesigner::WidgetFactory
{
	Q_OBJECT

	public:
		StdWidgetFactory(QObject *parent, const char *name, const QStringList &args);
		~StdWidgetFactory();

		virtual QString				name();
		virtual KFormDesigner::WidgetList	classes();
		virtual QWidget				*create(const QString &, QWidget *, const char *, KFormDesigner::Container *);
		virtual void				createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container);
		virtual void		startEditing(const QString &classname, QWidget *w, KFormDesigner::Container *container);
		virtual void	saveSpecialProperty(const QString &classname, const QString &name, const QVariant &value, QWidget *w,
		         QDomElement &parentNode, QDomDocument &parent);
		virtual void            readSpecialProperty(const QString &classname, QDomElement &node, QWidget *w);
		virtual bool		showProperty(const QString &classname, QWidget *w, const QString &property, bool multiple);
		virtual QStringList     autoSaveProperties(const QString &classname);

	protected:
		virtual void  changeText(const QString &newText);
		virtual void  resetEditor();

	private:
		KFormDesigner::WidgetList		m_classes;
		KFormDesigner::Container		*m_container;
};

#endif
