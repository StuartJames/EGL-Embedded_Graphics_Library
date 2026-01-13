/*
 *                LEGL 2025-2026 HydraSystems.
 *
 *  This program is free software; you can redistribute it and/or   
 *  modify it under the terms of the GNU General Public License as  
 *  published by the Free Software Foundation; either version 2 of  
 *  the License, or (at your option) any later version.             
 *                                                                  
 *  This program is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of  
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   
 *  GNU General Public License for more details.                    
 * 
 *  Based on a design by LVGL Kft
 * 
 * =====================================================================
 *
 * Edit     Date     Version       Edit Description
 * ====  ==========  ======= ===========================================
 * SJ    2025/08/18   1.a.1    Original by LVGL Kft
 *
 */

#pragma once

#include "EG_IntrnlConfig.h"

#if EG_USE_FRAGMENT

#include "core/EG_Object.h"

///////////////////////////////////////////////////////////////////////////////

class EGFragment;
class EGFragmentManager;

typedef struct EGFragmentClass_t
{
	void        (*ConstructorCB)(EGFragment *self, void *args);
	void        (*DestructorCB)(EGFragment *self);
	void        (*AttachedCB)(EGFragment *self); // Fragment attached to manager
	void        (*DetachedCB)(EGFragment *self);	// Fragment detached from manager
	EGObject*   (*CreateObjCB)(EGFragment *self, EGObject *pContainer);
	void        (*ObjCreatedCB)(EGFragment *self, EGObject *obj);
	void        (*ObjWillDeleteCB)(EGFragment *self, EGObject *obj);// Called before objects in the fragment will be deleted.
	void        (*ObjDeletedCB)(EGFragment *self, EGObject *obj);   // Called when the object created by fragment received `EG_EVENT_DELETE` event
	bool        (*EventCB)(EGFragment *self, int code, void *userdata);

	size_t      InstanceSize;
} EGFragmentClass_t;


typedef struct FragmentStates_t {
	const EGFragmentClass_t *pClass;          // Class of the fragment
	EGFragmentManager       *pManager;        // Manager the fragment is attached to
	EGObject *const         *pContainer;	    // Container object the fragment adding view to
	EGFragment              *pInstance;	      // Fragment instance
	bool                    ObjCreated;       // true between `create_obj_cb` and `obj_deleted_cb`
	bool                    DestroyingObj;	  // true before `lv_fragment_del_obj` is called. Don't touch any object if this is true
	bool                    InStack;          // true if this fragment is in navigation stack that can be popped
} FragmentStates_t;

typedef struct StackItem_t {
	FragmentStates_t *pStates;
} StackItem_t;


///////////////////////////////////////////////////////////////////////////////

class EGFragmentManager
{
public:
	                    EGFragmentManager(void){};
	                    EGFragmentManager(EGFragment *parent);
	                    ~EGFragmentManager(void);
  void                CreateObj(void);
  void                DeleteObj(void);
  void                Add(EGFragment *pFragment, EGObject *const *pContainer);
  void                Remove(EGFragment *pFragment);
  void                Push(EGFragment *pFragment, EGObject *const *pContainer);
  bool                Pop(void);
  void                Replace(EGFragment *pFragment, EGObject *const *pContainer);
  size_t              GetStackSize(void);
  EGFragment*         GetTop(void);
  EGFragment*         FindByContainer(const EGObject *pContainer);
  EGFragment*         GetParentFragment(void);

  static bool         SendEvent(EGFragmentManager *pManager, int Code, void *pExtData);

private:
	void                ItemCreateObj(FragmentStates_t *item);
	void                ItemDeleteObj(FragmentStates_t *item);
	void                ItemDeleteFragment(FragmentStates_t *item);
	FragmentStates_t*   AttachFragment(EGFragment *pFragment,	EGObject *const *pContainer);

	EGFragment          *m_pParent;
	static EGList       m_Attached;      // Linked list to store attached fragments
	static EGList       m_Stack;         // Linked list to store fragments in stack
};

///////////////////////////////////////////////////////////////////////////////

class EGFragment
{
public:
                            EGFragment(const EGFragmentClass_t *pClass, void *pArgs);
                            ~EGFragment(void);
  EGFragmentManager*        GetManager(void);
  const EGObject*           GetContainer(void);
  EGFragment*               GetParent(void);
  EGObject*                 CreateObj(EGObject *pContainer);
  void                      DeleteObj(void);
  void                      RecreateObj(void);


	const EGFragmentClass_t   *m_pClass;	// Class of this fragment
	FragmentStates_t          *m_pManaged;	// Managed fragment states. If not null, then this fragment is managed.
	EGFragmentManager         *m_pChildManager;
	EGObject                  *m_pObj;
};

///////////////////////////////////////////////////////////////////////////////

#endif
