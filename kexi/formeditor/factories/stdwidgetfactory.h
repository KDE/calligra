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

	private:
		KFormDesigner::WidgetList		m_classes;
};

#endif
