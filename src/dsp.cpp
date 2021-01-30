#include "dsp.hpp"
#include "stdio.h"

#include <SDL_mixer.h>

SteelmillDSP::SteelmillDSP()
{
    model = std::make_shared<PSPhysMod>();
    obj_type = PS_OBJECT_TUBE;
    actuationType = ActuationType::ACTUATION_COMPRESSION;

    height = 5;
    width = 5;
    tension = 4.0f;
    speed = 0.2f;
    damping = 0.05f;
    velocity = 1.0f;
    length = 10.f;
    innodeNormal = 0.f;
    outnodeNormal = 1.f;
}

void SteelmillDSP::trigger()
{
    isTriggered = true;
    timeout = 10;
    maxamp = 0;
    hipass = 0;
    lowpass = 0;

    switch (obj_type)
    {
    case PSObjType::PS_OBJECT_ROD:
        ps_metal_obj_rod_initialize(model->obj, model->height, model->tension);
        break;
    case PSObjType::PS_OBJECT_TUBE:
        ps_metal_obj_tube_initialize(model->obj, model->height, model->width, model->tension);
        break;
    case PSObjType::PS_OBJECT_PLANE:
        ps_metal_obj_plane_initialize(model->obj, model->height, model->width, model->tension);
        // DEBUG("plane initialized");
        break;
    default:
        break;
    };

    metalObject->initializeNodes();
}

void SteelmillDSP::changeActuation(ActuationType type)
{
    model->actuation = type;
}

void SteelmillDSP::changeSetting(SettingType setting, float value)
{
    // int h, w;
    // bool rerender = false;

    // // Settings that require a re-render
    // switch (setting)
    // {
    // case SETTING_HEIGHT:
    //     h = (int)value;
    //     if (model->height != h)
    //     {
    //         model->height = h;
    //         rerender = true;
    //     }
    //     break;
    // case SETTING_WIDTH:
    //     w = (int)value;
    //     if (model->width != w)
    //     {
    //         model->width = w;
    //         rerender = true;
    //     }
    //     break;
    // default:
    //     break;
    // }

    // // Settings that are 0-1
    // switch (setting)
    // {
    // case SETTING_TENSION:
    //     if (fabs(model->tension - value) > 0.02)
    //     {
    //         model->tension = value;
    //     }
    //     break;
    // case SETTING_SPEED:
    //     model->speed = value;
    //     break;
    // case SETTING_DAMPING:
    //     model->damping = value;
    //     break;
    // case SETTING_VELOCITY:
    //     model->velocity = value;
    //     break;
    // case SETTING_HIT:
    //     updateHitNode(value);
    //     break;
    // case SETTING_SAMPLE:
    //     updateSampleNode(value);
    //     break;
    // case SETTING_GAIN:
    //     model->gain = value;
    //     break;
    // default:
    //     break;
    // }

    // if (rerender)
    //     changeShape(obj_type);
}

void SteelmillDSP::setHitNode() {
    int min, max;

    switch(obj_type) {
        case PSObjType::PS_OBJECT_TUBE:
            min = metalObject->width;
            max = metalObject->width * (metalObject->height - 1) - 1;
            break;
        case PSObjType::PS_OBJECT_ROD:
            min = 1;
            max = metalObject->height - 2;
            break;
        case PSObjType::PS_OBJECT_PLANE:
            min = 1;
            max = metalObject->height * metalObject->width - 2;
            break;
        default:
            break;
    }

    metalObject->innode = (int)((max - min) * innodeNormal + min);
    printf("set innode to %d\n", metalObject->innode);
}

void SteelmillDSP::setSampleNode() {
    int min, max;

    switch (obj_type)
    {
    case PSObjType::PS_OBJECT_TUBE:
        min = metalObject->width;
        max = metalObject->width * (metalObject->height - 1) - 1;
        break;
    case PSObjType::PS_OBJECT_ROD:
        min = 1;
        max = metalObject->height - 2;
        break;
    case PSObjType::PS_OBJECT_PLANE:
        min = 1;
        max = metalObject->height * metalObject->width - 2;
        break;
    default:
        break;
    }

    metalObject->outnode = (int)((max - min) * outnodeNormal + min);
    printf("set outnode to %d\n", metalObject->outnode);
}

void SteelmillDSP::process(float sampleTime, float sampleRate)
{
    // sampleCount = (sampleCount + 1) % (int)sampleRate;

    // if (isTriggered)
    // {
    //     timeout -= sampleTime;

    //     if (timeout <= 0)
    //     {
    //         timeout = 0;
    //         isTriggered = false;
    //         sample = 0;
    //         return;
    //     }

    //     sample = nextSample(sampleRate);
    //     // if (sampleCount % 3 == 0)
    //         // sample = clamp(nextSample(sampleRate / 3.f) * model->gain, -5.f, 5.f);
    // }
    // else
    // {
    //     sample = 0;
    // }
}

