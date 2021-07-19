#include "tools.h"

IntersectionResult::IntersectionResult()
{
	this -> bHasIntersect = false;
}

IntersectionResult::IntersectionResult(float fY, float fX)
{
	this -> fY = fY;
	this -> fX = fX;
	this -> bHasIntersect = true;
}

void drawCeilAndFloor(sf::RenderWindow& window, float fFloorAndCeilSectionWidth, float fFloorAndCeilAlphaStep, float fFloorAndCeilAlphaValue,
	sf::Color color)
{
	float fFloorAlpha = 0;
	for (float i = 0; i < mWHHalf; i += fFloorAndCeilSectionWidth, fFloorAlpha += fFloorAndCeilAlphaStep)
	{
		sf::RectangleShape rectFloor(sf::Vector2f(mWW, fFloorAndCeilSectionWidth));
		rectFloor.setPosition(0, mWHHalf + i);
		rectFloor.setFillColor(sf::Color(color.r, color.g, color.b, fFloorAlpha));

		window.draw(rectFloor);
	}

	float fCeilAlpha = fFloorAndCeilAlphaValue;
	for (float i = 0; i < mWHHalf; i += fFloorAndCeilSectionWidth, fCeilAlpha -= fFloorAndCeilAlphaStep)
	{
		sf::RectangleShape rectFloor(sf::Vector2f(mWW, fFloorAndCeilSectionWidth));
		rectFloor.setPosition(0, i);
		rectFloor.setFillColor(sf::Color(color.r, color.g, color.b, fCeilAlpha));

		window.draw(rectFloor);
	}
}

void collisionDetection(sf::Vector2f& playerPos, std::vector<Ray>& collisionRays, float fDT, float fCollisionRayStep, float fAngle,
	float fPlayerSpeed, float fCollisionDistance)
{
	float fCollisionRayAlpha = fAngle - mPi;

	for (int i = 0; i < collisionRays.size(); ++i, fCollisionRayAlpha += fCollisionRayStep)
		if (collisionRays[i].fDist <= fCollisionDistance)
		{
			playerPos.y -= sin(fCollisionRayAlpha) * fPlayerSpeed * fDT;
			playerPos.x -= cos(fCollisionRayAlpha) * fPlayerSpeed * fDT;
		}
}

void drawWalls(sf::RenderWindow& window, std::vector<Ray>& rays, float fAngle, float fFovHalf, float fRayStep,
	float fDistToProjectionPlane, float fShadeIntensity, float fShadeMultiplier, int iWallHCoeff, int iWallSliceW)
{
	float fFovAlpha = -fFovHalf;

	for (int i = 0; i < rays.size(); ++i, fFovAlpha += fRayStep)
	{
		float fDist = cos(fFovAlpha) * rays[i].fDist;
		float fWallH = iWallHCoeff * 2 / fDist * fDistToProjectionPlane;

		int iAlphaChannel = 255 * (fShadeIntensity / fDist * fShadeMultiplier);
		if (iAlphaChannel > 200)
			iAlphaChannel = 200;
		
		sf::RectangleShape slice(sf::Vector2f(iWallSliceW, fWallH));
		slice.setFillColor(sf::Color(rays[i].color.r, rays[i].color.g, rays[i].color.b, iAlphaChannel));
		slice.setPosition(i * iWallSliceW, mWHHalf - fWallH / 2);
		
		window.draw(slice);
	}
}

