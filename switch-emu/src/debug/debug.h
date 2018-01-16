#pragma once

namespace debug {

void init();

void debugBreakHandler();
void resumeFromDebugBreak();
void resumeFromDebugBreakForSingleStep();

} // namespace debug
