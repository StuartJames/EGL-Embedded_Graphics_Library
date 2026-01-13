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

#include "extra/others/EG_Fragment.h"

#if EG_USE_FRAGMENT

///////////////////////////////////////////////////////////////////////////////

static void DeleteAssertionCB(EGEvent *event);

///////////////////////////////////////////////////////////////////////////////

EGFragment::EGFragment(const EGFragmentClass_t *pClass, void *pArgs)
{
	EG_ASSERT_NULL(pClass);
	EG_ASSERT_NULL(pClass->CreateObjCB);
	EG_ASSERT(pClass->InstanceSize > 0);
	m_pClass = pClass;
	m_pChildManager = new EGFragmentManager(this);
	if(pClass->ConstructorCB) {
		pClass->ConstructorCB(this, pArgs);
	}
}

///////////////////////////////////////////////////////////////////////////////

EGFragment::~EGFragment(void)
{
	if(m_pManaged) {
		m_pManaged->pManager->Remove(this);
		return;
	}
	if(m_pObj) {
		DeleteObj();
	}
	// Objects will leak if this function called before objects deleted 
	const EGFragmentClass_t *pClass = m_pClass;
	if(pClass->DestructorCB) {
		pClass->DestructorCB(this);
	}
  delete m_pChildManager;
}

///////////////////////////////////////////////////////////////////////////////

EGFragmentManager* EGFragment::GetManager(void)
{
	EG_ASSERT_NULL(m_pManaged);
	return m_pManaged->pManager;
}

///////////////////////////////////////////////////////////////////////////////

const EGObject* EGFragment::GetContainer(void)
{
	EG_ASSERT_NULL(m_pManaged);
	return (EGObject*)m_pManaged->pContainer;
}

///////////////////////////////////////////////////////////////////////////////

EGFragment* EGFragment::GetParent(void)
{
	EG_ASSERT_NULL(m_pManaged);
	return m_pManaged->pManager->GetParentFragment();
}

///////////////////////////////////////////////////////////////////////////////

EGObject* EGFragment::CreateObj(EGObject *pContainer)
{
	FragmentStates_t *pStates = m_pManaged;
	if(pStates) {
		pStates->DestroyingObj = false;
	}
	const EGFragmentClass_t *pClass = m_pClass;
	EGObject *pObj = pClass->CreateObjCB(this, pContainer);
	EG_ASSERT_NULL(pObj);
	m_pObj = pObj;
	m_pChildManager->CreateObj();
	if(pStates) {
		pStates->ObjCreated = true;
		EGEvent::AddEventCB(pObj, DeleteAssertionCB, EG_EVENT_DELETE, NULL);
	}
	if(pClass->ObjCreatedCB) {
		pClass->ObjCreatedCB(this, pObj);
	}
	return m_pObj;
}

///////////////////////////////////////////////////////////////////////////////

void EGFragment::DeleteObj(void)
{
	m_pChildManager->DeleteObj();
	FragmentStates_t *pStates = m_pManaged;
	if(pStates) {
		if(!pStates->ObjCreated) return;
		pStates->DestroyingObj = true;
		bool cb_removed = EGEvent::RemoveEventCB(m_pObj, DeleteAssertionCB);
		EG_ASSERT(cb_removed);
	}
	EG_ASSERT_NULL(m_pObj);
	const EGFragmentClass_t *pClass = m_pClass;
	if(pClass->ObjWillDeleteCB) {
		pClass->ObjWillDeleteCB(this, m_pObj);
	}
	EGObject::Delete(m_pObj);
	if(pClass->ObjDeletedCB) {
		pClass->ObjDeletedCB(this, m_pObj);
	}
	if(pStates) {
		pStates->ObjCreated = false;
	}
	m_pObj = nullptr;
}

///////////////////////////////////////////////////////////////////////////////

void EGFragment::RecreateObj(void)
{
	EG_ASSERT_NULL(m_pManaged);
	DeleteObj();
	CreateObj(*m_pManaged->pContainer);
}

///////////////////////////////////////////////////////////////////////////////

static void DeleteAssertionCB(EGEvent *event)
{
	EG_UNUSED(event);
	EG_ASSERT_MSG(0, "Please delete objects with lv_fragment_destroy_obj");
}

#endif 
