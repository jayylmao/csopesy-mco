#include "ForCommand.h"

#include <stdexcept>

ForCommand::ForCommand(std::vector<std::unique_ptr<ICommand>> instructions, int repeats)
	: ICommand(Type::FOR), repeats(repeats), instructions(std::move(instructions)) {
}

void ForCommand::addCommand(std::unique_ptr<ICommand> command) {
	if (depth + 1 > 3) {
		throw std::runtime_error("Maximum depth exceeded.");
	} else {
		command->depth = depth + 1;
		instructions.emplace_back(std::move(command));
	}
}

void ForCommand::execute(Process& process) {
	for (int i = 0; i < repeats; ++i) {
		for (auto& cmd : instructions) {
			if (cmd) {
				// Re-execute a clone of the command or re-run safely
				cmd->execute(process);
			}
		}
	}
}

