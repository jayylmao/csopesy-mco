#pragma once
#include "ICommand.h"
#include<vector>
#include <string>

class PrintCommand : public ICommand {
public:
   
    explicit PrintCommand(const std::vector<std::string>& elements);

    explicit PrintCommand(const std::string& msg);
    

    void execute(Process& process) override;
	const std::string& getMsg() const { return msg; }

private:
    std::vector<std::string> printElements;
    std::string msg;
    std::string var;
    uint16_t val;
};