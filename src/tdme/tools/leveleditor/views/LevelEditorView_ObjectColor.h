#pragma once

#include <tdme/tdme.h>
#include <tdme/tools/leveleditor/views/fwd-tdme.h>

using tdme::tools::leveleditor::views::LevelEditorView;

/**
 * Object Color
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::leveleditor::views::LevelEditorView_ObjectColor final
{
	friend class LevelEditorView;

private:
	LevelEditorView* levelEditorView { nullptr };

	float colorMulR;
	float colorMulG;
	float colorMulB;
	float colorAddR;
	float colorAddG;
	float colorAddB;

public:
	/**
	 * Public constructor
	 * @param levelEditorView level editor view
	 * @param colorMulR colorMulR
	 * @param colorMulG colorMulG
	 * @param colorMulB colorMulB
	 * @param colorAddR colorAddR
	 * @param colorAddG colorAddG
	 * @param colorAddB colorAddB
	 */
	LevelEditorView_ObjectColor(LevelEditorView* levelEditorView, float colorMulR, float colorMulG, float colorMulB, float colorAddR, float colorAddG, float colorAddB);
};
