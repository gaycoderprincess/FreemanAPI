// HL math and unit conversions
namespace HLMovement {
	inline double UnitsToMeters(double f) {
		return f * 0.0254;
	}

	inline double MetersToUnits(double f) {
		return f / 0.0254;
	}

	inline double DotProduct(const NyaVec3Double& x, const NyaVec3Double& y) {
		return (x[0]*y[0]+x[1]*y[1]+x[2]*y[2]);
	}
	inline void CrossProduct(const NyaVec3Double v1, const NyaVec3Double v2, NyaVec3Double cross) {
		cross[0] = v1[1]*v2[2] - v1[2]*v2[1];
		cross[1] = v1[2]*v2[0] - v1[0]*v2[2];
		cross[2] = v1[0]*v2[1] - v1[1]*v2[0];
	}
	inline void VectorSubtract(const NyaVec3Double& a, const NyaVec3Double& b, NyaVec3Double& c) {
		c[0]=a[0]-b[0];
		c[1]=a[1]-b[1];
		c[2]=a[2]-b[2];
	}
	inline void VectorAdd(const NyaVec3Double& a, const NyaVec3Double& b, NyaVec3Double& c) {
		c[0]=a[0]+b[0];
		c[1]=a[1]+b[1];
		c[2]=a[2]+b[2];
	}
	inline void VectorCopy(const NyaVec3Double& a, NyaVec3Double& b) {
		b[0]=a[0];
		b[1]=a[1];
		b[2]=a[2];
	}
	inline void VectorScale(const NyaVec3Double& a, float b, NyaVec3Double& c) {
		c[0]=b*a[0];
		c[1]=b*a[1];
		c[2]=b*a[2];
	}
	inline float VectorNormalize(NyaVec3Double& v) {
		float length, ilength;

		length = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
		if (length) {
			ilength = 1/length;
			v *= ilength;
		}
		return length;
	}
	inline void VectorClear(NyaVec3Double& a) {
		a[0]=0.0;
		a[1]=0.0;
		a[2]=0.0;
	}
	inline void VectorMA(const NyaVec3Double &start, float scale, const NyaVec3Double &direction, NyaVec3Double &dest) {
		dest.x = start.x + scale * direction.x;
		dest.y = start.y + scale * direction.y;
		dest.z = start.z + scale * direction.z;
	}

	void AngleVectors(const NyaVec3Double& angles, NyaVec3Double& fwd, NyaVec3Double& right, NyaVec3Double& up) {
		auto anglesRad = angles * (std::numbers::pi / 180.0);
		anglesRad[PITCH] *= -1;

		auto mat = NyaMat4x4();
		mat.Rotate(NyaVec3(anglesRad[1], anglesRad[2], anglesRad[0]));

		// this is disgusting.
		fwd.x = (*(NyaVec3*)&mat[FORWARD*4]).x;
		fwd.y = (*(NyaVec3*)&mat[FORWARD*4]).y;
		fwd.z = (*(NyaVec3*)&mat[FORWARD*4]).z;
		right.x = mat.x.x;
		right.y = mat.x.y;
		right.z = mat.x.z;
		up.x = (*(NyaVec3*)&mat[UP*4]).x;
		up.y = (*(NyaVec3*)&mat[UP*4]).y;
		up.z = (*(NyaVec3*)&mat[UP*4]).z;
	}

	NyaVec3Double vec3_origin = {0,0,0};
}