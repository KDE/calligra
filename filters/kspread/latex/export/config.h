/*
** Header file for inclusion with kword_xml2latex.c
**
** Copyright (C) 2002 Robert JACOLIN
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** To receive a copy of the GNU Library General Public License, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
*/

#ifndef __KWORD_LATEX_CONFIG_H__
#define __KWORD_LATEX_CONFIG_H__

#include <qtextstream.h>

/***********************************************************************/
/* Class: Config                                                       */
/***********************************************************************/

/**
 * This class hold all parameters and configuration from a file or from
 * the filter configuration dialog box. it's a singleton, so you may use
 * the instance() method to get this instance.
 */
class Config
{
	int _tabSize;	/* Size of the para indentation. */
	int _tabulation;	/* Total size  of the indentation. */
	bool _useLatexStyle;
	bool _useLatin1;
	bool _useUnicode;
	QString _class;
	bool _isEmbeded;

	public:

		static const char SPACE_CHAR;
		
		static Config* instance(void);
		
		Config(const Config&);

		/* 
		 * Destructor
		 */
		virtual ~Config();

		/**
		 * Accessors
		 */

		/**
		 * Return the value of a tabulation.
		 */
		int getTabSize() const { return _tabSize; }
		int getIndentation() const { return _tabulation; }
		bool isKwordStyleUsed() const { return (_useLatexStyle == false); }
		bool mustUseUnicode() const { return _useUnicode; }
		bool mustUseLatin1() const { return _useLatin1; }
		QString getClass() const { return _class; }
		bool isEmbeded() const { return _isEmbeded; }

		/**
		 * Modifiers
		 */

		/**
		 * Initialise teh tab size.
		 * @param size New size. Must be superior or eguals to 0.
		 */
		void setTabSize(int size)
		{
			if(size >= 0)
				_tabSize = size;
		}
		void setIndentation(int indent) { _tabulation = indent; }
		void useUnicodeEnc() { _useUnicode    = true; _useLatin1 = false;  }
		void useLatin1Enc () { _useLatin1     = true; _useUnicode = false; }
		void useLatexStyle() { _useLatexStyle = true;  }
		void useKwordStyle() { _useLatexStyle = false; }
		/** The class can be article, book, letter, report or slides. It's the type of the
		 * latex document. */
		void setClass(QString classDoc) { _class = classDoc; }
		void setEmbeded(bool emb) { _isEmbeded = emb; }
		
		/**
		 * Helpfull functions
		 */
		void indent();
		void desindent();

		void writeIndent(QTextStream& out);

	protected:
		/**
		 * Constructors
		 *
		 * Creates a new instance of Config.
		 * Initialise param. at default value.
		 */
		Config(); /* Ensure singleton */

		static Config* _instance; /* Singleton */

	private:

};

#endif /* __KWORD_LATEX_CONFIG_H__ */
