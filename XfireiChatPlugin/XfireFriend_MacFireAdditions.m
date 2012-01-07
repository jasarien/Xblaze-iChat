/*******************************************************************
	FILE:		XfireFriend_MacFireAdditions.m
	
	COPYRIGHT:
		Copyright 2008, the MacFire.org team.
		Use of this software is governed by the license terms
		indicated in the License.txt file (a BSD license).
	
	DESCRIPTION:
		Additions to the XfireFriend class for use by the UI.  This
		purposefully splits UI code from Xfire library code.
	
	HISTORY:
		2008 04 27  Added sorting method.
		2008 04 12  Created.
*******************************************************************/

#import "XfireFriend_MacFireAdditions.h"
#import "MFGameRegistry.h"

@implementation XfireFriend (MacFireAdditions)

- (NSImage *)displayImage
{
	NSInteger gid = [self gameID];
	
	NSImage *dispImg = [[MFGameRegistry registry] defaultImage];
	if( gid != 0 )
	{
		NSImage *tmp = [[MFGameRegistry registry] iconForGameID:gid];
		if( tmp )
			dispImg = tmp;
	}
	
	return dispImg;
}

- (NSString *)statusDisplayString
{
	if( [self isOnline] )
	{
		NSString *frStatStr = [self statusString];
		NSString *frGameStr = nil;
		NSInteger gid = [self gameID];
		if( gid != 0 )
			frGameStr = [MFGameRegistry longNameForGameID:gid];
		
		if( [frGameStr length] == 0 )
			return frStatStr;
		
		if( [frStatStr length] > 0 )
		{
			return [NSString stringWithFormat:@"%@ - %@",
				frGameStr,
				frStatStr];
		}
		
		return frGameStr;
	}
	
	return @"";
}

@end
