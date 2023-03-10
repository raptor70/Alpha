#include "game.h"

#include "file/base_file.h"
#include "input/input_manager.h"
#include "input/device_mouse.h"
#include "platform/platform_manager.h"

#ifdef WIN32
int main( int argc, char ** argv )
{
	int result = Game().Run(argc,argv);
	return result;
}
#elif defined(ANDROID)

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    gpg::AndroidInitialization::JNI_OnLoad(vm);

    return JNI_VERSION_1_6;
}

extern "C" {
    JNIEXPORT void JNICALL Java_com_kevinchoteau_splode_SOBLib_init(JNIEnv * env, jobject obj, jobject activity);
	JNIEXPORT void JNICALL Java_com_kevinchoteau_splode_SOBLib_changeSurface(JNIEnv * env, jobject obj,  jint width, jint height);
    JNIEXPORT void JNICALL Java_com_kevinchoteau_splode_SOBLib_step(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_kevinchoteau_splode_SOBLib_testAsset(JNIEnv* env, jobject obj, jobject assetManager, jstring path, jstring cachepath, jstring assetPath);
	JNIEXPORT void JNICALL Java_com_kevinchoteau_splode_SOBLib_inputPos(JNIEnv * env, jobject obj,  jint id, jboolean down, jint posX, jint posY);
	JNIEXPORT jboolean JNICALL Java_com_kevinchoteau_splode_SOBLib_MustLaunchPurchase(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_kevinchoteau_splode_SOBLib_OnStartPurchase(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_kevinchoteau_splode_SOBLib_OnEndPurchase(JNIEnv * env, jobject obj,jboolean _bSuccess);
	JNIEXPORT void Java_com_kevinchoteau_splode_SOBLib_OnActivityResult(JNIEnv *env, jobject thiz, jobject activity, jint requestCode,jint resultCode, jobject data);
	JNIEXPORT void Java_com_kevinchoteau_splode_SOBLib_OnActivityCreated( JNIEnv *env, jobject thiz, jobject activity, jobject saved_instance_state);
	JNIEXPORT void Java_com_kevinchoteau_splode_SOBLib_OnActivityDestroyed( JNIEnv *env, jobject thiz, jobject activity);
	JNIEXPORT void Java_com_kevinchoteau_splode_SOBLib_OnActivityPaused( JNIEnv *env, jobject thiz, jobject activity);
	JNIEXPORT void Java_com_kevinchoteau_splode_SOBLib_OnActivityResumed( JNIEnv *env, jobject thiz, jobject activity);
	JNIEXPORT void Java_com_kevinchoteau_splode_SOBLib_OnActivitySaveInstanceState( JNIEnv *env, jobject thiz, jobject activity, jobject out_state);
	JNIEXPORT void Java_com_kevinchoteau_splode_SOBLib_OnActivityStarted(JNIEnv *env, jobject thiz, jobject activity);
	JNIEXPORT void Java_com_kevinchoteau_splode_SOBLib_OnActivityStopped(JNIEnv *env, jobject thiz, jobject activity);
};

Game* g_pGame = NULL;

JNIEXPORT void JNICALL Java_com_kevinchoteau_splode_SOBLib_init(JNIEnv * env, jobject obj, jobject activity)
{
	__android_log_print(ANDROID_LOG_INFO,"Game","com.android.sob.SOBLib.init\n");
	g_pGame = NEW Game();
	g_pGame->Init(0,0);
	PlatformManager::GetInstance().SetActivity(activity);
	PlatformManager::GetInstance().Init();
}

JNIEXPORT void Java_com_kevinchoteau_splode_SOBLib_OnActivityResult(JNIEnv *env, jobject thiz, jobject activity, jint requestCode,jint resultCode, jobject data)
{
	__android_log_print(ANDROID_LOG_INFO,"Game","com.android.sob.SOBLib.OnActivityResult\n");
	gpg::AndroidSupport::OnActivityResult(env, activity, requestCode, resultCode,data);
}

JNIEXPORT void Java_com_kevinchoteau_splode_SOBLib_OnActivityCreated( JNIEnv *env, jobject thiz, jobject activity, jobject saved_instance_state)
{
	__android_log_print(ANDROID_LOG_INFO,"Game","com.android.sob.SOBLib.OnActivityCreated\n");
	gpg::AndroidSupport::OnActivityCreated(env, activity, saved_instance_state);
}

JNIEXPORT void Java_com_kevinchoteau_splode_SOBLib_OnActivityDestroyed( JNIEnv *env, jobject thiz, jobject activity)
{
	__android_log_print(ANDROID_LOG_INFO,"Game","com.android.sob.SOBLib.OnActivityDestroyed\n");
	gpg::AndroidSupport::OnActivityDestroyed(env, activity);
	g_pGame->Release(true);
	DELETE g_pGame;
	g_pGame = NULL;
}

JNIEXPORT void Java_com_kevinchoteau_splode_SOBLib_OnActivityPaused( JNIEnv *env, jobject thiz, jobject activity)
{
	__android_log_print(ANDROID_LOG_INFO,"Game","com.android.sob.SOBLib.OnActivityPaused\n");
	gpg::AndroidSupport::OnActivityPaused(env, activity);
	if( g_pGame )
		g_pGame->Pause();
}

JNIEXPORT void Java_com_kevinchoteau_splode_SOBLib_OnActivityResumed( JNIEnv *env, jobject thiz, jobject activity)
{
	__android_log_print(ANDROID_LOG_INFO,"Game","com.android.sob.SOBLib.OnActivityResumed\n");
	gpg::AndroidSupport::OnActivityResumed(env, activity);
	if( g_pGame )
		g_pGame->Resume();
}

JNIEXPORT void Java_com_kevinchoteau_splode_SOBLib_OnActivitySaveInstanceState( JNIEnv *env, jobject thiz, jobject activity, jobject out_state)
{
	__android_log_print(ANDROID_LOG_INFO,"Game","com.android.sob.SOBLib.OnActivitySaveInstanceState\n");
	gpg::AndroidSupport::OnActivitySaveInstanceState(env, activity, out_state);
}

JNIEXPORT void Java_com_kevinchoteau_splode_SOBLib_OnActivityStarted(JNIEnv *env, jobject thiz, jobject activity)
{
	__android_log_print(ANDROID_LOG_INFO,"Game","com.android.sob.SOBLib.OnActivityStarted\n");
	gpg::AndroidSupport::OnActivityStarted(env, activity);
	SoundManager::GetInstance().Resume();
}

JNIEXPORT void Java_com_kevinchoteau_splode_SOBLib_OnActivityStopped(JNIEnv *env, jobject thiz, jobject activity)
{
	__android_log_print(ANDROID_LOG_INFO,"Game","com.android.sob.SOBLib.OnActivityStopped\n");
	gpg::AndroidSupport::OnActivityStopped(env, activity);
	SoundManager::GetInstance().Pause();
}

JNIEXPORT void JNICALL Java_com_kevinchoteau_splode_SOBLib_changeSurface(JNIEnv * env, jobject obj,  jint width, jint height)
{
	__android_log_print(ANDROID_LOG_INFO,"Game","com.android.sob.SOBLib.changeSurface\n");
    g_pGame->SetWindowParams((S32)width,(S32)height,"Sob",TRUE);
}

JNIEXPORT void JNICALL Java_com_kevinchoteau_splode_SOBLib_step(JNIEnv * env, jobject obj)
{
	//__android_log_print(ANDROID_LOG_INFO,"Game","com.android.sob.SOBLib.step\n");
	//renderFrame();
	g_pGame->DoFrame();
}

JNIEXPORT void JNICALL Java_com_kevinchoteau_splode_SOBLib_testAsset(JNIEnv* env, jobject obj, jobject assetManager, jstring path, jstring cachepath, jstring assetPath)
{
	__android_log_print(ANDROID_LOG_INFO,"Game","com.android.sob.SOBLib.testAsset\n");
	OldFile::m_pAssetMgr = AAssetManager_fromJava(env,assetManager);
	const char *nativeString = env->GetStringUTFChars(path, JNI_FALSE);
	OldFile::m_sDataPath = nativeString;
	OldFile::m_sDataPath += "/";
	env->ReleaseStringUTFChars(path, nativeString);

	nativeString = env->GetStringUTFChars(cachepath, JNI_FALSE);
	OldFile::m_sCachePath = nativeString;
	OldFile::m_sCachePath += "/";
	env->ReleaseStringUTFChars(cachepath, nativeString);

	const char *assetString = env->GetStringUTFChars(assetPath, JNI_FALSE);
	OldFile::m_sAssetPath = assetString;
	env->ReleaseStringUTFChars(assetPath, assetString);
}

JNIEXPORT void JNICALL Java_com_kevinchoteau_splode_SOBLib_inputPos(JNIEnv * env, jobject obj,  jint id,  jboolean down,  jint posX, jint posY)
{
	InputManager::GetInstance().GetMouse(id).UpdateValue(Vec2i(posX,posY));
	InputManager::GetInstance().GetMouse(id).SetKeyState(MOUSE_Left,down);
}

JNIEXPORT jboolean JNICALL Java_com_kevinchoteau_splode_SOBLib_MustLaunchPurchase(JNIEnv * env, jobject obj)
{
	return PlatformManager::GetInstance().MustLaunchPurchase();
}

JNIEXPORT void JNICALL Java_com_kevinchoteau_splode_SOBLib_OnStartPurchase(JNIEnv * env, jobject obj)
{
	PlatformManager::GetInstance().OnStartPurchase();
}

JNIEXPORT void JNICALL Java_com_kevinchoteau_splode_SOBLib_OnEndPurchase(JNIEnv * env, jobject obj,jboolean _bSuccess)
{
	PlatformManager::GetInstance().OnEndPurchase(_bSuccess);
}

#endif
