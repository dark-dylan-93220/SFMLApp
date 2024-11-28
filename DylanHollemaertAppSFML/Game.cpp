#include "Game.h"
#pragma comment(lib, "User32.lib") // Pour la commande ShowWindow() qui r�duit la fen�tre.

// Global variables
namespace {
	// Indexes
	int DVDColorIndex = 0;
	// DVD Logo App
	float f_xVelocity = 4.f;
	float f_yVelocity = 4.f;
	// Tilebreaker
	float f_TBK_xVelocity = 5.f;
	float f_TBK_yVelocity = 5.f;
	const int Y_TILES_GAP_TBK = 20;
	// Pong
	int scoreLeftPlayer = 0;
	int scoreRightPlayer = 0;
	// Useful colors
	const sf::Color LIGHT_GRAY = sf::Color(128, 128, 128);
	const sf::Color DARK_GRAY = sf::Color(50, 50, 50);
	const sf::Color DARKEST_GRAY = sf::Color(32, 32, 32);
}

// Converts float numbers to nanoseconds time duration
static std::chrono::nanoseconds durationToDuration(const float& time_s) {
	return std::chrono::round<std::chrono::nanoseconds>(std::chrono::duration<float>{ time_s });
}

// Constructor / Destructor
Game::Game() :
	window(new sf::RenderWindow(sf::VideoMode(1200, 715), "App SFML", sf::Style::None)),
	m_isRunning(true)
{
	initFonts();
	initTextures();
	initTexts();
	window->setVerticalSyncEnabled(true);
	t0 = std::chrono::high_resolution_clock::now();
	t1 = t0;

}
Game::~Game() {}

