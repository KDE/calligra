

#ifndef __LATEXPARSER_COMMAND_H__
#define __LATEXPARSER_COMMAND_H__

#include "param.h"
//#include "option.h"
#include "element.h"

#include <qstring.h>
#include <qmap.h>
#include <qptrlist.h>

class Command: public Element
{
	public:
		Command();
		/**
		 * Create a command which match to \today
		 */
		Command(const char* command);

		/**
		 * Create a command which match to \renewcommand{\headrulewidth}{opt}
		 */
		Command(const char* name, QPtrList<QPtrList<Element> >* groups);

		/**
		 * Create a command which match to \documentclass[11pt]{guidepra} or 
		 * \put(1,1){\circle[}}.
		 */
		Command(const char* name, QPtrList<QPair<QString, QString> >* options,
				QPtrList<QPtrList<Element> >* groups);

		/**
		 * Create a command which match to \parpic(3cm,3cm)[f]{text}
		 */
		Command(const char* name, QPtrList<QPair<QString, QString> >* options1, QPtrList<QPair<QString,
				QString> >* options2, QPtrList<QPtrList<Element> >* groups);

		~Command();

		QString getName() const { return _name; }

		QPtrList<Param> getParams() const { return _params; }
		
		QPtrList<Param> getOptions() const { return _options; }
		
		QPtrList<QPtrList<Element> > getChildren() const { return _elements; }

		void setName(const char* name) { _name = name; }
		void addParam(const char* param);
		void addParam(QString key, QString value) { _params.append(new Param(key, value)); }
		void addParams(QPtrList<Param> params) { _params = params; }
		void addGroups(QPtrList<QPtrList<Element> >* elts) { _elements = *elts; }

		void addOption(const char* option);
		void addOption(QString key, QString value) { _options.append(new Param(key, value)); }
	
		void addChild(QPtrList<Element>* elt) { _elements.append(elt); }

	private:
		QString _name;
		QPtrList<Param> _params;
		QPtrList<Param> _options;
		QPtrList<QPtrList<Element> > _elements;

};

#endif /* __LATEXPARSER_COMMAND_H__ */

