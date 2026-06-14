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
#include "fixpp.h"
#include "edms_int.h"
#include "idof.h"
#include "lg.h"
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
extern Q *utility_pointer[MAX_OBJ];

//	Pelvis...
//	---------
typedef struct {

    fix mass, size, hardness, pep, gravity, height;

    int32_t cyber_space;

} Pelvis;

//	Here we go...
//	=============
#define HARD_FAC Q_as_int(6)

//      Thatnk God we have only 16 bits of fraction!
//      ============================================
Q old_state[6], new_state[6];

//	Here are the bridge routines to the models...
//	=============================================

//      Pelvis routines...
//	==================
physics_handle EDMS_make_pelvis(Pelvis *p, State *s) {

    Q params[10], init_state[6][3];

    Q mass, pep, hardness, size, gravity, height;

    int on = 0, cyber_space = 0;

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

    mass = Q_as_fix(p->mass);
    size = Q_as_fix(p->size);
    //	if ( size > .45/hash_scale ) size = .45/hash_scale;
    hardness = Q_as_fix(p->hardness);
    pep = Q_as_fix(p->pep);
    gravity = Q_as_fix(p->gravity);
    height = Q_as_fix(p->height);
    if (height.val > Q_mul(Q_as_int(3), size).val)
        height = Q_mul(Q_as_int(3), size);

    //	mout << "Pelvis: \n";
    //	mout << "	mass: " << mass << "\n";
    //	mout << "	size: " << size << "\n";
    //	mout << "	hard: " << hardness << "\n";
    //	mout << "	pepp: " << pep << "\n";
    //	mout << "	grav: " << gravity << "\n";
    //	mout << "	hght: " << height << "\n";

    //	hardness = hardness*(mass*4/size);
    hardness = Q_mul(hardness, Q_div(Q_mul(mass, HARD_FAC), size));
    params[0] = hardness;
    params[1] = Q_mul(Q_as_int(3), Q_sqrt(Q_mul(params[0], mass)));
    params[2] = size;
    params[3] = Q_mul(pep, mass);
    params[4] = Q_div(Q_as_double(1.), mass);
    params[5] = gravity;
    params[6] = mass;
    params[7] = Q_div(Q_as_double(1.), Q_mul(Q_mul(Q_mul(Q_as_double(.4), mass), size), size));
    params[8] = Q_mul(Q_as_double(5.), Q_div(Q_as_double(1.), params[7]));
    params[9] = Q_mul(Q_mul(Q_mul(Q_as_double(.4), mass), size), size);
    //      mout << "I29 from make! " << params[9] << "\n";

    on = make_pelvis(init_state, params);

    //              Turn on Cyber Space...
    //              ----------------------
    cyber_space = p->cyber_space;
    if (cyber_space < 0 || cyber_space > 2)
        cyber_space = 0; // Hey, why you do that?

    I[on][10] = Q_as_int(cyber_space);
    I[on][0] = I[on][6] = Q_sub(height, size); // Ok...

    ph = EDMS_bind_object_number(on);

    return ph;
}

//      This works just like the robot model...
//      ---------------------------------------
void EDMS_control_pelvis(physics_handle ph, fix forward, fix turn, fix sidestep, fix lean, fix jump, int32_t crouch) {

    // Silly, no?
    Q FF, TT, SS, LL, JJ;

#ifdef EDMS_SHIPPABLE
    if (ph < 0)
        mout << "Hey, you are and idiot...";
#endif

    FF = Q_as_fix(forward);
    TT = Q_as_fix(turn);
    SS = Q_as_fix(sidestep);
    LL = Q_as_fix(lean);
    JJ = Q_as_fix(jump);

    int32_t on = physics_handle_to_object_number(ph);

    if (I[on][IDOF_MODEL].val == PELVIS.val)
        pelvis_set_control(on, FF, TT, SS, LL, JJ, crouch);
}

