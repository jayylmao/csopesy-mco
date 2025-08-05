#pragma once
#include "ICommand.h"
#include<vector>
#include <string>

class PrintCommandC : public ICommand {
public:

    explicit PrintCommandC(const std::vector<std::string>& elements, std::vector<std::string>& logs);

    explicit PrintCommandC(const std::string& msg, std::vector<std::string>& logs);


    void execute(Process& process) override;
    const std::string& getMsg() const { return msg; }

private:
    std::vector<std::string> printElements;
    std::vector<std::string>& logsToEdit;
    std::string msg;
    std::string var;
};