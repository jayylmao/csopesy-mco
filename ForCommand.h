#pragma once
#include "ICommand.h"
#include <vector>
#include <memory>

class Process;

class ForCommand : public ICommand {
public:
	ForCommand(std::vector<std::unique_ptr<ICommand>> instructions, int repeats);

	void addCommand(std::unique_ptr<ICommand> command);

	void execute(Process& process) override;

private:
	int repeats;
	std::vector<std::unique_ptr<ICommand>> instructions;
};
