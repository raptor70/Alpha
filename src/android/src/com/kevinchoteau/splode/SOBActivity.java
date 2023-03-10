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

import java.io.DataInputStream;
import java.io.File;
import java.io.IOException;
import java.util.zip.CRC32;

import com.android.vending.expansion.zipfile.ZipResourceFile;
import com.android.vending.expansion.zipfile.ZipResourceFile.ZipEntryRO;
import com.google.android.vending.expansion.downloader.Constants;
import com.google.android.vending.expansion.downloader.DownloadProgressInfo;
import com.google.android.vending.expansion.downloader.DownloaderClientMarshaller;
import com.google.android.vending.expansion.downloader.DownloaderServiceMarshaller;
import com.google.android.vending.expansion.downloader.Helpers;
import com.google.android.vending.expansion.downloader.IDownloaderClient;
import com.google.android.vending.expansion.downloader.IDownloaderService;
import com.google.android.vending.expansion.downloader.IStub;
import com.kevinchoteau.splode.util.IabResult;
import com.kevinchoteau.splode.util.Inventory;
import com.kevinchoteau.splode.util.Purchase;
import com.kevinchoteau.splode.util.IabHelper;

import android.app.Activity;
import android.app.PendingIntent;
import android.os.storage.OnObbStateChangeListener;
import android.os.storage.StorageManager;

import android.os.AsyncTask;
import android.os.Environment;
import android.os.Bundle;
import android.os.Messenger;
import android.os.SystemClock;
import android.provider.Settings;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.support.v4.view.MotionEventCompat;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.AssetManager;

public class SOBActivity extends Activity implements IDownloaderClient {

	public static final String BASE64_PUBLIC_KEY = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAhsX6sAEwk6wW3ktCEgmd7ywcJrzlZuyOdUGaL5WqbYs62PR6haNNypo9T9ou1K2Z7pFlLGyK5SEsV5nSfTpcqZnE6YIemSzfTu2Kol6KD42fgnX4ravmS3fBJc2284DGYUKR+hssSQxer9wASw6Z1qpikJGHVQ+EdYhN89lzRX0H9EZlPXutTgAd4P8q0GxWGzT4OBzPXFf6NTwybBibi9S78ylttQbg4ngpVbuNYaFTAqIHfO8I1loTTZJ3rDWYXElKQ+P/fukMny0aC924hlU5BwkcKmeOoemLULLASK+zhqqm0pRxeNJ2VxlRaWe64Fovz2CrtjtneMy+EJBm0QIDAQAB";
	SOBView mView;
	static AssetManager assetManager;
	static Activity currentAct;
	boolean bIsInputPressed;
	int posX, posY;
	IabHelper mHelper;
	boolean bIsDownloader;
	
	/**
	 * Here is where you place the data that the validator will use to determine
	 * if the file was delivered correctly. This is encoded in the source code
	 * so the application can easily determine whether the file has been
	 * properly delivered without having to talk to the server. If the
	 * application is using LVL for licensing, it may make sense to eliminate
	 * these checks and to just rely on the server.
	 */
	private static final XAPKFile[] xAPKS = { new XAPKFile(true, // true
																	// signifies
																	// a main
																	// file
			3, // the version of the APK that the file was uploaded
				// against
			87521942L // the length of the file in bytes
	),
	/*
	 * new XAPKFile( false, // false signifies a patch file 4, // the version of
	 * the APK that the patch file was uploaded // against 512860L // the length
	 * of the patch file in bytes )
	 */
	};

	@Override
	protected void onCreate(Bundle icicle) {
		super.onCreate(icicle);
		
		Log.v("Debug","onCreate");

		onCreateDownloader();
		
		SOBLib.OnActivityCreated(this, icicle);
	}

