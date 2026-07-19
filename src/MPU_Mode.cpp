/* Copyright (C) 2026  Samuel Trassare (https://github.com/Tiogaplanet)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/** 
 * @file MPU_Mode.cpp
 * @brief Implements switching between and querying the various game/app modes
 *        of the WowWee MiP robot.
 *
 * Modes include App, Cage, Dance, Stack, Trick, and Roam. All changes are
 * verified with read-back confirmation and automatic retries where possible.
 */
#include "MPU_D1_mini.h"

// MiP Protocol Commands related to modes.
// These command codes are placed in the first byte of requests sent to the MiP
// and responses sent back from the MiP. See
// https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md
// for the complete list.
#define MIP_CMD_SET_GAME_MODE 0x76
#define MIP_CMD_GET_GAME_MODE 0x82

void MiP::enableAppMode() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Mode->enableAppMode()");
  verifiedSetGameMode(MIP_APP_MODE);
}
void MiP::enableCageMode() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Mode->enableCageMode()");
  verifiedSetGameMode(MIP_CAGE_MODE);
}
void MiP::enableDanceMode() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Mode->enableDanceMode()");
  verifiedSetGameMode(MIP_DANCE_MODE);
}
void MiP::enableStackMode() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Mode->enableStackMode()");
  verifiedSetGameMode(MIP_STACK_MODE);
}
void MiP::enableTrickMode() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Mode->enableTrickMode()");
  verifiedSetGameMode(MIP_TRICK_MODE);
}
void MiP::enableRoamMode() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Mode->enableRoamMode()");
  verifiedSetGameMode(MIP_ROAM_MODE);
}

bool MiP::isAppModeEnabled() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Mode->isAppModeEnabled()");
  return checkGameMode(MIP_APP_MODE);
}
bool MiP::isCageModeEnabled() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Mode->isCageModeEnabled()");
  return checkGameMode(MIP_CAGE_MODE);
}
bool MiP::isDanceModeEnabled() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Mode->isDanceModeEnabled()");
  return checkGameMode(MIP_DANCE_MODE);
}
bool MiP::isStackModeEnabled() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Mode->isStackModeEnabled()");
  return checkGameMode(MIP_STACK_MODE);
}
bool MiP::isTrickModeEnabled() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Mode->isTrickModeEnabled()");
  return checkGameMode(MIP_TRICK_MODE);
}
bool MiP::isRoamModeEnabled() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Mode->isRoamModeEnabled()");
  return checkGameMode(MIP_ROAM_MODE);
}

bool MiP::checkGameMode(MiPGameMode expectedMode) {
  MIP_DEBUG_INFO_PRINTLN("MiP->Mode->checkGameMode()");
  int8_t result;
  for (uint8_t retry = 0; retry < MIP_MAX_RETRIES; retry++) {
    MiPGameMode currentMode;
    result = rawGetGameMode(currentMode);
    if (result == MIP_ERROR_NONE)
      return currentMode == expectedMode;

    // An error was encountered so we will loop around and try again.
    // Wait for a bit before the next retry.
    delay(MIP_RETRY_WAIT);
  }
  m_lastError = result;
  return false;
}

// This internal protected method sends the command to change the game mode and
// then sends a request to get the new mode. If this request fails or the new
// mode isn't as expected, it will retry the command.
void MiP::verifiedSetGameMode(MiPGameMode desiredMode) {
  int8_t result;
  for (uint8_t retry = 0; retry < MIP_MAX_RETRIES; retry++) {
    rawSetGameMode(desiredMode);

    // Read back and make sure that it was set as expected.
    MiPGameMode actualMode;
    result = rawGetGameMode(actualMode);
    if (result == MIP_ERROR_NONE && actualMode == desiredMode) {
      // The set was successful so return immediately.
      m_lastError = MIP_ERROR_NONE;
      return;
    }

    // An error was encountered so we will loop around and try again.
    // Wait for a bit before the next retry.
    delay(MIP_RETRY_WAIT);
  }

  if (result != MIP_ERROR_NONE) {
    // Kept getting an error back from rawGetGameMode().
    m_lastError = result;
  } else {
    // rawGetGameMode() was successful but didn't match mode to which we were
    // attempting to change.
    m_lastError = MIP_ERROR_MAX_RETRIES;
  }
}

// This internal protected method sends the set game mode command with no error
// checking. The error handling / recovery happens at a higher level of the
// driver.
void MiP::rawSetGameMode(MiPGameMode mode) {
  // Might not accept command if currently running another game mode so Stop
  // first.
  stop();

  uint8_t command[1 + 1] = {MIP_CMD_SET_GAME_MODE, mode};
  rawSend(command, sizeof(command));
}

// This internal protected method sends the get game mode command with minimal
// error handling. The error recovery happens at a higher level of the driver.
int8_t MiP::rawGetGameMode(MiPGameMode& mode) {
  const uint8_t getGameMode[1] = {MIP_CMD_GET_GAME_MODE};
  uint8_t response[1 + 1];
  size_t responseLength;

  // Might not accept get game mode command when currently running a game mode
  // so Stop first.
  stop();

  int8_t result = rawReceive(getGameMode,
                             sizeof(getGameMode),
                             response,
                             sizeof(response),
                             responseLength);
  if (result)
    return result;
  if (responseLength != 2 || response[0] != MIP_CMD_GET_GAME_MODE ||
      (response[1] != MIP_APP_MODE && response[1] != MIP_CAGE_MODE &&
       response[1] != MIP_TRACKING_MODE && response[1] != MIP_DANCE_MODE &&
       response[1] != MIP_DEFAULT_MODE && response[1] != MIP_STACK_MODE &&
       response[1] != MIP_TRICK_MODE && response[1] != MIP_ROAM_MODE)) {
    return MIP_ERROR_BAD_RESPONSE;
  }
  mode = (MiPGameMode)response[1];

  // Restart the game mode now that we have successfully retrieved it.
  rawSetGameMode(mode);

  return MIP_ERROR_NONE;
}
