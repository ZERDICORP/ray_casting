#include "config.h"

struct Ray
{
	float fDist;

	sf::Color color;
};

struct Boundary
{
	std::vector<sf::Vector2f> line;

	sf::Color color;
};

struct IntersectionResult
{
	bool bHasIntersect;

	float fY;
	float fX;

	IntersectionResult();
	IntersectionResult(float fY, float fX);
};

IntersectionResult intersection(std::vector<sf::Vector2f> line1, std::vector<sf::Vector2f> line2);

void drawCeilAndFloor(sf::RenderWindow& window, float fHorizontalSurfaceH, float fHorizontalSurfaceStep, float fHorizontalSurfaceAlphaStep,
	float fHorizont, sf::Color color);
void collisionDetection(sf::Vector2f& playerPos, std::vector<Ray>& collisionRays, float fDT, float fCollisionRayStep, float fAngle,
	float fPlayerSpeed, float fCollisionDistance);
void drawWalls(sf::RenderWindow& window, std::vector<Ray>& rays, float fAngle, float fFovHalf, float fRayStep,
	float fDistToProjectionPlane, float fShadeIntensity, float fShadeMultiplier, float fHorizont, int iWallHCoeff, int iWallSliceW);
void mouseControl(sf::RenderWindow& window, float& fAngle, float& fHorizont, float fDT, float fSensitivity);
void keyControl(sf::Vector2f& playerPos, float& fAngle, float fDT, float fPlayerRotateSpeed, float fPlayerSpeed);
void createCircleShape(std::vector<Boundary>& boundaries, float fY, float fX, float fRadius, int iVertexCount, sf::Color color = sf::Color(255, 255, 255));
void displayConsoleInformation(std::map<std::string, float>& cfg);

std::map<std::string, float> readConfig(std::string sConfigPath);

EVENT_CODE eventListener(sf::RenderWindow& window);

int init(sf::RenderWindow& window);
int main();