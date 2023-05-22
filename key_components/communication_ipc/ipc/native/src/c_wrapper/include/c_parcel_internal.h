/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef IPC_C_PARCEL_INTERANL_H
#define IPC_C_PARCEL_INTERANL_H

#include "c_parcel.h"
#include "c_ashmem_internal.h"

#include <refbase.h>
#include "message_parcel.h"

struct MessageParcelHolder : public virtual OHOS::RefBase {
    explicit MessageParcelHolder(OHOS::MessageParcel *parcel = nullptr);
    ~MessageParcelHolder();

    OHOS::MessageParcel *parcel_;

private:
    bool isExternal_;
};

#endif /* IPC_C_PARCEL_INTERANL_H */
