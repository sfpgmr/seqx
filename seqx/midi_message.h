#pragma once
/** @file
*  @brief MIDI Messageのヘッダーファイル
*/
#include <stdint.h>
#include <vector>
#include "seq_message.h"
#include "midi_input.h"
#include "midi_output.h"

namespace sf {
    namespace midi_message 
    {

    enum struct ID 
    {
        note_on = 0x90,
        note_off = 0x80,
        poly_key_pressure = 0xa0,
        control_change = 0xb0,
        program_change = 0xc0,
        channel_pressure = 0xd0,
        pitch_bend = 0xe0,
        sys_ex = 0xf0,
        quota_frame = 0xf1,
        song_position_pointer = 0xf2,
        song_select = 0xf3,
        tune_request = 0xf6,
        end_of_sys_ex = 0xf7,
        timing_clock = 0xf8,
        midi_clock_start = 0xfa,
        midi_clock_continue = 0xfb,
        midi_clock_stop = 0xfc,
        active_sensing = 0xfe,
        system_reset = 0xff
      };

    /*
    0 00 32 20 bank_select（音色バンクの切替） 
    1 01 33 21 モジュレーション・デプス 
    2 02 34 22 breath_control（息を吹き込む強さによるコントロール） 
    3 03 35 23 （未定義） 
    4 04 36 24 foot_control（フットペダルによるコントロール） 
    5 05 37 25 portament_time 
    6 06 38 26 data_entry（ＲＰＮ／ＮＲＰＮで指定したパラメータのvalueを設定） 
    7 07 39 27 main_volume(チャンネルの音量を設定） 
    8 08 40 28 balance_control（上の音域と下の音域のバランス） 
    9 09 41 29 （未定義） 
    10 0A 42 2A panpot（定位－左右のバランス） 
    11 0B 43 2B expression（音量の抑揚をつける） 
    12 0C 44 2C （未定義） 
    13 0D 45 2D （未定義） 
    14 0E 46 2E （未定義） 
    15 0F 47 2F （未定義） 
    16 10 48 30 汎用操作子1 
    17 11 49 31 汎用操作子2 
    18 12 50 32 汎用操作子3 
    19 13 51 33 汎用操作子4 

    ７ビット連続可変
    No. Hex 機能 
    64 40 hold1（ダンパメダル） 
    65 41 portament 
    66 42 sostenuto（chode_hold） 
    67 43 soft_pedal 
    68 44 （未定義） 
    69 45 hold2（フリーズ） 
    70 46 memory_batch_select 
    71-79 47-4F （未定義） 
    80 50 generic_controller5 
    81 51 generic_controller6 
    82 52 generic_controller7 
    83 53 generic_controller8 
    84-90 54-5A （未定義） 
    91 5B generic_effect1（reverb） 
    92 5C generic_effect2（tremolo） 
    93 5D generic_effect3（chorus） 
    94 5E generic_effect4（celeste） 
    95 5F generic_effect5（フェイザ） 



    RPN-NRPN
    No. Hex 機能 
    96 60 data_increment 
    97 61 data_decrement 
    LSB MSB 
    No. Hex No. Hex 機能 
    98 62 99 63 NRPN 
    100 64 101 65 RPN 

    */
    enum struct control_id 
    {
      bank_select = 0x0,
      modulation_depth = 0x1,
      breath_control =0x2,
      foot_control = 0x4,
      portament_time =0x5,
      data_entry = 0x6,
      main_volume = 0x7,
      balance_control = 0x8,
      panpot = 0xa,
      expression = 0xb,
      hold = 0x40,
      portament = 0x41,
      chode_hold = 0x42,
      soft_pedal = 0x43,
      hold2 = 0x45,
      memory_batch_select = 0x46,
      reverb = 0x5b,
      tremolo = 0x5c,
      chorus = 0x5d,
      celeste = 0x5e,
      phaser = 0x5f,
      all_sound_off = 0x78,
      reset_all_controller = 0x79,
      local_control = 0x7a,
      all_note_off = 0x7b,
      omni_off =0x7c,
      omni_on = 0x7d,
      mono_mode_on = 0x7e,
      poly_mode_on = 0x7f
    };

      template <uint16_t Tag>
      struct tag 
      {
        static const uint16_t type = Tag;
      };

