#include <cassert>

#include "Screen.hpp"

Screen::Screen(size_t _width, size_t _height, size_t xBlocks, size_t yBlocks) :
	width(_width), height(_height), blockWidth(width / xBlocks), bgColor(0, 0, 0), bottomRight(xBlocks, yBlocks)
{
	// the blocks should fit evenly into the screen with no leftover space
	assert((width % xBlocks) == 0 && (height % yBlocks) == 0);
	// there should be a "center" block in both dimensions
	assert((bottomRight.x % 2) == 0 && (bottomRight.y % 2) == 0);
	// blocks should be square, not rectangular
	assert(blockWidth == (height / yBlocks));

	// TODO: check for errors in return value
	screen = SDL_SetVideoMode(width, height, 0, SDL_ANYFORMAT | SDL_SWSURFACE);
}

SDL_Surface* Screen::GetSurface()
{
	++(screen->refcount);
	return screen;
}

Point Screen::ResolveIndex(Point p) const
{
	return Point(blockWidth * p.x, blockWidth * p.y);
}
void Screen::Update()
{
	// TODO: check return value
	SDL_Flip(screen);
}
void Screen::Clear()
{
	SDL_Rect blank;
	blank.x = blank.y = 0;
	blank.w = width;
	blank.h = height;
	SDL_FillRect(screen, &blank, SDL_MapRGB(screen->format, bgColor.red, bgColor.green, bgColor.blue));
}
void Screen::DrawRect(const Point& loc, const Color24& color)
{
	SDL_Rect rect;
	rect.h = rect.w = blockWidth;
	Point realIndex = ResolveIndex(loc);
	rect.x = realIndex.x;
	rect.y = realIndex.y;

	// TODO: check for errors here
	SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, color.red, color.green, color.blue));
}