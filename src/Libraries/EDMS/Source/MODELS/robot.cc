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
//	Robot.cc is a test object for the Citadel physics system.  It uses the Citadel database
//	for B/C, and should be fairly simple and robust.  Use the vector integrator!
//	============================================================================

//	Seamus, June 29, 1993...
//	========================

//#include <iostream>
//#include <conio.h>
#include "edms_int.h" //This is the object type library. It is universal.
#include "idof.h"
//#ifdef EDMS_SHIPPABLE
////#include <mout.h>
//#endif

#include "edms_chk.h"

// extern "C" {
//#include <i86.h>
//#include <dpmi.h>
#include "ss_flet.h"
//}

//	State information and utilities...
//	==================================
extern EDMS_Argblock_Pointer A;
extern Q S[MAX_OBJ][7][4], I[MAX_OBJ][DOF_MAX];
extern int32_t no_no_not_me[MAX_OBJ];

#define SOLITION_FRAME_CNT

//	Functions...
//	============
extern void (*idof_functions[MAX_OBJ])(int32_t), (*equation_of_motion[MAX_OBJ][7])(int32_t);

//	Callbacks themselves...
//	-----------------------
extern void (*EDMS_object_collision)(physics_handle caller, physics_handle victim, int32_t badness, int32_t DATA1,
                                     int32_t data2, fix loc[3]),
    (*EDMS_wall_contact)(physics_handle caller);

// extern int	are_you_there( int );			//Collisions...
// extern int	check_for_hit( int );

static Q fix_one = Q_from_double(1.), point_five = Q_from_double(.5), two_pi = Q_from_double(6.283185);

//	Just a thought...
//	=================
static Q object0, object1, object2, object3, object4, // Howzat??
    object5, object6, object7, object8, object9, object10, object11, object12, object13, object14, object15, object16,
    object17, object18, object19;

//	First, here are the equations of motion (outdated!)...
//	======================================================
int32_t EDMS_robot_global_badness_indicator = 0;

//	Variables that are NOT on the stack...
//	======================================
static Q A00, A10, A20, A30, A01, A11, A21, A31;

static Q checker, check0, check1, check2, V_wall0, V_wall1;

static Q drug, butt;

const Q wt_pos = Q_from_double(0.001), wt_neg = Q_from_double(-0.001);

#pragma require_prototypes off

