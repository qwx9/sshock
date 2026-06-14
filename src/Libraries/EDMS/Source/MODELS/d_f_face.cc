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

#include "fixpp.h"
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
#define DIRAC_HARD_FAC Q_as_int(10)

//      Hack hack hack...
//      =================
static Q size = Q_from_double(.3);

//      Thank God we have only 16 bits of fraction!
//      ===========================================
static Q old_state[7], new_state[7];

//	Here are the bridge routines to the models...
//	=============================================

//      Dirac Frame routines...
//	=======================
physics_handle EDMS_make_Dirac_frame(Dirac_frame *d, State *s) {

    //	Variables for tha actual conversion...
    //	--------------------------------------
    Q params[10], init_state[6][3];

    Q mass, hardness, gravity, roughness;

    int32_t on = 0;

    physics_handle ph = 0;

    init_state[0][0] = Q_as_fix(s->X);
    init_state[0][1] = Q_as_fix(s->X_dot);
    init_state[1][0] = Q_as_fix(s->Y);
    init_state[1][1] = Q_as_fix(s->Y_dot);
    init_state[2][0] = Q_as_fix(s->Z);
    init_state[2][1] = Q_as_fix(s->Z_dot);
    init_state[3][0] = Q_as_fix(s->alpha);
    init_state[3][1] = Q_as_fix(s->alpha_dot);
    init_state[4][0] = Q_as_fix(s->beta);
    init_state[4][1] = Q_as_fix(s->beta_dot);
    init_state[5][0] = Q_as_fix(s->gamma);
    init_state[5][1] = Q_as_fix(s->gamma_dot);

    mass = Q_as_fix(d->mass);
    roughness = Q_as_fix(d->roughness);
    hardness = Q_as_fix(d->hardness);
    gravity = Q_as_fix(d->gravity);

    //	hardness = hardness*(mass*4/size);
    hardness = Q_mul(hardness, (Q_div(Q_mul(mass, DIRAC_HARD_FAC), size))); // Node Size goes here!!!

    params[0] = mass;
    params[1] = Q_div(Q_as_int(1), mass);
    params[2] = Q_div(Q_as_int(1), (Q_mul(Q_mul(Q_mul(Q_as_double(.4), mass), size), size)));
    params[3] = hardness;
    params[4] = Q_mul(Q_sqrt(params[3]), Q_sqrt(mass));
    //	params[4] = 20*sqrt( params[0] * mass );
    params[5] = roughness;
    params[6] = Q_as_double(.2);
    params[7] = Q_as_int(0); // gravity;
    params[8] = Q_as_int(0);
    params[9] = Q_as_int(0);

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
    // void render_globalize( Q &X, Q &Y, Q &Z, int );
    void render_localize(Q * X, Q * Y, Q * Z, int);

    //	For Euler angle conversion...
    //	-----------------------------
    Q dirac_temp[9];
    Q alpha, beta, gamma;

    int32_t on = ph2on[ph];

    Q delta = Q_as_int(0);

    if (I[on][30].val == D_FRAME.val) {

        new_state[0] = (S[on][0][0]);
        new_state[1] = (S[on][1][0]);
        new_state[2] = (S[on][2][0]);

        new_state[3] = S[on][3][0];
        new_state[4] = S[on][4][0];
        new_state[5] = S[on][5][0];
        new_state[6] = S[on][6][0];

        delta = Q_add(Q_add(Q_add(Q_add(Q_add(Q_add(
        		Q_mul((Q_sub(new_state[0], old_state[0])), (Q_sub(new_state[0], old_state[0]))),
                Q_mul((Q_sub(new_state[1], old_state[1])), (Q_sub(new_state[1], old_state[1])))),
                Q_mul((Q_sub(new_state[2], old_state[2])), (Q_sub(new_state[2], old_state[2])))),
                Q_mul((Q_sub(new_state[3], old_state[3])), (Q_sub(new_state[3], old_state[3])))),
                Q_mul((Q_sub(new_state[4], old_state[4])), (Q_sub(new_state[4], old_state[4])))),
                Q_mul((Q_sub(new_state[5], old_state[5])), (Q_sub(new_state[5], old_state[5])))),
                Q_mul((Q_sub(new_state[6], old_state[6])), (Q_sub(new_state[6], old_state[6]))));

        if (delta.val > Q_as_double(.00003).val) {

            old_state[0] = new_state[0];
            old_state[1] = new_state[1];
            old_state[2] = new_state[2];

            old_state[3] = new_state[3];
            old_state[4] = new_state[4];
            old_state[5] = new_state[5];
            old_state[6] = new_state[6];
        }

        s->X = Q_to_fix(old_state[0]);
        s->Y = Q_to_fix(old_state[1]);
        s->Z = Q_to_fix(old_state[2]);

        EDMS_get_Euler_angles(&alpha, &beta, &gamma, on);

        s->alpha = -Q_to_fix(gamma);
        s->beta = -Q_to_fix(alpha);
        s->gamma = -Q_to_fix(beta);

        //      Set up global vectors...
        //      ------------------------
        dirac_temp[0] = dirac_temp[1] = dirac_temp[2] = dirac_temp[3] = dirac_temp[4] = dirac_temp[5] = dirac_temp[6] =
            dirac_temp[7] = dirac_temp[8] = Q_as_int(0);

        dirac_temp[0] = Q_as_int(1);
        dirac_temp[4] = Q_as_int(1);
        dirac_temp[8] = Q_as_int(1);

        //      Transform to the new basis...
        //      -----------------------------
        render_localize(&dirac_temp[0], &dirac_temp[1], &dirac_temp[2], on);
        render_localize(&dirac_temp[3], &dirac_temp[4], &dirac_temp[5], on);
        render_localize(&dirac_temp[6], &dirac_temp[7], &dirac_temp[8], on);

        //      Stuff into Matt's order...
        //      --------------------------
        /*
                Dirac_basis[0] = dirac_temp[0].to_fix();
                Dirac_basis[1] =-dirac_temp[6].to_fix();
                Dirac_basis[2] = dirac_temp[3].to_fix();
                Dirac_basis[3] =-dirac_temp[2].to_fix();
                Dirac_basis[4] = dirac_temp[8].to_fix();
                Dirac_basis[5] =-dirac_temp[5].to_fix();
                Dirac_basis[6] = dirac_temp[1].to_fix();
                Dirac_basis[7] =-dirac_temp[7].to_fix();
                Dirac_basis[8] = dirac_temp[4].to_fix();
        */

        //      Almost...
        /*
                Dirac_basis[0] = dirac_temp[3].to_fix();
                Dirac_basis[1] =-dirac_temp[6].to_fix();
                Dirac_basis[2] =-dirac_temp[0].to_fix();
                Dirac_basis[3] =-dirac_temp[5].to_fix();
                Dirac_basis[4] = dirac_temp[8].to_fix();
                Dirac_basis[5] = dirac_temp[2].to_fix();
                Dirac_basis[6] = dirac_temp[4].to_fix();
                Dirac_basis[7] =-dirac_temp[7].to_fix();
                Dirac_basis[8] =-dirac_temp[1].to_fix();
        */

        Dirac_basis[0] = -Q_to_fix(dirac_temp[3]);
        Dirac_basis[1] = -Q_to_fix(dirac_temp[6]);
        Dirac_basis[2] =  Q_to_fix(dirac_temp[0]);
        Dirac_basis[3] =  Q_to_fix(dirac_temp[5]);
        Dirac_basis[4] =  Q_to_fix(dirac_temp[8]);
        Dirac_basis[5] = -Q_to_fix(dirac_temp[2]);
        Dirac_basis[6] = -Q_to_fix(dirac_temp[4]);
        Dirac_basis[7] = -Q_to_fix(dirac_temp[7]);
        Dirac_basis[8] =  Q_to_fix(dirac_temp[1]);

    } // End of check for Dirac_frame or not...
}

