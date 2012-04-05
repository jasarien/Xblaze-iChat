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

@interface XfireServicePlugin : NSObject <IMServicePlugIn, IMServicePlugInInstantMessagingSupport, IMServicePlugInGroupListSupport, IMServicePlugInPresenceSupport, IMServicePlugInGroupListHandlePictureSupport,
                                          IMServicePlugInGroupListAuthorizationSupport, IMServicePlugInGroupListEditingSupport> {
	
	id <IMServiceApplication, IMServiceApplicationInstantMessagingSupport, IMServiceApplicationGroupListSupport, IMServiceApplicationGroupListAuthorizationSupport> _application;
	NSDictionary *_accountSettings;
	
	XfireSession *_xfSession;
	
	NSString *_defaultIconHash;
}

@end
