#include <math.h>

#include "core/math.h"

Vec2 Camera::project(Vec3 point) const
{
	const float deg_to_rad = acosf(-1.0f) / 180.f;

	const float cx = cosf(yaw * deg_to_rad);
	const float cy = cosf(pitch * deg_to_rad);
	const float cz = cosf(roll * deg_to_rad);
	const float sx = sinf(yaw * deg_to_rad);
	const float sy = sinf(pitch * deg_to_rad);
	const float sz = sinf(roll * deg_to_rad);

	// Convert from world space to camera space (x is towards scene, y is right, z is up)
	const Vec3 delta = point - position;
	const Vec3 cs = {
		.x = delta.x * (cy * cx) + delta.y * (cy * sx) + delta.z * (-sy),
		.y = delta.x * (sy * sz * cx - sx * cz) + delta.y * (sx * sy * sz + cx * cz) + delta.z * (sz * cy),
		.z = delta.x * (sy * cz * cx + sx * sz) + delta.y * (sx * sy * cz - cx * sy) + delta.z * (cz * cy),
	};

	// Convert from camera space to view space (x is right, y is down, z is towards scene)
	const Vec3 view = {
		.x = cs.y,
		.y = -cs.z,
		.z = cs.x,
	};

	// Perspective projection matrix
	const float tangent = tanf(horizontal_fov / 2.f * deg_to_rad);

	const float m00 = 1.f / tangent;
	const float m01 = 0.f;
	const float m02 = 0.f;
	const float m03 = 0.f;

	const float m10 = 0.f;
	const float m11 = aspect_ratio / tangent;
	const float m12 = 0.f;
	const float m13 = 0.f;

	const float m20 = 0.f;
	const float m21 = 0.f;
	const float m22 = far_plane / (far_plane - near_plane);
	const float m23 = 1.f;

	const float m30 = 0.f;
	const float m31 = 0.f;
	const float m32 = -(far_plane * near_plane) / (far_plane - near_plane);
	const float m33 = 0.f;

	// Project to clip space (same axis as view space but frustum mapped to [-1, 1])
	Vec3 clip = {
		.x = view.x * m00 + view.y * m10 + view.z * m20 + m30,
		.y = view.x * m01 + view.y * m11 + view.z * m21 + m31,
		.z = view.x * m02 + view.y * m12 + view.z * m22 + m32,
	};
	const float w = view.x * m03 + view.y * m13 + view.z * m23 + m33;
	clip /= w;

	// Map screen to [0, 1] x [0, 1] with top-left at (0, 0)
	return {
		.x = (clip.x + 1.f) / 2.f,
		.y = (clip.y + 1.f) / 2.f,
	};
}