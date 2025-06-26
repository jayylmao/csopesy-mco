#include "ForCommand.h"
#include "Process.h"

ForCommand::ForCommand(std::vector<std::unique_ptr<ICommand>> instructions, int repeats)
	: ICommand(Type::FOR), repeats(repeats), instructions(std::move(instructions)) {
}

void ForCommand::execute(Process& process) {
	for (int i = 0; i < repeats; ++i) {
		for (const auto& cmd : instructions) {
			if (cmd) {
				// Re-execute a clone of the command or re-run safely
				cmd->execute(process);
			}
		}
	}

	std::cout << "for loop starting " << std::endl;
	process.logs.push_back("FOR loop executed " + std::to_string(repeats) + " times.");
}