//	Utilities for the weak spirited...
//	==================================
void EDMS_set_Dirac_frame_parameters(physics_handle ph, Dirac_frame *d) {

    Q mass, hardness, roughness, gravity;

    mass = Q_as_fix(d->mass);
    hardness = Q_as_fix(d->hardness);
    gravity = Q_as_fix(d->gravity);
    roughness = Q_as_fix(d->roughness);

    int32_t on = physics_handle_to_object_number(ph);

    hardness = Q_mul(hardness, (Q_div(Q_mul(mass, DIRAC_HARD_FAC), size)));
    I[on][20] = mass;
    I[on][21] = Q_div(Q_as_int(1), mass);
    I[on][22] = Q_div(Q_as_int(1), (Q_mul(Q_mul(Q_mul(Q_as_double(.4), mass), size), size)));
    I[on][23] = hardness;
    I[on][24] = Q_mul(Q_sqrt(I[on][23]), Q_sqrt(mass));
    //	I[on][24] = 20*sqrt( I[on][20] * mass );
    I[on][25] = roughness;
    I[on][26] = Q_as_double(.2);
    I[on][27] = Q_as_int(0); // gravity;
    I[on][28] = Q_as_int(0);
    I[on][29] = Q_as_int(0);

    //	Done!
    //	-----
}

