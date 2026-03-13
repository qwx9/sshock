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
//	Here is the bridge routine for maintenance and upkeep of the pelvis models...
//	=============================================================================

////#include <conio.h>
#include "fix.h"
#include "edms_int.h"
#include "idof.h"
//#ifdef EDMS_SHIPPABLE
////#include <mout.h>
//#endif

//	Here we need include files for each and every model that we'll be using...
//	==========================================================================
#include "pelvis.h"

//	The physics handles definitions...
//	==================================
#include "physhand.h"

//	Pointers to skeletons (for bipeds, as it were and will be)...
//	=============================================================
extern fix *utility_pointer[MAX_OBJ];

//	Pelvis...
//	---------
typedef struct {

    fix mass, size, hardness, pep, gravity, height;

    int32_t cyber_space;

} Pelvis;

//	Here we go...
//	=============
#define HARD_FAC fix_make(6, 0)

//      Thatnk God we have only 16 bits of fraction!
//      ============================================
fix old_state[6], new_state[6];

//	Here are the bridge routines to the models...
//	=============================================

//      Pelvis routines...
//	==================
physics_handle EDMS_make_pelvis(Pelvis *p, State *s) {

    fix params[10], init_state[6][3];

    fix mass, pep, hardness, size, gravity, height;

    int on = 0, cyber_space = 0;

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

    mass = p->mass;
    size = p->size;
    //	if ( size > .45/hash_scale ) size = .45/hash_scale;
    hardness = p->hardness;
    pep = p->pep;
    gravity = p->gravity;
    height = p->height;
    if (height > fix_mul(fix_make(3,0), size))
        height = fix_mul(fix_make(3,0), size);

    //	mout << "Pelvis: \n";
    //	mout << "	mass: " << mass << "\n";
    //	mout << "	size: " << size << "\n";
    //	mout << "	hard: " << hardness << "\n";
    //	mout << "	pepp: " << pep << "\n";
    //	mout << "	grav: " << gravity << "\n";
    //	mout << "	hght: " << height << "\n";

    //	hardness = hardness*(mass*4/size);
    hardness = fix_mul(hardness, fix_mul_div(mass, HARD_FAC, size));
    params[0] = hardness;
    params[1] = fix_mul(fix_make(3,0), fix_sqrt(fix_mul(params[0], mass)));	/* FIXME: EINVAL in sqrt */
    params[2] = size;
    params[3] = fix_mul(pep, mass);
    params[4] = fix_div(fix_from_float(1.), mass);
    params[5] = gravity;
    params[6] = mass;
    params[7] = fix_div(fix_from_float(1.), fix_mul(fix_mul(fix_mul(fix_from_float(.4), mass), size), size));
    params[8] = fix_mul(fix_from_float(5.), fix_div(fix_from_float(1.), params[7]));
    params[9] = fix_mul(fix_mul(fix_mul(fix_from_float(.4), mass), size), size);
    //      mout << "I29 from make! " << params[9] << "\n";

    on = make_pelvis(init_state, params);

    //              Turn on Cyber Space...
    //              ----------------------
    cyber_space = p->cyber_space;
    if (cyber_space < 0 || cyber_space > fix_make(2,0))
        cyber_space = 0; // Hey, why you do that?

    I[on][10] = cyber_space;
    I[on][0] = I[on][6] = height - size; // Ok...

    ph = EDMS_bind_object_number(on);

    return ph;
}

//      This works just like the robot model...
//      ---------------------------------------
void EDMS_control_pelvis(physics_handle ph, fix forward, fix turn, fix sidestep, fix lean, fix jump, int32_t crouch) {

    // Silly, no?
    fix FF, TT, SS, LL, JJ;

#ifdef EDMS_SHIPPABLE
    if (ph < 0)
        mout << "Hey, you are and idiot...";
#endif

    FF = forward;
    TT = turn;
    SS = sidestep;
    LL = lean;
    JJ = jump;

    int32_t on = physics_handle_to_object_number(ph);

    if (I[on][IDOF_MODEL] == PELVIS)
        pelvis_set_control(on, FF, TT, SS, LL, JJ, crouch);
}

