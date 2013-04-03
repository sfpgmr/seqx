#pragma once
/*
  ==============================================================================

   This file is part of the async
   Copyright 2005-11 by Satoshi Fujiwara.

   async can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   async is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with async; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
   Boston, MA 02111-1307 USA

  ==============================================================================
*/
#include <audioclient.h>
#include "exception.h"
#include "audio_source.h"
#include "sf_memory.h"
#include "audio_processor.h"

namespace sf {

//#pragma pack(push,8)

  struct sequencer;

  struct command_base 
  {
    //virtual void process(sequencer& s);
  };

  struct seq_event
  {

    uint64_t step() {return step_;}
    std::unique_ptr<command_base>& command() {return command_;}

    void step(uint64_t v) {step_ = v;}
  private:
    uint64_t step_;
    std::unique_ptr<command_base> command_;
  };

  struct track_base 
  {
    const std::wstring& name() {return name_;}
    const std::wstring& comment() {return comment_;}

    void name(const std::wstring& v) {name_ = v;}
    void comment(const std::wstring& v) {comment_ = v;}

  private:
    std::wstring name_;
    std::wstring comment_;
  };

  struct track : public track_base
  {
    uint64_t step() {return step_;}// ステップオフセット
    int32_t key() {return key_;}// キーオフセット
    uint32_t midi_channel() {return midi_channel_;}// MIDIチャンネル
    audio_processor* processor() {return processor_;}// オーディオエンジンへの参照
    std::vector<seq_event>& events() {return events_;}// イベント

    void step(uint64_t v) {step_ = v;}// ステップオフセット
    void key(int32_t v) {key_ = v;}// キーオフセット
    void midi_channel(uint32_t v) {midi_channel_ = v;}// MIDIチャンネル
    void processor(audio_processor* v) {processor_ = v;}// オーディオエンジンへの参照

  private:
    uint64_t step_;// ステップオフセット
    int32_t key_;// キーオフセット
    uint32_t midi_channel_;// MIDIチャンネル
    audio_processor* processor_;// オーディオエンジンへの参照
    std::vector<seq_event> events_;// イベント
  };

  struct pattern : public command_base
  {

    const std::wstring& name() {return name_;}
    const std::wstring& comment() {return comment_;}
    int32_t key() {return key_;}// キーオフセット
    uint32_t step() {return step_;}// ステップオフセット
    std::vector<track_base>& tracks() {return tracks_;}// 

    void name(const std::wstring v) {name_ = v;}
    void comment(const std::wstring v) {comment_ = v;}
    void key(int32_t v) {key_ = v;}// キーオフセット
    void step(uint32_t v) {step_ = v;}// ステップオフセット
  private:
    std::wstring name_;
    std::wstring comment_;
    int32_t key_;// キーオフセット
    uint32_t step_;// ステップオフセット
    std::vector<track_base> tracks_;// 
  };

  struct song_t 
  {
    song_t();
    
    std::wstring& name() {return name_;}
    std::wstring& comment() {return comment_;}
    uint32_t time_base(){return time_base_;}// タイムベース(四分音符の分解能)
    uint32_t tempo(){return tempo_;}// テンポ
    uint32_t denominator(){return denominator_;}// 分母
    uint32_t numerator(){return numerator_;}// 分子
    int32_t key(){return key_;}// 調
    std::vector<pattern>& patterns(){return patterns_;}// パターン配列

    void name(std::wstring& v) {name_ = v;}
    void comment(std::wstring& v) {comment_ = v;}
    void time_base(uint32_t v) {time_base_ = v;}// タイムベース(四分音符の分解能)
    void tempo(uint32_t v) {tempo_ = v;}// テンポ
    void denominator(uint32_t v) {denominator_ = v;}// 分母
    void numerator(uint32_t v) {numerator_ = v;}// 分子
    void key(int32_t v) {key_ = v;}// 調

  private:
    std::wstring name_;
    std::wstring comment_;
    uint32_t time_base_;// タイムベース(四分音符の分解能)
    uint32_t tempo_;// テンポ
    uint32_t denominator_;// 分母
    uint32_t numerator_;// 分子
    int32_t key_;// 調
    std::vector<pattern> patterns_;// パターン配列
  };

  struct note_command : public command_base
  {
    note_command(uint32_t n,uint32_t g,uint32_t v) 
      : note_(n),gate_time_(g),velocity_(v) {};

    uint32_t note() {return note_;};
    uint32_t gate_time() {return gate_time_;};
    uint32_t velocity() {return velocity_;};

    void note(uint32_t v) {note_ = v;}
    void gate_time(uint32_t v) {gate_time_ = v;}
    void velocity(uint32_t v) {velocity_ = v;}

  private:
    uint32_t note_;
    uint32_t gate_time_;
    uint32_t velocity_;
  };

//#pragma pack(pop)

  class sequencer : public audio_source
  {
  public:

    sequencer();
    ~sequencer(){};
    virtual bool seekable(){return true;};
    virtual bool stream_status(){return true;};
    virtual WAVEFORMATEXTENSIBLE &get_wave_format() {return WAVEFORMATEXTENSIBLE();};
    virtual bool more_data_available(){return false;};
    virtual void read_data(BYTE *buffer, uint64_t numbytes){};
    virtual void reset_data_position(){};
    virtual uint64_t total_data_bytes(){return 0;};
    virtual void seek(uint64_t pos){};
    virtual uint64_t data_bytes_remaining(){return 0;};
    virtual HANDLE raw_handle(){return 0;};
    virtual void wait(int timer = -1){};
    song_t& song() {return song_;};

  private:
    
    uint32_t current_tempo_;
    uint32_t current_denominator_;// 分母
    uint32_t current_numerator_;// 分子
    uint64_t current_pos_;// 再生ポジション
    uint32_t current_key_;// キーオフセット
    uint32_t current_meas_;
    song_t song_;
  };
}

 

