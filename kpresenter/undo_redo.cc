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
/* Module: KPresenter Undo/Redo                                   */
/******************************************************************/

#include "undo_redo.h"
#include "undo_redo.moc"

/******************************************************************/
/* Class: UndoRedoBaseClass                                       */
/******************************************************************/

/*==================== default constructor =======================*/
UndoRedoBaseClass::UndoRedoBaseClass()
{
  description = "";
}
/*================= overloaded constructor =======================*/
UndoRedoBaseClass::UndoRedoBaseClass(QString _description_)
{
  description = _description_;
}

/*==================== destructor ================================*/
UndoRedoBaseClass::~UndoRedoBaseClass()
{
}
  
/*==================== set description ===========================*/
void UndoRedoBaseClass::set_description(QString _description_)
{
  description = _description_;
}

/*==================== get description ===========================*/
QString UndoRedoBaseClass::get_description()
{
  return description;
}

/*==================== revert (do undo) ==========================*/
void UndoRedoBaseClass::revert(Action)
{
}

/******************************************************************/
/* Class: UndoRedoStack                                           */
/******************************************************************/

/*====================== set maximal level =======================*/
template<class T>
void UndoRedoStack<T>::set_max_level(int _max_level_)
{
  max_level = _max_level_;
  check_max_level();
}

/*====================== get maximal level =======================*/
template<class T>
int UndoRedoStack<T>::get_max_level()
{
  return max_level;
}

/*======================= push item on stack =====================*/
template<class T>
void UndoRedoStack<T>::push(T *_item_)
{
  insert(0,_item_);
  check_max_level();
}

/*======================== pop item from stack ===================*/
template <class T>
T* UndoRedoStack<T>::pop()
{
  if (!isEmpty()) return take(0);
  else return 0;
}

/*====================== void check maximal level ================*/
template<class T>
void UndoRedoStack<T>::check_max_level()
{
  if (count() > (unsigned int)max_level + 1)
    remove(count() - 1);
}

/******************************************************************/
/* Class: UndoRedoAdmin                                           */
/******************************************************************/

/*==================== default constructor =======================*/
UndoRedoAdmin::UndoRedoAdmin(int max_level = 64)
{
  undo_stack.setAutoDelete(true);
  undo_stack.set_max_level(max_level);
  redo_stack.setAutoDelete(true);
  redo_stack.set_max_level(max_level);
}

/*======================= destructor =============================*/
UndoRedoAdmin::~UndoRedoAdmin()
{
  undo_stack.clear();
  redo_stack.clear();
}

/*======================= add action =============================*/
void UndoRedoAdmin::add_undo_item(UndoRedoBaseClass *_action_)
{
  undo_stack.push(_action_);

  emit undo_redo_change((undo_stack.isEmpty() ? QString("") : undo_stack.at(0)->get_description()),!undo_stack.isEmpty(),
			(redo_stack.isEmpty() ? QString("") : redo_stack.at(0)->get_description()),!redo_stack.isEmpty());
}

/*======================== undo ==================================*/
void UndoRedoAdmin::undo()
{
  UndoRedoBaseClass *_item_;
  
  _item_ = undo_stack.pop();

  if (!_item_) warning("Couldn't do undo!");
  else
    {
      _item_->revert(UndoRedoBaseClass::UNDO);
      redo_stack.push(_item_);
    }

  emit undo_redo_change((undo_stack.isEmpty() ? QString("") : undo_stack.at(0)->get_description()),!undo_stack.isEmpty(),
			(redo_stack.isEmpty() ? QString("") : redo_stack.at(0)->get_description()),!redo_stack.isEmpty());
}

/*======================== redo ==================================*/
void UndoRedoAdmin::redo()
{
  UndoRedoBaseClass *_item_;
  
  _item_ = redo_stack.pop();

  if (!_item_) warning("Couldn't do redo!");
  else
    {
      _item_->revert(UndoRedoBaseClass::REDO);
      undo_stack.push(_item_);
    }

  emit undo_redo_change((undo_stack.isEmpty() ? QString("") : undo_stack.at(0)->get_description()),!undo_stack.isEmpty(),
			(redo_stack.isEmpty() ? QString("") : redo_stack.at(0)->get_description()),!redo_stack.isEmpty());
}

/******************************************************************/
/* Class: UndoRedoPageObjects                                     */
/******************************************************************/

/*======================== constructor ===========================*/
UndoRedoPageObjects::UndoRedoPageObjects(PageObjects *_ptr_old_,PageObjects *_ptr_new_,QString _description_)
  : UndoRedoBaseClass(_description_)
{
  ptr_old = _ptr_old_;
  ptr_new = _ptr_new_;
}

