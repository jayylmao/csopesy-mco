#pragma once
#include <string>

class Process;

/**
 * @class ICommand
 * @brief Interface for commands to implement.
 */
class ICommand {
public:
	enum Type
	{
		PRINT, //0
		DECLARE,
		ADD,
		SUBTRACT,
		SLEEP,
		FOR //5
	};

	ICommand(Type type);
	virtual ~ICommand() = default; // Add virtual destructor for proper cleanup
	
	/**
	 * @brief Get the type of command.
	 */
	Type getType() const;

	/**
	 * @brief Execute the instruction. The action performed depends on the implementation.
	 */
	virtual void execute(Process& process);

	std::string getTimestamp();

protected:
	std::string creationTimestamp;
	Type type; // command type.
};