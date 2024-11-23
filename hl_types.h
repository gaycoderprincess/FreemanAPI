// HL game types
namespace HLMovement {
	typedef int physent_t;

	struct pmplane_t {
		NyaVec3Double	normal;
		float	dist;
	};

	struct pmtrace_t {
		size_t size;
		bool allsolid;			// if true, plane is not valid
		bool startsolid;		// if true, the initial point was in a solid area
		bool inopen, inwater;	// End point is in empty space or in water
		float fraction;			// time completed, 1.0 = didn't hit anything
		NyaVec3Double endpos;	// final position
		pmplane_t plane;		// surface normal at impact
		int ent;				// entity at impact

		// newly added members
		int surfaceId;

		pmtrace_t() {
			Default();
		}

		void Default() {
			size = sizeof(*this);
			allsolid = false;
			startsolid = false;
			inopen = true;
			inwater = false;
			fraction = 1.0f;
			endpos = {0,0,0};
			plane.normal = {0,0,0};
			plane.dist = 9999;
			ent = -1;
			surfaceId = 0;
		}
	};

	struct movevars_s {
		float gravity;  			// Gravity for map
		float stopspeed;			// Deceleration when not moving
		float maxspeed; 			// Max allowed speed
		float accelerate;     		// Acceleration factor
		float airaccelerate;  		// Same for when in open air
		float wateraccelerate;		// Same for when in water
		float friction;
		float edgefriction;			// Extra friction near dropofs
		float waterfriction;		// Less in water
		//float entgravity;  		// 1.0
		float bounce;      			// Wall bounce value. 1.0
		float stepsize;    			// sv_stepsize;
		float maxvelocity; 			// maximum server velocity.
		//bool footsteps;			// Play footstep sounds
		float rollangle;
		float rollspeed;
	} *movevars = new movevars_s;

	typedef struct usercmd_s {
		uint32_t msec;				// Duration in ms of command
		NyaVec3Double viewangles;	// Command view angles.

		// intended velocities
		float forwardmove;			// Forward velocity.
		float sidemove;				// Sideways velocity.
		float upmove;				// Upward velocity.
		unsigned short buttons;		// Attack buttons
	} usercmd_t;

	struct playermove_s {
		float frametime;					// Duration of this frame
		NyaVec3Double forward, right, up;	// Vectors for angles

		// player state
		NyaVec3Double origin;				// Movement origin.
		NyaVec3Double angles;				// Movement view angles.
		NyaVec3Double oldangles;			// Angles before movement view angles were looked at.
		NyaVec3Double velocity;				// Current movement direction.
		NyaVec3Double movedir;				// For waterjumping, a forced forward velocity so we can fly over lip of ledge.
		NyaVec3Double basevelocity;			// Velocity of the conveyor we are standing, e.g.

		// For ducking/dead
		NyaVec3Double view_ofs;				// Our eye position.
		float flDuckTime;					// Time we started duck
		bool bInDuck;						// In process of ducking or ducked already?

		// For walking/falling
		int	flTimeStepSound;				// Next time we can play a step sound
		int	iStepLeft;

		float flFallVelocity;
		NyaVec3Double punchangle;

		float flSwimTime;

		int	flags;							// FL_ONGROUND, FL_DUCKING, etc.
		int	usehull;						// 0 = regular player hull, 1 = ducked player hull, 2 = point hull
		float gravity;						// Our current gravity and friction.
		float friction;
		int	oldbuttons;						// Buttons last usercmd
		float waterjumptime;				// Amount of time left in jumping out of water cycle.
		bool dead;							// Are we a dead player?
		int	movetype;						// Our movement type, NOCLIP, WALK, FLY

		int onground;
		int waterlevel;
		int watertype;
		int oldwaterlevel;

		int chtexturetype;

		float maxspeed;
		float clientmaxspeed;				// Player specific maxspeed

		usercmd_t cmd;

		NyaVec3Double player_mins[4];
		NyaVec3Double player_maxs[4];
	} *pmove = new playermove_s;
}