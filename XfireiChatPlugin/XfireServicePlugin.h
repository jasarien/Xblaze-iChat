//
//  XfireServicePlugin.h
//  XfireiChatPlugin
//
//  Created by James Addyman on 09/07/2011.
//  Copyright 2011 Grapple Mobile. All rights reserved.
//

#import <IMServicePlugIn/IMServicePlugIn.h>
#import <IMServicePlugIn/IMServicePlugInInstantMessageSupport.h>
#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>

@class XfireSession;

@interface XfireServicePlugin : NSObject <IMServicePlugIn, IMServicePlugInInstantMessagingSupport, IMServicePlugInGroupListSupport, IMServicePlugInGroupListHandlePictureSupport> {
	
	id <IMServiceApplication, IMServiceApplicationInstantMessagingSupport, IMServiceApplicationGroupListSupport> _application;
	NSDictionary *_accountSettings;
	
	XfireSession *_xfSession;
}

@end
