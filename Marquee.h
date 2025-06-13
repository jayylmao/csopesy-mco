#pragma once

#include <Windows.h>
#include <string>
#include <conio.h>
#include <thread>
#include <chrono>
#include "shell.h"

constexpr short MARQUEE_RIGHT = 1;
constexpr short MARQUEE_UP = 1;
constexpr short MARQUEE_LEFT = -1;
constexpr short MARQUEE_DOWN = -1;

class Marquee {
public:
	/**
	 * @brief Get screen buffer info to initialize screen parameters.
	 * @param refreshRate Set screen's refresh rate.
	 * @param message Set marquee message.
	 */
	Marquee(short refreshRate, const std::string message);

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
	void setWidth(short width);

	/**
	 * @brief Set the height of the terminal.
	 */
	void setHeight(short height);

	/**
	 * @brief Set the screen's update speed.
	 * @param n Frequency of display refresh in frames per second.
	 */
	void setRefreshRate(short n);

	/**
	 * @brief Set the message to be displayed in the marquee.
	 * @param message Message to be displayed in the marquee.
	 */
	void setMessage(const std::string message);

	/**
	 * @brief Get frequency of screen redraw in milliseconds (1000 / refresh rate).
	 */
	short getRefreshRate() const;

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
	short getXPos() const;

	/**
	 * @brief Get Y-coordinate of marquee text.
	 */
	short getYPos() const;

	/**
	 * @brief Get screen width.
	 */
	short getWidth() const;

	/**
	 * @brief Get screen height.
	 */
	short getHeight() const;

	/**
	 * @brief Get the horizontal direction of the marquee.
	 */
	short getXDir() const;

	/**
	 * @brief Get the vertical direction of the marquee.
	 */
	short getYDir() const;

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
	void setCursorPos(short x, short y);

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
	void setXPos(short x);

	/**
	 * @brief Set marquee y-coordinate.
	 * @param y Y-coordinate to display marquee at.
	 */
	void setYPos(short y);

	/**
	 * @brief Toggle the horizontal direction of the marquee.
	 */
	void setXDir(short dir);

	/**
	 * @brief Toggle the vertical direction of the marquee.
	 */
	void setYDir(short dir);

	/**
	 * @brief Process current input and move it to previously processed input.
	 */
	void processInput();

	/**
	 * @brief Move marquee based on x-direction and y-direction.
	 */
	void moveMarquee();

	/**
	 * @brief Redraw the display with new contents.
	 */
	void refreshDisplay();

private:
	std::string processedInput; // store last processed command.
	std::string input; // screen is redrawn every frame, so input needs to be stored. cin won't work.
	short xPos; // x-coordinate of marquee text.
	short yPos; // y-coordinate of marquee text.
	short width; // width of the screen that marquee will bounce around.
	short height; // height of the screen that marquee will bounce around.
	short refreshRate; // frequency of screen redraw.
	std::string message; // marquee message to display.

	int xDir; // horizontal direction of the marquee.
	int yDir; // vertical direction of the marquee.

	bool quit; // track if user requests to quit to main shell.
};