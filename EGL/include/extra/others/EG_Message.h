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

#include "core/EG_Object.h"
#if EG_USE_MSG

#define LV_MSG_ID_ANY UINT32_MAX
EG_EXPORT_CONST_INT(LV_MSG_ID_ANY);

///////////////////////////////////////////////////////////////////////////////

typedef struct EG_Message_t{
  uint32_t       ID;            // Identifier of the message
  void          *pExtData;       // Set the the user_data set in `lv_msg_subscribe`
  void          *pPrivateData;  // Used internally
  const void    *pPayload;      // Pointer to the data of the message
} EG_Message_t;

typedef void (*EG_MessageSubscribeCB_t)(void *pSubscribe, EG_Message_t *pMessage);
typedef void (*EG_MessageRequestCB_t)(void *pRequest, uint32_t MessageID);

typedef struct SubscribeDiscriptor_t{
	uint32_t              MessageID;
	EG_MessageSubscribeCB_t Callback;
	void                  *pExtData;
	void                  *pPrivateData; // Internal: used only store 'obj' in lv_obj_subscribe
} SubscribeDiscriptor_t;

extern EG_EventCode_e EG_EVENT_MSG_RECEIVED;

///////////////////////////////////////////////////////////////////////////////

class EGMessage
{
public:
                          EGMessage(void) : m_pMessage(nullptr) {};
                          ~EGMessage(void);
  SubscribeDiscriptor_t*  Subsribe(uint32_t MessageID, EG_MessageSubscribeCB_t SubscribeCB, void *pExtData);
  SubscribeDiscriptor_t*  SubsribeObj(uint32_t MessageID, EGObject *pObj, void *pExtData);
  void                    Send(uint32_t MessageID, const void *pPayload);
  EG_Message_t*           GetMessage(EGEvent *pEvent);
  uint32_t                GetID(EG_Message_t *pMessage);
  const void*             GetPayload(EG_Message_t *pMessage);
  void*                   GetExtData(EG_Message_t *pMessage);
  void                    Notify(EG_Message_t *pMessage);

  static void             Initialise(void);
  static void             Unsubscribe(void *pSubscribe);
  static uint32_t         UnsubscribeObj(uint32_t MessageID, EGObject *pObj);
  static void             NotifyObjCB(void *pSubscribe, EG_Message_t *pMessage);
  static void             DeleteObjEventCB(EGEvent *pEvent);

private:
  EG_Message_t            *m_pMessage;
  static EGList           m_MessageList;
};

///////////////////////////////////////////////////////////////////////////////

#endif 