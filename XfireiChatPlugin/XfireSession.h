/*******************************************************************
	FILE:		XfireSession.h
	
	COPYRIGHT:
		Copyright 2007-2009, the MacFire.org team.
		Use of this software is governed by the license terms
		indicated in the License.txt file (a BSD license).
	
	DESCRIPTION:
		Represents one log-in session.  It tracks the associated
		connections, chats, friends, etc. while the user is online.
	
	HISTORY:
		2008 04 06  Changed copyright to BSD license.
		2008 02 10  Eliminated secondary reader thread.
		2008 01 12  Began integration into final phase of MacFire
		            project.  Added copyright notice.
		2007 10 13  Created.
*******************************************************************/

#import <Foundation/Foundation.h>

#import "XfireFriend.h"
#import "XfireFriendGroup.h"
#import "XfireSkin.h"
#import "XfireChat.h"

/*
	This is the public interface for the entire Xfire protocol library.
*/

@class XfireLoginConnection;
@class XfireFriendGroupController;

// login failure reasons
extern NSString *kXfireVersionTooOldReason;
extern NSString *kXfireInvalidPasswordReason;
extern NSString *kXfireNetworkErrorReason;

// Disconnect reasons
extern NSString *kXfireOtherSessionReason;
extern NSString *kXfireServerHungUpReason;
extern NSString *kXfireUnknownNetworkErrorReason;
extern NSString *kXfireServerStoppedRespondingReason;
extern NSString *kXfireServerConnectionTimedOutReason;
extern NSString *kXfireNormalDisconnectReason;
extern NSString *kXfireReadTimeOutReason;
extern NSString *kXfireWriteTimeOutReason;

// For the userOptions dictionary
// Except where noted, the value is an NSNumber containing a BOOL
extern NSString *kXfireShowMyFriendsOption;
extern NSString *kXfireShowMyGameServerDataOption;
extern NSString *kXfireShowOnMyProfileOption;
extern NSString *kXfireShowChatTimeStampsOption;
extern NSString *kXfireShowFriendsOfFriendsOption;
extern NSString *kXfireShowMyOfflineFriendsOption;
extern NSString *kXfireShowNicknamesOption; // NOT USED - FOR PRESERVING STATE ONLY
extern NSString *kXfireShowVoiceChatServerOption; // NOT USED - FOR PRESERVING STATE ONLY
extern NSString *kXfireShowWhenITypeOption;

typedef enum
{
	kXfireSessionStatusOffline = 0,
	kXfireSessionStatusOnline,
	kXfireSessionStatusLoggingOn,
	kXfireSessionStatusLoggingOff
} XfireSessionStatus;

@interface XfireSession : NSObject
{
	XfireSessionStatus			_status;
	NSString					*_xfireHost;
	unsigned short				_xfirePort;
	id							_delegate;
	NSMutableArray				*_connections;
	XfireLoginConnection		*_loginConnection;
	NSMutableArray				*_friends; // array of XfireFriend
	NSMutableArray				*_clanMembers;
	NSMutableArray				*_pendingFriends; // array of XfireFriend - these are pending FoF requests (don't have valid usernames yet)
	NSMutableArray				*_chats; // array of XfireChat
	XfireFriend					*_loginIdent; // Not all parameters are valid for the login account!
	NSTimer						*_keepAliveTimer;
	XfireFriendGroupController	*_friendGroupController;
	
	NSMutableDictionary			*_userOptions;
	
	NSUInteger				_latestClientVersion;
	NSUInteger				_posingVersion;
}

//------------------------------------------------------------------
// Creating a new session

// Return value is not autoreleased
+ (XfireSession *)newSessionWithHost:(NSString *)host port:(unsigned short)portNumber;

//------------------------------------------------------------------
// Accessors

// Get session status
- (XfireSessionStatus)status;

// Get active connections for this session
- (NSArray *)connections;

// Get/set delegate
// Set is ignored once a session has connected
- (void)setDelegate:(id)aDelegate;
- (id)delegate;

//------------------------------------------------------------------
// Client version

- (NSUInteger)compiledClientVersion; // how it was compiled
- (NSUInteger)latestClientVersion; // the latest, as identified by a login failure packet
- (void)setPosingClientVersion:(NSUInteger)posingVersion; // must call this before calling -connect
- (NSUInteger)posingClientVersion;

//------------------------------------------------------------------
// Starting and Ending a Session

// Start the session by logging in
// The delegate must be set first or this will throw an exception
- (void)connect;
- (void)disconnect;

//------------------------------------------------------------------
// Changing Client Status

- (void)setStatusString:(NSString *)text;
- (void)setNickname:(NSString *)text;
- (void)enterGame:(NSUInteger)gid;
- (void)exitGame:(NSUInteger)gid;

//------------------------------------------------------------------
// Other Commands

- (void)beginUserSearch:(NSString *)searchString;
- (void)requestInfoViewInfoForFriend:(XfireFriend *)friend;

//------------------------------------------------------------------
// Friends
// These only act on our friends (TBD and friends of friends).
// Results in arrays are in no particular order.
// Use TBD methods to search the Xfire database for other people.

// The person we're logged in as, or nil if not logged in
- (XfireFriend *)loginIdentity;

