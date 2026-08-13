/**
 * @file MPU_Sound.h
 * @brief Defines the public interface for sound playback in the MiP library.
 *
 * @details This header declares the sound API used to play audio clips, build
 * multi-sound sequences, and manage speaker volume levels.
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
 * @brief Built-in sound effects, mood sound clips, and inline audio sequence
 * volume commands.
 */
enum MiPSoundIndex : uint8_t {
  MIP_SOUND_ONEKHZ_500MS_8K16BIT = 1,  ///< 1 kHz test tone (500 ms).
  MIP_SOUND_ACTION_BURPING,            ///< Action sound: Burping.
  MIP_SOUND_ACTION_DRINKING,           ///< Action sound: Drinking.
  MIP_SOUND_ACTION_EATING,             ///< Action sound: Eating.
  MIP_SOUND_ACTION_FARTING_SHORT,      ///< Action sound: Short fart.
  MIP_SOUND_ACTION_OUT_OF_BREATH,      ///< Action sound: Out of breath.
  MIP_SOUND_BOXING_PUNCHCONNECT_1,     ///< Boxing sound: Punch impact 1.
  MIP_SOUND_BOXING_PUNCHCONNECT_2,     ///< Boxing sound: Punch impact 2.
  MIP_SOUND_BOXING_PUNCHCONNECT_3,     ///< Boxing sound: Punch impact 3.
  MIP_SOUND_FREESTYLE_TRACKING_1,      ///< Sound effect: Tracking loop 1.
  MIP_SOUND_MIP_1,                     ///< Voice clip: MiP 1.
  MIP_SOUND_MIP_2,                     ///< Voice clip: MiP 2.
  MIP_SOUND_MIP_3,                     ///< Voice clip: MiP 3.
  MIP_SOUND_MIP_APP,                   ///< Voice clip: MiP App.
  MIP_SOUND_MIP_AWWW,                  ///< Voice clip: Awww.
  MIP_SOUND_MIP_BIG_SHOT,              ///< Voice clip: Big shot.
  MIP_SOUND_MIP_BLEH,                  ///< Voice clip: Bleh.
  MIP_SOUND_MIP_BOOM,                  ///< Voice clip: Boom.
  MIP_SOUND_MIP_BYE,                   ///< Voice clip: Bye bye.
  MIP_SOUND_MIP_CONVERSE_1,        ///< Voice clip: Conversational gibberish 1.
  MIP_SOUND_MIP_CONVERSE_2,        ///< Voice clip: Conversational gibberish 2.
  MIP_SOUND_MIP_DROP,              ///< Voice clip: Drop.
  MIP_SOUND_MIP_DUNNO,             ///< Voice clip: I don't know.
  MIP_SOUND_MIP_FALL_OVER_1,       ///< Voice clip: Falling over 1.
  MIP_SOUND_MIP_FALL_OVER_2,       ///< Voice clip: Falling over 2.
  MIP_SOUND_MIP_FIGHT,             ///< Voice clip: Fight.
  MIP_SOUND_MIP_GAME,              ///< Voice clip: Game.
  MIP_SOUND_MIP_GLOAT,             ///< Voice clip: Gloating.
  MIP_SOUND_MIP_GO,                ///< Voice clip: Go.
  MIP_SOUND_MIP_GOGOGO,            ///< Voice clip: Go go go!
  MIP_SOUND_MIP_GRUNT_1,           ///< Voice clip: Grunt 1.
  MIP_SOUND_MIP_GRUNT_2,           ///< Voice clip: Grunt 2.
  MIP_SOUND_MIP_GRUNT_3,           ///< Voice clip: Grunt 3.
  MIP_SOUND_MIP_HAHA_GOT_IT,       ///< Voice clip: Ha ha got it.
  MIP_SOUND_MIP_HI_CONFIDENT,      ///< Voice clip: Confident hello.
  MIP_SOUND_MIP_HI_NOT_SURE,       ///< Voice clip: Hesitant hello.
  MIP_SOUND_MIP_HI_SCARED,         ///< Voice clip: Scared hello.
  MIP_SOUND_MIP_HUH,               ///< Voice clip: Huh?
  MIP_SOUND_MIP_HUMMING_1,         ///< Voice clip: Humming 1.
  MIP_SOUND_MIP_HUMMING_2,         ///< Voice clip: Humming 2.
  MIP_SOUND_MIP_HURT,              ///< Voice clip: Hurt.
  MIP_SOUND_MIP_HUUURGH,           ///< Voice clip: Huuurgh.
  MIP_SOUND_MIP_IN_LOVE,           ///< Voice clip: In love.
  MIP_SOUND_MIP_IT,                ///< Voice clip: It.
  MIP_SOUND_MIP_JOKE,              ///< Voice clip: Joke.
  MIP_SOUND_MIP_K,                 ///< Voice clip: OK.
  MIP_SOUND_MIP_LOOP_1,            ///< Sound effect: Loop 1.
  MIP_SOUND_MIP_LOOP_2,            ///< Sound effect: Loop 2.
  MIP_SOUND_MIP_LOW_BATTERY,       ///< Voice clip: Low battery warning.
  MIP_SOUND_MIP_MIPPEE,            ///< Voice clip: MiP-pee!
  MIP_SOUND_MIP_MORE,              ///< Voice clip: More.
  MIP_SOUND_MIP_MUAH_HA,           ///< Voice clip: Muah ha ha!
  MIP_SOUND_MIP_MUSIC,             ///< Voice clip: Music.
  MIP_SOUND_MIP_OBSTACLE,          ///< Voice clip: Obstacle alert.
  MIP_SOUND_MIP_OHOH,              ///< Voice clip: Oh oh!
  MIP_SOUND_MIP_OH_YEAH,           ///< Voice clip: Oh yeah!
  MIP_SOUND_MIP_OOPSIE,            ///< Voice clip: Oopsie.
  MIP_SOUND_MIP_OUCH_1,            ///< Voice clip: Ouch 1.
  MIP_SOUND_MIP_OUCH_2,            ///< Voice clip: Ouch 2.
  MIP_SOUND_MIP_PLAY,              ///< Voice clip: Play.
  MIP_SOUND_MIP_PUSH,              ///< Voice clip: Push.
  MIP_SOUND_MIP_RUN,               ///< Voice clip: Run.
  MIP_SOUND_MIP_SHAKE,             ///< Voice clip: Shake.
  MIP_SOUND_MIP_SIGH,              ///< Voice clip: Sigh.
  MIP_SOUND_MIP_SINGING,           ///< Voice clip: Singing.
  MIP_SOUND_MIP_SNEEZE,            ///< Voice clip: Sneeze.
  MIP_SOUND_MIP_SNORE,             ///< Voice clip: Snore.
  MIP_SOUND_MIP_STACK,             ///< Voice clip: Stack.
  MIP_SOUND_MIP_SWIPE_1,           ///< Voice clip: Swipe 1.
  MIP_SOUND_MIP_SWIPE_2,           ///< Voice clip: Swipe 2.
  MIP_SOUND_MIP_TRICKS,            ///< Voice clip: Tricks.
  MIP_SOUND_MIP_TRIIICK,           ///< Voice clip: Triiick.
  MIP_SOUND_MIP_TRUMPET,           ///< Sound effect: Trumpet.
  MIP_SOUND_MIP_WAAAAA,            ///< Voice clip: Waaaaa!
  MIP_SOUND_MIP_WAKEY,             ///< Voice clip: Wakey wakey.
  MIP_SOUND_MIP_WHEEE,             ///< Voice clip: Wheee!
  MIP_SOUND_MIP_WHISTLING,         ///< Voice clip: Whistling.
  MIP_SOUND_MIP_WHOAH,             ///< Voice clip: Whoah!
  MIP_SOUND_MIP_WOO,               ///< Voice clip: Woo!
  MIP_SOUND_MIP_YEAH,              ///< Voice clip: Yeah!
  MIP_SOUND_MIP_YEEESSS,           ///< Voice clip: Yeeesss!
  MIP_SOUND_MIP_YO,                ///< Voice clip: Yo.
  MIP_SOUND_MIP_YUMMY,             ///< Voice clip: Yummy.
  MIP_SOUND_MOOD_ACTIVATED,        ///< Mood sound: Activated.
  MIP_SOUND_MOOD_ANGRY,            ///< Mood sound: Angry.
  MIP_SOUND_MOOD_ANXIOUS,          ///< Mood sound: Anxious.
  MIP_SOUND_MOOD_BORING,           ///< Mood sound: Boring.
  MIP_SOUND_MOOD_CRANKY,           ///< Mood sound: Cranky.
  MIP_SOUND_MOOD_ENERGETIC,        ///< Mood sound: Energetic.
  MIP_SOUND_MOOD_EXCITED,          ///< Mood sound: Excited.
  MIP_SOUND_MOOD_GIDDY,            ///< Mood sound: Giddy.
  MIP_SOUND_MOOD_GRUMPY,           ///< Mood sound: Grumpy.
  MIP_SOUND_MOOD_HAPPY,            ///< Mood sound: Happy.
  MIP_SOUND_MOOD_IDEA,             ///< Mood sound: Idea.
  MIP_SOUND_MOOD_IMPATIENT,        ///< Mood sound: Impatient.
  MIP_SOUND_MOOD_NICE,             ///< Mood sound: Nice.
  MIP_SOUND_MOOD_SAD,              ///< Mood sound: Sad.
  MIP_SOUND_MOOD_SHORT,            ///< Mood sound: Short.
  MIP_SOUND_MOOD_SLEEPY,           ///< Mood sound: Sleepy.
  MIP_SOUND_MOOD_TIRED,            ///< Mood sound: Tired.
  MIP_SOUND_SOUND_BOOST,           ///< Sound effect: Boost.
  MIP_SOUND_SOUND_CAGE,            ///< Sound effect: Cage.
  MIP_SOUND_SOUND_GUNS,            ///< Sound effect: Guns firing.
  MIP_SOUND_SOUND_ZINGS,           ///< Sound effect: Zings.
  MIP_SOUND_SHORT_MUTE_FOR_STOP,   ///< Internal short mute padding sound.
  MIP_SOUND_FREESTYLE_TRACKING_2,  ///< Sound effect: Tracking loop 2.
  MIP_SOUND_VOLUME_OFF = 0xF7,     ///< Sound sequence command: Turn volume off.
  MIP_SOUND_VOLUME_1 = 0xF8,       ///< Sound sequence command: Set volume to 1.
  MIP_SOUND_VOLUME_2 = 0xF9,       ///< Sound sequence command: Set volume to 2.
  MIP_SOUND_VOLUME_3 = 0xFA,       ///< Sound sequence command: Set volume to 3.
  MIP_SOUND_VOLUME_4 = 0xFB,       ///< Sound sequence command: Set volume to 4.
  MIP_SOUND_VOLUME_5 = 0xFC,       ///< Sound sequence command: Set volume to 5.
  MIP_SOUND_VOLUME_6 = 0xFD,       ///< Sound sequence command: Set volume to 6.
  MIP_SOUND_VOLUME_7 = 0xFE        ///< Sound sequence command: Set volume to 7
                                   ///< (maximum).
};

