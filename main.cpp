//added iostream for basic input output.
#include <iostream>
//adding cstdlib for easier general number and datatype conversions.
#include <cstdlib>
//adding ctime for better random food generation.
#include <ctime>
//adding fstream to use file handling methods.
#include <fstream>
//including string to add track names/ status text.
#include <string>
//including sfml graphics to create a gui.
#include <SFML/Graphics.hpp>
//including sfml audio to add background music.
#include <SFML/Audio.hpp>


using namespace std;


//declaring constants for window size of game (updated for a better gui)
const int Tile = 8;
const int Scale = 2;
const int Cell = Tile * Scale;
const int Cols = 30;
const int Rows = 20;

const int Border = Cell;
const int HeadsUpDisplay = 48;
const int WindowWidth = Cols * Cell + Border * 2;
const int WindowHeight = Rows * Cell + Border * 2 + HeadsUpDisplay;
//snake head and body sprite constants.
const int HeadUpCol = 1, HeadUpRow = 3;
const int HeadLeftCol = 2, HeadLeftRow = 3;
const int HeadDownCol = 3, HeadDownRow = 3;
const int HeadRightCol = 4, HeadRightRow = 3;
const int BodyCol = 5, BodyRow = 3;
//tail sprite constants.
const int TailUpCol = 1, TailUpRow = 7;
const int TailLeftCol = 2, TailLeftRow = 7;
const int TailDownCol = 3, TailDownRow = 7;
const int TailRightCol = 4, TailRightRow = 7;


//adding a solid square background.
const int SolidX = 1 * Tile + 1;
const int SolidY = 2 * Tile + 1;
const int SolidSize = 6;

//adding different levels and a background for menu.
const int NumLevels = 3;
string sheetFiles[NumLevels] = { "assets/snake1.png", "assets/snake2.png", "assets/snake3.png" };
string backgroundFiles[NumLevels] = { "assets/grassbackground.jpeg", "assets/sandybackground.jpeg", "assets/dirtbackground.png" };
string levelNames[NumLevels] = { "Grass", "Sand", "Dirt" };
string menuFile = "assets/menubackground.png";

int wallCol[NumLevels] = { 12, 7, 7 };
int wallRow[NumLevels] = { 0, 0, 1 };


const int FruitCount = 3;
int fruitCol[FruitCount] = { 6, 6, 6 };
int fruitRow[FruitCount] = { 3, 0, 4 };
int foodType = 0;

//single looping soundtrack
string trackFile = "assets/soundtrack.ogg";
int musicVolume = 40;
bool musicOn = true;
bool musicSpeedUp = false;


int snakeX[Cols * Rows];
int snakeY[Cols * Rows];
int length = 5;
int dirX = 1, dirY = 0;
int nextDirX = 1, nextDirY = 0;
int lastTailX = 0, lastTailY = 0;



//drawTile draws a sprite at a pixel position in the window.
void drawTile(sf::RenderWindow& window, sf::Sprite& sprite, int TileCol, int TileRow, int Xaxis, int Yaxis) {
    sprite.setTextureRect(sf::IntRect(TileCol * Tile, TileRow * Tile, Tile, Tile));
    sprite.setScale((float)Scale, (float)Scale);
    sprite.setColor(sf::Color::White);
    sprite.setPosition((float)Xaxis, (float)Yaxis);
    window.draw(sprite);
}

//drawSolid draws a colored rectangle using a sprite.
void drawSolid(sf::RenderWindow& window, sf::Sprite& sprite, int Xaxis, int Yaxis, int width, int height, sf::Color color) {
    sprite.setTextureRect(sf::IntRect(SolidX, SolidY, SolidSize, SolidSize));
    sprite.setScale((float)width / SolidSize, (float)height / SolidSize);
    sprite.setColor(color);
    sprite.setPosition((float)Xaxis, (float)Yaxis);
    window.draw(sprite);
}

//converts grid cell size to pixels
int cellXaxis(int x) {
    return Border + x * Cell;
}
int cellYaxis(int y) {
    return Border + y * Cell;
}