// All friends, or subset of friends
- (NSArray *)clanMembers;
- (NSArray *)friends;
- (NSArray *)friendsOnline; // gets all friends that are online

// Search for a specific friend, return nil on failure
- (XfireFriend *)friendForUserID:(NSUInteger)userID;
- (XfireFriend *)friendForUserName:(NSString *)name;
- (XfireFriend *)friendForSessionID:(NSData *)anID;

// Send a friend-add request
- (void)sendFriendInvitation:(NSString *)username message:(NSString *)msg;

// Remove a friend
- (void)sendRemoveFriend:(XfireFriend *)fr;

// Accept/decline incoming friendship requests
- (void)acceptFriendRequest:(XfireFriend *)fr;
- (void)declineFriendRequest:(XfireFriend *)fr;

//------------------------------------------------------------------
// Friend Groups

- (NSArray *)friendGroups;
- (void)requestNewFriendGroup:(NSString *)groupName;
- (void)renameFriendGroup:(XfireFriendGroup *)group newName:(NSString *)name;
- (void)removeFriendGroup:(XfireFriendGroup *)group;

//------------------------------------------------------------------
// User Options

+ (NSDictionary *)defaultUserOptions;
- (NSDictionary *)userOptions;
- (void)setUserOptions:(NSDictionary *)options;
- (BOOL)shouldShowFriendsOfFriends; // short-cut helper
- (BOOL)shouldShowOfflineFriends; // short-cut helper

//------------------------------------------------------------------
// Chatting

- (XfireChat *)beginChatWithFriend:(XfireFriend *)fr;
- (XfireChat *)chatForSessionID:(NSData *)anID;
- (void)closeChat:(XfireChat*)aChat;

@end


// For delegate method -xfireSession:friendDidChange:attribute:
typedef enum
{
	kXfireFriendNicknameDidChange = 1,			// nickname changed
	kXfireFriendWasAdded,						// a new friend was added to the list
	kXfireFriendWasRemoved,						// a friend is being removed
	kXfireFriendOnlineStatusWillChange,			// will go offline or come online
	kXfireFriendOnlineStatusDidChange,			// just went online or offline
	kXfireFriendGameInfoDidChange,				// still playing a game, but either ID, IP, or port changed
	kXfireFriendStatusStringDidChange,			// status string changed
	kXfireFriendAvatarInfoPacketDidArrive		// avatar info packet arrived
} XfireFriendChangeAttribute;

extern NSString *XfireFriendDidChangeNotification; // the object is the XfireFriend, the userInfo contains 1 object
extern NSString *kXfireFriendChangeAttribute; // value in userInfo is an NSNumber from enum XfireFriendChangeAttribute
//extern NSString *kXfireFriendChangeFriend;    // value in userInfo is an XfireFriend

// Any delegate methods not indicated REQUIRED here are OPTIONAL.
@interface NSObject (XfireDelegate)

// Return the plaintext password and usernames for the given session
// THIS IS REQUIRED!
- (void)xfireGetSession:(XfireSession *)session userName:(NSString **)aName password:(NSString **)password;

// Get the current skin/theme
// THIS IS REQUIRED!
- (XfireSkin *)xfireSessionSkin:(XfireSession *)session;

// Get the folder path for connection logs
// Specific file name in the folder is determined at run time
// Return nil if logging is not desired; default is nil
- (NSString *)xfireSessionLogPath:(XfireSession *)session;

// The session status changed
- (void)xfireSession:(XfireSession *)session didChangeStatus:(XfireSessionStatus)newStatus;

// Login failed
- (void)xfireSessionLoginFailed:(XfireSession *)session reason:(NSString *)reason;

// Session is being disconnected
- (void)xfireSessionWillDisconnect:(XfireSession *)session reason:(NSString *)reason;

// Used to notify of changes to your nickname
- (void)xfireSession:(XfireSession *)session nicknameDidChange:(NSString *)newNick;

// Friends list changes
// Used for all changes to friends list (including adding and removing)
- (void)xfireSession:(XfireSession *)session friendDidChange:(XfireFriend *)fr attribute:(XfireFriendChangeAttribute)attr;

// Friend group change
- (void)xfireSession:(XfireSession *)session friendGroupDidChange:(XfireFriendGroup *)grp;

// Friend group added/removed
- (void)xfireSession:(XfireSession *)session friendGroupWasAdded:(XfireFriendGroup *)grp;
- (void)xfireSession:(XfireSession *)session friendGroupWillBeRemoved:(XfireFriendGroup *)grp;

// User search results
// Passes an array of XfireFriend .. only username, first name, and last name are valid
- (void)xfireSession:(XfireSession *)session searchResults:(NSArray *)friends;

// Friendship requests
// Array of XfireFriend, valid username, nickname, and status (contains the personal message)
- (void)xfireSession:(XfireSession *)session didReceiveFriendshipRequests:(NSArray *)requestors;

// A new incoming chat (instant) message
// the handler is expected to configure a delegate for the chat;
// everything for this chat subsequent to this is handled by the XfireChat
- (void)xfireSession:(XfireSession *)session didBeginChat:(XfireChat *)chat;

// A chat is ending.
- (void)xfireSession:(XfireSession *)session chatDidEnd:(XfireChat *)aChat;

@end
