/**
 * @file MPU_Sound.h
 * @brief Defines the public interface for sound playback in the MiP library.
 *
 * @details This header declares the sound API used to play audio and manage
 * volume.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MPU_SOUND_H
#define MPU_SOUND_H

#include <stdint.h>

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Sound effects and built-in audio clips.
 */
enum MiPSoundIndex : uint8_t {
  MIP_SOUND_ONEKHZ_500MS_8K16BIT = 1,
  MIP_SOUND_ACTION_BURPING,
  MIP_SOUND_ACTION_DRINKING,
  MIP_SOUND_ACTION_EATING,
  MIP_SOUND_ACTION_FARTING_SHORT,
  MIP_SOUND_ACTION_OUT_OF_BREATH,
  MIP_SOUND_BOXING_PUNCHCONNECT_1,
  MIP_SOUND_BOXING_PUNCHCONNECT_2,
  MIP_SOUND_BOXING_PUNCHCONNECT_3,
  MIP_SOUND_FREESTYLE_TRACKING_1,
  MIP_SOUND_MIP_1,
  MIP_SOUND_MIP_2,
  MIP_SOUND_MIP_3,
  MIP_SOUND_MIP_APP,
  MIP_SOUND_MIP_AWWW,
  MIP_SOUND_MIP_BIG_SHOT,
  MIP_SOUND_MIP_BLEH,
  MIP_SOUND_MIP_BOOM,
  MIP_SOUND_MIP_BYE,
  MIP_SOUND_MIP_CONVERSE_1,
  MIP_SOUND_MIP_CONVERSE_2,
  MIP_SOUND_MIP_DROP,
  MIP_SOUND_MIP_DUNNO,
  MIP_SOUND_MIP_FALL_OVER_1,
  MIP_SOUND_MIP_FALL_OVER_2,
  MIP_SOUND_MIP_FIGHT,
  MIP_SOUND_MIP_GAME,
  MIP_SOUND_MIP_GLOAT,
  MIP_SOUND_MIP_GO,
  MIP_SOUND_MIP_GOGOGO,
  MIP_SOUND_MIP_GRUNT_1,
  MIP_SOUND_MIP_GRUNT_2,
  MIP_SOUND_MIP_GRUNT_3,
  MIP_SOUND_MIP_HAHA_GOT_IT,
  MIP_SOUND_MIP_HI_CONFIDENT,
  MIP_SOUND_MIP_HI_NOT_SURE,
  MIP_SOUND_MIP_HI_SCARED,
  MIP_SOUND_MIP_HUH,
  MIP_SOUND_MIP_HUMMING_1,
  MIP_SOUND_MIP_HUMMING_2,
  MIP_SOUND_MIP_HURT,
  MIP_SOUND_MIP_HUUURGH,
  MIP_SOUND_MIP_IN_LOVE,
  MIP_SOUND_MIP_IT,
  MIP_SOUND_MIP_JOKE,
  MIP_SOUND_MIP_K,
  MIP_SOUND_MIP_LOOP_1,
  MIP_SOUND_MIP_LOOP_2,
  MIP_SOUND_MIP_LOW_BATTERY,
  MIP_SOUND_MIP_MIPPEE,
  MIP_SOUND_MIP_MORE,
  MIP_SOUND_MIP_MUAH_HA,
  MIP_SOUND_MIP_MUSIC,
  MIP_SOUND_MIP_OBSTACLE,
  MIP_SOUND_MIP_OHOH,
  MIP_SOUND_MIP_OH_YEAH,
  MIP_SOUND_MIP_OOPSIE,
  MIP_SOUND_MIP_OUCH_1,
  MIP_SOUND_MIP_OUCH_2,
  MIP_SOUND_MIP_PLAY,
  MIP_SOUND_MIP_PUSH,
  MIP_SOUND_MIP_RUN,
  MIP_SOUND_MIP_SHAKE,
  MIP_SOUND_MIP_SIGH,
  MIP_SOUND_MIP_SINGING,
  MIP_SOUND_MIP_SNEEZE,
  MIP_SOUND_MIP_SNORE,
  MIP_SOUND_MIP_STACK,
  MIP_SOUND_MIP_SWIPE_1,
  MIP_SOUND_MIP_SWIPE_2,
  MIP_SOUND_MIP_TRICKS,
  MIP_SOUND_MIP_TRIIICK,
  MIP_SOUND_MIP_TRUMPET,
  MIP_SOUND_MIP_WAAAAA,
  MIP_SOUND_MIP_WAKEY,
  MIP_SOUND_MIP_WHEEE,
  MIP_SOUND_MIP_WHISTLING,
  MIP_SOUND_MIP_WHOAH,
  MIP_SOUND_MIP_WOO,
  MIP_SOUND_MIP_YEAH,
  MIP_SOUND_MIP_YEEESSS,
  MIP_SOUND_MIP_YO,
  MIP_SOUND_MIP_YUMMY,
  MIP_SOUND_MOOD_ACTIVATED,
  MIP_SOUND_MOOD_ANGRY,
  MIP_SOUND_MOOD_ANXIOUS,
  MIP_SOUND_MOOD_BORING,
  MIP_SOUND_MOOD_CRANKY,
  MIP_SOUND_MOOD_ENERGETIC,
  MIP_SOUND_MOOD_EXCITED,
  MIP_SOUND_MOOD_GIDDY,
  MIP_SOUND_MOOD_GRUMPY,
  MIP_SOUND_MOOD_HAPPY,
  MIP_SOUND_MOOD_IDEA,
  MIP_SOUND_MOOD_IMPATIENT,
  MIP_SOUND_MOOD_NICE,
  MIP_SOUND_MOOD_SAD,
  MIP_SOUND_MOOD_SHORT,
  MIP_SOUND_MOOD_SLEEPY,
  MIP_SOUND_MOOD_TIRED,
  MIP_SOUND_SOUND_BOOST,
  MIP_SOUND_SOUND_CAGE,
  MIP_SOUND_SOUND_GUNS,
  MIP_SOUND_SOUND_ZINGS,
  MIP_SOUND_SHORT_MUTE_FOR_STOP,
  MIP_SOUND_FREESTYLE_TRACKING_2,
  MIP_SOUND_VOLUME_OFF = 0xF7,
  MIP_SOUND_VOLUME_1 = 0xF8,
  MIP_SOUND_VOLUME_2 = 0xF9,
  MIP_SOUND_VOLUME_3 = 0xFA,
  MIP_SOUND_VOLUME_4 = 0xFB,
  MIP_SOUND_VOLUME_5 = 0xFC,
  MIP_SOUND_VOLUME_6 = 0xFD,
  MIP_SOUND_VOLUME_7 = 0xFE
};

