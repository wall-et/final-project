package finalProject.version1;

import java.util.EnumSet;

import javax.swing.JFrame;

import intel.rssdk.PXCMCapture;
import intel.rssdk.PXCMImage;
import intel.rssdk.PXCMSenseManager;
import intel.rssdk.PXCMSession;
import intel.rssdk.pxcmStatus;

/*
 * The following class is a simple class to initialize the realsense manager
 * and to display two simple windows with the depth and color streams.
 * */
public class SimpleCameraStream {
	
	static int colorWidth  = 640;
    static int colorHeight = 480;
    static int depthWidth, depthHeight;
    //private PXCMSenseManager senseManager;
    
    //function with boolean return value to check if there is a 3d realSense device connected.
    private static boolean PrintConnectedDevices()
    {
        PXCMSession session = PXCMSession.CreateInstance();
        PXCMSession.ImplDesc desc = new PXCMSession.ImplDesc();        
        PXCMSession.ImplDesc outDesc = new PXCMSession.ImplDesc();        
        desc.group = EnumSet.of(PXCMSession.ImplGroup.IMPL_GROUP_SENSOR);
        desc.subgroup = EnumSet.of(PXCMSession.ImplSubgroup.IMPL_SUBGROUP_VIDEO_CAPTURE);
                
        int numDevices = 0;
        for (int i = 0; ;i++)
        {
            if (session.QueryImpl(desc, i, outDesc).isError())
                break;
            
            PXCMCapture capture = new PXCMCapture();
            if (session.CreateImpl(outDesc, capture).isError())
                continue;
            
            for (int j = 0; ;j++)
            {
                PXCMCapture.DeviceInfo info = new PXCMCapture.DeviceInfo();
                if (capture.QueryDeviceInfo(j, info).isError())
                    break;
                
                System.out.println(info.name);
                numDevices++;
            }
        }
        
        System.out.println("Found " + numDevices + " devices");
        
        if(numDevices == 0){
        	return false;
        }else{
        	return true;
        }
    }
    
