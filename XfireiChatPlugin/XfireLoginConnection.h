/*******************************************************************
	FILE:		XfireLoginConnection.h
	
	COPYRIGHT:
		Copyright 2007-2009, the MacFire.org team.
		Use of this software is governed by the license terms
		indicated in the License.txt file (a BSD license).
	
	DESCRIPTION:
		Represents the log-in connection (the connection to the Xfire
		master server cs.xfire.com).  Handles sending/receiving
		packets.
	
	HISTORY:
		2008 04 06  Changed copyright to BSD license.
		2008 02 10  Eliminated secondary reader thread.
		2008 01 12  Revised to use new XfireConnection structure, and
		            added game status method.
		2007 11 18  Created.
*******************************************************************/

#import <Foundation/Foundation.h>
#import "XfireConnection.h"
#import "XfireSession.h"

@class XfireFriend;

// Represents the log-in connection for a given session
@interface XfireLoginConnection : XfireConnection
{
	NSMutableData		*_availableData;
	NSTimer				*_keepAliveResponseTimer;
}

// Specific packet processors
- (void)processLoginPacket:(XfirePacket *)pkt;
- (void)processLoginSuccessPacket:(XfirePacket *)pkt;
- (void)processVersionTooOldPacket:(XfirePacket *)pkt;
- (void)processFriendsListPacket:(XfirePacket *)pkt;
- (void)processSessionIDPacket:(XfirePacket *)pkt;
- (void)processFriendStatusPacket:(XfirePacket *)pkt;
- (void)processGameStatusPacket:(XfirePacket *)pkt;
- (void)processFriendOfFriendPacket:(XfirePacket *)pkt;
- (void)processNicknameChangePacket:(XfirePacket *)pkt;
- (void)processSearchResultsPacket:(XfirePacket *)pkt;
- (void)processRemoveFriendPacket:(XfirePacket *)pkt;
- (void)processFriendRequestPacket:(XfirePacket *)pkt;
- (void)processFriendGroupNamePacket:(XfirePacket *)pkt;
- (void)processFriendGroupMemberPacket:(XfirePacket *)pkt;
- (void)processFriendGroupListPacket:(XfirePacket *)pkt;
- (void)processUserOptionsPacket:(XfirePacket *)pkt;
- (void)processChatMessagePacket:(XfirePacket *)pkt;
- (void)processDisconnectPacket:(XfirePacket *)pkt;
- (void)processKeepAliveResponse:(XfirePacket *)pkt;
- (void)processFriendAvatarPacket:(XfirePacket *)pkt;
- (void)processClanListPacket:(XfirePacket *)pkt;
- (void)processClanMembersPacket:(XfirePacket *)pkt;
- (void)processClanMemberLeftClanPacket:(XfirePacket *)pkt;
- (void)processClanMemberChangedNicknamePacket:(XfirePacket *)pkt;
- (void)processClanEventsPacket:(XfirePacket *)pkt;
- (void)processClanEventDeletedPacket:(XfirePacket *)pkt;
- (void)processClanNewsPostedPacket:(XfirePacket *)pkt;

// Other internal stuff
- (void)requestInfoViewDetailsForFriend:(XfireFriend *)friend;
- (void)raiseFriendNotification:(XfireFriend *)aFriend attribute:(XfireFriendChangeAttribute)attr;

// Stuff you can only do on the log-in connection (to the Xfire master server)
- (void)setGameStatus:(NSUInteger)gameID gameIP:(NSUInteger)gip gamePort:(NSUInteger)gp;
- (void)setStatusText:(NSString *)text;
- (void)changeNickname:(NSString *)text;
- (void)beginUserSearch:(NSString *)searchString;
- (void)sendFriendInvitation:(NSString *)username message:(NSString *)msg;
- (void)sendRemoveFriend:(XfireFriend *)fr;
- (void)acceptFriendRequest:(XfireFriend *)fr;
- (void)declineFriendRequest:(XfireFriend *)fr;
- (void)addCustomFriendGroup:(NSString *)groupName;
- (void)renameCustomFriendGroup:(NSInteger)groupID newName:(NSString *)groupName;
- (void)removeCustomFriendGroup:(NSInteger)groupID;
- (void)addFriend:(XfireFriend *)fr toGroup:(XfireFriendGroup *)group;
- (void)removeFriend:(XfireFriend *)fr fromGroup:(XfireFriendGroup *)group;
- (void)setUserOptions:(NSDictionary *)options;

@end
