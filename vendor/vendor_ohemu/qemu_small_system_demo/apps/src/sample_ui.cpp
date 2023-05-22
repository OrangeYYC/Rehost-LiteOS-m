/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "gfx_utils/graphic_types.h"
#include "font/ui_font_header.h"
#include "window/window.h"
#include "common/screen.h"
#include "engines/gfx/soft_engine.h"
#include "hal_tick.h"
#include "hilog/log.h"

#include <stdio.h>
#include <unistd.h>

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#undef LOG_TAG
#define LOG_TAG "UiDemo"


using namespace OHOS;
static uint32_t g_fontMemBaseAddr[OHOS::MIN_FONT_PSRAM_LENGTH / 4];

void GUIInit()
{
    OHOS::GraphicStartUp::Init();
    static OHOS::SoftEngine localGfxEngine;
    OHOS::SoftEngine::InitGfxEngine(&localGfxEngine);

    OHOS::GraphicStartUp::InitFontEngine(
            reinterpret_cast<uintptr_t>(g_fontMemBaseAddr),
            OHOS::MIN_FONT_PSRAM_LENGTH,
            VECTOR_FONT_DIR,
            DEFAULT_VECTOR_FONT_FILENAME);
}

void GUITaskHandler()
{
    OHOS::TaskManager::GetInstance()->TaskHandler();
}

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
        label_->SetText("旋转动画");
        label_->SetPosition(100, 100, 100, 50);
        label_->SetStyle(STYLE_TEXT_COLOR, Color::Black().full);
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
            HILOG_DEBUG(HILOG_MODULE_APP, "%u fps\n", frame_cnt_);
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
        HILOG_DEBUG(HILOG_MODULE_APP, "click at (%d,%d), move to (%d,%d)\n", pos.x, pos.y, x, y);
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
    HILOG_DEBUG(HILOG_MODULE_APP, "rootView %d-%d\n", rootView_->GetWidth(), rootView_->GetHeight());
    if (viewScaleRotate_ == nullptr) {
        viewScaleRotate_ = new UIViewScaleRotate();
        viewScaleRotate_->SetUp();
    }

    btn_ = new UILabelButton();
    btn_->SetPosition(150, 200, 150, 64);
    btn_->SetText("点不到我!");
    rootView_->Add(btn_);
    btn_->SetOnClickListener(this);

    rootView_->Invalidate();
}

void UiDemoStart(void)
{
    GUIInit();
    UiDemo::GetInstance()->Start();
    OHOS::WindowConfig config = {};
    config.rect.SetRect(0, 0, OHOS::Screen::GetInstance().GetWidth() - 1,
            OHOS::Screen::GetInstance().GetHeight() - 1);
    OHOS::Window* window = OHOS::Window::CreateWindow(config);
    if (window == nullptr) {
        GRAPHIC_LOGE("Create window false!");
        return;
    }
    window->BindRootView(OHOS::RootView::GetInstance());
    window->Show();

    while (1) {
        GUITaskHandler();
        usleep(16); /* 16 ms*/
    }
}

int main(int argc, char* argv[])
{
    UiDemoStart();
    return 0;
}

