#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_net.h>
#include <SDL_ttf.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <random>
//#include <SDL_gpu.h>
//#include <SFML/Network.hpp>
//#include <SFML/Graphics.hpp>
#include <algorithm>
#include <atomic>
#include <codecvt>
#include <functional>
#include <locale>
#include <mutex>
#include <thread>
#include <array>
#ifdef __ANDROID__
#include "vendor/PUGIXML/src/pugixml.hpp"
#include <android/log.h> //__android_log_print(ANDROID_LOG_VERBOSE, "AutumnLeafCollector", "Example number log: %d", number);
#include <jni.h>
#else
#include <filesystem>
#include <pugixml.hpp>
#ifdef __EMSCRIPTEN__
namespace fs = std::__fs::filesystem;
#else
namespace fs = std::filesystem;
#endif
using namespace std::chrono_literals;
#endif
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

// NOTE: Remember to uncomment it on every release
//#define RELEASE

#if defined _MSC_VER && defined RELEASE
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

//240 x 240 (smart watch)
//240 x 320 (QVGA)
//360 x 640 (Galaxy S5)
//640 x 480 (480i - Smallest PC monitor)

#define ROT_INIT_DELAY_IN_MS 5000
#define GRAPE_INIT_SPAWN_DELAY_IN_MS 10000
#define APPLE_INIT_SPAWN_DELAY_IN_MS 5000
#define BANANA_INIT_SPAWN_DELAY_IN_MS 3000
#define CARROT_INIT_SPAWN_DELAY_IN_MS 5000
#define POTATO_INIT_SPAWN_DELAY_IN_MS 7000
#define PUMPKIN_INIT_SPAWN_DELAY_IN_MS 3000
#define SUNSET_HOUR_BEGIN 7
#define SUNSET_HOUR_END 17
#define MAX_ENERGY_INIT 10
#define PLAYER_SPEED 0.1
#define MAX_TREES 3
#define SCREEN_PADDING 50
#define EPSILON 0.001
#define BROWN_COLOR 140, 56, 4, 255
#define GRAY_COLOR 181, 189, 160, 255

#define HOME_BUTTON_SELECTED \
    {                        \
        242, 182, 61, 255    \
    }
#define HOME_BUTTON_UNSELECTED \
    {                          \
        209, 180, 140, 255     \
    }
#define LETTER_WIDTH 25

int windowWidth = 800;
int windowHeight = 600;
SDL_Point mousePos;
SDL_Point realMousePos;
bool keys[SDL_NUM_SCANCODES];
bool buttons[SDL_BUTTON_X2 + 1];
SDL_Window* window;
SDL_Renderer* renderer;
TTF_Font* robotoF;
bool running = true;

std::string prefPath;
SDL_Texture* leavesT;
SDL_Texture* treeT;
SDL_Texture* tree1T;
SDL_Texture* tree2T;
SDL_Texture* tree3T;
SDL_Texture* shopT;
SDL_Texture* backArrowT;
SDL_Texture* buyT;
SDL_Texture* leafWithSprayerT;
SDL_Texture* vineT;
SDL_Texture* grapeT;
SDL_Texture* appleT;
SDL_Texture* appleTreeT;
SDL_Texture* bananaT;
SDL_Texture* bananaTreeT;
SDL_Texture* carrotT;
SDL_Texture* soilT;
SDL_Texture* mutedT;
SDL_Texture* soundT;
SDL_Texture* leafT;
SDL_Texture* sunT;
SDL_Texture* moonT;
SDL_Texture* energyT;
SDL_Texture* rotT;
SDL_Texture* moreEnergyT;
SDL_Texture* houseflyT;
SDL_Texture* wormT;
SDL_Texture* potatoT;
SDL_Texture* pumpkinT;
SDL_Texture* pumpkinTreeT;
SDL_Texture* tradeT;
SDL_Texture* playerT;
SDL_Texture* houseT;
SDL_Texture* collectT;
SDL_Texture* drawT;
SDL_Texture* openT;
SDL_Texture* xT;
SDL_Texture* bedT;
Mix_Music* josephKosmaM;
Mix_Music* antonioVivaldiM;
Mix_Chunk* doorS;
Mix_Chunk* pickupS;
Mix_Chunk* powerupS;
Mix_Chunk* sleepS;

void logOutputCallback(void* userdata, int category, SDL_LogPriority priority, const char* message)
{
    std::cout << message << std::endl;
}

int random(int min, int max)
{
    return min + rand() % ((max + 1) - min);
}

int SDL_QueryTextureF(SDL_Texture* texture, Uint32* format, int* access, float* w, float* h)
{
    int wi, hi;
    int result = SDL_QueryTexture(texture, format, access, &wi, &hi);
    if (w) {
        *w = wi;
    }
    if (h) {
        *h = hi;
    }
    return result;
}

SDL_bool SDL_PointInFRect(const SDL_Point* p, const SDL_FRect* r)
{
    return ((p->x >= r->x) && (p->x < (r->x + r->w)) && (p->y >= r->y) && (p->y < (r->y + r->h))) ? SDL_TRUE : SDL_FALSE;
}

std::ostream& operator<<(std::ostream& os, SDL_FRect r)
{
    os << r.x << " " << r.y << " " << r.w << " " << r.h;
    return os;
}

std::ostream& operator<<(std::ostream& os, SDL_Rect r)
{
    SDL_FRect fR;
    fR.w = r.w;
    fR.h = r.h;
    fR.x = r.x;
    fR.y = r.y;
    os << fR;
    return os;
}

SDL_Texture* renderText(SDL_Texture* previousTexture, TTF_Font* font, SDL_Renderer* renderer, const std::string& text, SDL_Color color)
{
    if (previousTexture) {
        SDL_DestroyTexture(previousTexture);
    }
    SDL_Surface* surface;
    if (text.empty()) {
        surface = TTF_RenderUTF8_Blended(font, " ", color);
    }
    else {
        surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    }
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        return texture;
    }
    else {
        return 0;
    }
}

struct Text {
    std::string text;
    SDL_Surface* surface = 0;
    SDL_Texture* t = 0;
    SDL_FRect dstR{};
    bool autoAdjustW = false;
    bool autoAdjustH = false;
    float wMultiplier = 1;
    float hMultiplier = 1;

    ~Text()
    {
        if (surface) {
            SDL_FreeSurface(surface);
        }
        if (t) {
            SDL_DestroyTexture(t);
        }
    }

    Text()
    {
    }

    Text(const Text& rightText)
    {
        text = rightText.text;
        if (surface) {
            SDL_FreeSurface(surface);
        }
        if (t) {
            SDL_DestroyTexture(t);
        }
        if (rightText.surface) {
            surface = SDL_ConvertSurface(rightText.surface, rightText.surface->format, SDL_SWSURFACE);
        }
        if (rightText.t) {
            t = SDL_CreateTextureFromSurface(renderer, surface);
        }
        dstR = rightText.dstR;
        autoAdjustW = rightText.autoAdjustW;
        autoAdjustH = rightText.autoAdjustH;
        wMultiplier = rightText.wMultiplier;
        hMultiplier = rightText.hMultiplier;
    }

    Text& operator=(const Text& rightText)
    {
        text = rightText.text;
        if (surface) {
            SDL_FreeSurface(surface);
        }
        if (t) {
            SDL_DestroyTexture(t);
        }
        if (rightText.surface) {
            surface = SDL_ConvertSurface(rightText.surface, rightText.surface->format, SDL_SWSURFACE);
        }
        if (rightText.t) {
            t = SDL_CreateTextureFromSurface(renderer, surface);
        }
        dstR = rightText.dstR;
        autoAdjustW = rightText.autoAdjustW;
        autoAdjustH = rightText.autoAdjustH;
        wMultiplier = rightText.wMultiplier;
        hMultiplier = rightText.hMultiplier;
        return *this;
    }

