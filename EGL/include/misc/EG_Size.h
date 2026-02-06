/*
 *                EGL 2025-2026 HydraSystems.
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
 * ====  ==========  ======= =====================================================
 * SJ    2025/08/18   1.a.1    Original by LVGL Kft
 *
 */

#pragma once

#include "../EG_IntrnlConfig.h"
#include <stdbool.h>
#include <stdint.h>
#include "EG_Types.h"

////////////////////////////////////////////////////////////////////////////////

#define EG_SCALE_NONE                       256        // Value for not zooming the image
EG_EXPORT_CONST_INT(EG_SCALE_NONE);

////////////////////////////////////////////////////////////////////////////////

class EGSize
{
public:
                      EGSize();
	                    EGSize(const EGSize &InSize);
	                    EGSize(const EGSize *pInSize);
	                    EGSize(EG_Coord_t X, EG_Coord_t Y);
	void                Set(EG_Coord_t X, EG_Coord_t Y);
  void                operator = (const EGSize &rval);
	void                operator += (const EGSize rval);
	void                operator -= (const EGSize rval);
	void                operator *= (const EGSize rval);
	void                operator++ (void);
	void                operator-- (void);
	bool                operator == (const EGSize &rval) const;

	EG_Coord_t          m_X;
	EG_Coord_t          m_Y;
};

///////////////////////////////////////////////////////////////////////////////////////

class EGScale : public EGSize
{
public:
                      EGScale(void);
	                    EGScale(EG_Coord_t X, EG_Coord_t Y) : EGSize(X, Y){};
	                    EGScale(EG_Coord_t XY) : EGSize(XY, XY){};
  bool                IsScaled(void) const;
  void                Normalise(void);
  void                Negate(void);
  EG_Coord_t          Minimum(void);
};

