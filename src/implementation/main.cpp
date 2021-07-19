#include "config.h"
#include "tools.h"

#define toRadians zer::athm::toRadians

void rayCasting(std::vector<Ray>& rays, std::vector<Boundary>& boundaries, sf::Vector2f& playerPos, float fAngle, float fFovHalf,
	float fRayLength, float fRayStep)
{
	for (float f = -fFovHalf; f < fFovHalf; f += fRayStep)
	{
		float fY = playerPos.y + sin(fAngle + f) * fRayLength;
		float fX = playerPos.x + cos(fAngle + f) * fRayLength;
		float fDist = fRayLength;

		sf::Color rayColor = sf::Color(255, 255, 255);

		for (int i = 0; i < boundaries.size(); ++i)
		{
			/*
				Finding intersection.
			*/
			IntersectionResult result = intersection(std::vector<sf::Vector2f>({
				sf::Vector2f(playerPos.x, playerPos.y),
				sf::Vector2f(fX, fY)
			}), boundaries[i].line);

			if (result.bHasIntersect)
			{
				float fCatet1 = playerPos.y - result.fY;
				float fCatet2 = playerPos.x - result.fX;
				float fHypot = sqrt(fCatet1 * fCatet1 + fCatet2 * fCatet2);

				if (fHypot < fDist)
				{
					rayColor = boundaries[i].color;
					fDist = fHypot;
				}
			}
		}

		rays.push_back(Ray{fDist, rayColor});
	}
}

