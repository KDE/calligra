/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* KPresenter is under GNU GPL                                    */
/******************************************************************/
/* Module: KPresenter Undo/Redo (header)                          */
/******************************************************************/

#ifndef _UNDO_REDO_
#define _UNDO_REDO_

#include <qstring.h>
#include <qlist.h>
#include <qobject.h>

#include "global.h"
#include "graphobj.h"

/******************************************************************/
/* Class: UndoRedoBaseClass                                       */
/******************************************************************/
class UndoRedoBaseClass
{
public:
  UndoRedoBaseClass();
  UndoRedoBaseClass(QString _description_);
  virtual ~UndoRedoBaseClass();
  
  void set_description(QString _description_);
  QString get_description();

  /**
   * This methode does the undo/redo. So this methode <b>must</b> be reimplemented.
   */
  virtual void revert();

protected:
  QString description;
  
};


/******************************************************************/
/* Class: UndoRedoStack                                           */
/******************************************************************/
template <class T>
class UndoRedoStack : public QListT<T>
{

public:
  void set_max_level(int _max_level_);
  int get_max_level();

  void push(T *_item_);
  T* pop();

protected:
  void check_max_level();

  int max_level;

};

/******************************************************************/
/* Class: UndoRedoAdmin                                           */
/******************************************************************/
class UndoRedoAdmin : public QObject
{
  Q_OBJECT

public:
  UndoRedoAdmin(int max_level = 64);
  virtual ~UndoRedoAdmin();

  void add_undo_item(UndoRedoBaseClass *_action_);
  void undo();
  void redo();

signals:
  void undo_redo_change(QString _new_undo_,bool _enable_undo_,QString _new_redo_,bool _enable_redo_);

protected:
  UndoRedoStack<UndoRedoBaseClass> undo_stack;
  UndoRedoStack<UndoRedoBaseClass> redo_stack;

};

/******************************************************************/
/* Class: UndoRedoPageObjects                                     */
/******************************************************************/
class UndoRedoPageObjects : public UndoRedoBaseClass
{
public:
  UndoRedoPageObjects(PageObjects *_ptr_old_,PageObjects *_ptr_new_,QString _description_);
  virtual ~UndoRedoPageObjects();

  void set_old(PageObjects* _ptr_old_);
  PageObjects* get_old();
  void set_new(PageObjects* _ptr_new_);
  PageObjects* get_new();

  virtual void revert();

protected:
  PageObjects *ptr_old,*ptr_new;
  PageObjects tmp;

};

/******************************************************************/
/* Class: UndoRedoPageObjectsList                                 */
/******************************************************************/
class UndoRedoPageObjectsList : public UndoRedoBaseClass
{
public:
  UndoRedoPageObjectsList(QList<PageObjects> *_ptr_old_,QList<PageObjects> *_ptr_new_,QString _description_);
  virtual ~UndoRedoPageObjectsList();

  void set_old(QList<PageObjects>* _ptr_old_);
  QList<PageObjects>* get_old();
  void set_new(QList<PageObjects>* _ptr_new_);
  QList<PageObjects>* get_new();

  virtual void revert();

protected:
  QList<PageObjects> *ptr_old,*ptr_new;
  PageObjects tmp_obj;
  GraphObj tmp_graph;

};

#endif


