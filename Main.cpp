#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <random>
#include <chrono>
#include <fstream>
#include <sstream>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

//global common variables
int cellswidth = 30;
int cellsheight = 20;
int cellsize = 16;
int width = cellsize * cellswidth; //480
int height = cellsize * cellsheight; //320
int dir = 2;  //default dir (right)
int num = 4;  //number of segments

//struct for a snake with max size 100
struct Snake{
    int x,y;
} s[100];

struct Fruct{
    int x,y;
} f;

float timer = 0;
float delay = 0.2;
sf::Clock cl;

int score = -1;
enum game_states {SPLASH, MENU, GAME, END_GAME};
int state = SPLASH;
std::vector<int> vhiscores;

#include "Global.h"
#include "CSprite.h"
#include "Background.h"
#include "GameEngine.h"

//class variables
GameEngine *pGame;

//functions
void NewGame();

bool GameInitialize()
{
    pGame = new GameEngine("Snake",width,height);
    if(pGame == nullptr) return false;

    pGame->SetFrameRate(30);

    return true;
}

void GameStart()
{
    pGame->loadAssets();
    pGame->setVolume("awe", 25.f);
    pGame->setVolume("techno", 25.f);
    pGame->playMusic("awe",true);

    sf::FloatRect rcBounds(0,0,pGame->GetWidth(), pGame->GetHeight());

    ReadHiScores(vhiscores);
}

void GameEnd()
{
    WriteHiScores(vhiscores);
    pGame->stopMusic("awe");
    pGame->stopMusic("techno");

    pGame->CleanupAll();
    pGame->window.close();
    delete pGame;
}

void GameActivate()
{
    if(state == GAME)
        pGame->continueMusic("techno");
    else
        pGame->continueMusic("awe");
}

void GameDeactivate()
{
    if(state==GAME)
        pGame->pauseMusic("techno");
    else
        pGame->pauseMusic("awe");
}

void GamePaint(sf::RenderWindow &window)
{
    window.clear();

    switch(state)
    {
    case SPLASH:
        pGame->showTexture("start",0,0, window);
        break;
    case MENU:
        {
            pGame->showTexture("menu",0,0, window);

            //show hi scores
            std::string histr="HI-SCORES\n";
            for(int i=0;i<5;i++)
            {
                histr = histr + "     " + std::to_string(vhiscores[i]) + "\n";
            }
            pGame->Text(histr, 200, 100, sf::Color::Cyan, 20, "font", window);
            break;
        }
    case GAME:
        {
            //draw the map
            for(int i=0;i<cellswidth;i++)
            {
                for(int j=0;j<cellsheight;j++)
                {
                    pGame->showTexture("white", i*cellsize, j*cellsize, window);
                }
            }

            //draw the snake
            for(int i=0;i<num;i++)
            {
                pGame->showTexture("green", s[i].x*cellsize, s[i].y*cellsize, window);
            }

            //draw the fruit
            pGame->showTexture("red", f.x*cellsize, f.y*cellsize, window);
            break;
        }
    case END_GAME:
        {
            pGame->showTexture("end",0,0,window);
            break;
        }
    default:
        break;
    }
    window.display();
}

void GameCycle(sf::Time delta)
{
    if( state == GAME )
    {
        float time = cl.getElapsedTime().asSeconds();
        cl.restart();
        timer+= time;

        ///////Tick//////
        if (timer>delay)
        {
            //move the snake blocks to occupy the precedent block
            for (int i=num;i>0;--i)
            {
                s[i].x = s[i-1].x;
                s[i].y = s[i-1].y;
            }

            //move the head in the correct direction
            if (dir==0) s[0].y+=1;
            if (dir==1) s[0].x-=1;
            if (dir==2) s[0].x+=1;
            if (dir==3) s[0].y-=1;

            //if the head hit the fruit the snakes grows
            //and a new fruit appears
            if ((s[0].x==f.x) && (s[0].y==f.y))
              { num++; pGame->playSound("eat");
                f.x=rand() % cellswidth;
                f.y=rand() % cellsheight; }

            //if the snake hit the bounds
            if (s[0].x>cellswidth-1) s[0].x=0;
            if (s[0].x<0) s[0].x=cellswidth-1;
            if (s[0].y>cellsheight-1) s[0].y=0;
            if (s[0].y<0) s[0].y=cellsheight-1;

            //if the head hit the body then we lose
            for (int i=1;i<num;i++)
             if (s[0].x==s[i].x && s[0].y==s[i].y)
             {
                 state = END_GAME;
                 pGame->playSound("fail");
                 pGame->stopMusic("techno");
             }

            timer = 0;
        }

        //restore default values
        delay=0.3;
    }
}

void HandleKeys()
{
    switch(state)
    {
    case SPLASH:
        {
            if( pGame->KeyPressed(sf::Keyboard::Space)) state = MENU;
            break;
        }
    case MENU:
        {
            if( pGame->KeyPressed(sf::Keyboard::S) )
            {
                state = GAME;
                NewGame();
            }
            break;
        }
    case GAME:
        {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) dir=1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))  dir=2;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) dir=3;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) dir=0;
        break;
        }
    case END_GAME:
        {
            if( pGame->KeyPressed(sf::Keyboard::M) )
            {
                state = MENU;
                pGame->playMusic("awe",true);
            }
            break;
        }
    default:
        break;
    }
}

void MouseButtonDown(int x, int y, bool bLeft)
{
}

void MouseButtonUp(int x, int y, bool bLeft)
{

}

void MouseMove(int x, int y)
{

}

bool SpriteCollision(CSprite* pSpriteHitter, CSprite* pSpriteHittee)
{
    return false;
}

void SpriteDying(CSprite* pSprite)
{

}

void NewGame()
{
    score = 0;
    dir = 2;
    num = 4;

    //reset the snake
    for(int i=0;i<100;i++)
    {
        s[i].x = 0;
        s[i].y = 0;
    }
    s[0].x = 4;
    s[1].x = 3;
    s[2].x = 2;
    s[3].x = 1;

    //initial fruit
    f.x = 10;
    f.y = 10;

    pGame->stopMusic("awe");
    pGame->playMusic("techno",true);
}
