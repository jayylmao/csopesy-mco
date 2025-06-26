#include "ForCommand.h"
#include "Process.h"

ForCommand::ForCommand(std::vector<std::shared_ptr<ICommand>> instructions, int repeats)
	: ICommand(Type::FOR), repeats(repeats), body(std::move(body)) {
}

void ForCommand::execute(Process& process) {
	for (int i = 0; i < repeats; ++i) {
		for (const auto& cmd : body) {
			if (cmd) {
				// Re-execute a clone of the command or re-run safely
				cmd->execute(process);
			}
		}
	}

	process.logs.push_back("FOR loop executed " + std::to_string(repeats) + " times.");
}


