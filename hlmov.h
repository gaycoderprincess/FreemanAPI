#include "hl_config.h"
#include "hl_cvars.h"
#include "include/hl_consts.h"
#include "hl_types.h"
#include "hl_math.h"
#include "hl_game_ext.h"

namespace FreemanAPI {
	// HL2 helper funcs
	int GetPlayerHullID() {
		// hl2 should do this depending on the ducked var unless we're using point hull
		if (bHL2Mode && pmove->usehull != 2) {
			return pmove->m_bDucked ? 1 : 0;
		}
		return pmove->usehull;
	}

	bool IsDead() {
		return pmove->dead;
	}

	void* GetGroundEntity() {
		if (pmove->onground != -1) return (void*)1;
		return nullptr;
	}

	auto VEC_VIEW() {
		return bHL2Mode ? VEC_VIEW_HL2 : VEC_VIEW_HL1;
	}
	auto VEC_DUCK_VIEW() {
		return bHL2Mode ? VEC_DUCK_VIEW_HL2 : VEC_DUCK_VIEW_HL1;
	}

	NyaVec3Double VEC_HULL_MIN_SCALED() {
		return pmove->player_mins[0];
	}

	NyaVec3Double VEC_HULL_MAX_SCALED() {
		return pmove->player_maxs[0];
	}

	NyaVec3Double VEC_DUCK_HULL_MIN_SCALED() {
		return pmove->player_mins[1];
	}

	NyaVec3Double VEC_DUCK_HULL_MAX_SCALED() {
		return pmove->player_maxs[1];
	}

	void AddFlag(int flag) {
		pmove->flags = pmove->flags | flag;
	}

	void RemoveFlag(int flag) {
		pmove->flags = pmove->flags & ~flag;
	}

	NyaVec3Double GetPlayerMins(bool ducked) {
		return pmove->player_mins[ducked ? 1 : 0];
	}
	NyaVec3Double GetPlayerMaxs(bool ducked) {
		return pmove->player_maxs[ducked ? 1 : 0];
	}
	NyaVec3Double GetPlayerViewOffset(bool ducked) {
		NyaVec3Double out;
		out[UP] = ducked ? VEC_DUCK_VIEW() : VEC_VIEW();
		return out;
	}

	int nDefaultMoveType = MOVETYPE_WALK;

	// export func helpers
	pmtrace_t PointRaytrace(NyaVec3Double origin, NyaVec3Double end) {
		if (bConvertUnits) {
			for (int i = 0; i < 3; i++) {
				origin[i] = UnitsToMeters(origin[i]);
				end[i] = UnitsToMeters(end[i]);
			}
			origin *= vXYZUnitsMult;
			end *= vXYZUnitsMult;
		}
		auto trace = PointRaytraceGame(&origin, &end);
		if (bConvertUnits) {
			for (int i = 0; i < 3; i++) {
				trace->endpos[i] = MetersToUnits(trace->endpos[i]);
				trace->plane.dist = MetersToUnits(trace->plane.dist);
			}
			trace->endpos *= vXYZUnitsMult;
			trace->plane.normal *= vXYZUnitsMult;
		}
		return *trace;
	}

	pmtrace_t PM_PlayerTrace(NyaVec3Double origin, NyaVec3Double end) {
		if (bConvertUnits) {
			for (int i = 0; i < 3; i++) {
				origin[i] = UnitsToMeters(origin[i]);
				end[i] = UnitsToMeters(end[i]);
			}
			origin *= vXYZUnitsMult;
			end *= vXYZUnitsMult;
		}
		auto trace = PM_PlayerTraceGame(&origin, &end);
		if (bConvertUnits) {
			for (int i = 0; i < 3; i++) {
				trace->endpos[i] = MetersToUnits(trace->endpos[i]);
				trace->plane.dist = MetersToUnits(trace->plane.dist);
			}
			trace->endpos *= vXYZUnitsMult;
			trace->plane.normal *= vXYZUnitsMult;
		}
		return *trace;
	}

	pmtrace_t PM_PlayerTraceDown(NyaVec3Double origin, NyaVec3Double end) {
		if (bConvertUnits) {
			for (int i = 0; i < 3; i++) {
				origin[i] = UnitsToMeters(origin[i]);
				end[i] = UnitsToMeters(end[i]);
			}
			origin *= vXYZUnitsMult;
			end *= vXYZUnitsMult;
		}
		auto trace = PM_PlayerTraceDownGame(&origin, &end);
		if (bConvertUnits) {
			for (int i = 0; i < 3; i++) {
				trace->endpos[i] = MetersToUnits(trace->endpos[i]);
				trace->plane.dist = MetersToUnits(trace->plane.dist);
			}
			trace->endpos *= vXYZUnitsMult;
			trace->plane.normal *= vXYZUnitsMult;
		}
		return *trace;
	}

	std::string lastConsoleMsg;

	// default hullmins
	static const NyaVec3Double pm_hullmins[4] = {
			{ -16, -16, -36 },
			{ -16, -16, -18 },
			{   0,   0,   0 },
			{ -32, -32, -32 },
	};

	// defualt hullmaxs
	static const NyaVec3Double pm_hullmaxs[4] = {
			{  16,  16,  36 },
			{  16,  16,  18 },
			{   0,   0,   0 },
			{  32,  32,  32 },
	};

	// default hullmins
	static const NyaVec3Double pm_hullmins_hl2[4] = {
			{ -16, -16, 0 },
			{ -16, -16, 0 },
			{   0,   0,   0 },
			{ -32, -32, 0 },
	};

	// defualt hullmaxs
	static const NyaVec3Double pm_hullmaxs_hl2[4] = {
			{  16,  16,  72 }, // stand
			{  16,  16,  36 }, // duck
			{   0,   0,   0 }, // point
			{  32,  32,  64 },
	};

	void PM_DropPunchAngle(NyaVec3Double& punchangle) {
		auto len = VectorNormalize(punchangle);
		len -= (10.0 + len * 0.5) * pmove->frametime;
		len = std::max(len, 0.0f);
		VectorScale(punchangle, len, punchangle);
	}

	void DecayPunchAngle() {
		if (pmove->punchangle.LengthSqr() > 0.001 || pmove->punchangle.LengthSqr() > 0.001) {
			pmove->punchangle += pmove->m_vecPunchAngleVel * pmove->frametime;
			float damping = 1 - (PUNCH_DAMPING * pmove->frametime);

			if (damping < 0) {
				damping = 0;
			}
			pmove->m_vecPunchAngleVel *= damping;

			// torsional spring
			// UNDONE: Per-axis spring constant?
			float springForceMagnitude = PUNCH_SPRING_CONSTANT * pmove->frametime;
			if (springForceMagnitude < 0.f) springForceMagnitude = 0.f;
			if (springForceMagnitude > 2.f) springForceMagnitude = 2.f;
			pmove->m_vecPunchAngleVel -= pmove->punchangle * springForceMagnitude;

			// don't wrap around
			if (pmove->punchangle[PITCH] < -89.f) pmove->punchangle[PITCH] = -89.f;
			if (pmove->punchangle[YAW] < -179.f) pmove->punchangle[YAW] = -179.f;
			if (pmove->punchangle[ROLL] < -89.f) pmove->punchangle[ROLL] = -89.f;
			if (pmove->punchangle[PITCH] > 89.f) pmove->punchangle[PITCH] = 89.f;
			if (pmove->punchangle[YAW] > 179.f) pmove->punchangle[YAW] = 179.f;
			if (pmove->punchangle[ROLL] > 89.f) pmove->punchangle[ROLL] = 89.f;
		}
		else {
			pmove->punchangle = { 0, 0, 0 };
			pmove->m_vecPunchAngleVel = { 0, 0, 0 };
		}
	}

	void PlaySwimSound() {
		if (bHL2Mode) {
			switch (rand() % 8) {
				case 0:
					PlayGameSound("player/footsteps/wade1.wav", 1);
					break;
				case 1:
					PlayGameSound("player/footsteps/wade2.wav", 1);
					break;
				case 2:
					PlayGameSound("player/footsteps/wade3.wav", 1);
					break;
				case 3:
					PlayGameSound("player/footsteps/wade4.wav", 1);
					break;
				case 4:
					PlayGameSound("player/footsteps/wade5.wav", 1);
					break;
				case 5:
					PlayGameSound("player/footsteps/wade6.wav", 1);
					break;
				case 6:
					PlayGameSound("player/footsteps/wade7.wav", 1);
					break;
				case 7:
					PlayGameSound("player/footsteps/wade8.wav", 1);
					break;
			}
		}
		else {
			switch (rand() % 4) {
				case 0:
					PlayGameSound("player/pl_wade1.wav", 1);
					break;
				case 1:
					PlayGameSound("player/pl_wade2.wav", 1);
					break;
				case 2:
					PlayGameSound("player/pl_wade3.wav", 1);
					break;
				case 3:
					PlayGameSound("player/pl_wade4.wav", 1);
					break;
			}
		}
	}

	void PM_PlayWaterSounds() {
		// Did we enter or leave water?
		if ((pmove->oldwaterlevel == 0 && pmove->waterlevel != 0) || (pmove->oldwaterlevel != 0 && pmove->waterlevel == 0)) {
			PlaySwimSound();
		}
	}

	float V_CalcRoll(NyaVec3Double angles, NyaVec3Double velocity, float rollangle, float rollspeed) {
		float sign;
		float side;
		float value;
		NyaVec3Double forward, right, up;

		AngleVectors(angles, forward, right, up);

		side = DotProduct(velocity, right);
		sign = side < 0 ? -1 : 1;
		side = std::abs(side);

		value = rollangle;
		if (side < rollspeed) {
			side = side * value / rollspeed;
		}
		else {
			side = value;
		}

		return side * sign;
	}

	float V_CalcBob() {
		auto cl_bobcycle = bHL2Mode ? CVar_HL2::cl_bobcycle : CVar_HL1::cl_bobcycle;
		auto cl_bobup = bHL2Mode ? CVar_HL2::cl_bobup : CVar_HL1::cl_bobup;
		auto cl_bob = bHL2Mode ? CVar_HL2::cl_bob : CVar_HL1::cl_bob;

		static double bobtime;
		static float bob;
		float cycle;
		static float lasttime;
		NyaVec3Double vel;

		if (pmove->onground == -1 || pmove->cmd.msec == lasttime) {
			// just use old value
			return bob;
		}

		lasttime = pmove->cmd.msec;

		bobtime += pmove->frametime;
		cycle = bobtime - (int)(bobtime / cl_bobcycle) * cl_bobcycle;
		cycle /= cl_bobcycle;

		if (cycle < cl_bobup) {
			cycle = M_PI * cycle / cl_bobup;
		}
		else {
			cycle = M_PI + M_PI * (cycle - cl_bobup) / (1.0 - cl_bobup);
		}

		// bob is proportional to simulated velocity in the xy plane
		// (don't count Z, or jumping messes it up)
		VectorCopy(pmove->velocity, vel);
		vel[UP] = 0;

		bob = std::sqrt(vel[0] * vel[0] + vel[FORWARD] * vel[FORWARD]) * cl_bob;
		bob = bob * 0.3 + bob * 0.7 * std::sin(cycle);
		bob = std::min(bob, 4.0f);
		bob = std::max(bob, -7.0f);
		return bob;
	}

	void PM_CheckParamters() {
		float spd;
		float maxspeed;
		NyaVec3Double v_angle;

		spd = std::sqrt((pmove->cmd.forwardmove * pmove->cmd.forwardmove) +
			  (pmove->cmd.sidemove * pmove->cmd.sidemove) +
			  (pmove->cmd.upmove * pmove->cmd.upmove));

		maxspeed = pmove->clientmaxspeed; // atof(pmove->PM_Info_ValueForKey(pmove->physinfo, "maxspd"));
		if (maxspeed != 0.0) {
			pmove->maxspeed = std::min(maxspeed, pmove->maxspeed);
		}

		// Slow down, I'm pulling it! (a box maybe) but only when I'm standing on ground
		//
		// JoshA: Moved this to CheckParamters rather than working on the velocity,
		// as otherwise it affects every integration step incorrectly.
		if ((pmove->onground != -1) && (pmove->cmd.buttons & IN_USE)) {
			pmove->maxspeed *= 1.0f / 3.0f;
		}

		if ((spd != 0.0) && (spd > pmove->maxspeed)) {
			float fRatio = pmove->maxspeed / spd;
			pmove->cmd.forwardmove *= fRatio;
			pmove->cmd.sidemove *= fRatio;
			pmove->cmd.upmove *= fRatio;
		}

		if (pmove->flags & FL_FROZEN ||  pmove->flags & FL_ONTRAIN || pmove->dead) {
			pmove->cmd.forwardmove = pmove->cmd.sidemove = pmove->cmd.upmove = 0;
		}

		if (bHL2Mode) {
			DecayPunchAngle();
		}
		else {
			PM_DropPunchAngle(pmove->punchangle);
		}

		// Take angles from command.
		if (!pmove->dead) {
			v_angle = pmove->cmd.viewangles;

			auto punch = pmove->punchangle;
			punch[PITCH] /= 3.0; // 1/3 pitch from xash3d
			v_angle += punch;

			// Set up view angles.
			// using V_CalcRoll here instead of PM_CalcRoll
			pmove->angles[ROLL] = v_angle[ROLL] + V_CalcRoll(v_angle, pmove->velocity, movevars->rollangle, movevars->rollspeed);
			pmove->angles[PITCH] = v_angle[PITCH];
			pmove->angles[YAW] = v_angle[YAW];
		}
		else
		{
			VectorCopy(pmove->oldangles, pmove->angles);
		}

		// Set dead player view_offset
		if (pmove->dead) {
			pmove->view_ofs[UP] = PM_DEAD_VIEWHEIGHT;
		}

		// Adjust client view angles to match values used on server.
		if (pmove->angles[YAW] > 180.0f) {
			pmove->angles[YAW] -= 360.0f;
		}
		if (pmove->angles[YAW] < -180.0f) {
			pmove->angles[YAW] += 360.0f;
		}
	}

	void PM_ReduceTimers() {
		if (pmove->flTimeStepSound > 0) {
			pmove->flTimeStepSound -= pmove->cmd.msec;
			if (pmove->flTimeStepSound < 0) {
				pmove->flTimeStepSound = 0;
			}
		}

		if (pmove->flDuckTime > 0) {
			pmove->flDuckTime -= pmove->cmd.msec;
			if (pmove->flDuckTime < 0) {
				pmove->flDuckTime = 0;
			}
		}

		if (pmove->m_flDuckJumpTime > 0) {
			pmove->m_flDuckJumpTime -= pmove->cmd.msec;
			if (pmove->m_flDuckJumpTime < 0) {
				pmove->m_flDuckJumpTime = 0;
			}
		}

		if (pmove->m_flJumpTime > 0) {
			pmove->m_flJumpTime -= pmove->cmd.msec;
			if (pmove->m_flJumpTime < 0) {
				pmove->m_flJumpTime = 0;
			}
		}

		if (pmove->flSwimTime > 0) {
			pmove->flSwimTime -= pmove->cmd.msec;
			if (pmove->flSwimTime < 0) {
				pmove->flSwimTime = 0;
			}
		}
	}

	int nPhysicsSteps = 4;
	int nColDensity = 2;

	// get avg between min and max up, should end up 0 for HL1
	float GetPlayerCenterUp() {
		auto min = pmove->player_mins[GetPlayerHullID()];
		auto max = pmove->player_maxs[GetPlayerHullID()];
		return (min[UP] + max[UP]) * 0.5;
	}

	NyaVec3Double GetCenterRelativeBBoxMin() {
		auto v = pmove->player_mins[GetPlayerHullID()];
		v[UP] -= GetPlayerCenterUp();
		return v;
	}

	NyaVec3Double GetCenterRelativeBBoxMax() {
		auto v = pmove->player_maxs[GetPlayerHullID()];
		v[UP] -= GetPlayerCenterUp();
		return v;
	}

	// for ground movement
	pmtrace_t GetTopFloorForBBox(NyaVec3Double origin) {
		std::vector<pmtrace_t> traces;

		origin[UP] += GetPlayerCenterUp();

		for (int x = -nColDensity; x <= nColDensity; x++) {
			auto posX = (double)x / nColDensity;
			for (int z = -nColDensity; z <= nColDensity; z++) {
				auto posZ = (double)z / nColDensity;

				auto bbox = GetCenterRelativeBBoxMax();

				// do a raycast from all sides downwards

				auto start = origin;
				auto end = origin;
				end[0] += posX * bbox.x;
				end[FORWARD] += posZ * bbox[FORWARD];
				end[UP] -= bbox[UP];

				auto trace = PointRaytrace(start, end);
				if (trace.ent != -1) {
					trace.endpos[UP] += bbox[UP];
					traces.push_back(trace);
				}
			}
		}

		auto out = pmtrace_t();
		out.Default();
		out.endpos = {-999999,-999999,-999999};
		for (auto& tr : traces) {
			if (tr.ent != -1 && tr.endpos[UP] > out.endpos[UP]) out = tr;
		}
		out.endpos[UP] -= GetPlayerCenterUp();
		return out;
	}