// Initialization
void Game::initFonts() {
	Impact.loadFromFile("Assets/Fonts/impact.ttf");
	Arial.loadFromFile("Assets/Fonts/arial.ttf");
	Comic.loadFromFile("Assets/Fonts/comic.ttf");
	GoodTiming.loadFromFile("Assets/Fonts/goodTiming.otf");
}
void Game::initTextures() {
	// Load from file
	// --- "Songs" --- //
	mus_megalovania.openFromFile("Assets/Songs/megalovania.ogg");
	TBKTileBroken.openFromFile("Assets/Songs/TileBreaker/broken_tile.mp3");
	TBKTileBroken.setPlayingOffset(sf::Time(sf::milliseconds(0)));
	// --- "Images" --- //
	appIcon.loadFromFile("Assets/Images/Icons/icon.png");
	logoDVD.loadFromFile("Assets/Images/Logos/Logo_DVD.png");
	smallAppIcon.loadFromFile("Assets/Images/Icons/iconSmall.png");
	closeWindowIcon.loadFromFile("Assets/Images/Icons/closeWindowIcon.png");
	minimizeWindowIcon.loadFromFile("Assets/Images/Icons/minimizeWindowIcon.png");

	// App's icon in the taskbar
	window->setIcon(appIcon.getSize().x, appIcon.getSize().y, appIcon.getPixelsPtr());
	// App's icon in the top left corner
	smallAppIconSpr.setTexture(smallAppIcon);
	smallAppIconSpr.setPosition(5.f, 5.f);
	// Minimize window icon
	minimizeWindowIconSpr.setTexture(minimizeWindowIcon);
	minimizeWindowIconSpr.setPosition(window->getSize().x - (2 * 24.f + 5), 7.f);
	// Close window icon
	closeWindowIconSpr.setTexture(closeWindowIcon);
	closeWindowIconSpr.setPosition(window->getSize().x - 23.f, 8.f);

	// Colors init
	sf::Color col1(255, 0, 0); sf::Color col2(255, 128, 0); sf::Color col3(255, 255, 0);
	sf::Color col4(128, 255, 0); sf::Color col5(0, 255, 0); sf::Color col6(0, 255, 128);
	sf::Color col7(0, 255, 255); sf::Color col8(0, 128, 255); sf::Color col9(0, 0, 255);
	sf::Color col10(128, 0, 255); sf::Color col11(255, 0, 255); sf::Color col12(255, 0, 128);
	colors = {
		col1, col2, col3, col4, col5, col6, col7, col8, col9, col10, col11, col12
	};

	// Shapes init
	// Window boundaries (makes user experience better)
	windowBounds.setPosition(1.f, 1.f);
	windowBounds.setSize(sf::Vector2f((float)window->getSize().x - 2.f, (float)window->getSize().y - 2.f));
	windowBounds.setFillColor(sf::Color::Transparent);
	windowBounds.setOutlineThickness(1.f);
	windowBounds.setOutlineColor(sf::Color::Black);
	// Topbar
	CustomTitleBarBG.setOrigin(sf::Vector2f(0.f, 0.f));
	CustomTitleBarBG.setSize(sf::Vector2f((float)window->getSize().x, m_topBarHeight));
	CustomTitleBarBG.setFillColor(sf::Color::White);
	// Topbar buttons
	// --- Exit cross
	windowExitCross.setPosition(window->getSize().x - (30.f), 0);
	windowExitCross.setSize(sf::Vector2f(30.f, 30.f));
	windowExitCross.setFillColor(sf::Color::White);
	windowExitCrossRect.setPosition(window->getSize().x - 30.f, 0.f);
	windowExitCrossRect.setSize(sf::Vector2f(28.f, 30.f));
	windowExitCrossRect.setFillColor(sf::Color::Transparent);
	windowExitCrossRect.setOutlineThickness(1.f);
	windowExitCrossRect.setOutlineColor(sf::Color::Black);
	// --- Reduce line
	windowReduceLine.setPosition(window->getSize().x - (2 * 30.f), 0.f);
	windowReduceLine.setSize(sf::Vector2f(30.f, 30.f));
	windowReduceLine.setFillColor(sf::Color::White);
	windowReduceLineRect.setPosition(window->getSize().x - 29.5f, -2.f);
	windowReduceLineRect.setSize(sf::Vector2f(28.f, 30.f));
	windowReduceLineRect.setFillColor(sf::Color::Transparent);
	windowReduceLineRect.setOutlineThickness(1.f);
	windowReduceLineRect.setOutlineColor(sf::Color::Black);

	// Main application textures
	sprLogoDVD.setTexture(logoDVD);
	sprLogoDVD.scale(.4f, .4f);
	sprLogoDVD.setPosition(sf::Vector2f( // Essentially placing it in the middle of the window
		((float)((window->getSize().x) / 2) - (float)((sprLogoDVD.getGlobalBounds().width) / 2)),
		((float)((window->getSize().y) / 2) - (float)((sprLogoDVD.getGlobalBounds().height) / 2))
	));

	// Paint mode textures
	paintWhiteBoard.setSize(sf::Vector2f(1000, 563));
	paintWhiteBoard.setFillColor(sf::Color::White);
	paintWhiteBoard.setPosition(sf::Vector2f((window->getSize().x / 2) - (paintWhiteBoard.getSize().x / 2), ((window->getSize().y / 2) - (paintWhiteBoard.getSize().y / 2)) + m_topBarHeight + 15.f)); // 30.f palette
	paintWhiteBoard.setOutlineThickness(1.f);
	paintWhiteBoard.setOutlineColor(sf::Color::Black);
	paintTopBarPalette.setSize(sf::Vector2f(paintWhiteBoard.getSize().x, 60.f));
	paintTopBarPalette.setPosition(sf::Vector2f((window->getSize().x / 2) - (paintWhiteBoard.getSize().x / 2), 60.f));
	paintTopBarPalette.setOutlineThickness(1.f);
	paintTopBarPalette.setOutlineColor(sf::Color::Black);
	paintTopBarPalette.setFillColor(sf::Color::White);

	// Pong mode textures
	pongPlayBall.setRadius(15.f);
	pongPlayBall.setPosition(sf::Vector2f((float)(window->getSize().x / 2) - (float)(pongPlayBall.getRadius()), (float)(window->getSize().y / 2) - (float)(pongPlayBall.getRadius())));
	pongPlayBall.setOutlineThickness(1.f);
	pongPlayBall.setOutlineColor(sf::Color(DARK_GRAY));
	pongMiddleLine.setSize(sf::Vector2f(3, window->getSize().y - m_topBarHeight - 60));
	pongMiddleLine.setPosition(sf::Vector2f(window->getSize().x / 2 - pongMiddleLine.getSize().x / 2, 90));
	pongMiddleLine.setFillColor(sf::Color(DARK_GRAY));
	pongPlayerRacket.setSize(sf::Vector2f(30, 150));
	pongPlayerRacket.setOutlineColor(sf::Color(DARK_GRAY));
	pongOpponentRacket.setSize(sf::Vector2f(30, 150));
	pongOpponentRacket.setOutlineColor(sf::Color(DARK_GRAY));
	pongPlayerRacket.setFillColor(sf::Color::White);
	pongOpponentRacket.setFillColor(sf::Color::White);
	pongPlayerRacket.setPosition(sf::Vector2f(50, (window->getSize().y / 2) - (pongPlayerRacket.getSize().y / 2) + 15));
	pongOpponentRacket.setPosition(sf::Vector2f(window->getSize().x - pongOpponentRacket.getSize().x - 50, (window->getSize().y / 2) - (pongOpponentRacket.getSize().y / 2) + 15));
	pongScoreBox.setSize(sf::Vector2f(200, 60));
	pongScoreBox.setOutlineThickness(3.f);
	pongScoreBox.setOutlineColor(sf::Color(DARK_GRAY));
	pongScoreBox.setFillColor(sf::Color(DARK_GRAY));
	pongScoreBox.setPosition(sf::Vector2f((window->getSize().x / 2) - (pongScoreBox.getSize().x / 2), 30));

	// Tilebreaker mode textures
	TBKBall.setRadius(10.f);
	TBKBall.setPosition((float)(window->getSize().x / 2) - TBKBall.getRadius(), (float)window->getSize().y - 150.f);
	TBKBall.setOutlineThickness(2.f);
	TBKBall.setOutlineColor(sf::Color::Black);
	TBKBall.setFillColor(sf::Color::White);
	TBKScene.setSize(sf::Vector2f(window->getSize().y - m_topBarHeight + 4, window->getSize().y - m_topBarHeight));
	TBKScene.setPosition(sf::Vector2f(((float)(window->getSize().x - window->getSize().y) / 2) - 2, m_topBarHeight));
	TBKScene.setFillColor(sf::Color(DARK_GRAY));
	TBKScene.setOutlineThickness(1.f);
	TBKScene.setOutlineColor(sf::Color::Black);
	TBKPaddle.setSize(sf::Vector2f(100, 20));
	TBKPaddle.setFillColor(sf::Color::White);
	TBKPaddle.setOutlineThickness(1.f);
	TBKPaddle.setOutlineColor(sf::Color::Black);
	TBKPaddle.setPosition((float)(window->getSize().x - TBKPaddle.getSize().x) / 2, (float)window->getSize().y - 80.f);
	// Setup des tuiles
	for (int i = 0; i < 70; ++i) {
		if (i % 7 == 0) tileLife = 7; // More life as the layers go up, starts at 7, -1 every 10 blocks.
		TBKTile.setSize(sf::Vector2f((float)(TBKScene.getSize().x / 10.f), 50.f));
		TBKTile.setOutlineThickness(1.f);
		TBKTile.setOutlineColor(sf::Color::Black);
		tilesTbk[i] = TBKTile;
		tilesTbkState[i] = true; // Each tile is declared as "alive" for the setup
		tileLifeTbk[69 - i] = tileLife;
		tilesLifesTbk[i].setFont(GoodTiming);
		tilesLifesTbk[i].setCharacterSize(45);
		tilesLifesTbk[i].setFillColor(sf::Color(180,180,180));
		tilesLifesTbk[i].setOutlineThickness(1.f);
		tilesLifesTbk[i].setOutlineColor(sf::Color(DARKEST_GRAY));
		tilesLifesTbk[i].setString(std::to_string(tileLife)); // Un truc qui oscille entre 0 et 7 de 0 � 70 donc 0..0..0 ... 1..1..1 ... 7..7..7
		std::cout << "Vie de la case " << i << " : " << tileLife << "HP" << "\n";
		tileLife--;
	}

	// Contextual menu when clicking the top left corner icon
	menuBackground.setSize(sf::Vector2f(118, 210)); // 210 for 7 items of 30px height
	menuBackground.setFillColor(DARK_GRAY);
	menuBackground.setOutlineThickness(1.f);
	menuBackground.setOutlineColor(sf::Color::Black);
	// Choices in that menu ^^^
	menuTextOneRect.setSize(sf::Vector2f(118, 30));   menuTextTwoRect.setSize(sf::Vector2f(118, 30));
	menuTextThreeRect.setSize(sf::Vector2f(118, 30)); menuTextFourRect.setSize(sf::Vector2f(118, 30));
	menuTextFiveRect.setSize(sf::Vector2f(118, 30));  menuTextSixRect.setSize(sf::Vector2f(118, 30));
	menuTextSevenRect.setSize(sf::Vector2f(118, 30));
	menuTextOneRect.setFillColor(DARK_GRAY);          menuTextTwoRect.setFillColor(DARK_GRAY);
	menuTextThreeRect.setFillColor(DARK_GRAY);        menuTextFourRect.setFillColor(DARK_GRAY);
	menuTextFiveRect.setFillColor(DARK_GRAY);         menuTextSixRect.setFillColor(DARK_GRAY);
	menuTextSevenRect.setFillColor(DARK_GRAY);
}
void Game::initTexts() {
	menuTextOne.setString("Fermer");              menuTextTwo.setString("R�duire");
	menuTextThree.setString("Accueil");           menuTextFour.setString("Paint");
	menuTextFive.setString("Pong");               menuTextSix.setString("Casse briques");
	menuTextSeven.setString("Megalovania");
	menuTextOne.setCharacterSize(17);             menuTextTwo.setCharacterSize(17);
	menuTextThree.setCharacterSize(17);           menuTextFour.setCharacterSize(17);
	menuTextFive.setCharacterSize(17);            menuTextSix.setCharacterSize(17);
	menuTextSeven.setCharacterSize(17);
	menuTextOne.setFont(Impact);                  menuTextTwo.setFont(Impact);
	menuTextThree.setFont(Impact);                menuTextFour.setFont(Impact);
	menuTextFive.setFont(Impact);                 menuTextSix.setFont(Impact);
	menuTextSeven.setFont(Impact);
	menuTextOne.setFillColor(sf::Color::White);   menuTextTwo.setFillColor(sf::Color::White);
	menuTextThree.setFillColor(sf::Color::White); menuTextFour.setFillColor(sf::Color::White);
	menuTextFive.setFillColor(sf::Color::White);  menuTextSix.setFillColor(sf::Color::White);
	menuTextSeven.setFillColor(sf::Color::White);

	menuBackground.setPosition(0.f, 30.f);
	menuTextOneRect.setPosition(0.f, 30.f);
	menuTextTwoRect.setPosition(0.f, 60.f);
	menuTextThreeRect.setPosition(0.f, 90.f);
	menuTextFourRect.setPosition(0.f, 120.f);
	menuTextFiveRect.setPosition(0.f, 150.f);
	menuTextSixRect.setPosition(0.f, 180.f);
	menuTextSevenRect.setPosition(0.f, 210.f);
	menuTextOne.setPosition(10.f, 35.f);
	menuTextTwo.setPosition(10.f, 65.f);
	menuTextThree.setPosition(10.f, 95.f);
	menuTextFour.setPosition(10.f, 125.f);
	menuTextFive.setPosition(10.f, 155.f);
	menuTextSix.setPosition(10.f, 185.f);
	menuTextSeven.setPosition(10.f, 215.f);

	windowTitle.setFont(Arial);
	windowTitle.setCharacterSize(12);
	windowTitle.setFillColor(sf::Color::Black);
	windowTitle.setPosition(30.f, 7.5f);
	windowTitle.setString("App SFML");

	FPString.setFont(Impact);
	FPString.setCharacterSize(20);
	FPString.setFillColor(sf::Color::White);
	FPString.setPosition(5.f, (5.f + CustomTitleBarBG.getSize().y));

	timeSinceStart.setFont(Impact);
	timeSinceStart.setCharacterSize(20);
	timeSinceStart.setFillColor(sf::Color::White);
	timeSinceStart.setPosition(5.f, ((float)window->getSize().y - CustomTitleBarBG.getSize().y));

	pongScoreLeftPlayer.setFont(Impact);
	pongScoreLeftPlayer.setCharacterSize(30);
	pongScoreLeftPlayer.setFillColor(sf::Color::White);
	pongScoreLeftPlayer.setPosition(sf::Vector2f((float)(window->getSize().x / 2) - 30.f, (5.f + CustomTitleBarBG.getSize().y)));
	pongScoreLeftPlayer.setString("0");

	pongScoreRightPlayer.setFont(Impact);
	pongScoreRightPlayer.setCharacterSize(30);
	pongScoreRightPlayer.setFillColor(sf::Color::White);
	pongScoreRightPlayer.setPosition(sf::Vector2f((float)(window->getSize().x / 2) + 7.f, (5.f + CustomTitleBarBG.getSize().y)));
	pongScoreRightPlayer.setString("0");

	pongControlsLeft.setFont(GoodTiming);            pongControlsRight.setFont(GoodTiming);
	pongControlsLeft.setString("Z / S"); pongControlsRight.setString("Up / Down");
	pongControlsLeft.setCharacterSize(30);           pongControlsRight.setCharacterSize(30);
	pongControlsLeft.setPosition(sf::Vector2f(100, ((window->getSize().y - m_topBarHeight) / 2) + (pongControlsLeft.getCharacterSize() / 2)));
	pongControlsRight.setPosition(sf::Vector2f((float)window->getSize().x - 265.f, ((float)(window->getSize().y - m_topBarHeight) / 2) + (pongControlsRight.getCharacterSize() / 2)));
}