/*======================== destructor ============================*/
UndoRedoPageObjects::~UndoRedoPageObjects()
{
  if (ptr_old->graphObj)
    delete ptr_old->graphObj;
  if (ptr_old->textObj)
    delete ptr_old->textObj;
  delete ptr_old;
}

/*========================== set old =============================*/
void UndoRedoPageObjects::set_old(PageObjects* _ptr_old_)
{
  ptr_old = _ptr_old_;
}

/*========================= get old ==============================*/
PageObjects* UndoRedoPageObjects::get_old()
{
  return ptr_old;
}

/*========================= set new ==============================*/
void UndoRedoPageObjects::set_new(PageObjects* _ptr_new_)
{
  ptr_new = _ptr_new_;
}

/*========================== get new =============================*/
PageObjects* UndoRedoPageObjects::get_new()
{
  return ptr_new;
}

/*========================== do revert ===========================*/
void UndoRedoPageObjects::revert(Action a)
{
  if (ptr_new->graphObj)
    tmp_graph = *ptr_new->graphObj;
  if (ptr_new->textObj)
    tmp_txt = *ptr_new->textObj;
  tmp_obj = *ptr_new;

  if (ptr_old->textObj)
    *ptr_new->textObj = *ptr_old->textObj;
  *ptr_new = *ptr_old;
  if (ptr_old->graphObj)
    *ptr_new->graphObj = *ptr_old->graphObj;
      
  if (ptr_new->textObj)
    *ptr_old->textObj = tmp_txt;
  *ptr_old = tmp_obj;
  if (ptr_new->graphObj)
    *ptr_old->graphObj = tmp_graph;

  UndoRedoBaseClass::revert(a);
}

/******************************************************************/
/* Class: UndoRedoPageObjectsList                                 */
/******************************************************************/

/*======================== constructor ===========================*/
UndoRedoPageObjectsList::UndoRedoPageObjectsList(QList<PageObjects> *_ptr_old_,QList<PageObjects> *_ptr_new_,QString _description_)
  : UndoRedoBaseClass(_description_)
{
  ptr_old = _ptr_old_;
  ptr_new = _ptr_new_;
}

/*======================== destructor ============================*/
UndoRedoPageObjectsList::~UndoRedoPageObjectsList()
{
  PageObjects *objPtr_old;

  for (objPtr_old = ptr_old->first();objPtr_old != 0;objPtr_old = ptr_old->next())
    {
      if (objPtr_old->graphObj)
	delete objPtr_old->graphObj;
     if (objPtr_old->textObj)
	delete objPtr_old->textObj;
    }

  ptr_old->clear();
}

/*========================== set old =============================*/
void UndoRedoPageObjectsList::set_old(QList<PageObjects> *_ptr_old_)
{
  ptr_old = _ptr_old_;
}

/*========================= get old ==============================*/
QList<PageObjects>* UndoRedoPageObjectsList::get_old()
{
  return ptr_old;
}

/*========================= set new ==============================*/
void UndoRedoPageObjectsList::set_new(QList<PageObjects>* _ptr_new_)
{
  ptr_new = _ptr_new_;
}

/*========================== get new =============================*/
QList<PageObjects>* UndoRedoPageObjectsList::get_new()
{
  return ptr_new;
}

/*========================== do revert ===========================*/
void UndoRedoPageObjectsList::revert(Action a)
{
  PageObjects *objPtr_new,*objPtr_old;

  for (objPtr_new = ptr_new->first(),objPtr_old = ptr_old->first();objPtr_new != 0 && objPtr_old != 0;
       objPtr_new = ptr_new->next(),objPtr_old = ptr_old->next())
    {
      if (objPtr_new->graphObj)
	tmp_graph = *objPtr_new->graphObj;
      if (objPtr_new->textObj)
	tmp_txt = *objPtr_new->textObj;
      tmp_obj = *objPtr_new;

      if (objPtr_old->textObj)
	*objPtr_new->textObj = *objPtr_old->textObj;
      *objPtr_new = *objPtr_old;
      if (objPtr_old->graphObj)
	*objPtr_new->graphObj = *objPtr_old->graphObj;
     
      if (objPtr_new->textObj)
	*objPtr_old->textObj = tmp_txt;
      *objPtr_old = tmp_obj;
      if (objPtr_new->graphObj)
	*objPtr_old->graphObj = tmp_graph;
    }

  UndoRedoBaseClass::revert(a);
}

/******************************************************************/
/* Class: UndoRedoInsertPageObject                                */
/******************************************************************/