/**
 * @brief Speaker volume levels (0 = mute to 7 = maximum volume).
 */
enum MiPVolume : uint8_t {
  MIP_VOLUME_OFF = 0,        ///< Speaker output disabled (Mute / Volume 0).
  MIP_VOLUME_1 = 1,          ///< Speaker volume level 1.
  MIP_VOLUME_2 = 2,          ///< Speaker volume level 2.
  MIP_VOLUME_3 = 3,          ///< Speaker volume level 3.
  MIP_VOLUME_4 = 4,          ///< Speaker volume level 4.
  MIP_VOLUME_5 = 5,          ///< Speaker volume level 5.
  MIP_VOLUME_6 = 6,          ///< Speaker volume level 6.
  MIP_VOLUME_7 = 7,          ///< Speaker volume level 7 (Maximum volume).
  MIP_VOLUME_DEFAULT = 0xFF  ///< Special flag to maintain current volume
                             ///< setting.
};

/**
 * @brief Manages MiP's sound playback, audio sequence building, and volume
 * levels.
 */
class MiP_Sound {
public:
  /**
   * @brief Starts a new sound list sequence.
   *
   * @details Clears internal sequence indices and prepares the command buffer.
   * Must be called prior to invoking addEntryToList().
   */
  void beginList();

  /**
   * @brief Adds a sound entry (with optional delay and inline volume
   * modification) to the sequence.
   *
   * @details Appends a sound clip to the sequence buffer. If a custom volume is
   * specified that differs from the sequence volume, an inline volume change
   * command is automatically injected. Up to 8 sound entries can be added per
   * sequence.
   *
   * @param sound     Sound index to add.
   * @param delayTime Delay in milliseconds before playing the next sound (0 to
   * 7650 ms, converted to 30ms ticks).
   * @param volume    Inline volume for this sound, or MIP_VOLUME_DEFAULT to
   * maintain active volume.
   */
  void addEntryToList(MiPSoundIndex sound, uint16_t delayTime = 0,
                      MiPVolume volume = MIP_VOLUME_DEFAULT);

