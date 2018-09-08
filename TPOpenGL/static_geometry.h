#pragma once

class StaticGeometry
{
public:

	static void load();

	static void renderLightCube();

	static void deleteGeometry();

private:
	static unsigned int cubeVBO, cubeVAO;
	static unsigned int lightVAO;
};

