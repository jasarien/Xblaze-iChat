//
//  XfireServicePlugin.m
//  XfireiChatPlugin
//
//  Created by James Addyman on 09/07/2011.
//  Copyright 2011 Grapple Mobile. All rights reserved.
//

#import "XfireServicePlugin.h"
#import "MFGameMonitor.h"
#import "MFGameRegistry.h"
#import "XfireFriend_MacFireAdditions.h"
#import "XfireSession_Private.h"
#import "XfireFriendGroup_Private.h"
#import "XfireFriendGroupController.h"

@implementation XfireServicePlugin

- (id)initWithServiceApplication:(id <IMServiceApplication, IMServiceApplicationInstantMessagingSupport, IMServiceApplicationGroupListSupport>)IMClientInterface
{
	if (self = [super init])
	{
		_application = IMClientInterface;
		
		_xfSession = [XfireSession newSessionWithHost:@"cs.xfire.com" port:27999];
		[_xfSession setPosingClientVersion:9999];
		[_xfSession setDelegate:self];
	}
	
	return self;
}

- (oneway void)login
{
	NSLog(@"Logging in");
	[_xfSession connect];
}

- (oneway void)logout
{
	NSLog(@"Logging out");
	[_xfSession disconnect];
}

- (oneway void)updateAccountSettings:(NSDictionary *)accountSettings
{
	NSLog(@"Update account settings: %@", accountSettings);
	_accountSettings = [accountSettings retain];
}

- (oneway void)userDidStartTypingToHandle:(NSString *)handle 
{
	XfireFriend *friend = [_xfSession friendForUserName:handle];
	XfireChat *chat = [_xfSession chatForSessionID:[friend sessionID]];
	[chat sendTypingNotification];
}
- (oneway void) userDidStopTypingToHandle:(NSString *)handle
{
}

- (oneway void)sendMessage:(IMServicePlugInMessage *)message toHandle:(NSString *)handle
{
	NSLog(@"send message");
	XfireFriend *friend = [_xfSession friendForUserName:handle];
	XfireChat *chat = [_xfSession chatForSessionID:[friend sessionID]];
	if (!chat)
	{
		chat = [_xfSession beginChatWithFriend:friend];
	}
	[chat sendMessage:[[message content] string]];
	[_application plugInDidSendMessage:message toHandle:handle error:nil];
}

- (oneway void) updateSessionProperties:(NSDictionary *)properties
{
	NSString *awayMessage = [properties objectForKey:IMSessionPropertyStatusMessage];
	[_xfSession setStatusString:awayMessage];
}

- (oneway void) requestGroupList
{
	NSMutableArray *ichatGroups = [NSMutableArray array];
	NSArray *xfireGroups = [[_xfSession friendGroupController] groups];
	NSNumber *permissions = [NSNumber numberWithInt:IMGroupListCanReorderGroup];
	
	for (XfireFriendGroup *group in xfireGroups)
	{
		if ([group members] == 0)
			continue;
			
		NSMutableArray *handles = [NSMutableArray array];
		
		for (XfireFriend *friend in [group members])
		{
			[handles addObject:[friend userName]];
		}
		
		NSString *groupName = [group groupName];
		if ([group groupID] == kXfireFriendGroupOnlineID || [group groupID] == kXfireFriendGroupOfflineID)
		{
			groupName = IMGroupListDefaultGroup;
		}
		
		NSDictionary *ichatGroup = [[NSDictionary alloc] initWithObjectsAndKeys: 
									groupName, IMGroupListNameKey,
									handles, IMGroupListHandlesKey, 
									permissions, IMGroupListPermissionsKey,
									nil];
		[ichatGroups addObject:ichatGroup];
	}
	
	[_application plugInDidUpdateGroupList:ichatGroups error:nil];
}

#pragma mark XfireSessionDelegate

- (void)xfireGetSession:(XfireSession *)session userName:(NSString **)aName password:(NSString **)aPassword
{
	*aName = [[_accountSettings objectForKey:IMAccountSettingLoginHandle] copy];
	*aPassword = [[_accountSettings objectForKey:IMAccountSettingPassword] copy];
}

