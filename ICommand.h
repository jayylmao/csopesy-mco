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

	ICommand(Type type);
	
	/**
	 * @brief Get the type of command.
	 */
	Type getType();

	/**
	 * @brief Execute the instruction. The action performed depends on the implementation.
	 */
	virtual void execute();

protected:
	Type type; // command type.
};