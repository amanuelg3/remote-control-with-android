package cs.tongji.mad.fproj;

import android.app.Application;

public class TestContext extends Application {
	private static TestContext instance;

    public static TestContext getInstance() {
        return instance;
    }

    @Override
    public void onCreate() {
        // TODO Auto-generated method stub
        super.onCreate();
        instance = this;
    }
}
