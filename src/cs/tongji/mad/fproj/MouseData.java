package cs.tongji.mad.fproj;

import java.io.IOException;
import java.io.PrintWriter;

import org.xmlpull.v1.XmlSerializer;

public class MouseData implements ControlData {

	public static final int MOVE = 1;
	public static final int BUTTONDOWN = 2;
	public static final int BUTTONUP = 3;
	public static final int WHEEL = 4;
	
	private int type;
	private int mouseX;
	private int mouseY;
	private boolean absolute;
	private int buttonNumber;
	private int wheelDelta;
	
	public MouseData() {
		setMove(0,0);
	}
	
	public void setMove(int mouseX, int mouseY, boolean absolute) {
		this.mouseX = mouseX;
		this.mouseY = mouseY;
		this.absolute = absolute;
		this.type = MOVE;
	}
	
	public void setMove(int mouseX, int mouseY) {
		setMove(mouseX, mouseY, false);
	}
	
	public void setButtonDown(int buttonNumber) {
		this.buttonNumber = buttonNumber;
		this.type = BUTTONDOWN;
	}

	public void setButtonUp(int buttonNumber) {
		this.buttonNumber = buttonNumber;
		this.type = BUTTONUP;
	}	
	
	public void setWheel(int wheelDelta) {
		this.wheelDelta = wheelDelta;
		this.type = WHEEL;
	}
	
	@Override
	public void generateXML(XmlSerializer serializer, PrintWriter output)
			throws IllegalArgumentException, IllegalStateException, IOException {
		
		serializer.startTag(ControlDatas.NAMESPACE, "MouseData");
		
		switch (type) {
		
		case MOVE:
			serializer.startTag(ControlDatas.NAMESPACE, "Move");
			serializer.attribute(ControlDatas.NAMESPACE, "MouseX", 
					Integer.toString(mouseX));
			serializer.attribute(ControlDatas.NAMESPACE, "MouseY", 
					Integer.toString(mouseY));
			if (absolute) {
				serializer.attribute(ControlDatas.NAMESPACE, "Absolute" , "true");				
			}
			serializer.endTag(ControlDatas.NAMESPACE, "Move");
			break;
			
		case BUTTONDOWN:
			serializer.startTag(ControlDatas.NAMESPACE, "ButtonDown");
			serializer.attribute(ControlDatas.NAMESPACE, "ButtonNumber", 
					Integer.toString(buttonNumber));
			serializer.endTag(ControlDatas.NAMESPACE, "ButtonDown");			
			break;
			
		case BUTTONUP:
			serializer.startTag(ControlDatas.NAMESPACE, "ButtonUp");
			serializer.attribute(ControlDatas.NAMESPACE, "ButtonNumber", 
					Integer.toString(buttonNumber));
			serializer.endTag(ControlDatas.NAMESPACE, "ButtonUp");			
			break;

		case WHEEL:
			serializer.startTag(ControlDatas.NAMESPACE, "Wheel");
			serializer.attribute(ControlDatas.NAMESPACE, "WheelDelta", 
					Integer.toString(wheelDelta));
			serializer.endTag(ControlDatas.NAMESPACE, "Wheel");			
			break;
		}

		serializer.endTag(ControlDatas.NAMESPACE, "MouseData");

	}

}
