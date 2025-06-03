#pragma once

#include <Windows.h>
#include <string>
#include <conio.h>

#include "shell.h"

#define MARQUEE_RIGHT 1
#define MARQUEE_UP 1
#define MARQUEE_LEFT -1
#define MARQUEE_DOWN -1

class Marquee {
public:
	/**
	 * @brief Get screen buffer info to initialize screen parameters.
	 * @param refreshRate Set screen's refresh rate.
	 * @param message Set marquee message.
	 */
	Marquee(int refreshRate, const std::string message);

	/**
	 * @brief Start marquee program interactive loop.
	 */
	void start();

	/**
	 * @brief Append a character to the input string.
	 * @param c Character to append to input string.
	 */
	void addChar(char c);

	/**
	 * @brief Remove a character from the input string.
	 */
	void deleteChar();

	/**
	 * @brief Set the width of the terminal.
	 */
	void setWidth(int width);

	/**
	 * @brief Set the height of the terminal.
	 */
	void setHeight(int height);

	/**
	 * @brief Set the screen's update speed.
	 * @param n Frequency of display refresh in frames per second.
	 */
	void setRefreshRate(int n);

	/**
	 * @brief Set the message to be displayed in the marquee.
	 * @param message Message to be displayed in the marquee.
	 */
	void setMessage(const std::string message);

	/**
	 * @brief Get frequency of screen redraw in milliseconds (1000 / refresh rate).
	 */
	int getRefreshRate() const;

	/**
	 * @brief Get message to display in marquee.
	 */
	std::string getMessage() const;

	/**
	 * @brief Get current input.
	 */
	std::string getInput() const;

	/**
	 * @brief Get previously processed input.
	 */
	std::string getProcessedInput() const;

	/**
	 * @brief Get X-coordinate of marquee text.
	 */
	int getXPos() const;

	/**
	 * @brief Get Y-coordinate of marquee text.
	 */
	int getYPos() const;

	/**
	 * @brief Get screen width.
	 */
	int getWidth() const;

	/**
	 * @brief Get screen height.
	 */
	int getHeight() const;

	/**
	 * @brief Get the horizontal direction of the marquee.
	 */
	int getXDir() const;

	/**
	 * @brief Get the vertical direction of the marquee.
	 */
	int getYDir() const;

	/**
	 * @brief Check if user requests quit.
	 */
	bool isQuit() const;

	/**
	 * @brief Set quit flag.
	 * @param q Value to set flag to.
	 */
	void setQuit(bool q);

	/**
	 * @brief Set cursor position in console to a given set of coordinates.
	 * @param x X-coordinate to move cursor to.
	 * @param y Y-coordinate to move cursor to.
	 */
	void setCursorPos(int x, int y);

	/**
	 * @brief Set input to a given string.
	 * @param input Message to set string to.
	 */
	void setInput(const std::string input);

	/**
	 * @brief Set previously processed input to a given string.
	 * @param input Message to set previously processed string to.
	 */
	void setProcessedInput(const std::string input);

	/**
	 * @brief Set marquee x-coordinate.
	 * @param x X-coordinate to display marquee at.
	 */
	void setXPos(int x);

	/**
	 * @brief Set marquee y-coordinate.
	 * @param y Y-coordinate to display marquee at.
	 */
	void setYPos(int y);

	/**
	 * @brief Toggle the horizontal direction of the marquee.
	 */
	void setXDir(int dir);

	/**
	 * @brief Toggle the vertical direction of the marquee.
	 */
	void setYDir(int dir);

	/**
	 * @brief Process current input and move it to previously processed input.
	 */
	void processInput();

	/**
	 * @brief Move marquee based on x-direction and y-direction.
	 */
	void moveMarquee();

private:
	std::string processedInput; // store last processed command.
	std::string input; // screen is redrawn every frame, so input needs to be stored. cin won't work.
	int xPos; // x-coordinate of marquee text.
	int yPos; // y-coordinate of marquee text.
	int width; // width of the screen that marquee will bounce around.
	int height; // height of the screen that marquee will bounce around.
	int refreshRate; // frequency of screen redraw.
	std::string message; // marquee message to display.

	int xDir; // horizontal direction of the marquee.
	int yDir; // vertical direction of the marquee.

	bool quit; // track if user requests to quit to main shell.
};