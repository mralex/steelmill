#pragma once

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


class Industrializer {
public:
    bool isTriggered = false;
    float sample = 0;

    Industrializer();

    void trigger();
    void process(float, float);

    void createObject(int);

    void changeShape(PSObjType);
    void changeActuation(ActuationType);
    void changeSetting(SettingType, float);

private:
    int sampleCount = 0;
    float timeout;
    PSPhysMod *model = nullptr;

    float maxamp, hipass, lowpass;

    size_t currentSampleIndex;

    float nextSample(float);

    void updateHitNode(float value);
    void updateSampleNode(float value);
};
