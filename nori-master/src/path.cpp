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
    class PathTracing : public Integrator {
    enum class SAMPLING_MODE { MATERIAL, LIGHT, MIS };

    public:
        PathTracing(const PropertyList& propList) {
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

        struct PathInfo 
        {
        public:
            unsigned int depth;
            Color3f pathThroughput;


            /// Return a human-readable string summary
            std::string toString() const {
                return tfm::format("[%d, %f, %f, %f]", depth, pathThroughput[0],
                    pathThroughput[1], pathThroughput[2]);
            }
        };

        Color3f LiRecursive(const Scene* scene, Sampler* sampler, const Ray3f& ray, PathInfo& pathInfo) const
        {
            pathInfo.depth++;

            Color3f myLi;
            Color3f myLe;
            Color3f fr;
            Color3f myLr;

            Intersection its;
            if (scene->rayIntersect(ray, its))
            {
                if (its.mesh->isEmitter())
                {
                    EmitterQueryRecord queryRecord(its.toLocal(-ray.d), its.shFrame.n);

                    myLe = its.mesh->getEmitter()->eval(queryRecord);
                }

                const nori::BSDF* materialBSDF = its.mesh->getBSDF();
                BSDFQueryRecord queryRecord2(its.toLocal(-ray.d));

                Color3f bsdfSample = materialBSDF->sample(queryRecord2, sampler->next2D());
                float probability = std::max({ pathInfo.pathThroughput.x(), pathInfo.pathThroughput.y(), pathInfo.pathThroughput.z() });
                
                if (sampler->next1D() <= probability)
                {
                    Ray3f newRay(its.p, its.toWorld(queryRecord2.wo));

                    myLi = LiRecursive(scene, sampler, newRay, pathInfo);
                    fr = bsdfSample / probability;

                    myLr += myLi * fr * Frame::cosTheta(queryRecord2.wo);
                    pathInfo.pathThroughput *= fr;
                }

                return myLe + myLr;
            }
            else
            {
                return Color3f(0.0f);
            }
        }

        Color3f Li(const Scene* scene, Sampler* sampler, const Ray3f& ray) const 
        {

            PathInfo pathInfo;
            pathInfo.depth = 0;
            pathInfo.pathThroughput.setOnes();

            return LiRecursive(scene, sampler, ray, pathInfo);

            /*Intersection its;

            if (scene->rayIntersect(ray, its))
            {
                if (its.mesh->isEmitter())
                {
                    Vector3f inverseRay = -ray.d;
                    EmitterQueryRecord queryRecord;
                    queryRecord.wi = inverseRay;
                    queryRecord.intersectionNormal = its.shFrame.n;

                    return its.mesh->getEmitter()->eval(queryRecord);
                }
                else
                {
                    Vector3f inverseRay = -ray.d;
                    const nori::BSDF* materialBSDF = its.mesh->getBSDF();
                    BSDFQueryRecord queryRecord2(inverseRay);

                    auto bsdfSample = materialBSDF->sample(queryRecord2, sampler->next2D());
                    
                    Ray3f ray2(its.p, its.toWorld(queryRecord2.wo));
                    Intersection its2;

                    if (scene->rayIntersect(ray2, its2))
                    {
                        if (its2.mesh->isEmitter())
                        {
                            EmitterQueryRecord queryRecord;
                            queryRecord.wi = -ray2.d;
                            queryRecord.intersectionNormal = its2.shFrame.n;

                            return bsdfSample * its2.mesh->getEmitter()->eval(queryRecord);
                        }
                    }
                }
            }

            return Color3f(0.0f);*/
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

NORI_REGISTER_CLASS(PathTracing, "pathtracer");
NORI_NAMESPACE_END