	public void onCreateGame() {
		bIsDownloader = false;
		
		mView = new SOBView(getApplication());
		setContentView(mView);
		
		assetManager = getAssets();
		//StorageManager sm = (StorageManager) getSystemService(STORAGE_SERVICE);
		String obbFile = Helpers.getExpansionAPKFileName(this, xAPKS[0].mIsMain,xAPKS[0].mFileVersion);
		obbFile = Helpers.generateSaveFileName(SOBActivity.this, obbFile);
		//String assetsPath = sm.getMountedObbPath(obbFile);
		SOBLib.testAsset(assetManager, getFilesDir().getAbsolutePath(),getCacheDir().getAbsolutePath(),obbFile);
		
		SOBView.mPurchaseCallBack = purchaseCallback;
		currentAct = this;

		/*
		 * base64EncodedPublicKey should be YOUR APPLICATION'S PUBLIC KEY (that
		 * you got from the Google Play developer console). This is not your
		 * developer public key, it's the *app-specific* public key.
		 * 
		 * Instead of just storing the entire literal string here embedded in
		 * the program, construct the key at runtime from pieces or use bit
		 * manipulation (for example, XOR with some other string) to hide the
		 * actual key. The key itself is not secret information, but we don't
		 * want to make it easy for an attacker to replace the public key with
		 * one of their own and then fake messages from the server.
		 */

		// Create the helper, passing it our context and the public key to
		// verify signatures with
		mHelper = new IabHelper(this, BASE64_PUBLIC_KEY);

		// enable debug logging (for a production application, you should set
		// this to false).
		mHelper.enableDebugLogging(true);

		// Start setup. This is asynchronous and the specified listener
		// will be called once setup completes.
		mHelper.startSetup(new IabHelper.OnIabSetupFinishedListener() {
			public void onIabSetupFinished(IabResult result) {

				if (!result.isSuccess()) {
					// Oh noes, there was a problem.
					Log.v("Inapp Purchase", "Purchase setup failed: " + result);
					return;
				}

				// Have we been disposed of in the meantime? If so, quit.
				if (mHelper == null)
					return;
				
			}
		});
	}
	

    
    static String SKU_UNLOCK = "purchase_unlock";
    
	SOBView.LaunchPurchaseCallBack purchaseCallback = new SOBView.LaunchPurchaseCallBack() {

		public void LaunchPurchase() {
			Log.v("Inapp Purchase", "OnStartPurchase");
			SOBLib.OnStartPurchase();
			mHelper.queryInventoryAsync(mGotInventoryListener);
		}
	};
	
	IabHelper.QueryInventoryFinishedListener mGotInventoryListener = new IabHelper.QueryInventoryFinishedListener() {
        public void onQueryInventoryFinished(IabResult result,
                Inventory inventory) {
            if (result.isFailure()) {
				SOBLib.OnEndPurchase(false);
            	Log.v("Inapp Purchase", "Failed to query inventory: " + result);
                return;
            }

            Purchase purchase = inventory.getPurchase(SKU_UNLOCK);
            if (purchase != null) {
            	Log.v("Inapp Purchase", "We have unlock. Consuming it.");
                mHelper.consumeAsync(purchase, mConsumeFinishedListener);
                return;
            }
			else
			{
				Log.v("Inapp Purchase", "Not already owned, buy it");
				mHelper.launchPurchaseFlow(currentAct, SKU_UNLOCK, 2212, mPurchaseFinishedListener);
				return;
			}
        }
    };
	
	IabHelper.OnConsumeFinishedListener mConsumeFinishedListener =
			   new IabHelper.OnConsumeFinishedListener() {
			   public void onConsumeFinished(Purchase purchase, IabResult result) {
			      if (result.isSuccess()) {
			    	  Log.v("Inapp Purchase", "Consume success");
			    	  SOBLib.OnEndPurchase(true);
			      }
			      else {
			    	  Log.v("Inapp Purchase", "Consume fail");
			    	  SOBLib.OnEndPurchase(false);
			      }
			   }
			};

	IabHelper.OnIabPurchaseFinishedListener mPurchaseFinishedListener = new IabHelper.OnIabPurchaseFinishedListener() {
		public void onIabPurchaseFinished(IabResult result, Purchase purchase) {
			Log.v("Inapp Purchase", "Purchase finished: " + result
					+ ", purchase: " + purchase);
			
			if( result.isSuccess()  )
			{
				Log.v("Inapp Purchase", "Purchase success, consume it !");
				mHelper.consumeAsync(purchase, mConsumeFinishedListener);
			}
			else
			{
				Log.v("Inapp Purchase", "Purchase failed, stop !");
				SOBLib.OnEndPurchase(false);
			}
		}
	};

