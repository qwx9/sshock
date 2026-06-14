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
//	Here is the bridge routine for maintenance and upkeep of the FTL models...
//	==========================================================================

#include "fixpp.h"
#include "edms_int.h"

//#ifdef EDMS_SHIPPABLE
////#include <mout.h>
//#endif

//	Here we need include files for each and every model that we'll be using...
//	==========================================================================
#include "ftl.h"

//	The physics handles definitions...
//	==================================
#include "physhand.h"

//	Data...
//	=======
extern EDMS_Argblock_Pointer A;
extern Q S[MAX_OBJ][7][4], I[MAX_OBJ][DOF_MAX];

//	Structs...
//	==========
//	Nope.

//	Here are the bridge routines to the models...
//	=============================================

//	Beam weapon ray caster...
//	=========================
physics_handle EDMS_beam_weapon(fix X[3], fix D[3], fix kick, fix knock, fix size, fix range, physics_handle exclude,
                                physics_handle shooter) {

    //	Return me, baby...
    //	------------------
    physics_handle ph = -1;
    int32_t EXCLUDE = 0;

    Q DD[3];

    Q Kick, Knock, Size, Range;

    Q *XX = (Q *)&X[0];

    DD[0] = Q_as_fix(D[0]);
    DD[1] = Q_as_fix(D[1]);
    DD[2] = Q_as_fix(D[2]);

    Kick = Q_as_fix(kick);
    Knock = Q_as_fix(knock);
    Size = Q_as_fix(size);
    Range = Q_as_fix(range);

    //	Is this a valid physics handle???
    //	=================================
    if (exclude > -1)
        EXCLUDE = ph2on[exclude];
    //	if ( exclude < 0 ) { EXCLUDE = -1; mout << "!EDMS: exclude warning is okay!\n"; }

    //		Do it...
    //		========
    ph = EDMS_cast_projectile(XX, DD, Kick, Knock, Size, Range, EXCLUDE, shooter);

    //      	Convert back to the goofbakk fixpoint system...
    //		-----------------------------------------------
    X[0] = Q_to_fix(XX[0]);
    X[1] = Q_to_fix(XX[1]);
    X[2] = Q_to_fix(XX[2]);

    //	Return a physics handle...
    //	==========================
    return ph;
}
