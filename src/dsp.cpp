#include "dsp.hpp"

Industrializer::Industrializer()
{
    model = new PSPhysMod();

    model->obj_type = PS_OBJECT_PLANE;
    model->height = 5;
    model->width = 5;
    model->tension = 4.0f;
    model->speed = 0.2f;
    model->damping = 0.05f;
    model->actuation = 0;
    model->velocity = 1.0f;
    model->gain = 1.0f;

    changeShape(PSObjType::PS_OBJECT_TUBE);
}

void Industrializer::trigger()
{
    isTriggered = true;
    timeout = 10;
    maxamp = 0;
    hipass = 0;
    lowpass = 0;

    switch (model->obj_type)
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
}

void Industrializer::changeShape(PSObjType shape)
{
    model->obj_type = shape;

    if (model->obj)
        ps_metal_obj_free(model->obj);

    switch (shape)
    {
    case PSObjType::PS_OBJECT_TUBE:
        model->obj = ps_metal_obj_new_tube(model->height, model->width, model->tension);

        /*
            value: model->width + model->width / 2
            lower: model->width
            upper: model->width * (model->height - 1) - 1
        */
        model->innode = model->width + model->height / 2;

        /*
            value: (model->height - 2) * model->width
            lower: model->width
            upper: model->width * (model->height - 1) - 1
        */
        model->outnode = (model->height - 2) * model->width;
        break;
    case PSObjType::PS_OBJECT_ROD:
        model->obj = ps_metal_obj_new_rod(model->height, model->tension);

        /*
            value: 1
            lower: 1
            upper: model->length - 2
        */
        model->innode = 1;

        /*
            value: model->height - 2
            lower: 1
            upper: model->height - 2
        */
        model->outnode = model->height - 2;
        break;
    case PSObjType::PS_OBJECT_PLANE:
        model->obj = ps_metal_obj_new_plane(model->height, model->width, model->tension);

        /*
            value: 1
            lower: 1
            upper: model->height * model->width - 2
        */
        model->innode = 1;

        /*
            value: (model->height - 1) * model->width - 1
            lower: 1
            upper: model->height * model->width - 2
        */
        model->outnode = (model->height - 1) * model->width - 1;
        break;

    default:
        break;
    }

    isTriggered = false;
}

void Industrializer::changeActuation(ActuationType type)
{
    model->actuation = type;
}

void Industrializer::changeSetting(SettingType setting, float value)
{
    int h, w;
    bool rerender = false;

    // Settings that require a re-render
    switch (setting)
    {
    case SETTING_HEIGHT:
        h = (int)value;
        if (model->height != h)
        {
            model->height = h;
            rerender = true;
        }
        break;
    case SETTING_WIDTH:
        w = (int)value;
        if (model->width != w)
        {
            model->width = w;
            rerender = true;
        }
        break;
    default:
        break;
    }

    // Settings that are 0-1
    switch (setting)
    {
    case SETTING_TENSION:
        if (fabs(model->tension - value) > 0.02)
        {
            model->tension = value;
        }
        break;
    case SETTING_SPEED:
        model->speed = value;
        break;
    case SETTING_DAMPING:
        model->damping = value;
        break;
    case SETTING_VELOCITY:
        model->velocity = value;
        break;
    case SETTING_HIT:
        updateHitNode(value);
        break;
    case SETTING_SAMPLE:
        updateSampleNode(value);
        break;
    case SETTING_GAIN:
        model->gain = value;
        break;
    default:
        break;
    }

    if (rerender)
        changeShape(model->obj_type);
}

void Industrializer::updateHitNode(float value) {
    int min, max;
    
    if (value == model->innodeNormal) {
        return;
    }

    model->innodeNormal = value;
    
    switch(model->obj_type) {
        case PSObjType::PS_OBJECT_TUBE:
            min = model->width;
            max = model->width * (model->height - 1) - 1;
            break;
        case PSObjType::PS_OBJECT_ROD:
            min = 1;
            max = model->height - 2;
            break;
        case PSObjType::PS_OBJECT_PLANE:
            min = 1;
            max = model->height * model->width - 2;
            break;
        default:
            break;
    }

    model->innode = floor((max - min) * value + min);
}

void Industrializer::updateSampleNode(float value) {
    int min, max;

    if (value == model->outnodeNormal)
    {
        return;
    }

    model->outnodeNormal = value;

    switch (model->obj_type)
    {
    case PSObjType::PS_OBJECT_TUBE:
        min = model->width;
        max = model->width * (model->height - 1) - 1;
        break;
    case PSObjType::PS_OBJECT_ROD:
        min = 1;
        max = model->height - 2;
        break;
    case PSObjType::PS_OBJECT_PLANE:
        min = 1;
        max = model->height * model->width - 2;
        break;
    default:
        break;
    }

    model->outnode = floor((max - min) * value + min);
}

void Industrializer::process(float sampleTime, float sampleRate)
{
    sampleCount = (sampleCount + 1) % (int)sampleRate;

    if (isTriggered)
    {
        timeout -= sampleTime;

        if (timeout <= 0)
        {
            timeout = 0;
            isTriggered = false;
            sample = 0;
            return;
        }

        if (sampleCount % 3 == 0)
            sample = clamp(nextSample(sampleRate / 3.f) * model->gain, -5.f, 5.f);
    }
    else
    {
        sample = 0;
    }
}

float Industrializer::nextSample(float rate)
{
    // Target attenuation before ending
    // TODO: Make this configurable?
    float att = -50;

    float stasis, damp;
    float sample, hipass_coeff, lowpass_coeff;

    float curr_att = 0.0f;

    if (model->actuation)
    {
        stasis = model->obj->nodes[model->outnode]->pos.z();
        model->obj->nodes[model->innode]->pos.vec[2] += model->velocity;
    }
    else
    {
        stasis = 0;
        model->obj->nodes[model->innode]->pos.vec[0] += model->velocity;
    }

    hipass_coeff = pow(0.5, 5.0 / rate);
    lowpass_coeff = 1 - 20.0 / rate; /* 50 ms integrator */
    damp = pow(0.5, 1.0 / (model->damping * rate));

    ps_metal_obj_perturb(model->obj, model->speed, damp);

    if (model->actuation)
        sample = model->obj->nodes[model->outnode]->pos.z() - stasis;
    else
    {
        sample = model->obj->nodes[model->outnode]->pos.x() - stasis;
    }

    hipass = hipass_coeff * hipass + (1.0 - hipass_coeff) * sample;
    sample -= hipass;

    lowpass = lowpass_coeff * lowpass + (1.0 - lowpass_coeff) * fabs(sample);

    if (maxamp < lowpass)
        maxamp = lowpass;

    if (maxamp > 0.0)
        curr_att = 20 * log10(lowpass / maxamp);

    if (curr_att <= att)
    {
        isTriggered = false;
        timeout = 0;
    }

    return sample;
}
