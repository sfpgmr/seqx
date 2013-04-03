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

// SDKのサンプルを改造した作ったもの。内容はほぼそのままだが、非同期読み込みに
// 対応させた。

#include "stdafx.h"
#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#include <stdio.h>
#include <assert.h>
#include <wtypes.h>
#include <mmreg.h>
#include <ks.h>
#include <ksmedia.h>
#include "stdafx.h"
#include "sequencer.h"
using namespace std;

namespace sf
{
  song_t::song_t()
    :
    time_base_(960),// タイムベース(四分音符の分解能)
    tempo_(120),// テンポ
    denominator_(4),// 分母
    numerator_(4),// 分子
    key_(0)// 調
  {

  }

  sequencer::sequencer() :
    current_tempo_(120),
    current_denominator_(4),
    current_numerator_(4),
    current_pos_(0),
    current_key_(0),
    current_meas_(0)
  {

  }
}