//	Here are the internal degrees of freedom:
//	=========================================
void robot_idof(int32_t object) {

    //	Call me instead of having special code everywhere...
    //	====================================================
    extern void shall_we_dance(int object, Q *result0, Q *result1, Q *result2);

    A00 = A[object][0][0]; // Dereference NOW!
    A10 = A[object][1][0];
    A20 = A[object][2][0];
    A30 = A[object][3][0];
    A01 = A[object][0][1];
    A11 = A[object][1][1];
    A21 = A[object][2][1];
    A31 = A[object][3][1];

    I[object][17] = Q_as_int(0); // Make sure we REALLY want to climb...

    EDMS_robot_global_badness_indicator = 0;

    // Get the info...
    indoor_terrain(A00, A10, A20, I[object][IDOF_ROBOT_RADIUS], on2ph[object], TFD_FULL);

    //        if (EDMS_robot_global_badness_indicator != 0 ) {
    //                                        mout << "!Robot.cc: thinks that X: " << A00 << ", Y: " << A10 << ", Z: "
    //                                        << A20 << "\n"; mout << "!Robot.cc: has object number " << object << ",
    //                                        ph: " << on2ph[object] << "\n";
    //                                       }

    //              Boy, will this be faster in the new order...
    //              ============================================
    Q w0, w1, w2, w3, w4;
    w0 = Q_as_fix(terrain_info.wx);
    w1 = Q_as_fix(terrain_info.wy);
    w2 = Q_as_fix(terrain_info.wz);

    //		w3 = sqrt( w0*w0 + w1*w1 + w2*w2 );
    //		w4 = 2*( w3 - .5*I[object][22] );
    //		if ( w4 < 0 ) w4 = 0;   //Ouch!
    //		w4 = w4 / w3;
    //		w0 *= w4;       w1 *= w4;       w2 *= w4;

    if (w0.val == Q_as_int(0).val)
        check0 = Q_as_int(1);
    else
        check0 = Q_as_int(0);

    if (w1.val == Q_as_int(0).val)
        check1 = Q_as_int(1);
    else
        check1 = Q_as_int(0);

    if ((terrain_info.fz == 0) && (terrain_info.cz == 0))
        check2 = Q_as_int(1);
    else
        check2 = Q_as_int(0);

    object0 = Q_as_fix(terrain_info.fx + terrain_info.cx);
    object1 = Q_as_fix(terrain_info.fy + terrain_info.cy);
    object2 = Q_as_fix(terrain_info.fz + terrain_info.cz);

    object0 = Q_add(object0, w0);
    object1 = Q_add(object1, w1);
    object2 = Q_add(object2, w2);

    checker = Q_sqrt(Q_add(Q_add(Q_mul(object0, object0), Q_mul(object1, object1)), Q_mul(object2, object2)));

    if (checker.val > Q_as_double(.0005).val) {
        object3 = Q_div(fix_one, checker); // To get primitive...
        //                                               mout << "NZero!!  " << checker << "\n";
    } else
        checker = object3 = Q_as_int(0);

    object4 = Q_mul(object3, object0); // The primitive V_n...
    object5 = Q_mul(object3, object1);
    object6 = Q_mul(object3, object2);

    object7 = Q_mul(Q_mul(Q_as_double(.75), I[object][21]),
              Q_add(Q_add(Q_mul(A01, object4), // Delta_magnitude...
               Q_mul(A11, object5)), Q_mul(A21, object6)));

    object8 = I[object][20]; // Omega_magnitude...
    //		if( I[object][10]<0 ) object7 *= 4;
    //		if( I[object][10]<0 ) object8 *= 2;

    //              mout << "o7: " << object7 << "\n";
    object4 = Q_mul(object7, object4); // Delta...
    object5 = Q_mul(object7, object5);
    object6 = Q_mul(object7, object6);

    object9 = Q_as_int((checker.val > Q_as_double(.001).val) || (I[object][10].val > Q_as_int(0).val)); // Are we in the rub???

    //		Let's not power through the walls anymore...
    //		--------------------------------------------
    I[object][18] = Q_mul(I[object][18], check0);
    I[object][19] = Q_mul(I[object][19], check1);

    //      Here are collisions with other objects...
    //      =========================================
    object10 = object11 = object12 = Q_as_int(0);

    if (I[object][5].val == Q_as_int(0).val) {
        shall_we_dance(object, &object10, &object11, &object12);
        object10 = Q_mul(object10, Q_mul(I[object][20], check0)); // More general than it was...
        object11 = Q_mul(object11, Q_mul(I[object][20], check1));
        //  	object12 *= I[object][20]*check2;
    }

    //      Climbing overriden with repulsors...
    //      ====================================
    if (ss_edms_bcd_flags & SS_BCD_REPUL_ON) {

        //              Get the speed...
        Q repulsor_speed = Q_as_int(21);
        if ((ss_edms_bcd_flags & SS_BCD_REPUL_SPD) == SS_BCD_REPUL_NORM)
            repulsor_speed = Q_as_int(7);

        //              Assume we're going up, unless...
        if ((ss_edms_bcd_flags & SS_BCD_REPUL_TYPE) == SS_BCD_REPUL_DOWN)
            repulsor_speed = Q_mul(repulsor_speed, Q_as_double(-.5));

        //              The parameter should be the desired height....
        Q repul_height;
        repul_height = Q_as_fix(ss_edms_bcd_param);

        Q nearness_or_something = Q_sub(repul_height, A[object][2][0]);
        if (Q_abs(nearness_or_something).val <= Q_as_double(.333).val) {
            repulsor_speed = Q_mul(repulsor_speed, Q_mul(Q_as_int(3), nearness_or_something));
        }

        Q io17 = repulsor_speed;

        I[object][17] = Q_mul(I[object][26], Q_add(Q_sub(io17, A[object][2][1]), I[object][25]));

        object9 = Q_as_int(1);
    }

    //      AutoClimbing(tm) is for wussies (is superseeded by climbing)...
    //      ===============================================================
    if ((ss_edms_bcd_flags & SS_BCD_MISC_STAIR)) {

        Q o1 = Q_as_int(0), o0 = Q_as_int(0);

        if ((checker.val > Q_as_int(0).val) && (Q_add(Q_abs(I[object][18]), Q_abs(I[object][19])).val > Q_as_double(.01).val)) {

            Q ratio = Q_add(Q_mul(Q_add(I[object][18], A[object][0][1]), object0), Q_mul(Q_add(I[object][19], A[object][1][1]), object1));

            Q io17 = Q_as_double(.5);

            if (ratio.val <= Q_as_int(0).val) {
                o1 = object1;
                o0 = object0;
            } else
                o1 = o0 = io17 = Q_as_int(0);

            I[object][18] = Q_add(Q_div(Q_mul(Q_mul(Q_mul(Q_as_double(-.3), I[object][22]), o0), object8), checker), Q_mul(Q_as_double(.1), I[object][18]));
            I[object][19] = Q_add(Q_div(Q_mul(Q_mul(Q_mul(Q_as_double(-.3), I[object][22]), o1), object8), checker), Q_mul(Q_as_double(.1), I[object][19]));
            //	        	io18 = -.3*I[object][22]*o0*object8/checker + .1*I[object][18];
            //		        io19 = -.3*I[object][22]*o1*object8/checker + .1*I[object][19];

            //                    Set the mojo...
            //                    ===============
            I[object][17] = Q_mul(Q_as_int(800), (Q_sub(io17, A[object][2][1])));
        }
    }

    //	Angular play (citadel) ...
    //	==========================
    if (S[object][3][0].val > two_pi.val)
        S[object][3][0] = Q_sub(S[object][3][0], two_pi);
    if (S[object][3][0].val < Q_neg(two_pi).val)
        S[object][3][0] = Q_add(S[object][3][0], two_pi);

    //	Don't be stupid...
    //	------------------
    drug = Q_mul(Q_neg(object9), I[object][23]);
    //      mout << drug << "\n";
    butt = I[object][24];

    //	Try the equations of motion here for grins...
    //	=============================================
    S[object][2][2] = Q_sub(Q_mul(butt, (Q_add(Q_add(Q_add(Q_sub(Q_mul(object8, object2), // Elasticity...
                              object6),         // Drag...
                              I[object][17]),   // Control...
                              Q_mul(drug, A21)), object12))),

                      I[object][25]); // Grav'ty...

    S[object][0][2] = Q_mul(butt, Q_add(Q_add(Q_add(Q_sub(Q_mul(object8, object0),         // Elasticity...
                              object4),                 // Drag...
                              Q_mul(object9, I[object][18])), // Control...
                              Q_mul(drug, A01)),              // Drag...
                              object10));              // Collide...

    S[object][1][2] = Q_mul(butt, Q_add(Q_add(Q_add(Q_sub(Q_mul(object8, object1),         // Elasticity...
                              object5),                 // Drag...
                              Q_mul(object9, I[object][19])), // Control...
                              Q_mul(drug, A11)),              // Drag...
                              object11));              // Collide...

    S[object][3][2] = Q_mul(I[object][27], Q_sub(I[object][16],          // Control...
                                       Q_mul(I[object][28], A31))); // Drag...

    //        mout << "Butt: " << butt << "\n";
    //        mout << "1X: " << object0 << " 1Y: " << object1 << " 1Z: " << object2 << "\n";
    //        mout << "VX: " << A01 << " VY: " << A11 << " VZ: " << A21 << "\n";
    //        mout << "2X: " << object4 << " 2Y: " << object5 << " 2Z: " << object6 << "\n";
    //        mout << "3X: " << object8*object0 << " 3Y: " << object8*object1 << " 3Z: " << object8*object2 << "\n";
    //        mout << "FX: " << object8*object0 - object4 << " FY: " << object8*object1 - object5 << " FZ: " <<
    //        object8*object2 - object6 << "\n"; mout << "xx: " << drug*A01 << " yy: " << drug*A11 << " zz: " <<
    //        drug*A21 << "\n";
    //          mout << " ZZ: " << S[object][2][2] << " : " << butt*(object8*object2 - object6) - I[object][25] <<
    //          " : " << drug*A21 << " : " << object12 << " : " << I[object][17] << "\n";
    //        mout << I[object][17] << " : " << object << "\n";

    //      Damnage...
    //      ==========
    Q dam0 = Q_sub(Q_mul(object8, object0), object4);
    Q dam1 = Q_sub(Q_mul(object8, object1), object5);
    Q dam2 = Q_sub(Q_mul(object8, object2), object6);

    I[object][14] = Q_mul(I[object][IDOF_ROBOT_MASS_RECIP], Q_add(Q_add(Q_abs(dam0), Q_abs(dam1)), Q_abs(dam2))); // Damage??

    //	Is there a projectile hit?
    //	==========================
    if (I[object][35].val > Q_as_int(0).val) {

        //		Let's not power through the walls anymore...
        //		--------------------------------------------
        //      mout << "knock " << I[object][32] << " " << I[object][33] << " " << I[object][34] << ": I[24] " <<
        //      I[object][24] << "\n";

        //      if (I[object][24] > 1.0)
        //      {
        //         if ( abs(I[object][32]) > 1000 ) I[object][32] = 1000*(1-2*(I[object][32]<0));
        //         if ( abs(I[object][33]) > 1000 ) I[object][33] = 1000*(1-2*(I[object][33]<0));
        //         if ( abs(I[object][34]) > 1000 ) I[object][34] = 1000*(1-2*(I[object][34]<0));
        //      }

        //      mout << "clamp " << I[object][32] << " " << I[object][33] << " " << I[object][34] << "\n";

        S[object][0][2] = Q_add(S[object][0][2], /* I[object][24]* */ Q_mul(I[object][32], check0));
        S[object][1][2] = Q_add(S[object][0][2], /* I[object][24]* */ Q_mul(I[object][33], check1));
        S[object][2][2] = Q_add(S[object][0][2], /* I[object][24]* */ Q_mul(I[object][34], check2));

        //      mout << "  add " << /* I[object][24]* */ I[object][32]*check0 << " " <<
        //                          /* I[object][24]* */ I[object][33]*check0 << " " <<
        //                          /* I[object][24]* */ I[object][34]*check0 << "\n";

        //                mout << "R: " << object << " K: " << I[object][24]*I[object][32]*check0 << " : " <<
        //                I[object][24]*I[object][33]*check1 << " : " << I[object][24]*I[object][34] << "\n";

        I[object][35] = Q_as_int(0);
        I[object][32] = Q_as_int(0);
        I[object][33] = Q_as_int(0);
        I[object][34] = Q_as_int(0);
    }

    //	That's all, folks...
    //	====================
}

