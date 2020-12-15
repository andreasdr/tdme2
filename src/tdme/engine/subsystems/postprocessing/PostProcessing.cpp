#include <map>
#include <string>

using std::map;
using std::string;

#include <tdme/engine/subsystems/postprocessing/PostProcessing.h>

#include <tdme/engine/model/Color4.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingProgram.h>
#include <tdme/engine/subsystems/rendering/EntityRenderer.h>

using tdme::engine::subsystems::postprocessing::PostProcessing;

using tdme::engine::model::Color4;
using tdme::engine::subsystems::postprocessing::PostProcessingProgram;
using tdme::engine::subsystems::rendering::EntityRenderer;

PostProcessing::PostProcessing() {
	{
		auto program = new PostProcessingProgram(PostProcessingProgram::RENDERPASS_FINAL);
		program->addPostProcessingStep("depth_blur", PostProcessingProgram::FRAMEBUFFERSOURCE_SCREEN, PostProcessingProgram::FRAMEBUFFERTARGET_SCREEN);
		if (program->isSupported() == true) {
			programs["depth_blur"] = program;
		} else {
			delete program;
		}

	}
	{
		auto program = new PostProcessingProgram(PostProcessingProgram::RENDERPASS_FINAL);
		program->addPostProcessingStep("desaturation", PostProcessingProgram::FRAMEBUFFERSOURCE_SCREEN, PostProcessingProgram::FRAMEBUFFERTARGET_SCREEN);
		if (program->isSupported() == true) {
			programs["desaturation"] = program;
		} else {
			delete program;
		}

	}
	{
		auto program = new PostProcessingProgram(PostProcessingProgram::RENDERPASS_OBJECTS);
		program->addPostProcessingStep("ssao_map", PostProcessingProgram::FRAMEBUFFERSOURCE_SCREEN, PostProcessingProgram::FRAMEBUFFERTARGET_TEMPORARY);
		program->addPostProcessingStep("ssao", PostProcessingProgram::FRAMEBUFFERSOURCE_SCREEN, PostProcessingProgram::FRAMEBUFFERTARGET_SCREEN, true);
		if (program->isSupported() == true) {
			programs["ssao"] = program;
		} else {
			delete program;
		}
	}
	{
		auto program = new PostProcessingProgram(PostProcessingProgram::RENDERPASS_FINAL);
		program->addEffectPass(
			Engine::EFFECTPASS_LIGHTSCATTERING,
			2,
			2,
			"ls_",
			false,
			false,
			false,
			EntityRenderer::RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY |
				EntityRenderer::RENDERTYPE_MATERIALS_DIFFUSEMASKEDTRANSPARENCY |
				EntityRenderer::RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY,
			Color4(1.0f, 1.0f, 1.0f, 0.0f),
			true,
			true
		);
		program->addPostProcessingStep(
			"light_scattering",
			PostProcessingProgram::FRAMEBUFFERSOURCE_EFFECTPASS0,
			PostProcessingProgram::FRAMEBUFFERTARGET_SCREEN,
			false,
			PostProcessingProgram::FRAMEBUFFERSOURCE_SCREEN
		);
		if (program->isSupported() == true) {
			programs["light_scattering"] = program;
		} else {
			delete program;
		}
	}
	{
		auto program = new PostProcessingProgram(PostProcessingProgram::RENDERPASS_FINAL);
		program->addPostProcessingStep("vignette", PostProcessingProgram::FRAMEBUFFERSOURCE_SCREEN, PostProcessingProgram::FRAMEBUFFERTARGET_SCREEN);
		if (program->isSupported() == true) {
			programs["vignette"] = program;
		} else {
			delete program;
		}

	}
}

PostProcessingProgram* PostProcessing::getPostProcessingProgram(const string& programId) {
	auto programIt = programs.find(programId);
	return programIt == programs.end()?nullptr:programIt->second;
}


