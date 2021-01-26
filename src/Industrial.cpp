#include "plugin.hpp"
#include "industrial/dsp.hpp"

struct Industrial : Module {
	enum ParamIds
	{
		TYPE_BTN_PARAM,
		ACTUATION_BTN_PARAM,
		DIAL_WIDTH_PARAM,
		DIAL_HEIGHT_PARAM,
		DIAL_HIT_PARAM,
		DIAL_SAMPLE_PARAM,
		DIAL_TENSION_PARAM,
		DIAL_SPEED_PARAM,
		DIAL_DAMPING_PARAM,
		DIAL_VELOCITY_PARAM,
		DIAL_GAIN_PARAM,

		DIAL_TENSION_TRIM_PARAM,
		DIAL_SPEED_TRIM_PARAM,
		DIAL_DAMP_TRIM_PARAM,
		DIAL_VELOCITY_TRIM_PARAM,

		TRIGGER_PARAM,
		NUM_PARAMS
	};
	enum InputIds 
	{
		TRIGGER_IN_INPUT,
		VOCT_INPUT,
		SPEED_INPUT,
		DAMP_INPUT,
		VELOCITY_INPUT,
		NUM_INPUTS
	};
	enum OutputIds
	{
		OUT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		LIGHT_TYPE_TUBE_LIGHT,
		LIGHT_TYPE_ROD_LIGHT,
		LIGHT_TYPE_SHEET_LIGHT,
		LIGHT_ACT_COMP_LIGHT,
		LIGHT_ACT_PERP_LIGHT,
		TRIGGER_LIGHT,
		NUM_LIGHTS
	};

	dsp::BooleanTrigger trigger;
	dsp::BooleanTrigger shapeTrigger;
	dsp::BooleanTrigger actuationTrigger;
	dsp::BooleanTrigger cvTrigger;

	dsp::SampleRateConverter<1> inputSrc;
	dsp::SampleRateConverter<1> outputSrc;
	dsp::DoubleRingBuffer<dsp::Frame<1>, 256> inputBuffer;
	dsp::DoubleRingBuffer<dsp::Frame<1>, 256> outputBuffer;

	Industrializer *industrializer;

	int shape;
	int actuation;
	int updateCount;

	Industrial() {
		industrializer = new Industrializer();

		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(DIAL_TENSION_TRIM_PARAM, -1.f, 1.f, 0.f, "");
		configParam(DIAL_SPEED_TRIM_PARAM, -1.f, 1.f, 0.f, "");
		configParam(DIAL_DAMP_TRIM_PARAM, -1.f, 1.f, 0.f, "");
		configParam(DIAL_VELOCITY_TRIM_PARAM, -1.f, 1.f, 0.f, "");

		configParam(TRIGGER_PARAM, 0.f, 1.f, 0.f, "Trigger");
		configParam(TYPE_BTN_PARAM, 0.f, 1.f, 0.f, "Shape type");
		configParam(ACTUATION_BTN_PARAM, 0.f, 1.f, 0.f, "Actuation type");
		configParam(DIAL_WIDTH_PARAM, 3.f, 20.f, 5.f, "Width");
		configParam(DIAL_HEIGHT_PARAM, 3.f, 20.f, 5.f, "Height");
		configParam(DIAL_HIT_PARAM, 0.f, 1.f, 0.f, "Hit location");
		configParam(DIAL_SAMPLE_PARAM, 0.f, 1.f, 1.f, "Sample location");
		configParam(DIAL_TENSION_PARAM, 0.1f, 16.f, 4.f, "Spring tension");
		configParam(DIAL_SPEED_PARAM, 0.001f, 1.f, 0.1f, "Speed");
		configParam(DIAL_DAMPING_PARAM, 0.f, 1.f, 0.025f, "Damping");
		configParam(DIAL_VELOCITY_PARAM, 0.f, 1.f, 1.f, "Hit velocity");
		configParam(DIAL_GAIN_PARAM, 0.f, 100.f, 1.f, "Output gain");

		updateCount = 0;

		onReset();
	}

	void updateShapeAndActuation() {
		industrializer->changeShape((PSObjType)shape);
		industrializer->changeActuation((ActuationType)actuation);
	}

	void onReset() override {
		shape = PSObjType::PS_OBJECT_TUBE;
		actuation = ActuationType::ACTUATION_COMPRESSION;
		updateShapeAndActuation();
	}

	void onRandomize() override {
		shape = random::u32() % PSObjType::PS_NUM_OBJECTS;
		actuation = random::u32() % ActuationType::NUM_ACTUATION_TYPES;
		updateShapeAndActuation();
	}

