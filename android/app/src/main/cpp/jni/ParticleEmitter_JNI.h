//
//  ParticleEmitter_JNI.h
//  ViroRenderer
//
//  Copyright © 2017 Viro Media. All rights reserved.
//

#ifndef ANDROID_PARTICLE_EMITTER_JNI_H
#define ANDROID_PARTICLE_EMITTER_JNI_H

#include <memory>
#include <VROPlatformUtil.h>
#include "PersistentRef.h"
#include "VROParticleEmitter.h"

#include "VRODefines.h"
#include VRO_C_INCLUDE

namespace ParticleEmitter {

    inline VRO_REF jptr(std::shared_ptr<VROParticleEmitter> shared_node) {
        PersistentRef<VROParticleEmitter> *native_emitter = new PersistentRef<VROParticleEmitter>(shared_node);
        return reinterpret_cast<intptr_t>(native_emitter);
    }

    inline std::shared_ptr<VROParticleEmitter> native(VRO_REF ptr) {
        PersistentRef<VROParticleEmitter> *persistentObj = reinterpret_cast<PersistentRef<VROParticleEmitter> *>(ptr);
        return persistentObj->get();
    }

    inline std::shared_ptr<VROParticleModifier> getParticleModifier(JNIEnv *env,
                                                                    jstring jFactor,
                                                                    jobjectArray jInitialValue,
                                                                    jobjectArray jInterpolatedIntervals,
                                                                    jobjectArray jInterpolatedValues) {
        // Parse out the initial values for this modifier
        std::vector<VROVector3f> initialValues;
        int numberOfValues = env->GetArrayLength(jInitialValue);
        for (int i = 0; i < numberOfValues; i++) {
            jfloatArray vec3Value = (jfloatArray)env->GetObjectArrayElement(jInitialValue, i);
            VRO_FLOAT *vec3ValueArray = env->GetFloatArrayElements(vec3Value, 0);
            VROVector3f vec3 = VROVector3f(vec3ValueArray[0], vec3ValueArray[1], vec3ValueArray[2]);
            initialValues.push_back(vec3);
        }

        if (jInitialValue == NULL || jInterpolatedValues == NULL) {
            return std::make_shared<VROParticleModifier>(initialValues[0], initialValues[1]);
        }

        // Grab a reference factor for this modifier against which to interpolate.
        std::string strFactorType = VROPlatformGetString(jFactor, env);
        VROParticleModifier::VROModifierFactor bodyType = VROParticleModifier::getModifierFactorForString(strFactorType);

        // Parse out VROModifierIntervals containing sequentially interpolated target values and its
        // corresponding intervals.
        std::vector<VROParticleModifier::VROModifierInterval> interpolatedIntervals;
        int numberOfIntervals = env->GetArrayLength(jInterpolatedIntervals);
        for (int i = 0; i < numberOfIntervals; i++) {
            // Get the interval window this interpolation point applies to
            jfloatArray vec2Value = (jfloatArray)env->GetObjectArrayElement(jInterpolatedIntervals, i);
            VRO_FLOAT *jIntervalWindow = env->GetFloatArrayElements(vec2Value, 0);

            // Get the value to interpolate towards at the end of this interval window
            jfloatArray vec3Value = (jfloatArray)env->GetObjectArrayElement(jInterpolatedValues, i);
            VRO_FLOAT *jPoint = env->GetFloatArrayElements(vec3Value, 0);
            VROVector3f targetedValue = VROVector3f(jPoint[0], jPoint[1], jPoint[2]);

            VROParticleModifier::VROModifierInterval interpolatedPoint;
            interpolatedPoint.startFactor = jIntervalWindow[0];
            interpolatedPoint.endFactor = jIntervalWindow[1];
            interpolatedPoint.targetedValue = targetedValue;

            interpolatedIntervals.push_back(interpolatedPoint);
        }

        return std::make_shared<VROParticleModifier>(initialValues[0],
                                                        initialValues[1],
                                                        bodyType,
                                                        interpolatedIntervals);
    }
}

#endif