	/**
	 * Connect the stub to our service on start.
	 */
	@Override
	protected void onStart() {
		super.onStart();
		if (bIsDownloader) {
			if (null != mDownloaderClientStub) {
				mDownloaderClientStub.connect(this);
			}
		}
		else
		{
			SOBLib.OnActivityStarted(this);
		}
	}

	/**
	 * Disconnect the stub from our service on stop
	 */
	@Override
	protected void onStop() {
		if (bIsDownloader) {
			if (null != mDownloaderClientStub) {
				mDownloaderClientStub.disconnect(this);
			}
		}
		else
		{
			SOBLib.OnActivityStopped(this);
		}
		super.onStop();
	}

	@Override
	protected void onPause() {
		super.onPause();
		if (!bIsDownloader) {
			mView.onPause();

			SOBLib.OnActivityPaused(this);
		}
	}

	@Override
	protected void onResume() {
		super.onResume();
		if (!bIsDownloader) {
			mView.onResume();

			SOBLib.OnActivityResumed(this);
		}
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();

		if (bIsDownloader) {
			this.mCancelValidation = true;
		} else {
			if (mHelper != null) {
				mHelper.dispose();
				mHelper = null;
			}

			SOBLib.OnActivityDestroyed(this);
		}
		
		UnmountObb();

		finish();
		
		System.gc();
	}
	
    protected void onSaveInstanceState(Bundle outState) 
    {
        super.onSaveInstanceState(outState);
        if (!bIsDownloader) 
        {
        	SOBLib.OnActivitySaveInstanceState(this, outState);
        }
    }

	@Override
	public boolean onTouchEvent(MotionEvent event) {

		int action = MotionEventCompat.getActionMasked(event);
		int pointerIndex = event.getActionIndex();
		int pointerId = event.getPointerId(pointerIndex);
		
		if (bIsDownloader) {
			return super.onTouchEvent(event);
		}
		else
		{
			switch (action) {
			case (MotionEvent.ACTION_DOWN):
			case (MotionEvent.ACTION_POINTER_DOWN):
			case (MotionEvent.ACTION_MOVE):
				// Log.d("DebugInput","Action Down " + action);
				SOBLib.inputPos(pointerId, true, (int) event.getX(pointerIndex),
						(int) event.getY(pointerIndex));
				return true;
			case (MotionEvent.ACTION_UP):
			case (MotionEvent.ACTION_POINTER_UP):
			case (MotionEvent.ACTION_CANCEL):
				// Log.d("DebugInput","Action Up " + action);
				SOBLib.inputPos(pointerId, false, (int) event.getX(pointerIndex),
						(int) event.getY(pointerIndex));
				return true;
			default:
				return super.onTouchEvent(event);
			}
		}
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if( !bIsDownloader )
		{
			SOBLib.OnActivityResult(this, requestCode,resultCode, data);
		
			if (mHelper == null)
				return;
	
			// Pass on the activity result to the helper for handling
			if (!mHelper.handleActivityResult(requestCode, resultCode, data)) {
				// not handled, so handle it ourselves (here's where you'd
				// perform any handling of activity results not related to in-app
				// billing...
				
			}
		}
		super.onActivityResult(requestCode, resultCode, data);
	}

	// ------------------------------------------------------------
	// ------------------------------------------------------------
	// APK Downloader
	// ------------------------------------------------------------
	// ------------------------------------------------------------

	private ProgressBar mPB;

	private TextView mStatusText;
	private TextView mProgressFraction;
	private TextView mProgressPercent;
	private TextView mAverageSpeed;
	private TextView mTimeRemaining;

	private View mDashboard;
	private View mCellMessage;

	private Button mPauseButton;
	private Button mWiFiSettingsButton;

	private boolean mStatePaused;
	private int mState;

	private IDownloaderService mRemoteService;

	private IStub mDownloaderClientStub;

	private void setState(int newState) {
		if (mState != newState) {
			mState = newState;
			mStatusText.setText(Helpers.getDownloaderStringResourceIDFromState(newState));
		}
	}

	private void setButtonPausedState(boolean paused) {
		mStatePaused = paused;
		int stringResourceID = paused ? R.string.text_button_resume
				: R.string.text_button_pause;
		mPauseButton.setText(stringResourceID);
	}

