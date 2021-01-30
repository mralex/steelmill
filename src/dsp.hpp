#pragma once

#include <memory>
#include "psphymod/metal_object.hpp"
#include "psphymod/metal_object_rod.hpp"
#include "psphymod/metal_object_pipe.hpp"
#include "psphymod/metal_object_sheet.hpp"

#include "psphymod/psmetalobj.h"

typedef enum
{
    PS_OBJECT_TUBE = 0,
    PS_OBJECT_ROD,
    PS_OBJECT_PLANE,
    PS_NUM_OBJECTS
} PSObjType;

typedef enum 
{
    ACTUATION_COMPRESSION = 0,
    ACTUATION_HIT,
    NUM_ACTUATION_TYPES
} ActuationType;

typedef enum {
    SETTING_HEIGHT = 0,
    SETTING_WIDTH,
    SETTING_TENSION,
    SETTING_SAMPLE,
    SETTING_HIT,
    SETTING_SPEED,
    SETTING_DAMPING,
    SETTING_VELOCITY,
    SETTING_GAIN,
    NUM_SETTINGS
} SettingType;

typedef struct
{
    std::shared_ptr<MetalObject> metalObject;

    PSObjType obj_type;
    PSMetalObj *obj;

    int height, width;
    int innode, outnode;
    float innodeNormal, outnodeNormal;
    float tension, speed, damping;
    int actuation;
    float velocity;
    float percent;

    float gain;
} PSPhysMod;


class SteelmillDSP {
public:
    PSObjType obj_type;
    ActuationType actuationType;

    int width;
    int height;
    float tension;

    float innodeNormal, outnodeNormal;
    float speed, damping, velocity;

    float length, attenuation;

    bool isTriggered = false;
    bool isRendering = false;
    float percentComplete = 0.f;
    float sample = 0;

    float *data;

    SteelmillDSP();

    void trigger();


    void prepareObject();
    void render(int);
    
    
    void process(float, float);

    void createObject(int);

    void changeActuation(ActuationType);
    void changeSetting(SettingType, float);

private:
    int sampleCount = 0;
    float timeout;

    std::shared_ptr<MetalObject> metalObject = nullptr;
    std::shared_ptr<PSPhysMod> model = nullptr;

    float maxamp, hipass, lowpass;

    size_t currentSampleIndex;

    size_t doRender(int, size_t, float*);

    void setHitNode();
    void setSampleNode();
};
