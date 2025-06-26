#pragma once
#include "ICommand.h"
#include <vector>
#include <memory>

class ForCommand : public ICommand {
public:
	ForCommand(std::vector<std::unique_ptr<ICommand>> instructions, int repeats);

	void execute(Process& process) override;

private:
	int repeats;
	int depth; // track depth in for loop.
	std::vector<std::unique_ptr<ICommand>> instructions;
};