	/**
	 * This is a little helper class that demonstrates simple testing of an
	 * Expansion APK file delivered by Market. You may not wish to hard-code
	 * things such as file lengths into your executable... and you may wish to
	 * turn this code off during application development.
	 */
	private static class XAPKFile {
		public final boolean mIsMain;
		public final int mFileVersion;
		public final long mFileSize;

		XAPKFile(boolean isMain, int fileVersion, long fileSize) {
			mIsMain = isMain;
			mFileVersion = fileVersion;
			mFileSize = fileSize;
		}
	}

	/**
	 * Go through each of the APK Expansion files defined in the structure above
	 * and determine if the files are present and match the required size. Free
	 * applications should definitely consider doing this, as this allows the
	 * application to be launched for the first time without having a network
	 * connection present. Paid applications that use LVL should probably do at
	 * least one LVL check that requires the network to be present, so this is
	 * not as necessary.
	 * 
	 * @return true if they are present.
	 */
	boolean expansionFilesDelivered() {
		for (XAPKFile xf : xAPKS) {
			String fileName = Helpers.getExpansionAPKFileName(this, xf.mIsMain,
					xf.mFileVersion);
			if (!Helpers.doesFileExist(this, fileName, xf.mFileSize, false))
				return false;
		}
		return true;
	}

	/**
	 * Calculating a moving average for the validation speed so we don't get
	 * jumpy calculations for time etc.
	 */
	static private final float SMOOTHING_FACTOR = 0.005f;

	/**
	 * Used by the async task
	 */
	private boolean mCancelValidation;

	/**
	 * Go through each of the Expansion APK files and open each as a zip file.
	 * Calculate the CRC for each file and return false if any fail to match.
	 * 
	 * @return true if XAPKZipFile is successful
	 */
//	void validateXAPKZipFiles() {
//		AsyncTask<Object, DownloadProgressInfo, Boolean> validationTask = new AsyncTask<Object, DownloadProgressInfo, Boolean>() {
//
//			@Override
//			protected void onPreExecute() {
//				mDashboard.setVisibility(View.VISIBLE);
//				mCellMessage.setVisibility(View.GONE);
//				mStatusText.setText(R.string.text_verifying_download);
//				mPauseButton.setOnClickListener(new View.OnClickListener() {
//					@Override
//					public void onClick(View view) {
//						mCancelValidation = true;
//					}
//				});
//				mPauseButton.setText(R.string.text_button_cancel_verify);
//				super.onPreExecute();
//			}
//
//			@Override
//			protected Boolean doInBackground(Object... params) {
//				for (XAPKFile xf : xAPKS) {
//					String fileName = Helpers.getExpansionAPKFileName(
//							SOBActivity.this, xf.mIsMain, xf.mFileVersion);
//					if (!Helpers.doesFileExist(SOBActivity.this, fileName,
//							xf.mFileSize, false))
//						return false;
//					fileName = Helpers.generateSaveFileName(SOBActivity.this,
//							fileName);
//					ZipResourceFile zrf;
//					byte[] buf = new byte[1024 * 256];
//					try {
//						zrf = new ZipResourceFile(fileName);
//						ZipEntryRO[] entries = zrf.getAllEntries();
//						/**
//						 * First calculate the total compressed length
//						 */
//						long totalCompressedLength = 0;
//						for (ZipEntryRO entry : entries) {
//							totalCompressedLength += entry.mCompressedLength;
//						}
//						float averageVerifySpeed = 0;
//						long totalBytesRemaining = totalCompressedLength;
//						long timeRemaining;
//						/**
//						 * Then calculate a CRC for every file in the Zip file,
//						 * comparing it to what is stored in the Zip directory.
//						 * Note that for compressed Zip files we must extract
//						 * the contents to do this comparison.
//						 */
//						for (ZipEntryRO entry : entries) {
//							if (-1 != entry.mCRC32) {
//								long length = entry.mUncompressedLength;
//								CRC32 crc = new CRC32();
//								DataInputStream dis = null;
//								try {
//									dis = new DataInputStream(
//											zrf.getInputStream(entry.mFileName));
//
//									long startTime = SystemClock.uptimeMillis();
//									while (length > 0) {
//										int seek = (int) (length > buf.length ? buf.length
//												: length);
//										dis.readFully(buf, 0, seek);
//										crc.update(buf, 0, seek);
//										length -= seek;
//										long currentTime = SystemClock
//												.uptimeMillis();
//										long timePassed = currentTime
//												- startTime;
//										if (timePassed > 0) {
//											float currentSpeedSample = (float) seek
//													/ (float) timePassed;
//											if (0 != averageVerifySpeed) {
//												averageVerifySpeed = SMOOTHING_FACTOR
//														* currentSpeedSample
//														+ (1 - SMOOTHING_FACTOR)
//														* averageVerifySpeed;
//											} else {
//												averageVerifySpeed = currentSpeedSample;
//											}
//											totalBytesRemaining -= seek;
//											timeRemaining = (long) (totalBytesRemaining / averageVerifySpeed);
//											this.publishProgress(new DownloadProgressInfo(
//													totalCompressedLength,
//													totalCompressedLength
//															- totalBytesRemaining,
//													timeRemaining,
//													averageVerifySpeed));
//										}
//										startTime = currentTime;
//										if (mCancelValidation)
//											return true;
//									}
//									if (crc.getValue() != entry.mCRC32) {
//										Log.e(Constants.TAG,
//												"CRC does not match for entry: "
//														+ entry.mFileName);
//										Log.e(Constants.TAG, "In file: "
//												+ entry.getZipFileName());
//										return false;
//									}
//								} finally {
//									if (null != dis) {
//										dis.close();
//									}
//								}
//							}
//						}
//					} catch (IOException e) {
//						e.printStackTrace();
//						return false;
//					}
//				}
//				return true;
//			}
//
//			@Override
//			protected void onProgressUpdate(DownloadProgressInfo... values) {
//				onDownloadProgress(values[0]);
//				super.onProgressUpdate(values);
//			}
//
//			@Override
//			protected void onPostExecute(Boolean result) {
//				if (result) {
//					mDashboard.setVisibility(View.VISIBLE);
//					mCellMessage.setVisibility(View.GONE);
//					mStatusText.setText(R.string.text_validation_complete);
//					mPauseButton.setOnClickListener(new View.OnClickListener() {
//						@Override
//						public void onClick(View view) {
//							finish();
//						}
//					});
//					mPauseButton.setText(android.R.string.ok);
//				} else {
//					mDashboard.setVisibility(View.VISIBLE);
//					mCellMessage.setVisibility(View.GONE);
//					mStatusText.setText(R.string.text_validation_failed);
//					mPauseButton.setOnClickListener(new View.OnClickListener() {
//						@Override
//						public void onClick(View view) {
//							finish();
//						}
//					});
//					mPauseButton.setText(android.R.string.cancel);
//				}
//				super.onPostExecute(result);
//			}
//
//		};
//		validationTask.execute(new Object());
//	}

