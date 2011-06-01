package cs.tongji.mad.fproj;

import java.io.IOException;
import java.io.PrintWriter;

import org.xmlpull.v1.XmlSerializer;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.ImageView;

public class TouchButton extends ImageView implements OnTouchListener{
	private int buttonNumber;
	//public TouchPad tpref;

	public TouchButton(Context context) {
		super(context);
		//this.setOnTouchListener(this);
		
		// TODO Auto-generated constructor stub
	}

	public TouchButton(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		//this.setOnTouchListener(this);
		// TODO Auto-generated constructor stub
	}

	public TouchButton(Context context, AttributeSet attrs) {
		super(context, attrs);
		//this.setOnTouchListener(this);
		// TODO Auto-generated constructor stub
	}

	@Override
	public boolean onTouch(View arg0, MotionEvent arg1) {
		// TODO Auto-generated method stub
		switch (arg1.getAction() & MotionEvent.ACTION_MASK) {
		case MotionEvent.ACTION_DOWN:
			this.setColorFilter(0xff00ca00);
			//tpref.fingerCount = 1;
			Log.d("TOUCH", "BUTTON_DOWN");
			//return false;
			break;
//		case MotionEvent.ACTION_POINTER_DOWN:
//			Log.d("TOUCH", "POINTER_DOWN");
		case MotionEvent.ACTION_UP:
			this.setColorFilter(0xff000000);
			//tpref.fingerCount = 0;
			Log.d("TOUCH", "BUTTON_UP");
			break;
		default:
			break;
		}
		return true;
	}

	public void setButtonNumber(int buttonNumber) {
		this.buttonNumber = buttonNumber;
	}

	public int getButtonNumber() {
		return buttonNumber;
	}

	private void MakeXML(XmlSerializer serializer, PrintWriter output)
	throws IllegalArgumentException, IllegalStateException, IOException {
		
	}
}
