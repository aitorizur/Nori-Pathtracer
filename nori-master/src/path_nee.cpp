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
    class PathTracingNEE : public Integrator {
    enum class SAMPLING_MODE { MATERIAL, LIGHT, MIS };

    public:
        PathTracingNEE(const PropertyList& propList) {
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

        Color3f LiRecursive(const Scene* scene, Sampler* sampler, const Intersection& its, PathInfo& pathInfo) const
        {

            pathInfo.depth++;

            Color3f myLi;
            Color3f myLe;
            Color3f fr;
            Color3f myLr;

            //if (its.mesh->isEmitter())
            //{
            //    EmitterQueryRecord emitterRecord;
            //    emitterRecord.wi = its.toLocal(-ray.d);
            //    emitterRecord.intersectionNormal = its.shFrame.n;
            //
            //    myLe = its.mesh->getEmitter()->eval(emitterRecord);
            //}
            /*
            float probability = std::max({ pathInfo.pathThroughput.x(), pathInfo.pathThroughput.y(), pathInfo.pathThroughput.z() });


            if (sampler->next1D() <= probability)
            {
                //BSDF Sampling
                const nori::BSDF* materialBSDF = its.mesh->getBSDF();
                BSDFQueryRecord bsdfRecord(its.toLocal(-ray.d));

                Color3f bsdfSample = materialBSDF->sample(bsdfRecord, sampler->next2D());

                //Light Sampling
                EmitterQueryRecord emitterRecord2 = EmitterQueryRecord();
                scene->sampleEmitter(emitterRecord2, *sampler);

                Ray3f newRayLight(its.p, (emitterRecord2.sampledPoint - its.p));
                Intersection lightSampleIts;

                Color3f lightFr;
                Color3f lightLe;
                if (scene->rayIntersect(newRayLight, lightSampleIts) && lightSampleIts.mesh->getEmitter() == emitterRecord2.emitter)
                {
                    Color3f lightLe = emitterRecord2.emitter->sample(emitterRecord2, sampler->next2D());
                    lightFr = materialBSDF->eval() / materialBSDF->pdf();
                }
                else
                {
                    lightLe = Color3f(0.0f, 0.0f, 0.0f);
                }

                Ray3f nextEventRay(its.p, its.toWorld(bsdfRecord.wo));
                Intersection nextEventIts;

                //Next Event Estimation

                float indirectWeight;
                if (scene->rayIntersect(nextEventRay, nextEventIts))
                {
                    if (nextEventIts.mesh->isEmitter())
                    {
                        EmitterQueryRecord emitterRecord;
                        emitterRecord.wi = its.toLocal(-ray.d);
                        emitterRecord.intersectionNormal = its.shFrame.n;

                        myLe = its.mesh->getEmitter()->eval(emitterRecord);

                        indirectWeight = materialBSDF->pdf(bsdfRecord) / (materialBSDF->pdf(bsdfRecord) + emitterIts->pdf(bsdfEmitterRecord));
                    }
                    else
                    {
                        myLi = LiRecursive(scene, sampler, nextEventIts, pathInfo);
                        indirectWeight = 0.0f;
                    }


                }

                //Hacer aqui next event estimatio. Basicamnente lanzar el rayo y calcular la interseccion y si es una emisor calcular la probabilidad
                // y no lanzar la recursividad, pero si no lo es la probabilidad será 0

                Color3f bsdfFr = bsdfSample / probability; //* myLi;

                // MIS

                BSDFQueryRecord emitterBsdfRecord();

                float directWeight = emitterRecord2.emitter->pdf(emitterRecord2) / (emitterRecord2.emitter->pdf(emitterRecord2) + materialBSDF->pdf(emitterRecord2));


                //Linea de javi
                myLi = indirectWeight * bsdfFr * myLi + directWeight * lightFr * lightLe;


                myLr += myLi * fr * Frame::cosTheta(bsdfRecord.wo);
                pathInfo.pathThroughput *= bsdfFr;
            }

            return myLe + myLr;
            
            */

            return Color3f(0.0f);
        }

        Color3f Li(const Scene* scene, Sampler* sampler, const Ray3f& ray) const
        {
            
            PathInfo pathInfo;
            pathInfo.depth = 0;
            pathInfo.pathThroughput.setOnes();

            Intersection firstIntersection;
            Ray3f firstRay;

            if (scene->rayIntersect(firstRay, firstIntersection))
            {
                if (firstIntersection.mesh->isEmitter())
                {
                    EmitterQueryRecord emitterRecord;
                    emitterRecord.wi = firstIntersection.toLocal(-ray.d);
                    emitterRecord.intersectionNormal = firstIntersection.shFrame.n;

                    return firstIntersection.mesh->getEmitter()->eval(emitterRecord);
                }
                else
                {
                    return LiRecursive(scene, sampler, firstIntersection, pathInfo);
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

NORI_REGISTER_CLASS(PathTracingNEE, "pathtracer_nee");
NORI_NAMESPACE_END