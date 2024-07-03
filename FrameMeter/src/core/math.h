#pragma once

#include <compare>

struct Vec2
{
	float x = 0.f;
	float y = 0.f;

	Vec2 &operator+=(const Vec2 &rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	friend Vec2 operator+(Vec2 lhs, const Vec2 &rhs)
	{
		lhs += rhs;
		return lhs;
	}

	Vec2 &operator-=(const Vec2 &rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	friend Vec2 operator-(Vec2 lhs, const Vec2 &rhs)
	{
		lhs -= rhs;
		return lhs;
	}
};

struct Vec3
{
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

	Vec3 &operator+=(const Vec3 &rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	friend Vec3 operator+(Vec3 lhs, const Vec3 &rhs)
	{
		lhs += rhs;
		return lhs;
	}

	Vec3 &operator-=(const Vec3 &rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	friend Vec3 operator-(Vec3 lhs, const Vec3 &rhs)
	{
		lhs -= rhs;
		return lhs;
	}

	Vec3 &operator/=(float rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}

	friend Vec3 operator/(Vec3 lhs, float rhs)
	{
		lhs /= rhs;
		return lhs;
	}

	auto operator<=>(const Vec3 &) const = default;
};

struct Camera
{
	Vec3 position = {};
	float yaw = 0.f;
	float pitch = 0.f;
	float roll = 0.f;
	float horizontal_fov = 90.f;
	float near_plane = 1.f;
	float far_plane = 1000.f;
	float aspect_ratio = 1.f;

	Vec2 project(Vec3 point) const;
};