	// for unducking
	pmtrace_t GetBottomCeilingForBBox(NyaVec3Double origin) {
		std::vector<pmtrace_t> traces;

		origin[UP] += GetPlayerCenterUp();

		for (int x = -nColDensity; x <= nColDensity; x++) {
			auto posX = (double)x / nColDensity;
			for (int z = -nColDensity; z <= nColDensity; z++) {
				auto posZ = (double)z / nColDensity;

				auto bbox = GetCenterRelativeBBoxMax();

				// do a raycast from all sides upwards

				// start from the very bottom
				auto start = origin;
				start[UP] -= bbox[UP];

				auto end = origin;
				end[0] += posX * bbox.x;
				end[FORWARD] += posZ * bbox[FORWARD];
				end[UP] += bbox[UP];

				auto trace = PointRaytrace(start, end);
				if (trace.ent != -1) {
					trace.endpos[UP] -= bbox[UP];
					traces.push_back(trace);
				}
			}
		}

		auto out = pmtrace_t();
		out.Default();
		out.endpos = {999999,999999,999999};
		for (auto& tr : traces) {
			if (tr.ent != -1 && tr.endpos[UP] < out.endpos[UP]) out = tr;
		}
		out.endpos[UP] -= GetPlayerCenterUp();
		return out;
	}

	// for general collisions
	// returns the center as endpoint
	pmtrace_t GetClosestBBoxIntersection(NyaVec3Double origPos, NyaVec3Double targetPos) {
		auto distanceTraveled = (origPos - targetPos).length();

		origPos[UP] += GetPlayerCenterUp();
		targetPos[UP] += GetPlayerCenterUp();

		std::vector<pmtrace_t> traces;

		for (int x = -nColDensity; x <= nColDensity; x++) {
			auto posX = (double)x / nColDensity;
			for (int y = -nColDensity; y <= nColDensity; y++) {
				auto posY = (double)y / nColDensity;
				for (int z = -nColDensity; z <= nColDensity; z++) {
					auto posZ = (double)z / nColDensity;

					auto bbox = GetCenterRelativeBBoxMax();

					// cast from the origin outwards
					auto start = targetPos;
					auto offset = targetPos;
					offset.x = posX * bbox.x;
					offset.y = posY * bbox.y;
					offset.z = posZ * bbox.z;

					auto end = targetPos + offset;

					auto trace = PointRaytrace(start, end);
					if (trace.ent != -1) {
						trace.endpos -= offset;
						trace.fraction = (origPos - trace.endpos).length() / distanceTraveled;
						traces.push_back(trace);
					}
				}
			}
		}

		auto out = pmtrace_t();
		out.Default();
		out.endpos = targetPos;
		for (auto& tr : traces) {
			if (tr.ent != -1 && tr.fraction < out.fraction) out = tr;
		}
		out.endpos[UP] -= GetPlayerCenterUp();
		return out;
	}

	bool PM_CheckWater() {
		NyaVec3Double point;
		int	cont;
		int	truecont;
		float height;
		float heightover2;

		// Pick a spot just above the players feet.
		point[0] = pmove->origin[0] + (pmove->player_mins[GetPlayerHullID()][0] + pmove->player_maxs[GetPlayerHullID()][0]) * 0.5;
		point[FORWARD] = pmove->origin[FORWARD] + (pmove->player_mins[GetPlayerHullID()][FORWARD] + pmove->player_maxs[GetPlayerHullID()][FORWARD]) * 0.5;
		point[UP] = pmove->origin[UP] + pmove->player_mins[GetPlayerHullID()][UP] + 1;

		// Assume that we are not in water at all.
		pmove->waterlevel = 0;
		pmove->watertype = CONTENTS_EMPTY;

		// Grab point contents.
		cont = GetPointContentsGame(&point);
		// Are we under water? (not solid and not empty?)
		if (cont <= CONTENTS_WATER) {
			// Set water type
			pmove->watertype = cont;

			// We are at least at level one
			pmove->waterlevel = 1;

			height = (pmove->player_mins[GetPlayerHullID()][UP] + pmove->player_maxs[GetPlayerHullID()][UP]);
			heightover2 = height * 0.5;

			// Now check a point that is at the player hull midpoint.
			point[UP] = pmove->origin[UP] + heightover2;
			cont = GetPointContentsGame(&point);
			// If that point is also under water...
			if (cont <= CONTENTS_WATER) {
				// Set a higher water level.
				pmove->waterlevel = 2;

				// Now check the eye position.  (view_ofs is relative to the origin)
				point[UP] = pmove->origin[UP] + pmove->view_ofs[UP];

				cont = GetPointContentsGame(&point);
				if (cont <= CONTENTS_WATER) {
					pmove->waterlevel = 3;  // In over our eyes
				}
			}

			// Adjust velocity based on water current, if any.
			/*if ((truecont <= CONTENTS_CURRENT_0) && (truecont >= CONTENTS_CURRENT_DOWN)) {
				// The deeper we are, the stronger the current.
				static vec3_t current_table[] =
						{
								{1, 0, 0}, {0, 1, 0}, {-1, 0, 0},
								{0, -1, 0}, {0, 0, 1}, {0, 0, -1}
						};

				VectorMA (pmove->basevelocity, 50.0*pmove->waterlevel, current_table[CONTENTS_CURRENT_0 - truecont], pmove->basevelocity);
			}*/
		}

		return pmove->waterlevel > 1;
	}

	float fLastPlaneNormal;
	void PM_CatagorizePosition() {
		NyaVec3Double point;
		pmtrace_t tr;

		// if the player hull point one unit down is solid, the player
		// is on ground

		// see if standing on something solid

		// Doing this before we move may introduce a potential latency in water detection, but
		// doing it after can get us stuck on the bottom in water if the amount we move up
		// is less than the 1 pixel 'threshold' we're about to snap to.	Also, we'll call
		// this several times per frame, so we really need to avoid sticking to the bottom of
		// water on each call, and the converse case will correct itself if called twice.
		PM_CheckWater();

		point[0] = pmove->origin[0];
		point[FORWARD] = pmove->origin[FORWARD];
		point[UP] = pmove->origin[UP] - 2;

		float maxUpVelocity = bHL2Mode ? NON_JUMP_VELOCITY : 180;
		if (pmove->velocity[UP] > maxUpVelocity || pmove->movetype == MOVETYPE_NOCLIP) { // Shooting up really fast.  Definitely not on ground.
			pmove->onground = -1;
		} else {
			// Try and move down.
			if (IsUsingPlayerTraceFallback()) {
				tr = GetTopFloorForBBox(point);
			}
			else {
				tr = PM_PlayerTraceDown(pmove->origin, point);
			}

			fLastPlaneNormal = tr.plane.normal[UP];
			// If we hit a steep plane, we are not on ground
			if (tr.plane.normal[UP] < 0.7) {
				pmove->onground = -1;    // too steep
			} else {
				pmove->onground = tr.ent;  // Otherwise, point to index of ent under us.
			}

			// If we are on something...
			if (pmove->onground != -1) {
				pmove->chtexturetype = tr.surfaceId;

				// Then we are not in water jump sequence
				pmove->waterjumptime = 0;
				// If we could make the move, drop us down that 1 pixel
				if (pmove->waterlevel < 2/* && !tr.startsolid && !tr.allsolid*/) {
					if (IsUsingPlayerTraceFallback()) {
						// hacked here to only use the endpos Y
						pmove->origin[UP] = tr.endpos[UP];
					}
					else {
						VectorCopy(tr.endpos, pmove->origin);
					}
				}
			}

			// todo
			// Standing on an entity other than the world
			//if (tr.ent > 0) { // So signal that we are touching something.
			//	PM_AddToTouched(tr, pmove->velocity);
			//}
		}
	}

