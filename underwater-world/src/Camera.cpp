#include "Camera.h"

glm::mat4 Core::createPerspectiveMatrix(float zNear, float zFar)
{
	const float frustumScale = 1.1f;
	glm::mat4 perspective;
	perspective[0][0] = frustumScale;
	perspective[1][1] = frustumScale;
	perspective[2][2] = (zFar + zNear) / (zNear - zFar);
	perspective[3][2] = (2 * zFar * zNear) / (zNear - zFar);
	perspective[2][3] = -1;
	perspective[3][3] = 0;

	return perspective;
}

glm::mat4 Core::createViewMatrix( glm::vec3 position, glm::vec3 forward, glm::vec3 up )
{
	glm::vec3 side = glm::cross(forward, up);

	// Trzeba pamietac o minusie przy ustawianiu osi Z kamery.
	// Wynika to z tego, ze standardowa macierz perspektywiczna zaklada, ze "z przodu" jest ujemna (a nie dodatnia) czesc osi Z.
	glm::mat4 cameraRotation;
	cameraRotation[0][0] = side.x; cameraRotation[1][0] = side.y; cameraRotation[2][0] = side.z;
	cameraRotation[0][1] = up.x; cameraRotation[1][1] = up.y; cameraRotation[2][1] = up.z;
	cameraRotation[0][2] = -forward.x; cameraRotation[1][2] = -forward.y; cameraRotation[2][2] = -forward.z;

	glm::mat4 cameraTranslation;
	cameraTranslation[3] = glm::vec4(-position, 1.0f);

	return cameraRotation * cameraTranslation;
}

glm::mat4 Core::createViewMatrixQuat(glm::vec3 position, glm::quat rotation)
{
	glm::mat4 cameraTranslation;
	cameraTranslation[3] = glm::vec4(-position, 1.0f);

	return glm::mat4_cast(rotation) * cameraTranslation;
}
