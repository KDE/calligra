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
void UndoRedoBaseClass::revert()
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
      _item_->revert();
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
      _item_->revert();
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
void UndoRedoPageObjects::revert()
{
  tmp = *ptr_new;
  *ptr_new = *ptr_old;
  *ptr_old = tmp;
  UndoRedoBaseClass::revert();
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
  ptr_old->clear();
  delete ptr_old;
  ptr_new->clear();
  delete ptr_old;
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
void UndoRedoPageObjectsList::revert()
{
  PageObjects *objPtr_new,*objPtr_old;

  for (objPtr_new = ptr_new->first(),objPtr_old = ptr_old->first();objPtr_new != 0 && objPtr_old != 0;
       objPtr_new = ptr_new->next(),objPtr_old = ptr_old->next())
    {
      tmp = *objPtr_new;
      *objPtr_new = *objPtr_old;
      *objPtr_old = tmp;
    }

  UndoRedoBaseClass::revert();
}












