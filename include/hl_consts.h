// HL game consts
namespace FreemanAPI {
	//const int MAX_PHYSENTS = 600; 				// Must have room for all entities in the world.
	//const int MAX_MOVEENTS = 64;
	const int MAX_CLIP_PLANES = 5;

	// movement consts
	const float TIME_TO_DUCK 				= 0.4;
	const int VEC_HULL_MIN					= -36;
	const int VEC_HULL_MAX					= 36;
	const int VEC_VIEW_HL1					= 28;
	const int VEC_DUCK_HULL_MIN				= -18;
	const int VEC_DUCK_HULL_MAX				= 18;
	const int VEC_DUCK_VIEW_HL1				= 12;
	const int PM_DEAD_VIEWHEIGHT			= -8;
	const int PLAYER_FATAL_FALL_SPEED_HL1	= 1024;	// approx 60 feet
	const int PLAYER_MAX_SAFE_FALL_SPEED_HL1= 580;	// approx 20 feet
	const float DAMAGE_FOR_FALL_SPEED_HL1	= (float)100 / (PLAYER_FATAL_FALL_SPEED_HL1 - PLAYER_MAX_SAFE_FALL_SPEED_HL1); // damage per unit per second.
	const int PLAYER_MIN_BOUNCE_SPEED_HL1	= 200;
	const int PLAYER_FALL_PUNCH_THRESHOLD_HL1= 350;	// won't punch player's screen/make scrape noise unless player falling at least this fast.
	const int PLAYER_LONGJUMP_SPEED			= 350;	// how fast we longjump
	const float PLAYER_DUCKING_MULTIPLIER 	= 0.333;
	const float	STOP_EPSILON				= 0.1;
	const int WJ_HEIGHT						= 8;
	const float BUNNYJUMP_MAX_SPEED_FACTOR	= 1.7f; // Only allow bunny jumping up to 1.7x server / player maxspeed setting

	// HL2 consts
	const float TIME_TO_DUCK_MS					= 400.0f;
	const float TIME_TO_UNDUCK					= 0.2;
	const float TIME_TO_UNDUCK_MS				= 200.0f;
	const float GAMEMOVEMENT_DUCK_TIME			= 1000.0f; // ms
	const float GAMEMOVEMENT_JUMP_TIME			= 510.0f; // ms approx - based on the 21 unit height jump
	const float GAMEMOVEMENT_TIME_TO_UNDUCK		= (TIME_TO_UNDUCK * 1000.0f); // ms
	const float GAMEMOVEMENT_TIME_TO_UNDUCK_INV	= (GAMEMOVEMENT_DUCK_TIME - GAMEMOVEMENT_TIME_TO_UNDUCK);
	const int VEC_VIEW_HL2						= 64;
	const int VEC_DUCK_VIEW_HL2					= 28;
	const float PLAYER_FATAL_FALL_SPEED_HL2			= 922.5f; // approx 60 feet sqrt( 2 * gravity * 60 * 12 )
	const float PLAYER_MAX_SAFE_FALL_SPEED_HL2		= 526.5f; // approx 20 feet sqrt( 2 * gravity * 20 * 12 )
	const float PLAYER_LAND_ON_FLOATING_OBJECT_HL2	= 173; // Can fall another 173 in/sec without getting hurt
	const float PLAYER_MIN_BOUNCE_SPEED_HL2			= 173;
	const float PLAYER_FALL_PUNCH_THRESHOLD_HL2		= 303.0f; // // won't punch player's screen/make scrape noise unless player falling at least this fast - at least a 76" fall (sqrt( 2 * g * 76))
	const float DAMAGE_FOR_FALL_SPEED_HL2			= 100.0f / (PLAYER_FATAL_FALL_SPEED_HL2 - PLAYER_MAX_SAFE_FALL_SPEED_HL2); // damage per unit per second.
	const float NON_JUMP_VELOCITY 					= 140.0f; // NOTE: 145 is a jump.
	const float PUNCH_DAMPING						= 9.0f;		// bigger number makes the response more damped, smaller is less damped
																// currently the system will overshoot, with larger damping values it won't
	const float PUNCH_SPRING_CONSTANT				= 65.0f;	// bigger number increases the speed at which the view corrects

	// edict->flags
	const int FL_FLY				= (1<<0);	// Changes the SV_Movestep() behavior to not need to be on ground
	const int FL_SWIM				= (1<<1);	// Changes the SV_Movestep() behavior to not need to be on ground (but stay in water)
	const int FL_CONVEYOR			= (1<<2);
	const int FL_CLIENT				= (1<<3);
	const int FL_INWATER			= (1<<4);
	const int FL_MONSTER			= (1<<5);
	const int FL_GODMODE			= (1<<6);
	const int FL_NOTARGET			= (1<<7);
	const int FL_SKIPLOCALHOST		= (1<<8);	// Don't send entity to local host, it's predicting this entity itself
	const int FL_ONGROUND			= (1<<9);	// At rest / on the ground
	const int FL_PARTIALGROUND		= (1<<10);	// not all corners are valid
	const int FL_WATERJUMP			= (1<<11);	// player jumping out of water
	const int FL_FROZEN				= (1<<12);	// Player is frozen for 3rd person camera
	const int FL_FAKECLIENT			= (1<<13);	// JAC: fake client, simulated server side; don't send network messages to them
	const int FL_DUCKING			= (1<<14);	// Player flag -- Player is fully crouched
	const int FL_FLOAT				= (1<<15);	// Apply floating force to this entity when in water
	const int FL_GRAPHED			= (1<<16);	// worldgraph has this ent listed as something that blocks a connection

