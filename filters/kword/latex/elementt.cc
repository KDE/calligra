/*******************************************************/ 
/* ELEMENTT.CXX                                        */
/*******************************************************/
/* Implémente les méthodes de l'objet ElementT         */
/* CREATION     : 16/12/2000                           */
/* MODIFICATION :                                      */
/*                                                     */
/*******************************************************/

/* EN-TETES */
#include "elementt.h"

/* METHODES PRIVEES        */

/* METHODES PROTEGEES      */

/* METHODES PUBLIQUES      */

/* Constructeurs           */

/* Destructeurs            */

/* Accesseurs              */

/* Modifieurs              */
void ElementT::set_texte(const TextZone* text)
{
	if(_texte == 0)
		_texte = new TextZone;
	_texte = (TextZone*)text;	
}

void ElementT::rem_texte()
{
	delete _texte;
	_texte = 0;
}

void ElementT::set_next(ElementT* next)
{
	_next = next;
}

void ElementT::rem_next()
{
	delete _next;
	_next = 0;
}

/* Operateurs              */
ElementT& ElementT::operator = (const ElementT & elt)
{
	_texte = elt.get_texte();
	_next  = elt.get_next();
	return *this;
}

/* Fonctions de delegation */