	void dataFromJson(json_t *rootJ) override {
		json_t *shapeJ = json_object_get(rootJ, "shape");
		if (shapeJ) {
			shape = json_integer_value(shapeJ);
		}

		json_t *actuationJ = json_object_get(rootJ, "actuation");
		if (actuationJ) {
			actuation = json_integer_value(actuationJ);
		}
		updateShapeAndActuation();
	}

	json_t *dataToJson() override {
		json_t *rootJ = json_object();

		json_object_set_new(rootJ, "shape", json_integer(shape));
		json_object_set_new(rootJ, "actuation", json_integer(actuation));

		return rootJ;
	}

	void process(const ProcessArgs& args) override {
		if (updateCount++ > 4) {
			updateCount = 0;

			if (shapeTrigger.process(params[TYPE_BTN_PARAM].getValue() > 0.f))
			{
				// next shape
				shape = (shape + 1) % PSObjType::PS_NUM_OBJECTS;
				industrializer->changeShape((PSObjType)shape);
			}

			if (actuationTrigger.process(params[ACTUATION_BTN_PARAM].getValue() > 0.f))
			{
				// next actuation
				actuation = (actuation + 1) % ActuationType::NUM_ACTUATION_TYPES;
				industrializer->changeActuation((ActuationType)actuation);
			}


			if (trigger.process(params[TRIGGER_PARAM].getValue() > 0.f))
			{
				industrializer->trigger();
			}

			if (cvTrigger.process(inputs[TRIGGER_IN_INPUT].getVoltage() > 0.5f))
			{
				industrializer->trigger();
			}


			industrializer->changeSetting(SettingType::SETTING_HEIGHT, clamp(params[DIAL_HEIGHT_PARAM].getValue(), 3.f, 20.f));
			industrializer->changeSetting(SettingType::SETTING_WIDTH, clamp(params[DIAL_WIDTH_PARAM].getValue(), 3.f, 20.f));
			industrializer->changeSetting(SettingType::SETTING_SAMPLE, clamp(params[DIAL_SAMPLE_PARAM].getValue(), 0.f, 1.f));
			industrializer->changeSetting(SettingType::SETTING_HIT, clamp(params[DIAL_HIT_PARAM].getValue(), 0.f, 1.f));

			float voct = inputs[VOCT_INPUT].isConnected() ? rescale(inputs[VOCT_INPUT].getVoltage(), -10.f, 10.f, 0.1f, 16.f) * params[DIAL_TENSION_TRIM_PARAM].getValue() : 0.f;
			industrializer->changeSetting(SettingType::SETTING_TENSION, clamp(voct + params[DIAL_TENSION_PARAM].getValue(), 0.1f, 16.f));

			float speedIn = inputs[SPEED_INPUT].isConnected() ? rescale(inputs[SPEED_INPUT].getVoltage(), -10.f, 10.f, 0.001f, .5f) * params[DIAL_SPEED_TRIM_PARAM].getValue() : 0.f;
			industrializer->changeSetting(SettingType::SETTING_SPEED, clamp(speedIn + params[DIAL_SPEED_PARAM].getValue() * 0.5f, 0.001f, .5f));

			float dampIn = inputs[DAMP_INPUT].isConnected() ? rescale(inputs[DAMP_INPUT].getVoltage(), -10.f, 10.f, 0.f, 1.f) * params[DIAL_DAMP_TRIM_PARAM].getValue() : 0.f;
			industrializer->changeSetting(SettingType::SETTING_DAMPING, clamp(dampIn + params[DIAL_DAMPING_PARAM].getValue() * 0.5f, 0.f, 1.f));

			float velocityIn = inputs[VELOCITY_INPUT].isConnected() ? rescale(inputs[VELOCITY_INPUT].getVoltage(), -10.f, 10.f, 0.f, 1.f) * params[DIAL_VELOCITY_TRIM_PARAM].getValue() : 0.f;
			industrializer->changeSetting(SettingType::SETTING_VELOCITY, clamp(velocityIn + params[DIAL_VELOCITY_PARAM].getValue(), 0.f, 1.f));

			industrializer->changeSetting(SettingType::SETTING_GAIN, params[DIAL_GAIN_PARAM].getValue());
		}


		if (outputBuffer.empty()) {
			{
				// inputSrc.setRate(args.sampleRate, 16000);
				// inputSrc.setChannels()

				industrializer->process(args.sampleTime, args.sampleRate);

				dsp::Frame<1> f;
				f.samples[0] = industrializer->sample;
				outputBuffer.push(f);
			}
		}

		
		lights[TRIGGER_LIGHT].setBrightness((float)industrializer->isTriggered);
		lights[LIGHT_TYPE_ROD_LIGHT].setBrightness((float)shape == PSObjType::PS_OBJECT_ROD);
		lights[LIGHT_TYPE_TUBE_LIGHT].setBrightness((float)shape == PSObjType::PS_OBJECT_TUBE);
		lights[LIGHT_TYPE_SHEET_LIGHT].setBrightness((float)shape == PSObjType::PS_OBJECT_PLANE);

		lights[LIGHT_ACT_COMP_LIGHT].setBrightness((float)actuation == ActuationType::ACTUATION_COMPRESSION);
		lights[LIGHT_ACT_PERP_LIGHT].setBrightness((float)actuation == ActuationType::ACTUATION_HIT);

		if (!outputBuffer.empty()) {
			dsp::Frame<1> f = outputBuffer.shift();
			outputs[OUT_OUTPUT].setVoltage(f.samples[0]);
		}
	}
};


