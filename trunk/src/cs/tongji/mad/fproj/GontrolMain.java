package cs.tongji.mad.fproj;

import java.net.Socket;
import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Point;
import android.graphics.PorterDuff;
import android.graphics.Rect;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.preference.PreferenceManager;
import android.util.FloatMath;
import android.util.Log;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.inputmethod.InputMethodManager;
import android.widget.FrameLayout;
import android.widget.LinearLayout;

public class GontrolMain extends Activity implements OnTouchListener {

	/** Called when the activity is first created. */
    

	//setting area--tapping enable;tap sensitivity;wheel button sensitivity;flap speed
	private static boolean enTap = true;                     //enable tap*1
	private static int FIRST_TAP_TIME = 120;           //time between tap down and up*2 100-130
	private static int TAP_GAP = 5;                    //distance between tap down and up
	private static int COUNTDOWN_TIME = 110;           //timer for 3rd tap*
	private static int FLING_TAP_TIME = 120;           //2 pointer down time*3 100-130
	private static int FLING_TAP_DIS = 300;            //2 pointer distance
	private static int FLING_TAP_Y = 80;               //2 pointer y dif
	private static int FLING_MOVE_X = 30;              //x-axis cannot move more than this
	private static int FLING_SPEED = 8;                //the speed of fling*4 5-10
	

	// Sensors, no longer used
	private float fAxis[] = new float[3];
	private SensorManager sm;
	private Sensor sAcc;
	private SensorEventListener accListener;
	private final float FILTERING_VALAUE = 0.1f;
	private float lowX, lowY, lowZ;
	/*----------------------------------------------------------------------------------------*/
	// view group controls
	private LinearLayout llbg;
	private TouchButton iv1;
	private TouchButton iv2;
	//private TextView tx1;
	private FrameLayout fl1;
	private FrameLayout fl2;
	private FrameLayout fl0;

	// self-defined classes
	private TouchPad tonyP;// catch touchmotions
	private ControlData cd;// the data structure to be transferred to PC

	// networks
	private Socket DroidEnd;

	private int iWidth;
	private int iHeight;

	private Rect fl1rect;
	private Rect fl2rect;
	private Rect fl0rect;

	private int iBtnFlag;
	private boolean bDrag;
	private boolean bTap;
	private boolean bTap2;
	private boolean bTimerinUse;
	private boolean bFling;
	private boolean bGrab;
	private long timeDown;
	private long timeUp;
	private long timeFlingStart;
	private Point p4tap;
	private CountDownTimer cdt;
	private ControlDatas gcds;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		// setContentView(new TouchPad(this));

		iWidth = getWindowManager().getDefaultDisplay().getWidth();
		iHeight = getWindowManager().getDefaultDisplay().getHeight();

		iBtnFlag = 0;
		bDrag = false;
		bTap = false;
		bTap2 = false;
		bTimerinUse = false;
		bFling = false;
		bGrab = false;
		p4tap = new Point();
		gcds = new ControlDatas();
		
		SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(this);
		enTap = sp.getBoolean("et", true);
		FIRST_TAP_TIME = sp.getInt("ts", 120);
		FLING_TAP_TIME = sp.getInt("fs", 120);
		FLING_SPEED = sp.getInt("ws", 8);
		

		cdt = new CountDownTimer(COUNTDOWN_TIME, 1) {
			@Override
			public void onFinish() {
				// TODO Auto-generated method stub
				bTimerinUse = false;
				sendCDatas(gcds, null, 0, true);
			}

			@Override
			public void onTick(long millisUntilFinished) {
				// TODO Auto-generated method stub
			}

		};

		//tx1 = (TextView) findViewById(R.id.TextView1);

		iv1 = (TouchButton) findViewById(R.id.imageView1);
		iv1.setButtonNumber(1);
		iv1.setMaxWidth(iWidth / 2 - 1); // in case of to small screen

		iv2 = (TouchButton) findViewById(R.id.imageView2);
		iv2.setButtonNumber(2);
		iv2.setMaxWidth(iWidth / 2 - 1);

		fl0 = (FrameLayout) findViewById(R.id.frameLayout1);
		fl1 = (FrameLayout) findViewById(R.id.frameLayout2);
		fl2 = (FrameLayout) findViewById(R.id.frameLayout3);
		// fl.setOnTouchListener(iv1);

		llbg = (LinearLayout) findViewById(R.id.LinearLayaout1);
		llbg.setOnTouchListener(this);

