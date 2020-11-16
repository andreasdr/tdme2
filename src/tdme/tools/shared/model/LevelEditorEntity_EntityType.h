#pragma once

#include <string>

#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/utilities/Enum.h>

using std::string;

using tdme::utilities::Enum;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntity_EntityType;

class tdme::tools::shared::model::LevelEditorEntity_EntityType final
	: public Enum
{
	friend class LevelEditorEntity;

public:
	static LevelEditorEntity_EntityType* TRIGGER;
	static LevelEditorEntity_EntityType* MODEL;
	static LevelEditorEntity_EntityType* EMPTY;
	static LevelEditorEntity_EntityType* PARTICLESYSTEM;

	/**
	 * Public constructor
	 * @param name name
	 * @param ordinal ordinal
	 */
	LevelEditorEntity_EntityType(const string& name, int ordinal);

	/**
	 * Returns enum object given by name
	 * @param name name
	 * @return enum object
	 */
	static LevelEditorEntity_EntityType* valueOf(const string& name);

};
