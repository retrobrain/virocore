//
//  VROPBRDirectTest.cpp
//  ViroKit
//
//  Created by Raj Advani on 1/18/18.
//  Copyright © 2018 Viro Media. All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining
//  a copy of this software and associated documentation files (the
//  "Software"), to deal in the Software without restriction, including
//  without limitation the rights to use, copy, modify, merge, publish,
//  distribute, sublicense, and/or sell copies of the Software, and to
//  permit persons to whom the Software is furnished to do so, subject to
//  the following conditions:
//
//  The above copyright notice and this permission notice shall be included
//  in all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "VROPBRDirectTest.h"
#include "VROTestUtil.h"
#include "VROCompress.h"
#include "VROSphere.h"

VROPBRDirectTest::VROPBRDirectTest() :
    VRORendererTest(VRORendererTestType::PBRDirect) {
        _angle = 0;
}

VROPBRDirectTest::~VROPBRDirectTest() {
    
}

void VROPBRDirectTest::build(std::shared_ptr<VRORenderer> renderer,
                             std::shared_ptr<VROFrameSynchronizer> frameSynchronizer,
                             std::shared_ptr<VRODriver> driver) {
    _sceneController = std::make_shared<VROARSceneController>();
    std::shared_ptr<VROScene> scene = _sceneController->getScene();
    
    std::shared_ptr<VROPortal> rootNode = scene->getRootNode();
    rootNode->setPosition({0, 0, 0});
    
    VROVector3f lightPositions[] = {
        VROVector3f(-10,  10, 10),
        VROVector3f( 10,  10, 10),
        VROVector3f(-10, -10, 10),
        VROVector3f( 10, -10, 10),
    };
    
    float intensity = 300;
    VROVector3f lightColors[] = {
        VROVector3f(1, 1, 1),
        VROVector3f(1, 1, 1),
        VROVector3f(1, 1, 1),
        VROVector3f(1, 1, 1),
    };
    
    int rows = 7;
    int columns = 7;
    float spacing = 2.5;
    
    std::shared_ptr<VRONode> sphereContainerNode = std::make_shared<VRONode>();
    
    // Render an array of spheres, varying roughness and metalness
    for (int row = 0; row < rows; ++row) {
        float radius = 1;
        float metalness = (float) row / (float) rows;
        
        for (int col = 0; col < columns; ++col) {
            // Clamp the roughness to [0.05, 1.0] as perfectly smooth surfaces (roughness of 0.0)
            // tend to look off on direct lighting
            float roughness = VROMathClamp((float) col / (float) columns, 0.05, 1.0);
            
            std::shared_ptr<VROSphere> sphere = VROSphere::createSphere(radius, 20, 20, true);
            const std::shared_ptr<VROMaterial> &material = sphere->getMaterials().front();
            material->getDiffuse().setColor({ 0.5, 0.0, 0.0, 1.0 });
            material->getRoughness().setColor({ roughness, 1.0, 1.0, 1.0 });
            material->getMetalness().setColor({ metalness, 1.0, 1.0, 1.0 });
            material->getAmbientOcclusion().setColor({ 1.0, 1.0, 1.0, 1.0 });
            material->setLightingModel(VROLightingModel::PhysicallyBased);
            
            std::shared_ptr<VRONode> sphereNode = std::make_shared<VRONode>();
            sphereNode->setPosition({ (float)(col - (columns / 2)) * spacing,
                                      (float)(row - (rows    / 2)) * spacing,
                                      0.0f });
            sphereNode->setGeometry(sphere);
            sphereContainerNode->addChildNode(sphereNode);
        }
    }
    
    rootNode->addChildNode(sphereContainerNode);
    
    // Render the light sources as spheres as well
    for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i) {
        std::shared_ptr<VROLight> light = std::make_shared<VROLight>(VROLightType::Omni);
        light->setColor(lightColors[i]);
        light->setPosition(lightPositions[i]);
        light->setAttenuationStartDistance(20);
        light->setAttenuationEndDistance(30);
        light->setIntensity(intensity);
        rootNode->addLight(light);
    }
    
    std::shared_ptr<VRONodeCamera> camera = std::make_shared<VRONodeCamera>();
    std::shared_ptr<VRONode> cameraNode = std::make_shared<VRONode>();
    cameraNode->setPosition({ 0, 0, 9 });
    cameraNode->setCamera(camera);
    rootNode->addChildNode(cameraNode);
    
    std::shared_ptr<VROAction> action = VROAction::perpetualPerFrameAction([this] (VRONode *const node, float seconds) {
        _angle += .015;
        node->setRotation({ 0, _angle, 0});
        return true;
    });
    //sphereContainerNode->runAction(action);
    
    _pointOfView = cameraNode;
}

