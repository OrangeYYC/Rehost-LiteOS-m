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

#ifndef PRODUCT_GRAPHIC_LITE_CONFIG_H
#define PRODUCT_GRAPHIC_LITE_CONFIG_H

#define ENABLE_JPEG_AND_PNG 1

#define ENABLE_GIF 1

#define FULLY_RENDER 1

/**
 * @brief Graphics rendering hardware acceleration, which is enabled by default on Huawei LiteOS.
 */
#define ENABLE_HARDWARE_ACCELERATION 0

#define ENABLE_VIBRATOR 0

/**
 * @brief Function for monitoring the image refresh frame rate, which is disabled by default.
 */
#define ENABLE_FPS_SUPPORT 1

/**
 * @brief Log function of a graphics subsystem, which is disabled by default
 */
#define ENABLE_GRAPHIC_LOG 1

/**
 * @brief Defines the file name of default vector font.
 */
#define DEFAULT_VECTOR_FONT_FILENAME "font.ttf"

#define HORIZONTAL_RESOLUTION 800
#define VERTICAL_RESOLUTION 480

/* Default file path for font */
#define VECTOR_FONT_DIR "/data/" // It must end with '/'.

#define IMG_CACHE_SIZE 8

#endif // PRODUCT_GRAPHIC_LITE_CONFIG_H