//	We might for now want to set some external forces on the robot...
//	==================================================================
void robot_set_control(int32_t robot, Q thrust_lever, Q attitude_jet, Q jump) {

    Q_sincos(S[robot][3][0], &object0, &object1);

#ifdef EDMS_SHIPPABLE
    if (I[robot][30].val != ROBOT.val)
        mout << "You are an idiot: I'm not a ROBOT!\n";
#endif

    //	Here's the thrust of the situation...
    //	-------------------------------------
    I[robot][18] = Q_mul(Q_mul(thrust_lever, object1), I[robot][IDOF_ROBOT_MASS]);
    I[robot][19] = Q_mul(Q_mul(thrust_lever, object0), I[robot][IDOF_ROBOT_MASS]);
    I[robot][17] = Q_mul(I[robot][26], jump);

    //	And the turn of the...
    //	----------------------
    I[robot][16] = Q_mul(attitude_jet, I[robot][IDOF_ROBOT_MOI]);

    //	Wakee wakee...
    //	--------------
    no_no_not_me[robot] = (Q_add(Q_add(Q_add(Q_abs(I[robot][18]), Q_abs(I[robot][19])), Q_abs(I[robot][16])), Q_abs(I[robot][17])).val > Q_as_int(0).val);
}

//	Here is a separate control routine for robots under AI domination...
//	====================================================================
void robot_set_ai_control(int32_t robot, Q desired_heading, Q desired_speed, Q sidestep, Q urgency, Q *there_yet,
                          Q distance) {

    const Q one_by_pi = Q_as_double(0.31830), pi = Q_as_double(3.14159), two_pi = Q_as_double(6.28318);

#ifdef EDMS_SHIPPABLE
    if (I[robot][30].val != ROBOT.val)
        mout << "Hey, don't call control_robot on non-robots!\n";
#endif

    if (desired_heading.val > two_pi.val)
        desired_heading = Q_sub(desired_heading, two_pi);
    if (desired_heading.val < Q_as_int(0).val)
        desired_heading = Q_add(desired_heading, two_pi);

    //	Nota bene:  Here the desired heading is specified is in the range
    //		    0 <= desired_heading < 2pi.	Urgency is a number in the range
    //		    0 <= urgency <= 20.  A zero urgency will produce no control input.
    //		    ==================================================================

    //	Setup...
    //	--------
    Q speed = Q_sqrt(Q_add(Q_mul(S[robot][0][1], S[robot][0][1]), Q_mul(S[robot][1][1], S[robot][1][1]))),
      direction = Q_sub(desired_heading, S[robot][3][0]);

    Q_sincos(S[robot][3][0], &object0, &object1);

    //	Heading...
    //	----------
    if (direction.val > pi.val)
        direction = Q_neg(Q_sub(direction, pi));
    if (direction.val <= Q_neg(pi).val)
        direction = Q_neg(Q_add(direction, pi));

    //	Inform the caller if we're on course yet...
    //	-------------------------------------------
    *there_yet = Q_mul(Q_mul(one_by_pi, direction), Q_sub(Q_as_int(1), Q_mul(Q_as_int(2), Q_as_int(direction.val < Q_as_int(0).val))));

    //	Set the control...
    //	------------------
    I[robot][16] = Q_mul(Q_mul(Q_mul(Q_as_double(.1), urgency), direction), I[robot][29]);

    //	Speed...
    //	--------
    I[robot][17] = Q_mul(Q_mul(urgency, Q_div(Q_as_int(1), (Q_add(Q_mul(Q_as_int(10), *there_yet), Q_as_int(5))))), Q_sub(desired_speed, speed)); // temporary...
    if (I[robot][17].val < Q_as_int(0).val)
        I[robot][17] = Q_as_int(0);

    if (distance.val < Q_as_int(1).val)
        I[robot][17] = Q_mul(I[robot][17], distance);

    I[robot][18] = Q_add(Q_mul(object1, I[robot][17]), Q_mul(object0, sidestep));
    I[robot][19] = Q_sub(Q_mul(object0, I[robot][17]), Q_mul(object1, sidestep));
    I[robot][17] = Q_as_int(0); // No jumping for AIs

    //	Wakee wakee...
    //	--------------
    if (no_no_not_me[robot] == 0) {
        no_no_not_me[robot] = Q_add(Q_add(Q_abs(I[robot][18]), Q_abs(I[robot][19])), Q_abs(I[robot][16])).val > Q_as_int(0).val;
        //        if ( no_no_not_me[robot] != 0 ) mout << "R: " << robot << ", ph= " << on2ph[robot] << " awoken!  " <<
        //        no_no_not_me[robot] << "\n"; mout << ( abs( I[robot][18] ) + abs( I[robot][19] ) + 50*abs(
        //        I[robot][16] ) + abs( I[robot][17] ) ) << "\n"; mout << no_no_not_me[robot] << "\n";
    }

    //	mout << "Robot #" << robot << " with: " << ( abs( I[robot][18] ) + abs( I[robot][19] ) + 50*abs( I[robot][16] )
    //+ abs( I[robot][17] ) ) << ".\n";;
}

