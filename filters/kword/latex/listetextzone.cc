/*******************************************************/ 
/* LISTETEXTZONE.CC                                    */
/*******************************************************/
/* Implémente les méthodes de l'objet ListeTextZone    */
/* CREATION     : 15/12/2000                           */
/* MODIFICATION :                                      */
/*                                                     */
/*******************************************************/

/* INCLUDES */
#include <kdebug.h>
#include "listetextzone.h"

/* PRIVATE METHODS   */

/* PROTECTED METHODS */

/* PUBLIC METHODS    */

/* Constructors           */
ListeTextZone::ListeTextZone()
{
	_first = 0;
	_last  = 0;
	_size  = 0;
}

/* Destructors            */
ListeTextZone::~ListeTextZone()
{
	kdDebug() << "Destruction of a text zone list" << endl;
	vider();
}

/* Accessors              */

/* Modifiors              */
void ListeTextZone::addLast(TextZone* text)
{
	ElementT *new_last = new ElementT;

	new_last->setText(text);

	if(_size != 0)
	{
		_last->setNext(new_last);
		_last = new_last;
	}
	else
	{
		/* La liste est vide => _last = _first; */
		_last  = new_last;
		_first = _last;
	}	
	_size = _size + 1;
}

void ListeTextZone::addFirst(TextZone* text)
{
	ElementT *new_first = new ElementT;

	new_first->setText(text);
	new_first->setNext(_first);

	_first = new_first;
	if(_size == 0)
	{
		/* La liste est vide => _last = _first; */
		_last = _first;
	}	
	_size = _size + 1;
}

void ListeTextZone::remLast()
{
	ElementT *new_last = new ElementT(_first);
	
	for(int index = 1; index< _size - 1; new_last = new_last->getNext())
	{ }
	
	delete _last;
	_last = new_last;
	_size = _size - 1;
}

void ListeTextZone::remFirst()
{
	ElementT *first_saved;

	first_saved = _first->getNext();

	delete _first;
	_first = first_saved;
	_size  = _size - 1;
}

/* Operateurs              */

/* Fonctions de delegation */
void ListeTextZone::vider()
{
	while(_size != 0)
	{
		remFirst();
	}
}

/*********************************************************/
TextZoneIter::TextZoneIter(ListeTextZone *l)
{
	setList(l);
}

void TextZoneIter::setList(ListeTextZone *l)
{
	if(l!= 0)
		_courant = l->getFirstElt();
	else
		_courant = 0;
}
