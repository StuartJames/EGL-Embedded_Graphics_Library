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

EGList EGFragmentManager::m_Attached;  
EGList EGFragmentManager::m_Stack;        

///////////////////////////////////////////////////////////////////////////////

EGFragmentManager::EGFragmentManager(EGFragment *pParent)
{
	m_pParent = pParent;
}

///////////////////////////////////////////////////////////////////////////////

EGFragmentManager::~EGFragmentManager(void)
{
POSITION Pos = nullptr;
FragmentStates_t *pStates;

	for(pStates = (FragmentStates_t*)m_Attached.GetTail(Pos); pStates != nullptr; pStates = (FragmentStates_t*)m_Attached.GetPrev(Pos)){
		ItemDeleteObj(pStates);
		ItemDeleteFragment(pStates);
	}
  m_Attached.RemoveAll();
  m_Stack.RemoveAll();
}

///////////////////////////////////////////////////////////////////////////////

void EGFragmentManager::CreateObj(void)
{
POSITION Pos = nullptr;
FragmentStates_t *pStates = nullptr;

	StackItem_t *pTop = (StackItem_t*)m_Stack.GetTail();
	for(pStates = (FragmentStates_t*)m_Attached.GetTail(Pos); pStates != nullptr; pStates = (FragmentStates_t*)m_Attached.GetPrev(Pos)){
		if(pStates->InStack && pTop->pStates != pStates) {
			continue;			// Only create obj for top item in stack
		}
		ItemCreateObj(pStates);
	}
}

///////////////////////////////////////////////////////////////////////////////

void EGFragmentManager::DeleteObj(void)
{
POSITION Pos = nullptr;
FragmentStates_t *pStates = nullptr;

	for(pStates = (FragmentStates_t*)m_Attached.GetTail(Pos); pStates != nullptr; pStates = (FragmentStates_t*)m_Attached.GetPrev(Pos)){
		ItemDeleteObj(pStates);
	}
}

///////////////////////////////////////////////////////////////////////////////

void EGFragmentManager::Add(EGFragment *pFragment, EGObject *const *pContainer)
{
	FragmentStates_t *pStates = AttachFragment(pFragment, pContainer);
	if(!m_pParent || m_pParent->m_pManaged->ObjCreated) {
		ItemCreateObj(pStates);
	}
}

///////////////////////////////////////////////////////////////////////////////

void EGFragmentManager::Remove(EGFragment *pFragment)
{
POSITION Pos = nullptr;

	EG_ASSERT_NULL(pFragment);
	EG_ASSERT_NULL(pFragment->m_pManaged);
	EG_ASSERT(pFragment->m_pManaged->pManager == this);
	FragmentStates_t *pStates = pFragment->m_pManaged;
	FragmentStates_t *pPrevious = NULL;
	bool WasTop = false;
	if(pStates->InStack) {
		StackItem_t *pStackTop = (StackItem_t*)m_Stack.GetTail();
		StackItem_t *pItem = nullptr;
	  for(pItem = (StackItem_t*)m_Stack.GetTail(Pos); pItem != nullptr; pItem = (StackItem_t*)m_Stack.GetPrev(Pos)){
			if(pItem->pStates == pStates) {
				WasTop = pStackTop == pItem;
				StackItem_t *pStackPrev = (StackItem_t*)m_Stack.GetPrev(Pos);
				if(!pStackPrev) break;
				pPrevious = ((StackItem_t *)pStackPrev)->pStates;
				break;
			}
		}
		if(pItem) {
      if((Pos = m_Stack.Find(pItem)) != nullptr) m_Stack.RemoveAt(Pos);
			EG_FreeMem(pItem);
		}
	}
	ItemDeleteObj(pStates);
	ItemDeleteFragment(pStates);
  if((Pos = m_Attached.Find(pStates)) != nullptr) m_Attached.RemoveAt(Pos);
	EG_FreeMem(pStates);
	if(pPrevious && WasTop) {
		ItemCreateObj(pPrevious);
	}
}

///////////////////////////////////////////////////////////////////////////////

void EGFragmentManager::Push(EGFragment *pFragment, EGObject *const *pContainer)
{
  StackItem_t *pStackTop = (StackItem_t*)m_Stack.GetTail();
  if(pStackTop != nullptr) {
		ItemDeleteObj(pStackTop->pStates);
	}
	FragmentStates_t *pStates = AttachFragment(pFragment, pContainer);
	pStates->InStack = true;
	StackItem_t *pItem = (StackItem_t*)EG_AllocMem(sizeof(StackItem_t));
	EG_ZeroMem(pItem, sizeof(StackItem_t));
  m_Stack.AddTail(pItem);	// Add fragment to the top of the stack
	pItem->pStates = pStates;
	ItemCreateObj(pStates);
}

