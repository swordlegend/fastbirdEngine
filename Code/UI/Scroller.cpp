#include <UI/StdAfx.h>
#include <UI/Scroller.h>
#include <UI/Container.h>

namespace fastbird
{

Scroller::Scroller()
	: mScrollAmount(0.001f)
	, mOffset(0, 0)
	, mMaxOffset(0, 0)
	, mOwner(0)
	, mDestOffset(0)
	, mCurOffset(0)
	, mScrollAcc(10)
	, mCurScrollSpeed(0)
	, mMaxScrollSpeed(30)
{
	mUIObject = IUIObject::CreateUIObject(false, GetRenderTargetSize());
	mUIObject->mOwnerUI = this;
	mUIObject->mTypeString = ComponentType::ConvertToString(GetType());
}

Scroller::~Scroller()
{

}

void Scroller::GatherVisit(std::vector<IUIObject*>& v)
{
	if (!mVisible)
		return;
	v.push_back(mUIObject);	
}

bool Scroller::OnInputFromHandler(IMouse* mouse, IKeyboard* keyboard)
{
	if (!mParent || !mVisible)
		return false;

	bool isIn = __super::OnInputFromHandler(mouse, keyboard);
	long wheel = mouse->GetWheel();
	if (wheel && mParent->IsIn(mouse))
	{		
		float prevDestOffset = mDestOffset;
		mDestOffset += wheel * mScrollAmount;
		mDestOffset = std::min(0.f, mDestOffset);
		mDestOffset = std::max(-mMaxOffset.y, mDestOffset);
		if (prevDestOffset != mDestOffset || mDestOffset != mCurOffset)
			mouse->ClearWheel();
	}
	return isIn;
}

void Scroller::OnStartUpdate(float elapsedTime)
{
	__super::OnStartUpdate(elapsedTime);
	if (mDestOffset == mCurOffset)
		return;

	float gap = mDestOffset - mCurOffset;
	mCurScrollSpeed += mScrollAcc*elapsedTime;
	mCurScrollSpeed = std::min(mCurScrollSpeed, mMaxScrollSpeed);

	if (gap < 0)
		gap -= 0.01f;
	else if (gap>0)
		gap += 0.01f;
	mCurOffset += mCurScrollSpeed * elapsedTime * gap;
	if (gap < 0 && mDestOffset >= mCurOffset-0.005f)
	{
		mCurOffset = mDestOffset;
		mCurScrollSpeed = 0.f;
	}
	else if (gap > 0 && mDestOffset <= mCurOffset+0.005f)
	{
		mCurOffset = mDestOffset;
		mCurScrollSpeed = 0.f;
	}
	mOffset.y = mCurOffset;
	mOwner->Scrolled();
}
}