float determinant(sf::Vector2f& p1, sf::Vector2f& p2, sf::Vector2f& p3)
{
	return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

IntersectionResult intersection(std::vector<sf::Vector2f> line1, std::vector<sf::Vector2f> line2)
{
	float fZ1 = determinant(line1[0], line1[1], line2[0]);
	float fZ2 = determinant(line1[0], line1[1], line2[1]);
	
	if (zer::athm::sign(fZ1) == zer::athm::sign(fZ2))
		return IntersectionResult();

	float iZ3 = determinant(line2[0], line2[1], line1[0]);
	float iZ4 = determinant(line2[0], line2[1], line1[1]);

	if (zer::athm::sign(iZ3) == zer::athm::sign(iZ4))
		return IntersectionResult();
	
	float fY = line2[0].y + (line2[1].y - line2[0].y) * fabs(fZ1) / fabs(fZ2 - fZ1);
	float fX = line2[0].x + (line2[1].x - line2[0].x) * fabs(fZ1) / fabs(fZ2 - fZ1);

	return IntersectionResult(fY, fX);
}

void createCircleShape(std::vector<Boundary>& boundaries, float fY, float fX, float fRadius, int iVertexCount, sf::Color color)
{
	float fAngleStep = mPi * 2 / iVertexCount;
	float fLastY = fY + sin(0) * fRadius;
	float fLastX = fX + cos(0) * fRadius;
	float fAngle = fAngleStep;
	float fColorFlucCoeff = 0.5;

	for (; fAngle < mPi * 2; fAngle += fAngleStep)
	{
		float fTempY = fY + sin(fAngle) * fRadius;
		float fTempX = fX + cos(fAngle) * fRadius;

		boundaries.push_back(Boundary{
			std::vector<sf::Vector2f>({
				sf::Vector2f(fLastX, fLastY),
				sf::Vector2f(fTempX, fTempY)
			}),
			color
		});

		color.r *= fColorFlucCoeff;
		color.g *= fColorFlucCoeff;
		color.b *= fColorFlucCoeff;

		fColorFlucCoeff = fColorFlucCoeff == 0.5 ? 2.0 : 0.5;

		fLastY = fTempY;
		fLastX = fTempX;
	}

	boundaries.push_back(Boundary{
		std::vector<sf::Vector2f>({
			sf::Vector2f(fLastX, fLastY),
			sf::Vector2f(fX + cos(0) * fRadius, fY + sin(0) * fRadius)
		}),
		color
	});
}

void mouseControl(sf::RenderWindow& window, float& fAngle, float fDT, float fSensitivity)
{
	float fDY = sf::Mouse::getPosition(window).y - mWHHalf;
	float fDX = sf::Mouse::getPosition(window).x - mWWHalf;

	fAngle += fDX * fSensitivity * fDT;
	
	sf::Mouse::setPosition(sf::Vector2i(mWWHalf, mWHHalf), window);
}

void keyControl(sf::Vector2f& playerPos, float& fAngle, float fDT, float fPlayerRotateSpeed, float fPlayerSpeed)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		fAngle -= fPlayerRotateSpeed * fDT;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		fAngle += fPlayerRotateSpeed * fDT;

	float fSinAlpha = sin(fAngle) * fPlayerSpeed * fDT;
	float fCosAlpha = cos(fAngle) * fPlayerSpeed * fDT;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		playerPos.y += fSinAlpha;
		playerPos.x += fCosAlpha;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		playerPos.y -= fSinAlpha;
		playerPos.x -= fCosAlpha;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		playerPos.y -= fCosAlpha;
		playerPos.x += fSinAlpha;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		playerPos.y += fCosAlpha;
		playerPos.x -= fSinAlpha;
	}
}

void displayConsoleInformation(std::map<std::string, float>& cfg)
{
	system("cls");

	std::cout << "# " << mTitle << " #" << std::endl;
	std::cout << "\n[!] keyboard buttons for control:" << std::endl;
	std::cout << "\t [ ESC ] - exit;" << std::endl;
	std::cout << "\t [ R ] - restart;" << std::endl;
	std::cout << "\n[!] note: visit a \"" << mConfigPath << "\" file to change configuration;" << std::endl;
	std::cout << "\n[!] current configuration:" << std::endl;
	
	for (std::map<std::string, float>::iterator p = cfg.begin(); p != cfg.end(); p++)
		std::cout << "\t" << p -> first << " = " << p -> second << ";" << std::endl;
}

std::map<std::string, float> readConfig(std::string sConfigPath)
{
	std::map<std::string, float> cfg;

	zer::File file(sConfigPath);
	file.read({zer::file::Modifier::lines});

	for (int i = 0; i < file.linesLen(); ++i)
	{
		std::string sLine = file.lineAt(i);
		if (sLine.find(" = ") != std::string::npos)
		{
			std::vector<std::string> lineParts = zer::athm::split(sLine, " = ");
			cfg[lineParts[0]] = stof(lineParts[1]);
		}
	}

	return cfg;
}

EVENT_CODE eventListener(sf::RenderWindow& window)
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			return CLOSE_EVENT_CODE;
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape)
				return CLOSE_EVENT_CODE;
			if (event.key.code == sf::Keyboard::R)
				return RESTART_EVENT_CODE;
			if (event.key.code == sf::Keyboard::Space)
				return SHOW_MOUSE_EVENT_CODE;
		}
	}
	return NULL_EVENT_CODE;
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(mWW, mWH), mTitle);
	window.setMouseCursorVisible(false);

	sf::Mouse::setPosition(sf::Vector2i(mWWHalf, mWHHalf), window);

	return init(window);
}