- (void)xfireSessionLoginFailed:(XfireSession *)session reason:(NSString *)reason
{
	NSLog(@"Xblaze session login failed %@", reason);
	
	if( [reason isEqualToString:kXfireVersionTooOldReason] )
	{
		[[NSAlert alertWithMessageText:@"Version Too Old"
						defaultButton:@"OK"
					  alternateButton:nil
						  otherButton:nil
			informativeTextWithFormat:@"Your Xfire version is too old."] runModal];
	}
	else if( [reason isEqualToString:kXfireInvalidPasswordReason] )
	{
		[[NSAlert alertWithMessageText:@"Wrong Username or Password"
						 defaultButton:@"OK"
					   alternateButton:nil
						   otherButton:nil
			 informativeTextWithFormat:@"Unable to log in with the supplied username or password.\nPlease check them and try again."] runModal];
	}
	else if( [reason isEqualToString:kXfireNetworkErrorReason] )
	{
		[[NSAlert alertWithMessageText:@"Unable to Connect"
						 defaultButton:@"OK"
					   alternateButton:nil
						   otherButton:nil
			 informativeTextWithFormat:@"There was an error connecting to Xfire.\nPlease check your network connection and try again.\nThis may happen is Xfire's servers are experiencing difficulties."] runModal];
	}	
	
	[_application plugInDidLogOutWithError:nil reconnect:NO];
}

- (XfireSkin *)xfireSessionSkin:(XfireSession *)session
{
	return [XfireSkin theSkin];
}

- (void)xfireSession:(XfireSession *)session didChangeStatus:(XfireSessionStatus)newStatus
{
	if (newStatus == kXfireSessionStatusOnline)
	{
		NSArray *runningGames = [[MFGameMonitor sharedMonitor] runningGames];
		if ([runningGames count] > 1)
		{
			[_xfSession enterGame:[[[runningGames lastObject] objectForKey:kMFGameRegistryIDKey] unsignedIntValue]];
		}
		
		[_application plugInDidLogIn];
		//[self updateClanLists];
	}
	else if (newStatus == kXfireSessionStatusOffline)
	{
		[_application plugInDidLogOutWithError:nil reconnect:NO];
	}
}

//- (void)updateClanLists
//{
//	NSArray *friends = [_xfSession friends];
//	for (XfireFriend *friend in friends)
//	{
//		if ([friend isClanMember])
//		{
//			AIListContact *contact = [[adium contactController] existingContactWithService:[self service] account:self UID:[friend userName]];
//			if (!contact)
//			{
//				contact = [[adium contactController] contactWithService:[self service] account:self UID:[friend userName]];
//			}
//			
//			[contact setOnline:[friend isOnline] notify:NotifyLater silently:YES];
//			
//			NSString *nickName = [friend displayName];
//			
//			[contact setServersideAlias:nickName silently:YES];
//			[AIUserIcons flushAllCaches];
//			[AIUserIcons setServersideIconData:[[friend displayImage] TIFFRepresentation] forObject:contact notify:NotifyLater];
//			[contact setStatusMessage:[[[NSAttributedString alloc] initWithString:[friend statusDisplayString]] autorelease] notify:NotifyLater];
//			NSRange afkRange = [[friend statusDisplayString] rangeOfString:@"(AFK) Away From Keyboard"];
//			if (afkRange.location == NSNotFound)
//			{
//				[contact setStatusWithName:nil statusType:AIAvailableStatusType notify:NotifyLater];
//			}
//			else
//			{
//				[contact setStatusWithName:nil statusType:AIAwayStatusType notify:NotifyLater];
//			}
//			
//			NSMutableSet *clans = [NSMutableSet set];
//			for (NSNumber *clanID in [friend clanIDs])
//			{
//				[clans addObject:[[[[_xfSession friendGroupController] clans] groupForID:[clanID intValue]] groupName]];
//			}
//			
//			if ([friend isDirectFriend])
//			{
//				[clans addObject:@"Xfire"];
//			}
//			
//			[contact setRemoteGroupNames:clans];
//			[contact notifyOfChangedPropertiesSilently:YES];
//		}
//	}
//}