int32_t make_robot(Q init_state[6][3], Q params[10]) {

    //	Sets up everything needed to manufacture a robot with initial state vector
    //	init_state[][] and EDMS motion parameters params[] into soliton. Returns the
    //	object number, or else a negative error code (see Soliton.CPP for error handling and codes).
    //	============================================================================================

    //	Have some variables...
    //	======================
    int32_t object_number = -1, // Three guesses...
        error_code = -1;    // Guilty until...

    //	We need ignorable coordinates...
    //	================================
    extern void null_function(int32_t);

    //	First find out which object we're going to be...
    //	================================================
    while (S[++object_number][0][0].val > END.val)
        ; // Jon's first C trickie...

    //	Is it an allowed object number?  Are we full? Why are we here? Is there a God?
    //	==============================================================================
    if (object_number < MAX_OBJ) {

        //		Now we can create the robot:  first dump the initial state vector...
        //		=====================================================================
        for (int32_t coord = 0; coord < 6; coord++) {
            for (int32_t deriv = 0; deriv < 3; deriv++) { // Has alpha now...
                S[object_number][coord][deriv] = A[object_number][coord][deriv] =
                    init_state[coord][deriv]; // For collisions...
            }
        }

        //		Put in the appropriate robot parameters...
        //		===========================================
        for (int32_t copy = 0; copy < 10; copy++) {
            I[object_number][copy + 20] = params[copy];
        }
        I[object_number][IDOF_MODEL] = ROBOT; // Hey, you are what you eat.

        // Put in the collision information...
        // ===================================
        I[object_number][IDOF_RADIUS] = I[object_number][IDOF_ROBOT_RADIUS];
        I[object_number][32] = I[object_number][33] = I[object_number][34] = I[object_number][35] = Q_as_int(0);
        I[object_number][36] = I[object_number][IDOF_ROBOT_MASS_RECIP]; // Shrugoff "mass"...
        I[object_number][IDOF_COLLIDE] = Q_as_int(-1);
        I[object_number][IDOF_AUTODESTRUCT] = Q_as_int(0); // No kill I...

        // Turn ON collisions for this robot...
        // ------------------------------------
        I[object_number][5] = Q_as_int(0); // negative values are off...

        // Zero the control initially...
        // =============================
        I[object_number][16] = I[object_number][18] = I[object_number][19] = I[object_number][17] = Q_as_int(0);

        //		Now tell Soliton where to look for the equations of motion...
        //		=============================================================
        idof_functions[object_number] = robot_idof;

        equation_of_motion[object_number][0] = equation_of_motion[object_number][1] =
            equation_of_motion[object_number][2] = equation_of_motion[object_number][3] =
                equation_of_motion[object_number][4] = // Nice symmetries, huh.
            equation_of_motion[object_number][5] = null_function;

        //               for (int tt = 0; tt < 10; tt++ ) mout << params[tt] << " : ";
        //               mout << "\n";

        //		Wakee wakee...
        //		--------------
        no_no_not_me[object_number] = 1;

        //		Things seem okay...
        //		===================
        error_code = object_number;
    }

    //	Inform the caller...
    //	====================
    return error_code;
}

#pragma require_prototypes on

//	ATTENZIONE:  Los parametros del model son:
//	==========================================

//	Number   |   Comment
//	--------------------
//	0        |   K
//	1        |   d
//	2        |   Radius
//	3        |   Rolling Drag
//	4        |   1/Mass
//	5        |   gravity
//	6        |   mass
//	7	 |   1/moi
//	8        |   rotational drag
//	9	 |   moi
//	==========================================
//	So there.