//centres a text horizontally in the window at height y.
void CenterText(sf::Text& text, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setPosition((WindowWidth - bounds.width) / 2.f - bounds.left, y);
}

//loads the music tracks, loops forever.
void PlayTrack(sf::Music& music) {
    music.stop();
    if (!music.openFromFile(trackFile)) {
        cout << "Music failed to load: " << trackFile << endl;
        return;
    }
    music.setLoop(true);
    music.setPitch(1.0f);
    music.setVolume((float)musicVolume);
    if (musicOn) {
        music.play();
    }
}


void ResetSnake() {
    length = 5;
    snakeX[0] = 10; snakeY[0] = 10; // Starting positions of snake (set length to 5 to make checking for collisions easier)
    snakeX[1] = 9;  snakeY[1] = 10;
    snakeX[2] = 8;  snakeY[2] = 10;
    snakeX[3] = 7;  snakeY[3] = 10;
    snakeX[4] = 6;  snakeY[4] = 10;
    dirX = 1; dirY = 0;
    nextDirX = 1; nextDirY = 0;
}

void MoveSnake() {
    lastTailX = snakeX[length - 1]; // Basically sets the tail position for the snake at every point to make increasing the length easier for MEMBER C
    lastTailY = snakeY[length - 1];

    dirX = nextDirX; //changes the X and Y value according to the inputs of the arrow keys
    dirY = nextDirY;

    for (int i = length - 1;i > 0;i--) {
        snakeX[i] = snakeX[i - 1]; // Sets the new positions for the snake from the same array
        snakeY[i] = snakeY[i - 1];

    }
    snakeX[0] += dirX; // sets new position for the head
    snakeY[0] += dirY;

}
void UndoMove() { //To make game look cleaner it shows as if you hit the wall, basicaly undos the last move
    for (int i = 0; i < length - 1;i++) {
        snakeX[i] = snakeX[i + 1];
        snakeY[i] = snakeY[i + 1];
    }
    snakeX[length - 1] = lastTailX;
    snakeY[length - 1] = lastTailY;
}

bool CheckCollision() {
    if ((snakeX[0] < 0 || snakeX[0] >= Cols) || (snakeY[0] < 0 || snakeY[0] >= Rows)) { //Wall Collision checks if the snakes head position is 
        // at the edges then returns true to end the game
        cout << snakeX[0] << " ";
        cout << snakeY[0] << endl;
        return true;
    }


    for (int i = 1; i < length; i++) {
        if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {  // Self collision checks if any of the head position matches the
            // position of any of the values of snake

            return true;
        }
    }
    return false;
}


void PlaceFood(int& foodX, int& foodY) {
    bool found;

    do {
        found = false;

        foodX = rand() % Cols;
        foodY = rand() % Rows;

        for (int i = 0; i < length; i++) {
            if (foodX == snakeX[i] && foodY == snakeY[i]) {
                found = true;
            }
        }

    } while (found);
    foodType = rand() % FruitCount; //picks a random fruit sprite every time a food item is found. 
}

void LoadHighScore(int& highScore) {
    ifstream file("highscore.txt");

    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
}

void SaveHighScore(int highScore) {
    ofstream file("highscore.txt");

    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}

void ResetGame(int& score, bool& gameOver,
    bool& inMenu, int& foodX, int& foodY) {
    ResetSnake();

    score = 0;
    gameOver = false;
    inMenu = false;

    PlaceFood(foodX, foodY);
}


