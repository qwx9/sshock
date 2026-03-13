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
//	Here is the bridge routine for maintenance and upkeep of the dirac frame models...
//	==================================================================================

#include "fix.h"
#include "edms_int.h"

//      This matrix is for Douggie and his magic circus...
//      ==================================================
fix Dirac_basis[9];

//	Here we need include files for each and every model that we'll be using...
//	==========================================================================
#include "d_frame.h"

//	The physics handles definitions...
//	==================================
#include "physhand.h"

//	Data...
//	=======
#include "edms_mod.h"

//	Structs...
//	==========

//	Dirac Frame...
//	--------------
typedef struct {

    fix mass, hardness, roughness, gravity;

    fix corners[10][4];

} Dirac_frame;

//	Here we go...
//	=============
#define DIRAC_HARD_FAC 10

//      Hack hack hack...
//      =================
static fix size = fix_from_float(.3);

//      Thank God we have only 16 bits of fraction!
//      ===========================================
static fix old_state[7], new_state[7];

//	Here are the bridge routines to the models...
//	=============================================

//      Dirac Frame routines...
//	=======================
physics_handle EDMS_make_Dirac_frame(Dirac_frame *d, State *s) {

    //	Variables for tha actual conversion...
    //	--------------------------------------
    fix params[10], init_state[6][3];

    fix mass, hardness, gravity, roughness;

    int32_t on = 0;

    physics_handle ph = 0;

    init_state[0][0] = s->X;
    init_state[0][1] = s->X_dot;
    init_state[1][0] = s->Y;
    init_state[1][1] = s->Y_dot;
    init_state[2][0] = s->Z;
    init_state[2][1] = s->Z_dot;
    init_state[3][0] = s->alpha;
    init_state[3][1] = s->alpha_dot;
    init_state[4][0] = s->beta;
    init_state[4][1] = s->beta_dot;
    init_state[5][0] = s->gamma;
    init_state[5][1] = s->gamma_dot;

    mass = d->mass;
    roughness = d->roughness;
    hardness = d->hardness;
    gravity = d->gravity;

    //	hardness = hardness*(mass*4/size);
    hardness = fix_mul(hardness, fix_mul_div(mass, DIRAC_HARD_FAC, size)); // Node Size goes here!!!

    params[0] = mass;
    params[1] = fix_div(FIX_UNIT, mass);
    params[2] = fix_div(FIX_UNIT,
    	fix_mul(fix_mul(fix_mul(fix_from_float(.4), mass), size), size));
    params[3] = hardness;
    params[4] = fix_mul(fix_sqrt(params[3]), fix_sqrt(mass));
    //	params[4] = 20*sqrt( params[0] * mass );
    params[5] = roughness;
    params[6] = fix_from_float(.2);
    params[7] = 0; // gravity;
    params[8] = 0;
    params[9] = 0;

    //		Now actulally DO the dirty work...
    //		----------------------------------
    on = make_Dirac_frame(init_state, params);
    ph = EDMS_bind_object_number(on);

    return ph;
}

