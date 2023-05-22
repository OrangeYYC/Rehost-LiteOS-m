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

#include <ability_info.h>
#include <ability_manager.h>
#include <element_name.h>
#include <module_info.h>
#include <want.h>
#include "launcher.h"
#include "ability_test.h"

void StartLauncherApp(void)
{
    const char LAUNCHER_BUNDLE_NAME[] = "com.huawei.launcher";
    
    InstallLauncher();
    Want want = {nullptr};
    ElementName element = {nullptr};
    SetElementBundleName(&element, LAUNCHER_BUNDLE_NAME);
    SetWantElement(&want, element);
    StartAbility(&want);
    ClearElement(&element);
    ClearWant(&want);
}

void StartJSApp(void)
{
    const char JS_BUNDLE_NAME[] = "com.app.example";
    const char JS_APP_PATH[] = "/data/panel";

    Want want = {nullptr};
    ElementName element = {nullptr};
    SetElementBundleName(&element, JS_BUNDLE_NAME);
    SetWantElement(&want, element);
    SetWantData(&want, JS_APP_PATH, strlen(JS_APP_PATH) + 1);
    StartAbility(&want);
    ClearElement(&element);
    ClearWant(&want);
}
