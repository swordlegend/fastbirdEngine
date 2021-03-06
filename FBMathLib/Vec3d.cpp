/*
 -----------------------------------------------------------------------------
 This source file is part of fastbird engine
 For the latest info, see http://www.jungwan.net/
 
 Copyright (c) 2013-2015 Jungwan Byun
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 -----------------------------------------------------------------------------
*/

#include "stdafx.h"
#include "Vec3d.h"
#include "Vec3.h"
using namespace fb;

Vec3d::Vec3d()
{

}

Vec3d::Vec3d(double x, double y, double z) 
	: x(x)
	, y(y)
	, z(z)
{

}
Vec3d::Vec3d(const Vec3& other) 
	: x(other.x)
	, y(other.y)
	, z(other.z)
{

}

Vec3d Vec3d::NormalizeCopy() const
{
	Vec3d result = *this;
	result.Normalize();
	return result;
}

double Vec3d::Normalize() {
	double length = sqrt(x*x + y*y + z*z);
	if (length > 0.0)
	{
		double invLength = 1.0 / length;
		x *= invLength;
		y *= invLength;
		z *= invLength;
	}

	return length;
}

Vec3d Vec3d::operator-(const Vec3d& other) const {
	return Vec3d{ x - other.x, y - other.y, z - other.z };
}