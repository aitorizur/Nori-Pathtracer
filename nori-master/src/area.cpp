/*
    File added for exercise P2
*/

#include <nori/area.h>

NORI_NAMESPACE_BEGIN

Color3f AreaEmitter::sample(EmitterQueryRecord& lRec, const Point2f& sample) const
{
    return eval(lRec) / pdf(lRec);
}

NORI_REGISTER_CLASS(AreaEmitter, "area");
NORI_NAMESPACE_END