// Game loop
void Game::run() {
	std::chrono::steady_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	const float timeStep = 1.f / 60.f; // 60 FPS
	float timeAccumulator = 0.f;

	while (m_isRunning) {
		// --- RUNTIME SECONDS COUNTER --- //
		float f_elapsedTime = Clock.restart().asSeconds();
		t1 += durationToDuration(f_elapsedTime);
		auto dt = 1.e-9 * std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
		int timeDuration = (int)dt;
		timeSinceStart.setString("Time : " + std::to_string(timeDuration) + "s");

		// --- FPS CALCULATION --- //
		timeAccumulator += f_elapsedTime;
		float fps = 1.f / f_elapsedTime;
		// Set the FPS digit precision to 2
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(2) << fps;
		std::string FPSValue = oss.str();
		FPString.setString("FPS : " + FPSValue);

		// Events handling
		pollEvents();

		// Main loop
		update();
		// while (timeAccumulator >= timeStep) { // To get a consistent update time, every 60th of a frame -> 60FPS.
		//     timeAccumulator -= timeStep;
		//     update();
		// }

		// Rendering part
		render(); // Vsync UPS
	}
}

// Get the events happening inside of the window
void Game::pollEvents() {
	while (window->pollEvent(event)) {
		switch (event.type) {
		case sf::Event::Closed:
			std::cout << "Fenetre fermee par l'utilisateur" << std::endl;
			m_isRunning = false;
			window->close();
			break;
			// Key pressed cases
		case sf::Event::KeyPressed:
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				if (mode != "pong" || pongOpponentRacket.getGlobalBounds().top <= 30)
					break;
				pongOpponentRacket.move(sf::Vector2f(0, -10));
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				if (mode != "pong" || pongOpponentRacket.getGlobalBounds().top >= (window->getSize().y - pongOpponentRacket.getSize().y))
					break;
				pongOpponentRacket.move(sf::Vector2f(0, 10));
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) { // E in AZERTY
				mus_megalovania.play();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) { // R in AZERTY
				mus_megalovania.pause();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { // Z en AZERTY
				if (mode != "pong" || pongPlayerRacket.getGlobalBounds().top <= 30)
					break;
				pongPlayerRacket.move(sf::Vector2f(0, -10));
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { // Q en AZERTY
				if (mode != "tilebreaker" || TBKPaddle.getPosition().x <= (TBKScene.getGlobalBounds().left + 2))
					break;
				TBKPaddle.move(sf::Vector2f(-10, 0));
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { // S en AZERTY
				if (mode != "pong" || pongPlayerRacket.getGlobalBounds().top >= (window->getSize().y - pongPlayerRacket.getSize().y))
					break;
				pongPlayerRacket.move(sf::Vector2f(0, 10));
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { // D en AZERTY
				if (mode != "tilebreaker" || TBKPaddle.getPosition().x >= (TBKScene.getGlobalBounds().left + TBKScene.getSize().x - TBKPaddle.getSize().x))
					break;
				TBKPaddle.move(sf::Vector2f(10, 0));
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
				if (showMenu && mode == "default") {
					std::cout << "L'application a ete terminee par l'utilisateur a l'aide de la touche " << sf::Keyboard::getDescription(event.key.scancode).toAnsiString() << "\n";
					m_isRunning = false;
					window->close();
				}
				if (showMenu) {
					showMenu = false;
				}
				else {
					showMenu = true;
				}
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
				if (mode == "pong")
					pongStarted = true;
				if (mode == "tilebreaker") {
					TBKStarted = true;
					TBKPaused = false;
				}
			}
			break;
		// Mouse events
		case sf::Event::MouseMoved:
			if (m_isMouseDragging) {
				if (m_lastDownX >= 0 && m_lastDownX <= window->getSize().x && m_lastDownY >= 0 && m_lastDownY <= m_topBarHeight) // Dragging the window
					window->setPosition(window->getPosition() + sf::Vector2<int>(event.mouseMove.x - m_lastDownX, event.mouseMove.y - m_lastDownY));
			}
			// MENU ICONS
			if (menuTextOneRect.getGlobalBounds().contains((float)event.mouseMove.x, (float)event.mouseMove.y))
				menuTextOneRect.setFillColor(LIGHT_GRAY);
			else // 1
				menuTextOneRect.setFillColor(DARK_GRAY);

			if (menuTextTwoRect.getGlobalBounds().contains((float)event.mouseMove.x, (float)event.mouseMove.y))
				menuTextTwoRect.setFillColor(LIGHT_GRAY);
			else // 2
				menuTextTwoRect.setFillColor(DARK_GRAY);

			if (menuTextThreeRect.getGlobalBounds().contains((float)event.mouseMove.x, (float)event.mouseMove.y))
				menuTextThreeRect.setFillColor(LIGHT_GRAY);
			else // 3
				menuTextThreeRect.setFillColor(DARK_GRAY);

			if (menuTextFourRect.getGlobalBounds().contains((float)event.mouseMove.x, (float)event.mouseMove.y))
				menuTextFourRect.setFillColor(LIGHT_GRAY);
			else // 4
				menuTextFourRect.setFillColor(DARK_GRAY);

			if (menuTextFiveRect.getGlobalBounds().contains((float)event.mouseMove.x, (float)event.mouseMove.y))
				menuTextFiveRect.setFillColor(LIGHT_GRAY);
			else // 5
				menuTextFiveRect.setFillColor(DARK_GRAY);

			if (menuTextSixRect.getGlobalBounds().contains((float)event.mouseMove.x, (float)event.mouseMove.y))
				menuTextSixRect.setFillColor(LIGHT_GRAY);
			else // 6
				menuTextSixRect.setFillColor(DARK_GRAY);

			if (menuTextSevenRect.getGlobalBounds().contains((float)event.mouseMove.x, (float)event.mouseMove.y))
				menuTextSevenRect.setFillColor(LIGHT_GRAY);
			else // 7
				menuTextSevenRect.setFillColor(DARK_GRAY);
			// TOP RIGHT ICONS
			if (windowReduceLine.getGlobalBounds().contains((float)event.mouseMove.x, (float)event.mouseMove.y))
				windowReduceLine.setFillColor(sf::Color(46, 193, 255));
			else // REDUCE
				windowReduceLine.setFillColor(sf::Color::White);
			if (windowExitCross.getGlobalBounds().contains((float)event.mouseMove.x, (float)event.mouseMove.y))
				windowExitCross.setFillColor(sf::Color(255, 67, 67));
			else // CLOSE
				windowExitCross.setFillColor(sf::Color::White);
			break;
			// Mouse clicks
		case sf::Event::MouseButtonPressed:
			if (event.mouseButton.button == sf::Mouse::Right) {
				if (event.mouseButton.x >= 5 && event.mouseButton.x <= 25 && event.mouseButton.y >= 5 && event.mouseButton.y <= 25) {
					showMenu = true;
				}
			}
			else if (event.mouseButton.button == sf::Mouse::Left) {
				if (showMenu) showMenu = !showMenu;
				// "Fermer"
				if (menuTextOneRect.getGlobalBounds().contains((float)event.mouseButton.x, (float)event.mouseButton.y) ||
					windowExitCross.getGlobalBounds().contains((float)event.mouseButton.x, (float)event.mouseButton.y)) {
					std::cout << "Fenetre fermee par l'utilisateur" << std::endl;
					m_isRunning = false;
					window->close();
				}
				// "R�duire"
				else if (menuTextTwoRect.getGlobalBounds().contains((float)event.mouseButton.x, (float)event.mouseButton.y) ||
					windowReduceLine.getGlobalBounds().contains((float)event.mouseButton.x, (float)event.mouseButton.y)) {
					std::cout << "Fenetre reduite par l'utilisateur" << std::endl;
					windowReduceLine.setFillColor(sf::Color::White);
					HWND hwnd = window->getSystemHandle();
					ShowWindow(hwnd, SW_MINIMIZE);
					break;
				}
				// "Accueil"
				else if (menuTextThreeRect.getGlobalBounds().contains((float)event.mouseButton.x, (float)event.mouseButton.y)) {
					std::cout << "L'utilisateur retourne a l'accueil" << std::endl;
					// Code
					mode = "default";
					break;
				}
				// "Peindre"
				else if (menuTextFourRect.getGlobalBounds().contains((float)event.mouseButton.x, (float)event.mouseButton.y)) {
					std::cout << "Peindre execute par l'utilisateur" << std::endl;
					// Code
					mode = "paint";
					render();
					break;
				}
				// "Pong"
				else if (menuTextFiveRect.getGlobalBounds().contains((float)event.mouseButton.x, (float)event.mouseButton.y)) {
					std::cout << "Pong execute par l'utilisateur" << std::endl;
					mode = "pong";
					window->clear();
					break;
				}
				// "Casse briques"
				else if (menuTextSixRect.getGlobalBounds().contains((float)event.mouseButton.x, (float)event.mouseButton.y)) {
					std::cout << "Casse briques execute par l'utilisateur" << std::endl;
					// Code
					mode = "tilebreaker";
					break;
				}
				// "Megalovania"
				else if (menuTextSevenRect.getGlobalBounds().contains((float)event.mouseButton.x, (float)event.mouseButton.y)) {
					std::cout << "It's a beautiful day outside..." << std::endl;
					mode = "fiesta";
					mus_megalovania.play();
					break;
				}
				else
					showMenu = false; // Cacher le menu si on clique ailleurs
			}
			m_lastDownX = event.mouseButton.x;
			m_lastDownY = event.mouseButton.y;
			m_isMouseDragging = true;
			break;
		case sf::Event::MouseButtonReleased:
			m_isMouseDragging = false;
			break;
		default:
			break;
		}
	}
}

// Logic part of the game
void Game::update() {
	if (mode == "default") {
		float xPos = sprLogoDVD.getPosition().x;
		float yPos = sprLogoDVD.getPosition().y;
		sprLogoDVD.move(f_xVelocity, f_yVelocity);
		if (sprLogoDVD.getPosition().x < 0 || sprLogoDVD.getPosition().x >(window->getSize().x - sprLogoDVD.getGlobalBounds().width)) {
			f_xVelocity *= -1;
			++DVDColorIndex;
		}
		if (sprLogoDVD.getPosition().y < m_topBarHeight || sprLogoDVD.getPosition().y >(window->getSize().y - sprLogoDVD.getGlobalBounds().height)) {
			f_yVelocity *= -1;
			++DVDColorIndex;
		}
		if (DVDColorIndex % 12 == 0) DVDColorIndex = 0;
		sprLogoDVD.setColor(colors[DVDColorIndex]);
	} // \brief DVD Logo stuff
	if (mode == "paint") {
		// Code
	}
	if (mode == "pong") {
		float xPos = pongPlayBall.getPosition().x;
		float yPos = pongPlayBall.getPosition().y;
		if (pongStarted)
			pongPlayBall.move(f_xVelocity, f_yVelocity);
		if (pongPlayBall.getPosition().x < 0) {
			// Changement de score (+1 pour le joueur de droite)
			scoreRightPlayer++;
			pongScoreRightPlayer.setString(std::to_string(scoreRightPlayer));
			pongPlayBall.setPosition(sf::Vector2f((float)(window->getSize().x / 2) - (float)(pongPlayBall.getRadius()), (float)(window->getSize().y / 2) - (float)(pongPlayBall.getRadius())));
		}

		if (pongPlayBall.getPosition().x > (window->getSize().x - pongPlayBall.getGlobalBounds().width)) {
			// Changement de score (+1 pour le joueur de gauche)
			scoreLeftPlayer++;
			pongScoreLeftPlayer.setString(std::to_string(scoreLeftPlayer));
			pongPlayBall.setPosition(sf::Vector2f((float)(window->getSize().x / 2) - (float)(pongPlayBall.getRadius()), (float)(window->getSize().y / 2) - (float)(pongPlayBall.getRadius())));
		}
		if ((pongPlayBall.getGlobalBounds().intersects(pongPlayerRacket.getGlobalBounds()) && f_xVelocity < 0)
			|| (pongPlayBall.getGlobalBounds().intersects(pongOpponentRacket.getGlobalBounds()) && f_xVelocity > 0)) {
			f_xVelocity *= -1;
		}
		if (pongPlayBall.getPosition().y < m_topBarHeight || pongPlayBall.getPosition().y >(window->getSize().y - pongPlayBall.getGlobalBounds().height)) {
			f_yVelocity *= -1;
		}
	}
	if (mode == "tilebreaker") {
		std::chrono::steady_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		float xPos = TBKBall.getPosition().x;
		float yPos = TBKBall.getPosition().y;
		if (TBKPaused)
			std::cout << "Paused" << "\n";
		if (TBKStarted)
			TBKBall.move(f_TBK_xVelocity, f_TBK_yVelocity);
		for (int i = 0; i < 70; ++i) {
			if (TBKBall.getGlobalBounds().intersects(tilesTbk[i].getGlobalBounds()) && f_TBK_yVelocity < 0) {
				f_TBK_xVelocity *= -1;
				f_TBK_yVelocity *= -1;
				tilesTbkState[i] = false;
				tilesTbk[i].setPosition(sf::Vector2f(0, 0));
			}
		}
		if ((TBKBall.getGlobalBounds().intersects(TBKPaddle.getGlobalBounds()) && f_TBK_yVelocity > 0) ||
			(TBKBall.getPosition().y <= m_topBarHeight && f_TBK_yVelocity < 0)) {
			f_TBK_yVelocity *= -1;
		}
		if (TBKBall.getPosition().x <= (TBKScene.getGlobalBounds().left + 2)) {
			f_TBK_xVelocity *= -1;
		}
		if (TBKBall.getPosition().x >= (TBKScene.getGlobalBounds().left + TBKScene.getSize().x - (2 * TBKBall.getRadius())))
			f_TBK_xVelocity *= -1;
		if (TBKBall.getPosition().y >= window->getSize().y) {
			TBKStarted = false;
			TBKBall.setPosition((float)(window->getSize().x / 2) - TBKBall.getRadius(), (float)window->getSize().y - 150.f);
		}
	}
}

// Rendering function
void Game::render() {
	// Clear the window to a dark-themed background (#1E1E1E in hex)
	window->clear(DARKEST_GRAY);
	window->draw(FPString);
	window->draw(timeSinceStart);

	// Drawing part "default"
	if (mode == "default") {
		window->draw(sprLogoDVD);
	}
	// Drawing part "paint"
	if (mode == "paint") {
		window->draw(paintTopBarPalette);
		window->draw(paintWhiteBoard);
	}
	// Drawing part "pong"
	if (mode == "pong") {
		if (!pongStarted) {
			window->draw(pongControlsLeft);
			window->draw(pongControlsRight);
		}
		window->draw(pongMiddleLine);
		window->draw(pongScoreBox);
		window->draw(pongScoreLeftPlayer);
		window->draw(pongScoreRightPlayer);
		window->draw(pongPlayBall);
		window->draw(pongPlayerRacket);
		window->draw(pongOpponentRacket);
	}
	// Drawing part "tilebreaker"
	if (mode == "tilebreaker") {
		window->draw(TBKScene);
		window->draw(TBKPaddle);
		int it = 0;
		for (int i = 0; i < 10; ++i) {
			for (int j = 1; j < 8; ++j) {
				if (tilesTbkState[it] == true) { // Only set their position if they're alive
					tilesTbk[it].setPosition(sf::Vector2f((
						(window->getSize().x - window->getSize().y) / 2) + (tilesTbk[i].getSize().x * i),
						((tilesTbk[i].getSize().y + 10) * j)));
					tilesLifesTbk[it].setPosition(sf::Vector2f(tilesTbk[it].getPosition().x + 17.f, tilesTbk[it].getPosition().y - 4.f));
				}
				tilesTbk[it].setFillColor(sf::Color(colors[j-1]));
				if (tilesTbkState[it]) {// Draws the tile only if it's alive
					window->draw(tilesTbk[it]);      // Tile
					window->draw(tilesLifesTbk[it]); // Its HP
				}
				++it;
			}
		}
		cond = false;
		window->draw(TBKBall);
	}

	if (showMenu) {
		window->draw(menuBackground);
		window->draw(menuTextOneRect);
		window->draw(menuTextOne);
		window->draw(menuTextTwoRect);
		window->draw(menuTextTwo);
		window->draw(menuTextThreeRect);
		window->draw(menuTextThree);
		window->draw(menuTextFourRect);
		window->draw(menuTextFour);
		window->draw(menuTextFiveRect);
		window->draw(menuTextFive);
		window->draw(menuTextSixRect);
		window->draw(menuTextSix);
		window->draw(menuTextSevenRect);
		window->draw(menuTextSeven);
	}

	// Last layer elements separated from the other elements to avoid layering problems.
	// These textures show in every mode.
	window->draw(CustomTitleBarBG);
	window->draw(windowTitle);
	window->draw(windowReduceLine);
	window->draw(windowExitCross);
	window->draw(minimizeWindowIconSpr);
	window->draw(closeWindowIconSpr);
	window->draw(smallAppIconSpr);
	window->draw(windowBounds);

	window->display();
}