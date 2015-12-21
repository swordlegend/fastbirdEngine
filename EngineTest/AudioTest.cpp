/*
 -----------------------------------------------------------------------------
 This source file is part of fastbird engine
 For the latest info, see http://www.jungwan.net/
 
 Copyright (c) 2013-2015 Jungwan Byun
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 -----------------------------------------------------------------------------
*/

#include "stdafx.h"
#include "AudioTest.h"
#include "FBEngineFacade/EngineFacade.h"
#include "FBAudioPlayer/AudioEx.h"
using namespace fb;
class AudioTest::Impl{
public:
	AudioId mAudio;
	Impl(){
		//mAudio = EngineFacade::GetInstance().PlayAudio("data/audio/big_laser_fire_loop.ogg", Vec3(0, 10, 0));

		EngineFacade::GetInstance().SetListenerPosition(Vec3(0, 0, 0));
		AudioProperty prop;
		auto audioex = AudioEx::Create();
		audioex->SetStartLoopEnd("", "data/audio/big_laser_fire_loop.wav", "");
		audioex->Play(20.f);
		
	}

	void Update(float dt){
		/*static float time = 0;
		time += dt;
		if (time > 0.5f && mAudio != INVALID_AUDIO_ID){
			EngineFacade::GetInstance().StopAudio(mAudio);
			mAudio = INVALID_AUDIO_ID;
		}*/
	}
};

FB_IMPLEMENT_STATIC_CREATE(AudioTest);
AudioTest::AudioTest()
	: mImpl(new Impl)
{

}

AudioTest::~AudioTest(){

}

void AudioTest::Update(float dt){
	mImpl->Update(dt);
}