/**
 * @brief Speaker volume levels.
 */
enum MiPVolume : uint8_t {
  MIP_VOLUME_OFF = 0,
  MIP_VOLUME_1 = 1,
  MIP_VOLUME_2 = 2,
  MIP_VOLUME_3 = 3,
  MIP_VOLUME_4 = 4,
  MIP_VOLUME_5 = 5,
  MIP_VOLUME_6 = 6,
  MIP_VOLUME_7 = 7,
  MIP_VOLUME_DEFAULT = 0xFF
};

/**
 * @brief Manages MiP's sounds.
 */
class MiP_Sound {
 public:
/**
   * @brief MiP protocol command bytes used by the sound subsystem.
   *
   * These values are placed in the first byte of requests sent to the MiP
   * (and appear in the corresponding responses).  See the official
   * [MiP BLE Protocol](https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md)
   * for the complete list.
   */
  static constexpr uint8_t MIP_CMD_PLAY_SOUND = 0x06;
  static constexpr uint8_t MIP_CMD_SET_VOLUME = 0x15;
  static constexpr uint8_t MIP_CMD_GET_VOLUME = 0x16;

  /**
   * @brief Constructs the sound manager.
   * @param mip A reference to the main MiP object to access core services.
   */
  MiP_Sound(MiP& mip);

  /**
   * @brief Starts a new sound list sequence.
   *
   * Must be called before adding entries with addEntryToSoundList().
   */
  void beginList();

  /**
   * @brief Adds a sound (with optional delay and volume change) to the current
   *        sound list.
   *
   * @param sound     Sound index.
   * @param delayTime Delay in milliseconds before next sound (0-7650 ms).
   * @param volume    Volume for this sound (or MIP_VOLUME_DEFAULT to keep
   *                  previous).
   */
  void addEntryToList(MiPSoundIndex sound,
                      uint16_t delay = 0,
                      MiPVolume volume = MIP_VOLUME_DEFAULT);

  /**
   * @brief Plays the current sound list.
   *
   * @param repeatCount Number of times to repeat the entire list (0 = once).
   */
  void playList(uint8_t repeatCount = 0);

  /**
   * @brief Plays a single sound at the specified volume.
   *
   * Convenience method that builds and plays a one-entry sound list.
   *
   * @param sound  Sound index to play.
   * @param volume Volume level (default = MIP_VOLUME_DEFAULT).
   */
  void play(MiPSoundIndex sound, MiPVolume volume = MIP_VOLUME_DEFAULT);

  /**
   * @brief Sets the MiP speaker volume and verifies the change.
   *
   * Retries automatically on failure.
   *
   * @param volume Volume level (0-7).
   */
  void writeVolume(uint8_t volume);

  /**
   * @brief Reads the current speaker volume.
   *
   * Performs a verified read with retries.
   *
   * @return Current volume (0-7), or 0 on error.
   */
  uint8_t readVolume();

  /**
   * @brief Cleans up the sound subsystem upon end.
   *
   * Restores MiP's default volume.
   */
  void end();

 private:
  int8_t rawGetVolume(uint8_t& volume);
  void rawSetVolume(uint8_t volume);

  MiP& m_mip;                     // Stores a reference to the main MiP class.
  uint8_t m_playCommand[1 + 18];  // Gemini Enterprise: Increased by 1 to
                                  // prevent out-of-bounds write!
  int8_t m_soundIndex;
  uint8_t m_playVolume;
};

#endif  // MPU_SOUND_H
