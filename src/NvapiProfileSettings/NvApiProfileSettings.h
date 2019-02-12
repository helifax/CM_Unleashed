/*
* Copyright (c) Helifax 2019
*/

#ifndef __NVAPI_PROFILER_H__
#define __NVAPI_PROFILER_H__

// One function to load all the settings in the nvidia driver settings :)
bool NvApi_3DVisionProfileSetup(std::string gameExe, uint32_t ProfileValue, uint32_t ConvergenceValue, std::string Comments);

#endif