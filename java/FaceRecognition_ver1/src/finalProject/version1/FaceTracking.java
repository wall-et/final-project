package finalProject.version1;

import java.awt.Color;
import java.util.EnumSet;

import javax.swing.JFrame;

import intel.rssdk.PXCMCapture;
import intel.rssdk.PXCMFaceConfiguration;
import intel.rssdk.PXCMFaceData;
import intel.rssdk.PXCMFaceModule;
import intel.rssdk.PXCMImage;
import intel.rssdk.PXCMRectI32;
import intel.rssdk.PXCMSenseManager;
import intel.rssdk.PXCMSession;
import intel.rssdk.pxcmStatus;

public class FaceTracking {
	
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
         
         sts = senseMgr.EnableFace(null);        
         PXCMFaceModule faceModule = senseMgr.QueryFace();
         
         if (sts.isError() || faceModule == null)
         {
             System.out.println("Failed to initialize face module.");
             return;
         }
         
         // Retrieve the input requirements
         sts = pxcmStatus.PXCM_STATUS_DATA_UNAVAILABLE; 
         PXCMFaceConfiguration faceConfig = faceModule.CreateActiveConfiguration();
         faceConfig.SetTrackingMode(PXCMFaceConfiguration.TrackingModeType.FACE_MODE_COLOR_PLUS_DEPTH);        
         faceConfig.detection.isEnabled = true;        
         faceConfig.ApplyChanges();
         faceConfig.Update();
         
         sts = senseMgr.Init();
         System.out.println(sts);
         if (sts.isError())
         {
             System.out.println("Init failed: " + sts);
             return;
         }
         
         //mirroring the image to match reality
         senseMgr.QueryCaptureManager().QueryDevice().SetMirrorMode(PXCMCapture.Device.MirrorMode.MIRROR_MODE_HORIZONTAL);
         
         PXCMCapture.Device device = senseMgr.QueryCaptureManager().QueryDevice();
         PXCMCapture.Device.StreamProfileSet profiles = new PXCMCapture.Device.StreamProfileSet();
         device.QueryStreamProfileSet(profiles);
         
         depthWidth = profiles.depth.imageInfo.width;
         depthHeight = profiles.depth.imageInfo.height;
         
         PXCMFaceData faceData = faceModule.CreateOutput();
    	
         Listener listener = new Listener();
         
         CameraStream colorStream = new CameraStream(colorWidth, colorHeight, "Color Stream",listener,0);
         CameraStream depthStream = new CameraStream(depthWidth, depthHeight, "Depth Stream",listener,1);
         
         if (sts == pxcmStatus.PXCM_STATUS_NO_ERROR)
         {
        	 int framesCounter = 0;
             while (listener.exit == false)
             {
                 sts = senseMgr.AcquireFrame(true);
                 framesCounter++;
                 System.out.println("\nframe #"+framesCounter);
                 
                 if (sts == pxcmStatus.PXCM_STATUS_NO_ERROR)
                 {
                	 
                	 //FACE DETECTION LOGIC
                	 //PXCMCapture.Sample sample = senseMgr.QuerySample();
                	 PXCMCapture.Sample sample = senseMgr.QueryFaceSample();
                     
                	 faceData.Update();
                	 for (int fidx=0; ; fidx++) {
                		 PXCMFaceData.Face face = faceData.QueryFaceByIndex(fidx);
                		 if (face==null){
                			 //remove rect in case of no face found.
                			 if(fidx == 0){
                				 colorStream.content.setPoints(0,0,0,0);
                				 colorStream.content.repaint();
                				 depthStream.content.setPoints(0,0,0,0);
                				 depthStream.content.repaint();
                			 }
                			 System.out.println("found " + fidx + " faces in range");
                			 break;
                		 }

                		 PXCMFaceData.DetectionData detectData = face.QueryDetection(); 

                		 //detecting a face in thestream using the sdk rect
                		 if (detectData != null)
                		 {
                			 PXCMRectI32 rect = new PXCMRectI32();
                			 boolean ret = detectData.QueryBoundingRect(rect);
                			 if (ret) { 
                				 System.out.println ("Top Left corner: (" + rect.x + "," + rect.y + ")" ); 
                				 System.out.println ("Height: " + rect.h + " Width: " + rect.w);
                				 //technically only painting over one face at a time even though the program is finding as many as there are.
                				 //considering the program it makes sense.
                				 colorStream.content.setPoints(rect.x,rect.y,rect.w,rect.h);
                				 depthStream.content.setPoints(rect.x,rect.y,rect.w,rect.h);
                			 }
                		 } else {
                			 System.out.println("Error in detect data.");
                			 break;
                		 }
                	 }
                	 
                	 
                     {
                    	colorStream.updateStreamImage(sample);
 					}
 				
 	                if (sample.depth != null)
 					{       
 	                	depthStream.updateStreamImage(sample);
 	                }

                 }
                 else
                 {
                     System.out.println("Failed to acquire frame");
                 }
                 
                 senseMgr.ReleaseFrame();
             }
             
             faceData.close();
             senseMgr.Close();
             System.out.println("Done streaming");
         }
         else
         {
             System.out.println("Failed to initialize");
         }
         
         colorStream.window.dispose();
         depthStream.window.dispose();
    }
}

