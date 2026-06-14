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

#include "fixpp.h"

Q END = Q_from_double(-9999.), VACUUM = Q_from_double(0.), MARBLE = Q_from_double(1.), FIELD_POINT = Q_from_double(2.), ROBOT = Q_from_int(3), BIPED = Q_from_int(4), PELVIS = Q_from_int(5), DEATH = Q_from_int(6), D_FRAME = Q_from_int(7);

int32_t min_physics_handle = 0;

//	That's it for now.
//	==================