  /**
   * @brief Plays the currently assembled sound list sequence.
   *
   * @details Pads any remaining sequence slots with mute sound codes and
   * transmits the 18-byte play sound command to MiP over UART.
   *
   * @param repeatCount Number of times to repeat the entire sequence (0 = play
   * once).
   */
  void playList(uint8_t repeatCount = 0);

  /**
   * @brief Plays a single sound effect at an optional volume level.
   *
   * @details Convenience helper method that internally calls beginList(),
   * addEntryToList(), and playList().
   *
   * @param sound  Sound index to play.
   * @param volume Volume level for playback (default = MIP_VOLUME_DEFAULT).
   */
  void play(MiPSoundIndex sound, MiPVolume volume = MIP_VOLUME_DEFAULT);

  /**
   * @brief Sets MiP's speaker volume level and verifies the change.
   *
   * @details Sends the set volume command and reads back the active volume
   * level to confirm success. Automatically retries on communication failure or
   * mismatch.
   *
   * @param volume Speaker volume level (0 = mute to 7 = maximum volume).
   */
  void writeVolume(uint8_t volume);

  /**
   * @brief Reads the current speaker volume level from MiP.
   *
   * @details Performs a verified read query with automatic retries on
   * communication failure.
   *
   * @return uint8_t Current speaker volume level (0-7), or 0 on error.
   */
  uint8_t readVolume();

