/*
** Header file for inclusion with kword_xml2latex.c
**
** Copyright (C) 2002 - 2003 Robert JACOLIN
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
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#ifndef __LATEX_CONFIG_H__
#define __LATEX_CONFIG_H__

#include <QTextStream>
#include <QStringList>

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
	/* Document tab */
	bool _useLatexStyle;
	bool _isEmbeded;
	QString _class;
	QString _quality;
	unsigned int _defaultFontSize;
	
	/* Pictures tab */
	bool _convertPictures;
	QString _picturesDir;

	/* Language tab */
	//bool _useUnicode;
	//bool _useLatin1;
	QString _encoding;
	QStringList _languagesList;
	QString _defaultLanguage;

	int _tabSize;	/* Size of the para indentation. */
	int _tabulation;	/* Total size  of the indentation. */

	public:

		static const char SPACE_CHAR;
		
		static Config* instance(void);
		
		Config(const Config&);

		/* 
		 * Destructor
		 */
		virtual ~Config();

		/* ==== Getters ==== */

		/**
		 * Return the value of a tabulation.
		 */
		bool isKwordStyleUsed() const { return (_useLatexStyle == false); }
		bool isEmbeded() const { return _isEmbeded; }
		QString getClass() const { return _class; }
		QString getQuality() const { return _quality; }
		unsigned int getDefaultFontSize() const { return _defaultFontSize; }

		bool convertPictures() const { return _convertPictures; }
		QString getPicturesDir() const { return _picturesDir; }
		
		bool mustUseUnicode() const { return (_encoding == "unicode"); }
		bool mustUseLatin1() const { return (_encoding != "unicode"); }
		QString getEncoding() const { return _encoding; }
		QStringList getLanguagesList() const { return _languagesList; }
		QString getDefaultLanguage() const { return _defaultLanguage; }
		
		int getTabSize() const { return _tabSize; }
		int getIndentation() const { return _tabulation; }

		/* ==== Setters ==== */

		/**
		 * Initialise the tab size.
		 * @param size New size. Must be superior or eguals to 0.
		 */
		void setTabSize(int size)
		{
			if(size >= 0)
				_tabSize = size;
		}
		
		void useLatexStyle() { _useLatexStyle = true;  }
		void useKwordStyle() { _useLatexStyle = false; }
		void setEmbeded(bool emb) { _isEmbeded = emb; }
		/** The class can be article, book, letter, report or slides. It's the type of the
		 * latex document. */
		void setClass(const QString &lclass) { _class = lclass; }
		void setQuality(const QString &quality) { _quality = quality; }
		void setDefaultFontSize(int size) { _defaultFontSize = size; }

		void convertPictures(bool state) { _convertPictures = state; }
		void setPicturesDir(const QString &dir) { _picturesDir = dir; }

		void setEncoding(const QString &enc) { _encoding = enc; }
		void addLanguage(const QString &l) { _languagesList.append(l); }
		void setDefaultLanguage(const QString &l) { _defaultLanguage = l; }
		
		void setIndentation(int indent) { _tabulation = indent; }
		
		/* ==== Helpful functions ==== */
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

#endif /* __LATEX_CONFIG_H__ */
