/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob

    Nori is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Nori is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <nori/warp.h>
#include <nori/vector.h>
#include <nori/frame.h>

NORI_NAMESPACE_BEGIN

Point2f Warp::squareToUniformSquare(const Point2f &sample) 
{
    return sample;
}

float Warp::squareToUniformSquarePdf(const Point2f &sample) 
{
    return ((sample.array() >= 0).all() && (sample.array() <= 1).all()) ? 1.0f : 0.0f;
}

Point2f Warp::squareToTent(const Point2f &sample) 
{
    float sqrtX = sqrt(sample.x());
    return Point2f(1 - sqrtX, sample.y() * sqrtX);
}

float triangleArea(const Point2f a, const Point2f b, const Point2f c)
{
    return abs((a.x() * (b.y() - c.y()) + b.x() * (c.y() - a.y()) + c.x() * (a.y() - b.y())) / 2.0f);;
}

bool CompareFloat(float x, float y, float epsilon)
{
    return fabs(x - y) < epsilon;
}

bool isInside(const Point2f a, const Point2f b, const Point2f c, const Point2f &p)
{
    float abcArea = triangleArea(a, b, c);

    float pabArea = triangleArea(p, a, b);
    float pacArea = triangleArea(p, a, c);
    float pbcArea = triangleArea(p, b, c);

    return CompareFloat(pabArea + pacArea + pbcArea, abcArea, 0.0001f);
}

float Warp::squareToTentPdf(const Point2f &p) 
{
    float triangleArea = 0.5f;
    float probabilty = 1 / triangleArea;

    return isInside(Point2f(0, 0), Point2f(0, 1), Point2f(1, 0), p) ? probabilty : 0.0f;
}

Point2f Warp::squareToUniformDisk(const Point2f &sample) {
    throw NoriException("Warp::squareToUniformDisk() is not yet implemented!");
}

float Warp::squareToUniformDiskPdf(const Point2f &p) {
    throw NoriException("Warp::squareToUniformDiskPdf() is not yet implemented!");
}

Vector3f Warp::squareToUniformSphere(const Point2f &sample) 
{
    float x = cos(2 * M_PI * sample.y()) * 2 * sqrt(1 - pow(sample.x(), 2.0f));
    float y = sin(2 * M_PI * sample.y()) * 2 * sqrt(1 - pow(sample.x(), 2.0f));
    float z = 1 - 2 * sample.x();

    return Vector3f(x, y, z);
}

float Warp::squareToUniformSpherePdf(const Vector3f &v) 
{
    return INV_FOURPI;
}

Vector3f Warp::squareToUniformHemisphere(const Point2f &sample) 
{
    float x = cos(2 * M_PI * sample.y()) * sqrt(1 - pow(sample.x(), 2.0f));
    float y = sin(2 * M_PI * sample.y()) * sqrt(1 - pow(sample.x(), 2.0f));
    float z = sample.x();

    return Vector3f(x, y, z);
}

float Warp::squareToUniformHemispherePdf(const Vector3f &v) 
{
    return v.z() >= 0 ? INV_TWOPI : 0.0f;
}

Vector3f Warp::squareToCosineHemisphere(const Point2f &sample) 
{
    float teta = acos(sqrt(-sample.x() + 1));
    float fi = 2 * M_PI * sample.y();;

    float x = sin(teta) * cos(fi);
    float y = sin(teta) * sin(fi);
    float z = cos(teta);

    return Vector3f(x, y, z);
}

float Warp::squareToCosineHemispherePdf(const Vector3f &v) 
{
    return v.z() >= 0 ? v.z() * INV_PI : 0.0f;
}

Vector3f Warp::squareToBeckmann(const Point2f &sample, float alpha) {
    throw NoriException("Warp::squareToBeckmann() is not yet implemented!");
}

float Warp::squareToBeckmannPdf(const Vector3f &m, float alpha) {
    throw NoriException("Warp::squareToBeckmannPdf() is not yet implemented!");
}

NORI_NAMESPACE_END
