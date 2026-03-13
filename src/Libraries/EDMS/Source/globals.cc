/*

Copyright (C) 2015-2018 Night Dive Studios, LLC.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
/*
 * $Header: n:/project/lib/src/edms/RCS/globals.cc 1.1 1994/02/28 17:07:14 roadkill Exp $
 */

//	This is the GLOBALS file for EDMS.  It's a girl?!
//	=================================================

#include "fix.h"

fix END = fix_from_float(-9999.), VACUUM = fix_from_float(0.), MARBLE = fix_from_float(1.), FIELD_POINT = fix_from_float(2.), ROBOT = fix_make(3,0), BIPED = fix_make(4,0), PELVIS = fix_make(5,0), DEATH = fix_make(6,0), D_FRAME = fix_make(7,0);

int32_t min_physics_handle = 0;

//	That's it for now.
//	==================
