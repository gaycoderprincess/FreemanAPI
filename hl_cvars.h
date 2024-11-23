// HL game cvars
namespace HLMovement {
	float cl_bob = 0.01;
	float cl_bobcycle = 0.8;
	float cl_bobup = 0.5;

	float cl_forwardspeed = 400;
	float cl_sidespeed = 400;
	float cl_upspeed = 320;
	float cl_movespeedkey = 0.3;

	float sv_gravity = 800;  			// Gravity for map
	float sv_stopspeed = 100;			// Deceleration when not moving
	float sv_maxspeed = 320; 			// Max allowed speed
	float sv_noclipspeed = 320; 		// Max allowed speed
	float sv_accelerate = 10;			// Acceleration factor
	float sv_airaccelerate = 10;		// Same for when in open air
	float sv_wateraccelerate = 10;		// Same for when in water
	float sv_friction = 4;
	float sv_edgefriction = 2;			// Extra friction near dropofs
	float sv_waterfriction = 1;			// Less in water
	//float sv_entgravity = 1.0;  		// 1.0
	float sv_bounce = 1.0;      		// Wall bounce value. 1.0
	float sv_stepsize = 18;
	float sv_maxvelocity = 2000; 		// maximum server velocity.
	//bool mp_footsteps = true;			// Play footstep sounds
	float sv_rollangle = 2;
	float sv_rollspeed = 200;
}