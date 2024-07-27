#pragma once

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
    class AreaEmitter : public Emitter 
    {
    private:
        Color3f m_radiance;

    public:
        Mesh* m_mesh;

        AreaEmitter(const PropertyList& propList) : m_mesh(NULL) {
            /* Emitted radiance */
            m_radiance = propList.getColor("radiance");
        }

        Color3f sample(EmitterQueryRecord& lRec, const Point2f& sample) const;

        float pdf(const EmitterQueryRecord& lRec) const 
        {
            //AQUI FALTA LA CONVERSION A ANGULO SOLIDO POR LO QUE HACE FALTA EL PUNTO Y LA DIRECCION

            //Frame::cosTheta(lRec.wi);

            return 1 / GetEmitterArea();
        }

        Color3f eval(const EmitterQueryRecord& lRec) const
        {
            return Frame::cosTheta(lRec.wi) > 0 ? m_radiance : Color3f(0.0f);
        }

        float GetEmitterArea() const
        {
            return m_mesh->totalSurfaceArea();
        }

        void setParent(NoriObject* object) {
            if (object->getClassType() != EMesh)
                throw NoriException("AreaEmitter: attached to a non-mesh object!");
            m_mesh = static_cast<Mesh*>(object);
        }

        std::string toString() const {
            return tfm::format("AreaEmitter[radiance=%s]", m_radiance.toString());
        }
};

NORI_NAMESPACE_END
