/*******************************************************/ 
/* LISTETEXTZONE.CC                                    */
/*******************************************************/
/* Implémente les méthodes de l'objet ListeTextZone    */
/* CREATION     : 15/12/2000                           */
/* MODIFICATION :                                      */
/*                                                     */
/*******************************************************/

/* EN-TETES */
#include "listetextzone.h"

/* METHODES PRIVEES   */

/* METHODES PROTEGEES */

/* METHODES PUBLIQUES */

/* Constructeurs           */

/* Destructeurs            */

/* Accesseurs              */

/* Modifieurs              */
void ListeTextZone::add_last(const TextZone* text)
{
	ElementT *new_last = new ElementT;

	new_last->set_texte(text);

	if(_size != 0)
	{
		_last->set_next(new_last);
		_last = new_last;
	}
	else
	{
		/* La liste est vide => _last = _first; */
		_last = new_last;
		_first = _last;
	}	
	_size = _size + 1;
}

void ListeTextZone::add_first(const TextZone* text)
{
	ElementT *new_first = new ElementT;

	new_first->set_texte(text);
	new_first->set_next(_first);

	_first = new_first;
	if(_size == 0)
	{
		/* La liste est vide => _last = _first; */
		_last = _first;
	}	
	_size = _size + 1;
}

void ListeTextZone::rem_last()
{
	ElementT *new_last = new ElementT(_first);
	
	for(int index = 1; index< _size - 1; new_last = new_last->get_next())
	{ }
	
	delete _last;
	_last = new_last;
	_size = _size - 1;
}

void ListeTextZone::rem_first()
{
	ElementT *first_saved = new ElementT;

	delete _first;
	_first = first_saved;
	_size = _size - 1;
}

/* Operateurs              */

/* Fonctions de delegation */
void ListeTextZone::vider()
{
	while(_size != 0)
	{
		rem_first();
	}
}
