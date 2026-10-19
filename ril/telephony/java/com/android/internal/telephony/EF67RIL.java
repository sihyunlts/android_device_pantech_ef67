/*
 * Copyright (C) 2014 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony;

import android.content.Context;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;

public class EF67RIL extends RIL {
    public EF67RIL(Context context, int networkMode, int cdmaSubscription,
            Integer instanceId) {
        super(context, networkMode, cdmaSubscription, instanceId);
        mQANElements = 5;
    }

    @Override
    public void setDataAllowed(boolean allowed, final Message result) {
        if (!allowed || result == null) {
            super.setDataAllowed(allowed, result);
            return;
        }

        // The stock RIL attaches data without supporting ALLOW_DATA.
        Handler responseHandler = new Handler(result.getTarget().getLooper()) {
            @Override
            public void handleMessage(Message message) {
                AsyncResult response = (AsyncResult) message.obj;
                Throwable error = response.exception;
                if (error instanceof CommandException &&
                        ((CommandException) error).getCommandError() ==
                                CommandException.Error.REQUEST_NOT_SUPPORTED) {
                    error = null;
                }
                AsyncResult.forMessage(result, response.result, error);
                result.sendToTarget();
            }
        };
        super.setDataAllowed(true, responseHandler.obtainMessage());
    }

}
