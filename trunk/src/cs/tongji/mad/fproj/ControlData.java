package cs.tongji.mad.fproj;

import java.io.IOException;
import java.io.PrintWriter;

import org.xmlpull.v1.XmlSerializer;

public interface ControlData {
	public void generateXML(XmlSerializer serializer, PrintWriter output)
			throws IllegalArgumentException, IllegalStateException, IOException;
}
// public class ControlData {
// public MouseData mMouse;
//	public KeyboardData mKeyboard;
//
//	public ControlData() {
//		super();
//		mMouse = new MouseData(-1, -1, false, -1,
//				-1, -1);
//		mKeyboard = new KeyboardData(-1, -1);
//	}
//
//	class MouseData {
//		private int mouseX;
//		private int mouseY;
//		private boolean absolute;
//		private int buttonDownNumber;
//		private int buttonUpNumber;
//		private int wheelData;
//
//		public MouseData(int mouseX, int mouseY, boolean absolute,
//				int buttonDownNumber, int buttonUpNumber, int wheelData) {
//			super();
//			this.mouseX = mouseX;
//			this.mouseY = mouseY;
//			this.absolute = absolute;
//			this.buttonDownNumber = buttonDownNumber;
//			this.buttonUpNumber = buttonUpNumber;
//			this.wheelData = wheelData;
//		}
//
//		public void setMouseX(int mouseX) {
//			this.mouseX = mouseX;
//		}
//
//		public int getMouseX() {
//			return mouseX;
//		}
//
//		public void setMouseY(int mouseY) {
//			this.mouseY = mouseY;
//		}
//
//		public int getMouseY() {
//			return mouseY;
//		}
//
//		public void setAbsolute(boolean absolute) {
//			this.absolute = absolute;
//		}
//
//		public boolean isAbsolute() {
//			return absolute;
//		}
//
//		public void setButtonDownNumber(int buttonDownNumber) {
//			this.buttonDownNumber = buttonDownNumber;
//		}
//
//		public int getButtonDownNumber() {
//			return buttonDownNumber;
//		}
//
//		public void setButtonUpNumber(int buttonUpNumber) {
//			this.buttonUpNumber = buttonUpNumber;
//		}
//
//		public int getButtonUpNumber() {
//			return buttonUpNumber;
//		}
//
//		public void setWheelData(int wheelData) {
//			this.wheelData = wheelData;
//		}
//
//		public int getWheelData() {
//			return wheelData;
//		}
//	}
//
//	class KeyboardData {
//		private int keyDownNumber;
//		private int keyUpNumber;
//
//		public KeyboardData(int keyDownNumber, int keyUpNumber) {
//			super();
//			this.keyDownNumber = keyDownNumber;
//			this.keyUpNumber = keyUpNumber;
//		}
//
//		public void setKeyDownNumber(int keyDownNumber) {
//			this.keyDownNumber = keyDownNumber;
//		}
//
//		public int getKeyDownNumber() {
//			return keyDownNumber;
//		}
//
//		public void setKeyUpNumber(int keyUpNumber) {
//			this.keyUpNumber = keyUpNumber;
//		}
//
//		public int getKeyUpNumber() {
//			return keyUpNumber;
//		}
//	}
//}
