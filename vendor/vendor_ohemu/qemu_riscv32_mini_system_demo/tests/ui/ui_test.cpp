/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ui_test.h"

#include "common/screen.h"
#include "components/ui_canvas.h"
#include "components/ui_label.h"
#include "components/ui_scroll_view.h"
#include "components/ui_view_group.h"
#include "components/root_view.h"
#include "hal_tick.h"

#include <stdio.h>

using namespace OHOS;
class UITest : public UIView::OnClickListener
{
public:
    static UITest *GetInstance();
    void SetUp();
    void Start();
    const UIView* GetView();

private:
    UITest() {}
    ~UITest() {}
    UIViewGroup* CreateGroup() const;
    UILabel* CreateLabel() const;
    UICanvas* CreateCanvas() const;
    UIScrollView* container_ = nullptr;
    RootView* rootView_ = nullptr;
};

UITest *UITest::GetInstance()
{
    static UITest instance;
    return &instance;
}

void UITest::SetUp()
{
    if (rootView_ == nullptr) {
        rootView_ = RootView::GetInstance();
        rootView_->SetPosition(0, 0);
        rootView_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    }

    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        container_->SetHorizontalScrollState(false);
    }
}

UIViewGroup* UITest::CreateGroup() const
{
    UIViewGroup* group = new UIViewGroup();
    group->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    group->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    group->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    return group;
}

UILabel* UITest::CreateLabel() const
{
    UILabel* label = new UILabel();
    label->SetPosition(250, 200, 150, 64);
    return label;
}

UICanvas* UITest::CreateCanvas() const
{
    UICanvas* canvas = new UICanvas();
    canvas->SetHeight(200);
    canvas->SetWidth(200);
    canvas->SetPosition(48, 48);
    canvas->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    return canvas;
}

void UITest::Start()
{
    SetUp();

    if (rootView_ == nullptr) {
        return;
    }

    if (container_ == nullptr) {
        return;
    }
    UIViewGroup* group = CreateGroup();
    group->SetViewId("GROUP_001");
    group->SetPosition(0, 0);

    UILabel* label = CreateLabel();
    group->Add(label);
    label->SetText("水平直线绘制 ");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    canvas->DrawLine({50, 50}, {150, 50}, paint); // 50 : start x 50 : start y 150 : end x 50 : end y
    group->Add(canvas);
    container_->Add(group);
    rootView_->Add(container_);
    rootView_->Invalidate();
}

void UiTestStart(void)
{
    UITest::GetInstance()->Start();
}