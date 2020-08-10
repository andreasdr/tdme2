
#pragma once

#include <string>

#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/utils/Enum.h>

using std::string;

using tdme::utils::Enum;
using tdme::engine::model::ModelHelper;
using tdme::engine::model::ModelHelper_VertexOrder;

class tdme::engine::model::ModelHelper_VertexOrder final
	: public Enum
{
	friend class ModelHelper;
public:
	static ModelHelper_VertexOrder* CLOCKWISE;
	static ModelHelper_VertexOrder* COUNTERCLOCKWISE;

	ModelHelper_VertexOrder(const string& name, int ordinal);

public:
	static ModelHelper_VertexOrder* valueOf(const string& a0);
};
