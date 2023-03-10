/*
 * Copyright (C) 2007 The Android Open Source Project
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

package com.kevinchoteau.splode;

import android.content.Intent;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.app.Activity;

// Wrapper for native library

public class SOBLib {

     static {
		System.loadLibrary("fmod");
		System.loadLibrary("fmodstudio"); 
		System.loadLibrary("game");
     }

	 public static native void init(Activity activity);
	 public static native void OnActivityResult(Activity activity, int requestCode, int resultCode, Intent data);
	 public static native void OnActivityCreated(Activity activity, Bundle savedInstanceState);
	 public static native void OnActivityDestroyed(Activity activity);
	 public static native void OnActivityPaused(Activity activity);
	 public static native void OnActivityResumed(Activity activity);
	 public static native void OnActivitySaveInstanceState(Activity activity, Bundle outState);
	 public static native void OnActivityStarted(Activity activity);
	 public static native void OnActivityStopped(Activity activity);
     public static native void changeSurface(int width, int height);
     public static native void step();
	 public static native void testAsset(AssetManager assetManager, String filePath, String cachePath, String assetPath);
	 public static native void inputPos(int id, boolean down, int posX, int posY);
	 public static native boolean MustLaunchPurchase();
	 public static native void OnStartPurchase();
	 public static native void OnEndPurchase(boolean _bSuccess);
}