  /**
   * @brief Cleans up the sound subsystem when ending communication.
   *
   * @details Restores MiP's default speaker volume (level 7).
   */
  void end();

protected:
  /**
   * @brief MiP protocol command byte to play a sound effect or sound sequence.
   */
  static constexpr uint8_t MIP_CMD_PLAY_SOUND = 0x06;

  /**
   * @brief MiP protocol command byte to set the speaker volume level.
   */
  static constexpr uint8_t MIP_CMD_SET_VOLUME = 0x15;

  /**
   * @brief MiP protocol command byte to query the active speaker volume level.
   */
  static constexpr uint8_t MIP_CMD_GET_VOLUME = 0x16;

private:
  /**
   * @brief Private constructor; instantiated strictly by MiP orchestrator.
   *
   * @param mip A reference to the main MiP object to access core communication
   * services.
   */
  explicit MiP_Sound(MiP& mip);

  int8_t rawGetVolume(uint8_t& volume);
  void rawSetVolume(uint8_t volume);

  MiP& m_mip;                     // Stores a reference to the main MiP class.
  uint8_t m_playCommand[1 + 18];  // Buffer storing play command and 8 sound
                                  // entries
  int8_t m_soundIndex;
  uint8_t m_playVolume;

  /**
   * @brief Allows MiP to call private constructor.
   */
  friend class MiP;
};

#endif  // MPU_SOUND_H
