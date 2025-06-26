#include "Process.h"
#include <limits>

Process::Process(const std::string& name, int pid, int instructionCount)
	: name(name), pid(pid), totalInstructions(instructionCount), coreId(-1), finished(false)
{
	// store creation timestamp.
	auto now = std::chrono::system_clock::now();
	std::time_t raw_time = std::chrono::system_clock::to_time_t(now);
	std::tm* time_info = std::localtime(&raw_time);

	std::ostringstream oss;
	oss << std::put_time(time_info, "%m/%d/%Y, %I:%M:%S %p");
	creationTimestamp = oss.str();

	symbolTable["var1"] = 0;
	symbolTable["var2"] = 0;
	symbolTable["var3"] = 0;
	symbolTable["var4"] = 0;
	symbolTable["var5"] = 0;
	symbolTable["var6"] = 0;
	symbolTable["var7"] = 0;
	symbolTable["var8"] = 0;
	symbolTable["var9"] = 0;
	symbolTable["var10"] = 0;

	createInstructions();
}

void Process::createInstructions()
{
	const int NUM_TYPES = 6; // Adjust as needed: PRINT, DECLARE, ADD, etc. // 0-5
	int randVal;

	for (int i = 1; i <= totalInstructions; ++i) {
		randVal = rand() % NUM_TYPES; 

		auto cmd = createCommand(randVal);
		instructionQueue.push(std::move(cmd));
	}
}

void Process::executeInstruction()
{
	std::lock_guard<std::mutex> lock(mtx);
	if (!instructionQueue.empty()) {
		auto cmd = instructionQueue.front();
		instructionQueue.pop();

		if (cmd) {
			cmd->execute(*this);
			//THIS IS THE NEW ONE CAN REMOVE IF NOT WORKING|| BELOW THIS
			if (cmd->getType() == ICommand::PRINT) {
				auto printCmd = std::dynamic_pointer_cast<PrintCommand>(cmd);
				if (printCmd) {
					// Use this->coreId!
					auto now = std::chrono::system_clock::now();
					std::time_t now_c = std::chrono::system_clock::to_time_t(now);
					std::tm local_tm = *std::localtime(&now_c);
					std::ostringstream oss;
					oss << "(" << std::put_time(&local_tm, "%m/%d/%Y %I:%M:%S%p")
						<< ") Core: " << coreId
						<< " \"" << printCmd->getMsg() << "\"";
					logs.push_back(oss.str());
				}
			}
			//ABOVE THIS
		}

		if (instructionQueue.empty()) {
			finished = true;
		}
	}
}

std::shared_ptr<ICommand> Process::createCommand(int type)
{
	switch (type) {
	case ICommand::PRINT: {
		std::string msg = "Hello world from " + this->name + "!";
		return std::make_unique<PrintCommand>(msg);
		}
	case ICommand::DECLARE:{
		auto it = symbolTable.begin();
		std::advance(it, rand() % symbolTable.size());

		std::string varName = it->first;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<uint16_t> dist(0, std::numeric_limits<uint16_t>::max());

		uint16_t randomValue = dist(gen);

		return std::make_unique<DeclareCommand>(varName, randomValue);
		}
	case ICommand::ADD:{
		std::vector<std::string> vars;
		for (const auto& kv : symbolTable)
			vars.push_back(kv.first);

			auto randomVar = [&]() {
				return vars[rand() % vars.size()];
			};

			return std::make_unique<AddCommand>(randomVar(), randomVar(), randomVar());
		}
	case ICommand::SUBTRACT:{
		std::vector<std::string> vars;
		for (const auto& kv : symbolTable)
			vars.push_back(kv.first);

		auto randomVar = [&]() {
			return vars[rand() % vars.size()];
		};

			return std::make_unique<SubtractCommand>(randomVar(), randomVar(), randomVar());
		}
	case ICommand::SLEEP:{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<unsigned int> dist(1, std::numeric_limits<uint8_t>::max());
		
		uint8_t sleepTicks = static_cast<uint8_t>(dist(gen));
		return std::make_unique<SleepCommand>(sleepTicks);
	}
	case ICommand::FOR: {
		int nesting = rand() % 3 + 1; // 1–3 depth
		int repeatCount = rand() % 5 + 1;

		std::vector<std::shared_ptr<ICommand>> body;

		for (int i = 0; i < 2; ++i) {
			int type = rand() % 5; // exclude FOR to avoid infinite recursion
			body.push_back(createCommand(type)); // nested command
		}

		return std::make_unique<SleepCommand>(10);
	}
	default:
		throw std::invalid_argument("Unknown command type");
	}
}

bool Process::hasFinished() const
{
	std::lock_guard<std::mutex> lock(mtx);
	return finished;
}

void Process::setFinished(bool value)
{
	std::lock_guard<std::mutex> lock(mtx);
	finished = value;
}

int Process::getCurrentLine()
{
	std::lock_guard<std::mutex> lock(mtx);
	return this->totalInstructions - this->instructionQueue.size();
}

int Process::getPID()
{
	return this->pid;
}

std::string Process::getName()
{
	return this->name;
}

std::string Process::getCreationTimestamp() {
	
	return this->creationTimestamp;
}

void Process::setCoreId(int id)
{
	std::lock_guard<std::mutex> lock(mtx);
	coreId = id;
}

int Process::getCoreId() const
{
	std::lock_guard<std::mutex> lock(mtx);
	return coreId;
}

int Process::getTotalLines() const
{
	return totalInstructions;
}

uint16_t Process::getVar(std::string name)
{
	auto var = symbolTable.find(name);

	if (var == symbolTable.end()) {
		return 0;
	}
	else {
		return var->second;
	}
}

void Process::setVar(std::string name, uint16_t val)
{
	symbolTable[name] = val;
}