///////////////////////////////////////////////////////////////////////////////

bool EGFragmentManager::Pop(void)
{
	EGFragment *pTop = GetTop();
	if(pTop == NULL) return false;
	Remove(pTop);
	return true;
}

///////////////////////////////////////////////////////////////////////////////

void EGFragmentManager::Replace(EGFragment *pFragment, EGObject *const *pContainer)
{
	EGFragment *pTop = FindByContainer(*pContainer);
	if(pTop != nullptr) Remove(pTop);
	Add(pFragment, pContainer);
}

///////////////////////////////////////////////////////////////////////////////

bool EGFragmentManager::SendEvent(EGFragmentManager *pManager, int Code, void *pExtData)
{
FragmentStates_t *pStates = nullptr;
POSITION Pos = nullptr;

  for(pStates = (FragmentStates_t*)pManager->m_Attached.GetTail(Pos); pStates != nullptr; pStates = (FragmentStates_t*)pManager->m_Attached.GetPrev(Pos)){
		if(!pStates->ObjCreated || pStates->DestroyingObj) continue;
		EGFragment *pInstance = pStates->pInstance;
		if(!pInstance) continue;
		if(SendEvent(pInstance->m_pChildManager, Code, pExtData)) return true;
		if(pStates->pClass->EventCB && pStates->pClass->EventCB(pInstance, Code, pExtData)) return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////

size_t EGFragmentManager::GetStackSize(void)
{
	return m_Stack.GetSize();
}

///////////////////////////////////////////////////////////////////////////////

EGFragment* EGFragmentManager::GetTop(void)
{
  StackItem_t *pStackTop = (StackItem_t*)m_Stack.GetTail();
	if(!pStackTop) return nullptr;
	return pStackTop->pStates->pInstance;
}

///////////////////////////////////////////////////////////////////////////////

EGFragment* EGFragmentManager::FindByContainer(const EGObject *pContainer)
{
POSITION Pos = nullptr;
FragmentStates_t *pStates;

  for(pStates = (FragmentStates_t*)m_Attached.GetTail(Pos); pStates != nullptr; pStates = (FragmentStates_t*)m_Attached.GetPrev(Pos)){
		if(*pStates->pContainer == pContainer) return pStates->pInstance;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////

EGFragment* EGFragmentManager::GetParentFragment(void)
{
	return m_pParent;
}

///////////////////////////////////////////////////////////////////////////////

void EGFragmentManager::ItemCreateObj(FragmentStates_t *pItem)
{
	EG_ASSERT(pItem->pInstance);
	pItem->pInstance->CreateObj(pItem->pContainer ? *pItem->pContainer : nullptr);
}

///////////////////////////////////////////////////////////////////////////////

void EGFragmentManager::ItemDeleteObj(FragmentStates_t *pItem)
{
	pItem->pInstance->DeleteObj();
}

///////////////////////////////////////////////////////////////////////////////

void EGFragmentManager::ItemDeleteFragment(FragmentStates_t *pItem)
{
	EGFragment *pInstance = pItem->pInstance;
	if(pInstance->m_pClass->DetachedCB) {
		pInstance->m_pClass->DetachedCB(pInstance);
	}
	pInstance->m_pManaged = nullptr;
	delete pInstance;
	pItem->pInstance = nullptr;
}

///////////////////////////////////////////////////////////////////////////////

FragmentStates_t* EGFragmentManager::AttachFragment(EGFragment *pFragment,	EGObject *const *pContainer)
{
	EG_ASSERT(pFragment);
	EG_ASSERT(pFragment->m_pManaged == nullptr);
	FragmentStates_t *pStates = (FragmentStates_t*)EG_AllocMem(sizeof(FragmentStates_t));
	EG_ZeroMem(pStates, sizeof(FragmentStates_t));
  m_Attached.AddTail(pStates);
	pStates->pClass = pFragment->m_pClass;
	pStates->pManager = this;
	pStates->pContainer = pContainer;
	pStates->pInstance = pFragment;
	pFragment->m_pManaged = pStates;
	if(pFragment->m_pClass->AttachedCB) {
		pFragment->m_pClass->AttachedCB(pFragment);
	}
	return pStates;
}

#endif 
