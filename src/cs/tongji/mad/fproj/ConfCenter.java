package cs.tongji.mad.fproj;


import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.view.KeyEvent;
import android.widget.CheckBox;
import android.widget.SeekBar;
import android.widget.Toast;

public class ConfCenter extends Activity {
	public static final int RESULT_TO_PAD = 12580;

	private CheckBox cb;
	private SeekBar sb_tap;
	private SeekBar sb_flap;
	private SeekBar sb_wheel;

	private boolean bet;
	private int its;
	private int ifs;
	private int iws;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		this.setContentView(R.layout.configuration);
		cb = (CheckBox) findViewById(R.id.checkBox1);
		sb_tap = (SeekBar) findViewById(R.id.seekBar1);
		sb_wheel = (SeekBar) findViewById(R.id.seekBar2);
		sb_flap = (SeekBar) findViewById(R.id.seekBar3);

		final Bundle bundlegot = getIntent().getExtras();
		bet = bundlegot.getBoolean("et");
		its = bundlegot.getInt("ts");
		ifs = bundlegot.getInt("fs");
		iws = bundlegot.getInt("ws");
		
		cb.setChecked(bet);
        sb_tap.setProgress(its - 100);
        sb_flap.setProgress(ifs - 100);
        sb_wheel.setProgress(iws - 5);
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub
		if(KeyEvent.KEYCODE_BACK == keyCode && 0 == event.getRepeatCount()){
			
			//return true;
		}
		return super.onKeyDown(keyCode, event);
	}
	
    private void backtoPad(){
    	bet = cb.isChecked();
    	its = sb_tap.getProgress() + 100;
    	ifs = sb_flap.getProgress() + 100;
    	iws = sb_wheel.getProgress() + 5;
    	
    	Intent intent = new Intent();
    	intent.setClass(ConfCenter.this, GontrolMain.class);

		Bundle bundle4Pad = new Bundle();
		
		bundle4Pad.putBoolean("et", bet);       //et for enble tap
		bundle4Pad.putInt("ts", its);  //ts for tap sensi...
		bundle4Pad.putInt("fs", ifs);  //fs for flap sensi...
		bundle4Pad.putInt("ws", iws);     //ws for wheel speed...
		
		intent.putExtras(bundle4Pad);
		
		SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(this);
		SharedPreferences.Editor speditor = sp.edit();
		speditor.putBoolean("et", bet);
		speditor.putInt("ts", its);
		speditor.putInt("fs", ifs);
		speditor.putInt("ws", iws);
		if(!speditor.commit()){
			Toast.makeText(getApplicationContext(), R.string.confmsg2, Toast.LENGTH_SHORT).show();
		}else{
			Toast.makeText(TestContext.getInstance(), R.string.confmsg1, Toast.LENGTH_SHORT).show();
		}
		

		setResult(RESULT_TO_PAD, intent);

		ConfCenter.this.finish();
    }

	@Override
	public void onBackPressed() {
		// TODO Auto-generated method stub
		
		backtoPad();
		super.onBackPressed();
	}
    
    
    
    
}
