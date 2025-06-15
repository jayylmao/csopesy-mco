#pragma once

/**
 * @class ICommand
 * @brief Interface for commands to implement.
 */
class ICommand {
public:
	enum Type
	{
		PRINT,
		DECLARE,
		ADD,
		SUBTRACT,
		SLEEP,
		FOR
	};

	ICommand(int pid, Type type);
	
	/**
	 * @brief Get the type of command.
	 */
	Type getType() const;

	/**
	 * @brief Execute the instruction. The action performed depends on the implementation.
	 */
	virtual void execute();

protected:
	int pid;
	Type type; // command type.
};