	void PM_PlayStepSound(int step, float fvol) {
		static int iSkipStep = 0;
		NyaVec3Double hvel;

		pmove->iStepLeft = !pmove->iStepLeft;

		int irand = (rand() % 2) + (pmove->iStepLeft * 2);

		VectorCopy(pmove->velocity, hvel);
		hvel[UP] = 0.0;

		// irand - 0,1 for right foot, 2,3 for left foot
		// used to alternate left and right foot
		if (bHL2Mode) {
			switch (step) {
				default:
				case CHAR_TEX_CONCRETE:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/footsteps/concrete1.wav", fvol); break;
						case 1:	PlayGameSound("player/footsteps/concrete3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/footsteps/concrete2.wav", fvol); break;
						case 3:	PlayGameSound("player/footsteps/concrete4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_METAL:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/footsteps/metal1.wav", fvol); break;
						case 1:	PlayGameSound("player/footsteps/metal3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/footsteps/metal2.wav", fvol); break;
						case 3:	PlayGameSound("player/footsteps/metal4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_DIRT:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/footsteps/dirt1.wav", fvol); break;
						case 1:	PlayGameSound("player/footsteps/dirt3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/footsteps/dirt2.wav", fvol); break;
						case 3:	PlayGameSound("player/footsteps/dirt4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_VENT:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/footsteps/duct1.wav", fvol); break;
						case 1:	PlayGameSound("player/footsteps/duct3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/footsteps/duct2.wav", fvol); break;
						case 3:	PlayGameSound("player/footsteps/duct4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_GRATE:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/footsteps/metalgrate1.wav", fvol); break;
						case 1:	PlayGameSound("player/footsteps/metalgrate3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/footsteps/metalgrate2.wav", fvol); break;
						case 3:	PlayGameSound("player/footsteps/metalgrate4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_TILE:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/footsteps/tile1.wav", fvol); break;
						case 1:	PlayGameSound("player/footsteps/tile3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/footsteps/tile2.wav", fvol); break;
						case 3:	PlayGameSound("player/footsteps/tile4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_WOOD:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/footsteps/wood1.wav", fvol); break;
						case 1:	PlayGameSound("player/footsteps/wood3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/footsteps/wood2.wav", fvol); break;
						case 3:	PlayGameSound("player/footsteps/wood4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_SAND:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/footsteps/sand1.wav", fvol); break;
						case 1:	PlayGameSound("player/footsteps/sand3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/footsteps/sand2.wav", fvol); break;
						case 3:	PlayGameSound("player/footsteps/sand4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_MUD:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/footsteps/mud1.wav", fvol); break;
						case 1:	PlayGameSound("player/footsteps/mud3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/footsteps/mud2.wav", fvol); break;
						case 3:	PlayGameSound("player/footsteps/mud4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_GRASS:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/footsteps/grass1.wav", fvol); break;
						case 1:	PlayGameSound("player/footsteps/grass3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/footsteps/grass2.wav", fvol); break;
						case 3:	PlayGameSound("player/footsteps/grass4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_GRAVEL:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/footsteps/gravel1.wav", fvol); break;
						case 1:	PlayGameSound("player/footsteps/gravel3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/footsteps/gravel2.wav", fvol); break;
						case 3:	PlayGameSound("player/footsteps/gravel4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_CHAINLINK:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/footsteps/chainlink1.wav", fvol); break;
						case 1:	PlayGameSound("player/footsteps/chainlink3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/footsteps/chainlink2.wav", fvol); break;
						case 3:	PlayGameSound("player/footsteps/chainlink4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_SLOSH:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/footsteps/slosh1.wav", fvol); break;
						case 1:	PlayGameSound("player/footsteps/slosh3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/footsteps/slosh2.wav", fvol); break;
						case 3:	PlayGameSound("player/footsteps/slosh4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_WADE:
					// todo there's 8 of these!
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/footsteps/wade1.wav", fvol); break;
						case 1:	PlayGameSound("player/footsteps/wade2.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/footsteps/wade3.wav", fvol); break;
						case 3:	PlayGameSound("player/footsteps/wade4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_LADDER:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/footsteps/ladder1.wav", fvol); break;
						case 1:	PlayGameSound("player/footsteps/ladder3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/footsteps/ladder2.wav", fvol); break;
						case 3:	PlayGameSound("player/footsteps/ladder4.wav", fvol); break;
					}
					break;
			}
		}
		else {
			switch (step) {
				default:
				case CHAR_TEX_CONCRETE:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/pl_step1.wav", fvol); break;
						case 1:	PlayGameSound("player/pl_step3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/pl_step2.wav", fvol); break;
						case 3:	PlayGameSound("player/pl_step4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_CHAINLINK:
				case CHAR_TEX_METAL:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/pl_metal1.wav", fvol); break;
						case 1:	PlayGameSound("player/pl_metal3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/pl_metal2.wav", fvol); break;
						case 3:	PlayGameSound("player/pl_metal4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_DIRT:
				case CHAR_TEX_SAND:
				case CHAR_TEX_MUD:
				case CHAR_TEX_GRASS:
				case CHAR_TEX_GRAVEL:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/pl_dirt1.wav", fvol); break;
						case 1:	PlayGameSound("player/pl_dirt3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/pl_dirt2.wav", fvol); break;
						case 3:	PlayGameSound("player/pl_dirt4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_VENT:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/pl_duct1.wav", fvol); break;
						case 1:	PlayGameSound("player/pl_duct3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/pl_duct2.wav", fvol); break;
						case 3:	PlayGameSound("player/pl_duct4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_GRATE:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/pl_grate1.wav", fvol); break;
						case 1:	PlayGameSound("player/pl_grate3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/pl_grate2.wav", fvol); break;
						case 3:	PlayGameSound("player/pl_grate4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_TILE:
					if (!(rand() % 5)) {
						irand = 4;
					}

					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/pl_tile1.wav", fvol); break;
						case 1:	PlayGameSound("player/pl_tile3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/pl_tile2.wav", fvol); break;
						case 3:	PlayGameSound("player/pl_tile4.wav", fvol); break;
						case 4: PlayGameSound("player/pl_tile5.wav", fvol); break;
					}
					break;
				case CHAR_TEX_SLOSH:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/pl_slosh1.wav", fvol); break;
						case 1:	PlayGameSound("player/pl_slosh3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/pl_slosh2.wav", fvol); break;
						case 3:	PlayGameSound("player/pl_slosh4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_WADE:
					if (iSkipStep == 0) {
						iSkipStep++;
						break;
					}

					if (iSkipStep++ == 3) {
						iSkipStep = 0;
					}

					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/pl_wade1.wav", fvol); break;
						case 1:	PlayGameSound("player/pl_wade2.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/pl_wade3.wav", fvol); break;
						case 3:	PlayGameSound("player/pl_wade4.wav", fvol); break;
					}
					break;
				case CHAR_TEX_LADDER:
					switch (irand) {
						// right foot
						case 0:	PlayGameSound("player/pl_ladder1.wav", fvol); break;
						case 1:	PlayGameSound("player/pl_ladder3.wav", fvol); break;
						// left foot
						case 2:	PlayGameSound("player/pl_ladder2.wav", fvol); break;
						case 3:	PlayGameSound("player/pl_ladder4.wav", fvol); break;
					}
					break;
			}
		}
	}

	void PM_UpdateStepSound() {
		int	fWalking;
		float fvol;
		NyaVec3Double knee;
		NyaVec3Double feet;
		NyaVec3Double center;
		float height;
		float speed;
		float velrun;
		float velwalk;
		float flduck;
		int	fLadder;
		int step;

		if (pmove->flTimeStepSound > 0) return;
		if (pmove->flags & FL_FROZEN) return;

		// moved to walkmove
		//PM_CatagorizeTextureType();

		speed = pmove->velocity.length();

		// determine if we are on a ladder
		fLadder = pmove->movetype == MOVETYPE_FLY; // IsOnLadder();

		// UNDONE: need defined numbers for run, walk, crouch, crouch run velocities!!!!
		if ((pmove->flags & FL_DUCKING) || fLadder) {
			velwalk = 60;		// These constants should be based on cl_movespeedkey * cl_forwardspeed somehow
			velrun = 80;		// UNDONE: Move walking to server
			flduck = 100;
		} else {
			if (bHL2Mode) {
				velwalk = 90;
				velrun = 220;
			}
			else {
				velwalk = 120;
				velrun = 210;
			}
			flduck = 0;
		}

		// If we're on a ladder or on the ground, and we're moving fast enough,
		//  play step sound.  Also, if pmove->flTimeStepSound is zero, get the new
		//  sound right away - we just started moving in new level.
		if ((fLadder || (pmove->onground != -1)) && (pmove->velocity.length() > 0.0) && (speed >= velwalk || !pmove->flTimeStepSound)) {
			fWalking = speed < velrun;

			VectorCopy(pmove->origin, center);
			VectorCopy(pmove->origin, knee);
			VectorCopy(pmove->origin, feet);

			height = pmove->player_maxs[GetPlayerHullID()][UP] - pmove->player_mins[GetPlayerHullID()][UP];

			knee[UP] = pmove->origin[UP] - 0.3 * height;
			feet[UP] = pmove->origin[UP] - 0.5 * height;

			if (GetPointContentsGame(&knee) == CONTENTS_WATER) {
				step = CHAR_TEX_WADE;
				fvol = 0.65;
				pmove->flTimeStepSound = 600;
			} else if (GetPointContentsGame(&feet) == CONTENTS_WATER) {
				step = CHAR_TEX_SLOSH;
				fvol = fWalking ? 0.2 : 0.5;
				pmove->flTimeStepSound = fWalking ? 400 : 300;
			} else {
				step = pmove->chtexturetype;

				switch (pmove->chtexturetype) {
					default:
					case CHAR_TEX_CONCRETE:
						fvol = fWalking ? 0.2 : 0.5;
						pmove->flTimeStepSound = fWalking ? 400 : 300;
						break;

					case CHAR_TEX_METAL:
						fvol = fWalking ? 0.2 : 0.5;
						pmove->flTimeStepSound = fWalking ? 400 : 300;
						break;

					case CHAR_TEX_DIRT:
						fvol = fWalking ? 0.25 : 0.55;
						pmove->flTimeStepSound = fWalking ? 400 : 300;
						break;

					case CHAR_TEX_VENT:
						fvol = fWalking ? 0.4 : 0.7;
						pmove->flTimeStepSound = fWalking ? 400 : 300;
						break;

					case CHAR_TEX_GRATE:
						fvol = fWalking ? 0.2 : 0.5;
						pmove->flTimeStepSound = fWalking ? 400 : 300;
						break;

					case CHAR_TEX_TILE:
						fvol = fWalking ? 0.2 : 0.5;
						pmove->flTimeStepSound = fWalking ? 400 : 300;
						break;

					case CHAR_TEX_SLOSH:
						fvol = fWalking ? 0.2 : 0.5;
						pmove->flTimeStepSound = fWalking ? 400 : 300;
						break;
				}
			}

			pmove->flTimeStepSound += flduck; // slower step time if ducking

			// play the sound
			// 35% volume if ducking
			if (pmove->flags & FL_DUCKING) {
				fvol *= 0.35;
			}

			PM_PlayStepSound(step, fvol);
		}
	}

	void PM_UnDuck() {
		pmtrace_t trace;
		NyaVec3Double newOrigin = pmove->origin;

		if (pmove->onground != -1) {
			for (int i = 0; i < 3; i++) {
				newOrigin[i] += pmove->player_mins[1][i] - pmove->player_mins[0][i];
			}
		}

		if (IsUsingPlayerTraceFallback()) {
			trace = GetBottomCeilingForBBox(newOrigin);
			trace.startsolid = trace.ent != -1;
		}
		else {
			trace = PM_PlayerTrace(newOrigin, newOrigin);
		}

		if (!trace.startsolid) {
			pmove->usehull = 0;

			// Oh, no, changing hulls stuck us into something, try unsticking downward first.
			if (IsUsingPlayerTraceFallback()) {
				trace = GetBottomCeilingForBBox(newOrigin);
				trace.startsolid = trace.ent != -1;
			}
			else {
				trace = PM_PlayerTrace(newOrigin, newOrigin);
			}

			if (trace.startsolid) {
				// See if we are stuck?  If so, stay ducked with the duck hull until we have a clear spot
				lastConsoleMsg = "unstick got stuck";
				pmove->usehull = 1;
				return;
			}

			pmove->flags &= ~FL_DUCKING;
			pmove->bInDuckHL1  = false;
			pmove->view_ofs[UP] = VEC_VIEW();
			pmove->flDuckTime = 0;

			VectorCopy(newOrigin, pmove->origin);

			// Recatagorize position since ducking can change origin
			PM_CatagorizePosition();
		}
	}

	float PM_SplineFraction(float value, float scale) {
		value = scale * value;
		auto valueSquared = value * value;

		// Nice little ease-in, ease-out spline-like curve
		return 3 * valueSquared - 2 * valueSquared * value;
	}

	void PM_Duck() {
		float time;
		float duckFraction;

		int buttonsChanged = (pmove->oldbuttons ^ pmove->cmd.buttons);	// These buttons have changed this frame
		int nButtonPressed = buttonsChanged & pmove->cmd.buttons;		// The changed ones still down are "pressed"

		int duckchange = buttonsChanged & IN_DUCK ? 1 : 0;
		int duckpressed	= nButtonPressed & IN_DUCK ? 1 : 0;

		if (pmove->cmd.buttons & IN_DUCK) {
			pmove->oldbuttons |= IN_DUCK;
		} else {
			pmove->oldbuttons &= ~IN_DUCK;
		}

		// Prevent ducking if the iuser3 variable is set
		if (pmove->dead) {
			// Try to unduck
			if (pmove->flags & FL_DUCKING) {
				PM_UnDuck();
			}
			return;
		}

		if (!(pmove->m_iSpeedCropped & SPEED_CROPPED_DUCK) && pmove->flags & FL_DUCKING) {
			pmove->cmd.forwardmove *= PLAYER_DUCKING_MULTIPLIER;
			pmove->cmd.sidemove *= PLAYER_DUCKING_MULTIPLIER;
			pmove->cmd.upmove *= PLAYER_DUCKING_MULTIPLIER;
			pmove->m_iSpeedCropped |= SPEED_CROPPED_DUCK;
		}

		if ((pmove->cmd.buttons & IN_DUCK) || pmove->bInDuckHL1 || (pmove->flags & FL_DUCKING)) {
			if (pmove->cmd.buttons & IN_DUCK) {
				if ((nButtonPressed & IN_DUCK) && !(pmove->flags & FL_DUCKING)) {
					// Use 1 second so super long jump will work
					pmove->flDuckTime = 1000;
					pmove->bInDuckHL1 = true;
				}

				time = std::max(0.0, (1.0 - (float)pmove->flDuckTime / 1000.0));

				if (pmove->bInDuckHL1) {
					// Finish ducking immediately if duck time is over or not on ground
					if (((float)pmove->flDuckTime / 1000.0 <= (1.0 - TIME_TO_DUCK)) || (pmove->onground == -1)) {
						pmove->usehull = 1;
						pmove->view_ofs[UP] = VEC_DUCK_VIEW();
						pmove->flags |= FL_DUCKING;
						pmove->bInDuckHL1 = false;

						if (pmove->onground != -1) {
							for (int i = 0; i < 3; i++) {
								pmove->origin[i] -= pmove->player_mins[1][i] - pmove->player_mins[0][i];
							}

							// todo
							// See if we are stuck?
							//PM_FixPlayerCrouchStuck(STUCK_MOVEUP);

							// Recatagorize position since ducking can change origin
							PM_CatagorizePosition();
						}
					} else {
						float fMore = (VEC_DUCK_HULL_MIN - VEC_HULL_MIN);

						// Calc parametric time
						duckFraction = PM_SplineFraction(time, (1.0 / TIME_TO_DUCK));
						pmove->view_ofs[UP] = ((VEC_DUCK_VIEW() - fMore) * duckFraction) + (VEC_VIEW() * (1 - duckFraction));
					}
				}
			} else {
				// Try to unduck
				PM_UnDuck();
			}
		}
	}

	void PM_NoClip() {
		NyaVec3Double wishvel;
		float fmove, smove;

		// Copy movement amounts
		fmove = pmove->cmd.forwardmove;
		smove = pmove->cmd.sidemove;

		VectorNormalize(pmove->forward);
		VectorNormalize(pmove->right);

		for (int i = 0; i < 3; i++) { // Determine x and y parts of velocity
			wishvel[i] = pmove->forward[i] * fmove + pmove->right[i] * smove;
		}
		wishvel[UP] += pmove->cmd.upmove;

		VectorMA(pmove->origin, pmove->frametime, wishvel, pmove->origin);

		// Zero out the velocity so that we don't accumulate a huge downward velocity from
		//  gravity, etc.
		VectorClear(pmove->velocity);
	}

	bool PM_InWater() {
		return pmove->waterlevel > 1;
	}

	void PM_CheckVelocity() {
		if (bSmartVelocityCap) {
			// handle horizontal movement as one
			auto hvel = pmove->velocity;
			hvel[UP] = 0;
			if (hvel.length() > movevars->maxvelocity) {
				hvel.Normalize();
				hvel *= movevars->maxvelocity;
				pmove->velocity[0] = hvel[0];
				pmove->velocity[FORWARD] = hvel[FORWARD];
			}

			// bound the up vector normally
			if (pmove->velocity[UP] > movevars->maxvelocity) {
				pmove->velocity[UP] = movevars->maxvelocity;
			}
			else if (pmove->velocity[UP] < -movevars->maxvelocity) {
				pmove->velocity[UP] = -movevars->maxvelocity;
			}

			// this won't work for ABH
			//if (pmove->velocity.length() > movevars->maxvelocity) {
			//	pmove->velocity.Normalize();
			//	pmove->velocity *= movevars->maxvelocity;
			//}
		}
		else {
			//
			// bound velocity
			//
			for (int i = 0; i < 3; i++) {
				// Bound it.
				if (pmove->velocity[i] > movevars->maxvelocity) {
					lastConsoleMsg = "PM Got a velocity too high on " + std::to_string(i);
					pmove->velocity[i] = movevars->maxvelocity;
				}
				else if (pmove->velocity[i] < -movevars->maxvelocity) {
					lastConsoleMsg = "PM Got a velocity too low on " + std::to_string(i);
					pmove->velocity[i] = -movevars->maxvelocity;
				}
			}
		}
	}

	void PM_FixupGravityVelocity() {
		if (pmove->waterjumptime) return;

		float ent_gravity = pmove->gravity ? pmove->gravity : 1.0;

		// Get the correct velocity for the end of the dt
		pmove->velocity[UP] -= ent_gravity * movevars->gravity * pmove->frametime * 0.5;

		PM_CheckVelocity();
	}

	void PM_AddCorrectGravity() {
		if (pmove->waterjumptime) return;

		float ent_gravity = pmove->gravity ? pmove->gravity : 1.0;

		// Add gravity so they'll be in the correct position during movement
		// yes, this 0.5 looks wrong, but it's not.
		pmove->velocity[UP] -= ent_gravity * movevars->gravity * 0.5 * pmove->frametime;
		pmove->velocity[UP] += pmove->basevelocity[UP] * pmove->frametime;
		pmove->basevelocity[UP] = 0;

		PM_CheckVelocity();
	}

	void PM_Friction() {
		NyaVec3Double vel;
		float speed, newspeed, control;
		float friction;
		float drop;
		NyaVec3Double newvel;

		// If we are in water jump cycle, don't apply friction
		if (pmove->waterjumptime) return;

		// Get velocity
		vel = pmove->velocity;

		// Calculate speed
		speed = std::sqrt(vel[0]*vel[0] + vel[1]*vel[1] + vel[2]*vel[2]);

		// If too slow, return
		if (speed < 0.1f) {
			return;
		}

		drop = 0;

		// apply ground friction
		if (pmove->onground != -1) { // On an entity that is the ground
			NyaVec3Double start, stop;
			pmtrace_t trace;

			start[0] = stop[0] = pmove->origin[0] + vel[0]/speed*16;
			start[FORWARD] = stop[FORWARD] = pmove->origin[FORWARD] + vel[FORWARD]/speed*16;
			start[UP] = pmove->origin[UP] + pmove->player_mins[GetPlayerHullID()][UP];
			stop[UP] = start[UP] - 34;

			if (IsUsingPlayerTraceFallback()) {
				// todo - edgefriction is not implemented for the fallback
				friction = movevars->friction;
			}
			else {
				trace = PM_PlayerTrace(start, stop);

				if (trace.fraction == 1.0f) {
					friction = movevars->friction*movevars->edgefriction;
				} else {
					friction = movevars->friction;
				}
			}

			friction *= pmove->friction;  // player friction?

			// Bleed off some speed, but if we have less than the bleed
			//  threshhold, bleed the theshold amount.
			control = (speed < movevars->stopspeed) ? movevars->stopspeed : speed;
			// Add the amount to t'he drop amount.
			drop += control*friction*pmove->frametime;
		}

		// scale the velocity
		newspeed = speed - drop;
		if (newspeed < 0) {
			newspeed = 0;
		}

		// Determine proportion of old speed we are using.
		newspeed /= speed;

		// Adjust velocity according to proportion.
		newvel = vel * newspeed;

		VectorCopy(newvel, pmove->velocity);
	}

	void PM_AirAccelerate(NyaVec3Double wishdir, float wishspeed, float accel) {
		float addspeed, accelspeed, currentspeed, wishspd = wishspeed;

		if (pmove->dead) return;
		if (pmove->waterjumptime) return;

		// Cap speed
		//wishspd = VectorNormalize (pmove->wishveloc);

		if (wishspd > 30) {
			wishspd = 30;
		}
		// Determine veer amount
		currentspeed = DotProduct(pmove->velocity, wishdir);
		// See how much to add
		addspeed = wishspd - currentspeed;
		// If not adding any, done.
		if (addspeed <= 0) return;

		// Determine acceleration speed after acceleration
		accelspeed = accel * wishspeed * pmove->frametime * pmove->friction;
		// Cap it
		if (accelspeed > addspeed) {
			accelspeed = addspeed;
		}

		// Adjust pmove vel.
		pmove->velocity += wishdir * accelspeed;
	}

	int PM_ClipVelocity(const NyaVec3Double& in, const NyaVec3Double& normal, NyaVec3Double& out, float overbounce) {
		float backoff;
		float change;
		float angle;
		int	blocked;

		angle = normal[UP];

		blocked = 0x00; 	// Assume unblocked.
		if (angle > 0) {	// If the plane that is blocking us has a positive z component, then assume it's a floor.
			blocked |= 0x01;
		}
		if (!angle) {		// If the plane has no Z, it is vertical (wall/step)
			blocked |= 0x02;
		}

		// Determine how far along plane to slide based on incoming direction.
		// Scale by overbounce factor.
		backoff = DotProduct(in, normal) * overbounce;

		for (int i = 0; i < 3; i++) {
			change = normal[i] * backoff;
			out[i] = in[i] - change;
			// If out velocity is too small, zero it out.
			if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON) {
				out[i] = 0;
			}
		}

		// Return blocking flags.
		return blocked;
	}

	void PlayerRoughLandingEffects(float fvol) {
		if (fvol > 0.0) {
			//
			// Play landing sound right away.
			pmove->flTimeStepSound = 400;

			// Play step sound for current texture.
			PM_PlayStepSound(pmove->chtexturetype, fvol);

			//
			// Knock the screen around a little bit, temporary effect.
			//
			pmove->punchangle[ROLL] = pmove->flFallVelocity * 0.013;

			if (pmove->punchangle[PITCH] > 8) {
				pmove->punchangle[PITCH] = 8;
			}
		}
	}

	int PM_FlyMove() {
		int	bumpcount, numbumps;
		NyaVec3Double dir;
		float d;
		int	numplanes;
		NyaVec3Double planes[MAX_CLIP_PLANES];
		NyaVec3Double primal_velocity, original_velocity;
		NyaVec3Double new_velocity;
		pmtrace_t trace;
		NyaVec3Double end;
		float time_left, allFraction;
		int	blocked;

		numbumps  = IsUsingPlayerTraceFallback() ? 1 : 4;	// Bump up to four times

		blocked   = 0;           // Assume not blocked
		numplanes = 0;           //  and not sliding along any planes
		VectorCopy(pmove->velocity, original_velocity);  // Store original velocity
		VectorCopy(pmove->velocity, primal_velocity);

		allFraction = 0;
		time_left = pmove->frametime;   // Total time for this movement operation.

		for (bumpcount = 0; bumpcount < numbumps; bumpcount++) {
			if (!pmove->velocity[0] && !pmove->velocity[1] && !pmove->velocity[2]) {
				break;
			}

			// Assume we can move all the way from the current origin to the
			//  end point.
			end = pmove->origin + pmove->velocity * time_left;

			// See if we can make it from origin to end point.
			if (IsUsingPlayerTraceFallback()) {
				trace = GetClosestBBoxIntersection(pmove->origin, end);
			}
			else {
				trace = PM_PlayerTrace(pmove->origin, end);
			}

			allFraction += trace.fraction;
			// If we started in a solid object, or we were in solid space
			//  the whole way, zero out our velocity and return that we
			//  are blocked by floor and wall.
			if (trace.allsolid) { // entity is trapped in another solid
				VectorCopy(vec3_origin, pmove->velocity);
				lastConsoleMsg = "Trapped 4";
				return 4;
			}

			// If we moved some portion of the total distance, then
			//  copy the end position into the pmove->origin and
			//  zero the plane counter.
			if (trace.fraction > 0) { // actually covered some distance
				VectorCopy(trace.endpos, pmove->origin);
				VectorCopy(pmove->velocity, original_velocity);
				numplanes = 0;
			}

			// If we covered the entire distance, we are done
			//  and can return.
			if (trace.fraction == 1.0f) break; // moved the entire distance

			// todo
			// Save entity that blocked us (since fraction was < 1.0)
			//  for contact
			// Add it if it's not already in the list!!!
			//PM_AddToTouched(trace, pmove->velocity);

			// If the plane we hit has a high z component in the normal, then
			//  it's probably a floor
			if (trace.plane.normal[UP] > 0.7) {
				blocked |= 1;		// floor
			}
			// If the plane has a zero z component in the normal, then it's a
			//  step or wall
			if (!trace.plane.normal[UP]) {
				blocked |= 2;		// step / wall
				lastConsoleMsg = "Blocked by " + std::to_string(trace.ent);
			}

			// Reduce amount of pmove->frametime left by total time left * fraction
			//  that we covered.
			time_left -= time_left * trace.fraction;

			// Did we run out of planes to clip against?
			if (numplanes >= MAX_CLIP_PLANES) {	// this shouldn't really happen
				//  Stop our movement if so.
				VectorCopy(vec3_origin, pmove->velocity);
				lastConsoleMsg = "Too many planes 4";
				break;
			}

			// Set up next clipping plane
			VectorCopy(trace.plane.normal, planes[numplanes]);
			numplanes++;

			// modify original_velocity so it parallels all of the clip planes
			//
			// relfect player velocity
			// Only give this a try for first impact plane because you can get yourself stuck in an acute corner by jumping in place
			//  and pressing forward and nobody was really using this bounce/reflection feature anyway...
			if (numplanes == 1 && pmove->movetype == MOVETYPE_WALK && ((pmove->onground == -1) || (pmove->friction != 1))) {
				for (int i = 0; i < numplanes; i++) {
					if (planes[i][UP] > 0.7) { // floor or slope
						PM_ClipVelocity(original_velocity, planes[i], new_velocity, 1);
						VectorCopy(new_velocity, original_velocity);
					}
					else {
						PM_ClipVelocity(original_velocity, planes[i], new_velocity, 1.0 + movevars->bounce * (1 - pmove->friction));
					}
				}

				VectorCopy(new_velocity, pmove->velocity);
				VectorCopy(new_velocity, original_velocity);
			}
			else
			{
				int i = 0;
				for (i = 0; i < numplanes; i++) {
					PM_ClipVelocity(original_velocity,planes[i],pmove->velocity,1);
					int j = 0;
					for (j = 0; j < numplanes; j++) {
						if (j != i) {
							// Are we now moving against this plane?
							if (DotProduct(pmove->velocity, planes[j]) < 0) {
								break;    // not ok
							}
						}
					}
					if (j == numplanes) break; // Didn't have to clip, so we're ok
				}

				// Did we go all the way through plane set
				if (i != numplanes) {	// go along this plane
					// pmove->velocity is set in clipping call, no need to set again.
				}
				else { // go along the crease
					if (numplanes != 2) {
						lastConsoleMsg = "clip velocity, numplanes == " + std::to_string(numplanes);
						VectorCopy(vec3_origin, pmove->velocity);
						break;
					}
					CrossProduct(planes[0], planes[1], dir);
					d = DotProduct(dir, pmove->velocity);
					VectorScale(dir, d, pmove->velocity);
				}

				//
				// if original velocity is against the original velocity, stop dead
				// to avoid tiny occilations in sloping corners
				//
				if (DotProduct(pmove->velocity, primal_velocity) <= 0) {
					lastConsoleMsg = "Back";
					VectorCopy(vec3_origin, pmove->velocity);
					break;
				}
			}
		}

		if (allFraction == 0) {
			VectorCopy(vec3_origin, pmove->velocity);
			lastConsoleMsg = "Don't stick";
		}

		if (bHL2Mode) {
			// Check if they slammed into a wall
			float fSlamVol = 0.0f;

			auto primal_velocity_2d = primal_velocity;
			primal_velocity_2d[UP] = 0;
			auto velocity2d = pmove->velocity;
			velocity2d[UP] = 0;

			float fLateralStoppingAmount = primal_velocity_2d.length() - velocity2d.length();
			if (fLateralStoppingAmount > PLAYER_MAX_SAFE_FALL_SPEED_HL2 * 2.0f) {
				fSlamVol = 1.0f;
			}
			else if (fLateralStoppingAmount > PLAYER_MAX_SAFE_FALL_SPEED_HL2) {
				fSlamVol = 0.85f;
			}

			PlayerRoughLandingEffects(fSlamVol);
		}

		return blocked;
	}

	void PM_WaterMove() {
		NyaVec3Double wishvel;
		float wishspeed;
		NyaVec3Double wishdir;
		NyaVec3Double start, dest;
		NyaVec3Double  temp;
		pmtrace_t trace;

		float speed, newspeed, addspeed, accelspeed;

		//
		// user intentions
		//
		for (int i = 0; i < 3; i++) {
			wishvel[i] = pmove->forward[i] * pmove->cmd.forwardmove + pmove->right[i] * pmove->cmd.sidemove;
		}

		// Sinking after no other movement occurs
		if (!pmove->cmd.forwardmove && !pmove->cmd.sidemove && !pmove->cmd.upmove) {
			wishvel[UP] -= 60;        // drift towards bottom
		}
		else { // Go straight up by upmove amount.
			wishvel[UP] += pmove->cmd.upmove;
		}

		// Copy it over and determine speed
		VectorCopy(wishvel, wishdir);
		wishspeed = VectorNormalize(wishdir);

		// Cap speed.
		if (wishspeed > pmove->maxspeed) {
			VectorScale(wishvel, pmove->maxspeed/wishspeed, wishvel);
			wishspeed = pmove->maxspeed;
		}
		// Slow us down a bit.
		wishspeed *= 0.8;

		VectorAdd(pmove->velocity, pmove->basevelocity, pmove->velocity);
		// Water friction
		VectorCopy(pmove->velocity, temp);
		speed = VectorNormalize(temp);
		if (speed) {
			newspeed = speed - pmove->frametime * speed * movevars->friction * pmove->friction;

			if (newspeed < 0) {
				newspeed = 0;
			}
			VectorScale(pmove->velocity, newspeed/speed, pmove->velocity);
		}
		else {
			newspeed = 0;
		}

		//
		// water acceleration
		//
		if (wishspeed < 0.1f) {
			return;
		}

		addspeed = wishspeed - newspeed;
		if (addspeed > 0) {
			VectorNormalize(wishvel);
			accelspeed = movevars->accelerate * wishspeed * pmove->frametime * pmove->friction;
			if (accelspeed > addspeed) {
				accelspeed = addspeed;
			}

			pmove->velocity += wishvel * accelspeed;
		}

		if (!IsUsingPlayerTraceFallback()) {
			// Now move
			// assume it is a stair or a slope, so press down from stepheight above
			VectorMA(pmove->origin, pmove->frametime, pmove->velocity, dest);
			VectorCopy(dest, start);
			start[UP] += movevars->stepsize + 1;

			trace = PM_PlayerTrace(start, dest);

			if (!trace.startsolid && !trace.allsolid) { // walked up the step, so just keep result and exit
				VectorCopy(trace.endpos, pmove->origin);
				return;
			}
		}

		// Try moving straight along out normal path.
		PM_FlyMove();
	}

	void PM_AirMove() {
		NyaVec3Double wishvel;
		float fmove, smove;
		NyaVec3Double wishdir;
		float wishspeed;

		// Copy movement amounts
		fmove = pmove->cmd.forwardmove;
		smove = pmove->cmd.sidemove;

		// Zero out z components of movement vectors
		pmove->forward[UP] = 0;
		pmove->right[UP] = 0;
		// Renormalize
		VectorNormalize(pmove->forward);
		VectorNormalize(pmove->right);

		// Determine x and y parts of velocity
		wishvel[0] = pmove->forward[0]*fmove + pmove->right[0]*smove;
		wishvel[FORWARD] = pmove->forward[FORWARD]*fmove + pmove->right[FORWARD]*smove;
		// Zero out z part of velocity
		wishvel[UP] = 0;

		// Determine maginitude of speed of move
		VectorCopy(wishvel, wishdir);
		wishspeed = VectorNormalize(wishdir);

		// Clamp to server defined max speed
		if (wishspeed > pmove->maxspeed) {
			VectorScale(wishvel, pmove->maxspeed/wishspeed, wishvel);
			wishspeed = pmove->maxspeed;
		}

		PM_AirAccelerate(wishdir, wishspeed, movevars->airaccelerate);

		// Add in any base velocity to the current velocity.
		VectorAdd(pmove->velocity, pmove->basevelocity, pmove->velocity);

		PM_FlyMove();
	}

	void PM_Accelerate(NyaVec3Double wishdir, float wishspeed, float accel) {
		float addspeed, accelspeed, currentspeed;

		// Dead player's don't accelerate
		if (pmove->dead) return;

		// If waterjumping, don't accelerate
		if (pmove->waterjumptime) return;

		// See if we are changing direction a bit
		currentspeed = DotProduct(pmove->velocity, wishdir);

		// Reduce wishspeed by the amount of veer.
		addspeed = wishspeed - currentspeed;

		// If not going to add any speed, done.
		if (addspeed <= 0) return;

		// Determine amount of accleration.
		accelspeed = accel * pmove->frametime * wishspeed * pmove->friction;

		// Cap at addspeed
		if (accelspeed > addspeed) {
			accelspeed = addspeed;
		}

		pmove->velocity += wishdir * accelspeed;
	}

	void PM_WalkMove() {
		int clip;
		int oldonground;

		NyaVec3Double wishvel;
		float spd;
		float fmove, smove;
		NyaVec3Double wishdir;
		float wishspeed;

		NyaVec3Double dest, start;
		NyaVec3Double original, originalvel;
		NyaVec3Double down, downvel;
		float downdist, updist;

		pmtrace_t trace;

		// Copy movement amounts
		fmove = pmove->cmd.forwardmove;
		smove = pmove->cmd.sidemove;

		// Zero out z components of movement vectors
		pmove->forward[UP] = 0;
		pmove->right[UP] = 0;

		VectorNormalize(pmove->forward);  // Normalize remainder of vectors.
		VectorNormalize(pmove->right);    //

		wishvel[0] = pmove->forward[0]*fmove + pmove->right[0]*smove; // Determine x and y parts of velocity
		wishvel[FORWARD] = pmove->forward[FORWARD]*fmove + pmove->right[FORWARD]*smove;
		wishvel[UP] = 0;             // Zero out z part of velocity

		VectorCopy(wishvel, wishdir);   // Determine maginitude of speed of move
		wishspeed = VectorNormalize(wishdir);

		//
		// Clamp to server defined max speed
		//
		if (wishspeed > pmove->maxspeed) {
			VectorScale(wishvel, pmove->maxspeed/wishspeed, wishvel);
			wishspeed = pmove->maxspeed;
		}

		// Set pmove velocity
		pmove->velocity[UP] = 0;
		PM_Accelerate(wishdir, wishspeed, movevars->accelerate);
		pmove->velocity[UP] = 0;

		// Add in any base velocity to the current velocity.
		VectorAdd(pmove->velocity, pmove->basevelocity, pmove->velocity);

		spd = pmove->velocity.length();

		if (spd < 1.0f) {
			VectorClear(pmove->velocity);
			lastConsoleMsg = "clearing small speed";
			return;
		}

		oldonground = pmove->onground;

		// first try just moving to the destination
		dest[0] = pmove->origin[0] + pmove->velocity[0]*pmove->frametime;
		dest[FORWARD] = pmove->origin[FORWARD] + pmove->velocity[FORWARD]*pmove->frametime;
		dest[UP] = pmove->origin[UP];

		// first try moving directly to the next spot
		VectorCopy(dest, start);

		if (IsUsingPlayerTraceFallback()) {
			trace = GetClosestBBoxIntersection(pmove->origin, dest);
		}
		else {
			trace = PM_PlayerTrace(pmove->origin, dest);
		}

		// If we made it all the way, then copy trace end
		//  as new player position.
		if (trace.fraction == 1) {
			VectorCopy(trace.endpos, pmove->origin);
			return;
		}

		if (oldonground == -1 && pmove->waterlevel == 0) return; // Don't walk up stairs if not on ground.
		if (pmove->waterjumptime) return; // If we are jumping out of water, don't do anything more.

		// Try sliding forward both on ground and up 16 pixels
		//  take the move that goes farthest
		VectorCopy(pmove->origin, original);       // Save out original pos &
		VectorCopy(pmove->velocity, originalvel);  //  velocity.

		// Slide move
		clip = PM_FlyMove();

		// Copy the results out
		VectorCopy(pmove->origin, down);
		VectorCopy(pmove->velocity, downvel);

		// Reset original values.
		VectorCopy(original, pmove->origin);

		VectorCopy(originalvel, pmove->velocity);

		// Start out up one stair height
		VectorCopy(pmove->origin, dest);
		dest[UP] += movevars->stepsize;

		if (IsUsingPlayerTraceFallback()) {
			trace = GetClosestBBoxIntersection(pmove->origin, dest);
		}
		else {
			trace = PM_PlayerTrace(pmove->origin, dest);
		}

		// If we started okay and made it part of the way at least,
		//  copy the results to the movement start position and then
		//  run another move try.
		if (!trace.startsolid && !trace.allsolid) {
			VectorCopy(trace.endpos, pmove->origin);
		}

		// slide move the rest of the way.
		clip = PM_FlyMove();

		// Now try going back down from the end point
		//  press down the stepheight
		VectorCopy(pmove->origin, dest);
		dest[UP] -= movevars->stepsize;

		if (IsUsingPlayerTraceFallback()) {
			trace = GetClosestBBoxIntersection(pmove->origin, dest);
		}
		else {
			trace = PM_PlayerTrace(pmove->origin, dest);
		}

		// If we are not on the ground any more then
		//  use the original movement attempt
		if (trace.plane.normal[UP] < 0.7) {
			goto usedown;
		}

		// If the trace ended up in empty space, copy the end
		//  over to the origin.
		if (!trace.startsolid && !trace.allsolid) {
			VectorCopy(trace.endpos, pmove->origin);
		}
		// Copy this origion to up.
		VectorCopy(pmove->origin, pmove->up);

		// decide which one went farther
		downdist = (down[0] - original[0])*(down[0] - original[0])
				   + (down[FORWARD] - original[FORWARD])*(down[FORWARD] - original[FORWARD]);
		updist   = (pmove->up[0]   - original[0])*(pmove->up[0]   - original[0])
				   + (pmove->up[FORWARD]   - original[FORWARD])*(pmove->up[FORWARD]   - original[FORWARD]);

		if (downdist > updist) {
			usedown:
			VectorCopy(down, pmove->origin);
			VectorCopy(downvel, pmove->velocity);
		} else { // copy z value from slide move
			pmove->velocity[UP] = downvel[UP];
		}
	}

	void PM_PreventMegaBunnyJumping() {
		if (!bBhopCap) return;

		// Speed at which bunny jumping is limited
		auto maxscaledspeed = BUNNYJUMP_MAX_SPEED_FACTOR * pmove->maxspeed;

		// Don't divide by zero
		if (maxscaledspeed <= 0.0f) return;

		// Current player speed
		auto spd = pmove->velocity.length();

		if (spd <= maxscaledspeed) return;

		// If we have to crop, apply this cropping fraction to velocity
		auto fraction = (maxscaledspeed / spd) * 0.65; //Returns the modifier for the velocity

		VectorScale(pmove->velocity, fraction, pmove->velocity); //Crop it down!.
	}

	void PM_Jump() {
		if (pmove->dead) {
			pmove->oldbuttons |= IN_JUMP;	// don't jump again until released
			return;
		}

		// See if we are waterjumping.  If so, decrement count and return.
		if (pmove->waterjumptime) {
			pmove->waterjumptime -= pmove->cmd.msec;
			if (pmove->waterjumptime < 0) {
				pmove->waterjumptime = 0;
			}
			return;
		}

		// If we are in the water most of the way...
		if (pmove->waterlevel >= 2) {
			// swimming, not jumping
			pmove->onground = -1;

			if (pmove->watertype == CONTENTS_WATER) {   // We move up a certain amount
				pmove->velocity[UP] = 100;
			}
			else if (pmove->watertype == CONTENTS_SLIME) {
				pmove->velocity[UP] = 80;
			}
			else { // LAVA
				pmove->velocity[UP] = 50;
			}

			// play swiming sound
			if (pmove->flSwimTime <= 0) {
				// Don't play sound again for 1 second
				pmove->flSwimTime = 1000;

				switch (rand() % 4) {
					case 0:
						PlayGameSound("player/pl_wade1.wav", 1);
						break;
					case 1:
						PlayGameSound("player/pl_wade2.wav", 1);
						break;
					case 2:
						PlayGameSound("player/pl_wade3.wav", 1);
						break;
					case 3:
						PlayGameSound("player/pl_wade4.wav", 1);
						break;
				}
			}

			return;
		}

		// No more effect
		if (pmove->onground == -1) {
			pmove->oldbuttons |= IN_JUMP;	// don't jump again until released
			return;		// in air, so no effect
		}

		if (!bAutoHop && pmove->oldbuttons & IN_JUMP) return; // don't pogo stick

		// In the air now.
		pmove->onground = -1;

		PM_PreventMegaBunnyJumping();

		PM_PlayStepSound(pmove->chtexturetype, 1.0);

		// See if user can super long jump?
		bool cansuperjump = bCanLongJump;

		// Acclerate upward
		// If we are ducking...
		if (pmove->bInDuckHL1 || (pmove->flags & FL_DUCKING)) {
			// Adjust for super long jump module
			// UNDONE -- note this should be based on forward angles, not current velocity.
			if (cansuperjump && (pmove->cmd.buttons & IN_DUCK) && pmove->flDuckTime > 0 && pmove->velocity.length() > 50) {
				pmove->punchangle[PITCH] = -5;

				pmove->velocity[0] = pmove->forward[0] * PLAYER_LONGJUMP_SPEED * 1.6;
				pmove->velocity[FORWARD] = pmove->forward[FORWARD] * PLAYER_LONGJUMP_SPEED * 1.6;
				pmove->velocity[UP] = std::sqrt(2 * 800 * 56.0);
			} else {
				pmove->velocity[UP] = std::sqrt(2 * 800 * 45.0);
			}
		} else {
			pmove->velocity[UP] = std::sqrt(2 * 800 * 45.0);
		}

		if (bABH || bABHMixed) {
			// Add a little forward velocity based on your current forward velocity - if you are not sprinting.
			NyaVec3Double vecForward;
			AngleVectors(pmove->angles, vecForward);
			vecForward[UP] = 0;
			VectorNormalize(vecForward);

			// hack for hl1 to have more responsive duck ABH
			bool isSprinting = (pmove->cmd.buttons & IN_RUN) == 0;
			if (pmove->bInDuckHL1) isSprinting = false;

			auto vel2D = pmove->velocity;
			vel2D[UP] = 0;
			auto velLength2D = vel2D.length();

			// We give a certain percentage of the current forward movement as a bonus to the jump speed.  That bonus is clipped
			// to not accumulate over time.
			float flSpeedBoostPerc = (!isSprinting && !pmove->bInDuckHL1) ? 0.5f : 0.1f;
			float flSpeedAddition = std::abs(pmove->cmd.forwardmove * flSpeedBoostPerc);
			float flMaxSpeed = pmove->maxspeed + (pmove->maxspeed * flSpeedBoostPerc);
			float flNewSpeed = (flSpeedAddition + velLength2D);

			// If we're over the maximum, we want to only boost as much as will get us to the goal speed
			if (flNewSpeed > flMaxSpeed) {
				flSpeedAddition -= flNewSpeed - flMaxSpeed;
			}

			if (pmove->cmd.forwardmove < 0.0f) {
				flSpeedAddition *= -1.0f;
			}

			// Add it on
			auto endVel = pmove->velocity;
			VectorAdd((vecForward * flSpeedAddition), pmove->velocity, endVel);
			if (!bABHMixed || endVel.length() > pmove->velocity.length()) {
				pmove->velocity = endVel;
			}
		}

		// Decay it for simulation
		PM_FixupGravityVelocity();

		// Flag that we jumped.
		pmove->oldbuttons |= IN_JUMP;	// don't jump again until released
	}

	void PM_CheckWaterJump() {
		NyaVec3Double vecStart, vecEnd;
		NyaVec3Double flatforward;
		NyaVec3Double flatvelocity;
		float curspeed;
		pmtrace_t tr;
		int savehull;

		// Already water jumping.
		if (pmove->waterjumptime) return;

		// Don't hop out if we just jumped in
		if (pmove->velocity[UP] < -180) return; // only hop out if we are moving up

		// See if we are backing up
		flatvelocity[0] = pmove->velocity[0];
		flatvelocity[FORWARD] = pmove->velocity[FORWARD];
		flatvelocity[UP] = 0;

		// Must be moving
		curspeed = VectorNormalize(flatvelocity);

		// see if near an edge
		flatforward[0] = pmove->forward[0];
		flatforward[FORWARD] = pmove->forward[FORWARD];
		flatforward[UP] = 0;
		VectorNormalize(flatforward);

		// Are we backing into water from steps or something?  If so, don't pop forward
		if (curspeed != 0.0 && (DotProduct(flatvelocity, flatforward) < 0.0)) return;

		VectorCopy(pmove->origin, vecStart);
		vecStart[UP] += WJ_HEIGHT;

		VectorMA(vecStart, 24, flatforward, vecEnd);

		// Trace, this trace should use the point sized collision hull
		savehull = pmove->usehull;
		pmove->usehull = 2;
		tr = PointRaytrace(vecStart, vecEnd);
		if (tr.fraction < 1.0 && std::abs(tr.plane.normal[UP]) < 0.1f) { // Facing a near vertical wall?
			vecStart[UP] += pmove->player_maxs[savehull][UP] - WJ_HEIGHT;
			VectorMA(vecStart, 24, flatforward, vecEnd);
			VectorMA(vec3_origin, -50, tr.plane.normal, pmove->movedir);

			tr = PointRaytrace(vecStart, vecEnd);
			if (tr.fraction == 1.0) {
				pmove->waterjumptime = 2000;
				pmove->velocity[UP] = 225;
				pmove->oldbuttons |= IN_JUMP;
				pmove->flags |= FL_WATERJUMP;
			}
		}

		// Reset the collision hull
		pmove->usehull = savehull;
	}

	void PM_CheckFalling() {
		if (pmove->onground != -1 && !pmove->dead && pmove->flFallVelocity >= PLAYER_FALL_PUNCH_THRESHOLD_HL1) {
			float fvol = 0.5;

			if (pmove->waterlevel > 0) {

			}
			else if (pmove->flFallVelocity > PLAYER_MAX_SAFE_FALL_SPEED_HL1) {
				OnTakeFallDamage(pmove->flFallVelocity * DAMAGE_FOR_FALL_SPEED_HL1);

				PlayGameSound("player/pl_fallpain3.wav", 1);

				fvol = 1.0;
			}
			else if (pmove->flFallVelocity > PLAYER_MAX_SAFE_FALL_SPEED_HL1 / 2) {
				fvol = 0.85;
			}
			else if (pmove->flFallVelocity < PLAYER_MIN_BOUNCE_SPEED_HL1) {
				fvol = 0;
			}

			if (fvol > 0.0) {
				// Play landing step right away
				pmove->flTimeStepSound = 0;

				PM_UpdateStepSound();

				// play step sound for current texture
				PM_PlayStepSound(pmove->chtexturetype, fvol);

				// Knock the screen around a little bit, temporary effect
				pmove->punchangle[ROLL] = pmove->flFallVelocity * 0.013;	// punch z axis

				if (pmove->punchangle[PITCH] > 8) {
					pmove->punchangle[PITCH] = 8;
				}
			}
		}

		if (pmove->onground != -1) {
			pmove->flFallVelocity = 0;
		}
	}

	void PM_WaterJump() {
		if (pmove->waterjumptime > 10000) {
			pmove->waterjumptime = 10000;
		}

		if (!pmove->waterjumptime) return;

		pmove->waterjumptime -= pmove->cmd.msec;
		if (pmove->waterjumptime < 0 || !pmove->waterlevel) {
			pmove->waterjumptime = 0;
			pmove->flags &= ~FL_WATERJUMP;
		}

		pmove->velocity[0] = pmove->movedir[0];
		pmove->velocity[FORWARD] = pmove->movedir[FORWARD];
	}

	void SetDuckedEyeOffset(float duckFraction) {
		auto vDuckHullMin = GetPlayerMins(true);
		auto vStandHullMin = GetPlayerMins(false);

		float fMore = (vDuckHullMin[UP] - vStandHullMin[UP]);

		NyaVec3Double vecDuckViewOffset = GetPlayerViewOffset(true);
		NyaVec3Double vecStandViewOffset = GetPlayerViewOffset(false);
		NyaVec3Double temp = pmove->view_ofs;
		temp[UP] = ((vecDuckViewOffset[UP] - fMore) * duckFraction) + (vecStandViewOffset[UP] * (1 - duckFraction));
		pmove->view_ofs = temp;
	}

	void UpdateDuckJumpEyeOffset() {
		if (pmove->m_flDuckJumpTime != 0.0f) {
			float flDuckMilliseconds = std::max(0.0f, GAMEMOVEMENT_DUCK_TIME - (float)pmove->m_flDuckJumpTime);
			float flDuckSeconds = flDuckMilliseconds / GAMEMOVEMENT_DUCK_TIME;
			if (flDuckSeconds > TIME_TO_UNDUCK) {
				pmove->m_flDuckJumpTime = 0.0f;
				SetDuckedEyeOffset( 0.0f );
			}
			else {
				float flDuckFraction = SimpleSpline(1.0f - (flDuckSeconds / TIME_TO_UNDUCK));
				SetDuckedEyeOffset(flDuckFraction);
			}
		}
	}

	void HandleDuckingSpeedCrop() {
		if (!(pmove->m_iSpeedCropped & SPEED_CROPPED_DUCK) && (pmove->flags & FL_DUCKING) && (GetGroundEntity() != NULL)) {
			float frac = 0.33333333f;
			pmove->cmd.forwardmove *= frac;
			pmove->cmd.sidemove *= frac;
			pmove->cmd.upmove *= frac;
			pmove->m_iSpeedCropped |= SPEED_CROPPED_DUCK;
		}
	}

	void FinishDuck() {
		if (pmove->flags & FL_DUCKING) return;

		pmove->flags |= FL_DUCKING;
		pmove->m_bDucked = true;
		pmove->m_bDucking = false;

		pmove->view_ofs = GetPlayerViewOffset(true);

		// HACKHACK - Fudge for collision bug - no time to fix this properly
		if (GetGroundEntity() != NULL) {
			for (int i = 0; i < 3; i++) {
				pmove->origin[i] -= VEC_DUCK_HULL_MIN_SCALED()[i] - VEC_HULL_MIN_SCALED()[i];
			}
		}
		else {
			NyaVec3Double hullSizeNormal = VEC_HULL_MAX_SCALED() - VEC_HULL_MIN_SCALED();
			NyaVec3Double hullSizeCrouch = VEC_DUCK_HULL_MAX_SCALED() - VEC_DUCK_HULL_MIN_SCALED();
			NyaVec3Double viewDelta = (hullSizeNormal - hullSizeCrouch);
			VectorAdd( pmove->origin, viewDelta, pmove->origin );
		}

		// todo
		// See if we are stuck?
		//FixPlayerCrouchStuck( true );

		// Recategorize position since ducking can change origin
		PM_CatagorizePosition();
	}
	
	void StartUnDuckJump() {
		pmove->flags |= FL_DUCKING;
		pmove->m_bDucked = true;
		pmove->m_bDucking = false;

		pmove->view_ofs = GetPlayerViewOffset(true);

		NyaVec3Double hullSizeNormal = VEC_HULL_MAX_SCALED() - VEC_HULL_MIN_SCALED();
		NyaVec3Double hullSizeCrouch = VEC_DUCK_HULL_MAX_SCALED() - VEC_DUCK_HULL_MIN_SCALED();
		NyaVec3Double viewDelta = (hullSizeNormal - hullSizeCrouch);
		pmove->origin += viewDelta;

		// todo
		// See if we are stuck?
		//FixPlayerCrouchStuck( true );

		// Recategorize position since ducking can change origin
		PM_CatagorizePosition();
	}

	void FinishUnDuckJump(pmtrace_t &trace) {
		auto vecNewOrigin = pmove->origin;

		//  Up for uncrouching.
		NyaVec3Double hullSizeNormal = VEC_HULL_MAX_SCALED() - VEC_HULL_MIN_SCALED();
		NyaVec3Double hullSizeCrouch = VEC_DUCK_HULL_MAX_SCALED() - VEC_DUCK_HULL_MIN_SCALED();
		NyaVec3Double viewDelta = (hullSizeNormal - hullSizeCrouch);

		float flDeltaZ = viewDelta[UP];
		viewDelta[UP] *= trace.fraction;
		flDeltaZ -= viewDelta[UP];

		RemoveFlag(FL_DUCKING);
		pmove->m_bDucked = false;
		pmove->m_bDucking  = false;
		pmove->m_bInDuckJump = false;
		pmove->flDuckTime = 0.0f;
		pmove->m_flDuckJumpTime = 0.0f;
		pmove->m_flJumpTime = 0.0f;

		auto vecViewOffset = GetPlayerViewOffset(false);
		vecViewOffset[UP] -= flDeltaZ;
		pmove->view_ofs = vecViewOffset;

		VectorSubtract( vecNewOrigin, viewDelta, vecNewOrigin );
		pmove->origin = vecNewOrigin;

		// Recategorize position since ducking can change origin
		PM_CatagorizePosition();
	}
	void FinishUnDuck() {
		pmtrace_t trace;
		NyaVec3Double newOrigin;

		VectorCopy(pmove->origin, newOrigin);

		if (GetGroundEntity() != NULL) {
			for (int i = 0; i < 3; i++) {
				newOrigin[i] += (VEC_DUCK_HULL_MIN_SCALED()[i] - VEC_HULL_MIN_SCALED()[i]);
			}
		}
		else {
			// If in air an letting go of crouch, make sure we can offset origin to make
			//  up for uncrouching
			NyaVec3Double hullSizeNormal = VEC_HULL_MAX_SCALED() - VEC_HULL_MIN_SCALED();
			NyaVec3Double hullSizeCrouch = VEC_DUCK_HULL_MAX_SCALED() - VEC_DUCK_HULL_MIN_SCALED();
			NyaVec3Double viewDelta = (hullSizeNormal - hullSizeCrouch);
			viewDelta *= -1;
			VectorAdd( newOrigin, viewDelta, newOrigin );
		}

		pmove->m_bDucked = false;
		RemoveFlag(FL_DUCKING);
		pmove->m_bDucking  = false;
		pmove->m_bInDuckJump  = false;
		pmove->view_ofs = GetPlayerViewOffset(false);
		pmove->flDuckTime = 0;

		pmove->origin = newOrigin;

		// Recategorize position since ducking can change origin
		PM_CatagorizePosition();
	}

	bool CanUnDuckJump(pmtrace_t &trace) {
		// Trace down to the stand position and see if we can stand.
		auto vecEnd = pmove->origin;
		vecEnd[UP] -= 36.0f;						// This will have to change if bounding hull change!
		if (IsUsingPlayerTraceFallback()) {
			trace = GetClosestBBoxIntersection(pmove->origin, vecEnd);
		}
		else {
			trace = PM_PlayerTraceDown(pmove->origin, vecEnd);
		}

		if (trace.fraction < 1.0f) {
			// Find the endpoint.
			vecEnd[UP] = pmove->origin[UP] + ( -36.0f * trace.fraction );

			// Test a normal hull.
			pmtrace_t traceUp;
			bool bWasDucked = pmove->m_bDucked;
			pmove->m_bDucked = false;
			if (IsUsingPlayerTraceFallback()) {
				traceUp = GetClosestBBoxIntersection(vecEnd, vecEnd);
			}
			else {
				traceUp = PM_PlayerTrace(vecEnd, vecEnd);
			}
			pmove->m_bDucked = bWasDucked;
			if (!traceUp.startsolid)
				return true;
		}

		return false;
	}
	
	bool CanUnduck() {
		pmtrace_t trace;
		auto newOrigin = pmove->origin;

		if (GetGroundEntity() != NULL) {
			for (int i = 0; i < 3; i++) {
				newOrigin[i] += VEC_DUCK_HULL_MIN_SCALED()[i] - VEC_HULL_MIN_SCALED()[i];
			}
		}
		else {
			// If in air an letting go of crouch, make sure we can offset origin to make
			//  up for uncrouching
			NyaVec3Double hullSizeNormal = VEC_HULL_MAX_SCALED() - VEC_HULL_MIN_SCALED();
			NyaVec3Double hullSizeCrouch = VEC_DUCK_HULL_MAX_SCALED() - VEC_DUCK_HULL_MIN_SCALED();
			NyaVec3Double viewDelta = (hullSizeNormal - hullSizeCrouch);
			viewDelta *= -1;
			VectorAdd(newOrigin, viewDelta, newOrigin);
		}

		bool saveducked = pmove->m_bDucked;
		pmove->m_bDucked = false;
		if (IsUsingPlayerTraceFallback()) {
			trace = GetBottomCeilingForBBox(newOrigin);
			trace.startsolid = trace.ent != -1;
		}
		else {
			trace = PM_PlayerTrace(pmove->origin, newOrigin);
		}
		pmove->m_bDucked = saveducked;
		if (trace.startsolid || (!IsUsingPlayerTraceFallback() && trace.fraction != 1.0f))
			return false;

		return true;
	}

	void Duck() {
		auto mv = pmove;
		int buttonsChanged	= (mv->oldbuttons ^ mv->cmd.buttons);	// These buttons have changed this frame
		int buttonsPressed	=  buttonsChanged & mv->cmd.buttons;	// The changed ones still down are "pressed"
		int buttonsReleased	=  buttonsChanged & mv->oldbuttons;		// The changed ones which were previously down are "released"

		// Check to see if we are in the air.
		bool bInAir = (GetGroundEntity() == NULL);
		bool bInDuck = (pmove->flags & FL_DUCKING) != 0;
		bool bDuckJump = (pmove->m_flJumpTime > 0.0f);
		bool bDuckJumpTime = (pmove->m_flDuckJumpTime > 0.0f);

		if (mv->cmd.buttons & IN_DUCK) {
			mv->oldbuttons |= IN_DUCK;
		}
		else {
			mv->oldbuttons &= ~IN_DUCK;
		}

		// Handle death.
		if (IsDead()) return;

		// Slow down ducked players.
		HandleDuckingSpeedCrop();

		// If the player is holding down the duck button, the player is in duck transition, ducking, or duck-jumping.
		if ((mv->cmd.buttons & IN_DUCK) || mv->m_bDucking || bInDuck || bDuckJump) {
			// DUCK
			if ((mv->cmd.buttons & IN_DUCK) || bDuckJump) {
				// Have the duck button pressed, but the player currently isn't in the duck position.
				if ((buttonsPressed & IN_DUCK) && !bInDuck && !bDuckJump && !bDuckJumpTime) {
					mv->flDuckTime = GAMEMOVEMENT_DUCK_TIME;
					mv->m_bDucking = true;
				}

				// The player is in duck transition and not duck-jumping.
				if (mv->m_bDucking && !bDuckJump && !bDuckJumpTime) {
					float flDuckMilliseconds = std::max(0.0f, GAMEMOVEMENT_DUCK_TIME - mv->flDuckTime);
					float flDuckSeconds = flDuckMilliseconds * 0.001f;

					// Finish in duck transition when transition time is over, in "duck", in air.
					if ((flDuckSeconds > TIME_TO_DUCK) || bInDuck || bInAir) {
						FinishDuck();
					}
					else {
						// Calc parametric time
						float flDuckFraction = SimpleSpline(flDuckSeconds / TIME_TO_DUCK);
						SetDuckedEyeOffset(flDuckFraction);
					}
				}

				if (bDuckJump) {
					// Make the bounding box small immediately.
					if (!bInDuck) {
						StartUnDuckJump();
					}
					else {
						// Check for a crouch override.
						if (!(mv->cmd.buttons & IN_DUCK)) {
							pmtrace_t trace;
							if (CanUnDuckJump(trace)) {
								FinishUnDuckJump(trace);
								mv->m_flDuckJumpTime = (GAMEMOVEMENT_TIME_TO_UNDUCK * (1.0f - trace.fraction)) + GAMEMOVEMENT_TIME_TO_UNDUCK_INV;
							}
						}
					}
				}
			}
			// UNDUCK (or attempt to...)
			else {
				if (mv->m_bInDuckJump) {
					// Check for a crouch override.
					if (!(mv->cmd.buttons & IN_DUCK)) {
						pmtrace_t trace;
						if (CanUnDuckJump(trace)) {
							FinishUnDuckJump(trace);

							if (trace.fraction < 1.0f) {
								mv->m_flDuckJumpTime = (GAMEMOVEMENT_TIME_TO_UNDUCK * (1.0f - trace.fraction)) + GAMEMOVEMENT_TIME_TO_UNDUCK_INV;
							}
						}
					}
					else {
						mv->m_bInDuckJump = false;
					}
				}

				if (bDuckJumpTime) return;

				// Try to unduck unless automovement is not allowed
				// NOTE: When not onground, you can always unduck
				if (mv->m_bAllowAutoMovement || bInAir || mv->m_bDucking) {
					// We released the duck button, we aren't in "duck" and we are not in the air - start unduck transition.
					if ((buttonsReleased & IN_DUCK)) {
						if (bInDuck && !bDuckJump) {
							mv->flDuckTime = GAMEMOVEMENT_DUCK_TIME;
						}
						else if (mv->m_bDucking && !mv->m_bDucked) {
							// Invert time if release before fully ducked!!!
							float unduckMilliseconds = 1000.0f * TIME_TO_UNDUCK;
							float duckMilliseconds = 1000.0f * TIME_TO_DUCK;
							float elapsedMilliseconds = GAMEMOVEMENT_DUCK_TIME - mv->flDuckTime;

							float fracDucked = elapsedMilliseconds / duckMilliseconds;
							float remainingUnduckMilliseconds = fracDucked * unduckMilliseconds;

							mv->flDuckTime = GAMEMOVEMENT_DUCK_TIME - unduckMilliseconds + remainingUnduckMilliseconds;
						}
					}


					// Check to see if we are capable of unducking.
					if (CanUnduck()) {
						// or unducking
						if ((mv->m_bDucking || mv->m_bDucked)) {
							float flDuckMilliseconds = std::max(0.0f, GAMEMOVEMENT_DUCK_TIME - (float)mv->flDuckTime);
							float flDuckSeconds = flDuckMilliseconds * 0.001f;

							// Finish ducking immediately if duck time is over or not on ground
							if (flDuckSeconds > TIME_TO_UNDUCK || (bInAir && !bDuckJump)) {
								FinishUnDuck();
							}
							else {
								// Calc parametric time
								float flDuckFraction = SimpleSpline(1.0f - (flDuckSeconds / TIME_TO_UNDUCK));
								SetDuckedEyeOffset(flDuckFraction);
								mv->m_bDucking = true;
							}
						}
					}
					else {
						// Still under something where we can't unduck, so make sure we reset this timer so
						//  that we'll unduck once we exit the tunnel, etc.
						if (mv->flDuckTime != GAMEMOVEMENT_DUCK_TIME) {
							SetDuckedEyeOffset(1.0f);
							mv->flDuckTime = GAMEMOVEMENT_DUCK_TIME;
							mv->m_bDucked = true;
							mv->m_bDucking = false;
							mv->flags |= FL_DUCKING;
						}
					}
				}
			}
		}
	}
	
	bool CheckJumpButton() {
		if (pmove->dead) {
			pmove->oldbuttons |= IN_JUMP;	// don't jump again until released
			return false;
		}

		// See if we are waterjumping.  If so, decrement count and return.
		if (pmove->waterjumptime) {
			pmove->waterjumptime -= pmove->cmd.msec;
			if (pmove->waterjumptime < 0) {
				pmove->waterjumptime = 0;
			}

			return false;
		}

		// If we are in the water most of the way...
		if (pmove->waterlevel >= 2) {
			// swimming, not jumping
			pmove->onground = -1;

			if(pmove->watertype == CONTENTS_WATER) {    // We move up a certain amount
				pmove->velocity[UP] = 100;
			}
			else if (pmove->watertype == CONTENTS_SLIME) {
				pmove->velocity[UP] = 80;
			}

			// play swiming sound
			if (pmove->flSwimTime <= 0) {
				// Don't play sound again for 1 second
				pmove->flSwimTime = 1000;
				PlaySwimSound();
			}

			return false;
		}

		// No more effect
		if (GetGroundEntity() == NULL) {
			pmove->oldbuttons |= IN_JUMP;
			return false;		// in air, so no effect
		}

		if (!bAutoHop && pmove->oldbuttons & IN_JUMP) return false; // don't pogo stick

		// Cannot jump will in the unduck transition.
		if (pmove->m_bDucking && (pmove->flags & FL_DUCKING)) return false;

		// Still updating the eye position.
		if (pmove->m_flDuckJumpTime > 0.0f) return false;

		// In the air now.
		pmove->onground = -1;

		PM_PreventMegaBunnyJumping();

		PM_PlayStepSound(pmove->chtexturetype, 1.0);

		float flGroundFactor = 1.0f;
		// todo
		//if (player->m_pSurfaceData) {
		//	flGroundFactor = player->m_pSurfaceData->game.jumpFactor;
		//}

		float flMul = std::sqrt(2 * CVar_HL2::sv_gravity * CVar_HL2::GAMEMOVEMENT_JUMP_HEIGHT);

		// Acclerate upward
		// If we are ducking...
		float startz = pmove->velocity[UP];
		if ((pmove->m_bDucking) || (pmove->flags & FL_DUCKING)) {
			pmove->velocity[UP] = flGroundFactor * flMul;  // 2 * gravity * height
		}
		else {
			pmove->velocity[UP] += flGroundFactor * flMul;  // 2 * gravity * height
		}

		if (bABH || bABHMixed) {
			// Add a little forward velocity based on your current forward velocity - if you are not sprinting.
			NyaVec3Double vecForward;
			AngleVectors(pmove->angles, vecForward);
			vecForward[UP] = 0;
			VectorNormalize(vecForward);

			auto vel2D = pmove->velocity;
			vel2D[UP] = 0;
			auto velLength2D = vel2D.length();

			// We give a certain percentage of the current forward movement as a bonus to the jump speed.  That bonus is clipped
			// to not accumulate over time.
			float flSpeedBoostPerc = (!pmove->m_bIsSprinting && !pmove->m_bDucked) ? 0.5f : 0.1f;
			float flSpeedAddition = std::abs(pmove->cmd.forwardmove * flSpeedBoostPerc);
			float flMaxSpeed = pmove->maxspeed + (pmove->maxspeed * flSpeedBoostPerc);
			float flNewSpeed = (flSpeedAddition + velLength2D);

			// If we're over the maximum, we want to only boost as much as will get us to the goal speed
			if (flNewSpeed > flMaxSpeed) {
				flSpeedAddition -= flNewSpeed - flMaxSpeed;
			}

			if (pmove->cmd.forwardmove < 0.0f) {
				flSpeedAddition *= -1.0f;
			}

			// Add it on
			auto endVel = pmove->velocity;
			VectorAdd((vecForward * flSpeedAddition), pmove->velocity, endVel);
			if (!bABHMixed || endVel.length() > pmove->velocity.length()) {
				pmove->velocity = endVel;
			}
		}

		PM_FixupGravityVelocity();

		// todo?
		//mv->m_outJumpVel.z += pmove->velocity[UP] - startz;
		//mv->m_outStepHeight += 0.15f;

		// Set jump time.
		pmove->m_flJumpTime = GAMEMOVEMENT_JUMP_TIME;
		pmove->m_bInDuckJump = true;

		// Flag that we jumped.
		pmove->oldbuttons |= IN_JUMP;	// don't jump again until released
		return true;
	}

	void CheckFalling() {
		// this function really deals with landing, not falling, so early out otherwise
		if (GetGroundEntity() == NULL || pmove->flFallVelocity <= 0)
			return;

		if (!IsDead() && pmove->flFallVelocity >= PLAYER_FALL_PUNCH_THRESHOLD_HL2)
		{
			bool bAlive = true;
			float fvol = 0.5;

			if (pmove->waterlevel > 0) {
				// They landed in water.
			}
			else {
				// Scale it down if we landed on something that's floating...
				//if (player->GetGroundEntity()->IsFloating()) {
				//	pmove->flFallVelocity -= PLAYER_LAND_ON_FLOATING_OBJECT_HL2;
				//}

				//
				// They hit the ground.
				//
				//if (player->GetGroundEntity()->GetAbsVelocity().z < 0.0f) {
				//	// Player landed on a descending object. Subtract the velocity of the ground entity.
				//	pmove->m_flFallVelocity += player->GetGroundEntity()->GetAbsVelocity().z;
				//	pmove->m_flFallVelocity = std::max(0.1f, pmove->flFallVelocity);
				//}

				if (pmove->flFallVelocity > PLAYER_MAX_SAFE_FALL_SPEED_HL2) {
					// subtract off the speed at which a player is allowed to fall without being hurt,
					// so damage will be based on speed beyond that, not the entire fall

					// NOTE: this line introduces a bug with fall punch calculation in original HL2 but is part of CSingleplayRules::FlPlayerFallDamage
					// keeping it here to stay authentic
					pmove->flFallVelocity -= PLAYER_MAX_SAFE_FALL_SPEED_HL2;
					OnTakeFallDamage(pmove->flFallVelocity * DAMAGE_FOR_FALL_SPEED_HL2);
					
					switch (rand() % 2) {
						case 0:
							PlayGameSound("player/pl_fallpain1.wav", 1);
							break;
						case 1:
							PlayGameSound("player/pl_fallpain2.wav", 1);
							break;
					}
					fvol = 1.0;
				}
				else if (pmove->flFallVelocity > PLAYER_MAX_SAFE_FALL_SPEED_HL2 / 2) {
					fvol = 0.85;
				}
				else if (pmove->flFallVelocity < PLAYER_MIN_BOUNCE_SPEED_HL2) {
					fvol = 0;
				}
			}

			PlayerRoughLandingEffects(fvol);
		}

		//
		// Clear the fall velocity so the impact doesn't happen again.
		//
		pmove->flFallVelocity = 0;
	}

	void FullNoClipMove(float factor, float maxacceleration) {
		NyaVec3Double wishvel;
		NyaVec3Double forward, right, up;
		NyaVec3Double wishdir;
		float wishspeed;
		float maxspeed = CVar_HL2::sv_maxspeed * factor;

		AngleVectors(pmove->angles, forward, right, up);  // Determine movement angles

		if (pmove->cmd.buttons & IN_SPEED) {
			factor /= 2.0f;
		}

		// Copy movement amounts
		float fmove = pmove->cmd.forwardmove * factor;
		float smove = pmove->cmd.sidemove * factor;

		VectorNormalize(forward);  // Normalize remainder of vectors
		VectorNormalize(right);    //

		for (int i = 0; i < 3; i++) {       // Determine x and y parts of velocity
			wishvel[i] = forward[i] * fmove + right[i] * smove;
		}
		wishvel[UP] += pmove->cmd.upmove * factor;

		VectorCopy(wishvel, wishdir);   // Determine maginitude of speed of move
		wishspeed = VectorNormalize(wishdir);

		//
		// Clamp to server defined max speed
		//
		if (wishspeed > maxspeed) {
			VectorScale(wishvel, maxspeed/wishspeed, wishvel);
			wishspeed = maxspeed;
		}

		if (maxacceleration > 0.0) {
			// Set pmove velocity
			PM_Accelerate(wishdir, wishspeed, maxacceleration);

			float spd = pmove->velocity.length();
			if (spd < 1.0f) {
				pmove->velocity = {0,0,0};
				return;
			}

			// Bleed off some speed, but if we have less than the bleed
			//  threshhold, bleed the theshold amount.
			float control = (spd < maxspeed/4.0) ? maxspeed/4.0 : spd;

			float friction = CVar_HL2::sv_friction * pmove->friction;

			// Add the amount to the drop amount.
			float drop = control * friction * pmove->frametime;

			// scale the velocity
			float newspeed = spd - drop;
			if (newspeed < 0) {
				newspeed = 0;
			}

			// Determine proportion of old speed we are using.
			newspeed /= spd;
			VectorScale(pmove->velocity, newspeed, pmove->velocity);
		}
		else {
			VectorCopy(wishvel, pmove->velocity);
		}

		// Just move ( don't clip or anything )
		VectorMA(pmove->origin, pmove->frametime, pmove->velocity, pmove->origin);

		// Zero out velocity if in noaccel mode
		if (maxacceleration < 0.0f) {
			pmove->velocity = {0,0,0};
		}
	}

	void PM_AddGravity() {
		float ent_gravity = pmove->gravity ? pmove->gravity : 1.0;

		// Add gravity incorrectly
		pmove->velocity[UP] -= ent_gravity * movevars->gravity * pmove->frametime;
		pmove->velocity[UP] += pmove->basevelocity[UP] * pmove->frametime;
		pmove->basevelocity[UP] = 0;
		PM_CheckVelocity();
	}

	pmtrace_t PM_PushEntity(NyaVec3Double push) {
		pmtrace_t trace;
		NyaVec3Double end;

		VectorAdd(pmove->origin, push, end);

		if (IsUsingPlayerTraceFallback()) {
			trace = GetClosestBBoxIntersection(pmove->origin, end);
		}
		else {
			trace = PM_PlayerTrace(pmove->origin, end);
		}

		VectorCopy(trace.endpos, pmove->origin);

		// todo
		// So we can run impact function afterwards.
		//if (trace.fraction < 1.0 && !trace.allsolid) {
		//	PM_AddToTouched(trace, pmove->velocity);
		//}

		return trace;
	}

	void PM_Physics_Toss() {
		pmtrace_t trace;
		NyaVec3Double move;
		float backoff;

		PM_CheckWater();

		if (pmove->velocity[UP] > 0) {
			pmove->onground = -1;
		}

		// If on ground and not moving, return.
		if (pmove->onground != -1) {
			if (pmove->basevelocity.length() <= 0.0 && pmove->velocity.length() <= 0.0) {
				return;
			}
		}

		PM_CheckVelocity();

		// add gravity
		if (pmove->movetype != MOVETYPE_FLY && pmove->movetype != MOVETYPE_BOUNCEMISSILE && pmove->movetype != MOVETYPE_FLYMISSILE) {
			PM_AddGravity();
		}

		// move origin
		// Base velocity is not properly accounted for since this entity will move again after the bounce without
		// taking it into account
		VectorAdd(pmove->velocity, pmove->basevelocity, pmove->velocity);

		PM_CheckVelocity();
		VectorScale(pmove->velocity, pmove->frametime, move);
		VectorSubtract(pmove->velocity, pmove->basevelocity, pmove->velocity);

		trace = PM_PushEntity(move);	// Should this clear basevelocity

		PM_CheckVelocity();

		if (trace.allsolid) {
			// entity is trapped in another solid
			pmove->onground = trace.ent;
			VectorCopy(vec3_origin, pmove->velocity);
			return;
		}

		if (trace.fraction == 1.0f) {
			PM_CheckWater();
			return;
		}


		if (pmove->movetype == MOVETYPE_BOUNCE) {
			backoff = 2.0 - pmove->friction;
		}
		else if (pmove->movetype == MOVETYPE_BOUNCEMISSILE) {
			backoff = 2.0;
		}
		else {
			backoff = 1;
		}

		PM_ClipVelocity(pmove->velocity, trace.plane.normal, pmove->velocity, backoff);

		// stop if on ground
		if (trace.plane.normal[UP] > 0.7) {
			float vel;
			NyaVec3Double base;

			VectorClear(base);
			if (pmove->velocity[UP] < movevars->gravity * pmove->frametime) {
				// we're rolling on the ground, add static friction.
				pmove->onground = trace.ent;
				pmove->velocity[UP] = 0;
			}

			vel = DotProduct(pmove->velocity, pmove->velocity);

			if (vel < (30 * 30) || (pmove->movetype != MOVETYPE_BOUNCE && pmove->movetype != MOVETYPE_BOUNCEMISSILE)) {
				pmove->onground = trace.ent;
				VectorCopy(vec3_origin, pmove->velocity);
			}
			else {
				VectorScale(pmove->velocity, (1.0 - trace.fraction) * pmove->frametime * 0.9, move);
				trace = PM_PushEntity(move);
			}
			VectorSubtract(pmove->velocity, base, pmove->velocity);
		}

		// check for in water
		PM_CheckWater();
	}

	void PM_PlayerMove(double delta) {
		physent_t *pLadder = nullptr;

		// Adjust speeds etc.
		PM_CheckParamters();

		// Assume we don't touch anything
		//pmove->numtouch = 0;

		// # of msec to apply movement
		pmove->frametime = delta;
		pmove->cmd.msec = pmove->frametime * 1000;

		PM_ReduceTimers();

		// Convert view angles to vectors
		AngleVectors(pmove->angles, pmove->forward, pmove->right, pmove->up);

		// Always try and unstick us unless we are in NOCLIP mode
		/*if (pmove->movetype != MOVETYPE_NOCLIP && pmove->movetype != MOVETYPE_NONE) {
			if (PM_CheckStuck()) {
				// Let the user try to duck to get unstuck
				PM_Duck();

				if (PM_CheckStuck()) {
					return;  // Can't move, we're stuck
				}
			}
		}*/

		// Now that we are "unstuck", see where we are ( waterlevel and type, pmove->onground ).
		PM_CatagorizePosition();

		// Store off the starting water level
		pmove->oldwaterlevel = pmove->waterlevel;

		// If we are not on ground, store off how fast we are moving down
		if (pmove->onground == -1) {
			pmove->flFallVelocity = -pmove->velocity[UP];
		}

		// todo
		//g_onladder = 0;
		//// Don't run ladder code if dead or on a train
		//if (!pmove->dead && !(pmove->flags & FL_ONTRAIN)) {
		//	if (pLadder = PM_Ladder()) {
		//		g_onladder = 1;
		//	}
		//}

		PM_UpdateStepSound();

		if (bHL2Mode) {
			UpdateDuckJumpEyeOffset();
			Duck();
		}
		else {
			PM_Duck();
		}

		// todo
		// Don't run ladder code if dead or on a train
		//if (!pmove->dead && !(pmove->flags & FL_ONTRAIN)) {
		//	if (pLadder) {
		//		PM_LadderMove(pLadder);
		//	}
		//	else if (pmove->movetype != MOVETYPE_WALK && pmove->movetype != MOVETYPE_NOCLIP) {
		//		// Clear ladder stuff unless player is noclipping
		//		//  it will be set immediately again next frame if necessary
		//		pmove->movetype = MOVETYPE_WALK;
		//	}
		//}

		// Handle movement
		switch (pmove->movetype) {
			default:
				break;

			case MOVETYPE_NONE:
				break;

			case MOVETYPE_NOCLIP:
				if (bHL2Mode) {
					FullNoClipMove(CVar_HL2::sv_noclipspeed, CVar_HL2::sv_noclipaccelerate);
				}
				else {
					PM_NoClip();
				}
				break;

			case MOVETYPE_TOSS:
			case MOVETYPE_BOUNCE:
				PM_Physics_Toss();
				break;

			case MOVETYPE_FLY:
				PM_CheckWater();

				// Was jump button pressed?
				// If so, set velocity to 270 away from ladder.  This is currently wrong.
				// Also, set MOVE_TYPE to walk, too.
				if (pmove->cmd.buttons & IN_JUMP) {
					if (!pLadder) {
						PM_Jump();
					}
				} else {
					pmove->oldbuttons &= ~IN_JUMP;
				}

				// Perform the move accounting for any base velocity.
				VectorAdd(pmove->velocity, pmove->basevelocity, pmove->velocity);
				PM_FlyMove();
				VectorSubtract(pmove->velocity, pmove->basevelocity, pmove->velocity);
				break;

			case MOVETYPE_WALK:
				if (!PM_InWater()) {
					PM_AddCorrectGravity();
				}

				// If we are leaping out of the water, just update the counters.
				if (pmove->waterjumptime) {
					PM_WaterJump();
					PM_FlyMove();
					// Make sure waterlevel is set correctly
					PM_CheckWater();
					return;
				}

				// If we are swimming in the water, see if we are nudging against a place we can jump up out
				//  of, and, if so, start out jump.  Otherwise, if we are not moving up, then reset jump timer to 0
				if (pmove->waterlevel >= 2) {
					if (pmove->waterlevel == 2) {
						PM_CheckWaterJump();
					}

					// If we are falling again, then we must not trying to jump out of water any more.
					if (pmove->velocity[UP] < 0 && pmove->waterjumptime) {
						pmove->waterjumptime = 0;
					}

					// Was jump button pressed?
					if (pmove->cmd.buttons & IN_JUMP) {
						PM_Jump();
					} else {
						pmove->oldbuttons &= ~IN_JUMP;
					}

					// Perform regular water movement
					PM_WaterMove();

					VectorSubtract(pmove->velocity, pmove->basevelocity, pmove->velocity);

					// Get a final position
					PM_CatagorizePosition();
				}
				else { // Not underwater
					// Was jump button pressed?
					if (bHL2Mode) {
						if (pmove->cmd.buttons & IN_JUMP) {
							CheckJumpButton();
						}
						else {
							pmove->oldbuttons &= ~IN_JUMP;
						}
					}
					else {
						if (pmove->cmd.buttons & IN_JUMP) {
							if (!pLadder) {
								PM_Jump();
							}
						} else {
							pmove->oldbuttons &= ~IN_JUMP;
						}
					}

					// Fricion is handled before we add in any base velocity. That way, if we are on a conveyor,
					//  we don't slow when standing still, relative to the conveyor.
					if (pmove->onground != -1) {
						pmove->velocity[UP] = 0.0;
						PM_Friction();
					}

					// Make sure velocity is valid.
					PM_CheckVelocity();

					// Are we on ground now
					if (pmove->onground != -1) {
						PM_WalkMove();

						// basic col check to get us unstuck due to point raytrace inaccuracy
						if (IsUsingPlayerTraceFallback()) {
							auto collide = GetClosestBBoxIntersection(pmove->origin, pmove->origin);
							if (collide.ent != -1) {
								pmove->origin = collide.endpos;
							}
						}
					} else {
						PM_AirMove();  // Take into account movement when in air.
					}

					// Set final flags.
					PM_CatagorizePosition();

					// Now pull the base velocity back out.
					// Base velocity is set if you are on a moving object, like
					//  a conveyor (or maybe another monster?)
					VectorSubtract(pmove->velocity, pmove->basevelocity, pmove->velocity);

					// Make sure velocity is valid.
					PM_CheckVelocity();

					// Add any remaining gravitational component.
					if (!PM_InWater()) {
						PM_FixupGravityVelocity();
					}

					// If we are on ground, no downward velocity.
					if (pmove->onground != -1) {
						pmove->velocity[UP] = 0;
					}

					// See if we landed on the ground with enough force to play
					//  a landing sound.
					if (bHL2Mode) {
						CheckFalling();
					}
					else {
						PM_CheckFalling();
					}
				}

				// Did we enter or leave the water?
				if (bHL2Mode) {
					if ((pmove->oldwaterlevel == 0 && pmove->waterlevel != 0) || (pmove->oldwaterlevel != 0 && pmove->waterlevel == 0)) {
						PlaySwimSound();
					}
				}
				else {
					PM_PlayWaterSounds();
				}
				break;
		}
	}

	bool CanSprint() {
		return !(pmove->m_bDucked && !pmove->m_bDucking) && (pmove->waterlevel != 3);
	}

	void SetupMoveParams() {
		movevars->gravity = bHL2Mode ? CVar_HL2::sv_gravity : CVar_HL1::sv_gravity;  			// Gravity for map
		movevars->stopspeed = bHL2Mode ? CVar_HL2::sv_stopspeed : CVar_HL1::sv_stopspeed;			// Deceleration when not moving
		movevars->maxspeed = bHL2Mode ? CVar_HL2::sv_maxspeed : CVar_HL1::sv_maxspeed; 			// Max allowed speed
		movevars->accelerate = bHL2Mode ? CVar_HL2::sv_accelerate : CVar_HL1::sv_accelerate;			// Acceleration factor
		movevars->airaccelerate = bHL2Mode ? CVar_HL2::sv_airaccelerate : CVar_HL1::sv_airaccelerate;		// Same for when in open air
		movevars->wateraccelerate = bHL2Mode ? CVar_HL2::sv_wateraccelerate : CVar_HL1::sv_wateraccelerate;		// Same for when in water
		movevars->friction = bHL2Mode ? CVar_HL2::sv_friction : CVar_HL1::sv_friction;
		movevars->edgefriction = bHL2Mode ? CVar_HL2::sv_edgefriction : CVar_HL1::sv_edgefriction;
		movevars->waterfriction = bHL2Mode ? CVar_HL2::sv_waterfriction : CVar_HL1::sv_waterfriction;		// Less in water
		movevars->bounce = bHL2Mode ? CVar_HL2::sv_bounce : CVar_HL1::sv_bounce;      		// Wall bounce value. 1.0
		movevars->stepsize = bHL2Mode ? CVar_HL2::sv_stepsize : CVar_HL1::sv_stepsize;
		movevars->maxvelocity = bHL2Mode ? CVar_HL2::sv_maxvelocity : CVar_HL1::sv_maxvelocity; 		// maximum server velocity.
		movevars->rollangle = bHL2Mode ? CVar_HL2::sv_rollangle : CVar_HL1::sv_rollangle;
		movevars->rollspeed = bHL2Mode ? CVar_HL2::sv_rollspeed : CVar_HL1::sv_rollspeed;

		// todo train velocity
		pmove->basevelocity = {0,0,0};

		pmove->gravity = 1;
		pmove->friction = 1;
		GetGamePlayerViewAngle(&pmove->cmd.viewangles);
		pmove->clientmaxspeed = movevars->maxspeed;
		if (!bHL2Mode && pmove->movetype == MOVETYPE_NOCLIP) pmove->clientmaxspeed = CVar_HL1::sv_noclipspeed;
		pmove->maxspeed = pmove->clientmaxspeed; // not sure what the difference is here? todo?
		pmove->dead = GetGamePlayerDead();
		pmove->m_bIsSprinting = false;

		pmove->cmd.forwardmove = 0;
		pmove->cmd.sidemove = 0;
		pmove->cmd.upmove = 0;
		pmove->cmd.buttons = 0;

		if (bHL2Mode) {
			pmove->maxspeed = CVar_HL2::HL2_NORM_SPEED;
		}

		float forwardspeed = bHL2Mode ? CVar_HL2::cl_forwardspeed : CVar_HL1::cl_forwardspeed;
		float sidespeed = bHL2Mode ? CVar_HL2::cl_sidespeed : CVar_HL1::cl_sidespeed;
		float upspeed = bHL2Mode ? CVar_HL2::cl_upspeed : CVar_HL1::cl_upspeed;

		static bool bLastSprinting = false;

		if (EXT_GetGameMoveLeftRight) pmove->cmd.sidemove += sidespeed * EXT_GetGameMoveLeftRight();
		if (EXT_GetGameMoveFwdBack) pmove->cmd.forwardmove += forwardspeed * EXT_GetGameMoveFwdBack();
		if (EXT_GetGameMoveUpDown) pmove->cmd.upmove += upspeed * EXT_GetGameMoveUpDown();
		if (EXT_GetGameMoveUse && EXT_GetGameMoveUse()) pmove->cmd.buttons |= IN_USE;
		if (EXT_GetGameMoveJump && EXT_GetGameMoveJump()) pmove->cmd.buttons |= IN_JUMP;
		if (EXT_GetGameMoveDuck && EXT_GetGameMoveDuck()) pmove->cmd.buttons |= IN_DUCK;
		if (EXT_GetGameMoveRun && EXT_GetGameMoveRun()) {
			if (bHL2Mode) {
				if (CanSprint()) {
					pmove->m_bIsSprinting = true;
					pmove->cmd.buttons |= IN_SPEED;
					pmove->maxspeed = CVar_HL2::HL2_SPRINT_SPEED;
				}
				if (!bLastSprinting) {
					PlayGameSound("player/suit_sprint.wav", 1);
				}
			}
			else {
				pmove->cmd.buttons |= IN_RUN;
				pmove->cmd.forwardmove *= CVar_HL1::cl_movespeedkey;
				pmove->cmd.sidemove *= CVar_HL1::cl_movespeedkey;
			}
			bLastSprinting = true;
		}
		else {
			bLastSprinting = false;
		}

		if (pmove->cmd.buttons & IN_SPEED) {
			pmove->oldbuttons |= IN_SPEED;
		}
		else {
			pmove->oldbuttons &= ~IN_SPEED;
		}
	}

	void ApplyMoveParams() {
		auto eye = pmove->origin + pmove->view_ofs;
		eye[UP] += V_CalcBob();

		auto origin = pmove->origin;
		auto velocity = pmove->velocity;
		origin[UP] += GetPlayerCenterUp();
		if (bConvertUnits) {
			for (int i = 0; i < 3; i++) {
				eye[i] = UnitsToMeters(eye[i]);
				origin[i] = UnitsToMeters(origin[i]);
				velocity[i] = UnitsToMeters(velocity[i]);
			}
			eye *= vXYZUnitsMult;
			origin *= vXYZUnitsMult;
			velocity *= vXYZUnitsMult;
		}

		SetGamePlayerPosition(&origin, &velocity);
		SetGamePlayerViewPosition(&eye);
		SetGamePlayerViewAngle(&pmove->angles);
	}

	void SetPlayerBBoxes() {
		if (bHL2Mode) {
			for (int i = 0; i < 4; i++) {
				pmove->player_mins[i][0] = pm_hullmins_hl2[i][0];
				pmove->player_mins[i][FORWARD] = pm_hullmins_hl2[i][1];
				pmove->player_mins[i][UP] = pm_hullmins_hl2[i][2];
				pmove->player_maxs[i][0] = pm_hullmaxs_hl2[i][0];
				pmove->player_maxs[i][FORWARD] = pm_hullmaxs_hl2[i][1];
				pmove->player_maxs[i][UP] = pm_hullmaxs_hl2[i][2];
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				pmove->player_mins[i][0] = pm_hullmins[i][0];
				pmove->player_mins[i][FORWARD] = pm_hullmins[i][1];
				pmove->player_mins[i][UP] = pm_hullmins[i][2];
				pmove->player_maxs[i][0] = pm_hullmaxs[i][0];
				pmove->player_maxs[i][FORWARD] = pm_hullmaxs[i][1];
				pmove->player_maxs[i][UP] = pm_hullmaxs[i][2];
			}
		}
	}

	void Reset() {
		NyaVec3Double gamePlayer, gameVelocity;
		GetGamePlayerPosition(&gamePlayer);
		GetGamePlayerVelocity(&gameVelocity);
		if (bConvertUnits) {
			for (int i = 0; i < 3; i++) {
				gamePlayer[i] = MetersToUnits(gamePlayer[i]);
				gameVelocity[i] = MetersToUnits(gameVelocity[i]);
			}
			gamePlayer *= vXYZUnitsMult;
			gameVelocity *= vXYZUnitsMult;
		}
		pmove->origin = gamePlayer;
		pmove->origin[UP] -= GetPlayerCenterUp();
		pmove->velocity = gameVelocity;

		pmove->angles = {0,0,0};
		pmove->oldangles = {0,0,0};
		pmove->cmd.viewangles = {0,0,0};
		pmove->punchangle = {0,0,0};
		pmove->basevelocity = {0,0,0};
		pmove->movedir = {0,0,0};
		pmove->onground = -1;
		pmove->movetype = nDefaultMoveType;
		pmove->usehull = 0;
		pmove->view_ofs[0] = 0;
		pmove->view_ofs[FORWARD] = 0;
		pmove->view_ofs[UP] = VEC_VIEW();
		pmove->forward = {0,0,0};
		pmove->right = {0,0,0};
		pmove->up = {0,0,0};
		pmove->flDuckTime = 0;
		pmove->bInDuckHL1 = false;
		pmove->flTimeStepSound = 0;
		pmove->iStepLeft = 0;
		pmove->flFallVelocity = 0;
		pmove->flSwimTime = 0;
		pmove->flags = 0;
		pmove->gravity = 1;
		pmove->friction = 1;
		pmove->oldbuttons = 0;
		pmove->waterjumptime = 0;
		pmove->dead = false;
		pmove->onground = -1;
		pmove->waterlevel = 0;
		pmove->oldwaterlevel = 0;
		pmove->watertype = CONTENTS_EMPTY;
		pmove->chtexturetype = CHAR_TEX_CONCRETE;
		pmove->maxspeed = bHL2Mode ? CVar_HL2::sv_maxspeed : CVar_HL1::sv_maxspeed;
		pmove->clientmaxspeed = bHL2Mode ? CVar_HL2::sv_maxspeed : CVar_HL1::sv_maxspeed;
		pmove->cmd.forwardmove = 0;
		pmove->cmd.sidemove = 0;
		pmove->cmd.upmove = 0;
		pmove->cmd.buttons = 0;
		pmove->cmd.msec = 0;

		// hl2 vars
		pmove->m_bDucked = false;
		pmove->m_bDucking = false;
		pmove->m_bInDuckJump = false;
		pmove->m_flDuckJumpTime = 0;
		pmove->m_flJumpTime = 0;
		pmove->m_iSpeedCropped = 0;
		pmove->m_bIsSprinting = false;
		pmove->m_bAllowAutoMovement = true;
		pmove->m_vecPunchAngleVel = {0,0,0};
		SetPlayerBBoxes();
	}

	void ToggleNoclip() {
		if (!bNoclipKey) return;
		pmove->movetype = pmove->movetype == MOVETYPE_NOCLIP ? MOVETYPE_WALK : MOVETYPE_NOCLIP;
	}

	void ResetForHL2Swap() {
		// reset ducking and view offset if hl2 mode was swapped
		pmove->flDuckTime = 0;
		pmove->bInDuckHL1 = false;
		pmove->flags &= ~FL_DUCKING;
		pmove->m_bDucked = false;
		pmove->m_bDucking = false;
		pmove->m_bInDuckJump = false;
		pmove->m_flDuckJumpTime = 0;
		pmove->m_flJumpTime = 0;
		pmove->m_iSpeedCropped = 0;
		pmove->m_bIsSprinting = false;
		pmove->m_bAllowAutoMovement = true;
		pmove->m_vecPunchAngleVel = {0,0,0};
		pmove->view_ofs[0] = 0;
		pmove->view_ofs[FORWARD] = 0;
		pmove->view_ofs[UP] = VEC_VIEW();

		// add origin offset for different bbox center
		pmove->origin[UP] += GetPlayerCenterUp();
		SetPlayerBBoxes();
		pmove->origin[UP] -= GetPlayerCenterUp();
	}

	void Process(double delta) {
		static bool bLastHL2 = bHL2Mode;
		if (bLastHL2 != bHL2Mode) {
			ResetForHL2Swap();
		}
		bLastHL2 = bHL2Mode;

		SetupMoveParams();

		SetPlayerBBoxes();
		pmove->m_iSpeedCropped = SPEED_CROPPED_RESET;

		int numSteps = nPhysicsSteps;
		if (numSteps < 1) numSteps = 1;
		for (int i = 0; i < nPhysicsSteps; i++) {
			PM_PlayerMove(delta / (double)nPhysicsSteps);
		}

		ApplyMoveParams();
	}

	void FillConfig() {
		if (aBehaviorConfig.empty()) {
			AddBoolToCustomConfig(&aBehaviorConfig, "Enabled", "enabled", &bEnabled);
			AddBoolToCustomConfig(&aBehaviorConfig, "Half-Life 2 Mode", "hl2", &bHL2Mode);
			AddBoolToCustomConfig(&aBehaviorConfig, "ABH", "abh", &bABH);
			AddBoolToCustomConfig(&aBehaviorConfig, "Mixed ABH", "abh_mixed", &bABHMixed);
			AddBoolToCustomConfig(&aBehaviorConfig, "Long Jump Module (HL1)", "longjump", &bCanLongJump);
			AddBoolToCustomConfig(&aBehaviorConfig, "Bhop Speed Cap", "bhop_cap", &bBhopCap);
			AddBoolToCustomConfig(&aBehaviorConfig, "Better sv_maxvelocity", "better_maxvelocity", &bSmartVelocityCap);
			AddBoolToCustomConfig(&aBehaviorConfig, "Noclip Key", "noclip_toggle", &bNoclipKey);
		}
		if (aAdvancedConfig.empty()) {
			AddIntToCustomConfig(&aAdvancedConfig, "Collision Density", "collision_density", &nColDensity);
			AddIntToCustomConfig(&aAdvancedConfig, "Physics Steps", "physics_steps", &nPhysicsSteps);
		}
		if (aCVarConfigHL1.empty()) {
			AddFloatToCustomConfig(&aCVarConfigHL1, "cl_bob", "cl_bob", &CVar_HL1::cl_bob);
			AddFloatToCustomConfig(&aCVarConfigHL1, "cl_bobcycle", "cl_bobcycle", &CVar_HL1::cl_bobcycle);
			AddFloatToCustomConfig(&aCVarConfigHL1, "cl_bobup", "cl_bobup", &CVar_HL1::cl_bobup);
			AddFloatToCustomConfig(&aCVarConfigHL1, "cl_forwardspeed", "cl_forwardspeed", &CVar_HL1::cl_forwardspeed);
			AddFloatToCustomConfig(&aCVarConfigHL1, "cl_sidespeed", "cl_sidespeed", &CVar_HL1::cl_sidespeed);
			AddFloatToCustomConfig(&aCVarConfigHL1, "cl_upspeed", "cl_upspeed", &CVar_HL1::cl_upspeed);
			AddFloatToCustomConfig(&aCVarConfigHL1, "cl_movespeedkey", "cl_movespeedkey", &CVar_HL1::cl_movespeedkey);
			AddFloatToCustomConfig(&aCVarConfigHL1, "sv_gravity", "sv_gravity", &CVar_HL1::sv_gravity);
			AddFloatToCustomConfig(&aCVarConfigHL1, "sv_stopspeed", "sv_stopspeed", &CVar_HL1::sv_stopspeed);
			AddFloatToCustomConfig(&aCVarConfigHL1, "sv_maxspeed", "sv_maxspeed", &CVar_HL1::sv_maxspeed);
			AddFloatToCustomConfig(&aCVarConfigHL1, "sv_noclipspeed", "sv_noclipspeed", &CVar_HL1::sv_noclipspeed);
			AddFloatToCustomConfig(&aCVarConfigHL1, "sv_accelerate", "sv_accelerate", &CVar_HL1::sv_accelerate);
			AddFloatToCustomConfig(&aCVarConfigHL1, "sv_airaccelerate", "sv_airaccelerate", &CVar_HL1::sv_airaccelerate);
			AddFloatToCustomConfig(&aCVarConfigHL1, "sv_wateraccelerate", "sv_wateraccelerate", &CVar_HL1::sv_wateraccelerate);
			AddFloatToCustomConfig(&aCVarConfigHL1, "sv_friction", "sv_friction", &CVar_HL1::sv_friction);
			AddFloatToCustomConfig(&aCVarConfigHL1, "sv_edgefriction", "sv_edgefriction", &CVar_HL1::sv_edgefriction);
			AddFloatToCustomConfig(&aCVarConfigHL1, "sv_waterfriction", "sv_waterfriction", &CVar_HL1::sv_waterfriction);
			//AddFloatToCustomConfig(&aCVarConfigHL1, "sv_entgravity", "sv_entgravity", &CVar_HL1::sv_entgravity);
			AddFloatToCustomConfig(&aCVarConfigHL1, "sv_bounce", "sv_bounce", &CVar_HL1::sv_bounce);
			AddFloatToCustomConfig(&aCVarConfigHL1, "sv_stepsize", "sv_stepsize", &CVar_HL1::sv_stepsize);
			AddFloatToCustomConfig(&aCVarConfigHL1, "sv_maxvelocity", "sv_maxvelocity", &CVar_HL1::sv_maxvelocity);
			//AddBoolToCustomConfig(&aCVarConfigHL1, "mp_footsteps", "mp_footsteps", &CVar_HL1::mp_footsteps);
			AddFloatToCustomConfig(&aCVarConfigHL1, "sv_rollangle", "sv_rollangle", &CVar_HL1::sv_rollangle);
			AddFloatToCustomConfig(&aCVarConfigHL1, "sv_rollspeed", "sv_rollspeed", &CVar_HL1::sv_rollspeed);
		}
		if (aCVarConfigHL2.empty()) {
			AddFloatToCustomConfig(&aCVarConfigHL2, "cl_bob", "cl_bob", &CVar_HL2::cl_bob);
			AddFloatToCustomConfig(&aCVarConfigHL2, "cl_bobcycle", "cl_bobcycle", &CVar_HL2::cl_bobcycle);
			AddFloatToCustomConfig(&aCVarConfigHL2, "cl_bobup", "cl_bobup", &CVar_HL2::cl_bobup);
			AddFloatToCustomConfig(&aCVarConfigHL2, "cl_forwardspeed", "cl_forwardspeed", &CVar_HL2::cl_forwardspeed);
			AddFloatToCustomConfig(&aCVarConfigHL2, "cl_sidespeed", "cl_sidespeed", &CVar_HL2::cl_sidespeed);
			AddFloatToCustomConfig(&aCVarConfigHL2, "cl_upspeed", "cl_upspeed", &CVar_HL2::cl_upspeed);
			//AddFloatToCustomConfig(&aCVarConfigHL2, "cl_movespeedkey", "cl_movespeedkey", &CVar_HL2::cl_movespeedkey);
			AddFloatToCustomConfig(&aCVarConfigHL2, "sv_gravity", "sv_gravity", &CVar_HL2::sv_gravity);
			AddFloatToCustomConfig(&aCVarConfigHL2, "sv_stopspeed", "sv_stopspeed", &CVar_HL2::sv_stopspeed);
			AddFloatToCustomConfig(&aCVarConfigHL2, "sv_maxspeed", "sv_maxspeed", &CVar_HL2::sv_maxspeed);
			AddFloatToCustomConfig(&aCVarConfigHL2, "sv_noclipspeed", "sv_noclipspeed", &CVar_HL2::sv_noclipspeed);
			AddFloatToCustomConfig(&aCVarConfigHL2, "sv_noclipaccelerate", "sv_noclipaccelerate", &CVar_HL2::sv_noclipaccelerate);
			AddFloatToCustomConfig(&aCVarConfigHL2, "sv_accelerate", "sv_accelerate", &CVar_HL2::sv_accelerate);
			AddFloatToCustomConfig(&aCVarConfigHL2, "sv_airaccelerate", "sv_airaccelerate", &CVar_HL2::sv_airaccelerate);
			AddFloatToCustomConfig(&aCVarConfigHL2, "sv_wateraccelerate", "sv_wateraccelerate", &CVar_HL2::sv_wateraccelerate);
			AddFloatToCustomConfig(&aCVarConfigHL2, "sv_friction", "sv_friction", &CVar_HL2::sv_friction);
			AddFloatToCustomConfig(&aCVarConfigHL2, "sv_edgefriction", "sv_edgefriction", &CVar_HL2::sv_edgefriction);
			AddFloatToCustomConfig(&aCVarConfigHL2, "sv_waterfriction", "sv_waterfriction", &CVar_HL2::sv_waterfriction);
			//AddFloatToCustomConfig(&aCVarConfigHL2, "sv_entgravity", "sv_entgravity", &CVar_HL2::sv_entgravity);
			AddFloatToCustomConfig(&aCVarConfigHL2, "sv_bounce", "sv_bounce", &CVar_HL2::sv_bounce);
			AddFloatToCustomConfig(&aCVarConfigHL2, "sv_stepsize", "sv_stepsize", &CVar_HL2::sv_stepsize);
			AddFloatToCustomConfig(&aCVarConfigHL2, "sv_maxvelocity", "sv_maxvelocity", &CVar_HL2::sv_maxvelocity);
			//AddBoolToCustomConfig(&aCVarConfigHL2, "mp_footsteps", "mp_footsteps", &CVar_HL2::mp_footsteps);
			AddFloatToCustomConfig(&aCVarConfigHL2, "sv_rollangle", "sv_rollangle", &CVar_HL2::sv_rollangle);
			AddFloatToCustomConfig(&aCVarConfigHL2, "sv_rollspeed", "sv_rollspeed", &CVar_HL2::sv_rollspeed);
			AddFloatToCustomConfig(&aCVarConfigHL2, "HL2_NORM_SPEED", "HL2_NORM_SPEED", &CVar_HL2::HL2_NORM_SPEED);
			AddFloatToCustomConfig(&aCVarConfigHL2, "HL2_SPRINT_SPEED", "HL2_SPRINT_SPEED", &CVar_HL2::HL2_SPRINT_SPEED);
			AddFloatToCustomConfig(&aCVarConfigHL2, "GAMEMOVEMENT_JUMP_HEIGHT", "GAMEMOVEMENT_JUMP_HEIGHT", &CVar_HL2::GAMEMOVEMENT_JUMP_HEIGHT);
		}
	}

#ifdef FREEMANAPI_FOUC_MENULIB
	void ProcessMenu() {
		if (aBehaviorConfig.empty()) FillConfig();

		if (DrawMenuOption(std::format("Active - {}", bEnabled), "")) {
			bEnabled = !bEnabled;
			if (bEnabled) Reset();
		}

		if (DrawMenuOption("Behavior", "Adjust the movement physics")) {
			ChloeMenuLib::BeginMenu();

			for (auto& value : aBehaviorConfig) {
				value.DrawValueEditor();
			}
			for (auto& value : aCustomBehaviorConfig) {
				value.DrawValueEditor();
			}

			if (DrawMenuOption("Advanced")) {
				ChloeMenuLib::BeginMenu();
				for (auto& value : aAdvancedConfig) {
					value.DrawValueEditor();
				}
				for (auto& value : aCustomAdvancedConfig) {
					value.DrawValueEditor();
				}
				ChloeMenuLib::EndMenu();
			}

			ChloeMenuLib::EndMenu();
		}
		if (DrawMenuOption("Parameters", "Adjust console variables")) {
			ChloeMenuLib::BeginMenu();

			if (DrawMenuOption("HL1 Movement", "Adjust HL1 console variables")) {
				ChloeMenuLib::BeginMenu();
				for (auto& value : aCVarConfigHL1) {
					value.DrawValueEditor();
				}
				ChloeMenuLib::EndMenu();
			}
			if (DrawMenuOption("HL2 Movement", "Adjust HL2 console variables")) {
				ChloeMenuLib::BeginMenu();
				for (auto& value : aCVarConfigHL2) {
					value.DrawValueEditor();
				}
				ChloeMenuLib::EndMenu();
			}

			for (auto& value : aCustomCVarConfig) {
				value.DrawValueEditor();
			}

			ChloeMenuLib::EndMenu();
		}

		if (DrawMenuOption("Debug Info")) {
			ChloeMenuLib::BeginMenu();
			DrawMenuOption(std::format("Velocity - {:.2f} {:.2f} {:.2f}", pmove->velocity[0], pmove->velocity[1], pmove->velocity[2]));
			DrawMenuOption(std::format("Punch Angle - {:.2f} {:.2f} {:.2f}", pmove->punchangle[0], pmove->punchangle[1], pmove->punchangle[2]));
			DrawMenuOption(std::format("View Angle - {:.2f} {:.2f} {:.2f}", pmove->angles[0], pmove->angles[1], pmove->angles[2]));
			DrawMenuOption(std::format("Last Plane Normal - {:.2f}", fLastPlaneNormal));
			DrawMenuOption(std::format("On Ground - {}", pmove->onground));
			DrawMenuOption(lastConsoleMsg);
			ChloeMenuLib::EndMenu();
		}
	}
#endif

	std::string sConfigName = "FreemanAPI_gcp.toml";

	void LoadConfig() {
		if (aBehaviorConfig.empty()) FillConfig();

		auto config = toml::parse_file(sConfigName);
		for (auto& value : aBehaviorConfig) {
			value.ReadFromConfig(config, "main");
		}
		for (auto& value : aCustomBehaviorConfig) {
			value.ReadFromConfig(config, "main");
		}
		for (auto& value : aAdvancedConfig) {
			value.ReadFromConfig(config, "advanced");
		}
		for (auto& value : aCustomAdvancedConfig) {
			value.ReadFromConfig(config, "advanced");
		}
		for (auto& value : aCVarConfigHL1) {
			value.ReadFromConfig(config, "hl1");
		}
		for (auto& value : aCVarConfigHL2) {
			value.ReadFromConfig(config, "hl2");
		}
		for (auto& value : aCustomCVarConfig) {
			value.ReadFromConfig(config, "cvars");
		}
	}
}