#include "Application.h"


int main()
{
	auto *App = new Application();
	App->Run();

	delete App;

	return 0;
}