	const int FL_PROXY				= (1<<20);	// This is a spectator proxy
	const int FL_ALWAYSTHINK		= (1<<21);	// Brush model flag -- call think every frame regardless of nextthink - ltime (for constantly changing velocity/path)
	const int FL_BASEVELOCITY		= (1<<22);	// Base velocity has been applied this frame (used to convert base velocity into momentum)
	const int FL_MONSTERCLIP		= (1<<23);	// Only collide in with monsters who have FL_MONSTERCLIP set
	const int FL_ONTRAIN			= (1<<24);	// Player is _controlling_ a train, so movement commands should be ignored on client during prediction.
	const int FL_WORLDBRUSH			= (1<<25);	// Not moveable/removeable brush entity (really part of the world, but represented as an entity for transparency or something)
	const int FL_SPECTATOR      	= (1<<26);	// This client is a spectator, don't run touch functions, etc.
	const int FL_CUSTOMENTITY		= (1<<29);	// This is a custom entity
	const int FL_KILLME				= (1<<30);	// This entity is marked for death -- This allows the engine to kill ents at the appropriate time
	const int FL_DORMANT			= (1<<31);	// Entity is dormant, no updates to client

	const int IN_ATTACK				= (1 << 0);
	const int IN_JUMP				= (1 << 1);
	const int IN_DUCK				= (1 << 2);
	const int IN_FORWARD			= (1 << 3);
	const int IN_BACK				= (1 << 4);
	const int IN_USE				= (1 << 5);
	const int IN_CANCEL				= (1 << 6);
	const int IN_LEFT				= (1 << 7);
	const int IN_RIGHT				= (1 << 8);
	const int IN_MOVELEFT			= (1 << 9);
	const int IN_MOVERIGHT 			= (1 << 10);
	const int IN_ATTACK2			= (1 << 11);
	const int IN_RUN      			= (1 << 12);
	const int IN_RELOAD				= (1 << 13);
	const int IN_ALT1				= (1 << 14);
	const int IN_SPEED				= (1 << 15);

	// texture types
	enum {
		CHAR_TEX_CONCRETE,
		CHAR_TEX_METAL,
		CHAR_TEX_DIRT,
		CHAR_TEX_VENT,
		CHAR_TEX_GRATE,
		CHAR_TEX_TILE,
		CHAR_TEX_SLOSH,
		CHAR_TEX_WOOD,
		CHAR_TEX_COMPUTER,
		CHAR_TEX_GLASS,
		CHAR_TEX_FLESH,
		CHAR_TEX_WADE,
		CHAR_TEX_LADDER,
		CHAR_TEX_SAND,
		CHAR_TEX_MUD,
		CHAR_TEX_GRASS,
		CHAR_TEX_GRAVEL,
		CHAR_TEX_CHAINLINK,
	};

	// edict->movetype values
	enum {
		MOVETYPE_NONE			= 0,		// never moves
		MOVETYPE_ANGLENOCLIP	= 1,
		MOVETYPE_ANGLECLIP		= 2,
		MOVETYPE_WALK			= 3,		// Player only - moving on the ground
		MOVETYPE_STEP			= 4,		// gravity, special edge handling -- monsters use this
		MOVETYPE_FLY			= 5,		// No gravity, but still collides with stuff
		MOVETYPE_TOSS			= 6,		// gravity/collisions
		MOVETYPE_PUSH			= 7,		// no clip to world, push and crush
		MOVETYPE_NOCLIP			= 8,		// No gravity, no collisions, still do velocity/avelocity
		MOVETYPE_FLYMISSILE		= 9,		// extra size to monsters
		MOVETYPE_BOUNCE			= 10,		// Just like Toss, but reflect velocity when contacting surfaces
		MOVETYPE_BOUNCEMISSILE	= 11,		// bounce w/o gravity
		MOVETYPE_FOLLOW			= 12,		// track movement of aiment
		MOVETYPE_PUSHSTEP		= 13,		// BSP model that needs physics/world collisions (uses nearest hull for world collision)
	};

	// water contents - changed for simplicity
	enum {
		CONTENTS_LAVA		= 0,
		CONTENTS_SLIME		= 1,
		CONTENTS_WATER		= 2,
		CONTENTS_EMPTY		= 3,
	};

	enum {
		SPEED_CROPPED_RESET = 0,
		SPEED_CROPPED_DUCK = 1,
		SPEED_CROPPED_WEAPON = 2,
	};
}