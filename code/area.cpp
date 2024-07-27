/*
    File added for exercise P2
*/

#include <nori/emitter.h>
#include <nori/mesh.h>
#include <nori/dpdf.h>
#include <nori/warp.h>

NORI_NAMESPACE_BEGIN

/**
 * \brief Simple area emitter with uniform emittance
 */
    class AreaEmitter : public Emitter {
    public:
        AreaEmitter(const PropertyList& propList) : m_mesh(NULL) {
            /* Emitted radiance */
            m_radiance = propList.getColor("radiance");
        }

        Color3f sample(EmitterQueryRecord& lRec, const Point2f& sample) const 
        {
            float trianglePointProbability;
            lRec.sampledPoint = m_mesh->samplePoint(sample, trianglePointProbability);
            lRec.emitterProbability *= trianglePointProbability;
            lRec.intersectionNormal = Vector3f(0.0f, -1.0f, 0.0f);
            lRec.wi = (lRec.startingPoint - lRec.sampledPoint).normalized();

            return eval(lRec);
        }

        float pdf(const EmitterQueryRecord& lRec) const {
            return 0.f;
        }

        Color3f eval(const EmitterQueryRecord& lRec) const 
        {
            return lRec.intersectionNormal.dot(lRec.wi) > 0 ? m_radiance : Color3f(0.0f);
        }

        void setParent(NoriObject* object) {
            if (object->getClassType() != EMesh)
                throw NoriException("AreaEmitter: attached to a non-mesh object!");
            m_mesh = static_cast<Mesh*>(object);
        }

        std::string toString() const {
            return tfm::format("AreaEmitter[radiance=%s]", m_radiance.toString());
        }
    private:
        Color3f m_radiance;
        Mesh* m_mesh;
};

NORI_REGISTER_CLASS(AreaEmitter, "area");
NORI_NAMESPACE_END