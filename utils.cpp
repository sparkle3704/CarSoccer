#pragma once
#include "utils.h"
#include <iostream>
#include "SDL_Contexts.h"
#include "SDL2/SDL.h"
#include <unordered_map>
#include <map>
#include <tuple>

std::vector<int> vFonts;
std::pair<TTF_Font*, TTF_Font*> FONT[300];
std::vector<SDL_Surface*> vSurfaces;

SmartSurface::SmartSurface(TTF_Font* font, std::string textureText, SDL_Color textColor) :
    m_surface(TTF_RenderText_Blended(font, textureText.c_str(), textColor))
{}

SmartSurface::~SmartSurface() {
//    free();
}

void SmartSurface::free() {
    SDL_FreeSurface(m_surface);
    m_surface = nullptr;
}

SDL_Surface* SmartSurface::GetSDLSurface() {
    return m_surface;
}

SmartText::SmartText() {
    texture = nullptr;
    w = 0;
    h = 0;
    x = 0;
    y = 0;
}

void SmartText::free() {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
        w = 0;
        h = 0;
    }
}

bool SmartText::valid() {
    if (texture != nullptr) {
        return 1;
    }
    return 0;
}

SmartText::~SmartText() {
    free();
}

bool SmartText::loadFromFile(std::string path) {
  // Get rid of preexisting texture
  free();
  // The final texture
  SDL_Texture* newTexture = NULL;

  // Load image at specified path
  SDL_Surface* loadedSurface = IMG_Load(path.c_str());
  if (nullptr == loadedSurface) {
    std::cerr << "Unable to load image " << path.c_str() << ", " <<  "SDL_image Error: " << IMG_GetError() << "\n";
  } else {
    // Color key image
    SDL_SetColorKey(loadedSurface, SDL_TRUE,
                    SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
    // Create texture from surface pixel
    newTexture = SDL_CreateTextureFromSurface(renderer.get(), loadedSurface);
    if (nullptr == newTexture) {
      std::cerr << "Unable to create texture from " << path.c_str() << ", " << "SDL Error: " << SDL_GetError() << "\n";
    } else {
      // Get image dimensions
      w = loadedSurface->w;
      h = loadedSurface->h;
    }

    // Get rid of old loaded surface
    vSurfaces.push_back(loadedSurface);
//    SDL_FreeSurface(loadedSurface);
//    loadedSurface = nullptr;
  }

  // return success
  texture = newTexture;
  return (nullptr != texture);
}

bool setOutlineFont(TTF_Font*& font, int thickness = 2) {
    if (font == nullptr) {
        return 0;
    }
    TTF_SetFontOutline(font, thickness);
    return 1;
}

int SmartText::getX() {
    return x;
}

int SmartText::getY() {
    return y;
}

int SmartText::getW() {
    return w;
}

int SmartText::getH() {
    return h;
}

/// luu lai cac texture ung voi cac text, tao class rieng cho surface https://stackoverflow.com/questions/20145347/sdl2-memory-leaks-c
//bool SmartText::loadFromRenderedText(TTF_Font* font, std::string textureText, SDL_Color textColor) {
//  // Get rid of preexisting texture
//  free();
//
//  // Render text surface
//  SDL_Surface* textSurface = TTF_RenderText_Blended(font, textureText.c_str(), textColor);
//  if (nullptr == textSurface) {
//    std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << "\n";
//  } else {
//    // Create texture from surface pixels
//    texture = SDL_CreateTextureFromSurface(renderer.get(), textSurface);
//    if (nullptr == texture) {
//      std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << "\n";
//
//    } else {
//      // Get image dimensions
//      w = textSurface->w;
//      h = textSurface->h;
//    }
//
//    // Get rid of old surface
//    vSurfaces.push_back(textSurface);
////    SDL_FreeSurface(textSurface);
////    textSurface = nullptr;
//  }
//
//  // Return success
//  return texture != NULL;
//}

bool SmartText::loadFromRenderedText(TTF_Font* font, std::string textureText, SDL_Color textColor) {
  // Get rid of preexisting texture
  free();

  // Create SmartSurface from font, textureText, and textColor
  SmartSurface mySurface(font, textureText, textColor);

  // Create texture from SmartSurface pixels
  texture = SDL_CreateTextureFromSurface(renderer.get(), mySurface.GetSDLSurface());

  if (nullptr == texture) {
    std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << "\n";
    return 0;

  } else {
    // Get image dimensions
    w = mySurface.GetSDLSurface()->w;
    h = mySurface.GetSDLSurface()->h;

//    mySurface.free();
    vSurfaces.push_back(mySurface.GetSDLSurface());
    return 1;
  }
}

//bool compareKeys(const std::tuple<TTF_Font*, std::string, SDL_Color>& lhs, const std::tuple<TTF_Font*, std::string, SDL_Color>& rhs) {
//  if (std::get<0>(lhs) != std::get<0>(rhs)) {
//    return std::get<0>(lhs) < std::get<0>(rhs);
//  } else if (std::get<1>(lhs) != std::get<1>(rhs)) {
//    return std::get<1>(lhs) < std::get<1>(rhs);
//  } else {
//      if (std::get<2>(lhs).r != std::get<2>(rhs).r) {
//        return std::get<2>(lhs).r < std::get<2>(rhs).r;
//      } else if (std::get<2>(lhs).g != std::get<2>(rhs).g) {
//        return std::get<2>(lhs).g < std::get<2>(rhs).g;
//      } else {
//        return std::get<2>(lhs).b < std::get<2>(rhs).b;
//      }
//  }
//}
//
//std::unordered_map<std::tuple<TTF_Font*, std::string, SDL_Color>, SDL_Texture*, decltype(&compareKeys)> textureMap(compareKeys);
//bool SmartText::loadFromRenderedText(TTF_Font* font, std::string textureText, SDL_Color textColor) {
//    auto key = std::make_tuple(font, textureText, textColor);
//    auto it = textureMap.find(key);
//    if (it != textureMap.end()) {
//        // Texture already exists, set it as the current texture and return success
//        texture = it->second;
//        SDL_QueryTexture(texture, NULL, NULL, &w, &h);
//        return true;
//    }
//
////    free();
//
//    // Texture doesn't exist, create it and store it in the map
//    SmartSurface mySurface(font, textureText, textColor);
//    texture = SDL_CreateTextureFromSurface(renderer.get(), mySurface.GetSDLSurface());
//    if (nullptr == texture) {
//        std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << "\n";
//        return false;
//    }
//
//    // Get image dimensions
//    w = mySurface.GetSDLSurface()->w;
//    h = mySurface.GetSDLSurface()->h;
//
//    // Store the texture in the map
//    textureMap[key] = texture;
////    vSurfaces.push_back(mySurface.GetSDLSurface());
//
////    mySurface.free();
//    // Return success
//    return true;
//}


//bool SmartText::loadFromRenderedText(TTF_Font* font, std::string textureText, SDL_Color textColor) {
//  // Get rid of preexisting texture
//  free();
//
//  // Create SmartSurface from font, textureText, and textColor
//  SmartSurface mySurface(font, textureText, textColor);
//
//  // Create texture from SmartSurface pixels
//  texture = SDL_CreateTextureFromSurface(renderer.get(), mySurface.GetSDLSurface());
//  if (nullptr == texture) {
//    std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << "\n";
//
//  } else {
//    // Get image dimensions
//    w = mySurface.GetSDLSurface()->w;
//    h = mySurface.GetSDLSurface()->h;
//  }
//
//  // Get rid of old surface
//  vSurfaces.push_back(&mySurface);
//
//  // Return success
//  return texture != NULL;
//}



LTimer::LTimer() {
  // Initialize the variables
  mStartTicks = 0;
  mPausedTicks = 0;

  mPaused = false;
  mStarted = false;
}


void SmartText::render(int x, int y) {
    SDL_Rect renderQuad = {x - w*0.5, y - h*0.5, w, h};
    SDL_RenderCopy(renderer.get(), texture, NULL, &renderQuad);
}

void SmartText::renderLeftMiddle(int x, int y) {
    SDL_Rect renderQuad = {x, y - h*0.5, w, h};
    SDL_RenderCopy(renderer.get(), texture, NULL, &renderQuad);
}

void LTimer::start() {
  // Start the timer
  mStarted = true;

  // Unpaused the timer
  mPaused = false;

  // Get the current clock time
  mStartTicks = SDL_GetTicks();
  mPausedTicks = 0;
}


void LTimer::stop() {
  // Stop the timer
  mStarted = false;

  // Unpaused the timer
  mPaused = false;

  // Clear tick variables
  mStartTicks = 0;
  mPausedTicks = 0;
}

void LTimer::paused() {
  // if the timer is running and isn't already paused
  if (mStarted && !mPaused) {
    // Pause the timer
    mPaused = true;

    // Calculate the paused ticks
    mPausedTicks = SDL_GetTicks() - mStartTicks;
    mStartTicks = 0;
  }
}

void LTimer::unpaused() {
  if (mStarted && mPaused) {
    // Unpause the timer
    mPaused = false;

    // Reset the starting ticks
    mStartTicks = SDL_GetTicks() - mPausedTicks;

    // Reset the paused ticks
    mPausedTicks = 0;
  }
}

Uint64 LTimer::getTicks() {
  Uint64 time = 0;

  if (mStarted) {
    // Return the number of ticks when the timer was paused
    if (mPaused) {
      time = mPausedTicks;
    } else {
      // Return the current time minus the start time
      time = SDL_GetTicks() - mStartTicks;
    }
  }
  else {
    time = SDL_GetTicks() - mStartTicks;
  }

  return time;
}

bool LTimer::isStarted() {
  // Timer is running and paused or unpaused
  return mStarted;
}

bool LTimer::isPaused() {
  // Timer is running and paused
  return mPaused && mStarted;
}

void closeFonts() {
    for (auto& i : vFonts) {
        if (FONT[i].first != nullptr) {
            TTF_CloseFont(FONT[i].first);
            FONT[i].first = nullptr;
        }
        if (FONT[i].second != nullptr) {
            TTF_CloseFont(FONT[i].second);
            FONT[i].second = nullptr;
        }
    }
}

TTF_Font* getFont(int fontSize, bool outline, int thickness) {
    if (outline == 0) {
        if (FONT[fontSize].first == nullptr) {
            FONT[fontSize].first = TTF_OpenFont(fontPath.c_str(), fontSize);
            if (FONT[fontSize].second == nullptr) {
                vFonts.push_back(fontSize);
            }
        }
        return FONT[fontSize].first;
    }
    else {
        if (FONT[fontSize].second == nullptr) {
            FONT[fontSize].second = TTF_OpenFont(fontPath.c_str(), fontSize);
            if (FONT[fontSize].first == nullptr) {
                vFonts.push_back(fontSize);
            }
            TTF_SetFontOutline(FONT[fontSize].second, thickness);
        }
        return FONT[fontSize].second;
    }
}

void freeSurfaces() {
    for (auto& x : vSurfaces) {
        if (x != nullptr) {
            SDL_FreeSurface(x);
            x = nullptr;
        }
    }
    vSurfaces.clear();
}

