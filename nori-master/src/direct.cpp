/*
    File added for exercise P2
*/

#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/sampler.h>
#include <nori/bsdf.h>
#include <nori/emitter.h>
#include <nori/vector.h>

NORI_NAMESPACE_BEGIN

/**
 * \brief Direct illumination: integrator that implements material sampling, emitter light sampling,
 and some basic multiple importance sampling.
 */
    class DirectIllumination : public Integrator {
    enum class SAMPLING_MODE { MATERIAL, LIGHT, MIS };

    public:
        DirectIllumination(const PropertyList& propList) {
            /* Sampling mode */
            std::string sampling = propList.getString("sampling", "material");
            if (sampling == std::string("MIS"))
            {
                m_sampling = SAMPLING_MODE::MIS;
            }
            else if (sampling == std::string("light"))
            {
                m_sampling = SAMPLING_MODE::LIGHT;
            }
            else
            {
                m_sampling = SAMPLING_MODE::MATERIAL;
            }
        }

        Color3f Li(const Scene* scene, Sampler* sampler, const Ray3f& ray) const {
            /* Find the surface that is visible in the requested direction */

            Intersection its;

            if (!scene->rayIntersect(ray, its))
            {
                return Color3f(0.0f);
            }
            else if (its.mesh->isEmitter())
            {
                Vector3f inverseRay = -ray.d;
                const nori::Emitter* emitterBSDF = its.mesh->getEmitter();
                EmitterQueryRecord emiterRecord(inverseRay, its.shFrame.n);

                return emitterBSDF->eval(emiterRecord);
            }
            else
            {
                Vector3f inverseRay = -ray.d;

                EmitterQueryRecord emitterRecord(inverseRay, its.shFrame.n);
                scene->sampleEmitter(emitterRecord, *sampler);

                Vector3f lightDirection = emitterRecord.sampledPoint - its.p;
                Ray3f lightSampleRay(its.p, lightDirection);
                Intersection lightIts;

                if (scene->rayIntersect(lightSampleRay, lightIts))
                {
                    if (lightIts.mesh->getEmitter() == emitterRecord.emitter)
                    {
                        Color3f emitterSample = emitterRecord.emitter->sample(emitterRecord, sampler->next2D());

                        auto itsNormal = -lightIts.shFrame.n;
                        float dotProduct = lightDirection.dot(itsNormal);
                        float magnitudeProduct = lightDirection.norm() * itsNormal.norm();
                        emitterRecord.emitterProbability *= (pow(lightDirection.norm(), 2) / (dotProduct / magnitudeProduct));
                        emitterSample = emitterSample / emitterRecord.emitterProbability;

                        const nori::BSDF* materialBSDF = its.mesh->getBSDF();
                        BSDFQueryRecord queryRecord(inverseRay);
                        Color3f matSample = materialBSDF->eval(queryRecord);

                        return matSample * its.shFrame.n.dot(lightDirection) * emitterSample;
                    }
                }
            }
            
            return Color3f(0.0f);
        }

        std::string toString() const {
            return tfm::format(
                "DirectIllumination[\n"
                "  method = %s,\n"
                "]",
                m_sampling == SAMPLING_MODE::MATERIAL ? "material" :
                (m_sampling == SAMPLING_MODE::LIGHT ? "light" : "MIS")
            );
        }
    private:
        SAMPLING_MODE m_sampling;
};

NORI_REGISTER_CLASS(DirectIllumination, "direct");
NORI_NAMESPACE_END