int loop(sf::RenderWindow& window, std::map<std::string, float>& cfg)
{
	bool bNeedToUpdateConsole = true;
	bool bShowMouse = false;
	
	float fAngle = toRadians(90);
	float fFov = toRadians(cfg["FOV"]);
	float fFovHalf = fFov / 2;
	float fRayLength = cfg["rayLength"];
	float fRayStep = fFov / cfg["projectionPlaneWidth"];
	float fWallSliceWidth = mWW / cfg["projectionPlaneWidth"];
	float fWallHeightCoeff = cfg["wallHeightCoeff"];
	float fDistanceToProjectionPlane = cfg["projectionPlaneWidth"] / tan(fFovHalf);
	float fCollisionRayStep = mDoublePi / cfg["collisionRaysCount"];
	float fCollisionDistance = cfg["collisionDistance"];
	float fPlayerRotateSpeed = cfg["playerRotateSpeed"];
	float fPlayerSpeed = cfg["playerSpeed"];
	float fSensitivity = cfg["sensitivity"];
	float fShadeIntensity = cfg["shadeIntensity"];
	float fShadeMultiplier = cfg["shadeMultiplier"];
	float fLastTime = 0;
	float fObjectsAngleOffset = 0;
	float fObjectsRadius = cfg["objectsRadius"];
	float fObjectsOrbitalRadius = cfg["objectsOrbitalRadius"];
	float fFloorAndCeilSectionWidth = cfg["floorAndCeilSectionWidth"];
	float fFloorAndCeilAlphaValue = cfg["floorAndCeilAlphaValue"];
	float fFloorAndCeilAlphaStep = fFloorAndCeilAlphaValue / (mWHHalf / fFloorAndCeilSectionWidth);

	sf::Vector2f mapCenterPos(cfg["mapWidth"] / 2, cfg["mapHeight"] / 2);
	sf::Vector2f playerPos(mapCenterPos.x, 100);

	sf::Clock clock;

	std::vector<Boundary> boundaries;
	std::vector<Boundary> movingBoundaries;

	/*
		Adding central object.
	*/
	createCircleShape(boundaries, mapCenterPos.y, mapCenterPos.x, fObjectsRadius / 2, 20, sf::Color(64, 224, 208));

	/*
		Adding map borders.
	*/
	boundaries.push_back(Boundary{
		std::vector<sf::Vector2f>({
			sf::Vector2f(0, 0),
			sf::Vector2f(cfg["mapWidth"], 0)
		}),
		sf::Color(255, 0, 0)
	});

	boundaries.push_back(Boundary{
		std::vector<sf::Vector2f>({
			sf::Vector2f(0, cfg["mapHeight"]),
			sf::Vector2f(cfg["mapWidth"], cfg["mapHeight"])
		}),
		sf::Color(0, 255, 0)
	});

	boundaries.push_back(Boundary{
		std::vector<sf::Vector2f>({
			sf::Vector2f(0, 0),
			sf::Vector2f(0, cfg["mapHeight"])
		}),
		sf::Color(0, 0, 255)
	});

	boundaries.push_back(Boundary{
		std::vector<sf::Vector2f>({
			sf::Vector2f(cfg["mapWidth"], 0),
			sf::Vector2f(cfg["mapWidth"], cfg["mapHeight"])
		}),
		sf::Color(255, 0, 255)
	});

	while (window.isOpen())
	{
		if (movingBoundaries.size() > 0)
		{
			boundaries.resize(boundaries.size() - movingBoundaries.size());
			movingBoundaries.resize(0);
		}

		/*
			Adding moving objects.
		*/
		createCircleShape(movingBoundaries, mapCenterPos.y + sin(toRadians(fObjectsAngleOffset + 0)) * fObjectsOrbitalRadius,
			mapCenterPos.x + cos(toRadians(fObjectsAngleOffset + 0)) * fObjectsOrbitalRadius, fObjectsRadius, 3);
		
		createCircleShape(movingBoundaries, mapCenterPos.y + sin(toRadians(fObjectsAngleOffset + 90)) * fObjectsOrbitalRadius,
			mapCenterPos.x + cos(toRadians(fObjectsAngleOffset + 90)) * fObjectsOrbitalRadius, fObjectsRadius, 4);
		
		createCircleShape(movingBoundaries, mapCenterPos.y + sin(toRadians(fObjectsAngleOffset + 180)) * fObjectsOrbitalRadius,
			mapCenterPos.x + cos(toRadians(fObjectsAngleOffset + 180)) * fObjectsOrbitalRadius, fObjectsRadius, 6);
		
		createCircleShape(movingBoundaries, mapCenterPos.y + sin(toRadians(fObjectsAngleOffset + 270)) * fObjectsOrbitalRadius,
			mapCenterPos.x + cos(toRadians(fObjectsAngleOffset + 270)) * fObjectsOrbitalRadius, fObjectsRadius, 20);

		fObjectsAngleOffset = (int)fObjectsAngleOffset % 360;
		fObjectsAngleOffset -= cfg["objectsRotateSpeed"];

		boundaries.insert(boundaries.end(), movingBoundaries.begin(), movingBoundaries.end());

		/*
			Finding rays intersections with boundaries.
		*/
		std::vector<Ray> rays;
		std::vector<Ray> collisionRays;

		rayCasting(rays, boundaries, playerPos, fAngle, fFovHalf, fRayLength, fRayStep);
		rayCasting(collisionRays, boundaries, playerPos, fAngle, mPi, fRayLength, fCollisionRayStep);

		/*
			Counting delta time.
		*/
		float fDT = fLastTime - clock.getElapsedTime().asSeconds();
		fLastTime = clock.getElapsedTime().asSeconds() * 100.0;
		clock.restart();

		/*
			Handling mouse and keyboard events.
		*/
		if (!bShowMouse)
			mouseControl(window, fAngle, fDT, fSensitivity);
		keyControl(playerPos, fAngle, fDT, fPlayerRotateSpeed, fPlayerSpeed);

		/*
			Collision detection based on intersecting collision rays with boundaries.
		*/
		collisionDetection(playerPos, collisionRays, fDT, fCollisionRayStep, fAngle, fPlayerSpeed, fCollisionDistance);

		window.clear();

		drawCeilAndFloor(window, fFloorAndCeilSectionWidth, fFloorAndCeilAlphaStep, fFloorAndCeilAlphaValue, sf::Color(76, 47, 39));

		/*
			Drawing vertical slices based on the distance to the intersection of rays with boundaries.
		*/
		drawWalls(window, rays, fAngle, fFovHalf, fRayStep, fDistanceToProjectionPlane, fShadeIntensity, fShadeMultiplier, fWallHeightCoeff,
			fWallSliceWidth);

		window.display();

		if (bNeedToUpdateConsole)
		{
			displayConsoleInformation(cfg);
			bNeedToUpdateConsole = false;
		}

		switch (eventListener(window))
		{
			case (CLOSE_EVENT_CODE):
				window.close();
				break;

			case (RESTART_EVENT_CODE):
				init(window);
				break;

			case (SHOW_MOUSE_EVENT_CODE):
				bShowMouse = !bShowMouse;

				window.setMouseCursorVisible(bShowMouse);
				sf::Mouse::setPosition(sf::Vector2i(mWWHalf, mWHHalf), window);
				
				break;
		}
	}
	return 0;
}

int init(sf::RenderWindow& window)
{
	std::map<std::string, float> cfg = readConfig(mConfigPath);
	return loop(window, cfg);
}