//      At some point we need the viewpoint offered by the neck...
//      ==========================================================
void EDMS_get_Dirac_frame_viewpoint(physics_handle ph, State *s) {

    //      For getting the new basis...
    //      ----------------------------
    void render_localize(fix *X, fix *Y, fix *Z, int);

    //	For Euler angle conversion...
    //	-----------------------------
    fix dirac_temp[9];
    fix alpha, beta, gamma;

    int32_t on = ph2on[ph];

    fix delta = 0;

    if (I[on][30] == D_FRAME) {

        new_state[0] = (S[on][0][0]);
        new_state[1] = (S[on][1][0]);
        new_state[2] = (S[on][2][0]);

        new_state[3] = S[on][3][0];
        new_state[4] = S[on][4][0];
        new_state[5] = S[on][5][0];
        new_state[6] = S[on][6][0];

		delta += fix_mul(new_state[0] - old_state[0], new_state[0] - old_state[0]);
		delta += fix_mul(new_state[1] - old_state[1], new_state[1] - old_state[1]);
		delta += fix_mul(new_state[2] - old_state[2], new_state[2] - old_state[2]);
		delta += fix_mul(new_state[3] - old_state[3], new_state[3] - old_state[3]);
		delta += fix_mul(new_state[4] - old_state[4], new_state[4] - old_state[4]);
		delta += fix_mul(new_state[5] - old_state[5], new_state[5] - old_state[5]);
		delta += fix_mul(new_state[6] - old_state[6], new_state[6] - old_state[6]);

        if (delta > fix_from_float(.00003)) {

            old_state[0] = new_state[0];
            old_state[1] = new_state[1];
            old_state[2] = new_state[2];

            old_state[3] = new_state[3];
            old_state[4] = new_state[4];
            old_state[5] = new_state[5];
            old_state[6] = new_state[6];
        }

        s->X = old_state[0];
        s->Y = old_state[1];
        s->Z = old_state[2];

        EDMS_get_Euler_angles(&alpha, &beta, &gamma, on);

        s->alpha = -gamma;
        s->beta = -alpha;
        s->gamma = -beta;

        //      Set up global vectors...
        //      ------------------------
        dirac_temp[0] = dirac_temp[1] = dirac_temp[2] = dirac_temp[3] = dirac_temp[4] = dirac_temp[5] = dirac_temp[6] =
            dirac_temp[7] = dirac_temp[8] = 0;

        dirac_temp[0] = FIX_UNIT;
        dirac_temp[4] = FIX_UNIT;
        dirac_temp[8] = FIX_UNIT;

        //      Transform to the new basis...
        //      -----------------------------
        render_localize(&dirac_temp[0], &dirac_temp[1], &dirac_temp[2], on);
        render_localize(&dirac_temp[3], &dirac_temp[4], &dirac_temp[5], on);
        render_localize(&dirac_temp[6], &dirac_temp[7], &dirac_temp[8], on);

        //      Stuff into Matt's order...
        //      --------------------------
        /*
                Dirac_basis[0] = dirac_temp[0];
                Dirac_basis[1] =-dirac_temp[6];
                Dirac_basis[2] = dirac_temp[3];
                Dirac_basis[3] =-dirac_temp[2];
                Dirac_basis[4] = dirac_temp[8];
                Dirac_basis[5] =-dirac_temp[5];
                Dirac_basis[6] = dirac_temp[1];
                Dirac_basis[7] =-dirac_temp[7];
                Dirac_basis[8] = dirac_temp[4];
        */

        //      Almost...
        /*
                Dirac_basis[0] = dirac_temp[3];
                Dirac_basis[1] =-dirac_temp[6];
                Dirac_basis[2] =-dirac_temp[0];
                Dirac_basis[3] =-dirac_temp[5];
                Dirac_basis[4] = dirac_temp[8];
                Dirac_basis[5] = dirac_temp[2];
                Dirac_basis[6] = dirac_temp[4];
                Dirac_basis[7] =-dirac_temp[7];
                Dirac_basis[8] =-dirac_temp[1];
        */

        Dirac_basis[0] = -dirac_temp[3];
        Dirac_basis[1] = -dirac_temp[6];
        Dirac_basis[2] =  dirac_temp[0];
        Dirac_basis[3] =  dirac_temp[5];
        Dirac_basis[4] =  dirac_temp[8];
        Dirac_basis[5] = -dirac_temp[2];
        Dirac_basis[6] = -dirac_temp[4];
        Dirac_basis[7] = -dirac_temp[7];
        Dirac_basis[8] =  dirac_temp[1];

    } // End of check for Dirac_frame or not...
}

