#pragma once
#include "loader\loader.h"
#include "types\KProcess.h"
#include "graphicsDriver.h"

namespace kernel {

bool init();
void exit();

uint32_t* getCommandBuffer();

void setGameCartPath(const std::string& path);
std::string getGameCartPath();

bool launchGameCart();
void stopGame();

bool isGameRunning();
ObjectPtr<KProcess> getCurrentProcess();
handle_t getCurrentProcessHandle();
kernel::loader::Loader& getLoader();

// TODO seperate .h?
void setGraphicsDriver(GraphicsDriver* driver);
GraphicsDriver* getGraphicsDriver();

void cpuEntry();

} // namespace kernel