int main() {
    //the game loop

    //rendering a window, with window title and resolution.
    sf::RenderWindow window(sf::VideoMode(WindowWidth, WindowHeight), "Snake Xenia");
    //restricting frame rate so that 100% of processor is not used.
    window.setFramerateLimit(60);


    //loading sprite sheet and one background image for each level.
    sf::Texture sheet[NumLevels];
    sf::Texture background[NumLevels];
    sf::Texture menuTexture;

    for (int i = 0; i < NumLevels; i++) {
        if (!sheet[i].loadFromFile(sheetFiles[i])) {
            cout << sheetFiles[i] << " failed to load \n";
            return 1;
        }
        sheet[i].setSmooth(false);
        if (!background[i].loadFromFile(backgroundFiles[i])) {
            cout << backgroundFiles[i] << " failed to load \n";
            return 1;
        }
    }
    if (!menuTexture.loadFromFile(menuFile)) {
        cout << menuFile << " failed to load \n";
        return 1;
    }

    //sprite for 8x8 tiles.
    sf::Sprite sprite;
    sprite.setTexture(sheet[0]);
    sprite.setScale((float)Scale, (float)Scale);

    //sprite for background image.
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(background[0]);
    backgroundSprite.setPosition((float)Border, (float)Border);

    //sprite for menu image.
    sf::Sprite menuSprite;
    menuSprite.setTexture(menuTexture);
    menuSprite.setPosition(0.f, 0.f);


    //adding music and sound effects to main.
    sf::Music music;
    PlayTrack(music);

    sf::SoundBuffer eatBuffer, overBuffer;
    if (!eatBuffer.loadFromFile("assets/20279__koops__apple_crunch_16.wav")) {
        cout << "20279__koops__apple_crunch_16.wav failed to load \n";
    }
    if (!overBuffer.loadFromFile("assets/gameover.wav")) {
        cout << "gameover.wav failed to load \n";
    }
    sf::Sound eatSound;
    eatSound.setBuffer(eatBuffer);
    sf::Sound overSound;
    overSound.setBuffer(overBuffer);


    int score = 0;
    bool gameOver = false;
    //level variable.
    int selectedLevel = 0;
    int foodX = 15;
    int foodY = 8;

    int highScore = 0;

    srand(time(0));

    ResetSnake();
    LoadHighScore(highScore);
    PlaceFood(foodX, foodY);


    //start menu variable.
    bool inMenu = true;

    //creating a score text and game over screen.
    //initializing the font using loadfromfile built in function.
    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        cout << "Font failed to load\n";
    }

    //position of hud strip below playing field.
    const float HeadsUpDisplayY = (float)(Rows * Cell + Border * 2 + 4);


    //creating a score text using built in functions.
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(22);
    scoreText.setFillColor(sf::Color::Cyan);
    scoreText.setOutlineColor(sf::Color::Black);
    scoreText.setOutlineThickness(1.5f);
    scoreText.setPosition((float)Border, HeadsUpDisplayY);

    //level text in the middle of hud.
    sf::Text levelText;
    levelText.setFont(font);
    levelText.setCharacterSize(22);
    levelText.setFillColor(sf::Color(225, 120, 200));
    levelText.setOutlineColor(sf::Color::Black);
    levelText.setOutlineThickness(1.5f);
    levelText.setPosition(200.f, HeadsUpDisplayY);



    //creating a high score text using built in functions.
    sf::Text highScoretext;
    highScoretext.setFont(font);
    highScoretext.setCharacterSize(22);
    highScoretext.setFillColor(sf::Color(255, 215, 0));
    highScoretext.setOutlineColor(sf::Color::Black);
    highScoretext.setOutlineThickness(1.5f);
    highScoretext.setPosition((float)(WindowWidth - Border - 190), HeadsUpDisplayY);

    //adding a music status line for better gui in the hud
    sf::Text musicText;
    musicText.setFont(font);
    musicText.setCharacterSize(13);
    musicText.setFillColor(sf::Color(210, 210, 210));
    musicText.setPosition((float)Border, HeadsUpDisplayY + 28);


    //creating a gameOver text using built in functions.
    sf::Text overText;
    overText.setFont(font);
    overText.setCharacterSize(12);
    overText.setFillColor(sf::Color::Yellow);
    overText.setOutlineColor(sf::Color::Black);
    overText.setOutlineThickness(2.f);
    overText.setString("GAME OVER - press Enter to restart, press L to return to Menu");
    CenterText(overText, Border + Rows * Cell / 2 - 14.f);



    //creating a start menu texts using built in functions.
    sf::Text titleText;
    titleText.setFont(font);
    titleText.setCharacterSize(36);
    titleText.setFillColor(sf::Color::White);
    titleText.setOutlineColor(sf::Color(20, 80, 30));
    titleText.setOutlineThickness(3.f);
    titleText.setString("SNAKE XENIA");
    titleText.setPosition(212.f, 28.f);

    //for instructions.
    sf::Text menuText;
    menuText.setFont(font);
    menuText.setCharacterSize(18);
    menuText.setFillColor(sf::Color::White);
    menuText.setOutlineColor(sf::Color::Black);
    menuText.setOutlineThickness(2.f);
    menuText.setString("Press 1, 2 or 3 for Level | Enter to Play, Esc to Quit");
    CenterText(menuText, 172.f);

    sf::Text levelSelectText;
    levelSelectText.setFont(font);
    levelSelectText.setCharacterSize(18);
    levelSelectText.setFillColor(sf::Color(255, 215, 0));
    levelSelectText.setOutlineColor(sf::Color::Black);
    levelSelectText.setOutlineThickness(1.5f);

    //adding music controls.
    sf::Text controlsText;
    controlsText.setFont(font);
    controlsText.setCharacterSize(12);
    controlsText.setFillColor(sf::Color(235, 235, 235));
    controlsText.setOutlineColor(sf::Color::Black);
    controlsText.setOutlineThickness(1.5f);
    controlsText.setString("M: Mute, [ ]: Volume up and down, P: Music speeds up with score");
    CenterText(controlsText, 230.f);


    //gui panel shapes for cleaner contrast on hud and overlays.
    sf::RectangleShape hudPanel;
    hudPanel.setSize(sf::Vector2f((float)WindowWidth, (float)HeadsUpDisplay));
    hudPanel.setPosition(0.f, (float)(Rows * Cell + Border * 2));
    hudPanel.setFillColor(sf::Color(18, 20, 26));

    sf::RectangleShape hudBorderLine;
    hudBorderLine.setSize(sf::Vector2f((float)WindowWidth, 2.f));
    hudBorderLine.setPosition(0.f, (float)(Rows * Cell + Border * 2));
    hudBorderLine.setFillColor(sf::Color(70, 80, 95));

    sf::RectangleShape overlayBox;
    overlayBox.setSize(sf::Vector2f((float)(Cols * Cell - 32), 110.f));
    overlayBox.setPosition((float)Border + 16.f, 155.f);
    overlayBox.setFillColor(sf::Color(15, 18, 24, 215));
    overlayBox.setOutlineColor(sf::Color(255, 215, 0));
    overlayBox.setOutlineThickness(2.f);



    sf::Clock moveClock;



    while (window.isOpen()) {

        //INPUT SECTION:
        sf::Event event;
        //making sure program runs until a specfic event occurs.
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (inMenu == true) {
                    //level controls
                    if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) {
                        selectedLevel = 0;
                    }
                    else if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2) {
                        selectedLevel = 1;
                    }
                    else if (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::Numpad3) {
                        selectedLevel = 2;
                    }
                    else if (event.key.code == sf::Keyboard::Left) {
                        selectedLevel = (selectedLevel + NumLevels - 1) % NumLevels;
                    }
                    else if (event.key.code == sf::Keyboard::Right) {
                        selectedLevel = (selectedLevel + 1) % NumLevels;
                    }
                }
                else {
                    // arrow key inputs for the head of the snake
                    if (event.key.code == sf::Keyboard::Up && nextDirY != 1) {
                        nextDirX = 0;  nextDirY = -1;
                    }
                    else if (event.key.code == sf::Keyboard::Down && nextDirY != -1) {
                        nextDirX = 0;  nextDirY = 1;
                    }
                    else if (event.key.code == sf::Keyboard::Left && nextDirX != 1) {
                        nextDirX = -1; nextDirY = 0;
                    }
                    else if (event.key.code == sf::Keyboard::Right && nextDirX != -1) {
                        nextDirX = 1;  nextDirY = 0;
                    }
                }
                //menu controls
                if (event.key.code == sf::Keyboard::Escape) { // quits the game when pressing esc
                    window.close();
                }
                else if (event.key.code == sf::Keyboard::Enter && (inMenu == true || gameOver == true)) { // when in menu or game over player presses enter it starts by resetting the snake
                    moveClock.restart();
                    ResetGame(score, gameOver, inMenu, foodX, foodY);
                    music.setPitch(1.0f);
                }
                else if (event.key.code == sf::Keyboard::L && gameOver == true) { // go back to menu to change level
                    inMenu = true;
                    gameOver = false;
                }
                //music controls
                else if (event.key.code == sf::Keyboard::M) {   // mute / unmute
                    musicOn = !musicOn;
                    if (musicOn) music.play();
                    else music.pause();
                }
                else if (event.key.code == sf::Keyboard::RBracket) {   // volume up
                    if (musicVolume < 100) musicVolume = musicVolume + 10;
                    music.setVolume((float)musicVolume);
                }
                else if (event.key.code == sf::Keyboard::LBracket) {   // volume down
                    if (musicVolume > 0) musicVolume = musicVolume - 10;
                    music.setVolume((float)musicVolume);
                }
                else if (event.key.code == sf::Keyboard::P) {  // toggle "music gets faster as score rises"
                    musicSpeedUp = !musicSpeedUp;
                    if (musicSpeedUp == false) music.setPitch(1.0f);
                }
            }
        }


        //UPDATE SECTION:
        if (inMenu == false && gameOver == false && moveClock.getElapsedTime().asSeconds() >= 0.15f) {  // built in function used to ensure the snake doesn't move off the screen instantly
            moveClock.restart();
            MoveSnake();
            gameOver = CheckCollision(); // check collision after the movement block 
            if (gameOver == true) {
                UndoMove();
                overSound.play();
                if (score > highScore) {
                    highScore = score;
                    SaveHighScore(highScore);
                }
            }
            else if (snakeX[0] == foodX && snakeY[0] == foodY) {
                if (length < Cols * Rows) {
                    snakeX[length] = lastTailX;
                    snakeY[length] = lastTailY;

                    length++;
                    score = score + 10;
                    eatSound.play();

                    if (musicSpeedUp == true) {
                        float pitch = 1.0f + score / 400.0f;
                        if (pitch > 1.5f) {
                            pitch = 1.5f;
                        }
                        music.setPitch(pitch);
                    }

                    if (score > highScore) {
                        highScore = score;
                        SaveHighScore(highScore);
                    }

                    PlaceFood(foodX, foodY);
                }
            }
        }


        //DRAW SECTION:

        //chosen level
        sprite.setTexture(sheet[selectedLevel]);
        backgroundSprite.setTexture(background[selectedLevel], true);


        //color of window by clearing entire window with single color and displaying it with updates..
        window.clear(sf::Color(20, 20, 20));

        if (inMenu) {
            window.draw(menuSprite);
        }
        else {
            window.draw(backgroundSprite);

            //draw food item at its grid position.
            drawTile(window, sprite, fruitCol[foodType], fruitRow[foodType], cellXaxis(foodX), cellYaxis(foodY));

            //draw snake head, body, and tail.
            for (int i = length - 1; i >= 0; i--) {
                if (i == 0) {
                    if (dirY == -1) {
                        drawTile(window, sprite, HeadUpCol, HeadUpRow, cellXaxis(snakeX[i]), cellYaxis(snakeY[i]));
                    }
                    else if (dirX == -1) {
                        drawTile(window, sprite, HeadLeftCol, HeadLeftRow, cellXaxis(snakeX[i]), cellYaxis(snakeY[i]));
                    }
                    else if (dirY == 1) {
                        drawTile(window, sprite, HeadDownCol, HeadDownRow, cellXaxis(snakeX[i]), cellYaxis(snakeY[i]));
                    }
                    else {
                        drawTile(window, sprite, HeadRightCol, HeadRightRow, cellXaxis(snakeX[i]), cellYaxis(snakeY[i]));
                    }
                }
                else if (i == length - 1) {
                    int tailXaxis = snakeX[i] - snakeX[i - 1];
                    int tailYaxis = snakeY[i] - snakeY[i - 1];
                    if (tailYaxis == -1) {
                        drawTile(window, sprite, TailUpCol, TailUpRow, cellXaxis(snakeX[i]), cellYaxis(snakeY[i]));
                    }
                    else if (tailXaxis == -1) {
                        drawTile(window, sprite, TailLeftCol, TailLeftRow, cellXaxis(snakeX[i]), cellYaxis(snakeY[i]));
                    }
                    else if (tailYaxis == 1) {
                        drawTile(window, sprite, TailDownCol, TailDownRow, cellXaxis(snakeX[i]), cellYaxis(snakeY[i]));
                    }
                    else {
                        drawTile(window, sprite, TailRightCol, TailRightRow, cellXaxis(snakeX[i]), cellYaxis(snakeY[i]));
                    }
                }
                else {
                    drawTile(window, sprite, BodyCol, BodyRow, cellXaxis(snakeX[i]), cellYaxis(snakeY[i]));
                }
            }
        }

        for (int x = 0; x < Cols + 2; x++) {
            drawTile(window, sprite, wallCol[selectedLevel] + (x % 2), wallRow[selectedLevel], x * Cell, 0);
            drawTile(window, sprite, wallCol[selectedLevel] + (x % 2), wallRow[selectedLevel], x * Cell, (Rows + 1) * Cell);
        }
        for (int y = 1; y <= Rows; y++) {
            drawTile(window, sprite, wallCol[selectedLevel] + (y % 2), wallRow[selectedLevel], 0, y* Cell);
            drawTile(window, sprite, wallCol[selectedLevel] + (y % 2), wallRow[selectedLevel], (Cols + 1) * Cell, y * Cell);
        }

        if (inMenu) {
            CenterText(titleText, 32.f);
            window.draw(titleText);

            overlayBox.setSize(sf::Vector2f((float)(Cols * Cell - 32), 105.f));
            overlayBox.setPosition((float)Border + 16.f, 165.f);
            window.draw(overlayBox);

            CenterText(menuText, 168.f);

            string levelPrompt = "Level: ";
            for (int i = 0; i < NumLevels; i++) {
                if (i == selectedLevel) {
                    levelPrompt = levelPrompt + "[" + to_string(i + 1) + ". " + levelNames[i] + "] ";
                }
                else {
                    levelPrompt = levelPrompt + to_string(i + 1) + ". " + levelNames[i] + " ";
                }
            }
            levelSelectText.setString(levelPrompt);
            CenterText(levelSelectText, 204.f);

            CenterText(controlsText, 240.f);
            window.draw(menuText);
            window.draw(levelSelectText);
            window.draw(controlsText);
        }

        //creating a gameover screen(only runs if true)
        if (gameOver) {
            overlayBox.setSize(sf::Vector2f((float)(Cols * Cell - 60), 66.f));
            overlayBox.setPosition((float)Border + 30.f, Border + Rows * Cell / 2 - 28.f);
            window.draw(overlayBox);

            CenterText(overText, Border + Rows * Cell / 2 - 14.f);
            window.draw(overText);
        }

        //draw hud panel and divider so text stays crisp over any background.
        window.draw(hudPanel);
        window.draw(hudBorderLine);

        //using built in functions to set score and draw the score text and the high score.
        scoreText.setString("Score: " + to_string(score));
        highScoretext.setString("High Score: " + to_string(highScore));
        levelText.setString("Level " + to_string(selectedLevel + 1));
        CenterText(levelText, HeadsUpDisplayY);
        window.draw(scoreText);
        window.draw(levelText);
        window.draw(highScoretext);


        string musicStatus;
        if (musicOn) {
            musicStatus = "Music: ON Vol " + to_string(musicVolume);
        }
        else {
            musicStatus = "Music: OFF";
        }
        musicStatus = musicStatus + " M Mute, [ ] Vol, P speed-up: ";
        if (musicSpeedUp) {
            musicStatus = musicStatus + "ON";
        }
        else {
            musicStatus = musicStatus + "OFF";
        }
        musicText.setString(musicStatus);
        window.draw(musicText);

        window.display();
    }
    return 0;
}