//	And the weak minded...
//	======================
void EDMS_get_Dirac_frame_parameters(physics_handle ph, Dirac_frame *d) {
    int32_t on = physics_handle_to_object_number(ph);

    d->roughness = Q_to_fix((Q_div(I[on][23], I[on][26])));
    d->hardness = Q_to_fix((Q_mul(Q_div(I[on][26], I[on][20]), DIRAC_HARD_FAC)));
    d->mass = Q_to_fix(I[on][20]);
    d->gravity = Q_to_fix(I[on][27]);
}

void EDMS_control_Dirac_frame(physics_handle ph, fix forward, fix pitch, fix yaw, fix roll) {

    int32_t on = ph2on[ph];

    Q F, P, Y, R;

    F = Q_as_fix(forward);

    //      System shock angle order, definition...
    //      =======================================
    R = Q_as_fix(roll);
    P = Q_as_fix(yaw);
    Y = Q_as_fix(pitch);
    Y = Q_neg(Y);

    control_dirac_frame(on, F, P, Y, R);
}

// Access to the Dirac matrix for the main game.
fix *EDMS_Dirac_basis(void) {
    return Dirac_basis;
}

void render_localize(Q *X, Q *Y, Q *Z, int32_t object) {

    Q e0, e1, e2, e3;

    Q x = *X, y = *Y, z = *Z;

    e0 = S[object][3][0];
    e1 = S[object][4][0];
    e2 = S[object][5][0];
    e3 = S[object][6][0];

    //	Go for it, sonny...
    //	-------------------
    *X = Q_add(Q_add(Q_mul(x, (Q_sub(Q_sub(Q_add(Q_mul(e0, e0), Q_mul(e1, e1)), Q_mul(e2, e2)), Q_mul(e3, e3)))), Q_mul(y, (Q_mul(Q_as_int(2), (Q_sub(Q_mul(e1, e2), Q_mul(e0, e3))))))), Q_mul(z, (Q_mul(Q_as_int(2), (Q_add(Q_mul(e1, e3), Q_mul(e0, e2)))))));

    *Y = Q_add(Q_add(Q_mul(x, (Q_mul(Q_as_int(2), (Q_add(Q_mul(e1, e2), Q_mul(e0, e3)))))), Q_mul(y, (Q_sub(Q_add(Q_sub(Q_mul(e0, e0), Q_mul(e1, e1)), Q_mul(e2, e2)), Q_mul(e3, e3))))), Q_mul(z, (Q_mul(Q_as_int(2), (Q_sub(Q_mul(e2, e3), Q_mul(e0, e1)))))));

    *Z = Q_add(Q_add(Q_mul(x, (Q_mul(Q_as_int(2), (Q_add(Q_mul(Q_neg(e0), e2), Q_mul(e1, e3)))))), Q_mul(y, (Q_mul(Q_as_int(2), (Q_add(Q_mul(e2, e3), Q_mul(e0, e1))))))), Q_mul(z, (Q_add(Q_sub(Q_sub(Q_mul(e0, e0), Q_mul(e1, e1)), Q_mul(e2, e2)), Q_mul(e3, e3)))));
}