/*=================== constructor ================================*/
UndoRedoInsertPageObject::UndoRedoInsertPageObject(QList<PageObjects> *_ptr_list_,PageObjects *_ptr_obj_,QString _description_)
  : UndoRedoBaseClass(_description_)
{
  ptr_list = _ptr_list_;
  ptr_obj = _ptr_obj_;
}

/*===================== destructor ===============================*/
UndoRedoInsertPageObject::~UndoRedoInsertPageObject()
{
  if (ptr_list && ptr_obj)
    {
      if (ptr_list->findRef(ptr_obj) == -1)
	{
	  if (ptr_obj->textObj)
	    delete ptr_obj->textObj;
	  if (ptr_obj->graphObj)
	    delete ptr_obj->graphObj;
	  delete ptr_obj;
	}
    }
}

/*======================== set list ==============================*/
void UndoRedoInsertPageObject::set_list(QList<PageObjects>* _ptr_list_)
{
  ptr_list = _ptr_list_;
}

/*========================== get list ============================*/
QList<PageObjects>* UndoRedoInsertPageObject::get_list()
{
  return ptr_list;
}

/*========================== set obj =============================*/
void UndoRedoInsertPageObject::set_obj(PageObjects* _ptr_obj_)
{
  ptr_obj = _ptr_obj_;
}

/*========================== get obj =============================*/
PageObjects* UndoRedoInsertPageObject::get_obj()
{
  return ptr_obj;
}

/*======================== revert ================================*/
void UndoRedoInsertPageObject::revert(Action a)
{
  switch (a)
    {
    case UNDO:
      {
	if (ptr_list && ptr_obj && !ptr_list->isEmpty())
	  {
	    int index = ptr_list->findRef(ptr_obj);
	    if (index != -1)
	      ptr_list->take(index);
	      
	  }
      } break;
    case REDO:
      {
	if (ptr_list && ptr_obj)
	  ptr_list->append(ptr_obj);
      } break;
    }

  UndoRedoBaseClass::revert(a);
}

/******************************************************************/
/* Class: UndoRedoDeletePageObjects                               */
/******************************************************************/

/*=================== constructor ================================*/
UndoRedoDeletePageObjects::UndoRedoDeletePageObjects(QList<PageObjects> *_ptr_list_,QList<PageObjects> *_ptr_objs_,QString _description_)
  : UndoRedoBaseClass(_description_)
{
  ptr_list = _ptr_list_;
  ptr_objs = _ptr_objs_;
}

/*===================== destructor ===============================*/
UndoRedoDeletePageObjects::~UndoRedoDeletePageObjects()
{
  if (ptr_list && ptr_objs && !ptr_list->isEmpty() && !ptr_objs->isEmpty())
    {
//       PageObjects *po;
//       int index;
      
//       for (po = ptr_objs->first();po != 0;po = ptr_objs->next())
// 	{
// 	  index = ptr_list->findRef(po);
// 	  if (index == -1) 
// 	    {
// 	      if (po->graphObj)
// 		delete po->graphObj;
// 	      if (po->textObj)
// 		delete po->textObj;
// 	      delete po;
// 	    }
// 	}

      ptr_objs->clear();
    }
}

/*======================== set list ==============================*/
void UndoRedoDeletePageObjects::set_list(QList<PageObjects>* _ptr_list_)
{
  ptr_list = _ptr_list_;
}

/*========================== get list ============================*/
QList<PageObjects>* UndoRedoDeletePageObjects::get_list()
{
  return ptr_list;
}

/*========================== set objs ============================*/
void UndoRedoDeletePageObjects::set_objs(QList<PageObjects>* _ptr_objs_)
{
  ptr_objs = _ptr_objs_;
}

/*========================== get objs ============================*/
QList<PageObjects>* UndoRedoDeletePageObjects::get_objs()
{
  return ptr_objs;
}

/*======================== revert ================================*/
void UndoRedoDeletePageObjects::revert(Action a)
{
  switch (a)
    {
    case UNDO:
      {
	if (ptr_list && ptr_objs && !ptr_objs->isEmpty())
	  {
	    PageObjects *po;

	    for (po = ptr_objs->first();po != 0;po = ptr_objs->next())
	      ptr_list->append(po);
	  }
      } break;
    case REDO:
      {
	if (ptr_list && ptr_objs && !ptr_list->isEmpty() && !ptr_objs->isEmpty())
	  {
	    PageObjects *po;
	    int index;

	    for (po = ptr_objs->first();po != 0;po = ptr_objs->next())
	      {
		index = ptr_list->findRef(po);
		if (index != -1) ptr_list->take(index);
	      }
	  }
 	
      } break;
    }

  UndoRedoBaseClass::revert(a);
}