		// no longer used
		// ****************************************cd = new ControlData();
		tonyP = (TouchPad) findViewById(R.id.surfaceView1);
		//tonyP.tv = tx1;
		tonyP.mCD = cd;
		tonyP.mDroidEnd = DroidEnd;
		// tonyP.setOnTouchListener(tonyP);

		// iv1.tpref = tonyP;
		// iv2.tpref = tonyP;
		// 从系统服务中获得传感器管理器
		sm = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
		// 从传感器管理器中获得全部的传感器列表
		List<Sensor> allSensors = sm.getSensorList(Sensor.TYPE_ACCELEROMETER);
		sAcc = sm.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
		// 显示有多少个传感器
		// tx1.setText("经检测该手机有" + allSensors.size() + "个传感器，他们分别是：\n");
		accListener = new SensorEventListener() {
			@Override
			public void onAccuracyChanged(Sensor sensor, int accuracy) {
				// TODO Auto-generated method stub
			}

			@Override
			public void onSensorChanged(SensorEvent event) {
				float x = event.values[SensorManager.DATA_X];
				float y = event.values[SensorManager.DATA_Y];
				float z = event.values[SensorManager.DATA_Z];
				// Low-Pass Filter
				lowX = x * FILTERING_VALAUE + lowX * (1.0f - FILTERING_VALAUE);
				lowY = y * FILTERING_VALAUE + lowY * (1.0f - FILTERING_VALAUE);
				lowZ = z * FILTERING_VALAUE + lowZ * (1.0f - FILTERING_VALAUE);
				// High-pass filter
				float highX = x - lowX;
				float highY = y - lowY;
				float highZ = z - lowZ;
				// TODO Auto-generated method stub
				fAxis[0] = event.values[0];
				fAxis[1] = event.values[1];
				fAxis[2] = event.values[2];
			}

		};

	}

	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();
	}

	@Override
	protected void onStop() {
		// TODO Auto-generated method stub
		super.onStop();
		sm.unregisterListener(accListener);
	}

	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		tonyP.pt.tFlag = false;
		super.onDestroy();

	}

	@Override
	synchronized public boolean onTouch(View arg0, MotionEvent arg1) {
		// TODO Auto-generated method stub
		switch (arg1.getAction() & MotionEvent.ACTION_MASK) {
		case MotionEvent.ACTION_DOWN: {// first pointer down
			setRects();
			getMousePos(arg1);
			buttonDown(arg1);
			//Log.d("TOUCH", "TP_DOWN" + arg0.toString());
			break;
		}
		case MotionEvent.ACTION_POINTER_DOWN:// second pointer down
			getMousePos(arg1);
			if (tonyP.fingerCount == 1 && isInRect(tonyP.mousePos, fl0rect)
					&& arg1.getActionIndex() == 1) {// button already down
				bDrag = true;
			}
			// bTap = false;//fling
			if (bTap2 && isInRect(tonyP.mousePos, fl0rect)) {// 2nd pointer on

				bTap = false;//set up another flag
				bTap2 = false;
				// the touchpad

				
				if ((getDistance(p4tap, arg1.getX(1), arg1.getY(1)) <= FLING_TAP_DIS)
						&& (Math.abs(p4tap.y - arg1.getY(1)) <= FLING_TAP_Y)) {
					Log.d("TOUCH_FLING",
							"WHEEL_BUTTON_DOWN"
									+ Math.abs(p4tap.y - arg1.getY(1))
									+ "||"
									+ getDistance(p4tap, arg1.getX(1),
											arg1.getY(1)));
					bFling = true;
					timeDown = arg1.getEventTime();
				    timeFlingStart = timeDown - timeFlingStart;//not in use
				}

			}
			//Log.d("TOUCH", "TP_POINTER_DOWN:");
			break;
		case MotionEvent.ACTION_MOVE:// pointers move
			// ControlDatas cds = new ControlDatas();
			if (bFling) {
				FlingMove(arg1);
				break;
			}
			if (tonyP.fingerCount == 0 || bDrag) {//moving normally or drag
				MouseData mouseData = new MouseData();

				tonyP.mousePos.x = (int) arg1.getX(tonyP.fingerCount);
				tonyP.mousePos.y = (int) arg1.getY(tonyP.fingerCount);
//				tonyP.mousePos.x = (int)arg1.getX(arg1.getPointerCount() - 1);
//				tonyP.mousePos.y = (int)arg1.getY(arg1.getPointerCount() - 1);

				int diffX = (tonyP.mousePos.x - tonyP.lastPos.x);
				int diffY = (tonyP.mousePos.y - tonyP.lastPos.y);
				mouseData.setMove(diffX, diffY);
				sendCDatas(mouseData);

				tonyP.lastPos.x = tonyP.mousePos.x;
				tonyP.lastPos.y = tonyP.mousePos.y;

//				tx1.setText("x: " + tonyP.mousePos.x + "____y: "
//						+ tonyP.mousePos.y);
			}
			//Log.d("TOUCH", "ACTION_MOVE_PURE" + arg1.getPointerCount());
			break;
		case MotionEvent.ACTION_POINTER_UP:// first pointer up

			if (bDrag) {//one pointer on the pad
				if (arg1.getActionIndex() == 1) {// pointer up
					bDrag = false;
					//moveUp();
				} else if (arg1.getActionIndex() == 0) {// button up
					bDrag = false;                               //a probable fix, may hide bug
					buttonUp();
				}
			}
			if(bGrab || tonyP.fingerCount == 0){             //further bugs to be detected
				if (arg1.getActionIndex() == 1) {//
					tonyP.mousePos.x = (int) arg1.getX(0);
					tonyP.mousePos.y = (int) arg1.getY(0);
					tonyP.lastPos.x = tonyP.mousePos.x;
					tonyP.lastPos.y = tonyP.mousePos.y;
				} else if (arg1.getActionIndex() == 0) {//
					tonyP.mousePos.x = (int) arg1.getX(1);
					tonyP.mousePos.y = (int) arg1.getY(1);
					tonyP.lastPos.x = tonyP.mousePos.x;
					tonyP.lastPos.y = tonyP.mousePos.y;
				}
			}
			if (bFling) {// one pointer up when flipping
				timeUp = arg1.getEventTime();
				timeDown = timeUp - timeDown;
				if (timeDown <= FLING_TAP_TIME) {//wheel button click
					Log.d("TOUCH_FLING", "WHEEL_BUTTON_UP");
					MouseData mouseData1 = new MouseData();
					MouseData mouseData2 = new MouseData();
					gcds.clear();
					mouseData1.setButtonDown(2);
					sendCDatas(gcds, mouseData1, 1, false);
					mouseData2.setButtonUp(2);
					sendCDatas(gcds, mouseData2, 1, true);
				}
				bFling = false;
			}
			//Log.d("TOUCH", "ACTION_POINTER_UP:" + arg1.getActionIndex());
			break;
		case MotionEvent.ACTION_UP:// second pointer up
			if (bTap) {
				bTap = false; // newly added
				timeUp = arg1.getEventTime();
				timeDown = timeUp - timeDown;
				//Log.d("TOUCH", "DOWN_TIME:" + timeDown);
				if (timeDown <= FIRST_TAP_TIME) {
					int iDis = (int) getDistance(p4tap, arg1.getX(),
							arg1.getY());
					if (iDis <= TAP_GAP) {// the pointer didn't move too far
						tapScreen(timeDown);
						break;
					}

				} else {// time down is quite long
					dragUp();
				}
			}
			if(bGrab){
				dragUp();
			}
			if (tonyP.fingerCount == 1) {
				buttonUp();
			} else if (tonyP.fingerCount == 0) {// pointer up
				moveUp();
			}

			//Log.d("TOUCH", "ACTION_UP");
			break;
		default:
			break;
		}
		return true;
	}

	private boolean isInRect(Point p, Rect area) {
		return p.x <= area.right && p.x >= area.left && p.y <= area.bottom
				&& p.y >= area.top;
	}

	private void getMousePos(MotionEvent arg) {
		tonyP.mousePos.x = (int) arg.getX(tonyP.fingerCount);
		tonyP.mousePos.y = (int) arg.getY(tonyP.fingerCount);
		tonyP.lastPos.x = tonyP.mousePos.x;
		tonyP.lastPos.y = tonyP.mousePos.y;
	}

	private void moveUp() {
		//tonyP.mousePos.x = -20;
		//tonyP.mousePos.y = -20;
	}

	private void buttonUp() {
		MouseData mouseData = new MouseData();
		switch (iBtnFlag) {
		case 1:
			mouseData.setButtonUp(1);
			iBtnFlag = 0;
			tonyP.fingerCount = 0;
			iv1.setColorFilter(0xff000000, PorterDuff.Mode.SRC_ATOP);
			break;
		case 2:
			mouseData.setButtonUp(3);
			iBtnFlag = 0;
			tonyP.fingerCount = 0;
			iv2.setColorFilter(0xff000000, PorterDuff.Mode.SRC_ATOP);
			break;
		default:
			break;
		}
		// Log.d("TOUCH", mouseData.toString());
		sendCDatas(mouseData);
	}

	synchronized private void buttonDown(MotionEvent arg1) {
		MouseData mouseData = new MouseData();
		if (isInRect(tonyP.mousePos, fl1rect)) {// left button down
			mouseData.setButtonDown(1);
			//iv1.setColorFilter(0xff00ca00);
			iv1.setColorFilter(0xff00ca00, PorterDuff.Mode.SRC_ATOP);
			tonyP.fingerCount = 1;
			iBtnFlag = 1;
		} else if (isInRect(tonyP.mousePos, fl2rect)) {// right button down
			mouseData.setButtonDown(3);
			iv2.setColorFilter(0xff00ca00, PorterDuff.Mode.SRC_ATOP);
			tonyP.fingerCount = 1;
			iBtnFlag = 2;
		} else if (isInRect(tonyP.mousePos, fl0rect)) {// down on the pad, tap
			if(enTap){
				bTap = true;                           //set here to disable tapping
			}
			bTap2 = true;
			p4tap.x = (int) arg1.getX(); // position for tap down
			p4tap.y = (int) arg1.getY();
			timeDown = arg1.getEventTime(); // when the tap is down
			timeFlingStart = timeDown;
			// timeUp = timeDown - timeUp;
			if (bTimerinUse) {// still counting down, the second tap, add some
								// signal to represent the 2nd tap
				mouseData.setButtonDown(1);
				bTimerinUse = false;
				cdt.cancel();
				sendCDatas(gcds, null, -1, true);// only button down
				bGrab = true;
				//Log.d("TOUCH_TAP", "DRAG_DOWN");
				return;
			}
		}
		sendCDatas(mouseData);
	}

	private void tapScreen(long sleepTime) {

		MouseData mouseData1 = new MouseData();
		MouseData mouseData2 = new MouseData();

		gcds.clear();

		mouseData1.setButtonDown(1);
		mouseData2.setButtonUp(1);
		if (bGrab) {
			bGrab = false;
			sendCDatas(gcds, mouseData2, 1, false);
		}
		sendCDatas(gcds, mouseData1, 1, false);
		sendCDatas(gcds, mouseData2, 1, false);

		bTimerinUse = true;
		cdt.start(); // start new timer
		// iv1.setColorFilter(0xff000000);
	}

	private void FlingMove(MotionEvent arg1) {
		MouseData mouseData = new MouseData();
		Point psec = new Point();
		psec.x = (int) arg1.getX(1);
		psec.y = (int) arg1.getY(1);

		tonyP.mousePos.x = (int) arg1.getX(tonyP.fingerCount);
		tonyP.mousePos.y = (int) arg1.getY(tonyP.fingerCount);

		int diff2py = Math.abs(tonyP.mousePos.y - psec.y);

		int diffX = (tonyP.mousePos.x - tonyP.lastPos.x);
		int diffY = (tonyP.mousePos.y - tonyP.lastPos.y);
		if (Math.abs(diffX) <= FLING_MOVE_X && diff2py <= FLING_TAP_Y           //
				&& (getDistance(tonyP.mousePos, psec.x, psec.y) <= FLING_TAP_DIS)) {
			mouseData.setWheel(FLING_SPEED * diffY);
			// mouseData.setMove(diffX, diffY);
			sendCDatas(mouseData);
		} else {
			// bFling = false; //wait until pointer up
		}
		// mouseData.setMove(diffX, diffY);

		// sendCDatas(mouseData);

		tonyP.lastPos.x = tonyP.mousePos.x;
		tonyP.lastPos.y = tonyP.mousePos.y;

		//tx1.setText("x: " + tonyP.mousePos.x + "____y: " + tonyP.mousePos.y);
	}
	private void dragUp(){
		MouseData mouseData = new MouseData();
		mouseData.setButtonUp(1);
		bGrab = false;
		//Log.d("TOUCH_TAP", "DRAG_UP");
		sendCDatas(mouseData);
	}

	// /////////////////////////////////////////////////////////
	// marvelous dividing line
	// /////////////////////////////////////////////////////////
	private void setRects() {
		if (fl1rect == null || fl2rect == null) {
			fl0rect = new Rect();
			fl1rect = new Rect();
			fl2rect = new Rect();
			fl0rect.set(fl0.getLeft(), fl0.getTop(), fl0.getRight(),
					fl0.getBottom());
			fl1rect.set(fl1.getLeft(), fl1.getTop(), fl1.getRight(),
					fl1.getBottom());
			fl2rect.set(fl2.getLeft(), fl2.getTop(), fl2.getRight(),
					fl2.getBottom());
			Log.d("BOUNDS",
					fl0rect.toString() + fl1rect.toString()
							+ fl2rect.toString());
		}
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {

		int virtualKey = KeyboardMapping.mapAndroidKeyToWindowsVirtual(keyCode);
		if (virtualKey != 0) {
			ControlDatas datas = new ControlDatas();
			KeyboardData kbData = new KeyboardData();
			kbData.setKeyDown(virtualKey);
			datas.add(kbData);
			WifiComm.getInstance().send(datas);
			return true;
		}
		return super.onKeyDown(keyCode, event);

	}

	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {

		int virtualKey = KeyboardMapping.mapAndroidKeyToWindowsVirtual(keyCode);
		if (virtualKey != 0) {
			ControlDatas datas = new ControlDatas();
			KeyboardData kbData = new KeyboardData();
			kbData.setKeyUp(virtualKey);//
			datas.add(kbData);
			WifiComm.getInstance().send(datas);
			return true;
		}
		return super.onKeyUp(keyCode, event);
	}

	@Override
	public void onCreateContextMenu(ContextMenu menu, View v,
			ContextMenuInfo menuInfo) {
		// TODO Auto-generated method stub
		super.onCreateContextMenu(menu, v, menuInfo);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.gontrol_main, menu);
		return super.onCreateOptionsMenu(menu);
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case R.id.keyboard:
			InputMethodManager imm = (InputMethodManager) this
					.getSystemService(INPUT_METHOD_SERVICE);
			imm.toggleSoftInput(0, InputMethodManager.HIDE_NOT_ALWAYS);
			Log.v("Softkeyboard", "open");
			return true;
		case R.id.confitem:
			gotoConf();
			return true;
		default:
			return super.onOptionsItemSelected(item);
		}
	}

	private void sendCDatas(ControlData cd) {
		ControlDatas cds = new ControlDatas();
		cds.add(cd);
		WifiComm.getInstance().send(cds);
	}

	private void sendCDatas(ControlDatas cds, ControlData cd, int qCtrl,
			boolean sCtrl) {
		if (qCtrl == 1) {
			cds.add(cd);
		} else if (qCtrl == -1) {
			cds.removeLast();
		} else if (qCtrl == 0) {

		}
		if (sCtrl) {
			WifiComm.getInstance().send(cds);
		}
	}

	private float getDistance(Point p, float x, float y) {
		float diffx = p.x - x;
		float diffy = p.y - y;
		return FloatMath.sqrt(diffx * diffx + diffy * diffy);
	}
	private void gotoConf(){
		Intent intent = new Intent();
		Bundle bundle4Conf = new Bundle();
		intent.setClass(GontrolMain.this, ConfCenter.class);
		bundle4Conf.putBoolean("et", enTap);       //et for enble tap
		bundle4Conf.putInt("ts", FIRST_TAP_TIME);  //ts for tap sensi...
		bundle4Conf.putInt("fs", FLING_TAP_TIME);  //fs for flap sensi...
		bundle4Conf.putInt("ws", FLING_SPEED);     //ws for wheel speed...
		
		intent.putExtras(bundle4Conf);
		
		//setResult(GontrolMain.RESULT_TO_CONF, intent);
		//GontrolMain.this.finish();
		startActivityForResult(intent, 0);
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		// TODO Auto-generated method stub
		switch(resultCode){
		case ConfCenter.RESULT_TO_PAD:
			Bundle bundle = data.getExtras();
			
			enTap = bundle.getBoolean("et");
			FIRST_TAP_TIME = bundle.getInt("ts");
			FLING_TAP_TIME = bundle.getInt("fs");
			FLING_SPEED = bundle.getInt("ws");
			return;
			default:
				break;
		}
		super.onActivityResult(requestCode, resultCode, data);
	}
	
	
}