- (void)xfireSession:(XfireSession *)session didBeginChat:(XfireChat *)chat
{
	[chat setDelegate:self];
}

- (void)xfireSession:(XfireSession *)session chat:(XfireChat *)aChat didReceiveMessage:(NSString *)msg
{
	XfireFriend *friend = [aChat remoteFriend];
	
	NSAttributedString *attrString = [[NSAttributedString alloc] initWithString:msg];
    IMServicePlugInMessage *message = [IMServicePlugInMessage servicePlugInMessageWithContent:attrString];
    [attrString release];
	
	[_application plugInDidReceiveMessage:message
							   fromHandle:[friend userName]];
}

- (void)xfireSession:(XfireSession *)session chat:(XfireChat *)aChat didReceiveTypingNotification:(BOOL)isTyping
{
	XfireFriend *friend = [aChat remoteFriend];
	if (isTyping)
		[_application handleDidStartTyping:[friend userName]];
	else
		[_application handleDidStopTyping:[friend userName]];
}

- (void)xfireSession:(XfireSession *)session friendDidChange:(XfireFriend *)fr attribute:(XfireFriendChangeAttribute)attr
{
	[self requestGroupList];
	
	NSMutableDictionary *properties = [NSMutableDictionary dictionary];
	
	if ([fr isOnline])
	{
		if ([[fr statusString] rangeOfString:@"(afk) away from keyboard" options:NSCaseInsensitiveSearch].location != NSNotFound)
		{
			[properties setObject:[NSNumber numberWithInt:IMHandleAvailabilityAway] forKey:IMHandlePropertyAvailability];
		}
		else
		{
			[properties setObject:[NSNumber numberWithInt:IMHandleAvailabilityAvailable] forKey:IMHandlePropertyAvailability];
		}
	}
	else
	{
		[properties setObject:[NSNumber numberWithInt:IMHandleAvailabilityOffline] forKey:IMHandlePropertyAvailability];
	}
	
	[properties setObject:[fr statusString] forKey:IMHandlePropertyStatusMessage];
	
	[_application plugInDidUpdateProperties:properties ofHandle:[fr userName]];
}

//- (void)xfireSession:(XfireSession *)session searchResults:(NSArray *)friends
//{
//	if( searchWindowController && [[searchWindowController window] isVisible] )
//	{
//		[searchWindowController handleSearchResults:friends];
//	}
//}

//- (void)xfireSession:(XfireSession *)session didReceiveFriendshipRequests:(NSArray *)requestors
//{
//	for (XfireFriend *friendToBe in requestors)
//	{
//		[[adium interfaceController] displayQuestion:[NSString stringWithFormat:@"%@ wants to be friends!", [friendToBe userName]]
//									 withDescription:[NSString stringWithFormat:@"%@ says \"%@\".", [friendToBe userName], [friendToBe statusString]]
//									 withWindowTitle:@"Friend Request"
//									   defaultButton:@"Accept"
//									 alternateButton:@"Decline"
//										 otherButton:@"Defer"
//										 suppression:nil
//											  target:self
//											selector:@selector(handleFriendRequest:userInfo:suppression:)
//											userInfo:friendToBe];
//	}
//}

//- (void)handleFriendRequest:(NSNumber *)returnCode userInfo:(id)info suppression:(NSNumber *)suppressed
//{
//	AITextAndButtonsReturnCode result = [returnCode integerValue];
//	XfireFriend *friend = info;
//	
//	switch (result)
//	{
//		case AITextAndButtonsDefaultReturn:
//			[_xfSession acceptFriendRequest:friend];
//			break;
//		case AITextAndButtonsAlternateReturn:
//			[_xfSession declineFriendRequest:friend];
//		default:
//			break;
//	}
//}

@end
