/* 
 * @memo Projet d'Infographie : Fichier en-tête de la classe Groupe.
 * 
 * @name groupe.hxx
 * Ce fichier contient les déclarations de la classe Groupe développée
 * dans le cadre d'un projet d'infographie de 3e année.\\
 * Consulter la documentaion relative à Groupe.cxx pour obtenir de plus amples
 * informations sur l'implémentation.
 * @author Robert Jacolin
 * @version 0.0.1 \\
 * CREATION     : 51/12/2000 \\
 * MODIFICATION :                                      
 * @see Ellipse.hxx Droite.hxx Hyperbole.hxx Cercle.hxx Figure.hxx
 */

#ifndef __LISTETEXTZONE_H__
#define __LISTETEXTZONE_H__

#include "textzone.h"
#include "elementt.h"

/**
* La classe Groupe permet de manipuler un ensemble de figures quelconques.
* Pour cela, il fournit un classe pour gérer un liste de figure (classe 
* Groupe) ainsi qu'un itérateur (Groupe_Iterator).
* La liste de figures peut etre vide. 
* @memo Liste de figures, parcourt d'une liste de figures
*/
class ListeTextZone
{
	QString _texte;

	private:
		ElementT *_first;      /* Pointeur sur le debut et        */
		ElementT *_last;       /* la fin de la liste.             */
		int _size;

	protected:
		
	public:
/**
 * @name Constructeurs
 */
//@{
	/// Constructeur par défaut
	ListeTextZone() { 
		_first = 0;
		_last  = 0;
		_size  = 0;
	}
//@}

/**
 * @name Destructeurs
 */
//@{
	  /// Destructeur
	virtual ~ListeTextZone() { vider(); }
//@}

/**
 * @name Accesseurs
 */
//@{
	TextZone* get_first    () const { return _first->get_texte(); }
	ElementT* get_first_elt() const { return _first;              }
	TextZone* get_last     () const { return _last->get_texte();  }
	bool      is_vide      () const { return (_size == 0);        }
	int       get_size     () const { return _size;               }
//@}

/**
 * @name Modifieurs
 */
//@{
	void add_last(const TextZone*);       /* Ajoute une figure à la fin */
	void add_first(const TextZone*);      /* ou au debut de la liste.   */
	void rem_last();                      /* Enleve la derniere ou      */
	void rem_first();                     /* la premiere figure.        */
	
//@}

/**
 * @name Operateurs
 */
//@{
 //@}
/**
 * ^name Fonctions de delegation
 */
//@{
	void vider();         /* Vide la liste */

//@}
};

/* Prendre en compte le fait que la listetextezone peut etre null */
class TextZoneIter {
	ElementT *_courant;
	
	protected:
	
	public:
/**
 * @name Constructeurs
 */
//@{
	/// Constructeur par défaut
	TextZoneIter()                 { _courant = 0;                  }
	/// Constructeur
	TextZoneIter(ListeTextZone l)  { _courant = l.get_first_elt();  }
	/// Constructeur
	TextZoneIter(ListeTextZone *l) { _courant = l->get_first_elt(); }
//@}

/**
 * @name Destructeurs
 */
//@{
	  /// Destructeur
	virtual ~TextZoneIter() { }
//@}

/**
 * @name Accesseurs
 */
//@{	

	TextZone* get_courant()  const { return _courant->get_texte(); }
	bool      is_terminate() const { return (_courant == 0); }
//@}

/**
 * @name Modifieurs
 */
//@{
	void next() { _courant = _courant->get_next(); }
//@}

/**
 * @name Operateurs
 */
//@{
 //@}
/**
 * @name Fonctions de delegation
 */
//@{

//@}

};

#endif /* __LISTETEXTZONE_H__ */