//      At some point we need the viewpoint offered by the neck...
//      ----------------------------------------------------------
void EDMS_get_pelvic_viewpoint(physics_handle ph, State *s) {

    int32_t on = ph2on[ph];

    fix delta = 0;

    if (I[on][IDOF_MODEL] == PELVIS) {

        fix new_neck = I[on][0]; // Rendered height...

        fix offset_x = fix_mul(new_neck, fix_sin(fix_to_fang(S[on][4][0]))), offset_y = fix_mul(fix_mul(fix_from_float(-1.5), new_neck), fix_sin(fix_to_fang(S[on][5][0]))),
          offset_z = fix_mul(fix_mul(new_neck, fix_cos(fix_to_fang(fix_mul(FIX_UNIT, S[on][4][0])))), fix_cos(fix_to_fang(fix_mul(FIX_UNIT, S[on][5][0]))));
        //	offset_z = new_neck*cos( (.2 + .8*(I[on][10]>0) )*S[on][4][0] )*cos( (.2 + .8*(I[on][10]>0)
        //)*S[on][5][0] );

        fix sin_alpha = 0, cos_alpha = 0;

        fix final_x = 0, final_y = 0;

        fix_sincos(fix_to_fang(-S[on][3][0]), &sin_alpha, &cos_alpha);
        final_x = fix_mul(cos_alpha, offset_x) + fix_mul(sin_alpha, offset_y);
        final_y = fix_mul(-sin_alpha, offset_x) + fix_mul(cos_alpha, offset_y);

        new_state[0] = (S[on][0][0] + final_x);
        new_state[1] = (S[on][1][0] + final_y);
        new_state[2] = (S[on][2][0] + offset_z);

        new_state[3] = S[on][3][0];
        new_state[4] = fix_mul((fix_from_float(.1) /*- .3*(I[on][10]>0)*/), S[on][4][0]);
        new_state[5] = fix_mul((fix_from_float(.03) /*+ 1.6*(I[on][10]>0)*/), S[on][5][0]);

        //        new_state[3] = S[on][3][0];
        //        new_state[4] = (.1  - .1*(I[on][10]>0) )*S[on][4][0];
        //        new_state[5] = (.03 + 1.6*(I[on][10]>0) )*S[on][5][0];

        if (I[on][10] == 2) {
            new_state[4] = S[on][4][0];
            new_state[5] = S[on][5][0];
        }

        delta = fix_mul(new_state[0] - old_state[0], new_state[0] - old_state[0]) +
                fix_mul(new_state[1] - old_state[1], new_state[1] - old_state[1]) +
                fix_mul(new_state[2] - old_state[2], new_state[2] - old_state[2]) +
                fix_mul(new_state[3] - old_state[3], new_state[3] - old_state[3]) +
                fix_mul(new_state[4] - old_state[4], new_state[4] - old_state[4]) +
                fix_mul(new_state[5] - old_state[5], new_state[5] - old_state[5]);

        if (delta > fix_from_float(.00003)) {

            old_state[0] = new_state[0];
            old_state[1] = new_state[1];
            old_state[2] = new_state[2];

            old_state[3] = new_state[3];
            old_state[4] = new_state[4];
            old_state[5] = new_state[5];
        }

        s->X = old_state[0];
        s->Y = old_state[1];
        s->Z = old_state[2];

        s->alpha = old_state[3];
        s->beta = old_state[4];
        s->gamma = old_state[5];

        //        if ( delta > 40 ) {
        //                mout << "Holy cow, batman, delta = " << delta << "\n";
        //                getch();
        //                }

    } // End of check for pelvis or not...

    //#ifdef EDMS_SHIPPABLE
    //       else {  mout << "Pelvic Viewpoint: physics handle " << ph << ", object #" << on << " isn't a Pelvis
    //       model!\n";
    //		mout << "Is is really a " << I[on][30] << " located at (" << S[on][0][0] << "," << S[on][1][0] << ")!\n";
    //} #endif
}

