#include <UI/StdAfx.h>
#include <UI/ListBox.h>
#include <UI/Scroller.h>
#include <UI/ImageBox.h>
#include <UI/Button.h>
#include <UI/CheckBox.h>
#include <UI/ListItem.h>
#include <UI/ListBoxData.h>
#include <UI/TextField.h>
#include <UI/IUIManager.h>
#include <Engine/TextManipulator.h>
#include <CommonLib/StringUtils.h>

namespace fastbird
{


//-----------------------------------------------------------------------------
ListBox::ListBox()
	: mNumCols(0)
	, mRowHeight(24)
	, mRowGap(4)
	, mHighlightColor("0.1, 0.3, 0.3, 0.7")
	, mData(0)
	, mStartIndex(0)
	, mEndIndex(10)
	, mFocusedListItem(0)
{
	mUIObject->mOwnerUI = this;
	mUIObject->mTypeString = ComponentType::ConvertToString(GetType());
	mColSizes.push_back(0.97f);
	SetProperty(UIProperty::SCROLLERV, "true");
}

ListBox::~ListBox()
{
	Clear();
	FB_DELETE(mData);
}

void ListBox::GatherVisit(std::vector<IUIObject*>& v)
{
	__super::GatherVisit(v);
}

std::string ListBox::GetSelectedString()
{
	if (mFocusedListItem)
	{
		unsigned row = mFocusedListItem->GetRowIndex();
		unsigned col = mFocusedListItem->GetColIndex();
		return WideToAnsi(mData->GetData(row)[col].GetText());

	}
	return std::string();
}

ListItem* ListBox::CreateNewItem(int row, int col)
{
	int hgap = mRowHeight + mRowGap;
	int y = hgap * row + mRowGap;
	float x = 0;
	for (int c = 0; c < col; ++c) {
		x += mColSizes[c];
	}
	auto npos = Vec2(x, PixelToLocalNHeight(y));
	auto nsize = Vec2(mColSizes[col], PixelToLocalNHeight(mRowHeight));

	ListItem* item = (ListItem*)AddChild(npos.x, npos.y, nsize.x, nsize.y, ComponentType::ListItem);
	item->SetRuntimeChild(true);
	if (col < (int)mColAlignes.size())
		item->SetProperty(UIProperty::TEXT_ALIGN, mColAlignes[col].c_str());
	if (col < (int)mTextSizes.size())
	{
		item->SetProperty(UIProperty::TEXT_SIZE, mTextSizes[col].c_str());
	}
	item->SetProperty(UIProperty::BACK_COLOR, mHighlightColor.c_str());
	item->SetProperty(UIProperty::NO_BACKGROUND, "true");
	item->RegisterEventFunc(IEventHandler::EVENT_MOUSE_LEFT_CLICK,
		std::bind(&ListBox::OnItemClicked, this, std::placeholders::_1));
	item->RegisterEventFunc(IEventHandler::EVENT_MOUSE_LEFT_DOUBLE_CLICK,
		std::bind(&ListBox::OnItemDoubleClicked, this, std::placeholders::_1));
	item->SetVisible(mVisibility.IsVisible());
	item->SetRowIndex(row);
	item->SetColIndex(col);
	return item;
}

unsigned ListBox::InsertItem(unsigned uniqueKey){
	if (!mData)
		return -1;
	unsigned index = mData->InsertData(uniqueKey);
	while (mItems.size() <= index)
	{
		mItems.push_back(ROW());
		auto& row = mItems.back();
		for (unsigned c = 0; c < mNumCols; ++c){
			row.push_back(0);
		}
	}

	return index;
}

unsigned ListBox::InsertItem(const wchar_t* uniqueKey)
{	
	if (!mData)
		return -1;
	unsigned index = mData->InsertData(uniqueKey);
	while (mItems.size() <= index)
	{
		mItems.push_back(ROW());
		auto& row = mItems.back();
		for (unsigned c = 0; c < mNumCols; ++c){
			row.push_back(0);
		}
	}
	return index;
}

void ListBox::SetItem(const Vec2I& rowcol, const wchar_t* string, ListItemDataType::Enum type){
	mData->SetData(rowcol, string, type);
	VisualizeData(rowcol.x);
}

void ListBox::SetItem(const Vec2I& rowcol, bool checked){
	mData->SetData(rowcol, checked);
	VisualizeData(rowcol.x);
}

bool ListBox::GetCheckBox(const Vec2I& indexRowCol) const{
	if (!mData){
		Log(FB_DEFAULT_DEBUG_ARG, "No data");
		return false;
	}
	if ((unsigned)indexRowCol.x >= mData->Size()){
		Log(FB_DEFAULT_DEBUG_ARG, "Invalid row index");
		return false;
	}
	if ((unsigned)indexRowCol.y >= mNumCols){
		Log(FB_DEFAULT_DEBUG_ARG, "Invalid col index");
		return false;
	}

	auto data = mData->GetData(indexRowCol.x);	
	auto item = data[indexRowCol.y];
	return item.GetChecked();
}

void ListBox::RemoveRow(const wchar_t* uniqueKey)
{
	if (!mData)
		return;
	unsigned deletedIndex = mData->DelDataWithKey(uniqueKey);
	if (mFocusedListItem->GetRowIndex() == deletedIndex)
		ChangeFocusItem(0);

	if (deletedIndex != -1)
	{
		for (auto it = mSelectedIndices.begin(); it != mSelectedIndices.end(); ++it)
		{
			if (*it == deletedIndex)
				SetHighlightRow(deletedIndex, false);
		}
		VisualizeData(deletedIndex);
	}
}

void ListBox::RemoveRow(unsigned uniqueKey){
	if (!mData)
		return;
	unsigned deletedIndex = mData->DelDataWithKey(uniqueKey);
	if (mFocusedListItem->GetRowIndex() == deletedIndex)
		ChangeFocusItem(0);
	if (deletedIndex != -1)
	{
		for (auto it = mSelectedIndices.begin(); it != mSelectedIndices.end(); ++it)
		{
			if (*it == deletedIndex)
				SetHighlightRow(deletedIndex, false);
		}
		VisualizeData(deletedIndex);
	}
}

void ListBox::RemoveRowWithIndex(unsigned index){
	if (!mData)
		return;
	if (index >= mItems.size())
	{
		Log(FB_DEFAULT_DEBUG_ARG, "Out of index.");
		return;
	}
	unsigned deletedIndex = mData->DelDataWithIndex(index);
	if (mFocusedListItem->GetRowIndex() == deletedIndex)
		ChangeFocusItem(0);
	if (deletedIndex != -1)
	{
		for (auto it = mSelectedIndices.begin(); it != mSelectedIndices.end(); ++it)
		{
			if (*it == deletedIndex)
				SetHighlightRow(deletedIndex, false);
		}

		VisualizeData(deletedIndex);
	}
}

void ListBox::SetHighlightRow(size_t row, bool highlight)
{
	for (size_t i = 0; i < mNumCols; ++i)
	{
		if (mItems[row].size() <= i || !mItems[row][i])
			break;

		mHighlighted[Vec2I((int)row, (int)i)] = highlight;

		if (highlight)
		{
			mItems[row][i]->SetProperty(UIProperty::NO_BACKGROUND, "false");
			mItems[row][i]->SetProperty(UIProperty::BACK_COLOR, mHighlightColor.c_str());
		}
		else
		{
			mItems[row][i]->SetProperty(UIProperty::NO_BACKGROUND, StringConverter::toString(mItems[row][i]->GetNoBackground()).c_str());
			mItems[row][i]->SetProperty(UIProperty::BACK_COLOR, mItems[row][i]->GetBackColor());
		}
	}
}

void ListBox::SetHighlightRowCol(unsigned row, unsigned col, bool highlight)
{
	if (row >= mData->Size() || col >= mNumCols)
		return;
	
	mHighlighted[Vec2I((int)row, (int)col)] = highlight;

	if (!mItems[row][col])
	{
		return;
	}
	if (highlight)
	{
		mItems[row][col]->SetProperty(UIProperty::NO_BACKGROUND, "false");
		mItems[row][col]->SetProperty(UIProperty::BACK_COLOR, mHighlightColor.c_str());
	}
	else
	{
		mItems[row][col]->SetProperty(UIProperty::NO_BACKGROUND, StringConverter::toString(mItems[row][col]->GetNoBackground()).c_str());
		mItems[row][col]->SetProperty(UIProperty::BACK_COLOR, mItems[row][col]->GetBackColor());
	}
}

void ListBox::SetHighlightRowAndSelect(size_t row, bool highlight)
{
	if (highlight)
	{
		if (ValueNotExistInVector(mSelectedIndices, row))
		{
			mSelectedIndices.push_back(row);
			if (mFocusedListItem){
				ChangeFocusItem(mItems[row][mFocusedListItem->GetColIndex()]);
			}
			else{
				ChangeFocusItem(mItems[row][0]);
			}
		}
	}
	else
	{
		DeleteValuesInVector(mSelectedIndices, row);
	}
	SetHighlightRow(row, highlight);
}

void ListBox::Clear()
{
	ChangeFocusItem(0);
	for (auto items : mItems)
	{
		for (auto item : items)
		{
			RemoveChild(item);
		}
	}
	mItems.clear();
	for (auto& cols : mRecycleBin)
	{
		for (auto item : cols){
			gFBEnv->pUIManager->DeleteComponent(item);
		}
	}
	mRecycleBin.clear();
	if (mData)
		mData->Clear();
	mSelectedIndices.clear();
	mHighlighted.clear();
	TriggerRedraw();
	gFBUIManager->DirtyRenderList(GetHwndId());
}

size_t ListBox::GetNumItems() const{
	if (!mData)
		return 0;
	return mData->Size();
}

bool ListBox::SetProperty(UIProperty::Enum prop, const char* val)
{
	switch (prop)
	{
	case UIProperty::LISTBOX_HIGHLIGHT_COLOR:
	{
		mHighlightColor = val;
		return true;
	}
	case UIProperty::LISTBOX_COL:
	{
		mStrCols = val;
		mNumCols = StringConverter::parseUnsignedInt(val);
		float colsize = 1.0f / (float)mNumCols;
		mColSizes.clear();
		for (unsigned i = 0; i < mNumCols; ++i)
		{
			mColSizes.push_back(colsize);
		}
		if (mData){
			Clear();
			FB_DELETE(mData);
		}
		mData = FB_NEW( ListBoxDataSet(mNumCols) );
		return true;
	}
	case UIProperty::LISTBOX_ROW_HEIGHT:
		{
			mRowHeight = StringConverter::parseInt(val);
			return true;
		}
	case UIProperty::LISTBOX_ROW_GAP:
	{
			mRowGap = StringConverter::parseInt(val);
			return true;
	}

	case UIProperty::LISTBOX_COL_SIZES:
		{
			// set UIProperty::LISTBOX_COL first
			// don't need to set this property if the num of col is 1.
			mStrColSizes = val;
			assert(mNumCols != 1);
			mColSizes.clear();
			StringVector strs = Split(val);
			assert(!strs.empty());
			for (unsigned i = 0; i < strs.size(); ++i)
			{
				mColSizes.push_back(StringConverter::parseReal(strs[i]));
			}
			return true;
		}

	case UIProperty::LISTBOX_TEXT_SIZES:
	{
		mStrTextSizes = val;
										  // set UIProperty::LISTBOX_COL first
										  mTextSizes.clear();
										  StringVector strs = Split(val);
										  assert(!strs.empty());
										  for (unsigned i = 0; i < strs.size(); ++i)
										  {
											  mTextSizes.push_back(strs[i]);
										  }
										  return true;
	}
	case UIProperty::LISTBOX_COL_ALIGNH:
		{
			mStrColAlignH = val;
			assert(mNumCols != 1);
			mColAlignes.clear();
			mColAlignes.reserve(mNumCols);
			StringVector strs = Split(val);
			assert(!strs.empty());
			unsigned i = 0;
			for (; i < strs.size(); ++i)
			{
				mColAlignes.push_back(strs[i]);
			}
			const char* lastAlign = "center";
			if (!mColAlignes.empty())
				lastAlign = mColAlignes.back().c_str();
			while (mColAlignes.size() < mNumCols)
			{
				mColAlignes.push_back(lastAlign);
			}
			return true;
		}

	case UIProperty::LISTBOX_COL_HEADERS_TEXT_SIZE:
	{
		mStrHeaderTextSizes = val;
										   assert(mNumCols != 1);
										   mHeaderTextSize.clear();
										   StringVector strs = Split(val);
										   assert(!strs.empty());
										   for (unsigned i = 0; i < strs.size(); ++i)
										   {
											   mHeaderTextSize.push_back(strs[i]);
										   }
										   return true;
	}

	case UIProperty::LISTBOX_COL_HEADERS:
		{
			mStrHeaders = val;
			StringVector strs = Split(val, ",");
			assert(strs.size() == mNumCols);
			float nh = PixelToLocalNHeight(mRowHeight);
			if (mHeaders.empty())
			{
				for (unsigned i = 0; i < mNumCols; ++i)
				{
					float posx = 0.0f;
					if (i >= 1)
					{
						posx = mHeaders[i - 1]->GetNPos().x + mHeaders[i - 1]->GetNSize().x;
					}

					mHeaders.push_back(static_cast<ListItem*>(
						AddChild(posx, 0.0f, mColSizes[i], nh, ComponentType::ListItem)));
					ListItem* pAddedItem = mHeaders.back();
					pAddedItem->SetRuntimeChild(true);
					const RECT& rect = mUIObject->GetRegion();
					pAddedItem->SetProperty(UIProperty::NO_BACKGROUND, "false");
					pAddedItem->SetProperty(UIProperty::BACK_COLOR, "0.0, 0.0, 0.0, 0.5");
					if (!mHeaderTextSize.empty() && i < mHeaderTextSize.size())
					{
						pAddedItem->SetProperty(UIProperty::TEXT_SIZE, mHeaderTextSize[i].c_str());
					}
					
					pAddedItem->SetProperty(UIProperty::TEXT_ALIGN, "center");
					pAddedItem->SetRowIndex(-1);
					pAddedItem->SetColIndex(i);
					pAddedItem->SetText(AnsiToWide(strs[i].c_str()));
				}
				assert(!mWndContentUI);
				mWndContentUI = (Wnd*)AddChild(0.0f, 0.0f, 1.0f, 1.0f, ComponentType::Window);
				mWndContentUI->SetRuntimeChild(true);
				Vec2I sizeMod = { 0, -(mRowHeight + 4) };
				mWndContentUI->SetSizeModificator(sizeMod);
				mWndContentUI->SetUseAbsYSize(true);
				mWndContentUI->SetPos(Vec2I(0, (mRowHeight + 4)));
				mWndContentUI->SetProperty(UIProperty::NO_BACKGROUND, "true");
				if (mUseScrollerV)
				{
					mUseScrollerV = false;
					mWndContentUI->SetProperty(UIProperty::SCROLLERV, "true");
				}
			}
			else
			{
				assert(mHeaders.size() == strs.size());
				int i = 0;
				for (auto& str : strs)
				{
					mHeaders[i]->SetText(AnsiToWide(strs[i].c_str()));
					++i;
				}
			}
			if (mUseScrollerV)
			{
				RemoveChild(mScrollerV, true);
				mScrollerV = 0;
			}
			
			return true;
		}
	case UIProperty::TEXTUREATLAS:
		{
										 assert(val);
										 mTextureAtlas = val;
										 return true;
		}
	}

	

	return __super::SetProperty(prop, val);
}

bool ListBox::GetProperty(UIProperty::Enum prop, char val[], bool notDefaultOnly)
{
	switch (prop)
	{
	case UIProperty::LISTBOX_HIGHLIGHT_COLOR:
	{
		if (notDefaultOnly)
		{
			if (mHighlightColor == UIProperty::GetDefaultValueString(prop))
				return false;
		}
		strcpy(val, mHighlightColor.c_str());
		return true;
	}
	case UIProperty::LISTBOX_COL:
	{
		if (notDefaultOnly)
		{
			if (mStrCols.empty())
				return false;
		}
		strcpy(val, mStrCols.c_str());
		return true;
	}
	case UIProperty::LISTBOX_ROW_HEIGHT:
	{
		if (notDefaultOnly)
		{
			if (mRowHeight == UIProperty::GetDefaultValueInt(prop))
				return false;
		}
		auto data = StringConverter::toString(mRowHeight);
		strcpy(val, data.c_str());
		return true;

		
	}
	case UIProperty::LISTBOX_ROW_GAP:
	{
		if (notDefaultOnly)
		{
			if (mRowGap == UIProperty::GetDefaultValueInt(prop))
				return false;
		}
		auto data = StringConverter::toString(mRowGap);
		strcpy(val, data.c_str());
		return true;
	}

	case UIProperty::LISTBOX_COL_SIZES:
	{
		if (notDefaultOnly)
		{
			if (mStrColSizes.empty())
				return false;
		}
		strcpy(val, mStrColSizes.c_str());
		return true;
	}

	case UIProperty::LISTBOX_TEXT_SIZES:
	{
		if (notDefaultOnly)
		{
			if (mStrTextSizes.empty())
				return false;
		}
		strcpy(val, mStrTextSizes.c_str());
		return true;
	}
	case UIProperty::LISTBOX_COL_ALIGNH:
	{
		if (notDefaultOnly)
		{
			if (mStrColAlignH.empty())
				return false;
		}
		strcpy(val, mStrColAlignH.c_str());
		return true;
	}

	case UIProperty::LISTBOX_COL_HEADERS_TEXT_SIZE:
	{
		if (notDefaultOnly)
		{
			if (mStrHeaderTextSizes.empty())
				return false;
		}
		strcpy(val, mStrHeaderTextSizes.c_str());
		return true;
	}

	case UIProperty::LISTBOX_COL_HEADERS:
	{
		if (notDefaultOnly)
		{
			if (mStrHeaders.empty())
				return false;
		}
		strcpy(val, mStrHeaders.c_str());
		return true;
	}
	case UIProperty::TEXTUREATLAS:
	{
		if (notDefaultOnly)
		{
			if (mTextureAtlas.empty())
				return false;
		}
		strcpy(val, mTextureAtlas.c_str());
		return true;
	}
	}

	return __super::GetProperty(prop, val, notDefaultOnly);

}
ListItem* ListBox::GetItem(const Vec2I& indexRowCol) const
{
	unsigned row = indexRowCol.x;
	unsigned col = indexRowCol.y;
	assert(row < mItems.size());
	assert(col < mItems[row].size());
	return mItems[row][col];

}

void ListBox::SelectRow(unsigned index)
{
	if (index < mItems.size())
	{
		if (ValueNotExistInVector(mSelectedIndices, index))
		{
			SetHighlightRow(index, true);
			mSelectedIndices.push_back(index);
		}
	}
	else if (!mItems.empty())
	{
		unsigned idx = mItems.size() - 1;
		if (ValueNotExistInVector(mSelectedIndices, idx))
		{
			SetHighlightRow(idx, true);
			mSelectedIndices.push_back(idx);
		}
	}
	OnEvent(IEventHandler::EVENT_MOUSE_LEFT_CLICK);
}

void ListBox::DeselectRow(unsigned index){
	if (index >= mItems.size())
		return;

	DeleteValuesInVector(mSelectedIndices, index);
	if (mSelectedIndicesSizeBefore != mSelectedIndicesSizeAfter){
		SetHighlightRow(index, false);
	}	
}

void ListBox::DeselectAll(){
	for (auto index : mSelectedIndices){
		SetHighlightRow(index, false);
	}
	mSelectedIndices.clear();
}

void ListBox::ToggleSelection(unsigned index){
	if (index >= mItems.size())
		return;
	if (ValueNotExistInVector(mSelectedIndices, index))
	{
		SelectRow(index);
	}
	else
	{
		DeselectRow(index);
	}
}

void ListBox::ClearSelection()
{
	for (auto& row : mSelectedIndices)
	{
		SetHighlightRow(row, false);
	}
	mSelectedIndices.clear();
	OnEvent(IEventHandler::EVENT_LISTBOX_CLEARED);
}

bool ListBox::IsSelected(unsigned row)
{
	return !ValueNotExistInVector(mSelectedIndices, row);
}

bool ListBox::OnInputFromHandler(IMouse* mouse, IKeyboard* keyboard)
{
	if (!mVisibility.IsVisible())
		return false;

	if (mNoMouseEvent)
	{
		return false;
	}
	
	bool mouseIn = __super::OnInputFromHandler(mouse, keyboard);

	if (keyboard->IsValid() && GetFocus(true))
	{
		auto c = keyboard->GetChar();
		if (c)
		{
			if (c == VK_TAB)
			{
				keyboard->PopChar();
				bool next = keyboard->IsKeyDown(VK_SHIFT) ? false : true;
				bool apply = true;
				IterateItem(next, apply);
			}
			else
			{
				keyboard->PopChar();
				if (mFocusedListItem) {
					SearchStartingChacrcter(c, mFocusedListItem->GetRowIndex());
				}
				else{
					SearchStartingChacrcter(c, -1);
				}
			}
		}

		if (keyboard->IsKeyPressed(VK_UP))
		{
			if (keyboard->IsKeyDown(VK_SHIFT))
			{
				if (mSelectedIndices.empty())
				{
					if (!mItems.empty())
					{
						unsigned rowIndex = mData->Size()-1;
						if (mFocusedListItem)
						{
							rowIndex = mFocusedListItem->GetRowIndex() - 1;
							if (rowIndex ==-1){
								rowIndex = mData->Size() - 1;
							}
						}
						MakeSureRangeFor(rowIndex);
						SetHighlightRowAndSelect(rowIndex, true);						
						keyboard->Invalidate();
						OnEvent(IEventHandler::EVENT_MOUSE_LEFT_CLICK);
					}
				}
				else
				{
					unsigned lastRow = mSelectedIndices.back();
					if (lastRow != 0){
						unsigned dest = lastRow - 1;
						if (IsSelected(dest))
						{
							SetHighlightRow(lastRow, false);
							DeleteValuesInVector(mSelectedIndices, lastRow);
							keyboard->Invalidate();
							OnEvent(IEventHandler::EVENT_MOUSE_LEFT_CLICK);
						}
						else
						{
							MakeSureRangeFor(dest);
							SetHighlightRowAndSelect(dest, true);
							keyboard->Invalidate();
							OnEvent(IEventHandler::EVENT_MOUSE_LEFT_CLICK);
						}
					}
					else{
						MakeSureRangeFor(0);
					}
				}
			}
			else
			{
				if (mSelectedIndices.empty())
				{
					if (!mItems.empty())
					{
						unsigned rowIndex = mData->Size() - 1;
						if (mFocusedListItem)
						{
							rowIndex = mFocusedListItem->GetRowIndex() - 1;
							if (rowIndex == -1){
								rowIndex = mData->Size() - 1;
							}
						}
						MakeSureRangeFor(rowIndex);
						SetHighlightRowAndSelect(rowIndex, true);
						keyboard->Invalidate();
						OnEvent(IEventHandler::EVENT_MOUSE_LEFT_CLICK);
					}
				}
				else
				{
					unsigned last = mSelectedIndices.back();
					if (last != 0)
					{
						ClearSelection();
						MakeSureRangeFor(last - 1);
						SetHighlightRowAndSelect(last-1, true);
						keyboard->Invalidate();
						OnEvent(IEventHandler::EVENT_MOUSE_LEFT_CLICK);
					}
					else
					{
						MakeSureRangeFor(0);
					}
				}
			}
		}
		else if (keyboard->IsKeyPressed(VK_DOWN))
		{
			if (keyboard->IsKeyDown(VK_SHIFT))
			{
				if (mSelectedIndices.empty())
				{
					if (!mItems.empty())
					{
						unsigned rowIndex = 0;
						if (mFocusedListItem)
						{
							rowIndex = mFocusedListItem->GetRowIndex()+1;
							if (rowIndex >= mData->Size()){
								rowIndex = 0;
							}
						}
						MakeSureRangeFor(rowIndex);
						SetHighlightRowAndSelect(rowIndex, true);
						keyboard->Invalidate();
						OnEvent(IEventHandler::EVENT_MOUSE_LEFT_CLICK);
					}
				}
				else
				{
					unsigned lastRow = mSelectedIndices.back();
					if (lastRow+1 < mItems.size())
					{
						unsigned dest = lastRow + 1;
						if (IsSelected(dest))
						{
							SetHighlightRowAndSelect(lastRow, false);
							keyboard->Invalidate();
							OnEvent(IEventHandler::EVENT_MOUSE_LEFT_CLICK);
						}
						else
						{
							MakeSureRangeFor(dest);
							SetHighlightRowAndSelect(dest, true);
							keyboard->Invalidate();
							OnEvent(IEventHandler::EVENT_MOUSE_LEFT_CLICK);
						}
					}
				}
			} //if (keyboard->IsKeyDown(VK_SHIFT))
			else
			{
				if (mSelectedIndices.empty())
				{
					if (!mItems.empty())
					{
						unsigned rowIndex = 0;
						if (mFocusedListItem)
						{
							rowIndex = mFocusedListItem->GetRowIndex() + 1;
							if (rowIndex >= mData->Size()){
								rowIndex = 0;
							}
						}
						SetHighlightRowAndSelect(rowIndex, true);
						MakeSureRangeFor(rowIndex);
						keyboard->Invalidate();
						OnEvent(IEventHandler::EVENT_MOUSE_LEFT_CLICK);
					}
				}
				else
				{
					unsigned last = mSelectedIndices.back();
					if (last +1 < mItems.size())
					{
						ClearSelection();
						SetHighlightRowAndSelect(last + 1, true);
						MakeSureRangeFor(last + 1);
						keyboard->Invalidate();
						OnEvent(IEventHandler::EVENT_MOUSE_LEFT_CLICK);
					}
				}
			}
		}

	}

	return mouseIn;
}

void ListBox::ChangeFocusItem(ListItem* newItem){
	mFocusedListItem = newItem;
}

void ListBox::OnItemClicked(void* arg)
{		
	IWinBase* clickedWin = (IWinBase*)arg;
	while (clickedWin && clickedWin->GetType() != ComponentType::ListItem){
		clickedWin = clickedWin->GetParent();
	}
	if (!clickedWin)
		return;
	ListItem* listItem = (ListItem*)clickedWin;
	
	size_t rowIndex = listItem->GetRowIndex();
	if (rowIndex != ListItem::INVALID_INDEX)
	{
		auto keyboard = gFBEnv->pEngine->GetKeyboard();
		unsigned clickedIndex = rowIndex;
		unsigned lastIndex = -1;
		if (!mSelectedIndices.empty())
			lastIndex = mSelectedIndices.back();

		if (keyboard->IsKeyDown(VK_SHIFT)){
			if (clickedIndex == lastIndex){
				DeselectRow(clickedIndex);
			}
			else if (lastIndex!=-1){
				if (clickedIndex < lastIndex){
					std::swap(clickedIndex, lastIndex);
				}
				for (unsigned index = lastIndex; index <= clickedIndex; ++index){
					SetHighlightRowAndSelect(index, true);
				}
			}
			else if (lastIndex == -1){
				ToggleSelection(clickedIndex);
			}
		}
		else if (keyboard->IsKeyDown(VK_CONTROL)){
			ToggleSelection(clickedIndex);
		}		
		else
		{
			DeselectAll();
			SetHighlightRowAndSelect(rowIndex, true);
		}
	}

	ChangeFocusItem(listItem);

	OnEvent(IEventHandler::EVENT_MOUSE_LEFT_CLICK);
}

void ListBox::OnItemDoubleClicked(void* arg)
{
	IWinBase* clickedWin = (IWinBase*)arg;
	while (clickedWin && clickedWin->GetType() != ComponentType::ListItem){
		clickedWin = clickedWin->GetParent();
	}
	if (!clickedWin)
		return;
	ListItem* listItem = (ListItem*)clickedWin;
	size_t rowIndex = listItem->GetRowIndex();
	if (rowIndex != ListItem::INVALID_INDEX)
	{
		DeselectAll();
		SetHighlightRowAndSelect(rowIndex, true);
	}
	ChangeFocusItem(listItem);
	OnEvent(IEventHandler::EVENT_MOUSE_LEFT_DOUBLE_CLICK);
}

void ListBox::OnItemEnter(void* arg){
	if (mFocusedListItem->GetChild(0) == arg)
	{
		unsigned rowIndex = mFocusedListItem->GetRowIndex();
		unsigned colIndex = mFocusedListItem->GetColIndex();
		auto data = mData->GetData(rowIndex);
		if (data[colIndex].GetDataType() == ListItemDataType::TextField)
		{
			auto textfield = dynamic_cast<TextField*>(mFocusedListItem->GetChild(0));
			if (wcscmp(textfield->GetText(), data[colIndex].GetText()) != 0){
				data[colIndex].SetText(textfield->GetText());
				OnEvent(EVENT_ENTER);
			}
		}
	}
}
unsigned ListBox::GetNumRows()
{
	return mItems.size();
}

IWinBase* ListBox::MakeMergedRow(unsigned row)
{
	if (mItems.size() < row)
		return 0;

	if (mItems[row].empty())
		return 0;

	mItems[row][0]->SetNSizeX(1.0f);
	mItems[row][0]->SetSizeModificator(Vec2I(-4, 0));
	mItems[row][0]->SetProperty(UIProperty::NO_BACKGROUND, "false");
	mItems[row][0]->SetProperty(UIProperty::BACK_COLOR, "0, 0, 0, 0.3");
	mItems[row][0]->SetProperty(UIProperty::NO_MOUSE_EVENT, "true");
	mItems[row][0]->SetProperty(UIProperty::TEXT_COLOR, "0x88cceeff");
	mItems[row][0]->SetProperty(UIProperty::TEXT_ALIGN, "center");
	mItems[row][0]->SetNoBackground(false);
	mItems[row][0]->SetBackColor("0, 0, 0, 0.3");


	return mItems[row][0];
}

IWinBase* ListBox::MakeMergedRow(unsigned row, const char* backColor, const char* textColor, bool noMouseEvent)
{
	if (mItems.size() < row)
		return 0;

	if (mItems[row].empty())
		return 0;

	mItems[row][0]->SetNSizeX(1.0f);
	mItems[row][0]->SetSizeModificator(Vec2I(-4, 0));
	
	if (backColor && strlen(backColor) != 0)
	{
		mItems[row][0]->SetProperty(UIProperty::BACK_COLOR, backColor);
		mItems[row][0]->SetBackColor(backColor);
		mItems[row][0]->SetProperty(UIProperty::NO_BACKGROUND, "false");
		mItems[row][0]->SetNoBackground(false);
	}
		
	if (textColor && strlen(textColor) !=0)
		mItems[row][0]->SetProperty(UIProperty::TEXT_COLOR, textColor);

	if (noMouseEvent)
		mItems[row][0]->SetProperty(UIProperty::NO_MOUSE_EVENT, "true");	
	mItems[row][0]->SetProperty(UIProperty::TEXT_ALIGN, "center");

	return mItems[row][0];
}

void ListBox::SwapItems(unsigned index0, unsigned index1)
{
	if (!mData)
		return;
	mData->SwapData(index0, index1);
	VisualizeData(index0);
	VisualizeData(index1);

	auto cols = mItems[index0].size();
	if_assert_fail(cols == mItems[index1].size())
		return;

	bool index0selected = !ValueNotExistInVector(mSelectedIndices, index0);
	bool index1selected = !ValueNotExistInVector(mSelectedIndices, index1);
	if (index0selected && !index1selected)
	{
		DeleteValuesInVector(mSelectedIndices, index0);
		mSelectedIndices.push_back(index1);
	}
	else if (!index0selected && index1selected)
	{
		DeleteValuesInVector(mSelectedIndices, index1);
		mSelectedIndices.push_back(index0);
	}
}

void ListBox::SwapItems(const wchar_t* uniqueKey0, const wchar_t* uniqueKey1){
	unsigned index0 = mData->FindRowIndexWithKey(uniqueKey0);
	unsigned index1 = mData->FindRowIndexWithKey(uniqueKey1);
	SwapItems(index0, index1);
}

unsigned ListBox::FindRowWithKey(unsigned key)
{
	if (!mData)
		return -1;
	return mData->FindRowIndexWithKey(key);
}

void ListBox::GetSelectedUniqueIdsString(std::vector<std::string>& ids) const{
	if (!mData)
		return;

	for (unsigned index : mSelectedIndices){
		ids.push_back(mData->GetStringKey(index));
	}
}
void ListBox::GetSelectedUniqueIdsUnsigned(std::vector<unsigned>& ids) const{
	if (!mData)
		return;

	for (unsigned index : mSelectedIndices){
		ids.push_back(mData->GetUnsignedKey(index));
	}
}


void ListBox::VisualizeData(unsigned index){
	if (!mData)
		return;

	if (index < mStartIndex || index > mEndIndex)
	{
		if (mItems[index][0])
		{
			MoveToRecycle(index);
		}
		return;
	}

	int hgap = mRowHeight + mRowGap;
	Vec2 offset(0, 0);
	if (mScrollerV)
	{
		offset = mScrollerV->GetOffset();
	}

	ListItem *keyItem = 0, *valueItem = 0;
	const auto data = mData->GetData(index);
	if (!data)
	{
		MoveToRecycle(index);
		return;
	}
	
	if (mItems[index][0])
	{
		FillItem(index);		
	}
	else
	{
		if (!mRecycleBin.empty())
		{
			auto items = mRecycleBin.back();
			int y = hgap * index + mRowGap;
			for (unsigned c = 0; c < mNumCols; ++c){
				items[c]->SetPosY(y);
				items[c]->SetWNPosOffset(offset);
				items[c]->SetRowIndex(index);
				mChildren.push_back(items[c]);
				mItems[index][c] = items[c];
			}
			mRecycleBin.pop_back();
			FillItem(index);
		}
		else
		{
			int y = hgap * index + mRowGap;
			float ny = PixelToLocalNHeight(y);
			for (unsigned c = 0; c < mNumCols; ++c){
				auto item = CreateNewItem(index, c);
				if (c == 0)
				{
					item->SetProperty(UIProperty::TEXT_LEFT_GAP, "5");
				}
				item->SetWNPosOffset(offset);
				mItems[index][c] = item;				
			}
			FillItem(index);
		}
	}	
}

void ListBox::FillItem(unsigned index){
	if (!mData)
		return;

	const auto data = mData->GetData(index);
	if_assert_fail (data)
		return;

	for (unsigned i = 0; i < mNumCols; ++i){
		auto item = mItems[index][i];
		assert(item);
		item->SetRowIndex(index);
		switch (data[i].GetDataType())
		{
		case ListItemDataType::CheckBox:
		{
			auto checkbox = dynamic_cast<CheckBox*>(item->GetChild(0));
			if (!checkbox)
			{
				item->RemoveAllChild();
				checkbox = (CheckBox*)item->AddChild(0.f, 0.f, 1.f, 1.f, ComponentType::CheckBox);
				checkbox->RegisterEventFunc(IEventHandler::EVENT_MOUSE_LEFT_CLICK,
					std::bind(&ListBox::OnItemClicked, this, std::placeholders::_1));
				checkbox->RegisterEventFunc(IEventHandler::EVENT_MOUSE_LEFT_DOUBLE_CLICK,
					std::bind(&ListBox::OnItemDoubleClicked, this, std::placeholders::_1));
				checkbox->SetRuntimeChild(true);
				checkbox->SetVisible(mVisibility.IsVisible());
			}
			if_assert_pass(checkbox){
				checkbox->SetCheck(data[i].GetChecked());
			}
			break;
		}
		case ListItemDataType::String:
		{
			item->SetText(data[i].GetText());
			break;
		}
		case ListItemDataType::TextField:
		{
			auto textField = dynamic_cast<TextField*>(item->GetChild(0));
			if (!textField)
			{
				item->RemoveAllChild();
				textField = (TextField*)item->AddChild(0.f, 0.f, 1.f, 1.f, ComponentType::TextField);
				textField->SetProperty(UIProperty::USE_BORDER, "true");
				textField->SetProperty(UIProperty::TEXT_LEFT_GAP, "4");
				textField->SetRuntimeChild(true);
				textField->SetVisible(mVisibility.IsVisible());
				textField->RegisterEventFunc(IEventHandler::EVENT_MOUSE_LEFT_CLICK,
					std::bind(&ListBox::OnItemClicked, this, std::placeholders::_1));
				textField->RegisterEventFunc(IEventHandler::EVENT_MOUSE_LEFT_DOUBLE_CLICK,
					std::bind(&ListBox::OnItemDoubleClicked, this, std::placeholders::_1));
				textField->RegisterEventFunc(IEventHandler::EVENT_ENTER,
					std::bind(&ListBox::OnItemEnter, this, std::placeholders::_1));
			}
			if_assert_pass(textField){
				textField->SetText(data[i].GetText());
			}
			break;
		}
		case ListItemDataType::TexturePath:
		{
			auto imageBox = dynamic_cast<ImageBox*>(item->GetChild(0));
			if (!imageBox)
			{
				item->RemoveAllChild();
				imageBox = (ImageBox*)item->AddChild(0.f, 0.f, 1.f, 1.f, ComponentType::ImageBox);
				imageBox->DrawAsFixedSizeAtCenter();
				imageBox->RegisterEventFunc(IEventHandler::EVENT_MOUSE_LEFT_CLICK,
					std::bind(&ListBox::OnItemClicked, this, std::placeholders::_1));
				imageBox->RegisterEventFunc(IEventHandler::EVENT_MOUSE_LEFT_DOUBLE_CLICK,
					std::bind(&ListBox::OnItemDoubleClicked, this, std::placeholders::_1));
				imageBox->SetRuntimeChild(true);
				imageBox->SetVisible(mVisibility.IsVisible());
			}
			if_assert_pass(imageBox){
				imageBox->SetTexture(WideToAnsi(data[i].GetText()));
			}
			break;
		}
		case ListItemDataType::TextureRegion:
		{
			auto imageBox = dynamic_cast<ImageBox*>(item->GetChild(0));
			if (!imageBox)
			{
				item->RemoveAllChild();
				imageBox = (ImageBox*)item->AddChild(0.f, 0.f, 1.f, 1.f, ComponentType::ImageBox);
				imageBox->DrawAsFixedSizeAtCenter();
				imageBox->RegisterEventFunc(IEventHandler::EVENT_MOUSE_LEFT_CLICK,
					std::bind(&ListBox::OnItemClicked, this, std::placeholders::_1));
				imageBox->RegisterEventFunc(IEventHandler::EVENT_MOUSE_LEFT_DOUBLE_CLICK,
					std::bind(&ListBox::OnItemDoubleClicked, this, std::placeholders::_1));
				imageBox->SetRuntimeChild(true);
				imageBox->SetVisible(mVisibility.IsVisible());
			}
			if_assert_pass(imageBox){
				imageBox->SetTextureAtlasRegion(mTextureAtlas.c_str(), WideToAnsi(data[i].GetText()));
			}
			break;
		}
		default:
			assert(0);
		}

		auto it = mHighlighted.Find(Vec2I((int)index, (int)i));
		if (it != mHighlighted.end() && it->second){
			SetHighlightRowCol(index, i, true);
		}
		else{
			SetHighlightRowCol(index, i, false);
		}
	}
}

void ListBox::MoveToRecycle(unsigned row){
	if (row < mItems.size())
	{
		if (mItems[row][0])
		{
			for (unsigned c = 0; c < mNumCols; ++c){
				if (mItems[row][c]->IsKeyboardFocused())
					return;
			}			
			
			mRecycleBin.push_back(ROW());
			auto& cols = mRecycleBin.back();			
			cols.reserve(mNumCols);
			for (unsigned c = 0; c < mNumCols; ++c){
				cols.push_back(mItems[row][c]);
				RemoveChildNotDelete(mItems[row][c]);
				mItems[row][c] = 0;
			}

			gFBUIManager->DirtyRenderList(mHwndId);
		}
	}
}

ListBoxData* ListBox::GetData(unsigned rowIndex, unsigned colIndex) const{
	if (!mData)
		return 0;
	if (rowIndex >= mData->Size() || colIndex >= mNumCols)
		return 0;

	auto cols = mData->GetData(rowIndex);
	return &cols[colIndex];

}

void ListBox::OnSizeChanged()
{
	if (mNSize.x == NotDefined || mNSize.y == NotDefined)
		return;

	__super::OnSizeChanged();
	Scrolled();
}

void ListBox::Scrolled()
{
	unsigned prevStart = mStartIndex;
	unsigned prevEnd = mEndIndex;
	int hgap = mRowHeight + mRowGap;

	if (mScrollerV)
	{
		Vec2 offset = mScrollerV->GetOffset();
		int scrolledLen = -Round(offset.y * GetRenderTargetSize().y) - mRowGap;
		int topToBottom = mSize.y + scrolledLen - mRowGap;

		// decide visual index range		
		mStartIndex = scrolledLen / hgap;
		mEndIndex = topToBottom / hgap;
		int remain = topToBottom % hgap;
		if (remain > 0)
			mEndIndex += 1;
	}
	else
	{
		// decide visual index range
		int topToBottom = mSize.y;
		mStartIndex = 0;
		mEndIndex = topToBottom / hgap;
		int remain = topToBottom % hgap;
		if (remain > 0)
			mEndIndex += 1;
	}

	// to recycle
	while (prevStart < mStartIndex)
	{
		unsigned index = prevStart++;
		MoveToRecycle(index);
	}

	while (prevEnd > mEndIndex)
	{
		unsigned index = prevEnd--;
		MoveToRecycle(index);
	}

	//to visual
	while (prevStart > mStartIndex)
	{
		--prevStart;
		unsigned visualIndex = prevStart;
		VisualizeData(visualIndex);
	}

	while (prevEnd < mEndIndex)
	{
		++prevEnd;
		unsigned visualIndex = prevEnd;
		VisualizeData(visualIndex);
	}

	__super::Scrolled();
}

float ListBox::GetContentHeight() const
{
	if (!mData)
		return mRowHeight / (float)GetRenderTargetSize().y;
	unsigned length = mData->Size();
	unsigned hgap = mRowHeight + mRowGap;
	unsigned contentLength = hgap * length + mRowGap; // for upper gap
	return contentLength / (float)GetRenderTargetSize().y;
}

void ListBox::Sort()
{
	mData->Sort(0);

	if (mStartIndex != -1 && mEndIndex != -1) {
		for (unsigned i = mStartIndex; i <= mEndIndex; i++) {
			VisualizeData(i);
		}
	}
}

void ListBox::SearchStartingChacrcter(char c, unsigned curIndex)
{
	unsigned index = mData->FindNext(0, c, curIndex);
	if (index == -1)
		return;

	if (mScrollerV)
	{
		if (index < mStartIndex + 1 || index >(mEndIndex >= 3 ? mEndIndex - 3 : mEndIndex))
		{
			unsigned hgap = mRowHeight + mRowGap;
			unsigned destY = hgap * index + mRowGap;

			mScrollerV->SetOffset(Vec2(0.f, -(destY / (float)GetRenderTargetSize().y)));
		}
	}
	if (mItems[index][0])
	{
		DeselectAll();
		SetHighlightRowAndSelect(index, true);
		ChangeFocusItem(mItems[index][0]);
		gFBUIManager->SetFocusUI(mItems[index][0]);
	}
}


void ListBox::IterateItem(bool next, bool apply)
{
	DeselectAll();

	unsigned rowIndex = 0;
	unsigned colIndex = 0;
	if (mFocusedListItem){
		rowIndex = mFocusedListItem->GetRowIndex();
		colIndex = mFocusedListItem->GetColIndex();
		if (apply)
		{
			auto data = mData->GetData(rowIndex);
			if (data[colIndex].GetDataType() == ListItemDataType::TextField)
			{
				auto textfield = dynamic_cast<TextField*>(mFocusedListItem->GetChild(0));
				if (wcscmp(textfield->GetText(), data[colIndex].GetText()) != 0){
					data[colIndex].SetText(textfield->GetText());
					OnEvent(EVENT_ENTER);
				}								
			}
			
		}
		SetHighlightRow(rowIndex, false);
		
		if (next)
			mData->FindNextFocus(rowIndex, colIndex);
		else
			mData->FindPrevFocus(rowIndex, colIndex);
	}
		
	MakeSureRangeFor(rowIndex);
	auto newFocusItem = mItems[rowIndex][colIndex];
	if_assert_pass(newFocusItem)
	{
		ChangeFocusItem(newFocusItem);
		SetHighlightRowAndSelect(rowIndex, true);
		auto child = newFocusItem->GetChild(0);
		if (child)
		{			
			gFBUIManager->SetFocusUI(child);
			if (child->GetType() == ComponentType::TextField)
			{
				gFBUIManager->GetTextManipulator()->SelectAll();
			}
		}
		else
			gFBUIManager->SetFocusUI(newFocusItem);
	}
		
}

void ListBox::MakeSureRangeFor(unsigned rowIndex){
	if (mScrollerV)
	{
		if (rowIndex < mStartIndex + 1 ||
			(rowIndex >(mEndIndex >= 3 ? mEndIndex - 3 : mEndIndex))
			)
		{
			unsigned hgap = mRowHeight + mRowGap;
			unsigned destY = hgap * rowIndex + mRowGap;

			mScrollerV->SetOffset(Vec2(0.f, -(destY / (float)GetRenderTargetSize().y)));
		}
	}
}

}