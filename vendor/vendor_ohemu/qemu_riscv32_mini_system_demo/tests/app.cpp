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

#if defined(UI_TEST) || defined(ABILITY_TEST)
#include "ohos_init.h"
#include "ui_adapter.h"
#if defined(UI_TEST)
#include "ui_test.h"
#include "sample_ui.h"
#elif defined(ABILITY_TEST)
#include "ability_test.h"
#endif

/* ui app entry */
void RunApp(void)
{
#ifdef UI_TEST
    AnimatorDemoStart();
#elif defined(ABILITY_TEST)
    StartJSApp();
#endif
}

void AppEntry(void)
{
    UiAdapterRun();
}

APP_FEATURE_INIT(AppEntry);
#endif