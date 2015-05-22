#pragma once

#include <AntTweakBar.h>
#include <iostream>
#include <fstream>
#include <string>
#include <istream>
#include <ostream>
#include <iterator>
#include <sstream>
#include <algorithm>

#include "icg_common.h"
#include "../camera/camera.h"
#include "../app/main.h"
#include "../scene/grid.h"
#include "../buffers/framebuffer.h"
#include "../noise/noise_quad.h"
#include "../noise/noise_generator.h"
#include "../scene/water.h"
#include "../scene/box.h"
#include "../app/constants.h"
#include "../tw/io.h"

void initAntTwBar(GridParams* grid_params, WindowParams* window_params, NoiseParams* noise_params, WaterParams* water_params, TextureParams* texture_params);
void twBarVisible(bool b);

// --- Value Callbacks --- //

void TW_CALL setIntParamCallback(const void* value, void* clientData);
void TW_CALL setFloatParamCallback(const void* value, void* clientData);
void TW_CALL setBoolParamCallback(const void* value, void* clientData);
void TW_CALL getIntParamCallback(void* value, void* clientData);
void TW_CALL getFloatParamCallback(void* value, void* clientData);
void TW_CALL getBoolParamCallback(void* value, void* clientData);

// --- Save Functions --- //

void TW_CALL CopyStdStringToClient(std::string& destinationClientString, const std::string& sourceLibraryString);
void TW_CALL SaveCB(void * clientData);
void TW_CALL LoadCB(void * clientData);