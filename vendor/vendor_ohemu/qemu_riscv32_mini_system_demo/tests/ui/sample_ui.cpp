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

#include "sample_ui.h"

#include "animator/animator.h"
#include "common/graphic_startup.h"
#include "common/task_manager.h"
#include "components/root_view.h"
#include "components/ui_image_view.h"
#include "components/ui_canvas.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "gfx_utils/heap_base.h"
#include "gfx_utils/geometry2d.h"
#include "gfx_utils/graphic_math.h"
#include "gfx_utils/image_info.h"
#include "common/screen.h"
#include "hal_tick.h"
#include "hilog/log.h"

#include <stdio.h>

using namespace OHOS;

class UIViewScaleRotate : public AnimatorCallback
{
public:
    UIViewScaleRotate() : animator_(this, nullptr, 50, true) {}

    ~UIViewScaleRotate()
    {
        if (label_ != nullptr) {
            delete label_;
            label_ = nullptr;
        }
    }

    void SetUp()
    {
        rootView_ = RootView::GetInstance();
        rootView_->SetPosition(0, 0);
        rootView_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        rootView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Green().full);
        label_ = new UILabel();
        label_->SetText("animator");
        label_->SetPosition(100, 100, 100, 50);
        label_->SetStyle(STYLE_TEXT_COLOR, Color::Black().full);
        label_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
        rootView_->Add(label_);

        animator_.Start();
        lastRun_ = HALTick::GetInstance().GetTime();
    }

    void Callback(UIView *view) override
    {
        angleValue_++;
        if (scaleValue_.x_ < 0.5f) {
            scaleStep_ = 0.01f;
        } else if (scaleValue_.x_ > 1.5f) {
            scaleStep_ = -0.01f;
        }
        scaleValue_.x_ += scaleStep_;
        scaleValue_.y_ += scaleStep_;
        label_->Rotate(angleValue_, VIEW_CENTER);
        label_->Scale(scaleValue_, VIEW_CENTER);

        frame_cnt_++;
        if (HALTick::GetInstance().GetElapseTime(lastRun_) >= 1000) {
            lastRun_ = HALTick::GetInstance().GetTime();
            frame_cnt_ = 0;
        }
    }

private:
    RootView *rootView_ = nullptr;
    UILabel *label_ = nullptr;
    const Vector2<float> VIEW_CENTER = {100.0f, 100.0f};
    Animator animator_;
    int16_t angleValue_ = 0;
    Vector2<float> scaleValue_ = {1.0f, 1.0f};
    float scaleStep_ = 0.01f;
    uint32_t lastRun_ = 0;
    uint32_t frame_cnt_ = 0;
};

class UiDemo : public UIView::OnClickListener
{
public:
    static UiDemo *GetInstance();
    void Start();

private:
    UiDemo() { srand(HALTick::GetInstance().GetTime()); }
    ~UiDemo();

    int random(int min, int max)
    {
        return rand() % (max - min) + min;
    }

    bool OnClick(UIView &view, const ClickEvent &event) override
    {
        Point pos = event.GetCurrentPos();
        int16_t x = random(view.GetWidth(), Screen::GetInstance().GetWidth() - view.GetWidth());
        int16_t y = random(view.GetHeight(), Screen::GetInstance().GetHeight() - view.GetHeight());
        view.SetPosition(x, y);
        RootView::GetInstance()->Invalidate();
        printf("click at (%d,%d), move to (%d,%d)\n", pos.x, pos.y, x, y);
        return true;
    }

    RootView *rootView_ = nullptr;
    UILabelButton *btn_ = nullptr;
    UILabel *label_ = nullptr;
    UIViewScaleRotate *viewScaleRotate_ = nullptr;
};


UiDemo *UiDemo::GetInstance()
{
    static UiDemo instance;
    return &instance;
}

UiDemo::~UiDemo()
{
    if (btn_ != nullptr) {
        delete btn_;
        btn_ = nullptr;
    }
    if (label_ != nullptr) {
        delete label_;
        label_ = nullptr;
    }
    if (viewScaleRotate_ != nullptr) {
        delete viewScaleRotate_;
        viewScaleRotate_ = nullptr;
    }
}

void UiDemo::Start()
{
    if (rootView_ != nullptr) {
        return;
    }
    rootView_ = RootView::GetInstance();
    rootView_->SetPosition(0, 0);
    rootView_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    rootView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Green().full);
    if (viewScaleRotate_ == nullptr) {
        viewScaleRotate_ = new UIViewScaleRotate();
        viewScaleRotate_->SetUp();
    }

    btn_ = new UILabelButton();
    btn_->SetPosition(250, 200, 150, 64);
    btn_->SetStyle(STYLE_BORDER_COLOR, Color::Red().full);
    btn_->SetTextColor(Color::Red());
    btn_->SetText("Don't click me");
    rootView_->Add(btn_);
    btn_->SetOnClickListener(this);

    rootView_->Invalidate();
}

void UiDemoStart(void)
{
    UiDemo::GetInstance()->Start();
}