size_t SteelmillDSP::doRender(int rate, size_t size, float *samples)
{
    // Target attenuation before ending
    // TODO: Make this configurable?
    float att = -50;

    float stasis, damp;
    float sample, hipass_coeff, lowpass_coeff;

    float curr_att = 0.0f;

    if (actuationType == ActuationType::ACTUATION_COMPRESSION)
    {
        stasis = metalObject->nodes[metalObject->outnode]->pos.z;
        metalObject->nodes[metalObject->innode]->pos.z += velocity;
    }
    else
    {
        stasis = 0;
        metalObject->nodes[metalObject->innode]->pos.x += velocity;
    }

    hipass_coeff = pow(0.5, 5.0 / rate);
    lowpass_coeff = 1 - 20.0 / rate; /* 50 ms integrator */
    damp = pow(0.5, 1.0 / (damping * rate));

    float maxvol = 0.001f;
    int i;
    for(i = 0; i < size && !metalObject->stop; i++) {
        metalObject->perturb(speed, damp);

        if (actuationType == ActuationType::ACTUATION_COMPRESSION)
            sample = metalObject->nodes[metalObject->outnode]->pos.z - stasis;
        else
        {
            sample = metalObject->nodes[metalObject->outnode]->pos.x - stasis;
        }

        hipass = hipass_coeff * hipass + (1.0 - hipass_coeff) * sample;
        samples[i] = sample - hipass;

        if (fabs(samples[i]) > maxvol) {
            maxvol = fabs(samples[i]);
        }

        lowpass = lowpass_coeff * lowpass + (1.0 - lowpass_coeff) * fabs(samples[i]);

        if (maxamp < lowpass)
            maxamp = lowpass;

        // if (maxamp > 0.0)
        //     curr_att = 20 * log10(lowpass / maxamp);

        // if (curr_att <= att)
        // {
        //     isTriggered = false;
        //     timeout = 0;
        // }
    }

    int length = i;
    maxvol = 1.f / maxvol;
    for (i = 0; i < length; i++) {
        samples[i] *= maxvol;

        // printf("%.4f\n", samples[i]);
    }

    return length;
}

void SteelmillDSP::prepareObject() {
    printf("Prepating object\n");
    switch (obj_type)
    {
    case PSObjType::PS_OBJECT_TUBE:
        metalObject = std::make_shared<MetalObjectPipe>(width, height, tension);
        break;
    case PSObjType::PS_OBJECT_ROD:
        metalObject = std::make_shared<MetalObjectRod>(height, tension);
        break;
    case PSObjType::PS_OBJECT_PLANE:
        metalObject = std::make_shared<MetalObjectSheet>(width, height, tension);
        break;

    default:
        break;
    }

    setHitNode();
    setSampleNode();
}

void SteelmillDSP::render(int sampleRate) {
    int size = (int)(sampleRate * length);
    data = (float *)malloc(sizeof(float) * size);
    uint16_t *samples = (uint16_t *)malloc(sizeof(uint16_t) * size);

    int i;

    printf("Rendering... %d...", size);
    isRendering = true;
    size_t length = doRender(sampleRate, size, data);

    for(i = 0; i < length; i++) {
        int out;

        if (data[i] >= 1.0)
            out = 127;
        else if (data[i] <= -1.0)
            out = -128;
        else
            out = (int)((data[i] + 1.0) * 128.0 - 128.0);

        samples[i] = out;
    }

    // for(i = 0; i < size && !metalObject->stop; i++) {
    //     data[i] = doRe(sampleRate);
    // }
    printf("Done!\n");

    int result = Mix_OpenAudio(sampleRate, AUDIO_S16SYS, 2, 512);
    if (result < 0)
    {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
        exit(-1);
    }

    result = Mix_AllocateChannels(4);
    if (result < 0)
    {
        fprintf(stderr, "Unable to allocate mixing channels: %s\n", SDL_GetError());
        exit(-1);
    }

    Mix_Chunk *mixSample;
    // memset(mixSample, 0, sizeof(Mix_Chunk));
    mixSample = Mix_QuickLoad_RAW(samples, length);
    if (mixSample == nullptr)
    {
        fprintf(stderr, "Unable to load raw data: %s\n", SDL_GetError());
        exit(-1);
    }

    Mix_PlayChannel(-1, mixSample, 0);
    // Mix_FreeChunk(mixSample);

    isRendering = false;
}