      /** note・オン */
      struct note_on
      {
          uint8_t note;
          uint8_t velocity;
          static const uint16_t ID = ID::note_on;
      };

      /** note・オフ */
      struct note_off
      {
          uint8_t note;
          uint8_t velocity;
          static const uint16_t ID = ID::note_off;
      };

      /** control_change */
      struct control_change
      {
          uint8_t number;
          int16_t value;
          static const uint16_t ID = ID::control_change;
      };

      /** bank_select(音色バンクの切り替え) */
      struct bank_select 
      {
          int16_t value;
          static const uint16_t ID = ID::control_change;
      };

      /** modulation_depth  */
      struct modulation_depth
      {
          int16_t value;
          static const uint16_t ID = ID::control_change;
      };

      /** breath_control  */
      struct breath_control
      {
          int16_t value;
          static const uint16_t ID = ID::control_change;
      };

      /** foot_control  */
      struct foot_control
      {
          int16_t value;
          static const uint16_t ID = ID::control_change;
      };

      /** portament_time */
      struct portament_time
      {
         int16_t value;
         static const uint16_t ID = ID::control_change;
       };

     /** data_entry */
      struct data_entry
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** main_volume */
      struct main_volume
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** balance_control */
      struct balance_control
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** panpot */
      struct panpot
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** expression */
      struct expression
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** generic_controller1 */
      struct generic_controller1
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** generic_controller2*/
      struct generic_controller2
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** generic_controller3 */
      struct generic_controller3
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** generic_controller4 */
      struct generic_controller4
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };


      /** hold1 */
      struct hold1
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** portament */
      struct portament
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** sostenuto（chode_hold） */
      struct sostenuto
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** soft_pedal */
      struct soft_pedal
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** hold2（フリーズ） */
      struct hold2
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** memory_batch_select */
      struct memory_batch_select
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** generic_controller5 */
      struct generic_controller5
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** generic_controller6 */
      struct generic_controller6
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** generic_controller7 */
      struct generic_controller7
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** generic_controller8 */
      struct generic_controller8
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** generic_effect1（reverb） */
      struct generic_effect1
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** generic_effect2（tremolo） */
      struct generic_effect2
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** 汎用エフェクト3      （chorus） */
      struct generic_effect3
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** generic_effect4（celeste） */
      struct generic_effect4
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** generic_effect5（フェーザ） */
      struct generic_effect5
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** data_increment */
      struct data_increment
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** data_decrement */
      struct data_decrement
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** data_decrement */
      struct NRPN
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** data_decrement */
      struct RPN
      {
         int16_t value;
          static const uint16_t ID = ID::control_change;
       };

      /** pitch_bend */
      struct pitch_bend
      {
          int16_t value;
          static const uint16_t ID = ID::pitch_bend;
      };

      /** poly_key_pressure */
      struct poly_key_pressure
      {
          uint8_t note;
          uint8_t value;
          static const uint16_t ID = ID::poly_key_pressure;
      };

      /** チャンネルプレッシャー */
      struct channel_pressure
      {
          uint8_t value;
          static const uint16_t ID = ID::channel_pressure;
      };

      /** program_change */
      struct program_change
      {
          uint8_t value;
          static const uint16_t ID = ID::program_change;

      };

      /** sys_ex */
      struct sys_ex
      {
          uint16_t manufacturers_id;
          std::vector<uint8_t> data;
          static const uint16_t ID = ID::sys_ex;
      };

      /** end_of_sys_ex */
      struct end_of_sys_ex
      {
          static const uint16_t ID = ID::end_of_sys_ex;
      };

      template <typename T_MIDIoutput, typename T_MIDIMessage>
      inline void send_midi_output(T_MIDIoutput& midi_output,T_MIDIMessage& midi_message)
      {
          DWORD output_message = midi_message.value << 8 + midi_message.ID;
          ::midiOutShortMsg(T_MIDIoutput.handle(),output_message);
      };

      /** ノートオンメッセージのoutput */
      inline void send_midi_output(midi_output& output,note_on& data)
      {
        ::midiOutShortMsg(output.handle(),(data.ID) | (data.note << 8) | (data.velocity << 16));
      };
      

    }
}