package cs.tongji.mad.fproj;

import java.io.IOException;
import java.io.PrintWriter;

import org.xmlpull.v1.XmlSerializer;

public class KeyboardData implements ControlData {

	public static final int KEYDOWN = 1;
	public static final int KEYUP = 2;
	
	private int type;
	private int virtualKey;
	
	public void setKeyDown(int virtualKey) {
		this.virtualKey = virtualKey;
		this.type = KEYDOWN;
	}
	
	public void setKeyUp(int virtualKey) {
		this.virtualKey = virtualKey;
		this.type = KEYUP;		
	}
	
	@Override
	public void generateXML(XmlSerializer serializer, PrintWriter output)
			throws IllegalArgumentException, IllegalStateException, IOException {
		
		serializer.startTag(ControlDatas.NAMESPACE, "KeyboardData");

		switch (type) {
		case KEYDOWN:
			serializer.startTag(ControlDatas.NAMESPACE, "KeyDown");
			serializer.attribute(ControlDatas.NAMESPACE, "VirtualKey", 
					Integer.toString(virtualKey));
			serializer.endTag(ControlDatas.NAMESPACE, "KeyDown");
			break;
			
		case KEYUP:
			serializer.startTag(ControlDatas.NAMESPACE, "KeyUp");
			serializer.attribute(ControlDatas.NAMESPACE, "VirtualKey", 
					Integer.toString(virtualKey));
			serializer.endTag(ControlDatas.NAMESPACE, "KeyUp");
			break;
			
		}	
		
		serializer.endTag(ControlDatas.NAMESPACE, "KeyboardData");

	}
}