//      At some point we need the viewpoint offered by the neck...
//      ----------------------------------------------------------
void EDMS_get_pelvic_viewpoint(physics_handle ph, State *s) {

	if(ph >= nelem(ph2on)){
		TRACE("EDMS_get_pelvic_viewpoint: ph overflow: %d/%d", ph, nelem(ph2on));
		abort();
	}
    int32_t on = ph2on[ph];

    Q delta = Q_as_int(0);

    if (I[on][IDOF_MODEL].val == PELVIS.val) {

        Q new_neck = I[on][0]; // Rendered height...

        Q offset_x = Q_mul(new_neck, Q_sin(S[on][4][0])), offset_y = Q_mul(Q_mul(Q_as_double(-1.5), new_neck), Q_sin(S[on][5][0])),
          offset_z = Q_mul(Q_mul(new_neck, Q_cos(Q_mul(Q_as_int(1), S[on][4][0]))), Q_cos(Q_mul(Q_as_int(1), S[on][5][0])));
        //	offset_z = new_neck*cos( (.2 + .8*(I[on][10]>0) )*S[on][4][0] )*cos( (.2 + .8*(I[on][10]>0)
        //)*S[on][5][0] );

        Q sin_alpha = Q_as_int(0), cos_alpha = Q_as_int(0);

        Q final_x = Q_as_int(0), final_y = Q_as_int(0);

        Q_sincos(Q_neg(S[on][3][0]), &sin_alpha, &cos_alpha);
        final_x = Q_add(Q_mul(cos_alpha, offset_x), Q_mul(sin_alpha, offset_y));
        final_y = Q_add(Q_mul(Q_neg(sin_alpha), offset_x), Q_mul(cos_alpha, offset_y));

        new_state[0] = (Q_add(S[on][0][0], final_x));
        new_state[1] = (Q_add(S[on][1][0], final_y));
        new_state[2] = (Q_add(S[on][2][0], offset_z));

        new_state[3] = S[on][3][0];
        new_state[4] = Q_mul((Q_as_double(.1) /*- .3*(I[on][10]>0)*/), S[on][4][0]);
        new_state[5] = Q_mul((Q_as_double(.03) /*+ 1.6*(I[on][10]>0)*/), S[on][5][0]);

        //        new_state[3] = S[on][3][0];
        //        new_state[4] = (.1  - .1*(I[on][10]>0) )*S[on][4][0];
        //        new_state[5] = (.03 + 1.6*(I[on][10]>0) )*S[on][5][0];

        if (I[on][10].val == Q_as_int(2).val) {
            new_state[4] = S[on][4][0];
            new_state[5] = S[on][5][0];
        }

        delta = Q_add(Q_add(Q_add(Q_add(Q_add(Q_mul((Q_sub(new_state[0], old_state[0])), (Q_sub(new_state[0], old_state[0]))),
                Q_mul((Q_sub(new_state[1], old_state[1])), (Q_sub(new_state[1], old_state[1])))),
                Q_mul((Q_sub(new_state[2], old_state[2])), (Q_sub(new_state[2], old_state[2])))),
                Q_mul((Q_sub(new_state[3], old_state[3])), (Q_sub(new_state[3], old_state[3])))),
                Q_mul((Q_sub(new_state[4], old_state[4])), (Q_sub(new_state[4], old_state[4])))),
                Q_mul((Q_sub(new_state[5], old_state[5])), (Q_sub(new_state[5], old_state[5]))));

        if (delta.val > Q_as_double(.00003).val) {

            old_state[0] = new_state[0];
            old_state[1] = new_state[1];
            old_state[2] = new_state[2];

            old_state[3] = new_state[3];
            old_state[4] = new_state[4];
            old_state[5] = new_state[5];
        }

        s->X = Q_to_fix(old_state[0]);
        s->Y = Q_to_fix(old_state[1]);
        s->Z = Q_to_fix(old_state[2]);

        s->alpha = Q_to_fix(old_state[3]);
        s->beta = Q_to_fix(old_state[4]);
        s->gamma = Q_to_fix(old_state[5]);

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
    Q mass, hardness, size, pep, height, gravity;

    int32_t cyber_space = 0;

    mass = Q_as_fix(p->mass);
    size = Q_as_fix(p->size);
    hardness = Q_as_fix(p->hardness);
    pep = Q_as_fix(p->pep);
    gravity = Q_as_fix(p->gravity);
    height = Q_as_fix(p->height);
    if (height.val > Q_mul(Q_as_int(3), size).val)
        height = Q_mul(Q_as_int(3), size);

    int32_t on = physics_handle_to_object_number(ph);

    //	hardness = hardness*(mass*4/size);
    hardness = Q_mul(hardness, (Q_div(Q_mul(mass, HARD_FAC), size)));
    I[on][IDOF_PELVIS_K] = hardness;
    I[on][IDOF_PELVIS_D] = Q_mul(Q_as_int(3), Q_sqrt(Q_mul(I[on][IDOF_PELVIS_K], mass)));
    I[on][IDOF_PELVIS_RADIUS] = size;
    I[on][IDOF_PELVIS_ROLL_DRAG] = Q_mul(pep, mass);
    I[on][IDOF_PELVIS_MASS_RECIP] = Q_div(Q_as_double(1.), mass);
    I[on][IDOF_PELVIS_GRAVITY] = gravity;
    I[on][IDOF_PELVIS_MASS] = mass;
    I[on][IDOF_PELVIS_MOI_RECIP] = Q_div(Q_as_double(1.), (Q_mul(Q_mul(Q_mul(Q_as_double(.4), mass), size), size)));
    I[on][IDOF_PELVIS_ROT_DRAG] = Q_mul(Q_as_double(5.), Q_div(Q_as_double(1.), I[on][IDOF_PELVIS_MOI_RECIP]));
    I[on][IDOF_PELVIS_MOI] = Q_mul(Q_mul(Q_mul(Q_as_double(.4), mass), size), size);
    //      mout << "I29 from set! " << I[on][29] << "\n";
    //        if ( I[on][30] != PELVIS ) mout << "!EDMS: You just screwed up the pelvis...\n";

    cyber_space = p->cyber_space;

    //	Turn on Cyber Space...
    //	----------------------
    cyber_space = p->cyber_space;
    I[on][7] = I[on][15] = Q_as_int(0);
    if (cyber_space < 0 || cyber_space > 2)
        cyber_space = 0; // Hey, why you do that?

    I[on][10] = Q_as_int(cyber_space);
    //      Won't need to be reset!
    //	I[on][0] = I[on][6] = height - size;

    //      Turn lean control off for skates!
    //      ---------------------------------
    if (I[on][10].val > Q_as_int(0).val)
        I[on][15] = I[on][7] = Q_as_int(0);
}

//	And the weak minded...
//	======================
void EDMS_get_pelvis_parameters(physics_handle ph, Pelvis *p) {
    int32_t on = physics_handle_to_object_number(ph);

    p->pep = Q_to_fix(Q_div(I[on][IDOF_PELVIS_ROLL_DRAG], I[on][IDOF_PELVIS_MASS]));
    p->size = Q_to_fix(I[on][IDOF_PELVIS_RADIUS]);
    //	p -> hardness = ( I[on][20]*I[on][22]/(I[on][26]*4) ).to_fix();
    p->hardness = Q_to_fix(Q_div(Q_mul(I[on][IDOF_PELVIS_K], I[on][IDOF_PELVIS_RADIUS]), Q_mul(I[on][IDOF_PELVIS_MASS], HARD_FAC)));
    p->mass = Q_to_fix(I[on][IDOF_PELVIS_MASS]);
    p->gravity = Q_to_fix(I[on][IDOF_PELVIS_GRAVITY]);
    p->cyber_space = Q_to_int(I[on][10]);
    p->height = Q_to_fix(Q_add(I[on][6], I[on][IDOF_PELVIS_RADIUS]));
}

//	And the compression test for terrain "traps..."
//	===============================================
fix EDMS_get_pelvis_damage(physics_handle ph, fix delta_t) {
    int32_t object;
    Q worker_bee_buzz_buzz = Q_as_int(0);

    object = ph2on[ph]; // As stupid as it gets...
    worker_bee_buzz_buzz = I[object][14];

    // FIXME What going on there?
    I[object][14] = Q_as_int(0);

    return fix_mul(delta_t, Q_to_fix(I[object][14]));
}