    public static void main(String s[])
    {
    	//check to find connected device
    	boolean isConnected = PrintConnectedDevices();
    	if(!isConnected){
    		System.out.println("No RealSense Device Found.\nProgram Terminated.");
    		System.exit(1);
    	}
    	
    	 PXCMSenseManager senseMgr = PXCMSenseManager.CreateInstance();        
         
         pxcmStatus sts = senseMgr.EnableStream(PXCMCapture.StreamType.STREAM_TYPE_COLOR, colorWidth, colorHeight);
         sts = senseMgr.EnableStream(PXCMCapture.StreamType.STREAM_TYPE_DEPTH);
         //infra red stream currently disabled.
         //senseMgr.EnableStream(PXCMCapture.StreamType.STREAM_TYPE_IR, colorWidth, colorHeight);
         
         sts = senseMgr.Init();
         System.out.println(sts);
         
         //mirroring the image to match reality
         senseMgr.QueryCaptureManager().QueryDevice().SetMirrorMode(PXCMCapture.Device.MirrorMode.MIRROR_MODE_HORIZONTAL);
         
         PXCMCapture.Device device = senseMgr.QueryCaptureManager().QueryDevice();
         PXCMCapture.Device.StreamProfileSet profiles = new PXCMCapture.Device.StreamProfileSet();
         device.QueryStreamProfileSet(profiles);
         
         depthWidth = profiles.depth.imageInfo.width;
         depthHeight = profiles.depth.imageInfo.height;
    	
         Listener listener = new Listener();
         /*
         //creating color stream window
         SimpleCameraStream c_raw = new SimpleCameraStream(); 
         DrawFrame c_df = new DrawFrame(colorWidth, colorHeight);
         JFrame cframe= new JFrame("Color Stream");	
         cframe.addWindowListener(listener);
         cframe.setSize(colorWidth, colorHeight); 
         cframe.add(c_df);
         cframe.setVisible(true);
         */
         
         /*
         //creating depth stream window
         SimpleCameraStream d_raw = new SimpleCameraStream(); 
         DrawFrame d_df=new DrawFrame(depthWidth, depthHeight);      
         JFrame dframe= new JFrame("Depth Stream"); 
         dframe.addWindowListener(listener);
         dframe.setSize(depthWidth, depthHeight); 
         dframe.add(d_df);
         dframe.setVisible(true);
         */
         
         CameraStream colorStream = new CameraStream(colorWidth, colorHeight, "Color Stream",listener);
         CameraStream depthStream = new CameraStream(depthWidth, depthHeight, "Depth Stream",listener);
         
         if (sts == pxcmStatus.PXCM_STATUS_NO_ERROR)
         {
             while (listener.exit == false)
             {
            	 //we are acquiring the data unaligned. each stream to itself.
                 sts = senseMgr.AcquireFrame(false);
                 
                 if (sts == pxcmStatus.PXCM_STATUS_NO_ERROR)
                 {
                  	PXCMCapture.Sample sample = senseMgr.QuerySample();
                     
                     if (sample.color != null)
                     {
     	                PXCMImage.ImageData cData = new PXCMImage.ImageData();                
         	            sts = sample.color.AcquireAccess(PXCMImage.Access.ACCESS_READ,PXCMImage.PixelFormat.PIXEL_FORMAT_RGB32, cData);
             	        if (sts.compareTo(pxcmStatus.PXCM_STATUS_NO_ERROR) < 0)
 						{
                 	        System.out.println ("Failed to AcquireAccess of color image data");
                     	    System.exit(3);
 	                    }
                      
 	                    int cBuff[] = new int[cData.pitches[0]/4 * colorHeight];
                         
 		                cData.ToIntArray(0, cBuff);
 	    	            //c_df.image.setRGB (0, 0, colorWidth, colorHeight, cBuff, 0, cData.pitches[0]/4);
 	        	        //c_df.repaint();  
 	           	        sts = sample.color.ReleaseAccess(cData);
 	           	        colorStream.content.image.setRGB (0, 0, colorWidth, colorHeight, cBuff, 0, cData.pitches[0]/4);
 	           	        colorStream.content.repaint();
 						
 	              	    if (sts.compareTo(pxcmStatus.PXCM_STATUS_NO_ERROR)<0)
 						{
 	                    	    System.out.println ("Failed to ReleaseAccess of color image data");
 	                        	System.exit(3);
 	                    }
 					}
 				
 	                if (sample.depth != null)
 					{       
 	                    PXCMImage.ImageData dData = new PXCMImage.ImageData();
 	                    sample.depth.AcquireAccess(PXCMImage.Access.ACCESS_READ,PXCMImage.PixelFormat.PIXEL_FORMAT_RGB32, dData);
 	                    if (sts.compareTo(pxcmStatus.PXCM_STATUS_NO_ERROR)<0)
 						{
 	                        System.out.println ("Failed to AcquireAccess of depth image data");
 	                        System.exit(3);
 	                    }

 	                    int dBuff[] = new int[dData.pitches[0]/4 * depthHeight];
 	                    dData.ToIntArray(0, dBuff);
 	                    //d_df.image.setRGB (0, 0, depthWidth, depthHeight, dBuff, 0, dData.pitches[0]/4);
 	                    //d_df.repaint();
 	                    depthStream.content.image.setRGB (0, 0, depthWidth, depthHeight, dBuff, 0, dData.pitches[0]/4);
	                    depthStream.content.repaint();
 	                    sts = sample.depth.ReleaseAccess(dData);
 	                    if (sts.compareTo(pxcmStatus.PXCM_STATUS_NO_ERROR)<0)
 	                    {
 	                        System.out.println ("Failed to ReleaseAccess of depth image data");
 	                        System.exit(3);
 	                    }
 	                }  
                 }
                 else
                 {
                     System.out.println("Failed to acquire frame");
                 }
                 
                 senseMgr.ReleaseFrame();
             }
             
             senseMgr.Close();
             System.out.println("Done streaming");
         }
         else
         {
             System.out.println("Failed to initialize");
         }
         
         //cframe.dispose();
         //dframe.dispose();
         colorStream.window.dispose();
         depthStream.window.disable();
    }
}
