

#ifndef __LATEXPARSER_ELEMENT_H__
#define __LATEXPARSER_ELEMENT_H__

/* Type of commands */

enum EType
{
	ELT_NONE,
	ELT_ENV,
	ELT_MATH
};

//#include <qptrlist.h>

class Element
{
	public:
		Element();

		~Element();

		/* getters */
		EType getType() const { return _type; }

		/* setters */
		void setType(EType t) { _type = t; }

		/* usefull methods */
		//void addParams(QPtrList<Param> params) { }
		//void addGroups(QPtrList<QPtrList<Element> >* elts) {  }
	private:
		EType _type;

};

#endif /* __LATEXPARSER_ELEMENT_H__ */

