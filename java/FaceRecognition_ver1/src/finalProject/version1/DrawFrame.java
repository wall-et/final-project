package finalProject.version1;

import java.awt.*;
import javax.swing.*;
import intel.rssdk.*;
import org.opencv.core.*;

import java.awt.Component;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.image.BufferedImage;

class DrawFrame extends Component { 
    public BufferedImage image;
    int x = 0;
    int y = 0;
    int h = 0;
    int w = 0;

    public DrawFrame(int width, int height) { 
       image=new BufferedImage(width,height,BufferedImage.TYPE_INT_RGB);
    }
    
    public DrawFrame(int width, int height, int x,int y, int w, int h) { 
		this.w = w;
        this.h = h;
        this.x = x;
        this.y = y;
        image=new BufferedImage(width,height,BufferedImage.TYPE_INT_RGB);
	}

    public void paint(Graphics g) {
        ((Graphics2D)g).drawImage(image,0,0,null);
        g.setColor(Color.RED);
        ((Graphics2D)g).drawRect(x, y, w, h);
    }
    
    public void setPoints(int x,int y, int w, int h){
		this.w = w;
        this.h = h;
        this.x = x;
        this.y = y;
	}
}

class Listener extends WindowAdapter {
    public boolean exit = false;
	@Override public void windowClosing(WindowEvent e) {
		exit=true;
	}
}

