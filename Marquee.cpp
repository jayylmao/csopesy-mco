#include "Marquee.h"

constexpr short DISPLAY_OFFSET = 3;

Marquee::Marquee(short refreshRate, const std::string message)
{
	// get info from the screen buffer.
	CONSOLE_SCREEN_BUFFER_INFO buffer;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &buffer);

	// set screen width and height based on terminal size.
	setWidth(buffer.srWindow.Right - buffer.srWindow.Left + 1);
	setHeight(buffer.srWindow.Bottom - buffer.srWindow.Top + 1);

	// set refresh rate.
	setRefreshRate(refreshRate);

	// set marquee message.
	setMessage(message);

	// set inputs.
	setInput("");
	setProcessedInput("");

	// initialize marquee coordinates as (0, 0).
	setXPos(0);
	setYPos(0);

	setXDir(MARQUEE_RIGHT);
	setYDir(MARQUEE_DOWN);

	// reset quit flag.
	setQuit(false);
}

void Marquee::addChar(char c)
{
	input.append(1, c);
}

void Marquee::deleteChar()
{
	// do nothing if input is already empty.
	if (input.empty()) {
		return;
	}

	input.pop_back();
}

void Marquee::setWidth(short width)
{
	this->width = width;
}

void Marquee::setHeight(short height)
{
	// subtract to make room for header and prompt.
	this->height = height - DISPLAY_OFFSET;
}

void Marquee::setRefreshRate(short n)
{
	this->refreshRate = n;
}

void Marquee::setMessage(const std::string message)
{
	this->message = message;
}

void Marquee::setXPos(short x)
{
	if (x > getWidth()) {
		this->xPos = width;
	}
	else if (x < 0) {
		this->xPos = 0;
	}
	else {
		this->xPos = x;
	}
}

void Marquee::setYPos(short y)
{
	if (y > getHeight()) {
		this->yPos = getHeight();
	}
	else if (y < 0) {
		this->yPos = 0;
	}
	else {
		this->yPos = y;
	}
}


short Marquee::getRefreshRate() const
{
	return 1000 / this->refreshRate;
}

std::string Marquee::getMessage() const
{
	return this->message;
}

short Marquee::getXPos() const
{
	return this->xPos;
}

short Marquee::getYPos() const
{
	return this->yPos;
}

short Marquee::getWidth() const
{
	return this->width;
}

short Marquee::getHeight() const
{
	return this->height;
}

std::string Marquee::getInput() const
{
	return this->input;
}

std::string Marquee::getProcessedInput() const
{
	return this->processedInput;
}

short Marquee::getXDir() const
{
	return this->xDir;
}

short Marquee::getYDir() const
{
	return this->yDir;
}

bool Marquee::isQuit() const
{
	return this->quit;
}

void Marquee::setQuit(bool q)
{
	this->quit = q;
}

void Marquee::setCursorPos(short x, short y)
{
	// create set of coordinates from given x and y.
	COORD c = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void Marquee::setProcessedInput(const std::string input)
{
	this->processedInput = input;
}

void Marquee::setInput(const std::string input)
{
	this->input = input;
}

void Marquee::setXDir(short dir)
{
	this->xDir = dir;
}

void Marquee::setYDir(short dir)
{
	this->yDir = dir;
}

void Marquee::processInput()
{
	if (_kbhit()) {
		char c = getch();

		// process input if carriage return is detected.
		if (c == '\r') {
			setProcessedInput(getInput());
			setInput("");
			std::system("cls");
		}
		else if (c == '\b') {
			deleteChar();
		}
		else {
			addChar(c);
		}

		// exit marquee console if command is given.
		if (getProcessedInput() == "exit") {
			setQuit(true);
		}
		else if (getProcessedInput() == "clear") {
			std::system("cls");
			setProcessedInput("");
		}
	}
}

void Marquee::moveMarquee()
{
	// increment x and y positions.
	setXPos(getXPos() + getXDir());
	setYPos(getYPos() + getYDir());

	// check x direction.
	if (getXPos() + getMessage().length() >= getWidth()) {
		setXDir(MARQUEE_LEFT);
	}
	if (getXPos() <= 0) {
		setXDir(MARQUEE_RIGHT);
	}
	
	// check y direction.
	if (getYPos() >= getHeight() - DISPLAY_OFFSET - 2) {
		setYDir(MARQUEE_DOWN);
	}
	if (getYPos() <= 0) {
		setYDir(MARQUEE_UP);
	}
}

void Marquee::start()
{
	Shell::clear();
	while (!isQuit()) {
		int currLine = 0;
		const std::string prompt = "user ~/marquee > ";

		// refresh screen
		Sleep(getRefreshRate());
		std::system("cls");
		
		// display header.
		setCursorPos(0, currLine);
		std::cout << "********************************" << std::endl
				  << "* Displaying a marquee console *" << std::endl
				  << "********************************" << std::endl;

		moveMarquee();
		setCursorPos(getXPos(), getYPos() + DISPLAY_OFFSET);
		std::cout << getMessage();

		currLine = getHeight();

		// display prompt.
		if (!getProcessedInput().empty()) {
			setCursorPos(0, currLine);
			std::cout << "[History] " << prompt << getProcessedInput() << std::endl;
			currLine++;
		}

		setCursorPos(0, currLine);
		std::cout << prompt << getInput() << std::endl;
		setCursorPos(prompt.length() + getInput().length(), currLine);

		processInput();
	}
	Shell::clear();
}