//	Utilities for the weak spirited...
//	==================================
void EDMS_set_Dirac_frame_parameters(physics_handle ph, Dirac_frame *d) {

    fix mass, hardness, roughness, gravity;

    mass = d->mass;
    hardness = d->hardness;
    gravity = d->gravity;
    roughness = d->roughness;

    int32_t on = physics_handle_to_object_number(ph);

    hardness = fix_mul(hardness, fix_mul_div(mass, DIRAC_HARD_FAC, size));
    I[on][20] = mass;
    I[on][21] = fix_div(FIX_UNIT, mass);
    I[on][22] = fix_div(FIX_UNIT,
    	fix_mul(fix_mul(fix_mul(fix_from_float(.4), mass), size), size));
    I[on][23] = hardness;
    I[on][24] = fix_mul(fix_sqrt(I[on][23]), fix_sqrt(mass));
    //	I[on][24] = 20*sqrt( I[on][20] * mass );
    I[on][25] = roughness;
    I[on][26] = fix_from_float(.2);
    I[on][27] = 0; // gravity;
    I[on][28] = 0;
    I[on][29] = 0;

    //	Done!
    //	-----
}

//	And the weak minded...
//	======================
void EDMS_get_Dirac_frame_parameters(physics_handle ph, Dirac_frame *d) {
    int32_t on = physics_handle_to_object_number(ph);

    d->roughness = fix_div(I[on][23], I[on][26]);
    d->hardness = fix_mul(fix_div(I[on][26], I[on][20]), DIRAC_HARD_FAC);
    d->mass = I[on][20];
    d->gravity = I[on][27];
}

void EDMS_control_Dirac_frame(physics_handle ph, fix forward, fix pitch, fix yaw, fix roll) {

    int32_t on = ph2on[ph];

    fix F, P, Y, R;

    F = forward;

    //      System shock angle order, definition...
    //      =======================================
    R = roll;
    P = yaw;
    Y = pitch;
    Y = -Y;

    control_dirac_frame(on, F, P, Y, R);
}

// Access to the Dirac matrix for the main game.
fix *EDMS_Dirac_basis(void) {
    return Dirac_basis;
}

void render_localize(fix *X, fix *Y, fix *Z, int32_t object) {

    fix e0, e1, e2, e3, fx, fy, fz;

    fix x = *X, y = *Y, z = *Z;

    e0 = S[object][3][0];
    e1 = S[object][4][0];
    e2 = S[object][5][0];
    e3 = S[object][6][0];

    //	Go for it, sonny...
    //	-------------------
    fx = fix_mul(e0, e0) + fix_mul(e1, e1) - fix_mul(e2, e2) - fix_mul(e3, e3);
    fy = fix_mul(e1, e2) - fix_mul(e0, e3);
    fy = fix_mul(fix_make(2, 0), fy);
    fz = fix_mul(e1, e3) + fix_mul(e0, e2);
    fz = fix_mul(fix_make(2, 0), fz);
    *X = fix_mul(x, fx) + fix_mul(y, fy) + fix_mul(z, fz);

    fx = fix_mul(e1, e2) + fix_mul(e0, e3);
    fx = fix_mul(fix_make(2, 0), fx);
    fy = fix_mul(e0, e0) - fix_mul(e1, e1) + fix_mul(e2, e2) - fix_mul(e3, e3);
    fz = fix_mul(e2, e3) - fix_mul(e0, e1);
    fz = fix_mul(fix_make(2, 0), fz);
    *Y = fix_mul(x, fx) + fix_mul(y, fy) + fix_mul(z, fz);

    fx = fix_mul(-e0, e2) + fix_mul(e1, e3);
    fx = fix_mul(fix_make(2, 0), fx);
    fy = fix_mul(e2, e3) + fix_mul(e0, e1);
    fy = fix_mul(fix_make(2, 0), fy);
    fz = fix_mul(e0, e0) - fix_mul(e1, e1) - fix_mul(e2, e2) + fix_mul(e3, e3);
    *Z = fix_mul(x, fx) + fix_mul(y, fy) + fix_mul(z, fz);
}