    void setText(SDL_Renderer* renderer, TTF_Font* font, std::string text, SDL_Color c = { 255, 255, 255 })
    {
        this->text = text;
#if 1 // NOTE: renderText
        if (surface) {
            SDL_FreeSurface(surface);
        }
        if (t) {
            SDL_DestroyTexture(t);
        }
        if (text.empty()) {
            surface = TTF_RenderUTF8_Blended(font, " ", c);
        }
        else {
            surface = TTF_RenderUTF8_Blended(font, text.c_str(), c);
        }
        if (surface) {
            t = SDL_CreateTextureFromSurface(renderer, surface);
        }
#endif
        if (autoAdjustW) {
            SDL_QueryTextureF(t, 0, 0, &dstR.w, 0);
        }
        if (autoAdjustH) {
            SDL_QueryTextureF(t, 0, 0, 0, &dstR.h);
        }
        dstR.w *= wMultiplier;
        dstR.h *= hMultiplier;
    }

    void setText(SDL_Renderer* renderer, TTF_Font* font, int value, SDL_Color c = { 255, 255, 255 })
    {
        setText(renderer, font, std::to_string(value), c);
    }

    void draw(SDL_Renderer* renderer)
    {
        if (t) {
            SDL_RenderCopyF(renderer, t, 0, &dstR);
        }
    }
};

int SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int radius)
{
    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius - 1;
    status = 0;

    while (offsety >= offsetx) {
        status += SDL_RenderDrawPoint(renderer, x + offsetx, y + offsety);
        status += SDL_RenderDrawPoint(renderer, x + offsety, y + offsetx);
        status += SDL_RenderDrawPoint(renderer, x - offsetx, y + offsety);
        status += SDL_RenderDrawPoint(renderer, x - offsety, y + offsetx);
        status += SDL_RenderDrawPoint(renderer, x + offsetx, y - offsety);
        status += SDL_RenderDrawPoint(renderer, x + offsety, y - offsetx);
        status += SDL_RenderDrawPoint(renderer, x - offsetx, y - offsety);
        status += SDL_RenderDrawPoint(renderer, x - offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2 * offsetx) {
            d -= 2 * offsetx + 1;
            offsetx += 1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}

int SDL_RenderFillCircle(SDL_Renderer* renderer, int x, int y, int radius)
{
    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius - 1;
    status = 0;

    while (offsety >= offsetx) {

        status += SDL_RenderDrawLine(renderer, x - offsety, y + offsetx,
            x + offsety, y + offsetx);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y + offsety,
            x + offsetx, y + offsety);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y - offsety,
            x + offsetx, y - offsety);
        status += SDL_RenderDrawLine(renderer, x - offsety, y - offsetx,
            x + offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2 * offsetx) {
            d -= 2 * offsetx + 1;
            offsetx += 1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}

struct Clock {
    Uint64 start = SDL_GetPerformanceCounter();

    float getElapsedTime()
    {
        Uint64 stop = SDL_GetPerformanceCounter();
        float secondsElapsed = (stop - start) / (float)SDL_GetPerformanceFrequency();
        return secondsElapsed * 1000;
    }

    float restart()
    {
        Uint64 stop = SDL_GetPerformanceCounter();
        float secondsElapsed = (stop - start) / (float)SDL_GetPerformanceFrequency();
        start = SDL_GetPerformanceCounter();
        return secondsElapsed * 1000;
    }
};

SDL_bool SDL_FRectEmpty(const SDL_FRect* r)
{
    return ((!r) || (r->w <= 0) || (r->h <= 0)) ? SDL_TRUE : SDL_FALSE;
}

SDL_bool SDL_IntersectFRect(const SDL_FRect* A, const SDL_FRect* B, SDL_FRect* result)
{
    int Amin, Amax, Bmin, Bmax;

    if (!A) {
        SDL_InvalidParamError("A");
        return SDL_FALSE;
    }

    if (!B) {
        SDL_InvalidParamError("B");
        return SDL_FALSE;
    }

    if (!result) {
        SDL_InvalidParamError("result");
        return SDL_FALSE;
    }

    /* Special cases for empty rects */
    if (SDL_FRectEmpty(A) || SDL_FRectEmpty(B)) {
        result->w = 0;
        result->h = 0;
        return SDL_FALSE;
    }

    /* Horizontal intersection */
    Amin = A->x;
    Amax = Amin + A->w;
    Bmin = B->x;
    Bmax = Bmin + B->w;
    if (Bmin > Amin)
        Amin = Bmin;
    result->x = Amin;
    if (Bmax < Amax)
        Amax = Bmax;
    result->w = Amax - Amin;

    /* Vertical intersection */
    Amin = A->y;
    Amax = Amin + A->h;
    Bmin = B->y;
    Bmax = Bmin + B->h;
    if (Bmin > Amin)
        Amin = Bmin;
    result->y = Amin;
    if (Bmax < Amax)
        Amax = Bmax;
    result->h = Amax - Amin;

    return (SDL_FRectEmpty(result) == SDL_TRUE) ? SDL_FALSE : SDL_TRUE;
}

SDL_bool SDL_HasIntersectionF(const SDL_FRect* A, const SDL_FRect* B)
{
    float Amin, Amax, Bmin, Bmax;

    if (!A) {
        SDL_InvalidParamError("A");
        return SDL_FALSE;
    }

    if (!B) {
        SDL_InvalidParamError("B");
        return SDL_FALSE;
    }

    /* Special cases for empty rects */
    if (SDL_FRectEmpty(A) || SDL_FRectEmpty(B)) {
        return SDL_FALSE;
    }

    /* Horizontal intersection */
    Amin = A->x;
    Amax = Amin + A->w;
    Bmin = B->x;
    Bmax = Bmin + B->w;
    if (Bmin > Amin)
        Amin = Bmin;
    if (Bmax < Amax)
        Amax = Bmax;
    if (Amax <= Amin)
        return SDL_FALSE;

    /* Vertical intersection */
    Amin = A->y;
    Amax = Amin + A->h;
    Bmin = B->y;
    Bmax = Bmin + B->h;
    if (Bmin > Amin)
        Amin = Bmin;
    if (Bmax < Amax)
        Amax = Bmax;
    if (Amax <= Amin)
        return SDL_FALSE;

    return SDL_TRUE;
}

int eventWatch(void* userdata, SDL_Event* event)
{
    // WARNING: Be very careful of what you do in the function, as it may run in a different thread
    if (event->type == SDL_APP_TERMINATING || event->type == SDL_APP_WILLENTERBACKGROUND) {
    }
    return 0;
}

enum class Direction {
    Left,
    Right,
};

enum class EntityType {
    Leaf,
    Apple,
    Grape,
    Banana,
    Carrot,
    Potato,
    Pumpkin
};

enum class Food {
    Empty = -1,
    Apple,
    Carrot,
    Grape,
    Potato,
    Pumpkin,
    Banana,
    NumFood
};

struct Entity {
    SDL_FRect r{};
    Direction direction = Direction::Right;
    SDL_FPoint offsetP{};
    EntityType entityType = EntityType::Leaf;
};

struct Player {
    SDL_FRect r{};
    int dx = 0;
    int dy = 0;
};

SDL_Texture* GetFoodTexture(Food type)
{
    switch (type) {
    case Food::Apple:
        return appleT;
    case Food::Banana:
        return bananaT;
    case Food::Carrot:
        return carrotT;
    case Food::Grape:
        return grapeT;
    case Food::Potato:
        return potatoT;
    case Food::Pumpkin:
        return pumpkinT;
    default:
        return NULL;
    }
}

struct Plot {
    SDL_FRect r;
    Food food{};
    int numProduce = 1;
    std::vector<SDL_FRect> producesR;

    Plot()
    {
        r.w = 200;
        r.h = 130;
        for (int i = 0; i < MAX_TREES; ++i) {
            SDL_FRect a{};
            a.w = 64;
            a.h = 64;
            producesR.push_back(a);
        }
    }

    void setFood(Food type)
    {
        food = type;
    }

    SDL_Texture* getPlotTexture()
    {
        switch (food) {
        case Food::Apple:
            return appleTreeT;
        case Food::Banana:
            return bananaTreeT;
        case Food::Carrot:
            return soilT;
        case Food::Grape:
            return vineT;
        case Food::Potato:
            return soilT;
        case Food::Pumpkin:
            return pumpkinTreeT;
        default:
            return appleTreeT;
        }
    }

    void renderPlot()
    {
        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 200);
        SDL_RenderFillRectF(renderer, &r);
        for (int i = 0; i < numProduce; ++i) {

            producesR[i].x = r.x + (r.w / (numProduce + 1) * (i + 1)) - producesR[i].w / 2.0f;
            producesR[i].y = r.y + r.h / 2.0f - producesR[i].h / 2.0f;

            SDL_RenderCopyF(renderer, getPlotTexture(), 0, &producesR[i]);
        }
    }

    bool isIntersecting(const SDL_FRect A)
    {
        for (int i = 0; i < producesR.size(); ++i) {
            if (SDL_HasIntersectionF(&A, &producesR[i])) {
                return true;
            }
        }

        return false;
    }
};

struct Button {
    SDL_FRect container;
    Text text;
    SDL_Color selected;
    SDL_Color unselected;
    SDL_Texture* image;
    bool isSelected = false;
    bool hasImage = false;

    void init(SDL_Renderer* renderer, const SDL_FRect size, const std::string text)
    {
        container = size;
        container.w = static_cast<float>(LETTER_WIDTH * text.length() + 30);
        container.x = size.x + size.w / 2.0f - container.w / 2.0f;
        this->text.dstR.w = LETTER_WIDTH * text.length();
        this->text.dstR.h = 50;
        this->text.dstR.x = container.x + container.w / 2.0f - this->text.dstR.w / 2.0f;
        this->text.dstR.y = container.y + container.h / 2.0f - this->text.dstR.h / 2.0f;
        this->text.setText(renderer, robotoF, text);
    }

    void setColors(SDL_Color selected, SDL_Color unselected)
    {
        this->selected = selected;
        this->unselected = unselected;
    }

    void draw(SDL_Renderer* renderer)
    {
        if (isSelected) {
            SDL_SetRenderDrawColor(renderer, selected.r, selected.g, selected.b, selected.a);
        }
        else {
            SDL_SetRenderDrawColor(renderer, unselected.r, unselected.g, unselected.b, unselected.a);
        }

        SDL_RenderFillRectF(renderer, &container);
        text.draw(renderer);
    }

    void draw(SDL_Renderer* renderer, SDL_Texture* texture, SDL_FRect textureR, float offset = 20)
    {
        SDL_FRect newContainer;
        newContainer.h = textureR.h + text.dstR.h + (2 * offset);
        newContainer.w = std::max(container.w, textureR.w + (2 * offset));
        newContainer.x = textureR.x - offset / 2.0f;
        newContainer.y = textureR.y - offset / 2.0f;
        container = newContainer;
        this->text.dstR.x = container.x + container.w / 2.0f - this->text.dstR.w / 2.0f;
        this->text.dstR.y = container.y + container.h - offset - this->text.dstR.h;

        textureR.x = container.x + container.w / 2.0f - textureR.w / 2.0f;

        draw(renderer);
        SDL_RenderCopyF(renderer, texture, 0, &textureR);
    }

    void updateButton()
    {
        if (SDL_PointInFRect(&mousePos, &container)) {
            if (!isSelected) {
                isSelected = true;
            }
        }
        else if (isSelected) {
            isSelected = false;
        }
    }
};

enum class State {
    Outside,
    Shop,
    Intro,
    Home,
};

struct Shop {
    SDL_FRect lessRotR{};
    Text lessRotText;
    Text lessRotPriceText;
    SDL_FRect lessRotBuyR{};

    SDL_FRect moreEnergyR{};
    Text moreEnergyText;
    Text moreEnergyPriceText;
    SDL_FRect moreEnergyBuyR{};

    SDL_FRect backArrowR{};
};

enum class Music {
    JosephKosma,
    AntonioVivaldi,
};

enum class Motion {
    Left,
    Right,
};

struct Intro {
    Clock introClock;
    SDL_FRect leafR{};
    Motion leafMotion = Motion::Right;
    int leafAngle = 0;
};

SDL_FRect grapeTreeR;
SDL_FRect appleTreeR;
SDL_FRect bananaTreeR;
SDL_FRect carrotTreeR;
SDL_FRect potatoTreeR;
SDL_FRect pumpkinTreeR;
std::vector<Plot> plots;
SDL_FRect soundBtnR;
std::vector<Entity> entities;
Clock leafClock;
Clock globalClock;
Clock grapeClock;
Clock appleClock;
Clock bananaClock;
Clock carrotClock;
Clock potatoClock;
Clock pumpkinClock;
Clock timeClock;
Clock tradeClock;
Text scoreText;
SDL_FRect shopR;
SDL_FRect chestR;
Shop shop;
State state = State::Intro;
Clock rotClock;
int rotDelayInMs = ROT_INIT_DELAY_IN_MS;
Music currentMusic = Music::JosephKosma;
int grapeSpawnDelayInMs = GRAPE_INIT_SPAWN_DELAY_IN_MS;
int appleSpawnDelayInMs = APPLE_INIT_SPAWN_DELAY_IN_MS;
int bananaSpawnDelayInMs = BANANA_INIT_SPAWN_DELAY_IN_MS;
int carrotSpawnDelayInMs = CARROT_INIT_SPAWN_DELAY_IN_MS;
int potatoSpawnDelayInMs = POTATO_INIT_SPAWN_DELAY_IN_MS;
int pumpkinSpawnDelayInMs = PUMPKIN_INIT_SPAWN_DELAY_IN_MS;
bool isMuted = false;
Intro intro;
Text hourText;
int hour = 7;
SDL_FRect sunR;
SDL_FRect energyR;
Text energyText;
bool shouldShowRotImage = false;
SDL_FRect rotR;
int maxEnergy = MAX_ENERGY_INIT;
SDL_FRect houseflyR;
bool houseflyGoingRight = true;
std::vector<SDL_FRect> tradeRects;
Player player;
SDL_FRect houseR;
bool isMoving = false;
Clock energyClock;
bool shouldGoHome = false;
SDL_FRect collectR;
bool isCollecting = false;
SDL_FRect inventorySlotR;
SDL_FRect inventorySlot2R;
SDL_FRect inventorySlotXR;
SDL_FRect inventorySlotX2R;
std::array<Food, 2> foods;
Button backHomeButton;
Button shopButton;
Button sleepButton;
bool exitedHome = false;
SDL_FRect doorR;
Text infoText;
bool shouldShowInfoText;
Text cantCollectText;
bool canCollect = true;
Text canSleepAndGoShopText;
bool shouldShowWhenCanSleepAndGoShop = false;

void muteMusicAndSounds()
{
    Mix_VolumeMusic(0);
    Mix_Volume(-1, 0);
}

void unmuteMusicAndSounds()
{
    Mix_VolumeMusic(128);
    Mix_Volume(-1, 128);
}

void saveData()
{
    pugi::xml_document doc;
    pugi::xml_node rootNode = doc.append_child("root");
    pugi::xml_node scoreNode = rootNode.append_child("score");
    scoreNode.append_child(pugi::node_pcdata).set_value(scoreText.text.c_str());
    pugi::xml_node rotDelayInMsNode = rootNode.append_child("rotDelayInMs");
    rotDelayInMsNode.append_child(pugi::node_pcdata).set_value(std::to_string(rotDelayInMs).c_str());
    pugi::xml_node isMutedNode = rootNode.append_child("isMuted");
    isMutedNode.append_child(pugi::node_pcdata).set_value(std::to_string(isMuted).c_str());
    pugi::xml_node maxEnergyNode = rootNode.append_child("maxEnergy");
    maxEnergyNode.append_child(pugi::node_pcdata).set_value(std::to_string(maxEnergy).c_str());
    doc.save_file((prefPath + "data.xml").c_str());
    // TODO: When adding emscripten add their saveing
}

void readData()
{
    pugi::xml_document doc;
    doc.load_file((prefPath + "data.xml").c_str());
    pugi::xml_node rootNode = doc.child("root");
    scoreText.setText(renderer, robotoF, rootNode.child("score").text().as_int());
    rotDelayInMs = rootNode.child("leafRotDelayInMs").text().as_int(ROT_INIT_DELAY_IN_MS);
    isMuted = rootNode.child("isMuted").text().as_bool();
    if (isMuted) {
        muteMusicAndSounds();
    }
    maxEnergy = rootNode.child("maxEnergy").text().as_int(MAX_ENERGY_INIT);
    energyText.setText(renderer, robotoF, maxEnergy);
}

float clamp(float n, float lower, float upper)
{
    return std::max(lower, std::min(n, upper));
}

void SetPosition()
{
    houseR.w = 64;
    houseR.h = 64;
    houseR.x = windowWidth / 2 - houseR.w / 2;
    houseR.y = windowHeight / 2 - houseR.h / 2;

    int numPlots = static_cast<int>(Food::NumFood);
    float angleIncrement = 2 * M_PI / numPlots;
    float distanceH = windowHeight - 2.0f * SCREEN_PADDING;
    float distanceW = windowWidth - 2.0f * SCREEN_PADDING;

    for (int i = 0; i < numPlots; ++i) {
        Plot plot{};
        plot.setFood(static_cast<Food>(i));

        float angle = angleIncrement * i;
        angle += (M_PI / 2.0f);
        if (angle < -M_PI) {
            angle += 2 * M_PI;
        }
        if (angle > M_PI) {
            angle -= 2 * M_PI;
        }
        float tanAngle = atan2f(distanceH, distanceW);

        int region;
        if ((angle > -tanAngle) && (angle <= tanAngle)) {
            region = 1;
        }
        else if ((angle > tanAngle)
            && (angle <= (M_PI - tanAngle))) {
            region = 2;
        }
        else if ((angle > (M_PI - tanAngle))
            || (angle <= -(M_PI - tanAngle))) {
            region = 3;
        }
        else {
            region = 4;
        }

        float xMultiplier = 1;
        float yMultiplier = 1;
        switch (region) {
        case 1:
            yMultiplier = -1;
            break;
        case 2:
            yMultiplier = -1;
            break;
        case 3:
            xMultiplier = -1;
            break;
        case 4:
            xMultiplier = -1;
            break;
        }

        if ((abs(angle - M_PI * 0.5f) < EPSILON) || (abs(angle + M_PI * 0.5f) < EPSILON)) {
            angle = 0.0f;
        }
        float theta = tanf(angle);
        if (region == 1 || region == 3) {
            plot.r.x = windowWidth / 2.0f + xMultiplier * distanceW / 2.0f;
            plot.r.y = windowHeight / 2.0f + yMultiplier * distanceW / 2.0f * theta - plot.r.h / 2.0f;
        }
        else {
            plot.r.x = windowWidth / 2.0f + xMultiplier * distanceH / 2.0f * theta - plot.r.w / 2.0f;
            plot.r.y = windowHeight / 2.0f + yMultiplier * distanceH / 2.0f;
        }
        plot.r.x = clamp(plot.r.x, static_cast<float>(SCREEN_PADDING), windowWidth - SCREEN_PADDING - plot.r.w);
        plot.r.y = clamp(plot.r.y, static_cast<float>(SCREEN_PADDING), windowHeight - SCREEN_PADDING - plot.r.h);
        plots.push_back(plot);
    }
}

void RenderScreen()
{
    for (auto& plot : plots) {
        plot.renderPlot();
    }
}

void drawInventory()
{
    SDL_SetRenderDrawColor(renderer, BROWN_COLOR);
    SDL_RenderFillRectF(renderer, &inventorySlotR);
    SDL_RenderFillRectF(renderer, &inventorySlot2R);
    if (foods[0] == Food::Apple) {
        SDL_RenderCopyF(renderer, appleT, 0, &inventorySlotR);
    }
    else if (foods[0] == Food::Potato) {
        SDL_RenderCopyF(renderer, potatoT, 0, &inventorySlotR);
    }
    else if (foods[0] == Food::Banana) {
        SDL_RenderCopyF(renderer, bananaT, 0, &inventorySlotR);
    }
    else if (foods[0] == Food::Carrot) {
        SDL_RenderCopyF(renderer, carrotT, 0, &inventorySlotR);
    }
    else if (foods[0] == Food::Grape) {
        SDL_RenderCopyF(renderer, grapeT, 0, &inventorySlotR);
    }
    else if (foods[0] == Food::Pumpkin) {
        SDL_RenderCopyF(renderer, pumpkinT, 0, &inventorySlotR);
    }
    if (foods[1] == Food::Apple) {
        SDL_RenderCopyF(renderer, appleT, 0, &inventorySlot2R);
    }
    else if (foods[1] == Food::Potato) {
        SDL_RenderCopyF(renderer, potatoT, 0, &inventorySlot2R);
    }
    else if (foods[1] == Food::Banana) {
        SDL_RenderCopyF(renderer, bananaT, 0, &inventorySlot2R);
    }
    else if (foods[1] == Food::Carrot) {
        SDL_RenderCopyF(renderer, carrotT, 0, &inventorySlot2R);
    }
    else if (foods[1] == Food::Grape) {
        SDL_RenderCopyF(renderer, grapeT, 0, &inventorySlot2R);
    }
    else if (foods[1] == Food::Pumpkin) {
        SDL_RenderCopyF(renderer, pumpkinT, 0, &inventorySlot2R);
    }
    if (foods[0] != Food::Empty) {
        SDL_RenderCopyF(renderer, xT, 0, &inventorySlotXR);
    }
    if (foods[1] != Food::Empty) {
        SDL_RenderCopyF(renderer, xT, 0, &inventorySlotX2R);
    }
}

void RenderUI()
{
    scoreText.draw(renderer);
    if (isMuted) {
        SDL_RenderCopyF(renderer, mutedT, 0, &soundBtnR);
    }
    else {
        SDL_RenderCopyF(renderer, soundT, 0, &soundBtnR);
    }
    hourText.draw(renderer);
    if (hour >= 7 && hour <= 17) {
        SDL_RenderCopyF(renderer, sunT, 0, &sunR);
    }
    else {
        SDL_RenderCopyF(renderer, moonT, 0, &sunR);
    }
    energyText.draw(renderer);
    SDL_RenderCopyF(renderer, energyT, 0, &energyR);
    drawInventory();
    SDL_RenderCopyF(renderer, playerT, 0, &player.r);
}

void HomeInit()
{
    SDL_FRect tempR;
    tempR.w = 200;
    tempR.h = 75;
    tempR.x = windowWidth / 2.0f - tempR.w / 2.0f;
    tempR.y = windowHeight - SCREEN_PADDING - tempR.h;
    backHomeButton.init(renderer, tempR, "Go Back to Farm");
    backHomeButton.setColors(HOME_BUTTON_SELECTED, HOME_BUTTON_UNSELECTED);

    shopR.w = 96;
    shopR.h = 96;
    shopR.x = windowWidth / 2.0f - shopR.w / 2.0f;
    shopR.y = windowHeight / 2.0f - shopR.h / 2.0f;

    tempR.w = 100;
    tempR.h = 50;
    tempR.x = shopR.x + shopR.w / 2.0f - tempR.w / 2.0f;
    tempR.y = shopR.y + shopR.h + 5;
    shopButton.init(renderer, tempR, "Shop");
    shopButton.setColors(HOME_BUTTON_SELECTED, HOME_BUTTON_UNSELECTED);

    sleepButton.init(renderer, tempR, "Sleep");
    sleepButton.setColors(HOME_BUTTON_SELECTED, HOME_BUTTON_UNSELECTED);
}

void RenderHome()
{
    backHomeButton.draw(renderer);
    shopButton.draw(renderer, shopT, shopR);
    shopR.x -= 200;
    sleepButton.draw(renderer, bedT, shopR);
    shopR.x += 200;

    RenderUI();
    if (shouldShowInfoText) {
        infoText.draw(renderer);
    }
    if (shouldShowWhenCanSleepAndGoShop) {
        canSleepAndGoShopText.draw(renderer);
    }
}

void moveTimeByOneHour()
{
    if (timeClock.getElapsedTime() > 1000) {
        ++hour;
        if (hour == 24) {
            hour = 0;
        }
        std::string s = std::to_string(hour);
        if (hour > 12) {
            s = std::to_string(hour - 12);
        }
        std::string h = s + ((hour <= 12) ? "am" : "pm");
        hourText.setText(renderer, robotoF, h);
        timeClock.restart();
    }
}

void mainLoop()
{
    float deltaTime = globalClock.restart();
    SDL_FRect windowR;
    windowR.w = windowWidth;
    windowR.h = windowHeight;
    windowR.x = 0;
    windowR.y = 0;
    if (!Mix_PlayingMusic()) {
        if (currentMusic == Music::JosephKosma) {
            currentMusic = Music::AntonioVivaldi;
            Mix_PlayMusic(antonioVivaldiM, 1);
        }
        else if (currentMusic == Music::AntonioVivaldi) {
            currentMusic = Music::JosephKosma;
            Mix_PlayMusic(josephKosmaM, 1);
        }
    }
    if (state == State::Intro) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                running = false;
                // TODO: On mobile remember to use eventWatch function (it doesn't reach this code when terminating)
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                SDL_RenderSetScale(renderer, event.window.data1 / (float)windowWidth, event.window.data2 / (float)windowHeight);
            }
            if (event.type == SDL_KEYDOWN) {
                keys[event.key.keysym.scancode] = true;
            }
            if (event.type == SDL_KEYUP) {
                keys[event.key.keysym.scancode] = false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                buttons[event.button.button] = true;
            }
            if (event.type == SDL_MOUSEBUTTONUP) {
                buttons[event.button.button] = false;
            }
            if (event.type == SDL_MOUSEMOTION) {
                float scaleX, scaleY;
                SDL_RenderGetScale(renderer, &scaleX, &scaleY);
                mousePos.x = event.motion.x / scaleX;
                mousePos.y = event.motion.y / scaleY;
                realMousePos.x = event.motion.x;
                realMousePos.y = event.motion.y;
            }
        }
        if (intro.introClock.getElapsedTime() > 1000) {
            state = State::Outside;
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_RenderCopyExF(renderer, leafT, 0, &intro.leafR, intro.leafAngle, 0, SDL_FLIP_NONE);
        if (intro.leafMotion == Motion::Right) {
            ++intro.leafAngle;
            if (intro.leafAngle == 75) {
                intro.leafMotion = Motion::Left;
            }
        }
        else if (intro.leafMotion == Motion::Left) {
            --intro.leafAngle;
            if (intro.leafAngle == -75) {
                intro.leafMotion = Motion::Right;
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(8);
    }
    else if (state == State::Outside) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                running = false;
                // TODO: On mobile remember to use eventWatch function (it doesn't reach this code when terminating)
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                SDL_RenderSetScale(renderer, event.window.data1 / (float)windowWidth, event.window.data2 / (float)windowHeight);
            }
            if (event.type == SDL_KEYDOWN) {
                keys[event.key.keysym.scancode] = true;
                if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                    if (hour >= 7 && hour <= 17) {
                        canCollect = true;
                        if (foods[0] == Food::Empty || foods[1] == Food::Empty) {
                            int index = foods[0] == Food::Empty ? 0 : 1;
                            for (auto& plot : plots) {
                                if (plot.isIntersecting(player.r)) {
                                    foods[index] = plot.food;
                                    Mix_PlayChannel(-1, pickupS, 0);
                                }
                            }
                        }
                    }
                    else {
                        canCollect = false;
                    }
                }
            }
            if (event.type == SDL_KEYUP) {
                keys[event.key.keysym.scancode] = false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                buttons[event.button.button] = true;
                if (SDL_PointInFRect(&mousePos, &soundBtnR)) {
                    isMuted = !isMuted;
                    if (isMuted) {
                        muteMusicAndSounds();
                    }
                    else {
                        unmuteMusicAndSounds();
                    }
                }
                if (SDL_PointInFRect(&mousePos, &inventorySlotXR)) {
                    foods[0] = Food::Empty;
                }
                if (SDL_PointInFRect(&mousePos, &inventorySlotX2R)) {
                    foods[1] = Food::Empty;
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP) {
                buttons[event.button.button] = false;
            }
            if (event.type == SDL_MOUSEMOTION) {
                float scaleX, scaleY;
                SDL_RenderGetScale(renderer, &scaleX, &scaleY);
                mousePos.x = event.motion.x / scaleX;
                mousePos.y = event.motion.y / scaleY;
                realMousePos.x = event.motion.x;
                realMousePos.y = event.motion.y;
            }
        }
        for (int i = 0; i < entities.size(); ++i) {
            if (entities[i].direction == Direction::Right) {
                entities[i].r.x += 0.01 * deltaTime;
            }
            else if (entities[i].direction == Direction::Left) {
                entities[i].r.x += -0.01 * deltaTime;
            }
            entities[i].r.y = std::pow(entities[i].r.x, 2) / 28;
        }
        if (rotClock.getElapsedTime() > rotDelayInMs) {
            if (std::stoi(scoreText.text) > 0) {
                scoreText.setText(renderer, robotoF, std::stoi(scoreText.text) - 1);
            }
            rotClock.restart();
        }
        shouldShowRotImage = rotClock.getElapsedTime() + 1000 > rotDelayInMs && std::stoi(scoreText.text) > 0;
        moveTimeByOneHour();
        if (tradeClock.getElapsedTime() > 1000) // TODO: Set it to 20000
        {
            tradeRects.push_back(SDL_FRect());
            tradeRects.back().w = windowWidth;
            tradeRects.back().h = windowHeight;
            tradeRects.back().x = 0;
            tradeRects.back().y = -windowHeight + 1;
            tradeClock.restart();
        }
        for (int i = 0; i < tradeRects.size(); ++i) {
            tradeRects[i].y += deltaTime;
            if (!SDL_HasIntersectionF(&tradeRects[i], &windowR)) {
                tradeRects.erase(tradeRects.begin() + i--);
                // TODO: Mix prize for each vegetable/fruit (save them to file also)
            }
        }
        player.dx = 0;
        player.dy = 0;
        if (shouldGoHome) {
            if (player.r.x + player.r.w < houseR.x) {
                player.r.x += PLAYER_SPEED * deltaTime;
            }
            else if (player.r.x > houseR.x + houseR.w) {
                player.r.x += -PLAYER_SPEED * deltaTime;
            }
            if (player.r.y + player.r.h < houseR.y) {
                player.r.y += PLAYER_SPEED * deltaTime;
            }
            else if (player.r.y > houseR.y + houseR.h) {
                player.r.y += -PLAYER_SPEED * deltaTime;
            }
        }
        else {
            if (keys[SDL_SCANCODE_A]) {
                player.dx = -1;
                if (!isMoving) {
                    energyClock.restart();
                }
                isMoving = true;
            }
            else if (keys[SDL_SCANCODE_D]) {
                player.dx = 1;
                if (!isMoving) {
                    energyClock.restart();
                }
                isMoving = true;
            }
            if (keys[SDL_SCANCODE_W]) {
                player.dy = -1;
                if (!isMoving) {
                    energyClock.restart();
                }
                isMoving = true;
            }
            else if (keys[SDL_SCANCODE_S]) {
                player.dy = 1;
                if (!isMoving) {
                    energyClock.restart();
                }
                isMoving = true;
            }
        }
        player.r.x = clamp(player.r.x, 0, windowWidth - player.r.w);
        player.r.y = clamp(player.r.y, 0, windowHeight - player.r.h);
        if (!keys[SDL_SCANCODE_A] && !keys[SDL_SCANCODE_D]
            && !keys[SDL_SCANCODE_W] && !keys[SDL_SCANCODE_S]) {
            isMoving = false;
        }
        if (isMoving) {
            if (energyClock.getElapsedTime() > 1000) {
                if (std::stoi(energyText.text) != 0) {
                    energyText.setText(renderer, robotoF, std::stoi(energyText.text) - 1);
                }
                if (energyText.text == "0") {
                    shouldGoHome = true;
                }
                energyClock.restart();
            }
        }
        player.r.x += player.dx * deltaTime * PLAYER_SPEED;
        player.r.y += player.dy * deltaTime * PLAYER_SPEED;
        if (SDL_HasIntersectionF(&houseR, &player.r)) {
            if (!exitedHome) {
                exitedHome = true;
                state = State::Home;
                Mix_PlayChannel(-1, doorS, 0);
            }
        }
        if (exitedHome) {
            exitedHome = false;
        }
        isCollecting = false;
        for (auto& plot : plots) {
            if (plot.isIntersecting(player.r)) {
                isCollecting = true;
                break;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
#if 0 // TODO: Turn it on when done?
        for (int i = 0; i < tradeRects.size(); ++i) {
            SDL_RenderCopyF(renderer, tradeT, 0, &tradeRects[i]);
        }
#endif
        RenderScreen();

        if (shouldShowRotImage) {
            SDL_RenderCopyF(renderer, rotT, 0, &rotR);
            SDL_RenderCopyF(renderer, houseflyT, 0, &houseflyR);
            if (houseflyGoingRight) {
                houseflyR.x += 0.1 * deltaTime;
                if (houseflyR.x + houseflyR.w >= rotR.x + rotR.w) {
                    houseflyGoingRight = false;
                    houseflyR.y -= 20;
                }
            }
            else {
                houseflyR.x -= 0.05 * deltaTime;
            }
        }
        else {
            houseflyR.x = rotR.x;
            houseflyGoingRight = true;
            houseflyR.y = rotR.y + rotR.h - houseflyR.h;
        }
        SDL_RenderCopyF(renderer, houseT, 0, &houseR);
        SDL_RenderCopyF(renderer, playerT, 0, &player.r);
        if (isCollecting) {
            SDL_RenderCopyF(renderer, collectT, 0, &collectR);
        }
        drawInventory();
        RenderUI();
        if (!canCollect) {
            cantCollectText.draw(renderer);
        }
        SDL_RenderPresent(renderer);
    }
    else if (state == State::Shop) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                running = false;
                // TODO: On mobile remember to use eventWatch function (it doesn't reach this code when terminating)
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                SDL_RenderSetScale(renderer, event.window.data1 / (float)windowWidth, event.window.data2 / (float)windowHeight);
            }
            if (event.type == SDL_KEYDOWN) {
                keys[event.key.keysym.scancode] = true;
            }
            if (event.type == SDL_KEYUP) {
                keys[event.key.keysym.scancode] = false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                buttons[event.button.button] = true;
                if (SDL_PointInFRect(&mousePos, &shop.backArrowR)) {
                    state = State::Home;
                }
                if (SDL_PointInFRect(&mousePos, &shop.lessRotBuyR)) {
                    if (std::stoi(scoreText.text) >= std::stoi(shop.lessRotPriceText.text)) {
                        scoreText.setText(renderer, robotoF, std::stoi(scoreText.text) - std::stoi(shop.lessRotPriceText.text));
                        rotDelayInMs += 1000;
                        Mix_PlayChannel(-1, powerupS, 0);
                    }
                }
                if (SDL_PointInFRect(&mousePos, &shop.moreEnergyBuyR)) {
                    if (std::stoi(scoreText.text) >= std::stoi(shop.moreEnergyPriceText.text)) {
                        scoreText.setText(renderer, robotoF, std::stoi(scoreText.text) - std::stoi(shop.moreEnergyPriceText.text));
                        ++maxEnergy;
                        Mix_PlayChannel(-1, powerupS, 0);
                    }
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP) {
                buttons[event.button.button] = false;
            }
            if (event.type == SDL_MOUSEMOTION) {
                float scaleX, scaleY;
                SDL_RenderGetScale(renderer, &scaleX, &scaleY);
                mousePos.x = event.motion.x / scaleX;
                mousePos.y = event.motion.y / scaleY;
                realMousePos.x = event.motion.x;
                realMousePos.y = event.motion.y;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_RenderCopyF(renderer, leafWithSprayerT, 0, &shop.lessRotR);
        shop.lessRotText.draw(renderer);
        shop.lessRotPriceText.draw(renderer);
        SDL_RenderCopyF(renderer, buyT, 0, &shop.lessRotBuyR);
        SDL_RenderCopyF(renderer, moreEnergyT, 0, &shop.moreEnergyR);
        shop.moreEnergyText.draw(renderer);
        shop.moreEnergyPriceText.draw(renderer);
        SDL_RenderCopyF(renderer, buyT, 0, &shop.moreEnergyBuyR);
        SDL_RenderCopyF(renderer, backArrowT, 0, &shop.backArrowR);
        SDL_RenderPresent(renderer);
    }
    else if (state == State::Home) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                running = false;
                // TODO: On mobile remember to use eventWatch function (it doesn't reach this code when terminating)
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                SDL_RenderSetScale(renderer, event.window.data1 / (float)windowWidth, event.window.data2 / (float)windowHeight);
            }
            if (event.type == SDL_KEYDOWN) {
                keys[event.key.keysym.scancode] = true;
            }
            if (event.type == SDL_KEYUP) {
                keys[event.key.keysym.scancode] = false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                buttons[event.button.button] = true;
                if (SDL_PointInFRect(&mousePos, &shopButton.container)) {
                    if (hour >= 0 && hour < 7 || hour > 17) {
                        state = State::Shop;
                        shouldShowWhenCanSleepAndGoShop = false;
                        Mix_PlayChannel(-1, doorS, 0);
                    }
                    else {
                        shouldShowWhenCanSleepAndGoShop = true;
                    }
                }
                else if (SDL_PointInFRect(&mousePos, &backHomeButton.container)) {
                    if (!shouldGoHome) {
                        state = State::Outside;
                        player.r.x = houseR.x + houseR.w + 5;
                        player.r.y = houseR.y + houseR.h / 2 - player.r.h / 2;
                        canCollect = true;
                        shouldShowWhenCanSleepAndGoShop = false;
                        Mix_PlayChannel(-1, doorS, 0);
                    }
                    else {
                        shouldShowInfoText = true;
                    }
                }
                else if (SDL_PointInFRect(&mousePos, &sleepButton.container)) {
                    if (hour >= 0 && hour < 7 || hour > 17) {
                        shouldShowWhenCanSleepAndGoShop = false;
                        hour = 7;
                        hourText.setText(renderer, robotoF, std::to_string(hour) + "am");
                        energyText.setText(renderer, robotoF, maxEnergy);
                        shouldGoHome = false;
                        shouldShowInfoText = false;
                        Mix_PlayChannel(-1, sleepS, 0);
                    }
                    else {
                        shouldShowWhenCanSleepAndGoShop = true;
                    }
                }
                if (SDL_PointInFRect(&mousePos, &soundBtnR)) {
                    isMuted = !isMuted;
                    if (isMuted) {
                        muteMusicAndSounds();
                    }
                    else {
                        unmuteMusicAndSounds();
                    }
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP) {
                buttons[event.button.button] = false;
            }
            if (event.type == SDL_MOUSEMOTION) {
                float scaleX, scaleY;
                SDL_RenderGetScale(renderer, &scaleX, &scaleY);
                mousePos.x = event.motion.x / scaleX;
                mousePos.y = event.motion.y / scaleY;
                realMousePos.x = event.motion.x;
                realMousePos.y = event.motion.y;

                backHomeButton.updateButton();
                shopButton.updateButton();
                sleepButton.updateButton();
            }
        }
        player.dx = 0;
        player.dy = 0;
        if (keys[SDL_SCANCODE_A]) {
            player.dx = -1;
        }
        else if (keys[SDL_SCANCODE_D]) {
            player.dx = 1;
        }
        if (keys[SDL_SCANCODE_W]) {
            player.dy = -1;
        }
        else if (keys[SDL_SCANCODE_S]) {
            player.dy = 1;
        }
        player.r.x += player.dx * deltaTime * PLAYER_SPEED;
        player.r.y += player.dy * deltaTime * PLAYER_SPEED;
        player.r.x = clamp(player.r.x, 0, windowWidth - player.r.w);
        player.r.y = clamp(player.r.y, 0, windowHeight - player.r.h);
        if (SDL_HasIntersectionF(&player.r, &doorR)) {
            state = State::Outside;
        }
        std::vector<int> scoreGain;
        for (int i = 0; i < (int)(Food::NumFood) - 1; ++i) {
            scoreGain.push_back(random(1, 10));
        }
        for (int i = 0; i < foods.size(); ++i) {
            if (foods[i] == Food::Apple) {
                foods[i] = Food::Empty;
                scoreText.setText(renderer, robotoF, std::stoi(scoreText.text) + scoreGain[(int)(Food::Apple)]);
            }
            else if (foods[i] == Food::Banana) {
                foods[i] = Food::Empty;
                scoreText.setText(renderer, robotoF, std::stoi(scoreText.text) + +scoreGain[(int)(Food::Banana)]);
            }
            else if (foods[i] == Food::Carrot) {
                foods[i] = Food::Empty;
                scoreText.setText(renderer, robotoF, std::stoi(scoreText.text) + +scoreGain[(int)(Food::Carrot)]);
            }
            else if (foods[i] == Food::Grape) {
                foods[i] = Food::Empty;
                scoreText.setText(renderer, robotoF, std::stoi(scoreText.text) + +scoreGain[(int)(Food::Grape)]);
            }
            else if (foods[i] == Food::Potato) {
                foods[i] = Food::Empty;
                scoreText.setText(renderer, robotoF, std::stoi(scoreText.text) + +scoreGain[(int)(Food::Potato)]);
            }
            else if (foods[i] == Food::Pumpkin) {
                foods[i] = Food::Empty;
                scoreText.setText(renderer, robotoF, std::stoi(scoreText.text) + +scoreGain[(int)(Food::Pumpkin)]);
            }
        }
        moveTimeByOneHour();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        RenderHome();
        SDL_RenderPresent(renderer);
    }
    saveData();
}