//	Utilities for the weak spirited...
//	==================================
void EDMS_set_pelvis_parameters(physics_handle ph, Pelvis *p) {
    fix mass, hardness, size, pep, height, gravity;

    int32_t cyber_space = 0;

    mass = p->mass;
    size = p->size;
    hardness = p->hardness;
    pep = p->pep;
    gravity = p->gravity;
    height = p->height;
    if (height > fix_mul(fix_make(3,0), size))
        height = fix_mul(fix_make(3,0), size);

    int32_t on = physics_handle_to_object_number(ph);

    //	hardness = hardness*(mass*4/size);
    hardness = fix_mul(hardness, fix_mul_div(mass, HARD_FAC, size));
    I[on][IDOF_PELVIS_K] = hardness;
    I[on][IDOF_PELVIS_D] = fix_mul(fix_make(3,0), fix_sqrt(fix_mul(I[on][IDOF_PELVIS_K], mass)));
    I[on][IDOF_PELVIS_RADIUS] = size;
    I[on][IDOF_PELVIS_ROLL_DRAG] = fix_mul(pep, mass);
    I[on][IDOF_PELVIS_MASS_RECIP] = fix_div(fix_from_float(1.), mass);
    I[on][IDOF_PELVIS_GRAVITY] = gravity;
    I[on][IDOF_PELVIS_MASS] = mass;
    I[on][IDOF_PELVIS_MOI_RECIP] = fix_div(fix_from_float(1.), fix_mul(fix_mul(fix_mul(fix_from_float(.4), mass), size), size));
    I[on][IDOF_PELVIS_ROT_DRAG] = fix_mul(fix_from_float(5.), (fix_div(fix_from_float(1.), I[on][IDOF_PELVIS_MOI_RECIP])));
    I[on][IDOF_PELVIS_MOI] = fix_mul(fix_mul(fix_mul(fix_from_float(.4), mass), size), size);
    //      mout << "I29 from set! " << I[on][29] << "\n";
    //        if ( I[on][30] != PELVIS ) mout << "!EDMS: You just screwed up the pelvis...\n";

    cyber_space = p->cyber_space;

    //	Turn on Cyber Space...
    //	----------------------
    cyber_space = p->cyber_space;
    I[on][7] = I[on][15] = 0;
    if (cyber_space < 0 || cyber_space > fix_make(2,0))
        cyber_space = 0; // Hey, why you do that?

    I[on][10] = cyber_space;
    //      Won't need to be reset!
    //	I[on][0] = I[on][6] = height - size;

    //      Turn lean control off for skates!
    //      ---------------------------------
    if (I[on][10] > 0)
        I[on][15] = I[on][7] = 0;
}

//	And the weak minded...
//	======================
void EDMS_get_pelvis_parameters(physics_handle ph, Pelvis *p) {
    int32_t on = physics_handle_to_object_number(ph);

    p->pep = fix_div(I[on][IDOF_PELVIS_ROLL_DRAG], I[on][IDOF_PELVIS_MASS]);
    p->size = I[on][IDOF_PELVIS_RADIUS];
    //	p -> hardness = ( I[on][20]*I[on][22]/(I[on][26]*4) );
    p->hardness = fix_mul_div(I[on][IDOF_PELVIS_K], I[on][IDOF_PELVIS_RADIUS], fix_mul(I[on][IDOF_PELVIS_MASS], HARD_FAC));
    p->mass = I[on][IDOF_PELVIS_MASS];
    p->gravity = I[on][IDOF_PELVIS_GRAVITY];
    p->cyber_space = fix_int(I[on][10]);
    p->height = (I[on][6] + I[on][IDOF_PELVIS_RADIUS]);
}

//	And the compression test for terrain "traps..."
//	===============================================
fix EDMS_get_pelvis_damage(physics_handle ph, fix delta_t) {
    int32_t object;
    fix worker_bee_buzz_buzz = 0;

    object = ph2on[ph]; // As stupid as it gets...
    worker_bee_buzz_buzz = I[object][14];

    // FIXME What going on there?
    I[object][14] = 0;

    return fix_mul(delta_t, I[object][14]);
}
