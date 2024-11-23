// HL game consts
namespace HLMovement {
	//const int MAX_PHYSENTS = 600; 				// Must have room for all entities in the world.
	//const int MAX_MOVEENTS = 64;
	const int MAX_CLIP_PLANES = 5;

	const float TIME_TO_DUCK 				= 0.4;
	const int VEC_HULL_MIN					= -36;
	const int VEC_HULL_MAX					= 36;
	const int VEC_VIEW						= 28;
	const int VEC_DUCK_HULL_MIN				= -18;
	const int VEC_DUCK_HULL_MAX				= 18;
	const int VEC_DUCK_VIEW					= 12;
	const int PM_DEAD_VIEWHEIGHT			= -8;
	const int PLAYER_FATAL_FALL_SPEED		= 1024;	// approx 60 feet
	const int PLAYER_MAX_SAFE_FALL_SPEED	= 580;	// approx 20 feet
	const float DAMAGE_FOR_FALL_SPEED		= (float)100 / (PLAYER_FATAL_FALL_SPEED - PLAYER_MAX_SAFE_FALL_SPEED); // damage per unit per second.
	const int PLAYER_MIN_BOUNCE_SPEED		= 200;
	const int PLAYER_FALL_PUNCH_THRESHHOLD	= 350;	// won't punch player's screen/make scrape noise unless player falling at least this fast.
	const int PLAYER_LONGJUMP_SPEED			= 350;	// how fast we longjump
	const float PLAYER_DUCKING_MULTIPLIER 	= 0.333;
	const float	STOP_EPSILON				= 0.1;
	const int WJ_HEIGHT						= 8;
	const float BUNNYJUMP_MAX_SPEED_FACTOR	= 1.7f; // Only allow bunny jumping up to 1.7x server / player maxspeed setting

	// edict->movetype values
	const int MOVETYPE_NONE			= 0;		// never moves
	const int MOVETYPE_ANGLENOCLIP	= 1;
	const int MOVETYPE_ANGLECLIP	= 2;
	const int MOVETYPE_WALK			= 3;		// Player only - moving on the ground
	const int MOVETYPE_STEP			= 4;		// gravity, special edge handling -- monsters use this
	const int MOVETYPE_FLY			= 5;		// No gravity, but still collides with stuff
	const int MOVETYPE_TOSS			= 6;		// gravity/collisions
	const int MOVETYPE_PUSH			= 7;		// no clip to world, push and crush
	const int MOVETYPE_NOCLIP		= 8;		// No gravity, no collisions, still do velocity/avelocity
	const int MOVETYPE_FLYMISSILE	= 9;		// extra size to monsters
	const int MOVETYPE_BOUNCE		= 10;		// Just like Toss, but reflect velocity when contacting surfaces
	const int MOVETYPE_BOUNCEMISSILE= 11;		// bounce w/o gravity
	const int MOVETYPE_FOLLOW		= 12;		// track movement of aiment
	const int MOVETYPE_PUSHSTEP		= 13;		// BSP model that needs physics/world collisions (uses nearest hull for world collision)

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
	const int IN_SCORE				= (1 << 15);	// Used by client.dll for when scoreboard is held down

	// texture types
	const int CHAR_TEX_CONCRETE		= 'C';
	const int CHAR_TEX_METAL		= 'M';
	const int CHAR_TEX_DIRT			= 'D';
	const int CHAR_TEX_VENT			= 'V';
	const int CHAR_TEX_GRATE		= 'G';
	const int CHAR_TEX_TILE			= 'T';
	const int CHAR_TEX_SLOSH		= 'S';
	const int CHAR_TEX_WOOD			= 'W';
	const int CHAR_TEX_COMPUTER		= 'P';
	const int CHAR_TEX_GLASS		= 'Y';
	const int CHAR_TEX_FLESH		= 'F';
	const int CHAR_TEX_WADE			= 'Z';
	const int CHAR_TEX_LADDER		= 'L';

	// water contents - changed for simplicity
	const int CONTENTS_LAVA			= 0;
	const int CONTENTS_SLIME		= 1;
	const int CONTENTS_WATER		= 2;
	const int CONTENTS_EMPTY		= 3;
}