int main(int argc, char* argv[])
{
    std::srand(std::time(0));
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
    SDL_LogSetOutputFunction(logOutputCallback, 0);
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    SDL_GetMouseState(&mousePos.x, &mousePos.y);
    window = SDL_CreateWindow("AutumnLeafCollector", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    robotoF = TTF_OpenFont("res/roboto.ttf", 72);
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    SDL_RenderSetScale(renderer, w / (float)windowWidth, h / (float)windowHeight);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_AddEventWatch(eventWatch, 0);
    prefPath = SDL_GetPrefPath("NextCodeApps", "AutumnLeafCollector");
    leavesT = IMG_LoadTexture(renderer, "res/leaves.png");
    treeT = IMG_LoadTexture(renderer, "res/tree.png");
    tree1T = IMG_LoadTexture(renderer, "res/tree1.png");
    tree2T = IMG_LoadTexture(renderer, "res/tree2.png");
    tree3T = IMG_LoadTexture(renderer, "res/tree3.png");
    shopT = IMG_LoadTexture(renderer, "res/shop.png");
    backArrowT = IMG_LoadTexture(renderer, "res/backArrow.png");
    buyT = IMG_LoadTexture(renderer, "res/buy.png");
    leafWithSprayerT = IMG_LoadTexture(renderer, "res/leafWithSprayer.png");
    vineT = IMG_LoadTexture(renderer, "res/vine.png");
    grapeT = IMG_LoadTexture(renderer, "res/grape.png");
    appleT = IMG_LoadTexture(renderer, "res/apple.png");
    appleTreeT = IMG_LoadTexture(renderer, "res/appleTree.png");
    bananaT = IMG_LoadTexture(renderer, "res/banana.png");
    carrotT = IMG_LoadTexture(renderer, "res/carrot.png");
    soilT = IMG_LoadTexture(renderer, "res/soil.png");
    bananaTreeT = IMG_LoadTexture(renderer, "res/bananaTree.png");
    mutedT = IMG_LoadTexture(renderer, "res/muted.png");
    soundT = IMG_LoadTexture(renderer, "res/sound.png");
    leafT = IMG_LoadTexture(renderer, "res/leaf.png");
    sunT = IMG_LoadTexture(renderer, "res/sun.png");
    moonT = IMG_LoadTexture(renderer, "res/moon.png");
    energyT = IMG_LoadTexture(renderer, "res/energy.png");
    rotT = IMG_LoadTexture(renderer, "res/rot.png");
    moreEnergyT = IMG_LoadTexture(renderer, "res/moreEnergy.png");
    houseflyT = IMG_LoadTexture(renderer, "res/housefly.png");
    wormT = IMG_LoadTexture(renderer, "res/worm.png");
    potatoT = IMG_LoadTexture(renderer, "res/potato.png");
    pumpkinT = IMG_LoadTexture(renderer, "res/pumpkin.png");
    pumpkinTreeT = IMG_LoadTexture(renderer, "res/pumpkinTree.png");
    tradeT = IMG_LoadTexture(renderer, "res/trade.png");
    playerT = IMG_LoadTexture(renderer, "res/player.png");
    houseT = IMG_LoadTexture(renderer, "res/house.png");
    collectT = IMG_LoadTexture(renderer, "res/collect.png");
    openT = IMG_LoadTexture(renderer, "res/open.png");
    xT = IMG_LoadTexture(renderer, "res/x.png");
    bedT = IMG_LoadTexture(renderer, "res/bed.png");
    josephKosmaM = Mix_LoadMUS("res/autumnLeavesJosephKosma.mp3");
    antonioVivaldiM = Mix_LoadMUS("res/jesienAntonioVivaldi.mp3");
    doorS = Mix_LoadWAV("res/door.wav");
    pickupS = Mix_LoadWAV("res/pickup.wav");
    powerupS = Mix_LoadWAV("res/powerup.wav");
    sleepS = Mix_LoadWAV("res/sleep.wav");
    Mix_PlayMusic(josephKosmaM, 1);
    SetPosition();
    soundBtnR.w = 48;
    soundBtnR.h = 48;
    soundBtnR.x = windowWidth - soundBtnR.w - 20;
    soundBtnR.y = 20;
    scoreText.setText(renderer, robotoF, 0);
    scoreText.dstR.w = 50;
    scoreText.dstR.h = 30;
    scoreText.dstR.x = windowWidth / 2.0f - scoreText.dstR.w / 2.0f;
    scoreText.dstR.y = SCREEN_PADDING / 2.0f;
    shop.backArrowR.w = 32;
    shop.backArrowR.h = 32;
    shop.backArrowR.x = 5;
    shop.backArrowR.y = 5;
    shop.lessRotR.w = 32;
    shop.lessRotR.h = 32;
    shop.lessRotR.x = shop.backArrowR.x + shop.backArrowR.w + 5;
    shop.lessRotR.y = shop.lessRotPriceText.dstR.y + shop.lessRotPriceText.dstR.h + 5;
    shop.lessRotText.setText(renderer, robotoF, "Less rot");
    shop.lessRotText.dstR.w = 65;
    shop.lessRotText.dstR.h = 35;
    shop.lessRotText.dstR.x = shop.lessRotR.x;
    shop.lessRotText.dstR.y = shop.lessRotR.y + shop.lessRotR.h;
    shop.lessRotPriceText.setText(renderer, robotoF, 100);
    shop.lessRotPriceText.dstR.w = 70;
    shop.lessRotPriceText.dstR.h = 35;
    shop.lessRotPriceText.dstR.x = shop.lessRotText.dstR.x;
    shop.lessRotPriceText.dstR.y = shop.lessRotText.dstR.y + shop.lessRotText.dstR.h;
    shop.lessRotBuyR.w = 45;
    shop.lessRotBuyR.h = 32;
    shop.lessRotBuyR.x = shop.lessRotPriceText.dstR.x;
    shop.lessRotBuyR.y = shop.lessRotPriceText.dstR.y + shop.lessRotPriceText.dstR.h;
    shop.moreEnergyR.w = 32;
    shop.moreEnergyR.h = 32;
    shop.moreEnergyR.x = shop.lessRotR.x + shop.lessRotR.w + 50;
    shop.moreEnergyR.y = shop.moreEnergyPriceText.dstR.y + shop.moreEnergyPriceText.dstR.h + 5;
    shop.moreEnergyText.setText(renderer, robotoF, "More energy");
    shop.moreEnergyText.dstR.w = 85;
    shop.moreEnergyText.dstR.h = 35;
    shop.moreEnergyText.dstR.x = shop.moreEnergyR.x;
    shop.moreEnergyText.dstR.y = shop.moreEnergyR.y + shop.moreEnergyR.h;
    shop.moreEnergyPriceText.setText(renderer, robotoF, 100);
    shop.moreEnergyPriceText.dstR.w = 70;
    shop.moreEnergyPriceText.dstR.h = 35;
    shop.moreEnergyPriceText.dstR.x = shop.moreEnergyText.dstR.x;
    shop.moreEnergyPriceText.dstR.y = shop.moreEnergyText.dstR.y + shop.moreEnergyText.dstR.h;
    shop.moreEnergyBuyR.w = 45;
    shop.moreEnergyBuyR.h = 32;
    shop.moreEnergyBuyR.x = shop.moreEnergyPriceText.dstR.x;
    shop.moreEnergyBuyR.y = shop.moreEnergyPriceText.dstR.y + shop.moreEnergyPriceText.dstR.h;
    intro.leafR.w = 64;
    intro.leafR.h = 64;
    intro.leafR.x = windowWidth / 2 - intro.leafR.w / 2;
    intro.leafR.y = windowHeight / 2 - intro.leafR.h / 2;
    hourText.setText(renderer, robotoF, "7 am");
    hourText.dstR.w = 100;
    hourText.dstR.h = 35;
    hourText.dstR.x = windowWidth - hourText.dstR.w - 20;
    hourText.dstR.y = soundBtnR.y + soundBtnR.h;
    sunR.w = 32;
    sunR.h = 32;
    sunR.x = hourText.dstR.x - sunR.w;
    sunR.y = hourText.dstR.y;
    energyText.setText(renderer, robotoF, MAX_ENERGY_INIT);
    energyText.dstR.w = 30;
    energyText.dstR.h = 20;
    energyText.dstR.x = 0;
    energyText.dstR.y = 0;
    energyR.w = 25;
    energyR.h = 15;
    energyR.x = energyText.dstR.x + energyText.dstR.w;
    energyR.y = energyText.dstR.y + energyText.dstR.h / 2 - energyR.h / 2;
    rotR.w = 64;
    rotR.h = 64;
    rotR.x = appleTreeR.x + appleTreeR.w / 2 - rotR.w / 2;
    rotR.y = appleTreeR.y - rotR.h;
    houseflyR.w = 32;
    houseflyR.h = 32;
    houseflyR.x = rotR.x;
    houseflyR.y = rotR.y + rotR.h - houseflyR.h;
    player.r.w = 64;
    player.r.h = 64;
    player.r.x = houseR.x + houseR.w + 5;
    player.r.y = houseR.y;
    collectR.w = 144;
    collectR.h = 104;
    collectR.x = windowWidth / 2 - collectR.w / 2;
    collectR.y = windowHeight - collectR.h;
    inventorySlotR.w = 64;
    inventorySlotR.h = 64;
    inventorySlotR.x = windowWidth - inventorySlotR.w * 2 - 10;
    inventorySlotR.y = windowHeight - inventorySlotR.h;
    inventorySlot2R.w = 64;
    inventorySlot2R.h = 64;
    inventorySlot2R.x = windowWidth - inventorySlot2R.w;
    inventorySlot2R.y = windowHeight - inventorySlot2R.h;
    inventorySlotXR.w = 16;
    inventorySlotXR.h = 16;
    inventorySlotXR.x = inventorySlotR.x - inventorySlotXR.w / 2;
    inventorySlotXR.y = inventorySlotR.y - inventorySlotXR.h;
    inventorySlotX2R = inventorySlotXR;
    inventorySlotX2R.x = inventorySlot2R.x - inventorySlotX2R.w / 2;
    doorR.w = 32;
    doorR.h = 32;
    doorR.x = windowWidth - doorR.w - 20;
    doorR.y = windowHeight / 2 - doorR.h / 2;
    chestR.w = 32;
    chestR.h = 32;
    chestR.x = 20;
    chestR.y = windowHeight / 2 - chestR.h / 2;
    infoText.setText(renderer, robotoF, "You don't have enought energy to leave");
    infoText.dstR.w = 250;
    infoText.dstR.h = 40;
    infoText.dstR.x = windowWidth / 2 - infoText.dstR.w / 2;
    infoText.dstR.y = 200;
    cantCollectText.setText(renderer, robotoF, "You can't collect in the night");
    cantCollectText.dstR.w = 180;
    cantCollectText.dstR.h = 40;
    cantCollectText.dstR.x = windowWidth / 2 - cantCollectText.dstR.w / 2;
    cantCollectText.dstR.y = 250;
    canSleepAndGoShopText.setText(renderer, robotoF, "You can sleep and go shop only in the night");
    canSleepAndGoShopText.dstR.w = 220;
    canSleepAndGoShopText.dstR.h = 40;
    canSleepAndGoShopText.dstR.x = windowWidth / 2 - canSleepAndGoShopText.dstR.w / 2;
    canSleepAndGoShopText.dstR.y = infoText.dstR.y - canSleepAndGoShopText.dstR.h;
    readData();
    leafClock.restart();
    globalClock.restart();
    rotClock.restart();
    grapeClock.restart();
    appleClock.restart();
    bananaClock.restart();
    intro.introClock.restart();
    timeClock.restart();
    potatoClock.restart();
    pumpkinClock.restart();
    HomeInit();
    for (auto& food : foods) {
        food = Food::Empty;
    }
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainLoop, 0, 1);
#else
    while (running) {
        mainLoop();
    }
#endif
    // TODO: On mobile remember to use eventWatch function (it doesn't reach this code when terminating)
    return 0;
}