#pragma once

#include <tdme/tdme.h>
#include <tdme/tools/sceneeditor/views/fwd-tdme.h>

using tdme::tools::sceneeditor::views::SceneEditorView;

/**
 * Scene editor view entity color class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::sceneeditor::views::SceneEditorView_EntityColor final
{
	friend class SceneEditorView;

private:
	SceneEditorView* sceneEditorView { nullptr };

	float colorMulR;
	float colorMulG;
	float colorMulB;
	float colorAddR;
	float colorAddG;
	float colorAddB;

public:
	/**
	 * Public constructor
	 * @param sceneEditorView scene editor view
	 * @param colorMulR colorMulR
	 * @param colorMulG colorMulG
	 * @param colorMulB colorMulB
	 * @param colorAddR colorAddR
	 * @param colorAddG colorAddG
	 * @param colorAddB colorAddB
	 */
	SceneEditorView_EntityColor(SceneEditorView* sceneEditorView, float colorMulR, float colorMulG, float colorMulB, float colorAddR, float colorAddG, float colorAddB);
};