struct IndustrialWidget : ModuleWidget {
	IndustrialWidget(Industrial* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Industrial.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<TL1105>((Vec(28.47, 54.13)), module, Industrial::TYPE_BTN_PARAM));
		addParam(createParamCentered<TL1105>((Vec(124.24, 54.13)), module, Industrial::ACTUATION_BTN_PARAM));
		addParam(createParamCentered<RoundBlackSnapKnob>((Vec(28.47, 122.71)), module, Industrial::DIAL_WIDTH_PARAM));
		addParam(createParamCentered<RoundBlackSnapKnob>((Vec(76.36, 122.71)), module, Industrial::DIAL_HEIGHT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>((Vec(124.24, 122.71)), module, Industrial::DIAL_HIT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>((Vec(172.13, 122.71)), module, Industrial::DIAL_SAMPLE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>((Vec(28.47, 181.29)), module, Industrial::DIAL_TENSION_PARAM));
		addParam(createParamCentered<RoundBlackKnob>((Vec(76.36, 181.29)), module, Industrial::DIAL_SPEED_PARAM));
		addParam(createParamCentered<RoundBlackKnob>((Vec(124.24, 181.29)), module, Industrial::DIAL_DAMPING_PARAM));
		addParam(createParamCentered<RoundBlackKnob>((Vec(172.13, 181.29)), module, Industrial::DIAL_VELOCITY_PARAM));

		addParam(createParamCentered<LEDBezel>((Vec(28.47, 286.88)), module, Industrial::TRIGGER_PARAM));
		addChild(createLightCentered<LEDBezelLight<GreenLight>>((Vec(28.47, 286.88)), module, Industrial::TRIGGER_LIGHT));


		addParam(createParamCentered<Trimpot>((Vec(28.47, 212)), module, Industrial::DIAL_TENSION_TRIM_PARAM));
		addParam(createParamCentered<Trimpot>((Vec(76.36, 212)), module, Industrial::DIAL_SPEED_TRIM_PARAM));
		addParam(createParamCentered<Trimpot>((Vec(124.24, 212)), module, Industrial::DIAL_DAMP_TRIM_PARAM));
		addParam(createParamCentered<Trimpot>((Vec(172.13, 212)), module, Industrial::DIAL_VELOCITY_TRIM_PARAM));

		addInput(createInputCentered<PJ301MPort>((Vec(28.47, 240.29)), module, Industrial::VOCT_INPUT));
		addInput(createInputCentered<PJ301MPort>((Vec(76.36, 240.29)), module, Industrial::SPEED_INPUT));
		addInput(createInputCentered<PJ301MPort>((Vec(124.24, 240.29)), module, Industrial::DAMP_INPUT));
		addInput(createInputCentered<PJ301MPort>((Vec(172.13, 240.29)), module, Industrial::VELOCITY_INPUT));

		addInput(createInputCentered<PJ301MPort>((Vec(28.47, 324)), module, Industrial::TRIGGER_IN_INPUT));

		addParam(createParamCentered<RoundBlackKnob>((Vec(179.13, 286.88)), module, Industrial::DIAL_GAIN_PARAM));
		addOutput(createOutputCentered<PJ301MPort>((Vec(179.13, 324)), module, Industrial::OUT_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>((Vec(53.47, 54.13)), module, Industrial::LIGHT_TYPE_TUBE_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>((Vec(71.34, 54.13)), module, Industrial::LIGHT_TYPE_ROD_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>((Vec(89.2, 54.13)), module, Industrial::LIGHT_TYPE_SHEET_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>((Vec(149.24, 54.13)), module, Industrial::LIGHT_ACT_COMP_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>((Vec(167.11, 54.13)), module, Industrial::LIGHT_ACT_PERP_LIGHT));
	}
};


Model* modelIndustrial = createModel<Industrial, IndustrialWidget>("Industrial");