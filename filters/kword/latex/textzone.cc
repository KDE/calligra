/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2000 Robert JACOLIN
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

#include <iostream.h>

#include <stdlib.h>

#include <kdebug.h>

#include "textzone.h"

TextZone::TextZone(const char *texte): _texte(texte)
{
	//_texte = new QString(texte);
}

void TextZone::analyse(const Markup * balise_initiale)
{
	Token *savedToken;
	//Token *p;

	// ON A UNE BALISE DE TYPE FORMAT id="1" pos="0" len="17">...</FORMAT>
	Markup *balise;
	
	// Analyse des paramètres
	analyse_param(balise_initiale);
	cout << "ANALYSE D'UNE ZONE" << endl;
	
	// Analyse des balises filles
	savedToken = enterTokenChild(balise_initiale);
	
	while((balise = getNextMarkup()) != NULL)
	{
		if(strcmp(balise->token.zText, "FONT")== 0)
		{
			cout << "FONT : " << endl;
			analyse_font(balise);
		}
		else if(strcmp(balise->token.zText, "ITALIC")== 0)
		{
			cout << "ITALIC : " << endl;
			analyse_italic(balise);
		}
		else if(strcmp(balise->token.zText, "WEIGHT")== 0)
		{
			cout << "WEIGTH : " << endl;
			analyse_weigth(balise);
		}
	}
	cout << "FIN D'UNE ZONE" << endl;
	setTokenCurrent(savedToken);
}

void TextZone::analyse_param(const Markup *balise)
{
	//<FORMAT id="1" pos="0" len="17">
	Arg *arg;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		cout << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "ID")== 0)
		{
			set_id(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "POS")== 0)
		{
			set_pos(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "LEN")== 0)
		{
			set_taille (atoi(arg->zValue));
		}
	}
	cout << get_pos() << " -- " << get_length() << endl;
	_texte = _texte.mid(get_pos(), get_length());
	cout << _texte.length() << endl;
	cout << _texte.local8Bit() << endl;
}

void TextZone::analyse_font(const Markup *balise)
{
	//<FONT name="times">
	Arg *arg;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		cout << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "NAME")== 0)
		{
			set_police(arg->zValue);
		}
	}
}

void TextZone::analyse_italic(const Markup *balise)
{
	//<FONT name="times">
	Arg *arg;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		cout << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "VALUE")== 0)
		{
			set_italic(arg->zValue);
		}
	}
}

void TextZone::analyse_weigth(const Markup *balise)
{
	//<FONT name="times">
	Arg *arg;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		cout << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "VALUE")== 0)
		{
			set_weight(atoi(arg->zValue));
		}
	}
}
void TextZone::generate(QTextStream &out)
{
	//fprintf(out, "\\begin{document}\n");
	// Parcourir les elements pour generer le fichier.
	// Pour chaque entete - footnote
	// _entete.genere(_outputFile);
	// Pour chaque corps de texte
	// _element.genere(_outputFile);
	cout << "." << endl;
	if(get_weight() > 0)
		out << " \\textbf{";
	if(is_italic())
		out << " \\textit{";
	//fprintf(out, "%s\n", _texte);
	out << _texte.latin1();
	if(is_italic())
		out << "}";
	if(get_weight() > 0)
		out << "}";
}

