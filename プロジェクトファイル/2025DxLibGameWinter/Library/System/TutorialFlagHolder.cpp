#include "TutorialFlagHolder.h"

TutorialFlagHolder::TutorialFlagHolder() :
	_hasStartedGameOnce(false)
{
}

TutorialFlagHolder& TutorialFlagHolder::GetInstance()
{
	static TutorialFlagHolder holder;
	return holder;
}