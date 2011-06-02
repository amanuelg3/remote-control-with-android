package cs.tongji.mad.fproj;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;

import org.xmlpull.v1.XmlSerializer;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.util.Log;
import android.util.Xml;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.TextView;

public class TouchPad extends SurfaceView implements SurfaceHolder.Callback,
		OnTouchListener {

	public PaintThread pt;

	// replaced by ControlData
	public Point mousePos;
	public Point lastPos;
	public ControlData mCD;

	public Socket mDroidEnd;

	//public TextView tv;
	
	public int fingerCount;           //how many fingers on the pad?

	private Communicator communicator;
	// private File tempXML;no real file needed

	public TouchPad(Context context) {
		super(context);
		getHolder().addCallback(this);
		pt = new PaintThread(getHolder(), this);
		mousePos = new Point(-20,-20);
		lastPos = new Point();
		fingerCount = 0;
		communicator = WifiComm.getInstance();
		// TODO Auto-generated constructor stub
	}

	public TouchPad(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		getHolder().addCallback(this);
		pt = new PaintThread(getHolder(), this);
		mousePos = new Point(-20,-20);
		lastPos = new Point();
		fingerCount = 0;
		communicator = WifiComm.getInstance();
		// TODO Auto-generated constructor stub
	}

	public TouchPad(Context context, AttributeSet attrs) {
		super(context, attrs);
		getHolder().addCallback(this);
		pt = new PaintThread(getHolder(), this);
		mousePos = new Point(-20,-20);
		lastPos = new Point();
		fingerCount = 0;
		communicator = WifiComm.getInstance();
		// TODO Auto-generated constructor stub
	}

	@Override
	public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2, int arg3) {
		// TODO Auto-generated method stub

	}

	@Override
	public void surfaceCreated(SurfaceHolder arg0) {
		// TODO Auto-generated method stub
		//pt.start();
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder arg0) {
		// TODO Auto-generated method stub

	}

	@Override
	protected void onDraw(Canvas canvas) {
		// TODO Auto-generated method stub
		super.onDraw(canvas);
		// Paint p = new Paint();
		// p.setColor(Color.BLUE);
		// // do painting
		// canvas.drawPaint(p);
		// p.setColor(Color.GREEN);
		// canvas.drawOval(new RectF(mousePos.x - 10, mousePos.y - 10,
		// mousePos.x + 10, mousePos.y + 10), p);
	}

	@Override
	public boolean onTouch(View arg0, MotionEvent arg1) {
		// TODO Auto-generated method stub
		switch (arg1.getAction() & MotionEvent.ACTION_MASK) {
		case MotionEvent.ACTION_DOWN:
			lastPos.x = (int) arg1.getX(fingerCount);
			lastPos.y = (int) arg1.getY(fingerCount);
			Log.d("TOUCH", "ACTION_DOWN");
			break;
		case MotionEvent.ACTION_MOVE:
			// 画点
			mousePos.x = (int) arg1.getX(fingerCount);
			mousePos.y = (int) arg1.getY(fingerCount);
			//tv.setText("x: " + mousePos.x + "____y: " + mousePos.y);
			int diffX = mousePos.x - lastPos.x;
			int diffY = mousePos.y - lastPos.y;
			ControlDatas datas = new ControlDatas();
			MouseData moustData = new MouseData();
			moustData.setMove(diffX, diffY);
			datas.add(moustData);
			communicator.send(datas);
			//datas = null;
			//***************************mCD.mMouse.setMouseX(diffX);
			//***************************mCD.mMouse.setMouseY(diffY);
			// first make an xml
			//SendXML();
			// do transporting here
			lastPos.x =  mousePos.x;
			lastPos.y = mousePos.y;
			Log.d("TOUCH", "ACTION_MOVE " + diffX + "," + diffY +
					"//" + mousePos.x + "," + mousePos.y);

			break;
		case MotionEvent.ACTION_UP:
			mousePos.x = -20;
			mousePos.y = -20;
			Log.d("TOUCH", "ACTION_UP");
			break;
		default:
			break;
		}
		return true;
	}


	// generate a XML file for use
	private void MakeXML(XmlSerializer serializer, PrintWriter output)
			throws IllegalArgumentException, IllegalStateException, IOException {
//		serializer.setOutput(output);
//		serializer.startDocument("UTF-8", true);
//		
//		serializer.startTag("http://android.sse.tongji.edu.cn/ControlData", "Controls");
//		
//		serializer.startTag(null, "MouseData");
//		serializer.startTag(null, "Move");
//		serializer.attribute(null, "MouseX",
//				String.valueOf(mCD.mMouse.getMouseX()));
//		serializer.attribute(null, "MouseY",
//				String.valueOf(mCD.mMouse.getMouseY()));
//		serializer.attribute(null, "Absolute",
//				String.valueOf(mCD.mMouse.getMouseX()));
//		serializer.endTag(null, "Move");
//		
//		serializer.startTag(null, "ButtonUp");
//		serializer.attribute(null, "ButtonNumber",
//				String.valueOf(mCD.mMouse.getButtonUpNumber()));
//		serializer.endTag(null, "ButtonUp");
//		
//		serializer.startTag(null, "ButtonDown");
//		serializer.attribute(null, "ButtonNumber",
//				String.valueOf(mCD.mMouse.getButtonUpNumber()));
//		serializer.endTag(null, "ButtonDown");
//		
//		serializer.startTag(null, "Wheel");
//		serializer.attribute(null, "WheelDelta",
//				String.valueOf(mCD.mMouse.getWheelData()));
//		serializer.startTag(null, "Wheel");
//		serializer.endTag(null, "MouseData");
//		
//		serializer.startTag(null, "KeyboardData");
//		
//		serializer.startTag(null, "KeyUp");
//		serializer.attribute(null, "KeyDownNumber",
//				String.valueOf(mCD.mKeyboard.getKeyDownNumber()));
//		serializer.startTag(null, "KeyUp");
//		serializer.endTag(null, "KeyDown");
//		serializer.attribute(null, "KeyUpNumber",
//				String.valueOf(mCD.mKeyboard.getKeyUpNumber()));
//		serializer.endTag(null, "KeyDown");
//		serializer.endTag(null, "KeyboardData");
//		
//		serializer.endTag(null, "ControlData");
//		serializer.endDocument();
	}

	// send this XML file via socket
	private void SendXML() {
		try {
			//File f = new File("GontrolTemp.xml");
			PrintWriter output = new PrintWriter(mDroidEnd.getOutputStream());
			//PrintWriter output = new PrintWriter(new FileWriter("/sdcard/GontrolTemp.xml"));
			XmlSerializer serializer = Xml.newSerializer();
			MakeXML(serializer, output);
			serializer.flush();
			//output.flush();
			output.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			Log.e("NETWORKING", "WRITER");
		}
	}

	class PaintThread extends Thread {

		private SurfaceHolder sh;
		private TouchPad tp;
		
		public boolean tFlag;

		public PaintThread(SurfaceHolder sh, TouchPad tp) {
			super();
			this.sh = sh;
			this.tp = tp;
			this.tFlag = true;
		}

		@Override
		public void run() {
			Canvas c = null;
			// TODO Auto-generated method stub
			while (tFlag) {
				// super.run();
				//Log.v("Draw?", "Always?");
				try {
					c = sh.lockCanvas();
					Paint p = new Paint();
					p.setColor(0x55ffffff & Color.BLACK);
					// do painting
					c.drawPaint(p);

					p.setColor(0x22ffffff & Color.GREEN);
					c.drawOval(new RectF(mousePos.x - 25, mousePos.y - 25,
							mousePos.x + 25, mousePos.y + 25), p);
					p.setColor(Color.GREEN);
					c.drawOval(new RectF(mousePos.x - 6, mousePos.y - 6,
							mousePos.x + 6, mousePos.y + 6), p);
					// tp.onDraw(c);
				} catch (Exception e) {

				} finally {
					if (c != null) {
						sh.unlockCanvasAndPost(c);
					}
				}
				// try {
				// sleep(2000);
				// } catch (InterruptedException e) {
				// // TODO Auto-generated catch block
				// e.printStackTrace();
				// }
			}

		}

	}
}
