#pragma once
#include "../GameInfo/GameInfo.h"
#include "../UE4/CoreUObject/CoreUObject_classes.hpp"
namespace ClassDefFinder
{
	bool FindUObjectDefs(UE4::UObject* CoreUObject, UE4::UObject* UEObject);
	bool FindUFieldDefs();
	bool FindUStructDefs();
	bool FindUFunctionDefs();
	bool FindUEProperty();
};