	/**
	 * If the download isn't present, we initialize the download UI. This ties
	 * all of the controls into the remote service calls.
	 */
	private void initializeDownloadUI() {
		mDownloaderClientStub = DownloaderClientMarshaller.CreateStub(this,
				SOBDownloaderService.class);
		setContentView(R.layout.downloader);

		mPB = (ProgressBar) findViewById(R.id.progressBar);
		mStatusText = (TextView) findViewById(R.id.statusText);
		mProgressFraction = (TextView) findViewById(R.id.progressAsFraction);
		mProgressPercent = (TextView) findViewById(R.id.progressAsPercentage);
		mAverageSpeed = (TextView) findViewById(R.id.progressAverageSpeed);
		mTimeRemaining = (TextView) findViewById(R.id.progressTimeRemaining);
		mDashboard = findViewById(R.id.downloaderDashboard);
		mCellMessage = findViewById(R.id.approveCellular);
		mPauseButton = (Button) findViewById(R.id.pauseButton);
		mWiFiSettingsButton = (Button) findViewById(R.id.wifiSettingsButton);

		mPauseButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				if (mStatePaused) {
					mRemoteService.requestContinueDownload();
				} else {
					mRemoteService.requestPauseDownload();
				}
				setButtonPausedState(!mStatePaused);
			}
		});

		mWiFiSettingsButton.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View v) {
				startActivity(new Intent(Settings.ACTION_WIFI_SETTINGS));
			}
		});

		Button resumeOnCell = (Button) findViewById(R.id.resumeOverCellular);
		resumeOnCell.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				mRemoteService
						.setDownloadFlags(IDownloaderService.FLAGS_DOWNLOAD_OVER_CELLULAR);
				mRemoteService.requestContinueDownload();
				mCellMessage.setVisibility(View.GONE);
			}
		});

	}

	/**
	 * Called when the activity is first create; we wouldn't create a layout in
	 * the case where we have the file and are moving to another activity
	 * without downloading.
	 */
	public boolean onCreateDownloader() {
		/**
		 * Both downloading and validation make use of the "download" UI
		 */
		
		bIsDownloader = true;
		/**
		 * Before we do anything, are the files we expect already here and
		 * delivered (presumably by Market) For free titles, this is probably
		 * worth doing. (so no Market request is necessary)
		 */
		if (!expansionFilesDelivered()) {
			Log.v("DebugDownloader","No obb file found : todo downloader");
			//finish();
			try {
				Log.v("DebugDownloader","initializeDownloadUI");
			  	initializeDownloadUI();
				Intent launchIntent = SOBActivity.this.getIntent();
				Intent intentToLaunchThisActivityFromNotification = new Intent(
						SOBActivity.this, SOBActivity.this.getClass());
				intentToLaunchThisActivityFromNotification
						.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK
								| Intent.FLAG_ACTIVITY_CLEAR_TOP);
				intentToLaunchThisActivityFromNotification
						.setAction(launchIntent.getAction());

				if (launchIntent.getCategories() != null) {
					for (String category : launchIntent.getCategories()) {
						intentToLaunchThisActivityFromNotification
								.addCategory(category);
					}
				}

				// Build PendingIntent used to open this activity from
				// Notification
				PendingIntent pendingIntent = PendingIntent.getActivity(
						SOBActivity.this, 0,
						intentToLaunchThisActivityFromNotification,
						PendingIntent.FLAG_UPDATE_CURRENT);
				// Request to start the download
				int startResult = DownloaderClientMarshaller
						.startDownloadServiceIfRequired(this, pendingIntent,
								SOBDownloaderService.class);
				Log.v("DebugDownloader","startDownloadServiceIfRequired");
				
				if (startResult != DownloaderClientMarshaller.NO_DOWNLOAD_REQUIRED) {
					// The DownloaderService has started downloading the files,
					// show progress
					Log.v("DebugDownloader","reinitializeDownloadUI");
					initializeDownloadUI();
					return true;
				} // otherwise, download not needed so we fall through to
					// starting the movie
			} catch (NameNotFoundException e) {
				Log.e("Downloader", "Cannot find own package! MAYDAY!");
				e.printStackTrace();
			}

		} else {
			// validateXAPKZipFiles();
			Log.v("Debug","Mount obb");
			MountObb();
			return false;
		}
		return true;
	}
	
	private void MountObb() 
	{
		/*StorageManager sm = (StorageManager) getSystemService(STORAGE_SERVICE);
		String obbFile = Helpers.getExpansionAPKFileName(this, xAPKS[0].mIsMain,xAPKS[0].mFileVersion);
		obbFile = Helpers.generateSaveFileName(SOBActivity.this, obbFile);
		Log.v("DebugDownloader","mountObb" + obbFile);
		if( !sm.isObbMounted(obbFile))
			sm.mountObb(obbFile, null, storageListener);
		else*/
			onCreateGame();
	}
	
	private void UnmountObb() 
	{
		/*StorageManager sm = (StorageManager) getSystemService(STORAGE_SERVICE);
		String obbFile = Helpers.getExpansionAPKFileName(this, xAPKS[0].mIsMain,xAPKS[0].mFileVersion);
		obbFile = Helpers.generateSaveFileName(SOBActivity.this, obbFile);
		Log.v("DebugDownloader","unmountObb" + obbFile);
		if( sm.isObbMounted(obbFile))
			sm.unmountObb(obbFile, true, storageListener);*/
	}

	OnObbStateChangeListener storageListener = new OnObbStateChangeListener() {

		public void onObbStateChange(String path, int state) {
			if (state == UNMOUNTED) {
				Log.v("Storage", "Unmounted " + path);
			}
			else if (state == MOUNTED) {
				Log.v("Storage", "Mounted " + path);
				onCreateGame();
			} else {
				Log.e("Storage", "No Mounted " + path);
				finish();
			}
		}
	};

	/**
	 * Critical implementation detail. In onServiceConnected we create the
	 * remote service and marshaler. This is how we pass the client information
	 * back to the service so the client can be properly notified of changes. We
	 * must do this every time we reconnect to the service.
	 */
	@Override
	public void onServiceConnected(Messenger m) {
		mRemoteService = DownloaderServiceMarshaller.CreateProxy(m);
		mRemoteService.onClientUpdated(mDownloaderClientStub.getMessenger());
	}

	/**
	 * The download state should trigger changes in the UI --- it may be useful
	 * to show the state as being indeterminate at times. This sample can be
	 * considered a guideline.
	 */
	@Override
	public void onDownloadStateChanged(int newState) {
		setState(newState);
		boolean showDashboard = true;
		boolean showCellMessage = false;
		boolean paused;
		boolean indeterminate;
		Log.v("DebugDownloader","onDownloadStateChanged" + newState);
		switch (newState) {
		case IDownloaderClient.STATE_IDLE:
			// STATE_IDLE means the service is listening, so it's
			// safe to start making calls via mRemoteService.
			paused = false;
			indeterminate = true;
			break;
		case IDownloaderClient.STATE_CONNECTING:
		case IDownloaderClient.STATE_FETCHING_URL:
			showDashboard = true;
			paused = false;
			indeterminate = true;
			break;
		case IDownloaderClient.STATE_DOWNLOADING:
			paused = false;
			showDashboard = true;
			indeterminate = false;
			break;

		case IDownloaderClient.STATE_FAILED_CANCELED:
		case IDownloaderClient.STATE_FAILED:
		case IDownloaderClient.STATE_FAILED_FETCHING_URL:
		case IDownloaderClient.STATE_FAILED_UNLICENSED:
			paused = true;
			showDashboard = false;
			indeterminate = false;
			break;
		case IDownloaderClient.STATE_PAUSED_NEED_CELLULAR_PERMISSION:
		case IDownloaderClient.STATE_PAUSED_WIFI_DISABLED_NEED_CELLULAR_PERMISSION:
			showDashboard = false;
			paused = true;
			indeterminate = false;
			showCellMessage = true;
			break;

		case IDownloaderClient.STATE_PAUSED_BY_REQUEST:
			paused = true;
			indeterminate = false;
			break;
		case IDownloaderClient.STATE_PAUSED_ROAMING:
		case IDownloaderClient.STATE_PAUSED_SDCARD_UNAVAILABLE:
			paused = true;
			indeterminate = false;
			break;
		case IDownloaderClient.STATE_COMPLETED:
			showDashboard = false;
			paused = false;
			indeterminate = false;
			finish();
			//validateXAPKZipFiles();
			//MountObb();
			return;
		default:
			paused = true;
			indeterminate = true;
			showDashboard = true;
		}
		int newDashboardVisibility = showDashboard ? View.VISIBLE : View.GONE;
		if (mDashboard.getVisibility() != newDashboardVisibility) {
			mDashboard.setVisibility(newDashboardVisibility);
		}
		int cellMessageVisibility = showCellMessage ? View.VISIBLE : View.GONE;
		if (mCellMessage.getVisibility() != cellMessageVisibility) {
			mCellMessage.setVisibility(cellMessageVisibility);
		}

		mPB.setIndeterminate(indeterminate);
		setButtonPausedState(paused);
	}

	/**
	 * Sets the state of the various controls based on the progressinfo object
	 * sent from the downloader service.
	 */
	@Override
	public void onDownloadProgress(DownloadProgressInfo progress) {
		mAverageSpeed.setText(getString(R.string.kilobytes_per_second,
				Helpers.getSpeedString(progress.mCurrentSpeed)));
		mTimeRemaining.setText(getString(R.string.time_remaining,
				Helpers.getTimeRemaining(progress.mTimeRemaining)));

		progress.mOverallTotal = progress.mOverallTotal;
		mPB.setMax((int) (progress.mOverallTotal >> 8));
		mPB.setProgress((int) (progress.mOverallProgress >> 8));
		mProgressPercent.setText(Long.toString(progress.mOverallProgress * 100
				/ progress.mOverallTotal)
				+ "%");
		mProgressFraction.setText(Helpers.getDownloadProgressString(
				progress.mOverallProgress, progress.mOverallTotal));
	}
}
